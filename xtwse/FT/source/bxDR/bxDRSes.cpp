#ifdef __BORLANDC__
  
#pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxDRSes.h"
#include "TwStkOrdCenter.h"
#include "FTSysSetting.h"
//#include "FileLog.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx;
using namespace Kway::Tw::Bx::DR;
static size_t RptTimeout = 60;   // 60 seconds
static std::string  LastMsg;
/////////////////////////////////////////////////////////////////////////////
/*
   說明:成交回報模組為XTRADE及檔案傳輸系統共用的模組,但如果和檔案傳輸同時使用時,便會有成交回報補漏的問題
        和檔案傳輸同時使用時,必須先定義 __bxFT 這個condition
        這裏以PMap 記錄每一筆成交序號,做為補漏依據
        補漏要求每次最多200筆,若5分鐘內未完成補漏則重新檢查PMap有無漏回報,有則啟動補漏
*/
K_ctor  TbxDRSes::TbxDRSes(TbxFTSesMgr* aSesMgr, KStk::TMarket aMrk)
   : inherited  (aSesMgr, aMrk),
     SesMgr_    (*aSesMgr),
     RptTimeOut_(RptTimeout),
     RecoverTimer_(this),
     Recovering_(false),
     LastSeqNo_ (0),    // RPT送出0時,表示希望由TSE所儲存之序號開始
     Finish_(false),
     IsNewLine_(false)
{
}
//---------------------------------------------------------------------------
K_dtor TbxDRSes::~TbxDRSes  ()
{
   if(!GetMes())
      return;
   TAlarmMsg  Alarm(GetPvcID().as_string());
   Alarm.SetMsg(SMsg::ACode_LineClosed, GetMes()->GetSetting().c_str(), "Line Closed");
   SesMgr_.WriteAlarm(&Alarm);
   Pmach_.Close();
   RecoverTimer_.Stop();
}
//---------------------------------------------------------------------------
K_mf(void) TbxDRSes::OnMesLinkReady(TMesBase* m)
{
   inherited::OnMesLinkReady(m);
   RecoverFName_ = std::string();
   //20111208 eric: 加入DR回補連線記錄, 待回補完成(R6)時解除註冊
   SesMgr_.RegDRSes((TbxSesBase*)this);
}
//---------------------------------------------------------------------------
K_mf(void)  TbxDRSes::OnMesLinkFail  (Kway::TMesBase* m, const std::string& reason)
{
   inherited::OnMesLinkFail(m, reason);
   if(!GetMes())
      return;
   TAlarmMsg  Alarm(GetPvcID().as_string());
   Alarm.SetMsg(SMsg::ACode_LineClosed, GetMes()->GetSetting().c_str(), "Line Closed");
   SesMgr_.WriteAlarm(&Alarm);
}
//---------------------------------------------------------------------------
K_mf(void)  TbxDRSes::OnMesLinkBroken  (Kway::TMesBase* m, const std::string& reason)
{
   inherited::OnMesLinkBroken(m, reason);
   if(!GetMes())
      return;
   TAlarmMsg  Alarm(GetPvcID().as_string());
   Alarm.SetMsg(SMsg::ACode_LineClosed, GetMes()->GetSetting().c_str(), "Line Closed");
   SesMgr_.WriteAlarm(&Alarm);
}
//---------------------------------------------------------------------------
K_mf(bool) TbxDRSes::SettingApply(const std::string& setting)
{
  //設定範例 BrokerID,PassWord,PVCID,ApCode,NewLine
  // BrokerID: 券商代號
  // PassWord: 密碼
  // PVCID   : pvc id
  // ApCode  : 業務系統代碼 A:擴充版成交回報, 3:一般成交回報
  // NewLine : 成交回報寫檔時, 是否使用換行字元(len: 63), 若不使用換行字元(len: 62)
  //1520,1234,04,A,Y
  RecoverFName_ = std::string();
  bool IsERpt = false;
  std::string setstr = setting;
  std::string::size_type   pos1 = 0;
  std::string::size_type   pos2 = 0;
  for(int i=0;i<5;i++){
     pos1 = setting.find(',', pos2);
     if(pos1 != std::string::npos){
       pos2 = pos1 + 1;
       if(i == 2){
          setstr = setting.substr(0, pos1);
          if(setting[pos2] == 'A')
             IsERpt = true;
       }
       if(i == 3 && (setting[pos2] == 'Y' ||  setting[pos2] == 'y'))
          IsNewLine_ = true;
     }else
        break;
  }
  if(inherited::SettingApply(setstr + "," + (IsERpt?(char)akERpt:(char)akRpt)) == false){
    return false;
  }
  AddWorkers();
  return true;
}
//---------------------------------------------------------------------------
K_mf(void) TbxDRSes::UpdateState()
{
  SetState(TRptState(GetSesState()), LastMsg);
}
//---------------------------------------------------------------------------
K_mf(void) TbxDRSes::SetState(TRptState state, const std::string& msg)
{
  LastMsg = msg;
  static char buf[256];
  switch(state)
  {
    case rpt_Data:
    case rpt_LinkChk:
    case rpt_RptReply:
    case rpt_ReData:
    case rpt_RptEnd:
      sprintf(buf, "DR Ready (%d/%d)%s:%s", SeqSet_.size(), LastSeqNo_,(Finish_?"[DONE]":""), msg.c_str());
      inherited::SetState(state, buf);
      break;

    case rpt_Logout:
      inherited::SetState(state, "DR Logout:" + msg);
      break;

    default:
      inherited::SetState(state, msg);
      break;
  }
}
//---------------------------------------------------------------------------
K_mf(TWorkKey) TbxDRSes::GetWorkKey(TFunCode code)
{
  return ((GetMarket()==KStk::m_Tse ? snRPTTse:snRPTOtc)*100 + code);
}
//---------------------------------------------------------------------------
K_mf(void) TbxDRSes::OnSubSysReady(TApKey aKey)
{
  if(!GetMes())
     return;
  TAlarmMsg  Alarm(GetPvcID().as_string());
  Alarm.SetMsg(SMsg::ACode_APPReady, GetMes()->GetSetting().c_str(), "APP Sys Ready");
  SesMgr_.WriteAlarm(&Alarm);
  StartRptSys();
}
//---------------------------------------------------------------------------
K_mf(void) TbxDRSes::OnSubSysBroken(TSubSysName subsys, TStCode code)
{
}
//---------------------------------------------------------------------------
K_mf(void) TbxDRSes::StartRptSys()
{
  TbxData aData;
  if(DoWorker(GetWorkKey(ckRPTLink), mgR1, aData))    //送出 R1
  {
//    SetState(rpt_Starting, GetMarket().as_string(true) + " Ready!");
    SetState(rpt_LinkChk,"委託回補-Waiting!");
//    SetState(rpt_Starting, "成交回報連線開始...");
    StartRptTimer();
  }
}
//---------------------------------------------------------------------------
K_mf(void) TbxDRSes::AddWorkers()
{
  R5000_.SetWorkKey(GetWorkKey(ckRPTLink));
  R5010_.SetWorkKey(GetWorkKey(ckRPTData));
  R5020_.SetWorkKey(GetWorkKey(ckRPTEnd));
  Workers_.SetWorker(&R5000_);
  Workers_.SetWorker(&R5010_);
  Workers_.SetWorker(&R5020_);
}
//---------------------------------------------------------------------------
K_mf(void) TbxDRSes::DoTimeOut(Kway::TTimerBase* tm)
{
  if(GetSesState() < rpt_Starting){
     inherited::DoTimeOut(tm);
     return;
  }
  TbxData aData;
  StopRptTimer();
  switch(GetSesState())
  {
    case rpt_Starting:
    case rpt_ReData:
      StartRptSys();
      break;
    case rpt_Logout:
      break;
    default:
      if(DoWorker(GetWorkKey(ckRPTLink), mgR4, aData))    //送出 R4
      {
        SetState(rpt_LinkChk, "R4");
      }
      break;
  }
  StartRptTimer();
}
//---------------------------------------------------------------------------
K_mf(bool) TbxDRSes::SendPkt(const TControlHead& pkt)
{
  const TbxData& data = static_cast<const TbxData&>(pkt);
  size_t sz = R5000_.BeforeAPacket(*this, data);
  if(sz==0) sz=R5000_.GetPacketSize(*this, data);
  WriteLog(reinterpret_cast<const char*>(&data), sz, GetPvcID(), false, GetBrokerID());
  return inherited::Send(const_cast<TbxData&>(data));
}
//---------------------------------------------------------------------------
K_mf(bool) TbxDRSes::WriteDR(const TDRBody& aData)
{
   SesMgr_.WriteDRData(GetMarket(), aData);
   return true;
}
//---------------------------------------------------------------------------
K_mf(void) TbxDRSes::DRFinish()
{
   SesMgr_.DelDRSes((TbxSesBase*)this);
}
//---------------------------------------------------------------------------
