//
// WIN32 實際同步機制的實作的地方
//    class TSyncObj::TSyncHandle; 實作 
//
// 版本: $Id: SyncEv_WIN32.h,v 1.3 2006/04/04 03:48:45 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SyncEv_WIN32H
#define SyncEv_WIN32H
//---------------------------------------------------------------------------
#include "SyncEv.h"
//---------------------------------------------------------------------------
#include <WinSock2.h>
#include <Windows.h>
#include <cc++/thread.h>
/////////////////////////////////////////////////////////////////////////////
namespace Kway {
//---------------------------------------------------------------------------
//底下的程式碼由 BCB-6: Classes.pas 修改而來
#pragma pack(push,1)
struct TMessage {
	UINT        Msg;
	union {
		struct {
			WORD  WParamLo;
			WORD  WParamHi;
			WORD  LParamLo;
			WORD  LParamHi;
			WORD  ResultLo;
			WORD  ResultHi;
		};
		struct {
			int   WParam;
			int   LParam;
			int   Result;
		};
	};
};
//---------------------------------------------------------------------------
typedef void (*TWndMethodFn) (void* thisObj, TMessage& mssage);
struct TWndMethod {
   union {
      TWndMethodFn   PMF_;
      void*          Fn_;
   };
   void*             ThisObj_;
};
//---------------------------------------------------------------------------
template <class T>
   inline TWndMethod  Make_TWndMethod  (T* obj, void (T::*afn) (TMessage&))
                     { TWndMethod   m;
                       m.Fn_      = *(void**)&afn;
                       m.ThisObj_ = obj;
                       return m; }
#pragma pack(pop)
/////////////////////////////////////////////////////////////////////////////
HWND  AllocateHWnd    (TWndMethod);
void  DeallocateHWnd  (HWND);
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSyncHandleObj;
//---------------------------------------------------------------------------
LIBNS_class TSyncThread : public ost::Thread
{
   typedef ost::Thread  inherited;
   DWORD    ThreadID_;

   //傳回true=Handled.
   virtual K_mf(bool) DoMessage(MSG&);

protected:
   void run ();

public:
   TSyncThread(bool createdStart);
   ~TSyncThread();

   K_mf(void) start();
   K_mf(void) ReallocSyncHandle(TSyncHandleObj*);
   K_mf(BOOL) PostMessage(UINT Msg, WPARAM wParam, LPARAM lParam);

   inline K_mf(bool) IsThreadTerminated() const {
	   return ThreadID_ == DWORD(-1);
   }
};
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSyncHandleObj
{
   HWND     HWnd_;
   DWORD    MainThread_;

   void WindowProc (TMessage&);

   virtual K_mf(bool) SyncExec () = 0;

   friend class TSyncThread;
   K_mf(void) ReallocHWnd ();
   K_mf(void) DeallocHWnd ();
public:
   //建構就啟動thread.
   K_ctor  TSyncHandleObj ();
   //解構會自動結束thread.
   K_dtor ~TSyncHandleObj ();

   K_mf(void)  SyncStart     (bool wait);
   K_mf(bool)  InMainThread  ();

   K_mf(void)  ProcessSync   ();//請參考 TSyncObj::ProcessSync() 的說明
};
//---------------------------------------------------------------------------
class TSyncObj::TSyncHandle : public TSyncHandleObj
{
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

