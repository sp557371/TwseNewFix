//
// �q�Tclasses��¦: Session-Messager
//    class TMesTCPServer;
//
// ����: $Id: MesTCPServer.cpp,v 1.5 2003/11/25 05:16:30 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "MesTSEServer.h"
#include "MesTSEServerBase.h"
#include "TimerThread.h"
#include "fixed_num.h"//for ltoa
/////////////////////////////////////////////////////////////////////////////
using namespace Kway;
/////////////////////////////////////////////////////////////////////////////
K_ctor  TMesTSEServer::TMesTSEServer  ()
   : TMesBase(),
     ListeningPort_(0),
     Impl_(0)
{
}
//---------------------------------------------------------------------------
K_dtor  TMesTSEServer::~TMesTSEServer  ()
{
   Close();
}
//---------------------------------------------------------------------------
K_mf(bool)  TMesTSEServer::SettingApply  (const std::string& setting)
{
   ListeningPort_ = (tpport_t) (atoi(setting.c_str()));
   return true;
}
//---------------------------------------------------------------------------
K_mf(std::string)  TMesTSEServer::GetSetting  ()
{
   char buf[64];
   ltoa(ListeningPort_, buf, 10);
   return std::string(buf);
}
//---------------------------------------------------------------------------
K_mf(bool)  TMesTSEServer::Open  ()
{
   if(Impl_==NULL) {
      // listen to the port
      OnStateChanged(mslNormal, "Opening...");
      try {
         Impl_ = new MesTSEServerImpl(*this);
         Impl_->StartServer();
      } catch(exception& e) {
         delete Impl_;
         OnStateChanged(mslFatal, e.what());
      } catch(...) {
         OnStateChanged(mslFatal, "Open fail!");
      }
   }
   return true;
}
//---------------------------------------------------------------------------
K_mf(bool)  TMesTSEServer::Close  ()
{
   MesTSEServerImpl* impl = Impl_;
   if(impl) {
      Impl_ = NULL;
      impl->MesServer_ = 0;
      impl->Free();
      OnStateChanged(mslNormal, "Closed!");
   }
   return true;
}
//---------------------------------------------------------------------------
K_mf(MesTSESvrClientImpl*)  TMesTSEServer::CreateClientPort  (SOCKET s)
{
   return new MesTSESvrClientImpl(*Impl_, s);
}
//---------------------------------------------------------------------------
/*fonwin:921109:begin{ �N���B�z����Thread����
K_mf(void)  TMesTSEServer::WaitClientReady  (MesTSESvrClientImpl* clientPort)
{
   clientPort->WaitLinkReady(Impl_->GetSocketService());
}
fonwin:921109:end} */
//---------------------------------------------------------------------------
K_mf(bool)  TMesTSEServer::AcceptClient  (MesTSESvrClientImpl*& client, SOCKET s)
{
   client = 0;
   if(Impl_==0)
      return false;
   TMesEvHandler* clientEvH = GetClientEvHandler();
   if(clientEvH == 0) {
      //�ڵ�client�s�u
      return false;
   }
   MesTSESvrClientImpl* clientPort = CreateClientPort(s);
   //try {
      //�קK�ƥ󤣦P�B(��p��,�b�٨SSetLinkReady()���e,������OnRecvReady()���ƥ�)
      //�ҥH���N�B�zClients��Thread�Ȱ�,���s��Client�ǳƦn����A�Ұ�
      // => �i�O�p�G SocketService_ ���b������a��,����O?
      //    SocketService_->suspend();
      //�ҥH�令�b�i�H output() ��,�~�]�w SetLinkReady()
      //���̫�令�b����������~attach(SocketService_)�N�i�קK�H�W�����D�F!
      TMesTSESvrClient*  clientMes = clientPort->GetClientMes();
      clientMes->SetEvHandler(clientEvH);
      if(OnClientConnect(clientMes)) {
         //clientMes->SetLinkReady();
         //fonwin:921109:�N���B�z����Thread����: WaitClientReady(clientPort);
         //              �ç令���U���覡:
         //clientPort->SetLinkReady(); fonwin:20031117:��� TSocketPortBase �B�z
         client = clientPort;
         //fonwin:921109:end
      }
   //} __finally {
      //SocketService_->resume();//�s��Client�ǳƦn�F,�Ұ�Thread
   //}
   return true;
}
/////////////////////////////////////////////////////////////////////////////

