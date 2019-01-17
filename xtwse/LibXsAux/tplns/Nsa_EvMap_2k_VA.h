/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvMap_2k_VA_h_)
#define _Nsa_EvMap_2k_VA_h_
//---------------------------------------------------------------------------
#include "ViewAreaMgr.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TEvMap2kVA
/////////////////////////////////////////////////////////////////////////////
template <class Map1T, class Fm1T, class Fm2T>
class TEvMap2kVA : public TVACreater
{
   typedef TVACreater                     inherited;
   typedef TEvMap2kVA<Map1T, Fm1T, Fm2T>  this_type;

public:
   //==============================================
   // Type
   typedef          Map1T               TMap1;
   typedef          Fm1T                TFm1;
   typedef          Fm2T                TFm2;
   typedef typename TMap1::mapped_type  TMappedType1;
   typedef typename TMappedType1::TMap  TMap2;

   //==============================================
   // TViewer
   typedef TTViewArea<TMap1, TFm1>  TViewer1;
   typedef TTViewArea<TMap2, TFm2>  TViewer2;

   //==============================================
   // class TVACr2
   class TVACr2 : public TVACreater
   {
      this_type*  Owner_;

   public:
      inline K_ctor TVACr2 (this_type* p) : Owner_(p)  {}

      K_mf(TaViewArea) CreateNewVA()
      {
         TFm2 maker;
         return TaViewArea(new TViewer2(NULL, maker, Owner_->GetVcs2()));
      }
   };

protected:
   friend class TVACr2;

   TMap1*  Map1_;
   TVACr2  VACr2_;

public:
   inline K_ctor TEvMap2kVA ()            : Map1_(NULL), VACr2_(this)  {}
   inline K_ctor TEvMap2kVA (TMap1* map1) : Map1_(map1), VACr2_(this)  {}

   //==============================================
   // overwrite TVACreater virtual function
   K_mf(size_t)      GetDetails           (const char**& a);
   K_mf(TVACreater*) GetDetailVACreater   (size_t detailID);
   K_mf(TVACreater*) OnMasterViewRowMoved (TViewArea& masterVA, TViewArea*& detailVA, size_t detailID);
   K_mf(TaViewArea)  CreateNewVA          ();

protected:
   //==============================================
   // TEvMap2kVA virtual function
   virtual K_mf(TViewControls) GetVcs1 ()  { return TViewArea::DefaultViewControls(); }
   virtual K_mf(TViewControls) GetVcs2 ()  { return TViewArea::DefaultViewControls(); }
};
/////////////////////////////////////////////////////////////////////////////
// class TEvMap2kVA impl
/////////////////////////////////////////////////////////////////////////////
#define Nsa_TEvMap2kVA_Impl(return_type)                                   \
   template <class Map1T, class Fm1T, class Fm2T>                          \
   K_mf(return_type) TEvMap2kVA<Map1T, Fm1T, Fm2T>::                       \
//---------------------------------------------------------------------------

Nsa_TEvMap2kVA_Impl(size_t) GetDetails (const char**& a)
{
   static const char* desp[] = {"子系統-訊息代碼"};
   a = desp;
   return numofele(desp);
}
//---------------------------------------------------------------------------

Nsa_TEvMap2kVA_Impl(TVACreater*) GetDetailVACreater (size_t detailID)
{
   switch (detailID) {
   case 0:  return &VACr2_;
   }

   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------

Nsa_TEvMap2kVA_Impl(TVACreater*) OnMasterViewRowMoved (TViewArea&  masterVA,
                                                       TViewArea*& detailVA, size_t detailID)
{
   typedef typename TViewer1::cursor_type  TViewerCursorType1;

   TViewer1&                 va         = static_cast<TViewer1&>(masterVA);
   const TViewerCursorType1* cur        = va.GetCurRow();
   TViewer2*                 DetailView = static_cast<TViewer2*>(detailVA);

   if (cur->IsInvalid())
      DetailView->SetContainer(0);
   else {
      TMap2* map2 = cur->GetIterator()->second.GetContainer(map2);
      
      DetailView->SetContainer(map2);
   }

   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------

Nsa_TEvMap2kVA_Impl(TaViewArea) CreateNewVA ()
{
   TFm1 maker;
   return TaViewArea(new TViewer1(Map1_, maker, GetVcs1()));
}

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvMap_2k_VA_h_)
