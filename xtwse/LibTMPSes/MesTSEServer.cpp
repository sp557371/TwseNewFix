//
// 通訊classes基礎: Session-Messager
//    class TMesTCPServer;
//
// 版本: $Id: MesTCPServer.cpp,v 1.5 2003/11/25 05:16:30 fonwin Exp $
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
/*fonwin:921109:begin{ 將此處理移到Thread之中
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
      //拒絕client連線
      return false;
   }
   MesTSESvrClientImpl* clientPort = CreateClientPort(s);
   //try {
      //避免事件不同步(比如說,在還沒SetLinkReady()之前,先收到OnRecvReady()的事件)
      //所以先將處理Clients的Thread暫停,等新的Client準備好之後再啟動
      // => 可是如果 SocketService_ 停在不恰當的地方,怎麼辦呢?
      //    SocketService_->suspend();
      //所以改成在可以 output() 時,才設定 SetLinkReady()
      //●最後改成在全部完成後才attach(SocketService_)就可避免以上的問題了!
      TMesTSESvrClient*  clientMes = clientPort->GetClientMes();
      clientMes->SetEvHandler(clientEvH);
      if(OnClientConnect(clientMes)) {
         //clientMes->SetLinkReady();
         //fonwin:921109:將此處理移到Thread之中: WaitClientReady(clientPort);
         //              並改成底下的方式:
         //clientPort->SetLinkReady(); fonwin:20031117:改由 TSocketPortBase 處理
         client = clientPort;
         //fonwin:921109:end
      }
   //} __finally {
      //SocketService_->resume();//新的Client準備好了,啟動Thread
   //}
   return true;
}
/////////////////////////////////////////////////////////////////////////////

