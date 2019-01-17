//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "SystexNotify.h"
#include "NotifyMgr.h"
#include "FileLog.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx::FT;
//---------------------------------------------------------------------------
K_ctor TSystexNSes::TSystexNSes(TSenderMgr* aOwner)
    : inherited(1024,"\n"),
      inherited_Handler(aOwner),
      SenderMgr_(aOwner)
{
	Ready_ = false;
	SenderMgr_->RegSes((TFTNotifyHandler*)this);
}
//---------------------------------------------------------------------------
K_dtor TSystexNSes::~TSystexNSes()
{
	Ready_ = false;
	SenderMgr_->DelSes((TFTNotifyHandler*)this);
}
//---------------------------------------------------------------------------
K_mf(void) TSystexNSes::OnMesLinkReady(TMesBase* m)
{
	inherited::OnMesLinkReady(m);
	Ready_ = true;
}
//---------------------------------------------------------------------------
K_mf(void) TSystexNSes::OnMesLinkBroken(TMesBase* m, const std::string& s)
{
	Ready_ = false;
}
//---------------------------------------------------------------------------
K_mf(bool) TSystexNSes::DoNotify(const TFTJob& job)
{
   char buf[2048];
   std::string str;
   memset(SystexNotify_.Data_,0,sizeof(SystexNotify_.Data_));
   sprintf(buf,"%04d",(int)job.SeqNo_);
   SystexNotify_.SeqNo_.assign(buf);
   SystexNotify_.Market_   = job.Market_ == KStk::m_Tse ? 'T':'O';
   SystexNotify_.MsgType_  = (job.Flow_ == fft_Finish?'F':'M');
   SystexNotify_.FileCode_.assign(job.FileCode_.as_string());
   SystexNotify_.Time_.assign(TKTime::Now().as_string(false));
   switch(job.Flow_){
   case fft_Q_ing:     SystexNotify_.Status_ = 'Q'; str = "排隊中";   break;
   case fft_W_ing:     SystexNotify_.Status_ = 'W'; str = "己傳送,等候回報";   break;
   case fft_R_ing:     SystexNotify_.Status_ = 'R'; str = "接收中";   break;
   case fft_P_ing:     SystexNotify_.Status_ = 'P'; str = "處理中";   break;
   case fft_S_ing:     SystexNotify_.Status_ = 'S'; str = "傳送中";   break;
   case fft_Finish:    SystexNotify_.Status_ = 'Y'; break;
   case fft_Timeout:
   case fft_Abort:     SystexNotify_.Status_ = 'E'; str = job.LastMsg_.as_string();   break;
   default:            return true;
   }
   if(job.Flow_ == fft_Finish){
   	if(job.FileCode_.as_string() == "L11" || job.FileCode_.as_string() == "L13"){
   		SystexNotify_.FileCode_.assign("L17");
         str = (std::string("L17") + (job.Market_ == m_Tse ? "V.DAT":"OV.DAT"));
   	}else if(job.FileCode_.as_string() == "L12" || job.FileCode_.as_string() == "L14"){
   		SystexNotify_.FileCode_.assign("L18");
         str = (std::string("L18") + (job.Market_ == m_Tse ? "V.DAT":"OV.DAT"));
      }else
         str = (job.FileCode_.as_string() + (job.Market_ == m_Tse ? "V.DAT":"OV.DAT"));
/*
   	if(job.DstData_[0] == 0 || job.DstData_[0] == ' ')
   	   str = (job.Market_ == KStk::m_Tse ? "T":"O"+job.FileCode_.as_string()+"."+SystexNotify_.SeqNo_.as_string());
   	else
   	   str = job.DstData_.as_string();
*/
   	sprintf(SystexNotify_.Data_,"%-300s\n",str.c_str());
   }else{
   	sprintf(SystexNotify_.Data_,"%04d%-296s\n",str.length(),str.c_str());
   }	
   memcpy(buf,&SystexNotify_,sizeof(SystexNotify_));
   return Send((void*)&buf, std::string(buf).length());
}
//---------------------------------------------------------------------------


