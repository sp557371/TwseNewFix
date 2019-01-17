//
// �q�Tclasses��¦: Session-Messager
//    class TMesTCPServer;
//
// ����: $Id: MesTCPServer.h,v 1.4 2005/04/28 06:27:13 fonwin Exp $
//
// 921107:
//    �N class MesTCPServiceServer;    ��W�� class MesTCPServerImpl; �ò���� MesTCPServerBase.h ����
//    �N class MesTCPServerSocketPort; ��W�� class MesSvrClientImpl; �ò���� MesTCPServerBase.h ����
//    �N class TMesTCPServerC;         ��W�� class TMesSvrClient;    �ò���� MesTCPServerBase.h ����
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

