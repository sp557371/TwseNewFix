//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "DspMgr.h"
#include "NotifyMgr.h"
#include "FTFlowDef.h"
#include "SysMgrObjs.h"
#include "FTSysSetting.h"
//#include "FileLog.h"
#include "KwayNotify.h"
#include "SystexNotify.h"
#include "Notify3T.h"
#include "MesRegs.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx;
using namespace Kway::Tw::Bx::FT;
//---------------------------------------------------------------------------
K_fn(const TSenderMgr&) Kway::Tw::Bx::FT::GetSender()
{
  static TSenderMgr senderMgr;
  return senderMgr;
}
//---------------------------------------------------------------------------
static Kway::TSesBase* TShellSesVAC_Creater(const Kway::TSesReg&, Kway::TSesMgr& aSesMgr)
{
  TSenderMgr* mgr = static_cast<TSenderMgr*>(&aSesMgr);
  return dynamic_cast<Kway::TSesBase*>(new TShellSes(mgr));
}
//---------------------------------------------------------------------------
static Kway::TSesBase* TNotify3TSes_Creater(const Kway::TSesReg&, Kway::TSesMgr& aSesMgr)
{
  TSenderMgr* mgr = static_cast<TSenderMgr*>(&aSesMgr);
  return dynamic_cast<Kway::TSesBase*>(new TNotify3TSes(mgr));
}
//---------------------------------------------------------------------------
static Kway::TSesBase* TSystexNotify_Creater(const Kway::TSesReg&, Kway::TSesMgr& aSesMgr)
{
  TSenderMgr* mgr = static_cast<TSenderMgr*>(&aSesMgr);
  return dynamic_cast<Kway::TSesBase*>(new TSystexNSes(mgr));
}
//---------------------------------------------------------------------------
static const Kway::TSesReg  SesReg[] = {
    {"ShellSes"    , TShellSesVAC_Creater },
    {"通用回覆介面", TSystexNotify_Creater },
    {"三段式回覆介面", TNotify3TSes_Creater },
};
//---------------------------------------------------------------------------
K_ctor TSenderMgr::TSenderMgr()
    : inherited(),
      Timer_(this)
{
  static bool isCreate;
  SeqNo_ = 0;
  if(isCreate == false)
  {
    SesVec_.reserve(64);  // 64 pointer
    Reg(Kway::TMesRegs::GetDefaultMesRegs());
    Reg(SesReg, SesReg+numofele(SesReg));
    ReloadSetting(GetProcConfPath()+"/Sender.Ses");

    const TSysMgrObjs& sys = GetSysMgrObjs();
    VAMgr_.Add("1.連線狀態", GetStatusVACreater());
    VAMgr_.Add("2.連線設定", GetSettingVACreater());
//    sys.SysVAMgr_->Add("訊息通知", VAMgr_);

    std::string fname = GetProcLogPath()+"/CallDB.log";
    TFileMode fmode = TFileMode(fmWrite|fmAppend|fmOpenAlways|fmCreatePath);
    Log_.Open(fname.c_str(), fmode);
    isCreate = true;
  }
   Setting_ = new TSettingPage(std::string(GetProcConfPath()+"/Notify.rec").c_str(),
                               std::string(GetProcLogPath()+"/ModifyLog/Notify.log").c_str());
   NotifyFirst_ = false;
   Setting_->Reg(&NotifyFirst_,"相同連線,僅通知第一個可用連線","");
   Setting_->SettingApply();
   VAMgr_.Add("3.通知設定", *Setting_);
   Timer_.Start(1000, &this_type::OnTimer); // 每一秒鐘檢核一次是否有到達條件的工作要通知
}
//---------------------------------------------------------------------------
K_dtor TSenderMgr::~TSenderMgr()
{
   UnlinkAll();
   Timer_.Stop();
   VAMgr_.Del(GetStatusVACreater());
   VAMgr_.Del(GetSettingVACreater());
   VAMgr_.Del(*Setting_);
   delete Setting_;
   const TSysMgrObjs& sys = GetSysMgrObjs();
//   sys.SysVAMgr_->Del(VAMgr_);
}
//---------------------------------------------------------------------------
K_mf(void) TSenderMgr::RegSes(TFTNotifyHandler* ses)
{
  //先檢查有沒相同的Ses 已註冊,若有則不再重覆註冊
  TSesVecI iter = std::find(SesVec_.begin(), SesVec_.end(), ses);
  if(iter == SesVec_.end())
  {
    SesVec_.push_back(ses);
    PushNotify();
  }
}
//---------------------------------------------------------------------------
K_mf(void) TSenderMgr::DelSes(TFTNotifyHandler* ses)
{
  TSesVecI iter = std::find(SesVec_.begin(), SesVec_.end(), ses);
  if(iter != SesVec_.end())
    SesVec_.erase(iter);
}
//---------------------------------------------------------------------------
K_mf(void) TSenderMgr::UnlinkAll()
{
  for(TSesVecI iter = SesVec_.begin(); iter != SesVec_.end(); ++iter)
    (*iter)->Unlink();
}
//---------------------------------------------------------------------------
K_mf(void) TSenderMgr::PushNotify()
{
  while(Queue_.empty() == false && SesVec_.size() > 0)  // has one or more session
  {
    const TFTJob& job = Queue_.front();
    bool N2T = false;
    bool N3T = false;
    for(TSesVecI iter = SesVec_.begin(); iter != SesVec_.end(); ++iter)
      if((*iter)->IsReady()){
      	if(job.Flow_ == fft_Finish){
      	   if(job.Done3T_ == false){
               if(((*iter)->GetEvMode() == TEnvMode(evm_2T)) && ((!NotifyFirst_) || (!N2T))){
                  (*iter)->DoNotify(job);
                  N2T = true;
               }
            }else{
               if(((*iter)->GetEvMode() == TEnvMode(evm_3T)) && ((!NotifyFirst_) || (!N3T))){
                  (*iter)->DoNotify(job);
                  N3T = true;
               }
            }
         }else{
            if(((*iter)->GetEvMode() == TEnvMode(evm_2T)) && ((!NotifyFirst_) || (!N2T))){
               (*iter)->DoNotify(job);
                  N2T = true;
            }
            if(((*iter)->GetEvMode() == TEnvMode(evm_3T)) && ((!NotifyFirst_) || (!N3T))){
               (*iter)->DoNotify(job);
               N3T = true;
            }
         }
      }
    Queue_.pop();
  }
}
//---------------------------------------------------------------------------
K_mf(bool) TSenderMgr::WriteLog(const char* data, size_t len)
{
   if(Log_.IsOpened())
   {
     static char buf[255];
     size_t sz = sprintf(buf, "%s.%s ", TKTime::Now().as_string().c_str(), TKTimeMS::Now().as_string().c_str());
     memcpy(&buf[sz], data, len);
     buf[sz+len] = 0;
     Log_.WriteLN(buf);
     return true;
   }
   return false;
}
//---------------------------------------------------------------------------
K_mf(void) TSenderMgr::SendNotify(const TFTJob& job, bool t)
{
   if(job.Flow_ == fft_Finish){
      Kway::word interval = (GetSysSetting().GetDelayTime(job.FileCode_));
      if(interval > 0){
         TJobMapi iter = JobMap_.find(SMarketJob(job.Market_,job.FileCode_));
         if(iter == JobMap_.end()){
            SJobTime jt;
            jt.Time_     = TKTime::Now();
            jt.Interval_ = interval;
            jt.Job_      = job;
            jt.Cmd_      = GetDispatchMgr().GetDLCmd(job, job.DstData_.as_string());
            JobMap_.insert(std::make_pair(SMarketJob(job.Market_,job.FileCode_),jt));
         }else{
            iter->second.Time_ = TKTime::Now();
         }
         return;
      }
   }else{
      Queue_.push(job);  
      PushNotify();
      return;
   }
   Queue_.push(job);  
   PushNotify();
   std::string cmd;
   if((cmd = GetDispatchMgr().GetDLCmd(job, job.DstData_.as_string()))!= std::string()){
   	if(t){
         if(SesVec_.size()){
            JobMap2_.insert(std::make_pair(++SeqNo_,job));
            for(TSesVecI iter = SesVec_.begin(); iter != SesVec_.end(); ++iter){
               if((*iter)->GetEvMode() == TEnvMode(evm_3T) && (*iter)->IsReady()){
                  (*iter)->SendFileCmd(false,SeqNo_,cmd);
                  if(NotifyFirst_)
                     break;
               }
            }
         }
      }
   }
}
//---------------------------------------------------------------------------
K_mf(void) TSenderMgr::OnTimer(Kway::TTimerBase*)
{
   if(JobMap_.size() > 0)
      for(TJobMapi iter = JobMap_.begin(); iter != JobMap_.end(); iter++){
         if((TKTime::Now() - iter->second.Time_) >= iter->second.Interval_){
            Queue_.push(iter->second.Job_);
            PushNotify();
            if(iter->second.Cmd_ != std::string()){
               if(SesVec_.size()){
                 	JobMap2_.insert(std::make_pair(++SeqNo_,iter->second.Job_));
                  for(TSesVecI itr = SesVec_.begin(); itr != SesVec_.end(); ++itr){
                     if((*itr)->GetEvMode() == TEnvMode(evm_3T) && (*itr)->IsReady()){
                        (*itr)->SendFileCmd(false,SeqNo_,iter->second.Cmd_); 
                        if(NotifyFirst_)
                           break;
                     }
                  }
               }
            }
            JobMap_.erase(iter);
            break;
         }
      }
}
//---------------------------------------------------------------------------
K_mf(void) TSenderMgr::DoFileCmd(int seq, std::string& fcmd)
{
   if(SesVec_.size())
      for(TSesVecI iter = SesVec_.begin(); iter != SesVec_.end(); ++iter){
         if((*iter)->GetEvMode() == TEnvMode(evm_3T) && (*iter)->IsReady()){
            (*iter)->SendFileCmd(true, seq, fcmd);
            if(NotifyFirst_)
               break;
         }
      }
}
//---------------------------------------------------------------------------
K_mf(void) TSenderMgr::FileDLEcho(int seq, bool result)
{
   TJobMap2::iterator iter = JobMap2_.find(seq);
   if(iter != JobMap2_.end()){
      TFTJob job = iter->second;
      JobMap2_.erase(iter);
      job.Flow_.assignT(fft_Finish);
      job.Done3T_ = true; // 下載作業三層式檔案複製完成
      Queue_.push(job);  
      PushNotify();
   }
}
//---------------------------------------------------------------------------
