//
// 通訊classes基礎: Session-Messager
//    class TMesTCPServer;
//
// 版本: $Id: MesTCPServer.h,v 1.4 2005/04/28 06:27:13 fonwin Exp $
//
// 921107:
//    將 class MesTCPServiceServer;    改名為 class MesTCPServerImpl; 並移放到 MesTCPServerBase.h 之中
//    將 class MesTCPServerSocketPort; 改名為 class MesSvrClientImpl; 並移放到 MesTCPServerBase.h 之中
//    將 class TMesTCPServerC;         改名為 class TMesSvrClient;    並移放到 MesTCPServerBase.h 之中
//
//---------------------------------------------------------------------------
#ifndef __MesTCPServerH__
#define __MesTCPServerH__
/////////////////////////////////////////////////////////////////////////////
#include <cc++/socket.h>
#include "Mes.h"
#include "KwayBase.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMesTCPServer;
class       MesTCPServerImpl;
class       MesSvrClientImpl;
class       TMesSvrClient;
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMesTCPServer : public TMesBase
{
   friend class MesTCPServerImpl;
   friend class MesSvrClientImpl;
public:
           K_ctor  TMesTCPServer  ();
   virtual K_dtor ~TMesTCPServer  ();

   virtual K_mf(bool)         SettingApply      (const std::string& setting);
   virtual K_mf(std::string)  GetSetting        ();
   inline  K_mf(int)          GetListeningPort  () const  { return ListeningPort_; }

   virtual K_mf(int)   Send   (const void* buffer, size_t size)   { return -1; }
   virtual K_mf(int)   Recv   (      void* buffer, size_t size)   { return -1; }
protected:
   virtual K_mf(bool)       AcceptClient     (MesSvrClientImpl*&, SOCKET);
   K_mf(MesSvrClientImpl*)  CreateClientPort (SOCKET);
 //K_mf(void)               WaitClientReady  (MesSvrClientImpl*);

   virtual K_mf(bool)  OpenImpl   ();
   virtual K_mf(bool)  CloseImpl  ();
private:
  int                ListeningPort_;
  MesTCPServerImpl*  Impl_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif //__MesTCPServerH__

