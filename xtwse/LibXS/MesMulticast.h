//
// 通訊classes基礎: Session-Messager
//    class TMesMulticast;
//
// 版本: $Id: MesMulticast.h,v 1.3 2004/01/09 08:47:10 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef __MesMulticast_H__
#define __MesMulticast_H__
/////////////////////////////////////////////////////////////////////////////
#include <cc++/socket.h>
#include "MesBaseSocket.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
class   MesMulticastService;
//---------------------------------------------------------------------------
LIBNS_class TMesMulticast: public TMesBaseSocket
{
   friend class MesMulticastService;
   typedef TMesBaseSocket  inherited;
public:
   K_ctor  TMesMulticast  ();
   K_dtor ~TMesMulticast  ();

   //format "Group:port:TTL:LoopBack"
   //    if(TTL==0)        recv only (no send)
   //    if(LoopBack==2)   send only (no recv)
   K_mf(bool)         SettingApply  (const std::string& str);
   K_mf(std::string)  GetSetting    ();
   K_mf(std::string)  GetUID        ();   //Get client id, <ipaddr:port>

private:
   K_mf(TMesBaseService*) CreateService ();

   std::string    GroupIP_;
   std::string    Interface_;
   int            Port_;
   int            TTL_;
   int            LoopBack_;
   sockaddr_in    LastRecvAddr_;
   bool           IsLastRecvAddrReady_;
   std::string    LastUID_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif //__MesMulticast_H__

