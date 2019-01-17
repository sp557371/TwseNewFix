//
// 通訊classes基礎: Session-Messager
//    class TMesIPX;
//
// 版本: $Id: MesIPX.h,v 1.2 2004/01/09 08:47:10 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef __MesIPX_H__
#define __MesIPX_H__
/////////////////////////////////////////////////////////////////////////////
#include "MesBaseSocket.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
class MesIPXService;
//---------------------------------------------------------------------------
LIBNS_class TMesIPX : public TMesBaseSocket
{
   typedef TMesBaseSocket  inherited;
   friend class MesIPXService;
public:
   K_ctor  TMesIPX  ();
   K_dtor ~TMesIPX  ();

   K_mf(bool)         SettingApply  (const std::string&);
   K_mf(std::string)  GetSetting    ();
   K_mf(std::string)  GetUID        ();

private:
   K_mf(TMesBaseService*) CreateService ();

   std::string     LastUID_;
   dword           RemoteNetwork_;
   byte            RemoteNode_[6];
   word            RemotePort_;
   word            LocalPort_;
};
////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
