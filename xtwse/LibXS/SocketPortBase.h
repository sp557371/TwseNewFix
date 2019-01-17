//
// �ϥ� ost::SocketPort, �P Kway::TMemQueue �� socket
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
{  //���Y ost::SocketService ���b run(), �Y�ϩI�s update(0)
   //  ��L�k�T�w ost::SocketService �O�_�w����,
   //  �ҥH�ǥ� Timer, �w���ˬd getCancel(),
   //  �� getCancel()==cancelImmediate, �h��ܥi�H�w���� delete �F
   //���ҥH�z���i�H���� delete TSocketService,
   //  �����Өϥ� Free() �ӵ��� SocketService
   struct FreeChecker;
   friend struct FreeChecker;
public:
   K_ctor  TSocketService (bool waitClients);//waitClients=��������clients�Ѻc��,�~�ѺcSocketService
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
   K_mf(void) OnMemQueueDataIn ();  //�Ұ� setDetectOutput(true)

#ifdef K_UNIXLIKE
   //UNIX: (Win32�w�令 WSAAsyncSelect() �B�ƥ�|�b main thread ����,�ҥH���|�����U������)
   // �]�� cc++ �b�B�z pending(),output(),disconnect() ��, �| lock mutex,
   //    �Y�b���ɰ��� SyncObj, �B�b SyncObj �����I�s attach() �� detach() (�]�ݭn lock mutex)
   //    ��... �N�y�������F!!
   // �ҥH: �令�b TimerThread �����B�z
   // 930116:�令�����ϥ� class TCheckSync: public TSyncObj;
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

   //��override ost::SocketPort,
   //  �q TSocketPortBase �l�ͪ��H, �����мg�� mf
   //  ���ϥ�: CheckOutput(),CheckPending(),CheckDisconnect()
	void output     ();
   void pending    ();
	void disconnect ();

   K_mf(bool)  checkSend   ();//���X MemQueue ����ƨöǰe, �Y�L��ƫh���� setDetectOutput(false)

protected:
   typedef Kstl::auto_ref<Kstl::ref_counter_ost<> >    TLocker;

           K_mf(bool)  CheckError      ();//if(�o��error) { disconnect();  return true; } else return false;
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
