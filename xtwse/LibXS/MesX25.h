//---------------------------------------------------------------------------
//
// 通訊classes基礎: Session-Messager
//    class TMesX25;
//
// 版本: $Id: MesX25.h,v 1.3 2005/04/28 06:27:51 fonwin Exp $
//
//
//---------------------------------------------------------------------------
#ifndef MesX25H
#define MesX25H
//---------------------------------------------------------------------------
#include "Mes.h"
//---------------------------------------------------------------------------
namespace Kway {
//--------------
struct TMesReg;
/////////////////////////////////////////////////////////////////////////////
class MesX25Service;
//---------------------------------------------------------------------------
LIBNS_class TMesX25 : public TMesBase
{
   typedef TMesBase  inherited;
   friend class MesX25Service;
public:
   K_ctor  TMesX25  ();
   K_dtor ~TMesX25  ();

   K_mf(bool)         SettingApply  (const std::string&);
   K_mf(std::string)  GetSetting    ();
   K_mf(bool)         OpenImpl      ();
   K_mf(bool)         CloseImpl     ();
   K_mf(int)          Send          (const void* buffer, size_t size);
   K_mf(int)          Recv          (      void* buffer, size_t size);

   static K_mf(TMesReg&)  GetReg  ();

private:
   word           LineNo_;
   word           PvcNo_;
   MesX25Service* Service_;
};
////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

