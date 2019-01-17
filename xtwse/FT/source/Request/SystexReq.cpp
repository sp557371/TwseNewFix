//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "SystexReq.h"
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
K_ctor TSystexSes::TSystexSes(TAPIMgr* aOwner, TFTFlowHandler* aInter, TFTCenterHandler* aCenter)
    : inherited(1024,"\n"),
      Interface_(*aInter),
      Center_   (*aCenter),
      APIMgr_   (aOwner)
{
}
//---------------------------------------------------------------------------
K_dtor TSystexSes::~TSystexSes()
{
}
//---------------------------------------------------------------------------
K_mf(void) TSystexSes::OnMesLinkReady(TMesBase* m)
{
	inherited::OnMesLinkReady(m);
	LastSeq_ = APIMgr_->GetSeqNo(BrkID_);
}
//---------------------------------------------------------------------------
K_mf(bool) TSystexSes::SettingApply(const std::string& str)
{
	BrkID_.assign(str);
	return true;
}
//---------------------------------------------------------------------------
K_mf(void) TSystexSes::OnPktRecv(void* buffer, int sz)
{
   TFTNewJob job;
   job.EnvMode_.assignT(evm_2T);
   memset(SystexReq_.Data_,0,sizeof(SystexReq_.Data_));
   if(sz > sizeof(SSystexReq) - sizeof(SystexReq_.Data_)){
      memcpy(&SystexReq_, buffer, sz);
      if(SystexReq_.SeqNo_.as_int() <= LastSeq_)
         return;
      else
         LastSeq_ = SystexReq_.SeqNo_.as_int();
      if(SystexReq_.MsgType_ == 'F'){
      	if(SystexReq_.FileCode_.as_string() == "F06")
      	   return;
         job.SeqNo_      = SystexReq_.SeqNo_.as_int();
         job.SourceID_   .assign(BrkID_.as_string().c_str());
         job.DestID_     = TSEID_;
         job.Market_     = SystexReq_.Market_ == 'T' ? KStk::m_Tse : KStk::m_Otc;
         job.FileCode_   .assign(SystexReq_.FileCode_.as_string().c_str());
         job.Type_       = ftt_File;
         job.Mode_       = ftm_Report;
         job.RecLen_     = 0;
         char buf[16];
         memcpy(buf, SystexReq_.Data_, 16);
         for(int i = 0; i < 16; i++)
            if(buf[i]==' ' || buf[i] == '\r' || buf[i] == '\n'){
            	buf[i] = 0;
            	break;
            }
         job.SrcData_    .assign(buf);
         memcpy(buf, SystexReq_.Data_+16, 16);
         for(int i = 0; i < 16; i++)
            if(buf[i]==' ' || buf[i] == '\r' || buf[i] == '\n'){
            	buf[i] = 0;
            	break;
            }
         if(std::string(buf).length() == 0){
            if(job.FileCode_.as_string() != "L11" && job.FileCode_.as_string() != "L12" &&
               job.FileCode_.as_string() != "L13" && job.FileCode_.as_string() != "L14")
               job.DstData_.assign(job.FileCode_.as_string() + (job.Market_ == m_Tse ? "V.DAT":"OV.DAT"));
//            job.DstData_.assign(job.Market_.as_string()+job.FileCode_.as_string()+"."+SystexReq_.SeqNo_.as_string());
         }else
               job.DstData_    .assign(buf);
         job.CreateTime_ = Kway::TKTime::Now();
      }else if(SystexReq_.MsgType_ == 'M'){
         job.SeqNo_      = SystexReq_.SeqNo_.as_int();
         job.SourceID_   .assign(BrkID_.as_string().c_str());
         job.DestID_     = TSEID_;
         job.Market_     = SystexReq_.Market_ == 'T' ? KStk::m_Tse : KStk::m_Otc;
         job.FileCode_   .assign(SystexReq_.FileCode_.as_string().c_str());
         job.Type_       = ftt_Msg;
         job.Mode_       = ftm_BrkMsg;
         job.SrcData_    .assign(SystexReq_.Data_+4);
         Kstl::char_ary<char,4,0>  len;
         memcpy(len.begin(), SystexReq_.Data_, 4);
         job.RecLen_     = len.as_int();
         job.CreateTime_ = Kway::TKTime::Now();
      }else
         return;
   }else
      return;
   APIMgr_->WriteSeqNo(BrkID_, LastSeq_);
   TSysID id = Interface_.DoReq(job);
   Center_.DoSend(job.SourceID_, job.Market_, id);
}
//---------------------------------------------------------------------------
