//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxFTSes.h"
#include "MesX25.h"
#include "FTSysSetting.h"
//#include "FileLog.h"
#include "ExtDef.h"
#include "ExcMgrBase.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx;
using namespace Kway::Tw::Bx::FT;
//---------------------------------------------------------------------------
static size_t   ReplyTimeOut    = 90;    // 檔案傳輸以 90 秒為TIMEOUT
static size_t   HBSendTimeOut   = 50;    // 確定連線的TIMEOUT以 60 秒
static size_t   HBWaitTimeOut   = 125;    // 確定連線等候回應的TIMEOUT以 90 秒, 20060731 eric for test 125 秒
//---------------------------------------------------------------------------
K_ctor TbxFTSes::TbxFTSes(TbxFTSesMgr* aSesMgr, KStk::TMarket aMrk, bool send)
    : inherited (aSesMgr, aMrk),
      SesMgr_   (*aSesMgr),
      ReplyTimeOut_(ReplyTimeOut),
      HBSendTimeOut_(HBSendTimeOut),
      HBWaitTimeOut_(HBWaitTimeOut),
      FTDirect_ (send ? eftdSend:eftdRecv),
      FTTimer_  (this),
      Transaction_(NULL),
      IsX25_(false)
{
//  SetFTErrorInfo();
   RetryCount_ = 0;
}
//---------------------------------------------------------------------------
K_dtor TbxFTSes::~TbxFTSes()
{
  SesMgr_.DelFTSes(GetBrokerID(), this);

   if(!GetMes())
      return;
   TAlarmMsg  Alarm(GetPvcID().as_string());
   Alarm.SetMsg(SMsg::ACode_LineClosed, GetMes()->GetSetting().c_str(), "Line Closed");
   SesMgr_.WriteAlarm(&Alarm);
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSes::OnMesLinkReady(Kway::TMesBase* m)
{
   inherited::OnMesLinkReady(m);
   RetryCount_ = 0;
   ID_ = GetBrokerID().as_string()+"-"+GetPvcID().as_string();
   if(GetExcMgr())
      GetExcMgr()->Alert("XF2000",ID_);
}
//---------------------------------------------------------------------------
K_mf(void)  TbxFTSes::OnMesLinkFail  (Kway::TMesBase* m, const std::string& reason)
{
   inherited::OnMesLinkFail(m, reason);
   RetryCount_++;
   if(GetExcMgr()){
      if(RetryCount_ == SesMgr_.GetFailCount())
         GetExcMgr()->Alert("XF2011",ID_);
      else
         GetExcMgr()->Alert("XF2010",ID_);
   }
   if(!GetMes())
      return;
   TAlarmMsg  Alarm(GetPvcID().as_string());
   Alarm.SetMsg(SMsg::ACode_LineClosed, GetMes()->GetSetting().c_str(), "Line Closed");
   SesMgr_.WriteAlarm(&Alarm);
}
//---------------------------------------------------------------------------
K_mf(void)  TbxFTSes::OnMesLinkBroken  (Kway::TMesBase* m, const std::string& reason)
{
   inherited::OnMesLinkBroken(m, reason);
   if(GetExcMgr())
      GetExcMgr()->Alert("XF2012",ID_);
   if(!GetMes())
      return;
   TAlarmMsg  Alarm(GetPvcID().as_string());
   Alarm.SetMsg(SMsg::ACode_LineClosed, GetMes()->GetSetting().c_str(), "Line Closed");
   SesMgr_.WriteAlarm(&Alarm);
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSes::SetState(eFTState state, const std::string& msg)
{
  static char buf[256];
  switch(state)
  {
    case efsError:
    case efsStart:
    case efsFT:
    case efsMsg:
    case efsStop:
    case efsHeartbeat:
    case efsWaitNextJob:
      sprintf(buf, "FT Ready:%s", msg.c_str());
      inherited::SetState(state, buf);
      break;

    default:
      inherited::SetState(state, msg);
      break;
  }
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSes::StartHB()
{
  if(IsExtend() && GetSesState() >= efsError)
  {
    if(IsSend())
      StartSendHBTimer();
    else
      StartWaitHBTimer();
  }
  else
    StopFTTimer();
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSes::StopHB()
{
  if(IsExtend())
    StopFTTimer();
}
//---------------------------------------------------------------------------
K_mf(TWorkKey) TbxFTSes::GetWorkKey(TFunCode code)
{
  return ((GetMarket()==KStk::m_Tse ? snFTTse:snFTOtc)*100 + code);
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSes::AddLinkWorkers()
{
  F2000_.SetWorkKey(GetWorkKey(ckFTStart));
  F2001_.SetWorkKey(GetWorkKey(ckFTCont));
  F2002_.SetWorkKey(GetWorkKey(ckFTMsg));
  F2003_.SetWorkKey(GetWorkKey(ckFTEnd));
  F2004_.SetWorkKey(GetWorkKey(ckFTLink));
  Workers_.SetWorker(&F2000_);
  Workers_.SetWorker(&F2001_);
  Workers_.SetWorker(&F2002_);
  Workers_.SetWorker(&F2003_);
  Workers_.SetWorker(&F2004_);
}
//---------------------------------------------------------------------------
/*
K_mf(void) TbxFTSes::SetFTErrorInfo ()
{
  FTErrInfo_[fec_Success]     = std::string("");
  FTErrInfo_[fec_ErrFileCode] = std::string("檔案代號錯誤");
  FTErrInfo_[fec_ErrEOF]      = std::string("EOF欄位值錯誤");
  FTErrInfo_[fec_ErrFileSize] = std::string("收到的總資料訊息長度不符合");
  FTErrInfo_[fec_TimingError] = std::string("送出的要求在規定作業時間外");
  FTErrInfo_[fec_FileNotReady]= std::string("所要求的檔案尚未完成");
  FTErrInfo_[fec_FileEmpty]   = std::string("所要求的檔案為空檔");
  FTErrInfo_[fec_Abort]       = std::string("主動端主動中止作業");
  FTErrInfo_[fec_DupRequest]  = std::string("上次要求尚未處理完畢");
  FTErrInfo_[fec_ErrSubName]  = std::string("SubSystemName 錯誤");
  FTErrInfo_[fec_ErrFunCode]  = std::string("FunctionCode 錯誤");
  FTErrInfo_[fec_ErrMsgType]  = std::string("MessageType 錯誤");
  FTErrInfo_[fec_ErrMsgTime]  = std::string("MessageTime 錯誤");
  FTErrInfo_[fec_ErrStatCode] = std::string("StatusCode 錯誤");
  FTErrInfo_[fec_ErrSourceID] = std::string("SourceID 錯誤");
  FTErrInfo_[fec_ErrObjectID] = std::string("ObjectID 錯誤");
  FTErrInfo_[fec_ErrBodyLen]  = std::string("BodyLength 錯誤");
  FTErrInfo_[fec_InternalErr] = std::string("Internal 錯誤");
  FTErrInfo_[fec_FTCountErr]  = std::string("累計錯誤次數超過限制");
  FTErrInfo_[fec_ReqCountErr] = std::string("傳輸檔案次數超過限制");
  FTErrInfo_[fec_CallTSE]     = std::string("請與交易所連絡");
}
*/
//---------------------------------------------------------------------------
K_mf(bool) TbxFTSes::SendPkt(const TFTHead& pkt)
{
  if(IsSend())  // only sender need send HB
    StopHB();
  const TControlHead& head = static_cast<const TControlHead&>(pkt);
  const TbxData& data      = static_cast<const TbxData&>     (head);
  size_t sz;
  if((!IsCompleteLog()) && (pkt.GetMsg()==mgF030 || pkt.GetMsg()==mgF210))
    sz = F2000_.GetPacketSize(*this, data) + sizeof(TFileCode) + sizeof(TEof);
  else
    sz = F2000_.BeforeAPacket(*this, data);
  WriteLog(reinterpret_cast<const char*>(&data), sz, GetPvcID(), false, GetBrokerID());

  bool ret = inherited::Send(const_cast<TbxData&>(data));
  StartWaitReplyTimer();
  if(IsSend())  // only sender need send HB
    StartHB();
  return ret;
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSes::SetJob(TFTReqBase* aTrans)
{
  if(Transaction_ != NULL)  // not finish?? why?
    delete Transaction_;
  Transaction_ = aTrans;
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSes::JobFinish(TStCode code)
{
  if(Transaction_ != NULL)
  {
    if(IsSend())
      UpdateFTStatus(fft_W_ing, code);
    else
    {
      Transaction_->OnFinish();
      UpdateFTStatus(fft_Finish, code);
    }
    delete Transaction_;
    Transaction_ = NULL;
  }
//  if(!SesMgr_.FetchNextJob(GetBrokerID()))
//  StartTimer(60); 
//  StartTimer(5); // 五秒後再取下一筆
  StartTimer(atoi(GetSysSetting().GetSetting(ssi_FTInterval).c_str())); // n秒後再取下一筆
}
//---------------------------------------------------------------------------
K_mf(TFTFileReq*) TbxFTSes::AllocNewFT(const TF010& f010)
{
   TSysID  id;
   const TFTJob* job;
   TFTFileReq* req;
   TFTCenter& ft = const_cast<TFTCenter&>(GetCenter());
   if(const_cast<TFTSysSetting&>(GetSysSetting()).IsAckless(f010.FileCode_))
      id = ft.FindFirstWaiting(f010.DestID_, GetMarket(), f010.FileCode_);
   else   
      id = SesMgr_.GetRpyID(TRptKey(f010.DestID_, GetMarket()), f010.FileCode_);
   if(id != TSysID(0) && ft.GetHnd(f010.DestID_, GetMarket(), id, &job)){
      WorkSysID_ = id;
      if(job->DstData_.empty()){
         std::string filename = GetMarket().as_string() + f010.FileCode_.as_string() + "." + f010.DestID_.as_string();
//         filename            += (GetHostID()==THostID(0)?"":GetHostID().as_string());
         ft.UpdateDstData(f010.DestID_, GetMarket(), id, filename);
      }
      req = new TFTFileReq(*job);
      if(req->GetFileCode().as_string() == "R30" ||
         (f010.FileCode_.as_string() == "L17" || f010.FileCode_.as_string() == "L18"))
         req->SetFileCode(f010.FileCode_);
   }else{
      TFTJob work;
      work.SeqNo_       = ft.FetchSeqNo(f010.DestID_, GetMarket());
      work.SourceID_    = f010.DestID_;
      work.DestID_      = f010.SourceID_;
      work.Market_      = GetMarket();
      work.FileCode_    = f010.FileCode_;
      work.Type_        = ftt_File;
      work.Mode_        = ftm_Daily;
      std::string filename = GetMarket().as_string() + f010.FileCode_.as_string() + "." + f010.DestID_.as_string();
//      filename         += (GetHostID()==THostID(0)?"":GetHostID().as_string());
      work.SrcData_     .clear();
      work.DstData_     .assign(filename.c_str());
      work.RecLen_      = f010.FileSize_.as_int();
      work.CreateTime_  = TKTime::Now();
      WorkSysID_        = ft.DoReq(work);
      req = new TFTFileReq(work);
   }
   req->SetSysOrdID(WorkSysID_);
   req->InitDownLoad(FileBlockSize_, f010.FileSize_.as_int());
   SetJob(req);
   return req;
}
//---------------------------------------------------------------------------
K_mf(TFTFileReq*) TbxFTSes::AllocNewFT(const TSysID& id, const TFTJob& job)
{
  WorkSysID_ = id;
  TFTFileReq* req   = new TFTFileReq(job);
  req->SetSysOrdID(id);
  if(req->InitUpLoad(FileBlockSize_, job.RecLen_)==false)
    return NULL;
  SetJob(req);
  return req;
}
//---------------------------------------------------------------------------
K_mf(TFTMsgReq*) TbxFTSes::AllocNewMsg(const TF050& f050)
{
  TFTCenter& ft = const_cast<TFTCenter&>(GetCenter());
  if(f050.FileCode_ == TFileCode("S10"))   // 通告
  {
    const TFTJob* work;
    TFileCode fcode;
    fcode.assign(f050.Msg_.as_string().substr(0, 3));
    WorkSysID_ = SesMgr_.GetRpyID(TRptKey(f050.DestID_, GetMarket()), fcode);
    if(WorkSysID_ != TSysID(0) && ft.GetHnd(f050.DestID_, GetMarket(), WorkSysID_, &work))
      UpdateFTStatus(fft_Abort, f050.GetCode(), f050.Msg_.as_string().c_str());

    WorkSysID_ = ft.DoS10Msg(GetMarket(), f050);
    if(ft.GetHnd(f050.DestID_, GetMarket(), WorkSysID_, &work))
    {
      SesMgr_.ProcessS10(*work);
      TFTMsgReq* req    = new TFTMsgReq(*work);
      req->SetSysOrdID(WorkSysID_);
      req->SetRpyMsg(f050.Msg_);
      SetJob(req);
      return req;
    }
  }
  else
  {
    TFTNewJob work;
    if(f050.FileCode_ == TFileCode("R50")){
       TSysID  id  = SesMgr_.GetRpyID(TRptKey(f050.DestID_, GetMarket()), f050.FileCode_);
       WorkSysID_        = id;
    }else
       work.SeqNo_       = ft.FetchSeqNo(f050.DestID_, GetMarket());
    work.SourceID_    = f050.DestID_;
    work.DestID_      = f050.SourceID_;
    work.Market_      = GetMarket();
    work.FileCode_    = f050.FileCode_;
    work.Type_        = ftt_Msg;
    work.Mode_        = ftm_TSEMsg;
    work.SrcData_     = f050.Msg_;
    work.DstData_     = f050.Msg_;
    work.RecLen_      = 0;
    work.CreateTime_  = TKTime::Now();
   
    if(f050.FileCode_ != TFileCode("R50"))
       WorkSysID_      = ft.DoReq(work);
    work.SourceID_    = f050.SourceID_;
    work.DestID_      = f050.DestID_;
    TFTMsgReq* req    = new TFTMsgReq(work);
    req->SetSysOrdID(WorkSysID_);
    req->SetReqMsg(f050.Msg_);
    SetJob(req);
    return req;  
  }
  return NULL;
}
//---------------------------------------------------------------------------
K_mf(TFTMsgReq*) TbxFTSes::AllocNewMsg(const TSysID& id, const TFTJob& job)
{
  WorkSysID_        = id;
  TFTMsgReq* req    = new TFTMsgReq(job);
  req->SetSysOrdID(WorkSysID_);
  req->SetReqMsg(job.SrcData_);
  SetJob(req);
  return req;
}
//---------------------------------------------------------------------------
K_mf(bool) TbxFTSes::CheckStatusCode(TSubSysName subsys, const TStCode aCode)
{
  // status code('8'或'9'開頭者)，表示雙方已在不同步狀態，必須重新建立連線關係.
  if(aCode >= fec_ErrSubName && aCode <= fec_CallTSE)
    ReLink();
  else if(aCode == fec_Success)
    return true;
  // status code不為'8'開頭或'9'或'00'之發生情形，
  // 此時傳輸子系統結束該訊息所對應之作業，然後回到傳輸子系統之開頭，處裡下一個作業
//  const std::string& msg = FTErrInfo_[static_cast<TFTErrorCode>(aCode)];
  char buf[16];
  sprintf(buf,"%02d",aCode);
//  const std::string& msg = GetSesMgr()->GetMsg((GetMarket()==Kway::Tw::Stk::m_Tse?"20":"92"),std::string(buf));
  const std::string& msg = GetSesMgr()->GetMsg(subsys.as_string(),std::string(buf));
  SetState(efsStop, msg);
  WriteLoger(SesName(), msg);

  if(Transaction_ != NULL)
  {
    Transaction_->OnFinish();
    UpdateFTStatus(fft_Abort, aCode, msg.c_str());
    delete Transaction_;
    Transaction_ = NULL;
  }
  return false;
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSes::OnSubSysReady(TApKey aKey)
{
   Status_.SetStatus(SStatusStr::Linked);
   if(GetExcMgr())
      GetExcMgr()->Alert("XF2001",ID_);
   RecID_ = GetBxMgr()->WritePVCStatus(RecID_, &Status_);
  SetState(efsStop, GetMarket().as_string(true) + " Ready!");
  SesMgr_.RegFTSes(GetBrokerID(), this);
  if(!GetMes())
     return;
  TAlarmMsg  Alarm(GetPvcID().as_string());
  Alarm.SetMsg(SMsg::ACode_APPReady, GetMes()->GetSetting().c_str(), "APP Sys Ready");
  SesMgr_.WriteAlarm(&Alarm);
  inherited::StopTimer();
  StartHB();
// 940119 by eric 連線成功後要先檢查queue 裏是否仍有要求等待傳送
  if(FTDirect_ == eftdSend)
     SesMgr_.FetchNextJob(GetBrokerID(), GetMarket());
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSes::OnSubSysBroken(TSubSysName subsys, TStCode code)
{
  StopHB();
  CheckStatusCode(subsys, code);
  SesMgr_.DelFTSes(GetBrokerID(), this);
}
//---------------------------------------------------------------------------
K_mf(bool) TbxFTSes::SettingApply(const std::string& setting)
{
  //設定範例 BrokerID,PassWord,PVCID
  //1520,1234,04,T
  std::string tmp = std::string("Creating session:")+setting;
  WriteLog(tmp.c_str(), tmp.length(), GetPvcID(), false, GetBrokerID());

  std::string setstr = setting;
  std::string::size_type pos = setting.find(std::string(",X"));
  if(pos != std::string::npos)
  {
    IsX25_ = true;
    setstr = setting.substr(0, pos);
  }
  setstr += ",";
  setstr += IsX25_ ? (char)akFT:(char)akEFT;
  // "1": FT 的 APCode
//  if(inherited::SettingApply(setting + "," + (char)akFT) == false)
//    return false;
  if(inherited::SettingApply(setstr) == false)
    return false;
  FileBlockSize_ = (IsX25_ ? sizeof(TData):sizeof(TLargeData));
//  FileBlockSize_ = sizeof(TData);
  AddLinkWorkers();
  WriteLog("Session Created!", 18, GetPvcID(), false, GetBrokerID());

  return true;
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSes::HBTimeOut(Kway::TTimerBase* tmbase)
{
  if(!IsExtend())  // TCP/IP
    return;
  if(IsSend())
  {
    // Send F170
    TControlHead head;
    head.SetHead(F2004_.GetWorkKey(), mgF170, 0);
    DoWorker(GetWorkKey(ckFTLink), mgF170, static_cast<TbxData&>(head));
  }
  else{  // too late, timeout - over 90 seconds without receive HB
    WriteLog("Time out, link confirm message expected!", 42, GetPvcID(), false, GetBrokerID()); //20060727 modified by eric
    ReLink();
  }
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSes::DoTimeOut(Kway::TTimerBase* tmbase)
{
  if(GetSesState() < efsError)
    inherited::DoTimeOut(tmbase);
  else if(GetSesState() < efsStop)  // not Send HB time-out
  {
     // too late, timeout - over 60 seconds without receive TSE reply
    if(!GetMes())
       return;
    TAlarmMsg  Alarm(GetPvcID().as_string());
    Alarm.SetMsg(SMsg::ACode_ApTimeout, GetMes()->GetSetting().c_str(), "Ap time out!");
    SesMgr_.WriteAlarm(&Alarm);
    ReLink();
  }else if(GetSesState() >= efsStop){
     StopTimer();
     SesMgr_.ChkRpyQue(TRptKey(GetBrokerID(), GetMarket()));
     if(!SesMgr_.FetchNextJob(GetBrokerID(), GetMarket()))
        StartTimer(60);
  }
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSes::UpdateFTStatus(EFTFlowID id, TStCode code, const char* msg)
{
  TFTCenter&  ft = const_cast<TFTCenter&> (GetCenter());
  TSenderMgr& mgr= const_cast<TSenderMgr&>(GetSender());
  const TFTJob* job;
  if(ft.GetHnd(GetBrokerID(), GetMarket(), WorkSysID_, &job))
  {
    ft.UpdateFTStatus(GetBrokerID(), GetMarket(), WorkSysID_, id, code, msg);
// 20070827 fft_Abort 時, 應該強制將queue中管制的file code 清除,以利進行下一個工作
    if(id == fft_Finish || id == fft_Abort)
      SesMgr_.RmRpyQue(TRptKey(GetBrokerID(), GetMarket()), job->FileCode_, id == fft_Abort ? true : false);
    mgr.SendNotify(*job,id==fft_Finish);
    if(id == fft_Abort)
       StartTimer(atoi(GetSysSetting().GetSetting(ssi_FTInterval).c_str())); // n秒後再取下一筆  
   }
}
//---------------------------------------------------------------------------
