//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "KwayNotify.h"
#include "NotifyMgr.h"
#include "FileLog.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx::FT;
//---------------------------------------------------------------------------
K_ctor TShellSes::TShellSes(TSenderMgr* aOwner)
    : inherited(aOwner),
      inherited_Handler(aOwner),
      SenderMgr_(aOwner){
  HeadCode_ = (char)HEAD_CODE;
   Ready_ = false;
   if(SenderMgr_)
      SenderMgr_->RegSes(this);
}
//---------------------------------------------------------------------------
K_dtor TShellSes::~TShellSes()
{
   Ready_ = false;
   if(SenderMgr_)
      SenderMgr_->DelSes(this);
}
//---------------------------------------------------------------------------
K_mf(void) TShellSes::OnMesLinkReady   (Kway::TMesBase*)
{
   Ready_ = true;
}
//---------------------------------------------------------------------------
K_mf(void) TShellSes::OnMesLinkBroken  (Kway::TMesBase*, const std::string& reason)
{
   Ready_ = false;
}
//---------------------------------------------------------------------------
K_mf(int) TShellSes::GetPacketSize (StXShell::SPkt& aPacket)
{
  return sizeof(StXShell::SHead);
}
//---------------------------------------------------------------------------
K_mf(int) TShellSes::BeforeAPacket(StXShell::SPkt& aPacket)
{
  return aPacket.Head_.Len_.as_int() + sizeof(StXShell::SHead) + sizeof(StXShell::TEndCode);
}
//---------------------------------------------------------------------------
K_mf(bool) TShellSes::APacket(StXShell::SPkt& aPacket)
{
   int sz = aPacket.Head_.Len_.as_int();;
   return bool(aPacket.Body_[sz] == (char)END_CODE);
}
//---------------------------------------------------------------------------
K_mf(bool) TShellSes::SendFeedBack(StXFile::SFileRequest& aPacket, char aProcFlag, char* aReplyCode,
    int aSeqNo)
{
  char temp[20];

  StXShell::SPkt aData;
  StXShell::SFeedBack* rtn = reinterpret_cast<StXShell::SFeedBack*>(&aData.Body_);

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
  int sz = szFeedBackHead + rtn->FileNameLen_.as_int();
  aData.Body_[sz] = (char)END_CODE;
  sprintf(temp, "%0*d", sizeof(aData.Head_.Len_), sz);
  aData.Head_.Len_.assign(temp);
  return Send(aData);
}
//---------------------------------------------------------------------------
K_mf(bool) TShellSes::DoNotify(const TFTJob& job)
{
  char temp[20];
  StXShell::SPkt aData;
  StXShell::SFeedBack* rtn = reinterpret_cast<StXShell::SFeedBack*>(&aData.Body_);

  rtn->Broker_   .assign(job.SourceID_.as_string());
  rtn->FileCode_ .assign(job.FileCode_.as_string());
  sprintf(temp, "%04d", (int)job.SeqNo_);
  rtn->SeqNo_    .assign(temp);
  rtn->SecCode_  = job.Market_ == KStk::m_Tse ? '1':'2';
  switch(job.Flow_)
  {
    case fft_Q_ing:     rtn->Process_ = 'Q';    break;
    case fft_Finish:    rtn->Process_ = 'Y';    break;
    case fft_Timeout:
    case fft_Abort:     rtn->Process_ = 'E';    break;
    default:            rtn->Process_ = 'P';    break;
  }
  if(job.FileCode_.as_string() == "S10")
    rtn->Status_.assign("S1");
  else
    rtn->Status_.assign(job.LastCode_.as_string());
  sprintf(temp, "%04d", job.DstData_.as_string().length());
  rtn->FileNameLen_.assign(temp);
  strncpy(rtn->FileName_, job.DstData_.as_string().c_str(), rtn->FileNameLen_.as_int());
  int sz = szFeedBackHead + rtn->FileNameLen_.as_int();
  aData.Body_[sz] = (char)END_CODE;
  sprintf(temp, "%0*d", sizeof(aData.Head_.Len_), sz);
  aData.Head_.Len_.assign(temp);
  if(SenderMgr_)
    SenderMgr_->WriteLog(aData.Body_, sz);
  return Send(aData);
}
//---------------------------------------------------------------------------


