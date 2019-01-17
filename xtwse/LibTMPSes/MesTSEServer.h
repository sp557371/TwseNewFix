//
// �q�Tclasses��¦: Session-Messager
//    class TMesTCPServer;
//
// ����: $Id: MesTCPServer.h,v 1.3 2003/11/25 05:16:30 fonwin Exp $
//
// 921107:
//    �N class MesTCPServiceServer;    ��W�� class MesTCPServerImpl; �ò���� MesTCPServerBase.h ����
//    �N class MesTCPServerSocketPort; ��W�� class MesSvrClientImpl; �ò���� MesTCPServerBase.h ����
//    �N class TMesTCPServerC;         ��W�� class TMesSvrClient;    �ò���� MesTCPServerBase.h ����
//
//---------------------------------------------------------------------------
#ifndef __MesTSEServerH__
#define __MesTSEServerH__
/////////////////////////////////////////////////////////////////////////////
#include <cc++/socket.h>
#include "Mes.h"
#include "KwayBase.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
K_class TMesTSEServer;
class   MesTSEServerImpl;
class   MesTSESvrClientImpl;
class   TMesTSESvrClient;
/////////////////////////////////////////////////////////////////////////////
K_class TMesTSEServer : public TMesBase
{
   friend class MesTSEServerImpl;
   friend class MesTSESvrClientImpl;
public:
           K_ctor  TMesTSEServer  ();
   virtual K_dtor ~TMesTSEServer  ();

   virtual K_mf(bool)         SettingApply      (const std::string& setting);
   virtual K_mf(std::string)  GetSetting        ();
   inline  K_mf(int)          GetListeningPort  () const  { return ListeningPort_; }

   virtual K_mf(bool)  Open   (void);
   virtual K_mf(bool)  Close  (void);
   virtual K_mf(int)   Send   (const void* buffer, size_t size)   { return -1; }
   virtual K_mf(int)   Recv   (      void* buffer, size_t size)   { return -1; }
protected:
   virtual K_mf(bool)       AcceptClient     (MesTSESvrClientImpl*&, SOCKET);
   K_mf(MesTSESvrClientImpl*)  CreateClientPort (SOCKET);
 //K_mf(void)               WaitClientReady  (MesSvrClientImpl*);
private:
  int                ListeningPort_;
  MesTSEServerImpl*  Impl_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif //__MesTCPServerH__

