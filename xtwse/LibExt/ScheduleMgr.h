//---------------------------------------------------------------------------
#ifndef ScheduleMgrH
#define ScheduleMgrH
//---------------------------------------------------------------------------
#include "KTime.h"
#include "TimerThread.h"
#include "ViewAreaMgr.h"
#include "StdRecorder.h"
#include "TableLogger.h"
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kstl;
//---------------------------------------------------------------------------
typedef char_ary<char,40,' '> TKMemo;
//---------------------------------------------------------------------------
K_class TScheduleBase
{
protected:
   K_mf(void) Reg();
   K_mf(void) Del();
public:
   char_ary<char,40,' '> Desp_;
   TKTime                Time_;
   TKMemo                Memo_;
   virtual K_mf(void) OnSchedule(bool) {};
};
//---------------------------------------------------------------------------
/**/template <class T>
/**/K_class TScheduleObj :public TScheduleBase
/**/{
/**/   typedef K_mf(void) (T::*TOnSchedule) (TKMemo&);
/**/private:
/**/   T*                Obj_;
/**/   TOnSchedule       OnSchedule_;
/**/public:
/**/   K_ctor TScheduleObj   (T* Obj, const char* desp, const char* time, TOnSchedule t = 0)
/**/   {
/**/      Obj_ = Obj;
/**/      Desp_.assign(desp);
/**/      Time_.assign(time);
/**/      OnSchedule_ = t;
/**/   }
/**/   K_dtor ~TScheduleObj(){/*Del();*/}
/**/   K_mf(void) OnSchedule(bool)
/**/   {
/**/      if(Obj_ && OnSchedule_)
/**/         (Obj_->*OnSchedule_)(Memo_);
/**/      else
/**/         Memo_.assign("沒有指定的工作!");
/**/   }
/**/};
//---------------------------------------------------------------------------
struct TimeDef
{
   char_ary<char,40,' '>    ItemDesp_;
   char                     DoDefault_;
   TKTime                   OpenTime_;
   char                     DoRightNow_;
   TKMemo                   Memo_;
   TScheduleBase*           JobObj_;
};
//---------------------------------------------------------------------------
class TSaveDef
{
public:
   char_ary<char,40,' '>    ItemDesp_;
   char                     DoDefault_;
   TKTime                   OpenTime_;
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
//---------------------------------------------------------------------------
typedef char_ary<char,3,' '>              TNKey;
typedef Kstl::ev_map<TNKey, TimeDef>      TSchedule;
typedef TSchedule::iterator               TSchedulei;
//typedef TStdRecorder<TSchedule, TimeDef>  TScheduleRecorder;
//---------------------------------------------------------------------------
struct TScheduleFieldMaker
{
   typedef TSchedule                TEvContainer;
   typedef TEvContainer::iterator   iterator;
   typedef size_t                   field_n_type;

   K_mf(const Kway::TFieldDesp&)  GetFieldDesp  (field_n_type n) const
          {
            static Kway::TFieldDesp f[] = {
                     { "項次",         4, Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter,  Kway::tfs_Null,    },
                     { "預設執行",     8, Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter,  Kway::tfs_CanEdit, } ,
                     { "處理項目名稱",40, Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  ,  Kway::tfs_Null, },
                     { "執行時間",    10, Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter,  Kway::tfs_CanEdit, },
                     { "立即執行",     8, Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter,  Kway::tfs_CanEdit, } ,
                     { "備註",        30, Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  ,  Kway::tfs_Null, } };
            return f[n];
          }
   inline K_mf(field_n_type)  GetFieldCount () const    { return 6; }

   K_mf(std::string)  Get  (const iterator& i, field_n_type fieldNo) const
         {
            switch(fieldNo) {
            case 0:  return i->first.as_string();
            case 1:  return (i->second.DoDefault_== 'Y'? "Y":"N");
            case 2:  return i->second.ItemDesp_.as_string();
            case 3:  return i->second.OpenTime_.as_string();
            case 4:  return (i->second.DoRightNow_== 'Y'? "Y":"N");
            case 5:  return i->second.Memo_.as_string();
            }
            return std::string();
         }
   K_mf(void)  Put  (TEvContainer& c, iterator& iter, field_n_type fieldNo, const std::string& str)
         {
            if(fieldNo == 0)
               return;
            TEvContainer::updater   u = c.begin_update(iter);
            switch(fieldNo) {
            case 1:  u->DoDefault_ = ((str == "Y" || str == "y")?'Y':'N');  break;
            case 3:  u->OpenTime_.assign(str);  break;
            case 4:  u->DoRightNow_ = ((str == "Y" || str == "y")?'Y':'N');  break;
            default:break;
            }
         }
   K_mf(void)  swap_mapped  (TNKey& a, TNKey& b)  { std::swap(a,b); } 

};
//---------------------------------------------------------------------------
K_class TTimeVAMaker : public TTViewArea<TSchedule,TScheduleFieldMaker>
{
   typedef TTViewArea<TSchedule,TScheduleFieldMaker> inherited;
private:
   TSchedule*          Origin_;
   TSchedule*          Shadow_;
   TScheduleFieldMaker f_;
   std::string         LogName_;
   K_mf(bool) SettingApply()
   {
      TTableLogger<TScheduleFieldMaker>*      Log_;
/*
      if(!LogName_.empty())
         Log_ = new TTableLogger<TScheduleFieldMaker>(LogName_.c_str());
*/
      TSchedule::iterator oiter = Origin_->begin();
      for (TSchedule::iterator iter = Shadow_->begin(); iter != Shadow_->end(); iter++){
/*
         char item[256];
         sprintf(item,"%d.%s",iter->first.as_int(),iter->second.ItemDesp_.as_string().c_str());
      	 if(Log_ && iter->second.DoDefault_ != oiter->second.DoDefault_)
      	    Log_->WriteMod(std::string(item), 1, oiter->second.DoDefault_=='Y'?"Y":"N",
      	                                                iter->second.DoDefault_=='Y'?"Y":"N", this);
      	 if(Log_ && !(iter->second.OpenTime_ == oiter->second.OpenTime_))
      	    Log_->WriteMod(std::string(item), 3, oiter->second.OpenTime_.as_string(),
      	                                                iter->second.OpenTime_.as_string(), this);
*/
         if (iter->second.DoRightNow_ == 'Y') {
            TSchedule::updater u = Shadow_->begin_update(iter);
            if(iter->second.JobObj_)
               iter->second.JobObj_->OnSchedule(false);
            u->DoRightNow_ = 'N';
            TKTime T = TKTime::Now();
            u->Memo_.assign(std::string(T.as_string())+std::string(iter->second.JobObj_->Memo_.as_string()));
         }
         oiter++;
      }
//      delete Log_;
      *Origin_ = *Shadow_ ;
      return true;
   }
   K_mf(bool) SettingCancel(){*Shadow_  = *Origin_; return true;}

public:
   K_ctor     TTimeVAMaker(TViewControls vcs, TSchedule* c, std::string& logname)
      : inherited(0,f_,vcs)
   {
      Origin_  = c;
      Shadow_  = new TSchedule();
      *Shadow_ = *c;
      LogName_ = logname;
      inherited::SetContainer(Shadow_);
   }
   K_dtor ~TTimeVAMaker(){delete Shadow_;}
};
//---------------------------------------------------------------------------
K_class TScheduleMgr :public TVACreater
{
typedef TScheduleMgr  this_type;
typedef TVACreater    inherited;
typedef Kway::TTimer<this_type> TScheduleTimer;
private:
   
   TSchedule*         Schedule_;
//   TScheduleRecorder* ScheduleRecorder_;
   TKTime             Now_;
   TScheduleTimer*    Timer_;
   std::string        LogName_;

   K_mf(void)        OnTimer      (TTimerBase*);
public:
   K_ctor            TScheduleMgr (const char* logname = 0);
   K_dtor            ~TScheduleMgr();
   K_mf(void)        Reg          (TScheduleBase* NewObj);
   K_mf(void)        Del          (TScheduleBase* Obj);
   K_mf(void)        Erase        (TScheduleBase* Obj);
   K_mf(TaViewArea)  CreateNewVA  ();
};
//---------------------------------------------------------------------------
K_fn(TScheduleMgr*)          GetScheduleMgr  ();
K_fn(void)                   SetScheduleMgr  (TScheduleMgr*);
//---------------------------------------------------------------------------
#endif
