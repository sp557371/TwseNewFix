/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvMap_2k1d_VA_h_)
#define _Nsa_EvMap_2k1d_VA_h_
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TEvMap2k1dVA
/////////////////////////////////////////////////////////////////////////////
template <class MapT1, class FmT1, class FmT2>
class TEvMap2k1dVA : public TVACreater
{
   typedef TVACreater                       inherited;
   typedef TEvMap2k1dVA<MapT1, FmT1, FmT2>  this_type;

public:
   typedef MapT1  TMap1;
   typedef FmT1   TFm1;
   typedef FmT2   TFm2;
   //
   typedef typename TMap1::mapped_type  TMap2;

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

      inline K_mf(TaViewArea) CreateNewVA ()
      {
         TFm2 maker;
         return TaViewArea(new TViewer2(NULL, maker, Owner_->GetVcs2()));
      }
   };

protected:
   TMap1*  Map1_;
   TVACr2  VACr2_;

public:
   inline K_ctor TEvMap2k1dVA ()            : Map1_(NULL), VACr2_(this)  {}
   inline K_ctor TEvMap2k1dVA (TMap1* Map1) : Map1_(Map1), VACr2_(this)  {}

   //==============================================
   // overwrite TVACreater virtual function
   K_mf(size_t)      GetDetails           (const char**& a);
   K_mf(TVACreater*) GetDetailVACreater   (size_t detailID);
   K_mf(TVACreater*) OnMasterViewRowMoved (TViewArea& masterVA, TViewArea*& detailVA, size_t detailID);
   K_mf(TaViewArea)  CreateNewVA          ();

protected:
   //==============================================
   // TEvMap2k1dVA virtual function
   virtual K_mf(TViewControls) GetVcs1 ()  { return TViewArea::DefaultViewControls(); }
   virtual K_mf(TViewControls) GetVcs2 ()  { return TViewArea::DefaultViewControls(); }
};
/////////////////////////////////////////////////////////////////////////////
// class TEvMap2k1dVA impl
/////////////////////////////////////////////////////////////////////////////
#define Nsa_TEvMap2k1dVA_Impl(return_type)                                 \
   template <class MapT1, class FmT1, class FmT2>                          \
   K_mf(return_type) TEvMap2k1dVA<MapT1, FmT1, FmT2>::                     \
//---------------------------------------------------------------------------

Nsa_TEvMap2k1dVA_Impl(size_t) GetDetails (const char**& a)
{
   static const char* desp[] = {"子系統-訊息代碼"};
   a = desp;
   return numofele(desp);
}
//---------------------------------------------------------------------------

Nsa_TEvMap2k1dVA_Impl(TVACreater*) GetDetailVACreater (size_t detailID)
{
   switch (detailID) {
   case 0:  return &VACr2_;
   }

   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------

Nsa_TEvMap2k1dVA_Impl(TVACreater*) OnMasterViewRowMoved (TViewArea&  masterVA,
                                                           TViewArea*& detailVA, size_t detailID)
{
   TViewer1&                    va         = static_cast<TViewer1&>(masterVA);
   const TViewer1::cursor_type* cur        = va.GetCurRow();
   TViewer2*                    DetailView = static_cast<TViewer2*>(detailVA);

   if (cur->IsInvalid())
      DetailView->SetContainer(0);
   else
      DetailView->SetContainer(&(cur->GetIterator()->second));

   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------

Nsa_TEvMap2k1dVA_Impl(TaViewArea) CreateNewVA ()
{
   TFm1 maker;
   return TaViewArea(new TViewer1(Map1_, maker, GetVcs1()));
}

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvMap_2k1d_VA_h_)
