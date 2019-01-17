//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "ExcMgrBase.h"
#include "FTRCSes.h"
#include "DspMgr.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw::Bx;
using namespace Kway::Tw::Bx::FT;
static char Msg_ORBroken[]= "RM02";  //Â¶³æ¤¤Â_½u
//---------------------------------------------------------------------------
K_ctor TRCSes::TRCSes(Kway::TSesMgr* sm)
     :Timer_(this)
{
   SesMgr_ = (TRSesMgr*)sm;
}
//---------------------------------------------------------------------------
K_dtor TRCSes::~TRCSes()
{
   Timer_.Stop();
   SesMgr_->SesDel(this);
}
//---------------------------------------------------------------------------
K_mf(void) TRCSes::OnMesLinkReady(TMesBase* M_)
{
   IP_ = GetMes()->GetUID();
   RIP_ = IP_;
   std::string::size_type pos = IP_.find(std::string(","));
   if(pos != std::string::npos){
    RIP_ = IP_.substr(0, pos);
   }
   if(GetExcMgr())
      GetExcMgr()->Alert("R001",RIP_);
   ShowMsg("Client Link Ready!");
   RemoteState_.clear();
   ResetTimer(RTime);
   SesMgr_->SesReg(this);
}
//---------------------------------------------------------------------------
K_mf(void) TRCSes::OnTimer(TTimerBase* t)
{
   SFTrm pkt;
   pkt.Code_   = orHB;
   pkt.Size_   = HSize;
   pkt.HostID_ = SesMgr_->GetLocalHID();
   Send(pkt);
}
//---------------------------------------------------------------------------
K_mf(void) TRCSes::ResetTimer(size_t secs)
{
   Timer_.Stop();
   Timer_.Start(secs*1000/2, &this_type::OnTimer);
}
//---------------------------------------------------------------------------
K_mf(void) TRCSes::OnMesLinkFail(TMesBase*, const std::string&)
{
   if(GetExcMgr())
      GetExcMgr()->Alert("R011",RIP_);
   ShowMsg("Link Fail!");
};
//---------------------------------------------------------------------------
K_mf(void) TRCSes::OnMesLinkBroken(TMesBase*, const std::string&)
{
   if(GetExcMgr())
      GetExcMgr()->Alert("R012",RIP_);
   Timer_.Stop();
   ShowMsg("Link Broken!");
   SesMgr_->ResetState(RemoteHID_);
   SesMgr_->SesDel(this);
}
//---------------------------------------------------------------------------
K_mf(int) TRCSes::GetPacketSize(SFTrm* pkt,bool f)
{
   return pkt->Size_;
}
//---------------------------------------------------------------------------
K_mf(void) TRCSes::OnPktRecv(SFTrm& pkt)
{
   ResetTimer(RTime);
   switch(pkt.Code_){
      case orHBEcho  : ShowMsg("orHBEcho");break;
      case orHB  : RemoteHID_ = pkt.HostID_; break;
      case orReqE: 
         ShowMsg("orReqE");
         break;
      case orState  :{
         ShowMsg("orState");
         RemoteHID_ = pkt.HostID_;
         TState ls;
         memcpy(&ls, pkt.buf_, pkt.Size_ - HSize);
         Remote3TtmpPath_    = ls.LocaltmpPath_.as_string();
         TStates::iterator i = RemoteState_.fetch(ls.BrkMkt_);
         i->second = ls;
         SesMgr_->ManageState(ls, RemoteHID_);
         SFTrm pkts;
         pkts.Code_ = orHBEcho;
         pkts.Size_ = HSize;
         pkts.HostID_ = SesMgr_->GetLocalHID();
         Send(pkts);
         break;
      }
      default:return;
   }
}
//---------------------------------------------------------------------------
K_mf(bool) TRCSes::SendReq(const TFTJob* job)
{
/*
   std::string LocalPath3T = GetDispatchMgr().Get3TLocalPath();
   if(job->Type_ == ftt_File && LocalPath3T != std::string() && Remote3TtmpPath_ != std::string()){
      char cmd[512];
      sprintf(cmd,"scp %s/%s %s:%s/%s", LocalPath3T.c_str(), job->SrcData_.as_string().c_str(), RIP_.c_str(),
                                        Remote3TtmpPath_.c_str(), job->SrcData_.as_string().c_str());
      ShowMsg(cmd);
      FILE* ptr;
      if((ptr = popen(cmd,"r")) == NULL){
         ShowMsg("File replication fail, must support SCP command!");
      }else{
         ShowMsg("File replication success!");
      }
      pclose(ptr);
   }
*/
   ShowMsg("orNewReq");
   SFTrm pkt;
   pkt.Code_ = orReq;
   pkt.Size_ = sizeof(TFTNewJob)+ HSize;
   pkt.HostID_ = SesMgr_->GetLocalHID();
   memcpy(pkt.buf_, job, sizeof(TFTNewJob));
   Send(pkt);
   return true;
}
//---------------------------------------------------------------------------
K_mf(void) TRCSes::ShowMsg(char* buf)
{
   char tmp[128];
   sprintf(tmp,"[%d]Client: %s",(Kway::word)RemoteHID_,buf);
   SesMgr_->WriteLog(tmp);
}
//---------------------------------------------------------------------------
K_mf(bool) TRCSes::SettingApply(const std::string& setting)
{
   IP_ = GetMes()->GetUID();
   RIP_ = IP_;
   std::string::size_type pos = IP_.find(std::string(","));
   if(pos != std::string::npos){
    RIP_ = IP_.substr(0, pos);
  }
}
//---------------------------------------------------------------------------
