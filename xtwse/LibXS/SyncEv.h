//
// 同一支程式的不同 thread, 全部回到 main thread 來同步
//    您必須在程式初始化時, 呼叫 TSyncObj::Initial();
//    若您有 busy loop, 請呼叫   TSyncObj::CheckSync();
//
// 版本: $Id: SyncEv.h,v 1.8 2006/04/04 03:51:18 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SyncEvH
#define SyncEvH
//---------------------------------------------------------------------------
#include "nsinit.h"
//---------------------------------------------------------------------------
//#include <cc++/thread.h>
namespace ost {
   class Thread;
}
//---------------------------------------------------------------------------
//在主執行緒執行一個事件!
//---------------------------------------------------------------------------
namespace Kway {
LIBNS_class TInpHandler;
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSyncObj
{
   class TSyncHandle;
   class TSyncWorker;
   friend class TSyncWorker;
   friend class TNSIniter;
   static K_mf(TSyncWorker&) GetSyncWorker  ();
   static K_mf(void)         InitSyncWorker ();
   #ifdef K_UNIXLIKE
      static K_mf(int)  GetSyncFD ();
   #endif

public:
   enum TState
   {
      ssWaitting,
      ssExecing,
      ssExecuted,
      ssAbort,
      ssIdle,
      ssDestroying,
   };

private:
   TState   State_;

public:
   inline  K_ctor  TSyncObj ()  : State_(ssIdle) {}
   virtual K_dtor ~TSyncObj ();

   //避免繼承者已死,但正要執行 Sync(), 若 Sync()=0, 會造成 pure virtual function.
   //所以此處必須要有一個不做任何事情的 Sync();
   virtual K_mf(void)  Sync  ();

   inline K_mf(TState)  GetState  () const  { return State_; }

   //回到主執行緒執行 operator(),
   //  執行完後才返回: true
   //  無法回到主執行緒執行: false
   K_mf(bool)  operator()  ();
   K_mf(bool)  WaitSync    (bool wait);

   // 93/08/01之後,請使用 nsinit.h 的 libns [啟動/結束]機制.
   // static K_mf(void)  Initial  ();   //請務必在您的主程式啟動時呼叫此 function

   static K_mf(void)  CheckSync  ();   //請在您的 busy loop 之中呼叫此 function (可在任意 Thread 中呼叫)

   //● 請在 AppRun() 之前, 先呼叫 Initial()
   //若您的 unix(Windows Console) 程式, 從 main() 開始, 而且沒有別的事情要做了,
   //可執行 AppRun() 來將程式變成 demon (TInpHandler*=0, 就會變成daemon了), 並不斷的處理 Sync 事件,
   static K_mf(int)   AppRun      (int (*MainInit) (), int argc, char** argv, TInpHandler*);
   //若您要自己控制程式的流程,請在 MainThread中, 適當的地方呼叫 ProcessSync()
   static K_mf(void)  ProcessSync ();
   //結束 AppRun(), AppQuit()仍會返回, 等該做的事情完成後, 程式會從AppRun()返回!
   static K_mf(void)  AppQuit ();
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

