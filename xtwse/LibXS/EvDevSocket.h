/** @file EvDevSocket.h

@brief Socket基本功能.

@author 風人 fonwin@ms21.hinet.net
*/
#ifndef EvDevSocketH
#define EvDevSocketH
/////////////////////////////////////////////////////////////////////////////
#include "EvDev.h"
#include "MesTcpCliSetting.h"
#include <memory>
//---------------------------------------------------------------------------
namespace Kway {
class TMemQueue;
typedef u_short   port_t;
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSocketFD : public TAutoEvFD
{
   typedef TAutoEvFD          inherited;
   bool                       IsLinkReady_;
   std::auto_ptr<TMemQueue>   MemQueue_;
   size_t                     TotRecvSz_;
   std::string                LocalUID_;
   std::string                RemoteUID_;

   virtual K_mf(EEvResult)  OnLinkReady  ()  { return er_Continue; }
   virtual K_mf(EEvResult)  OnLinkBroken ()  { return er_Remove;   }
   virtual K_mf(EEvResult)  OnLinkFail   ()  { return er_Remove;   }
   virtual K_mf(EEvResult)  OnSendEmpty  ()  { return er_Continue; }
   virtual K_mf(EEvResult)  OnRecvReady  ()  { return er_Continue; }
           K_mf(void)       SetUID       ();

protected:
   virtual K_mf(EEvResult)  OnLinkError  ();

   K_mf(EEvResult)  OnEvRead   (TFD, TEventsDev*);
   K_mf(EEvResult)  OnEvWrite  (TFD, TEventsDev*);

   K_mf(void)        ClrMemQueue  (size_t mergeSize);
   K_mf(bool)        SetMemQueue  (size_t mergeSize);
   K_mf(std::string) CheckReadyFD ();
 //K_mf(std::string) Connect      (TEventsDev&, const char* ip, port_t port, port_t localPort, bool localPortReuse, size_t mergeSize);
   K_mf(std::string) Connect      (const TMesTcpCliSetting& setting, TMesTcpConnecting* handler);
   K_mf(std::string) ConnectingHandler (int eno, TEventsDev& dev);

   K_mf(std::string) Listen       (TEventsDev&, port_t port, bool portReuse);
   K_mf(bool)        Close        ();
public:
   K_ctor  TSocketFD  (TFD);
   K_ctor  TSocketFD  ();  // auto use: socket(AF_INET, SOCK_STREAM, 0)
   K_dtor ~TSocketFD  ();

   K_mf(size_t)  Recv  (void* buffer, size_t sz);
   K_mf(size_t)  Send  (const void* buffer, size_t sz);

   inline K_mf(bool)                IsLinkReady  () const   { return IsLinkReady_; }
   inline K_mf(const std::string&)  GetLocalUID  () const   { return LocalUID_;    }
   inline K_mf(const std::string&)  GetRemoteUID () const   { return RemoteUID_;   }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

