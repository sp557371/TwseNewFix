/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvCtn_1k_VA_h_)
#define _Nsa_EvCtn_1k_VA_h_
//---------------------------------------------------------------------------
#include "ViewAreaMgr.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa {

/////////////////////////////////////////////////////////////////////////////
// class TEvCtn1kVA
/////////////////////////////////////////////////////////////////////////////
template <class EvContainer, class FieldMaker, class Viewer = TTViewArea<EvContainer, FieldMaker> >
class TEvCtn1kVA : public TVACreater
{
public:
   typedef EvContainer  TEvContainer;
   typedef FieldMaker   TFieldMaker;
   typedef Viewer       TViewer;

protected:
   TEvContainer*  EvContainer_;

public:
   inline K_ctor TEvCtn1kVA (TEvContainer* ctn)
      : EvContainer_(ctn)
   {}

   K_mf(TaViewArea) CreateNewVA ()
   {
      TFieldMaker maker;
      TViewer*    viewer = new TViewer(EvContainer_, maker, GetVcs1());

      return TaViewArea(viewer);
   }

protected:
   //==============================================
   // TEvCtn1kVA virtual function
   virtual K_mf(TViewControls) GetVcs1 ()  { return TViewArea::DefaultViewControls(); }
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvCtn_1k_VA_h_)
