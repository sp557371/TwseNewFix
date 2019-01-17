/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nse2_EvMap_2k2d_VA_h_)
#define _Nse2_EvMap_2k2d_VA_h_
//---------------------------------------------------------------------------
#include "VADef.h"

/////////////////////////////////////////////////////////////////////////////
namespace Nse2 {

/*
   class TFm1 : public TFiledMakerBase
   {
      ...
   };

   class TFm2 : public TFiledMakerBase
   {
      ...
   };
*/

/////////////////////////////////////////////////////////////////////////////
// class TEvMap2k2dVA
/////////////////////////////////////////////////////////////////////////////
template <class MapT1, class MapT12, class FmT1, class FmT2>
class TEvMap2k2dVA : public Kway::TVACreater
{
   typedef Kway::TVACreater  inherited;

public:
   typedef MapT1   TMap1;
   typedef MapT12  TMap12;
   typedef FmT1    TFm1;
   typedef FmT2    TFm2;
   //
   typedef TMap12::mapped_type  TMap2;

   // TViewer
   typedef Kway::TTViewArea<TMap1, TFm1>  TViewer1;
   typedef Kway::TTViewArea<TMap2, TFm2>  TViewer2;

   // class TVACr2
   class TVACr2 : public Kway::TVACreater
   {
   public:
      K_mf(Kway::TaViewArea) CreateNewVA()
      {
         TFm2 Maker;
         return Kway::TaViewArea(new TViewer2(NULL, Maker, Maker.GetViewControls()));
      }
   };

protected:
   TMap1*   Map1_;
   TMap12*  Map12_;
   TVACr2   VACr2_;

public:
   inline K_ctor TEvMap2k2dVA ()                           : Map1_(NULL), Map12_(NULL)   {}
   inline K_ctor TEvMap2k2dVA (TMap1* Map1, TMap12* Map12) : Map1_(Map1), Map12_(Map12)  {}

   //
   // overwrite Kway::TVACreater virtual function
   K_mf(size_t)            GetDetails           (const char**& a);
   K_mf(Kway::TVACreater*) GetDetailVACreater   (size_t detailID);
   K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);
   K_mf(Kway::TaViewArea)  CreateNewVA          ();

   //
   // TEvMap2k2dVA function
   inline K_mf(void) SetMap (TMap1* Map1, TMap12* Map12)  { Map1_ = Map1;  Map12_ = Map12; }
};
/////////////////////////////////////////////////////////////////////////////
// class TEvMap2k2dVA impl
/////////////////////////////////////////////////////////////////////////////
#define TEvMap2k2dVA_Impl(return_type)                                     \
   template <class MapT1, class MapT12, class FmT1, class FmT2>            \
   K_mf(return_type) TEvMap2k2dVA<MapT1, MapT12, FmT1, FmT2>::             \
//---------------------------------------------------------------------------

TEvMap2k2dVA_Impl(size_t) GetDetails (const char**& a)
{
   static const char* desp[] = {"子系統-訊息代碼"};
   a = desp;
   return numofele(desp);
}
//---------------------------------------------------------------------------

TEvMap2k2dVA_Impl(Kway::TVACreater*) GetDetailVACreater (size_t detailID)
{
   switch (detailID) {
   case 0:  return &VACr2_;
   }

   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------

TEvMap2k2dVA_Impl(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea&  masterVA,
                                                             Kway::TViewArea*& detailVA, size_t detailID)
{
   TViewer1&                    va         = static_cast<TViewer1&>(masterVA);
   const TViewer1::cursor_type* cur        = va.GetCurRow();
   TViewer2*                    DetailView = static_cast<TViewer2*>(detailVA);

   if (cur->IsInvalid() || Map12_ == NULL)
      DetailView->SetContainer(0);
   else
      DetailView->SetContainer(&Map12_->fetch(cur->GetIterator()->first)->second);

   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------

TEvMap2k2dVA_Impl(Kway::TaViewArea) CreateNewVA ()
{
   TFm1 Maker;
   return Kway::TaViewArea(new TViewer1(Map1_, Maker, Maker.GetViewControls()));
}


/////////////////////////////////////////////////////////////////////////////
}; // namespace Nse2

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nse2_EvMap_2k2d_VA_h_)

