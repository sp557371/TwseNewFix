//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "KwayReq.h"
#include "FTJobDef.h"
#include "KTime.h"
#include "TwStk.h"
#include "FileLog.h"
//---------------------------------------------------------------------------
#define FTSNAME     "FTS"
using namespace Kway::Tw::Bx::FT;
//---------------------------------------------------------------------------
#ifdef K_UNIXLIKE
static char* strupr(char* str)
{
  for(char* p=str; *p; p++)
    if(isascii(*p))
      *p = (char)toupper(*p);
  return str;
}
#endif
//---------------------------------------------------------------------------
K_ctor TAPISes::TAPISes(TAPIMgr* aOwner, TFTFlowHandler* aInter, TFTCenterHandler* aCenter)
    : inherited(static_cast<Kway::TSesMgr*>(aOwner)),
      Interface_(*aInter),
      Center_   (*aCenter),
      APIMgr_   (aOwner)
{
  HeadCode_ = (char)HEAD_CODE;
}
//---------------------------------------------------------------------------
K_dtor TAPISes::~TAPISes()
{
}
//---------------------------------------------------------------------------
K_mf(int) TAPISes::GetPacketSize (StXFile::SPkt& aPacket)
{
  return sizeof(StXFile::SHead);
}
//---------------------------------------------------------------------------
K_mf(int) TAPISes::BeforeAPacket (StXFile::SPkt& aPacket)
{
  return aPacket.Head_.Len_.as_int() + sizeof(StXFile::SHead) + sizeof(StXFile::TEndCode);
}
//---------------------------------------------------------------------------
K_mf(bool) TAPISes::APacket(StXFile::SPkt& aPacket)
{
  int sz = aPacket.Head_.Len_.as_int() - sizeof(aPacket.Type_);
  return bool(aPacket.Body_[sz] == (char)END_CODE);
}
//---------------------------------------------------------------------------
K_mf(void) TAPISes::AfterAPacket(StXFile::SPkt& aPacket)
{
  TFTNewJob job;
  if(aPacket.Type_ == '1') // file
  {
    StXFile::SFileRequest* req = reinterpret_cast<StXFile::SFileRequest*>(aPacket.Body_);
    req->NameData_[req->NameLen_.as_int()] = 0;
    if(APIMgr_)
      APIMgr_->WriteSendLog(reinterpret_cast<const char*>(req));
    if(Validate(*req) == false)
    {
      SendRep(ercDataErr);
      return;
    }
    job.SeqNo_      = req->SeqNo_.as_int();
    job.SourceID_   .assign(req->Broker_.as_string().c_str());
    job.DestID_     = TSEID_;
    job.Market_     = req->SecCode_ == '1' ? KStk::m_Tse : KStk::m_Otc;
    job.FileCode_   .assign(req->FileCode_.as_string().c_str());
    job.Type_       = ftt_File;
    job.Mode_       = ftm_Report;
    job.SrcData_    .assign(strupr(req->NameData_));    // 小寫全部轉成大寫
    char* p = strstr(req->NameData_, FTSNAME);
    if(p)
    {
      *(p+sizeof(FTSNAME)-2) = 'R';
      job.DstData_  .assign(req->NameData_);
    }
    job.RecLen_     = req->RecLen_.as_int();
    job.CreateTime_ = Kway::TKTime::Now();
  }
  else if(aPacket.Type_ == '2') // msg
  {
    StXFile::SMsgRequest* req = reinterpret_cast<StXFile::SMsgRequest*>(aPacket.Body_);
    req->MsgData_[req->MsgLen_.as_int()] = 0;
    if(APIMgr_)
      APIMgr_->WriteSendLog(reinterpret_cast<const char*>(req));
    if(Validate(*req) == false)
    {
      SendRep(ercDataErr);
      return;
    }
    job.SeqNo_      = req->SeqNo_.as_int();
    job.SourceID_   .assign(req->Broker_.as_string().c_str());
    job.DestID_     = TSEID_;
    job.Market_     = req->SecCode_ == '1' ? KStk::m_Tse : KStk::m_Otc;
    job.FileCode_   .assign(req->FileCode_.as_string().c_str());
    job.Type_       = ftt_Msg;
    job.Mode_       = ftm_BrkMsg;
    job.SrcData_    .assign(req->MsgData_);
    job.RecLen_     = req->MsgLen_.as_int();
    job.CreateTime_ = Kway::TKTime::Now();
  }
  else
  {
    if(APIMgr_)
      APIMgr_->WriteSendLog(reinterpret_cast<const char*>(&aPacket.Body_));
    SendRep(ercDataErr);
    return;
  }

  TSysID id = Interface_.DoReq(job);
  Center_.DoSend(job.SourceID_, job.Market_, id);
  SendRep(ercOK);
}
//---------------------------------------------------------------------------
K_mf(bool) TAPISes::Validate(StXFile::SFileRequest& req)
{
  return((req.SeqNo_    .as_int() > 0)                  &&
         (req.SecCode_ == '1' || req.SecCode_ == '2')   &&
         (!req.Broker_  .empty())                       &&
         (req.RecLen_   .as_int() > 0)                  &&
         (!req.FileCode_.empty())                       &&
         (req.NameLen_  .as_int() > 0)                  &&
         (strlen(req.NameData_)   > 0));
}
//---------------------------------------------------------------------------
K_mf(bool) TAPISes::Validate(StXFile::SMsgRequest& req)
{
  return((req.SeqNo_    .as_int() > 0)                  &&
         (req.SecCode_ == '1' || req.SecCode_ == '2')   &&
         (!req.Broker_  .empty())                       &&
         (!req.FileCode_.empty()));
}
//---------------------------------------------------------------------------
K_mf(bool) TAPISes::SendRep(eRetCode aCode)
{
  char buf[64];
  if(APIMgr_)
  {
    sprintf(buf, "%04d%c", sizeof(aCode), (char)(aCode+'0'));
    APIMgr_->WriteRecvLog(buf);
  }
  sprintf(buf, "%c%04d%c%c", HeadCode_, sizeof(aCode), (char)(aCode+'0'), END_CODE);
  return Send(reinterpret_cast<StXFile::SPkt&>(*buf));
}
//---------------------------------------------------------------------------



