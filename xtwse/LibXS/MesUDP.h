//
// 通訊classes基礎: Session-Messager
//    class TMesUDP;
//
// 版本: $Id: MesUDP.h,v 1.2 2004/01/09 08:47:10 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef __MesUDP_H__
#define __MesUDP_H__
/////////////////////////////////////////////////////////////////////////////
#include "MesBaseSocket.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
class MesUDPService;
//---------------------------------------------------------------------------
LIBNS_class TMesUDP : public TMesBaseSocket
{
   typedef TMesBaseSocket  inherited;
   friend class MesUDPService;
public:
   K_ctor  TMesUDP  ();
   K_dtor ~TMesUDP  ();

   K_mf(bool)         SettingApply  (const std::string&);
   K_mf(std::string)  GetSetting    ();
   K_mf(std::string)  GetUID        ();

private:
   K_mf(TMesBaseService*) CreateService ();

   std::string    LastUID_;
   std::string    RemoteIP_;
   int            RemotePort_;
   int            LocalPort_;
};
////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
