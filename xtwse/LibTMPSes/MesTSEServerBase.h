//
// 通訊classes基礎: Session-Messager
//    class TMesTCPServer;
//    class TMesTCPServerOnce;
//    共用的 classes
//
// 版本: $Id: MesTCPServerBase.h,v 1.12 2004/07/25 00:27:46 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef __MesTSEServerBaseH__
#define __MesTSEServerBaseH__
/////////////////////////////////////////////////////////////////////////////
#include "Mes.h"
#include "TimerThread.h"
#include "SocketPortBase.h"
#include "MesTSEClient.h"
#include "MesTSEServer.h"
/////////////////////////////////////////////////////////////////////////////
//MesTCPServer.cpp 與 MesTCPServerOnce.cpp 有許多共同的地方, 就放在這兒了
namespace Kway {
//---------------------------------------------------------------------------
#ifdef  CCXX_NAMESPACES
using namespace std;
using namespace ost;
#endif
//---------------------------------------------------------------------------
K_class TMesTSEServer;
/////////////////////////////////////////////////////////////////////////////
class MesTSEServerImpl : /*protected TCPSocket, */protected Thread
{
public:
   K_ctor  MesTSEServerImpl  (TMesTSEServer&);

   K_mf(void)  Free         ();
   K_mf(void)  StartServer  ();

   inline K_mf(TSocketService*)  GetClientService  ()  { return ClientService_; }
   inline K_mf(TMesTSEServer*)   GetMesServer      ()  { return MesServer_;     }
          K_mf(void)             UpdateMesState    (int adj);

protected:
   virtual ~MesTSEServerImpl  ();//必須使用 Free(), 不可直接 delete
   friend class TTimerFreeObj<MesTSEServerImpl>;
   static void AfClientServiceDelete (void*);
   inline K_mf(Thread*)  GetThread  ()  { return this; }

   void  run          ();
   void  checkAccept  ();

   friend class TMesTSEServer;
   TMesTSEServer*    MesServer_;
   TSocketService*   ClientService_;
   int               ClientServiceCount_;
   class TServerSocket;
   TServerSocket*    ServerSocket_;
   bool              QuitServer_;
};
/////////////////////////////////////////////////////////////////////////////
// When a client connected, this class will be called.
class MesTSESvrClientImpl : protected TSocketPortBase
{
   typedef TSocketPortBase  inherited;
   friend class TMesTSESvrClient;
public:
            MesTSESvrClientImpl (MesTSEServerImpl&, SOCKET);
   virtual ~MesTSESvrClientImpl ();

   inline K_mf(TMesTSESvrClient*)  GetClientMes  ()  { return ClientMes_; }
   inline K_mf(std::string)     GetUID        ()  { return UID_;       }

   inline K_mf(int)  receive  (void* buffer, size_t size, int flags = 0)
                                                  { Recved_ = true;
                                                    return inherited::receive(buffer, flags); }
//                                                    return inherited::receive(buffer, size, flags); }
 /*fonwin:921109:begin{ 將此處理移到Thread之中
   inline void            WaitLinkReady (SocketService* ClientService)
                                            { SetLinkReady();
                                              attach(ClientService); }
                                         // { return setDetectOutput(true); }//wait output ready for SetLinkReady()
 */
          K_mf(void)  attach  (TSocketService*);
 //fonwin:921109:end}

protected:
   K_mf(void)  CheckDisconnect  ();
   K_mf(void)  CheckPending     ();  // Data is ready for reading
   K_mf(void)  CheckOutput      ();  // connection is ready for output(SetLinkReady())
private:
   K_mf(void)  OnMemQueueEmpty  ();

  tpport_t           port_;
  InetHostAddress    ia_;
  TMesTSESvrClient*  ClientMes_;
  bool               ClientMesDestroyed_;
  MesTSEServerImpl&  ServerImpl_;
  std::string        UID_;
  bool               Recved_;
};
/////////////////////////////////////////////////////////////////////////////
class TMesTSESvrClient : public TMesNoThread
{
   typedef TMesNoThread    inherited;
   typedef TMesTSESvrClient   this_type;
   typedef TTimer<this_type> TMesTimer;
   friend class MesTSESvrClientImpl;
   friend class MesTSEServerImpl;
   MesTSESvrClientImpl* Impl_;
   TMesTimer         Timer_;
   THeartBeat        HB_;
   char              RingQ_[4096];
   int               Pos_;
   TTseHead*         Pkt_;
   K_mf(bool)  FreeImpl  ();
   K_mf(void)  OnTimer   (TTimerBase*);
   K_mf(bool)  OpenImpl ();
   K_mf(bool)  CloseImpl ();
public:
   K_ctor  TMesTSESvrClient  (MesTSESvrClientImpl&);
   K_dtor ~TMesTSESvrClient  ();

   K_mf(std::string)  GetUID  ();
   K_mf(int)          Send    (const void* buffer, size_t size);
   K_mf(int)          Recv    (      void* buffer, size_t size);
   K_mf(bool)         Close   ();
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
/////////////////////////////////////////////////////////////////////////////
#endif
