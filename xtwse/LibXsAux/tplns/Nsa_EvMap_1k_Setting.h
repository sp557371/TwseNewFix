/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvMap_1k_Setting_h_)
#define _Nsa_EvMap_1k_Setting_h_
//---------------------------------------------------------------------------
#include "ViewAreaMgr.h"
#include "Nsa_EvMap_0Setting.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TEvMap1kSetting
/////////////////////////////////////////////////////////////////////////////
template <class ExecObjT, class MapT1, class FmT1>
class TEvMap1kSetting : public TVACreater
{
   typedef TVACreater  inherited;

public:
   typedef ExecObjT  TExecObj;
   typedef MapT1     TMap1;
   typedef FmT1      TFm1;

   typedef TEvMapSettingViewer<TExecObj, TMap1, TFm1>  TViewer1;

protected:
   TExecObj*  ExecObj_;
   TMap1*     Map1_;

public:
   inline K_ctor TEvMap1kSetting (TExecObj* execObj, TMap1* map1)
      : ExecObj_(execObj)
      , Map1_   (map1)
   {}

   //==============================================
   // overwrite TVACreater virtual function
   K_mf(TaViewArea) CreateNewVA ();

protected:
   //==============================================
   // TEvMap1kSetting virtual function
   virtual K_mf(TViewControls) GetVcs1 ()  { return TViewArea::DefaultViewControls(); }
};
/////////////////////////////////////////////////////////////////////////////
// class TEvMap1kSetting impl
/////////////////////////////////////////////////////////////////////////////
#define Nsa_TEvMap1kSetting_Impl(return_type)                              \
   template <class ExecObjT, class MapT1, class FmT1>                      \
   K_mf(return_type) TEvMap1kSetting<ExecObjT, MapT1, FmT1>::              \
//---------------------------------------------------------------------------

Nsa_TEvMap1kSetting_Impl(TaViewArea) CreateNewVA ()
{
   //TFm1          maker;
   TViewControls vcs = GetVcs1();

   vcs.set(vco_Setting);

   TViewer1* va = new TViewer1(ExecObj_, vcs, Map1_);
   return TaViewArea(va);
}

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvMap_1k_Setting_h_)
