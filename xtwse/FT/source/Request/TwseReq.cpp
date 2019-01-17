//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseReq.h"
#include "FTJobDef.h"
#include "KTime.h"
#include "TwStk.h"
#include "FileLog.h"
//---------------------------------------------------------------------------
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
K_ctor TTwseReqSes::TTwseReqSes(TAPIMgr* aOwner, TFTFlowHandler* aInter, TFTCenterHandler* aCenter)
    : inherited(1024,"\n"),
      Interface_(*aInter),
      Center_   (*aCenter),
      APIMgr_   (aOwner)
{
}
//---------------------------------------------------------------------------
K_dtor TTwseReqSes::~TTwseReqSes()
{
}
//---------------------------------------------------------------------------
K_mf(void) TTwseReqSes::OnMesLinkReady(TMesBase* m)
{
	inherited::OnMesLinkReady(m);
	LastSeq_ = APIMgr_->GetSeqNo(BrkID_);
}
//---------------------------------------------------------------------------
K_mf(bool) TTwseReqSes::SettingApply(const std::string& str)
{
	BrkID_.assign(str);
	return true;
}
//---------------------------------------------------------------------------
K_mf(void) TTwseReqSes::OnPktRecv(void* buffer, int sz)
{
   TFTNewJob job;
   job.EnvMode_.assignT(evm_2T);
   if(sz >= sizeof(STwseReqBase)){
      memcpy(&TwseReq_, buffer, sizeof(STwseReqBase));
      if(TwseReq_.Time_.as_int() <= LastSeq_)
         return;
      else
         LastSeq_ = TwseReq_.Time_.as_int();
      if(TwseReq_.MsgType_ == 'F'){
         memset(&FileType_, 0, sizeof(TFType));
         memcpy(&FileType_, buffer, sz);
         job.SeqNo_      = FileType_.Time_.as_int();
         job.SourceID_   .assign(BrkID_.as_string().c_str());
         job.DestID_     = TSEID_;
         job.Market_     = FileType_.Market_ == 'T' ? KStk::m_Tse : KStk::m_Otc;
         job.FileCode_   .assign(FileType_.FileCode_.as_string().c_str());
         if(!FileType_.ULName_.empty())
            job.SrcData_.assign(FileType_.ULName_.as_string());
         job.Type_       = ftt_File;
         job.Mode_       = ftm_Report;
         job.RecLen_     = 0;
         char buf[16];
         sprintf(buf,FileType_.ULName_.as_string().c_str());
         for(int i = 0; i < 16; i++)
            if(buf[i]==' ' || buf[i] == '\r' || buf[i] == '\n'){
            	buf[i] = 0;
            	break;
            }
         job.SrcData_    .assign(buf);
         sprintf(buf,FileType_.DLName_.as_string().c_str());
         for(int i = 0; i < 16; i++)
            if(buf[i]==' ' || buf[i] == '\r' || buf[i] == '\n'){
            	buf[i] = 0;
            	break;
            }
         if(std::string(buf).length() == 0){
            if(job.FileCode_.as_string() != "L11" && job.FileCode_.as_string() != "L12" &&
               job.FileCode_.as_string() != "L13" && job.FileCode_.as_string() != "L14")
               job.DstData_.assign(job.FileCode_.as_string() + (job.Market_ == m_Tse ? "V.DAT":"OV.DAT"));
         }else
               job.DstData_    .assign(buf);
         job.CreateTime_ = Kway::TKTime::Now();
      }else if(TwseReq_.MsgType_ == 'M'){
         memset(&MsgType_, 0, sizeof(TMsgType));
         memcpy(&MsgType_, buffer, sz);
         job.SeqNo_      = MsgType_.Time_.as_int();
         job.SourceID_   .assign(BrkID_.as_string().c_str());
         job.DestID_     = TSEID_;
         job.Market_     = MsgType_.Market_ == 'T' ? KStk::m_Tse : KStk::m_Otc;
         job.FileCode_   .assign(MsgType_.FileCode_.as_string().c_str());
         job.Type_       = ftt_Msg;
         job.Mode_       = ftm_BrkMsg;
         job.SrcData_    .assign(MsgType_.Data_.as_string());
         job.RecLen_     = MsgType_.Length_.as_int();
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
