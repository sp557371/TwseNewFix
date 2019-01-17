//
// nslib支援的 [計時器]classes:
//    class TTimerBase;
//    class TSleeper;
//    template TTimer<>;
//
// 版本: $Id: TimerThread.h,v 1.10 2006/04/04 03:47:29 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TimerThreadH
#define TimerThreadH
//---------------------------------------------------------------------------
#include "KwayBase.h"
//---------------------------------------------------------------------------
namespace Kway {
//---------------------------------------------------------------------------
LIBNS_class TNSIniter;
//---------------------------------------------------------------------------
#ifdef _MSC_VER
//取消 Timer_(this) 的警告.
#pragma warning (disable : 4355)
#endif
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TTimerBase
{
public:
   enum TOnTimerStyle {
      ots_MainThread,
      ots_DontCare,
      ots_TimerThread,
      ots_Destroyed,
   };
           K_ctor  TTimerBase  (TOnTimerStyle);
   virtual K_dtor ~TTimerBase  ();

   /// 從 stop 狀態進入 start 狀態, 傳回 true,
   /// 若已是 start 狀態, 重覆呼叫(即使ms不同), 則傳回 false;
   K_mf(bool)   Start       (dword ms);

   /// 有啟動過,傳回true, 沒啟動過,傳回false;
   K_mf(bool)   Stop        ();

   inline K_mf(dword)  GetInterval () const  { return Interval_;  }
   inline K_mf(uint64) GetNextClock() const  { return NextClock_; }

   static K_mf(void)    InitThread  ();
   static K_mf(uint64)  GetClockMS  ();
private:
   class Thread;
   friend class Thread;

   friend class TNSIniter;
   static K_mf(void)  StopThread  ();

   enum TFlag {
      f_Add,
      f_Del,
      f_Init,
      f_Stop,
   };
   static K_mf(bool)  TimerThread  (TTimerBase*, TFlag);

   uint64         NextClock_;
   dword          Interval_;
   TOnTimerStyle  SyncStyle_;
   virtual K_mf(void)  OnTimer  () = 0;
};
//---------------------------------------------------------------------------
LIBNS_class TSleeper : protected TTimerBase
{
   typedef TTimerBase   inherited;
   bool  Sleeping_;
   virtual K_mf(void)  OnTimer  ();
public:
   K_ctor      TSleeper  ();
   K_dtor     ~TSleeper  ();
   K_mf(void)  Sleep     (dword ms);
   K_mf(void)  WakeUp    ();
};
//---------------------------------------------------------------------------
template <class T>
class TTimer : public TTimerBase
{
   typedef TTimerBase   inherited;
   typedef void (K_FASTCALL T::*TOnTimer) (TTimerBase*);
   T*          Obj_;
   TOnTimer    OnTimer_;

   K_mf(void)  OnTimer  ()
      {
         if(Obj_  &&  OnTimer_)   (Obj_->*OnTimer_)(this);
      }

public:
   K_ctor  TTimer  (T* obj, TOnTimerStyle ots = ots_MainThread)
      : inherited(ots),
        Obj_(obj),
        OnTimer_(0)
      { }

   K_mf(bool)  Start  (dword ms, TOnTimer onTimer) {
         OnTimer_ = onTimer;
         return inherited::Start(ms);
      }
};
/////////////////////////////////////////////////////////////////////////////
struct TTimerFreeChecker_AlwaysTrue {
   template <class T>
      inline K_mf(bool) operator()  (const T*) const  { return true; }
};
//---------------------------------------------------------------------------
struct TThreadFinishChecker {
   template <class Thr>
      inline K_mf(bool)  operator()  (Thr* thr)   { return !thr->isRunning(); }
};
//---------------------------------------------------------------------------
template <class T, class Checker = TTimerFreeChecker_AlwaysTrue>
class TTimerFreeObj : private TTimerBase {
   T*       obj_;
   Checker  Checker_;
   K_mf(void) OnTimer ()
      {
         if(Checker_(obj_))
            delete this;
      }
public:
   K_ctor  TTimerFreeObj  (T* obj, TOnTimerStyle ots, dword ms = 1,
                           const Checker& chk = Checker())
            : TTimerBase(ots),
              obj_(obj),
              Checker_(chk)
            { Start(ms); }
   K_dtor ~TTimerFreeObj  ()
            { delete obj_; }
};
//---------------------------------------------------------------------------
template<class T>
TTimerFreeObj<T>*  DelayFreeObj  (T* obj, TTimerBase::TOnTimerStyle ots = TTimerBase::ots_DontCare, dword ms = 1)
{
   return new TTimerFreeObj<T>(obj,ots,ms);
}
//---------------------------------------------------------------------------
template<class T, class Checker>
TTimerFreeObj<T,Checker>*  DelayFreeObjChecker  (T* obj, const Checker& chk = Checker(),
                                                 TTimerBase::TOnTimerStyle ots = TTimerBase::ots_MainThread,
                                                 dword ms = 1)
{
   return new TTimerFreeObj<T,Checker>(obj,ots,ms,chk);
}
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
