//---------------------------------------------------------------------------
#pragma hdrstop
#include "ScheduleMgr.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
using namespace Kway;
using namespace Kstl;
//---------------------------------------------------------------------------
static TScheduleMgr* ScheduleMgr_;
//---------------------------------------------------------------------------
K_fn(TScheduleMgr*) GetScheduleMgr  ()
{
   return ScheduleMgr_;
}
//---------------------------------------------------------------------------
K_fn(void) SetScheduleMgr  (TScheduleMgr* m)
{
   ScheduleMgr_ = m;
}
//---------------------------------------------------------------------------
K_ctor TScheduleMgr::TScheduleMgr(const char* logname)
{
   Timer_= new TScheduleTimer(this);
   Schedule_ = new TSchedule();
//   ScheduleRecorder_ = new TScheduleRecorder(*Schedule_, ".//Settings//Schedule.rec");
   Timer_->Start(1000*(60-TKTime::Now().GetSec()), &this_type::OnTimer);  // 每分鐘整檢查一次
//   if(logname)
//      LogName_ = std::string(logname);
}
//---------------------------------------------------------------------------
K_dtor TScheduleMgr::~TScheduleMgr()
{
   Timer_->Stop();
   delete Timer_;
   delete Schedule_;
//   delete ScheduleRecorder_;
}
//---------------------------------------------------------------------------
K_mf(void) TScheduleMgr::OnTimer(TTimerBase*)
{
   Timer_->Stop();
   Timer_->Start(1000*60, &this_type::OnTimer);  // 每60秒檢查一次
   Now_ = TKTime::Now();
   for (TSchedulei iter = Schedule_->begin(); iter!=Schedule_->end(); iter++) {
      if (Now_.GetHour() == iter->second.OpenTime_.GetHour() &&
          Now_.GetMin()  == iter->second.OpenTime_.GetMin()) 
         if (iter->second.JobObj_ && iter->second.DoDefault_ == 'Y') {
            iter->second.JobObj_->OnSchedule(true);
            TSchedule::updater u = Schedule_->begin_update(iter);
            u->DoRightNow_ = 'N';
            TKTime T = TKTime::Now();
            u->Memo_.assign(std::string(T.as_string())+std::string(iter->second.JobObj_->Memo_.as_string()));
         }
   }
}
//---------------------------------------------------------------------------
K_mf(TaViewArea) TScheduleMgr::CreateNewVA()
{
   TViewControls   vcs;
   vcs.set(vco_Setting);
   TTimeVAMaker* t = new TTimeVAMaker(vcs, Schedule_, LogName_);
   return TaViewArea(t);
}
//---------------------------------------------------------------------------
K_mf(void) TScheduleMgr::Reg(TScheduleBase* NewObj)
{
   for(TSchedule::iterator L = Schedule_->begin();  L !=Schedule_->end();  L++)
      if(L->second.ItemDesp_ == NewObj->Desp_){
         TSchedule::updater u = Schedule_->begin_update(L);
         u->JobObj_ = NewObj;
         u->Memo_   = NewObj->Memo_;
         return;
      }
   TNKey Sz_;
   char buf[8];
   sprintf(buf,"%02d",Schedule_->size());
   Sz_.assign(buf);
   TimeDef td_;
   td_.DoDefault_ = 'Y';
   td_.DoRightNow_= 'N';
   td_.ItemDesp_  = NewObj->Desp_;
   td_.OpenTime_  = NewObj->Time_;
   td_.JobObj_    = NewObj;
   td_.Memo_      = NewObj->Memo_;
   Schedule_->insert(std::make_pair(Sz_,td_));
   return;
}
//---------------------------------------------------------------------------
K_mf(void) TScheduleMgr::Del(TScheduleBase* Obj)
{
   for(TSchedule::iterator L = Schedule_->begin();  L !=Schedule_->end();  L++)
      if(L->second.JobObj_ == Obj){
         TSchedule::updater u = Schedule_->begin_update(L);
         u->JobObj_ = 0;
         return;
      }
   return;
}
//---------------------------------------------------------------------------
K_mf(void) TScheduleMgr::Erase(TScheduleBase* Obj)
{
   for(TSchedule::iterator L = Schedule_->begin();  L !=Schedule_->end();  L++)
      if(L->second.JobObj_ == Obj){
         Schedule_->erase(L);
         return;
      }
}
//---------------------------------------------------------------------------

