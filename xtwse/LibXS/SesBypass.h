//
// 通訊classes基礎: Session-Messager
//    class TSesBypass;
//    => Setting = MesName:MesSetting => 產生 MesBypass
//    => 從 SesMgr-Mes 收資料, 送到 MesBypass
//       從 MesBypass  收資料, 送到 SesMgr-Mes
//       當 SesMgr-Mes LinkReady 時, 開啟 MesBypass
//                     LinkBroken,   關閉 MesBypass
//
// 版本: $Id: SesBypass.h,v 1.3 2004/01/09 07:32:08 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SesBypassH
#define SesBypassH
//---------------------------------------------------------------------------
#include "Ses.h"
//---------------------------------------------------------------------------
namespace Kway {
struct      TSesReg;//in SesMgr.h
LIBNS_class TSesMgr;//in SesMgr.h
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSesBypass : public TSesBase
{
   typedef TSesBase  inherited;
   K_mf(void)  OnMesLinkReady    (TMesBase*);
   K_mf(void)  OnMesLinkBroken   (TMesBase*, const std::string& reason);
   K_mf(void)  OnMesLinkFail     (TMesBase*, const std::string& reason);
   K_mf(void)  OnMesRecvReady    (TMesBase*, size_t);
   K_mf(void)  OnMesStateChanged (TMesBase*, TMesStateLevel, const std::string&);
   K_mf(void)  OnMesEnter        (TMesBase*);
   K_mf(void)  OnMesSendEmpty    (TMesBase*);

   K_mf(bool)  SettingApply     (const std::string&);

   std::string    MesSetting_;
   TMesPtr        MesBypass_;
   TSesMgr*       SesMgr_;

public:
   K_ctor  TSesBypass  (TSesMgr*);
   K_dtor ~TSesBypass  ();

   static K_mf(const TSesReg&)  GetReg  ();
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
