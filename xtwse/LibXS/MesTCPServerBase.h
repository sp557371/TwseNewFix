//
// �q�Tclasses��¦: Session-Messager
//    class TMesTCPServer;
//    class TMesTCPServerOnce;
//    �@�Ϊ� classes
//
// ����: $Id: MesTCPServerBase.h,v 1.14 2006/04/04 03:03:02 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef __MesTCPServerBaseH__
#define __MesTCPServerBaseH__
/////////////////////////////////////////////////////////////////////////////
#include "Mes.h"
#include "TimerThread.h"
#include "SocketPortBase.h"
/////////////////////////////////////////////////////////////////////////////
//MesTCPServer.cpp �P MesTCPServerOnce.cpp ���\�h�@�P���a��, �N��b�o��F
namespace Kway {
//---------------------------------------------------------------------------
#ifdef  CCXX_NAMESPACES
using namespace std;
using namespace ost;
#endif
//---------------------------------------------------------------------------
LIBNS_class TMesTCPServer;
class       TMesSvrClient;
/////////////////////////////////////////////////////////////////////////////
class MesTCPServerImpl : /*protected TCPSocket, */protected Thread
{
public:
   K_ctor  MesTCPServerImpl  (TMesTCPServer&);

   K_mf(void)  Free         ();
   K_mf(void)  StartServer  ();

   inline K_mf(TSocketService*)  GetClientService  ()  { return ClientService_; }
   inline K_mf(TMesTCPServer*)   GetMesServer      ()  { return MesServer_;     }
          K_mf(void)             UpdateMesState    (int adj);

protected:
   virtual ~MesTCPServerImpl  ();//�����ϥ� Free(), ���i���� delete
   friend class TTimerFreeObj<MesTCPServerImpl>;
   static void AfClientServiceDelete (void*);
   inline K_mf(Thread*)  GetThread  ()  { return this; }

   void  run          ();
   void  checkAccept  ();

   friend class TMesTCPServer;
   TMesTCPServer*    MesServer_;
   TSocketService*   ClientService_;
   int               ClientServiceCount_;
   class TServerSocket;
   TServerSocket*    ServerSocket_;
   bool              QuitServer_;
};
/////////////////////////////////////////////////////////////////////////////
// When a client connected, this class will be called.
class MesSvrClientImpl : protected TSocketPortBase
{
   typedef TSocketPortBase  inherited;
   friend class TMesSvrClient;
public:
            MesSvrClientImpl (MesTCPServerImpl&, SOCKET);
   virtual ~MesSvrClientImpl ();

   inline K_mf(TMesSvrClient*)  GetClientMes  ()  { return ClientMes_; }
   inline K_mf(std::string)     GetUID        ()  { return UID_;       }

   inline K_mf(int)  receive  (void* buffer, size_t size)
                                                  { Recved_ = true;
                                                    return inherited::receive(buffer, size); }
 /*fonwin:921109:begin{ �N���B�z����Thread����
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
  TMesSvrClient*     ClientMes_;
  bool               ClientMesDestroyed_;
  MesTCPServerImpl&  ServerImpl_;
  std::string        UID_;
  bool               Recved_;
};
/////////////////////////////////////////////////////////////////////////////
class TMesSvrClient : public TMesNoThread
{
   typedef TMesNoThread    inherited;
   MesSvrClientImpl* Impl_;
   friend class MesSvrClientImpl;
   K_mf(bool)  FreeImpl  ();
   K_mf(bool)  OpenImpl ();
   K_mf(bool)  CloseImpl ();
public:
   K_ctor  TMesSvrClient  (MesSvrClientImpl&);
   K_dtor ~TMesSvrClient  ();

   K_mf(std::string)  GetUID  ();
   K_mf(int)          Send    (const void* buffer, size_t size);
   K_mf(int)          Recv    (      void* buffer, size_t size);

   //friend class MesSvrClientImpl;
   //friend class MesTCPServerImpl;
   using inherited::OnRecvReady;
   using inherited::OnSendEmpty;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
/////////////////////////////////////////////////////////////////////////////
#endif
