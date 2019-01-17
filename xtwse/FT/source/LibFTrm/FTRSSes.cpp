//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FTRSSes.h"
#include "FTCenter.h"
#include "bxSesMgr.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw::Bx;
using namespace Kway::Tw::Bx::FT;
//---------------------------------------------------------------------------
K_ctor TRSSes::TRSSes(Kway::TSesMgr* sm)
   : Timer_(this)
{
   SesMgr_ = (TRSesMgr*)sm;
}
//---------------------------------------------------------------------------
K_dtor TRSSes::~TRSSes()
{
   Timer_.Stop();
   SesMgr_->DelCli(this);
}
//---------------------------------------------------------------------------
K_mf(void) TRSSes::OnMesLinkReady(TMesBase* M_)
{
   ResetTimer(RTime);
   ShowMsg("Link Ready!");
   SFTrm pkt;
   pkt.Code_   = orHB;
   pkt.Size_   = HSize;
   pkt.HostID_ = SesMgr_->GetLocalHID();
   Send(pkt);
   SesMgr_->RegCli(this);
}
//---------------------------------------------------------------------------
K_mf(void) TRSSes::OnTimer(TTimerBase* t)
{
   ShowMsg("HB Time out , Close!");
   Close();
}
//---------------------------------------------------------------------------
K_mf(void) TRSSes::ResetTimer(size_t secs)
{
   Timer_.Stop();
   Timer_.Start(secs*1000, &this_type::OnTimer);
}
//---------------------------------------------------------------------------
K_mf(void) TRSSes::OnMesLinkBroken(TMesBase*, const std::string&)
{
   Timer_.Stop();
   if(SesMgr_ == 0)
      return;
   ShowMsg("Link Broken!");
   SesMgr_->DelCli(this);
}
//---------------------------------------------------------------------------
K_mf(int) TRSSes::GetPacketSize(SFTrm* pkt,bool f)
{
   return pkt->Size_;
}
//---------------------------------------------------------------------------
K_mf(void) TRSSes::OnPktRecv(SFTrm& pkt)
{
   ResetTimer(RTime);
   switch(pkt.Code_){
      case orHB     : ShowMsg("orHB");pkt.Code_ = orHBEcho; Send(pkt); break;
      case orReq:{
         ShowMsg("Recv orReq");
         TFTNewJob job;
         memcpy(&job, pkt.buf_, sizeof(TFTNewJob));
         TBrkMkt bm(job.SourceID_,job.Market_);
         SesMgr_->AddNewFCode(bm, job.FileCode_);
         TFTCenter&   ft    = const_cast<TFTCenter&>(GetCenter());
         TSysID id = ft.DoReq(job);
         if(job.Type_ == ftt_File && job.EnvMode_ == TEnvMode(evm_3T)){
            std::string fname = job.SrcData_.as_string();
            std::string cmd = GetDispatchMgr().GetULCmd(job, fname, std::string());
            if(cmd != std::string())
               GetBxMgr()->DoFileCmd(id, job, cmd);
         }else
            GetBxMgr()->DoSend(job.SourceID_, job.Market_, id);
         pkt.Code_ = orReqE;
         pkt.Size_ = HSize;
         Send(pkt);
         break;
      }
      default:return;
   }
}
//---------------------------------------------------------------------------
K_mf(void) TRSSes::SendState(TState ls, Kway::byte hid)
{
   SFTrm pkt;
   pkt.Code_   = orState;
   pkt.Size_   = ls.GetSize() + HSize;
   pkt.HostID_ = hid;
   memcpy(pkt.buf_, &ls, ls.GetSize());
   Send(pkt);
}
//---------------------------------------------------------------------------
K_mf(void) TRSSes::ShowMsg(char* buf)
{
   char tmp[128];
   sprintf(tmp,"[%d]Server: %s",(Kway::word)HostID_,buf);
   SesMgr_->WriteLog(tmp);
}
//---------------------------------------------------------------------------

