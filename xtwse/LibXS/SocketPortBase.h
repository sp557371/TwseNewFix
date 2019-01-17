//
// 使用 ost::SocketPort, 與 Kway::TMemQueue 的 socket
//
// $Id: SocketPortBase.h,v 1.8 2006/04/04 03:52:56 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SocketPortBaseH
#define SocketPortBaseH
//---------------------------------------------------------------------------
#ifdef  NS_USE_fdReactor
   #include "fdReactor.h"
   typedef Kway::TfdReactor   SocketService_Base;
   typedef Kway::TfdHandler   SocketPort_Base;
#else
   #ifdef _MSC_VER
   #pragma warning (disable : 4267)
   #endif
   #include <cc++/socket.h>
   typedef ost::SocketService    SocketService_Base;
   typedef ost::SocketPort       SocketPort_Base;
#endif
#include "MemPool.h"
#include "SyncEv.h"
#include "ref_counter.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
class TSocketService : public SocketService_Base
{  //※若 ost::SocketService 正在 run(), 即使呼叫 update(0)
   //  亦無法確定 ost::SocketService 是否已結束,
   //  所以藉由 Timer, 定時檢查 getCancel(),
   //  當 getCancel()==cancelImmediate, 則表示可以安全的 delete 了
   //※所以您不可以直接 delete TSocketService,
   //  而應該使用 Free() 來結束 SocketService
   struct FreeChecker;
   friend struct FreeChecker;
public:
   K_ctor  TSocketService (bool waitClients);//waitClients=等全部的clients解構後,才解構SocketService
          ~TSocketService ();

   typedef void (*TfnAfDelete) (void*);
   K_mf(void)  Free  (TfnAfDelete, void* obj);

   K_mf(void) start     ();
   K_mf(bool) IsRunning () const { return IsRunning_==1; }
protected:
	virtual void initial ();
	virtual void final   ();

private:
   struct TMfPack {
      TfnAfDelete    Fn_;
      void*          Obj_;
   };
   TMfPack  AfDelete_;
   bool     WaitClients_;
   int      IsRunning_;
};
/////////////////////////////////////////////////////////////////////////////
class TSocketPortBase : protected SocketPort_Base,
                        public    Kstl::ref_counter_ost<>,
                        protected TMemQueue
{
   K_mf(void) OnMemQueueDataIn ();  //啟動 setDetectOutput(true)

#ifdef K_UNIXLIKE
   //UNIX: (Win32已改成 WSAAsyncSelect() 且事件會在 main thread 產生,所以不會有底下的情形)
   // 因為 cc++ 在處理 pending(),output(),disconnect() 時, 會 lock mutex,
   //    若在此時執行 SyncObj, 且在 SyncObj 之中呼叫 attach() 或 detach() (也需要 lock mutex)
   //    那... 就造成死結了!!
   // 所以: 改成在 TimerThread 之中處理
   // 930116:改成直接使用 class TCheckSync: public TSyncObj;
   class TCheckSync : protected TSyncObj
   {
      typedef K_mf(void)  (TSocketPortBase::*TNotify)  ();
      TSocketPortBase*  Port_;
      TNotify           Notify_;
      K_mf(void) Sync ();
   public:
      inline K_ctor  TCheckSync  ()  : Port_(0), Notify_(0)  {}
      K_mf(void)     StartSync   (TSocketPortBase*, TNotify);
   };
   class TCheckSync;
   friend class TCheckSync;
   enum TWaitCheck {
         wc_Output,
         wc_Pending,
         wc_Disconnect,
         wc_Count,
   };
   ost::Mutex  Mutex_;
   TCheckSync  SyncObjs_[wc_Count];
   K_mf(void)  SyncPending  ();
#endif

   //↓override ost::SocketPort,
   //  從 TSocketPortBase 衍生的人, 不應覆寫此 mf
   //  應使用: CheckOutput(),CheckPending(),CheckDisconnect()
	void output     ();
   void pending    ();
	void disconnect ();

   K_mf(bool)  checkSend   ();//取出 MemQueue 的資料並傳送, 若無資料則關閉 setDetectOutput(false)

protected:
   typedef Kstl::auto_ref<Kstl::ref_counter_ost<> >    TLocker;

           K_mf(bool)  CheckError      ();//if(發生error) { disconnect();  return true; } else return false;
   virtual K_mf(void)  CheckOutput     ();//if(!checkSend()) CheckError()
   virtual K_mf(void)  CheckPending    () = 0;
   virtual K_mf(void)  CheckDisconnect () = 0;

public:
   K_ctor  TSocketPortBase  (int domain, int type, int protocol, size_t maxMergeSize);
// K_ctor  TSocketPortBase  (ost::SocketService *svc, ost::TCPSocket &tcp, size_t maxMergeSize);
   K_ctor  TSocketPortBase  (SOCKET, size_t maxMergeSize);
#ifdef K_UNIXLIKE
   K_dtor ~TSocketPortBase  ();
#endif

   K_mf(int)  send     (const void* buffer, size_t size);
   K_mf(int)  receive  (      void* buffer, size_t size);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
