#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxLinkSes.h"
#include "FTSysSetting.h"
//#include "FileLog.h"
using namespace Kway;
using namespace Kway::Tw::Bx;
/////////////////////////////////////////////////////////////////////////////
static size_t LinkTimeout = 60;   // 1 mins
//---------------------------------------------------------------------------
K_ctor TbxLinkSes::TbxLinkSes(TbxFTSesMgr* aSesMgr, TMarket aMrk)
    : inherited (*aSesMgr),
    Market_(aMrk),
    LinkTimeOut_(LinkTimeout),
    RetryCnt_   (0),
    RecID_      (0)
{
//  SetLinkErrInfo();
  Loger_ = new TKOESLog;
  Loger_->SetFileName(GetSysSetting().GetSetting(ssi_LineLog));
}
//---------------------------------------------------------------------------
K_dtor TbxLinkSes::~TbxLinkSes()
{
  WriteLog("Session destroyed!", 19, GetPvcID(), false, GetBrokerID());
  if(Loger_)
    delete Loger_;
}
//---------------------------------------------------------------------------
K_mf(bool) TbxLinkSes::Send(TbxData& data)
{
  if(data.GetWorkKey()/100 == snTSE || data.GetWorkKey()/100 == snOTC)
  {
    size_t sz = BeforeAPacket(data);
    if(sz==0) sz=GetPacketSize(data);
    WriteLog(reinterpret_cast<const char*>(&data), sz, PvcID_, false, GetBrokerID());
  }
  return inherited::Send(data);
}
//---------------------------------------------------------------------------
K_mf(bool) TbxLinkSes::SettingApply(const std::string& setting)
{
  std::string setstr = setting + ",";
  //設定範例 BrokerID,PassWord,PVCID,ApCode
  //1520,1234,04,0
  std::string::size_type   pos1 = 0;
  std::string::size_type   pos2 = 0;
  std::string  broker, password, pvcid, market, apcode;
  for(int i=0;i<4;i++)
  {
    pos1 = setstr.find(',', pos2);
    if(pos1 != std::string::npos)
    {
      switch(i)
      {
        case 0: broker   = setstr.substr(pos2, pos1 - pos2);    break;
        case 1: password = setstr.substr(pos2, pos1 - pos2);    break;
        case 2: pvcid    = setstr.substr(pos2, pos1 - pos2);    break;
        case 3: apcode   = setstr.substr(pos2, pos1 - pos2);    break;
      }
      pos2 = pos1 + 1;
    }
    else
    {
      SetState(ls_Error, "Setting Error!");
      return false;
    }
  }
  Broker_.assign(broker);
  Passwd_= (TBxSize)atoi(password.c_str());
  PvcID_ .assign(pvcid);
  ApCode_= apcode.c_str()[0];
  AddLinkWorkers();

  Status_.Broker_   = Broker_;
  Status_.Passwd_   .assign(Passwd_);
  Status_.PvcID_    = PvcID_;
  Status_.ApCode_   = ApCode_;
  Status_.SetStatus(SStatusStr::Retrying);
  RecID_ = GetBxMgr()->WritePVCStatus(RecID_, &Status_);
  return true;
}
//---------------------------------------------------------------------------
K_mf(void) TbxLinkSes::SetState(int state, const std::string& reason)
{
  State_ = state;
  OnStateChanged(reason);
  if(State_ == ls_Error)
    WriteLoger(SesName(), reason);
}
//---------------------------------------------------------------------------
K_mf(void) TbxLinkSes::SetError(const std::string& subsys, const std::string& stcode)
{
  const std::string& msg = GetSesMgr()->GetMsg(subsys, stcode);
  SetState(ls_Error, msg);
}
//---------------------------------------------------------------------------
K_mf(void) TbxLinkSes::WriteLoger(const std::string& ses, const std::string& mark)
{
  if(Loger_)
  {
    std::string sesname;
    sesname = (Market_==KStk::m_Tse ? "[上市]":"[上櫃]") + ses;
    Loger_->WriteLog(Broker_.as_string(), sesname, mark);
  }
}
//---------------------------------------------------------------------------
K_mf(void) TbxLinkSes::OnMesLinkReady(TMesBase*)
{
   // reset count
   RetryCnt_ = 0;
   WriteLog("Mes Link Ready!", 16, GetPvcID(), false, GetBrokerID());
   ReLink();
}
//---------------------------------------------------------------------------
K_mf(void) TbxLinkSes::OnMesLinkFail(TMesBase*, const std::string& reason)
{
   WriteLog("Mes Link Fail!", 15, GetPvcID(), false, GetBrokerID());
   SetState(ls_Error, reason);
}
//---------------------------------------------------------------------------
K_mf(void) TbxLinkSes::OnMesLinkBroken(TMesBase*, const std::string& reason)
{
   OnSubSysBroken(TSubSysName(GetMarket()==m_Tse?"10":"91"),TStCode(00));
   WriteLog("Mes Link Broken!", 18, GetPvcID(), false, GetBrokerID());
   SetState(ls_Error, reason);
}
//---------------------------------------------------------------------------
K_mf(void) TbxLinkSes::DoTimeOut(TTimerBase* tm)
{
  if(GetSesState() != ls_WaitWakeup && GetSesState() != ls_WaitRestart){ 
     ReLink();
  }else if(GetSesState() == ls_WaitRestart){
     if(RetryCnt_++ >= 5) {
        exit(-1);
        return;
     }
     StopTimer();
     Open();
     WriteLog("ReOpened!", 14, GetPvcID(), false, GetBrokerID());
     SetState(ls_WaitWakeup, "Wait LinkReady!");
     Status_.SetStatus(SStatusStr::Retrying);
     RecID_ = GetBxMgr()->WritePVCStatus(RecID_, &Status_);
     StartTimer(60);
  }else{
     StopTimer();
     Close();
     WriteLog("Wait Restarted!", 15, GetPvcID(), false, GetBrokerID());
     SetState(ls_WaitRestart, "Wait Restart!");
     StartTimer(3);
  }
}
//---------------------------------------------------------------------------
K_mf(TWorkKey) TbxLinkSes::GetWorkKey(TFunCode code)
{
  return (Market_==KStk::m_Tse ? snTSE:snOTC)*100 + code;
}
//---------------------------------------------------------------------------
K_mf(void) TbxLinkSes::ReLink()
{
   StopTimer();
   TbxData aData;
   if(DoWorker(GetWorkKey(ckWakeup), mgL010, aData))    //送出 L010
   {
     SetState(ls_WaitWakeup, "Send L010");
     StartTimer(LinkTimeOut_);
   }
   Status_.SetStatus(SStatusStr::Retrying);
   RecID_ = GetBxMgr()->WritePVCStatus(RecID_, &Status_);
}
//---------------------------------------------------------------------------
K_mf(void) TbxLinkSes::UnLink()
{
   StopTimer();
   TbxData aData;
   if(DoWorker(GetWorkKey(ckLogout), mgL070, aData))    //送出 L070
   {
     SetState(ls_Logout, "Send L070");
     StartTimer(LinkTimeOut_);
   }
}
//---------------------------------------------------------------------------
/*
K_mf(void) TbxLinkSes::SetLinkErrInfo()
{
  LinkErrInfo_[lec_Success]     = std::string("");
  LinkErrInfo_[lec_AppNoErr]    = std::string("01 檢查並更正APPEND-NO");
  LinkErrInfo_[lec_BrkIDErr]    = std::string("02 檢查並更正BROKER-ID");
  LinkErrInfo_[lec_ApCodeErr]   = std::string("03 檢查並更正AP-CODE");
  LinkErrInfo_[lec_KeyValueErr] = std::string("04 檢查並更正KEY-VALUE");
  LinkErrInfo_[lec_SysNotReady] = std::string("05 交易所系統尚未READY,請稍候");
  LinkErrInfo_[lec_TimeErr]     = std::string("06 作業時間錯誤");
  LinkErrInfo_[lec_SubSysErr]   = std::string("81 交易子系統錯誤");
  LinkErrInfo_[lec_FunCodeErr]  = std::string("82 FUNCTION-CODE錯誤");
  LinkErrInfo_[lec_MsgTypeErr]  = std::string("83 MESSAGE-TYPE錯誤");
  LinkErrInfo_[lec_MsgTimeErr]  = std::string("84 MESSAGE-TIME錯誤");
  LinkErrInfo_[lec_StatusErr]   = std::string("85 STATUS-CODE錯誤");
  LinkErrInfo_[lec_TradeSuspend]= std::string("86 此PVC停止作業");
  LinkErrInfo_[lec_MsgTimeOut]  = std::string("91 MESSAGE TIMEOUT");
  LinkErrInfo_[lec_MsgLenErr]   = std::string("92 MESSAGE LENGTH ERROR");
  LinkErrInfo_[lec_MsgFmtErr]   = std::string("93 MESSAGE FORMAT ERROR");
  LinkErrInfo_[lec_RSMsgErr]    = std::string("94 斷線");
  LinkErrInfo_[lec_UnknowMsg]   = std::string("95 雙方訊息不同步");
  LinkErrInfo_[lec_CallTse]     = std::string("99 請與交易所聯絡");
}
//---------------------------------------------------------------------------
*/
K_mf(void) TbxLinkSes::AddLinkWorkers()
{
  L1010_.SetWorkKey(GetWorkKey(ckWakeup));
  L1020_.SetWorkKey(GetWorkKey(ckLogon));
  L1030_.SetWorkKey(GetWorkKey(ckLogout));
  Workers_.SetWorker(&L1010_);
  Workers_.SetWorker(&L1020_);
  Workers_.SetWorker(&L1030_);
}
//---------------------------------------------------------------------------
K_mf(bool) TbxLinkSes::DoWorker(const TWorkKey aKey, const TBxMsg aMsg, const TControlHead& aPkt)
{
  StopTimer();
  TWorker* worker = Workers_.GetWorker(aKey);
  if(worker != NULL)
  {
    worker->DoReq(*this, aMsg, aPkt);
    return true;
  }
  else
    SetState(ls_Error, TWorker::WorkerErrMsg);
  return false;
}
//---------------------------------------------------------------------------


