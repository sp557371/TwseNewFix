//
// �q�Tclasses��¦: Session-Messager
//    class TMesTCPServer;
//    class TMesTCPServerOnce;
//    �@�Ϊ� classes
//
// ����: $Id: MesTCPServerBase.cpp,v 1.13 2004/07/25 00:27:46 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "MesTSEServerBase.h"
#include "MesTSEServer.h"
#include "SyncEv.h"
/////////////////////////////////////////////////////////////////////////////
using namespace Kway;
/////////////////////////////////////////////////////////////////////////////
class MesTSEServerImpl::TServerSocket : public TCPSocket
{
   typedef TCPSocket    inherited;
public:
   K_ctor  TServerSocket  (TMesTSEServer& mesServer)
      : inherited(InetHostAddress() = htonl(INADDR_ANY),                // service address
                  static_cast<tpport_t>(mesServer.GetListeningPort()))  // listen port
      { }
   using inherited::endSocket;
   using inherited::isPendingConnection;
   using inherited::so;
};
/////////////////////////////////////////////////////////////////////////////
K_ctor  MesTSEServerImpl::MesTSEServerImpl  (TMesTSEServer& mesServer)
   : Thread()
   , MesServer_(&mesServer)
   , ClientService_(0)
   , ClientServiceCount_(0)
   , ServerSocket_(0)
   , QuitServer_(false)
{
}
//---------------------------------------------------------------------------
MesTSEServerImpl::~MesTSEServerImpl  ()
{
   delete ServerSocket_;
}
//---------------------------------------------------------------------------
void MesTSEServerImpl::AfClientServiceDelete (void* obj)
{
   MesTSEServerImpl* impl = static_cast<MesTSEServerImpl*>(obj);
   if(--(impl->ClientServiceCount_) == 0)
      DelayFreeObjChecker(impl->GetThread(), TThreadFinishChecker());
}
//---------------------------------------------------------------------------
K_mf(void)  MesTSEServerImpl::StartServer  ()
{
   ++ClientServiceCount_;
   if(ClientService_)
      ClientService_->Free(AfClientServiceDelete, this);
   ClientService_ = new TSocketService(false);
   ClientService_->start();
   start(); // �}�l���� thread
}
//---------------------------------------------------------------------------
K_mf(void) MesTSEServerImpl::Free ()
{
   QuitServer_ = true;
   if(ServerSocket_)
      ServerSocket_->endSocket(); //�����b accept()/select()(::run() isPendingConnection())���a��i�H���_��!
   if(ClientService_) {
      ClientService_->Free(AfClientServiceDelete, this);
      ClientService_ = 0;
   } else
      DelayFreeObjChecker(GetThread(), TThreadFinishChecker());
}
//---------------------------------------------------------------------------
K_mf(void)  MesTSEServerImpl::UpdateMesState  (int adj)
{
   if(ClientService_==0  ||  MesServer_==0)
      return;
   char  msgbuf[64];
   sprintf(msgbuf, "Listing...(%d)", ClientService_->getCount()+adj);
   MesServer_->OnStateChanged(mslNormal, msgbuf);
}
//---------------------------------------------------------------------------
void  MesTSEServerImpl::checkAccept  ()
{
   class SyncAccept : public TSyncObj
   {  typedef TSyncObj  inherited;
   protected:
      typedef K_mf(bool) (TMesTSEServer::*TAcceptFn)(MesTSESvrClientImpl*&, SOCKET);

      TMesTSEServer*& MesServer_;
      SOCKET          accept_so_;
      TAcceptFn       AcceptFn_;
      bool            ResultSuccess_;
      K_mf(void)  Sync  ()    { if(MesServer_) {
                                   SOCKET s = accept(accept_so_,0,0);
                                   if(s==INVALID_SOCKET)
                                      ResultSuccess_ = false;
                                   else {
                                      ResultSuccess_ = (MesServer_->*AcceptFn_)(MesClientImpl_,s);
                                      if(!ResultSuccess_) {
                                         ::shutdown(s, 2);
                                         #ifdef WIN32
                                            ::closesocket(s);
                                         #else
                                            ::close(s);
                                         #endif
                                      }
                                   }
                                } else ResultSuccess_ = false; }
   public:
      inline K_ctor  SyncAccept  (TMesTSEServer*& MesServer, TAcceptFn acceptfn, SOCKET s)
         : MesServer_(MesServer),
           accept_so_(s),
           AcceptFn_(acceptfn),
           ResultSuccess_(false),
           MesClientImpl_(0)
         { }
      inline K_mf(bool)  operator()  ()
         { if(!inherited::operator()())
              return false;
           return ResultSuccess_; }
      MesTSESvrClientImpl* MesClientImpl_;
   };
   if(ServerSocket_ == 0)
      return;
   SyncAccept  acceptTest(MesServer_, &TMesTSEServer::AcceptClient, ServerSocket_->so);
   if(MesServer_  &&  acceptTest())
      if(acceptTest.MesClientImpl_)
         acceptTest.MesClientImpl_->attach(GetClientService());
}
//---------------------------------------------------------------------------
void  MesTSEServerImpl::run  ()
{
    while(!QuitServer_) {
      try {
         ServerSocket_ = new TServerSocket(*MesServer_);
         break;
      } catch(Socket*) {
         //delete s;
         MesServer_->OnStateChanged(mslFatal, "Cannot create listen port!");
      }
      int n = 50; // 50*100 = 5sec
      while(!QuitServer_  &&  --n > 0)
         sleep(100);
    }
    while(!QuitServer_) {
      UpdateMesState(0);
    #if defined(__linux)  ||  defined(__alpha)
      //�]���b so �Q��������, ���M�L�k��^, �ҥH�C�j�@�p�q�ɶ��N�����ˬd�O�_����.
      while(!ServerSocket_->isPendingConnection(50))
         if(QuitServer_)
            break;
      if(!QuitServer_)
         checkAccept();
    #else
      if(ServerSocket_->isPendingConnection())
         if(!QuitServer_)
            checkAccept();
    #endif
    }
}
/////////////////////////////////////////////////////////////////////////////
// When a client connectted, call this.
MesTSESvrClientImpl::MesTSESvrClientImpl (MesTSEServerImpl& ServerImpl, SOCKET s)
   ://�p���غc���K���F����exception! (accept()�����i�ॢ��)
   //TSocketPortBase(0/*Service.GetClientService()*/, *ServerImpl.GetTCPSocket(), 4096),
     TSocketPortBase(s, 4096),
     ia_(getPeer(&port_)),
     ClientMes_(new TMesTSESvrClient(*this)),
     ClientMesDestroyed_(false),
     ServerImpl_(ServerImpl),
     UID_(inet_ntoa(ia_.getAddress()))//(ia_.getHostname())
{
   char  buf[64];
   buf[0] = ':';
   //sprintf(buf+1, "%d/%d", port_, ServerImpl.GetMesServer()->GetListeningPort());
     sprintf(buf+1, "%d",    port_);
   UID_ += buf;
   setCompletion(false);   // Set up non-blocking reads
   //setDetectOutput(true); �b�٨S attach(svc) ���e, cc++ �| crash!! �ҥH��b attach() ����]�w!
}
//---------------------------------------------------------------------------
MesTSESvrClientImpl::~MesTSESvrClientImpl()
{
   TMesTSESvrClient*   mes = ClientMes_;
   ClientMes_ = 0;
   if(mes) {
      if(mes->IsLinkReady())
         mes->SetLinkError("PortDestroy");//�i��y�� ClientMes_ ���Ѻc
      if(!ClientMesDestroyed_)            // (!ClientMesDestroyed_) ���, ��誺 SetLinkError()
         delete mes;                      //  �S���y�� ClientMes_���Ѻc,�ҥH�n delete mes
   }
   attach(0);
   ServerImpl_.UpdateMesState(0);
   // 93.06.11 �ѨM linger �����D, �ҥH�b�����N so ����!
   #ifdef WIN32
	   closesocket(so);
   #else
	   close(so);
   #endif
	so = INVALID_SOCKET;
}
//---------------------------------------------------------------------------
K_mf(void) MesTSESvrClientImpl::CheckDisconnect ()
{
   if(ClientMes_)
      ClientMes_->SetLinkError("Disconnect");
}
//---------------------------------------------------------------------------
K_mf(void) MesTSESvrClientImpl::CheckOutput ()
{  // connection is ready for output
   if(ClientMes_  &&  !ClientMes_->IsLinkReady())
      ClientMes_->SetLinkReady();
   inherited::CheckOutput();
}
//---------------------------------------------------------------------------
K_mf(void) MesTSESvrClientImpl::CheckPending ()
{  //����ƨ��o!!
   if(ClientMes_ == 0)
      return;
   if(ClientMes_  &&  !ClientMes_->IsLinkReady())//�٨SSetLinkReady(),�N�������ƤF!
      ClientMes_->SetLinkReady();
   Recved_ = false;
   size_t   readySize = 0;//�p�G���D�bBuffer�����٦��h�ָ��,�h���X,�Y�����D,�h��J0
   ClientMes_->OnRecvReady(readySize);
   if(!Recved_) {//�S���I�s receive()
      char  buffer[1];
      receive(buffer, sizeof(buffer), MSG_PEEK);//�P�_�O�_�_�u?
   }
}
//---------------------------------------------------------------------------
K_mf(void) MesTSESvrClientImpl::attach  (TSocketService* svc)
{
#ifdef K_UNIXLIKE
   inherited::attach(svc);
   if(svc)                    //�b�٨S attach(svc) ���e, cc++ �| crash!!
      setDetectOutput(true);  //�ҥH��b attach() ����]�w!
#else
   if(svc)//WINDOWS�����b������attach(),�_�h�Y�battach()����,setDetectOutput()���e�_�u,�N�|crash
      setDetectOutput(true);  //UNIX����DelayFreeObj()����,�ҥH���|��������! �i�Ѧ� TMesTSESvrClient::FreeImpl()
   inherited::attach(svc);
#endif
}
//---------------------------------------------------------------------------
K_mf(void)  MesTSESvrClientImpl::OnMemQueueEmpty  ()
{
   if(ClientMes_ == 0)
      return;
   add_ref();
   ClientMes_->OnSendEmpty();
   release();
}
/////////////////////////////////////////////////////////////////////////////
K_ctor  TMesTSESvrClient::TMesTSESvrClient(MesTSESvrClientImpl& impl)
   : inherited(),
     Impl_(&impl)
   , Timer_(this)
{
   THeartBeat hb;
   hb.SetCtrlCode("10");
   if(Impl_)
      Impl_->send(&hb,sizeof(THeartBeat));
   Timer_.Start(10*1000, &this_type::OnTimer);
   Pkt_ = reinterpret_cast<TTseHead*>(RingQ_);
   Pos_ = 0;
}
//---------------------------------------------------------------------------
K_dtor  TMesTSESvrClient::~TMesTSESvrClient  ()
{
   FreeImpl();
   Timer_.Stop();
}
//---------------------------------------------------------------------------
K_mf(void)  TMesTSESvrClient::OnTimer  (TTimerBase*)
{
   if(Impl_)
      Impl_->send(&HB_,sizeof(THeartBeat));
}
//---------------------------------------------------------------------------
K_mf(std::string)  TMesTSESvrClient::GetUID  ()
{
   return(Impl_ ? Impl_->GetUID() : std::string());
}
//---------------------------------------------------------------------------
K_mf(int)  TMesTSESvrClient::Send  (const void* buffer, size_t size)
{
   char buf[4096];      
   const char* TMPMsg;
   TTseHead     Header;
   Header.SetCtrlCode("00");
   TMPMsg = static_cast<const char*>(buffer);
   Header.SetPkSize(size);
   memcpy(buf,&Header,HeadSize);
   memcpy(buf + HeadSize,TMPMsg,size);
   memcpy(buf + HeadSize + size,&Trailer,sizeof(Trailer));
   return Impl_ ? Impl_->send(buf, size + sizeof(Header) + sizeof(Trailer)) : -1;
}
//---------------------------------------------------------------------------
K_mf(int)  TMesTSESvrClient::Recv  (void* buffer, size_t size)
{
   int rtsz = -1;
   if(Impl_){
      rtsz = 0;
      int recsz = Impl_->receive(RingQ_ + Pos_, size);
      if(recsz > 0)
         Pos_ += recsz;
      while(Pos_ >= (Pkt_->GetPkSize() + HeadSize + TailSize)){
         int pktsz = Pkt_->GetPkSize()+ HeadSize + TailSize;
         memcpy((char*)buffer + rtsz, RingQ_ + HeadSize, Pkt_->GetPkSize());
         rtsz += Pkt_->GetPkSize();
         memcpy(RingQ_, RingQ_ + pktsz, Pos_ -= pktsz);
      }
   }
   return rtsz;
}
//---------------------------------------------------------------------------
K_mf(bool)  TMesTSESvrClient::Close  ()
{
   if(FreeImpl()  &&  IsLinkReady())
      SetLinkError("Closed!");
   return true;
}
//---------------------------------------------------------------------------
K_mf(bool)  TMesTSESvrClient::OpenImpl   ()
{
   SetLinkReady();
   return true;
}
//---------------------------------------------------------------------------^M
K_mf(bool)  TMesTSESvrClient::CloseImpl  ()
{
   return FreeImpl();
}
//---------------------------------------------------------------------------^M
K_mf(bool)  TMesTSESvrClient::FreeImpl  ()
{
   if(Impl_==0)
      return false;
   MesTSESvrClientImpl* impl = Impl_;
   Impl_ = 0;
   impl->ClientMesDestroyed_ = true;
   if(impl->ClientMes_ == this) {
      impl->ClientMes_ = 0;
      //#ifdef WIN32//fonwin: cc++���ק�L,�ҥH�i�H�A�Ω󪽱� delete
         impl->release();
      //#else
      //   DelayFreeObj(impl, TTimerBase::ots_TimerThread);
      //#endif
   }
   return true;
}
/////////////////////////////////////////////////////////////////////////////

