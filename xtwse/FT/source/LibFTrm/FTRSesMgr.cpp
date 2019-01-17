#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FTRSesMgr.h"
#include "FTRCSes.h"
#include "FTRSSes.h"
#include "MesRegs.h"
#include "bxSesMgr.h"
#include "SysMgrObjs.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx;
using namespace Kway::Tw::Bx::FT;
/////////////////////////////////////////////////////////////////////////////
static TSesBase*      TFTrmCSes_Creater  (const TSesReg&,Kway::TSesMgr& SesMgr){
   TRCSes* Ses = new TRCSes(&SesMgr);
   return (TSesBase*)(Ses);
}
//---------------------------------------------------------------------------
static TSesBase*      TFTrmSSes_Creater  (const TSesReg&,Kway::TSesMgr& SesMgr){
   TRSSes* Ses = new TRSSes(&SesMgr);
   return (TSesBase*)(Ses);
}
//---------------------------------------------------------------------------
static const TSesReg  SesRegs[] = {{"繞送傳送端",TFTrmCSes_Creater,},
                                   {"繞單接收端",TFTrmSSes_Creater,},
                                  };
/////////////////////////////////////////////////////////////////////////////
K_ctor TRSesMgr::TRSesMgr  ()
   :inherited("./Logs/%4y%2m%2d/ModifyLog/RSesMgr.log"),
    inherited_FT()
{
   ReloadSetting("./Settings/RSes.rec");
   Reg(Kway::TMesRegs::GetDefaultMesRegs());
   Reg(SesRegs, SesRegs+numofele(SesRegs));
   Kway::TKDate Date;
   Date = Date.Now();
   char Buf[128];
   sprintf(Buf,"./Logs/%s/rm.log",TKDate::Now().as_string(TKDate::sf_ELong,0).c_str());
   ORRecorder_.Open(Buf, (TFileMode)(fmRandomAccess|fmOpenAlways|fmCreatePath));
   ORRecorder_.Seek(0, fsEnd);
   RSetting_ = new TSettingPage("./Settings/ORM.rec","./Logs/%4y%2m%2d/ModifyLog/RM.log");
   RSetting_->Reg(&UseRM_,"啟用繞送機制","NULL");
   RSetting_->Reg(&UseRD_,"啟用分流機制","NULL");
   RSetting_->SettingApply();
   const TSysMgrObjs&  sys=GetSysMgrObjs();
   VAMgr_.Add("1.連線狀態", GetStatusVACreater());
   VAMgr_.Add("2.連線設定", GetSettingVACreater());
   VAMgr_.Add("3.其他設定", *RSetting_);
   sys.SysVAMgr_->Add("繞送模組",VAMgr_);
   HostRecs_ = new THostRecs();
   LocalHID_ = GetHostID();
}
//---------------------------------------------------------------------------
K_ctor TRSesMgr::~TRSesMgr  ()
{
   while(SesList_.size())
      delete((TRCSes*)(*SesList_.begin()));
   while(CliList_.size())
      delete((TRSSes*)(*CliList_.begin()));
   ORRecorder_.Close();
   const TSysMgrObjs&  sys=GetSysMgrObjs();
   sys.SysVAMgr_->Del(VAMgr_);
   VAMgr_.Del(GetSettingVACreater());
   VAMgr_.Del(GetStatusVACreater());
   VAMgr_.Del(*RSetting_);
   delete RSetting_;
   delete HostRecs_;
}
//---------------------------------------------------------------------------
K_mf(bool) TRSesMgr::SendReq(const TFTJob* job, int hid)
{
   TRCSes* Ses;
   int host = hid;
   if(hid == -1){
   	eLState ls = GetState(TBrkMkt(job->SourceID_,job->Market_), host);
   	if(ls < elsSend)
   	   return false;
   }
   if(SesList_.size())
      for (TSesList::iterator L = SesList_.begin(); L!= SesList_.end(); L++){
         Ses = ((TRCSes*)*L);
         if(Ses->GetRemoteHID() == host){
            
            Ses->SendReq(job);
            THostRec& hr = HostRecs_->fetch(TBrkMkt(job->SourceID_,job->Market_))->second.fetch(host)->second;
            TFCodes::iterator i = hr.FCodes_.find(job->FileCode_);
            if(i == hr.FCodes_.end())
               hr.FCodes_.fetch(job->FileCode_);
            else
               hr.QJobs_++;
            return true;
         }
      }
   return false;
}
//---------------------------------------------------------------------------
K_mf(void) TRSesMgr::SesReg(TSesBase* Ses)
{
   TSesList::iterator L = std::find(SesList_.begin(),SesList_.end(),Ses);
   if(L == SesList_.end())
      SesList_.push_back(Ses);
}
//---------------------------------------------------------------------------
K_mf(void) TRSesMgr::SesDel(TSesBase* Ses)
{
   TSesList::iterator L = std::find(SesList_.begin(),SesList_.end(),Ses);
   if(L != SesList_.end())
      SesList_.erase(L);
}
//---------------------------------------------------------------------------
K_mf(void) TRSesMgr::RegCli(TSesBase* Ses)
{
   TSesList::iterator L = std::find(CliList_.begin(),CliList_.end(),Ses);
   if(L == CliList_.end())
      CliList_.push_back(Ses);
   for(TStates::iterator i = LocalState_.begin(); i != LocalState_.end(); i++)
      ((TRSSes*)Ses)->SendState(i->second, LocalHID_);
}
//---------------------------------------------------------------------------
K_mf(void) TRSesMgr::DelCli(TSesBase* Ses)
{
   TSesList::iterator L = std::find(CliList_.begin(),CliList_.end(),Ses);
   if(L != CliList_.end())          
      CliList_.erase(L);
}
//---------------------------------------------------------------------------
K_mf(void) TRSesMgr::SendState(TState ls)
{
   TStates::iterator i = LocalState_.fetch(ls.BrkMkt_);
   i->second = ls;
   TProcRecs::iterator L = ProcRecs_.find(ls.BrkMkt_);
   if(L != ProcRecs_.end() && L->second.size()){
      for(int ict = 0; ict < ls.Count_; ict++){
   	   TFCodes::iterator L2 = L->second.find(ls.FCode_[ict]);
   	   if(L2 != L->second.end())
   	      L->second.erase(L2);
      }
   }
   if(CliList_.size() > 0)
      for (TSesList::iterator L3 = CliList_.begin(); L3!= CliList_.end(); L3++) 
         ((TRSSes*)(*L3))->SendState(ls, LocalHID_);
}
//---------------------------------------------------------------------------
K_mf(void) TRSesMgr::WriteLog(char* buf)
{
   char tmp[128];
   sprintf(tmp,"%s %s\n",TKDateTimeMS::Now().as_string(TKDate::sf_ELong).c_str(), buf);
   ORRecorder_.Write(tmp);
}
//---------------------------------------------------------------------------
K_mf(eLState) TRSesMgr::GetState(TBrkMkt m, int& hid)          
{ 
   THostRecs::iterator L = HostRecs_->find(m);
   eLState ls = elsNone;
   hid = -1;
   if(L == HostRecs_->end() || L->second.size() == 0)
      return elsNone;
   for(TRecs::iterator i = L->second.begin(); i != L->second.end(); i++){
      if(i->second.State_ > ls){
         ls = i->second.State_;
         hid = i->first;
      }
   }
   return ls;
}
//---------------------------------------------------------------------------
K_mf(int) TRSesMgr::TestHandle(TBrkMkt m,TFileCode fcode)
{
   THostRecs::iterator L = HostRecs_->find(m);
   if(L == HostRecs_->end() || L->second.size() == 0)
      return -1;
   for(TRecs::iterator i = L->second.begin(); i != L->second.end(); i++){
      if(i->second.FCodes_.size() == 0)
         return -1;
      TFCodes::iterator fi = i->second.FCodes_.find(fcode);
      if(fi != i->second.FCodes_.end())
         return i->first;
   }
   return -1;
};                                              
//---------------------------------------------------------------------------
K_mf(void) TRSesMgr::ManageState(TState e, Kway::byte hid)
{
   THostRec hr;
   hr.QJobs_ = e.QJobs_;
   hr.State_ = e.Recv_?elsRecv:elsNone;
   if(e.Send_)
      (Kway::byte)hr.State_ |= elsSend;
   hr.FCodes_.clear();
   if(e.Count_ > 0)
      for(int i = 0; i < e.Count_; i++)
         hr.FCodes_.fetch(e.FCode_[i]);
   THostRecs::iterator L1 = HostRecs_->find(e.BrkMkt_);
   if(L1 == HostRecs_->end())
   	L1 = HostRecs_->fetch(e.BrkMkt_);
   TRecs::iterator L2 = L1->second.fetch(hid);
   L2->second = hr;
}
//---------------------------------------------------------------------------
K_mf(void) TRSesMgr::ResetState(Kway::byte hid)
{
   for(THostRecs::iterator L = HostRecs_->begin(); L != HostRecs_->end(); L++){
      for(TRecs::iterator i = L->second.begin(); i != L->second.end(); i++){
         if(i->first == hid){
         	L->second.erase(i);
         	break;
         }
      }
   }
}
//---------------------------------------------------------------------------
K_mf(int) TRSesMgr::Dispatchable(TBrkMkt m, int count, TFileCode fc)
{
   if(!IsUseORD())
      return -1;
   TProcRecs::iterator p1 = ProcRecs_.find(m);
   if(p1 != ProcRecs_.end()){
      TFCodes::iterator p2 = p1->second.find(fc);
      if(p2 != p1->second.end())
         return -1;
   }
   THostRecs::iterator L = HostRecs_->find(m);
   if(L != HostRecs_->end())
      for(TRecs::iterator i = L->second.begin(); i != L->second.end(); i++){
         if((i->second.FCodes_.size()+i->second.QJobs_) < count && i->second.State_ == elsAll)
            return i->first;
      }
   return -1;
};                                              
//---------------------------------------------------------------------------
K_mf(void) TRSesMgr::AddNewFCode(TBrkMkt bm, TFileCode fc)
{
   ProcRecs_.fetch(bm)->second.fetch(fc);
}
//---------------------------------------------------------------------------

