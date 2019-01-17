//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Notify3T.h"
#include "NotifyMgr.h"
#include "FileLog.h"
#include "bxSesMgr.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx::FT;
//---------------------------------------------------------------------------
K_ctor TNotify3TSes::TNotify3TSes(TSenderMgr* aOwner)
    : inherited(aOwner),
      inherited_Handler(aOwner),
      SenderMgr_(aOwner),
      HBTimer_(this)
{
   EvMode_.assignT(evm_3T);
  HeadCode_ = (char)HEAD_CODE;
   Ready_ = false;
   if(SenderMgr_)
      SenderMgr_->RegSes(this);
}
//---------------------------------------------------------------------------
K_dtor TNotify3TSes::~TNotify3TSes()
{
   HBTimer_.Stop();
   Ready_ = false;
   if(SenderMgr_)
      SenderMgr_->DelSes(this);
}
//---------------------------------------------------------------------------
K_mf(void) TNotify3TSes::OnMesLinkReady   (Kway::TMesBase*)
{
   Ready_ = true;
   ResetTimer(HBInterval);
}
//---------------------------------------------------------------------------
K_mf(void) TNotify3TSes::OnMesLinkBroken  (Kway::TMesBase*, const std::string& reason)
{
   HBTimer_.Stop();
   Ready_ = false;
}
//---------------------------------------------------------------------------
K_mf(void) TNotify3TSes::OnHBTimer(TTimerBase* t)
{
   Close();
   Open();
}
//---------------------------------------------------------------------------
K_mf(void) TNotify3TSes::ResetTimer(size_t secs)
{
//20080116 暫時關閉, 以解決c11 會逾時的問題
//   HBTimer_.Stop();
//   HBTimer_.Start(secs*1000, &this_type::OnHBTimer);
}
//---------------------------------------------------------------------------
K_mf(int) TNotify3TSes::GetPacketSize (SNotifyPkt::SPkt& aPacket)
{
  return sizeof(SNotifyPkt::SHead);
}
//---------------------------------------------------------------------------
K_mf(int) TNotify3TSes::BeforeAPacket(SNotifyPkt::SPkt& aPacket)
{
  return aPacket.Head_.Len_.as_int() + sizeof(SNotifyPkt::SHead) + sizeof(SNotifyPkt::TEndCode);
}
//---------------------------------------------------------------------------
K_mf(bool) TNotify3TSes::APacket(SNotifyPkt::SPkt& aPacket)
{
   ResetTimer(HBInterval);
   char buf[256];
   int sz = aPacket.Head_.Len_.as_int();
   if(aPacket.Head_.Fun_[0] == '1'){  // upload cmd
      SNotifyPkt::SFileCmdEcho* req = reinterpret_cast<SNotifyPkt::SFileCmdEcho*>(&aPacket.Body_);
      sprintf(buf,"Upload echo : %s",(req->Result_[0]=='1'?"Success":"Fail"));
      SenderMgr_->WriteLog(buf, std::string(buf).length());
      GetBxMgr()->FileCmdEcho(req->SeqNo_.as_int(), (req->Result_[0]=='0'?false:true));
   }
   if(aPacket.Head_.Fun_[0] == '2'){  // download cmd
      SNotifyPkt::SFileCmdEcho* req = reinterpret_cast<SNotifyPkt::SFileCmdEcho*>(&aPacket.Body_);
      sprintf(buf,"Download echo : %s",(req->Result_[0]=='1'?"Success":"Fail"));
      SenderMgr_->WriteLog(buf, std::string(buf).length());
      SenderMgr_->FileDLEcho(req->SeqNo_.as_int(), (req->Result_[0]=='0'?false:true));
   }
   if(aPacket.Head_.Fun_[0] == '3'){ // 20070830 加入HB 機制
      sprintf(buf,"Heart Beat  : %s",GetMes()->GetUID().c_str());
      SenderMgr_->WriteLog(buf, std::string(buf).length());
      Send(aPacket);
   }
   return bool(aPacket.Body_[sz] == (char)END_CODE);
}
//---------------------------------------------------------------------------
K_mf(bool) TNotify3TSes::SendFeedBack(StXFile::SFileRequest& aPacket, char aProcFlag, char* aReplyCode,
    int aSeqNo)
{
  char temp[20];

  SNotifyPkt::SPkt aData;
  aData.Head_.Fun_.assign("0");
  SNotifyPkt::SFeedBack* rtn = reinterpret_cast<SNotifyPkt::SFeedBack*>(&aData.Body_);

  rtn->Broker_   .assign(aPacket.Broker_  .as_string());
  rtn->FileCode_ .assign(aPacket.FileCode_.as_string());
  if(aSeqNo == -1)
    rtn->SeqNo_  = aPacket.SeqNo_;
  else
  {
    sprintf(temp, "%0*d", sizeof(rtn->SeqNo_), aSeqNo);
    rtn->SeqNo_  .assign(temp);
  }
  rtn->SecCode_     = aPacket.SecCode_;
  rtn->Process_     .assign(aProcFlag);
  rtn->Status_      .assign(aReplyCode);
  rtn->FileNameLen_ .assign(aPacket.NameLen_.as_string());
  strncpy(rtn->FileName_, aPacket.NameData_, aPacket.NameLen_.as_int());
  int sz = szFeedBackH + rtn->FileNameLen_.as_int();
  aData.Body_[sz] = (char)END_CODE;
  sprintf(temp, "%0*d", sizeof(aData.Head_.Len_), sz);
  aData.Head_.Len_.assign(temp);
  return Send(aData);
}
//---------------------------------------------------------------------------
K_mf(bool) TNotify3TSes::DoNotify(const TFTJob& job)
{
  char temp[20];
  SNotifyPkt::SPkt aData;
  aData.Head_.Fun_.assign("0");
  SNotifyPkt::SFeedBack* rtn = reinterpret_cast<SNotifyPkt::SFeedBack*>(&aData.Body_);

  rtn->Broker_   .assign(job.SourceID_.as_string());
  rtn->FileCode_ .assign(job.FileCode_.as_string());
  sprintf(temp, "%04d", (int)job.SeqNo_);
  rtn->SeqNo_    .assign(temp);
  rtn->SecCode_  = job.Market_ == KStk::m_Tse ? '1':'2';
  rtn->ErrMsg_.clear();
  switch(job.Flow_){
    case fft_Q_ing:     rtn->Process_ = 'Q';    break;
    case fft_Finish:    rtn->Process_ = 'Y';    
                        sprintf(rtn->FileName_,"%s", job.DstData_.as_string().c_str());
                        break;
    case fft_Timeout:
    case fft_Abort:     rtn->Process_ = 'E';    
                        rtn->ErrMsg_.assign(job.LastMsg_.as_string()); 
                        break;
    default:            rtn->Process_ = 'P';    break;
  }
  if(job.FileCode_.as_string() == "S10")
     rtn->Status_.assign("S1");
  else{
     sprintf(temp,"%02d",job.LastCode_.as_int());
     rtn->Status_.assign(temp);
  }
  sprintf(temp, "%04d", job.DstData_.as_string().length());
  rtn->FileNameLen_.assign(temp);
  strncpy(rtn->FileName_, job.DstData_.as_string().c_str(), rtn->FileNameLen_.as_int());
  int sz = szFeedBackH + rtn->FileNameLen_.as_int();
  aData.Body_[sz] = (char)END_CODE;
  sprintf(temp, "%0*d", sizeof(aData.Head_.Len_), sz);
  aData.Head_.Len_.assign(temp);
  if(SenderMgr_)
    SenderMgr_->WriteLog(aData.Body_, sz);
  return Send(aData);
}
//---------------------------------------------------------------------------
K_mf(void) TNotify3TSes::SendFileCmd(bool isup, int seq, std::string& cmd)
{
   char buf[512];
   SNotifyPkt::SPkt aData;
   aData.Head_.Fun_.assign(isup?"1":"2");
   SNotifyPkt::SFileCmdReq* req = reinterpret_cast<SNotifyPkt::SFileCmdReq*>(&aData.Body_);
   sprintf(buf,"%0*d",sizeof(SNotifyPkt::TFileSeq),seq);
   req->SeqNo_.assign(buf);
   sprintf(req->Cmd_,"%s",cmd.c_str());
   int sz = sizeof(SNotifyPkt::TFileSeq) + cmd.length();
   aData.Body_[sz] = (char)END_CODE;
   sprintf(buf,"%0*d",sizeof(aData.Head_.Len_), sz);
   aData.Head_.Len_.assign(buf);
   if(SenderMgr_)
      SenderMgr_->WriteLog(aData.Body_, sz);
   Send(aData);
}
//---------------------------------------------------------------------------

