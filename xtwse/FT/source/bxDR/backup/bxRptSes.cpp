#ifdef __BORLANDC__  
#pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxRptSes.h"
#include "TwStkOrdCenter.h"
#include "FTSysSetting.h"
#include "FileLog.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx;
using namespace Kway::Tw::Bx::Rpt;
static size_t RptTimeout = 60;   // 60 seconds
static std::string  LastMsg;
/////////////////////////////////////////////////////////////////////////////
/*
   ����:����^���Ҳլ�XTRADE���ɮ׶ǿ�t�Φ@�Ϊ��Ҳ�,���p�G�M�ɮ׶ǿ�P�ɨϥή�,�K�|������^���ɺ|�����D
        �M�ɮ׶ǿ�P�ɨϥή�,�������w�q __bxFT �o��condition
        �o�إHPMap �O���C�@������Ǹ�,�����ɺ|�̾�
        �ɺ|�n�D�C���̦h200��,�Y5�������������ɺ|�h���s�ˬdPMap���L�|�^��,���h�Ұʸɺ|
*/
K_ctor  TbxRptSes::TbxRptSes(TbxFTSesMgr* aSesMgr, KStk::TMarket aMrk)
   : inherited  (aSesMgr, aMrk),
     SesMgr_    (*aSesMgr),
     RptTimeOut_(RptTimeout),
     LastSeqNo_ (0)    // RPT�e�X0��,��ܧƱ��TSE���x�s���Ǹ��}�l
{
#ifndef __BORLANDC__
  appLog.log("Create bxRptSes");
#endif
  TFTSysSetting& setting = const_cast<TFTSysSetting&>(GetSysSetting());
  std::string fname = setting.GetSetting(GetMarket() == KStk::m_Tse ? ssi_TsePmach:ssi_OtcPmach);
  Pmach_.Open(fname.c_str(), TFileMode(Kway::fmRandomAccess | Kway::fmOpenAlways | Kway::fmCreatePath));

  TRptBody rpt;
  char newline[2];
  long seqno;
  while(!Pmach_.IsEOF())
  {
    if(Pmach_.Read(&rpt, sizeof(rpt)) == sizeof(rpt))
    {
      seqno = rpt.SeqNo_.as_int();
      if(seqno == LastSeqNo_+1)   // ���s�򪺳̫�@��
        LastSeqNo_ = seqno;       
      SeqSet_.insert(seqno);
    }
    Pmach_.Read(newline, 1);
  }
}
//---------------------------------------------------------------------------
K_dtor TbxRptSes::~TbxRptSes  ()
{
   TAlarmMsg  Alarm(GetPvcID().as_string());
   if(!GetMes())
      return;
   Alarm.SetMsg(ACode_LineClosed,GetMes()->GetSetting().c_str(),"Line Closed");
   SesMgr_.WriteAlarm(&Alarm);
}
//---------------------------------------------------------------------------
K_mf(void)  TbxRptSes::OnMesLinkFail  (Kway::TMesBase* m, const std::string& reason)
{
   inherited::OnMesLinkFail(m, reason);
   TAlarmMsg  Alarm(GetPvcID().as_string());
   if(!GetMes())
      return;
   Alarm.SetMsg(ACode_LineClosed,GetMes()->GetSetting().c_str(),"Line Closed");
   SesMgr_.WriteAlarm(&Alarm);
}
//---------------------------------------------------------------------------
K_mf(void)  TbxRptSes::OnMesLinkBroken  (Kway::TMesBase* m, const std::string& reason)
{
   inherited::OnMesLinkBroken(m, reason);
   TAlarmMsg  Alarm(GetPvcID().as_string());
   if(!GetMes())
      return;
   Alarm.SetMsg(ACode_LineClosed,GetMes()->GetSetting().c_str(),"Line Closed");
   SesMgr_.WriteAlarm(&Alarm);
}
//---------------------------------------------------------------------------^M
K_mf(bool) TbxRptSes::SettingApply(const std::string& setting)
{
  //�]�w�d�� BrokerID,PassWord,PVCID
  //1520,1234,04
  std::string setstr = setting;
  if(inherited::SettingApply(setstr + "," + (char)akRpt) == false)
    return false;
  AddWorkers();
  return true;
}
//---------------------------------------------------------------------------
K_mf(void) TbxRptSes::UpdateState()
{
  SetState(TRptState(GetSesState()), LastMsg);
}
//---------------------------------------------------------------------------
K_mf(void) TbxRptSes::SetState(TRptState state, const std::string& msg)
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
      sprintf(buf, "RPT Ready (%d/%d):%s", SeqSet_.size(), LastSeqNo_, msg.c_str());
      inherited::SetState(state, buf);
      break;

    case rpt_Logout:
      inherited::SetState(state, "RPT Logout:" + msg);
      break;

    default:
      inherited::SetState(state, msg);
      break;
  }
}
//---------------------------------------------------------------------------
K_mf(TWorkKey) TbxRptSes::GetWorkKey(TFunCode code)
{
  return ((GetMarket()==KStk::m_Tse ? snRPTTse:snRPTOtc)*100 + code);
}
//---------------------------------------------------------------------------
K_mf(void) TbxRptSes::OnSubSysReady(TApKey aKey)
{
#ifndef __BORLANDC__
  appLog.log("bxRptSes OnSubSysReady");
#endif
  SesMgr_.RegRptSes(this);
  TAlarmMsg  Alarm(GetPvcID().as_string());
  if(!GetMes())
     return;
  Alarm.SetMsg(ACode_APPReady,GetMes()->GetSetting().c_str(),"APP Sys Ready");
  SesMgr_.WriteAlarm(&Alarm);
  StartRptSys();
  CheckSetCont();   // �ˬd�����ɪ������
}
//---------------------------------------------------------------------------
K_mf(void) TbxRptSes::OnSubSysBroken(TStCode code)
{
#ifndef __BORLANDC__
  appLog.log("bxRptSes OnSubSysBroken");
#endif
  SesMgr_.DelRptSes(this);
}
//---------------------------------------------------------------------------
K_mf(void) TbxRptSes::StartRptSys()
{
  TbxData aData;
  if(DoWorker(GetWorkKey(ckRPTLink), mgR1, aData))    //�e�X R1
  {
    SetState(rpt_Starting, GetMarket().as_string(true) + " Ready!");
//    SetState(rpt_Starting, "����^���s�u�}�l...");
    StartRptTimer();
  }
}
//---------------------------------------------------------------------------
K_mf(void) TbxRptSes::AddWorkers()
{
  R5000_.SetWorkKey(GetWorkKey(ckRPTLink));
  R5010_.SetWorkKey(GetWorkKey(ckRPTData));
  R5020_.SetWorkKey(GetWorkKey(ckRPTEnd));
  Workers_.SetWorker(&R5000_);
  Workers_.SetWorker(&R5010_);
  Workers_.SetWorker(&R5020_);
}
//---------------------------------------------------------------------------
K_mf(void) TbxRptSes::DoTimeOut(Kway::TTimerBase* tm)
{
  if(GetSesState() < rpt_Starting)
    return;
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
      if(DoWorker(GetWorkKey(ckRPTLink), mgR4, aData))    //�e�X R4
      {
        SetState(rpt_LinkChk, "R4");
        StartRptTimer();
      }
      break;
  }
  StartRptTimer();
}
//---------------------------------------------------------------------------
K_mf(TSeqNo) TbxRptSes::GetNextSeqNo()
{
  char buf[8];
  sprintf(buf, "%0*d", sizeof(TSeqNo), LastSeqNo_+1);
  UpdateState();
  return TSeqNo(buf);
}
//---------------------------------------------------------------------------
K_mf(bool) TbxRptSes::SendPkt(const TControlHead& pkt)
{
  const TbxData& data = static_cast<const TbxData&>(pkt);
  size_t sz = R5000_.BeforeAPacket(*this, data);
  if(sz==0) sz=R5000_.GetPacketSize(*this, data);
  WriteLog(reinterpret_cast<const char*>(&data), sz, GetPvcID(), false);
  return inherited::Send(const_cast<TbxData&>(data));
}
//---------------------------------------------------------------------------
K_mf(bool) TbxRptSes::WritePmach(const TRptBody& aData)
{
  // �C��b�w������o�e����, �|���@��������,
  // ��STK-NO, BUY-SELL, ODR-NO, BROKER-ID �����ť�
  // MTHQTY, MTHPR, IVACNO, REC-NO �����s
  // SEQ-NO �������`���浧��
  int seqno = aData.SeqNo_.as_int();
  if(aData.RecNo_.as_int() == 0)   // �`�ɽs����0 �N����^������
  {
    if(Pmach_.Write(&aData, sizeof(aData)) == sizeof(aData))
    {
      static char newline[] = "\n";
      Pmach_.Write(newline, sizeof(newline)-1);
    }
    SetState(rpt_RptEnd, "�^���@�~����");
    Pmach_.Close();
  }
  else
  {
    if(SeqSet_.find(seqno) == SeqSet_.end())  // not exist
    {
      if(Pmach_.Write(&aData, sizeof(aData)) == sizeof(aData))
      {
        static char newline[] = "\n";
        Pmach_.Write(newline, sizeof(newline)-1);
        SeqSet_.insert(seqno);
      }
    }
    if(seqno == LastSeqNo_+1)   // ���򪺤U�@��
      LastSeqNo_ = seqno;
    else
      return false;
  }
  UpdateState();
  return true;
}
//---------------------------------------------------------------------------
K_mf(void) TbxRptSes::CheckRptCount(long recs)
{
  if(SeqSet_.size() != (size_t)recs)    // ���Ƥ���
    CheckSetCont();
}
//---------------------------------------------------------------------------
K_mf(bool)  TbxRptSes::CallRecover(long from, long end)
{
  long recs = 0;
  while(from < end)
  {
    recs = end - from +1;
    if(recs > 199)
      recs = 199;

    for(long i=0; i<recs; i++)
      if(ReSeqSet_.find(from+i) != ReSeqSet_.end())
        recs = i;
    SesMgr_.RequestRpt(*this, GetBrokerID(), GetMarket(), from, from+recs);
    // �w�n�D���s�J�w���Ըɺ|�O��
    for(long i=0; i<recs; i++)
      ReSeqSet_.insert(from+i);
    from += recs;
  }
  return true;
}
//---------------------------------------------------------------------------
K_mf(void) TbxRptSes::CheckSetCont()
{
  long lastSeq = 0;
  for(TSeqSetI i=SeqSet_.begin(); i!=SeqSet_.end(); ++i)  {
    if(*i > lastSeq+1)
    {
      CallRecover(lastSeq + 1, *i - 1);
//      break;
    }
    lastSeq = *i;
  }
}
//---------------------------------------------------------------------------
/*K_mf(void)    TbxRptSes::RptRecover(TRptBody body, bool isEOF)
{
//   if(OrdCenterPtr_ != 0)
   {
      int qty = body.MthQty_ .as_int();
      KStk::TOrdCenter::TRpt::TDeal rpt;
      rpt.TseOrdKey_.BrokerID_.assign(body.BrkID_.as_string());
      rpt.TseOrdKey_.Market_  = GetMarket();
      rpt.TseOrdKey_.TseOrdID_.assign(body.OrdNo_.as_string());
      rpt.IvacNo_ .assign(body.IvacNo_ .as_string());
      rpt.BSCode_ .assign(body.BSCode_ .as_string());
      rpt.StkNo_  .assign(body.StkNo_  .as_string());
      rpt.Pri_    .assign(((double)body.MthPri_ .as_int())/100);
      rpt.OType_  .assign(body.OrdType_.as_string());
      rpt.TseTime_.assign(body.MthTime_.as_string());
      rpt.Qty_    = (body.ExcCode_.as_int() != 2 ? qty*1000:qty);
      rpt.BSeqNo_ .assign(body.SeqNo_ .as_string());
      rpt.MSeqNo_ .assign(body.RecNo_ .as_string());
//      OrdCenterPtr_->RptDeal(rpt);
   }
   SeqSet_.insert(body.SeqNo_.as_int());
   UpdateState();
//   if(isEOF)
//     CheckMapCont();
}*/
//---------------------------------------------------------------------------
K_mf(void)    TbxRptSes::RptRecover(const TRptBody& body, bool isEOF)
{
   Pmach_.Write(&body, sizeof(body));
   SeqSet_.insert(body.SeqNo_.as_int());
   UpdateState();
}
//---------------------------------------------------------------------------
K_mf(void)    TbxRptSes::ImpRptFile(std::string& fname)
{
  TFile file;
  file.Open(fname.c_str(), TFileMode(Kway::fmRandomAccess | Kway::fmOpenAlways | Kway::fmCreatePath));

  int recs;
  TRptBody rpt;
  if(file.IsOpened())
  {
    recs = (int)Pmach_.GetFileSize() / sizeof(TRptBody);
    for(int i=0;i<recs && file.IsEOF();i++)
      if(file.Read(&rpt, sizeof(rpt)) == sizeof(rpt))
        RptRecover(rpt, false);
  }
}
//---------------------------------------------------------------------------


