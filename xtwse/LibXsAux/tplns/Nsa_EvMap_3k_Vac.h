/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvMap_3k_Vac_h_)
#define _Nsa_EvMap_3k_Vac_h_
//---------------------------------------------------------------------------
#include "Nsa_EvMap_2k_VA.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TEvMap3kVac
/////////////////////////////////////////////////////////////////////////////
template <class Map1T, class Fm1T, class Fm2T, class Fm3T>
class TEvMap3kVac : public TVACreater
{
   typedef TVACreater                            inherited;
   typedef TEvMap3kVac<Map1T, Fm1T, Fm2T, Fm3T>  this_type;

public:
   //==============================================
   // Type
   typedef          Map1T               TMap1;
   typedef          Fm1T                TFm1;
   typedef          Fm2T                TFm2;
   typedef          Fm3T                TFm3;
   typedef typename TMap1::mapped_type  TMappedType1;
   typedef typename TMappedType1::TMap  TMap2;

   //==============================================
   // TDetailVac
   typedef TEvMap2kVA<TMap2, TFm2, TFm3>  TDetailVacBase;

   class TDetailVac : public TDetailVacBase
   {
      this_type*  Owner_;

   public:
      inline K_ctor TDetailVac (this_type* p) : Owner_(p)  {}

   protected:
      //==============================================
      // overwrite TDetailVacBase virtual function
      inline K_mf(TViewControls) GetVcs1 ()  { return Owner_->GetVcs2(); }
      inline K_mf(TViewControls) GetVcs2 ()  { return Owner_->GetVcs3(); }
   };

   //==============================================
   // TViewer
   typedef          TTViewArea<TMap1, TFm1>  TViewer1;
   typedef typename TDetailVac::TViewer1     TViewer2;

protected:
   friend class TDetailVac;

   TMap1*      Map1_;
   TDetailVac  DetailVac_;

public:
   inline K_ctor TEvMap3kVac ()            : Map1_(NULL), DetailVac_(this)  {}
   inline K_ctor TEvMap3kVac (TMap1* map1) : Map1_(map1), DetailVac_(this)  {}

   //==============================================
   // overwrite TVACreater virtual function
   K_mf(size_t)      GetDetails           (const char**& a);
   K_mf(TVACreater*) GetDetailVACreater   (size_t detailID);
   K_mf(TVACreater*) OnMasterViewRowMoved (TViewArea& masterVA, TViewArea*& detailVA, size_t detailID);
   K_mf(TaViewArea)  CreateNewVA          ();

protected:
   //==============================================
   // TEvMap3kVA virtual function
   virtual K_mf(TViewControls) GetVcs1 ()  { return TViewArea::DefaultViewControls(); }
   virtual K_mf(TViewControls) GetVcs2 ()  { return TViewArea::DefaultViewControls(); }
   virtual K_mf(TViewControls) GetVcs3 ()  { return TViewArea::DefaultViewControls(); }
};
/////////////////////////////////////////////////////////////////////////////
// class TEvMap3kVac impl
/////////////////////////////////////////////////////////////////////////////
#define Nsa_TEvMap3kVac_Impl(return_type)                                  \
   template <class Map1T, class Fm1T, class Fm2T, class Fm3T>              \
   K_mf(return_type) TEvMap3kVac<Map1T, Fm1T, Fm2T, Fm3T>::                \
//---------------------------------------------------------------------------

Nsa_TEvMap3kVac_Impl(size_t) GetDetails (const char**& a)
{
   static const char* desp[] = {"子系統-訊息代碼"};
   a = desp;
   return numofele(desp);
}
//---------------------------------------------------------------------------

Nsa_TEvMap3kVac_Impl(TVACreater*) GetDetailVACreater (size_t detailID)
{
   switch (detailID) {
   case 0:  return &DetailVac_;
   }

   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------

Nsa_TEvMap3kVac_Impl(TVACreater*) OnMasterViewRowMoved (TViewArea&  masterVA,
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

Nsa_TEvMap3kVac_Impl(TaViewArea) CreateNewVA ()
{
   TFm1 maker;
   return TaViewArea(new TViewer1(Map1_, maker, GetVcs1()));
}


/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvMap_3k_Vac_h_)

