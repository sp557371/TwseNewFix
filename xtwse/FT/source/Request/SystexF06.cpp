//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "SystexF06.h"
#include "FTJobDef.h"
#include "KTime.h"
#include "TwStk.h"
#include "FileLog.h"
#include "FTSysSetting.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx::FT;
//---------------------------------------------------------------------------
K_ctor TSystexF06Ses::TSystexF06Ses(TAPIMgr* aOwner, TFTFlowHandler* aInter, TFTCenterHandler* aCenter)
    : inherited(1024,"\n"),
      Interface_(*aInter),
      Center_   (*aCenter),
      APIMgr_   (aOwner)
{
}
//---------------------------------------------------------------------------
K_dtor TSystexF06Ses::~TSystexF06Ses()
{
}
//---------------------------------------------------------------------------
K_mf(void) TSystexF06Ses::OnMesLinkReady(TMesBase* m)
{
	inherited::OnMesLinkReady(m);
	LastSeq_ = APIMgr_->GetF06SeqNo(BrkID_,Mkt_);
}
//---------------------------------------------------------------------------
K_mf(bool) TSystexF06Ses::SettingApply(const std::string& str)
{
   std::string::size_type   pos1 = 0;
   std::string::size_type   pos2 = 0;
   pos1 = str.find(',', pos2);
   if(pos1 != std::string::npos){
      pos2 = pos1 + 1;
      BrkID_.assign(str.substr(0, pos1));
      if(str[pos2] == 'T')
         Mkt_ = m_Tse;
      else
         Mkt_ = m_Otc;
	}else{
		OnStateChanged("³]©w¿ù»~:BrokerID,Market");
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
K_mf(void) TSystexF06Ses::OnPktRecv(void* buffer, int sz)
{
   TFTNewJob job;
   char_ary<char, 5,' '> recseq;
   if(sz >= sizeof(recseq)){
      memcpy(&recseq, buffer, sizeof(recseq));
      if(recseq.as_int() <= LastSeq_)
         return;
      char filename[256];
      TFTSysSetting& sysset = const_cast<TFTSysSetting&>(GetSysSetting());
      std::string path = sysset.GetSetting(ssi_SaveTo);
      sprintf(filename,"F06S.%05d",recseq.as_int());
      path += "/"+std::string(filename);
      TFile F06(path.c_str(),TFileMode(fmWrite|fmAppend|fmOpenAlways|fmCreatePath));
      if(F06.IsOpened()){
      	F06.Write(buffer,sz);
      	F06.Close();
      }
      LastSeq_ = recseq.as_int();
      job.SeqNo_      = recseq.as_int();
      job.SourceID_   .assign(BrkID_.as_string().c_str());
      job.DestID_     = TSEID_;
      job.Market_     = Mkt_;
      job.FileCode_   .assign("F06");
      job.Type_       = ftt_File;
      job.Mode_       = ftm_Report;
      job.RecLen_     = 0;
      job.SrcData_    .assign(filename);
      job.DstData_.assign(job.FileCode_.as_string() + (job.Market_ == m_Tse ? "V.DAT":"OV.DAT"));
      job.CreateTime_ = Kway::TKTime::Now();
   }else
      return;
   APIMgr_->WriteF06SeqNo(BrkID_, Mkt_, LastSeq_);
   TSysID id = Interface_.DoReq(job);
   Center_.DoSend(job.SourceID_, job.Market_, id);

}
//---------------------------------------------------------------------------
