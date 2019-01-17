/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvMap_2k1d_Setting_h_)
#define _Nsa_EvMap_2k1d_Setting_h_
//---------------------------------------------------------------------------
#include "Nsa_EvMap_0Setting.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TEvMap2k1dSetting
/////////////////////////////////////////////////////////////////////////////
template <class ExecObjT, class MapT1, class FmT1, class FmT2>
class TEvMap2k1dSetting : public TVACreater
{
   typedef TVACreater                                      inherited;
   typedef TEvMap2k1dSetting<ExecObjT, MapT1, FmT1, FmT2>  this_type;

public:
   typedef ExecObjT  TExecObj;
   typedef MapT1     TMap1;
   typedef FmT1      TFm1;
   typedef FmT2      TFm2;
   //
   typedef typename TMap1::mapped_type  TMappedType1;
   typedef typename TMappedType1::TMap  TMap2;

   // TViewer
   typedef TTViewArea<TMap1, TFm1>                     TViewer1;
   typedef TEvMapSettingViewer<TExecObj, TMap2, TFm2>  TViewer2;

   //==============================================
   // class TVACr2
   class TVACr2 : public TVACreater
   {
      this_type*  Owner_;
      TExecObj*   ExecObj_;

   public:
      inline K_ctor TVACr2 (this_type* tt, TExecObj* execObj)
         : Owner_  (tt)
         , ExecObj_(execObj)
      {}

      K_mf(TaViewArea) CreateNewVA ()
      {
         //TFm2          maker;
         TViewControls vcs = Owner_->GetVcs2();

         vcs.set(vco_Setting);

         return TaViewArea(new TViewer2(ExecObj_, vcs, NULL));
      }
   };

   friend class TVACr2;

protected:
   TExecObj*  ExecObj_;
   TMap1*     Map1_;
   TVACr2     VACr2_;

public:
   inline K_ctor TEvMap2k1dSetting (TExecObj* execObj, TMap1* map1)
      : ExecObj_(execObj)
      , Map1_   (map1)
      , VACr2_  (this, execObj)
   {}

   //==============================================
   // overwrite TVACreater virtual function
   K_mf(size_t)      GetDetails           (const char**& a);
   K_mf(TVACreater*) GetDetailVACreater   (size_t detailID);
   K_mf(TVACreater*) OnMasterViewRowMoved (TViewArea& masterVA, TViewArea*& detailVA, size_t detailID);
   K_mf(TaViewArea)  CreateNewVA          ();

protected:
   //==============================================
   // TEvMap2k1dSetting virtual function
   virtual K_mf(TViewControls) GetVcs1 ()  { return TViewArea::DefaultViewControls(); }
   virtual K_mf(TViewControls) GetVcs2 ()  { return TViewArea::DefaultViewControls(); }
};
/////////////////////////////////////////////////////////////////////////////
// class Nsa_TEvMap2k1dSetting impl
/////////////////////////////////////////////////////////////////////////////
#define Nsa_TEvMap2k1dSetting(return_type)                                 \
   template <class ExecObjT, class MapT1, class FmT1, class FmT2>          \
   K_mf(return_type) TEvMap2k1dSetting<ExecObjT, MapT1, FmT1, FmT2>::      \
//---------------------------------------------------------------------------

Nsa_TEvMap2k1dSetting(size_t) GetDetails (const char**& a)
{
   static const char* desp[] = {"子系統-訊息代碼"};
   a = desp;
   return numofele(desp);
}
//---------------------------------------------------------------------------

Nsa_TEvMap2k1dSetting(TVACreater*) GetDetailVACreater (size_t detailID)
{
   switch (detailID) {
   case 0:  return &VACr2_;
   }

   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------

Nsa_TEvMap2k1dSetting(TVACreater*) OnMasterViewRowMoved (TViewArea&  masterVA,
                                                         TViewArea*& detailVA, size_t detailID)
{
   TViewer1&                    va         = static_cast<TViewer1&>(masterVA);
   const TViewer1::cursor_type* cur        = va.GetCurRow();
   TViewer2*                    DetailView = static_cast<TViewer2*>(detailVA);
   std::string                  PrevKey;
   
   if (cur->IsInvalid()) {
      DetailView->SetContainer(0);
      //if (ApplyObj_) {
      //   ApplyObj_->SetPrevKey(PrevKey);
      //}
   }
   else {
      DetailView->SetContainer(cur->GetIterator()->second.GetContainer());
      //if (ApplyObj_) {
      //   PrevKey = cur->GetIterator()->first.as_string();
      //   ApplyObj_->SetPrevKey(PrevKey);
      //}
   }

   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------

Nsa_TEvMap2k1dSetting(TaViewArea) CreateNewVA ()
{
   TFm1 maker;
   return TaViewArea(new TViewer1(Map1_, maker, GetVcs1()));
}

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvMap_2k1d_Setting_h_)
