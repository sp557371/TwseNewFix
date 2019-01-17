/////////////////////////////////////////////////////////////////////////////
#if !defined(_TwseDrProc_h_)
#define _TwseDrProc_h_
//---------------------------------------------------------------------------
#include "TwStkOrdCenter.h"
#include "FixStkOrdFlow.h"
#include "TwseFstFact.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   class TTwseSesApaSmp;

/////////////////////////////////////////////////////////////////////////////
// class TTwseDrProc
/////////////////////////////////////////////////////////////////////////////
class TTwseDrProc
{
   // Tw.Stk
   typedef Tw::Stk::TSysOrdID   TSysOrdID;
   typedef Tw::Stk::TStkQty     TStkQty;
   typedef Tw::Stk::TStkDeal    TStkDeal;
   typedef Tw::Stk::TStkDeals   TStkDeals;
   typedef Tw::Stk::TStkOrder   TStkOrder;
   typedef Tw::Stk::TSysOrds    TSysOrds;
   typedef Tw::Stk::TOrdCenter  TOrdCenter;
   // Fix.Stk
   typedef FIX::Stk::TFixSysOrdID      TFixSysOrdID;
   typedef FIX::Stk::SOfRnoField       SFixOfRnoField;
   typedef FIX::Stk::SRfBaseField      SFixRfBaseField;
   typedef FIX::Stk::SRfErField        SFixRfErField;
   typedef FIX::Stk::TFsStkOrder       TFsStkOrder;
   typedef FIX::Stk::TFsOrdFlow        TFixOrdFlow;
   typedef FIX::Stk::TRptFlow          TFixRptFlow;
   typedef FIX::Stk::TFsOrdCenter      TFixOrdCenter;
   typedef FIX::Stk::TTradeLog         TFixTradeLog;
   typedef FIX::Stk::TOfReqRnoBase     TFixOfReqRnoBase;
   typedef FIX::Stk::TOfReqRno         TFixOfReqRno;
   typedef FIX::Stk::TRfReqErOrdBase   TFixRfReqErOrdBase;
   typedef FIX::Stk::TRfReqErDealBase  TFixRfReqErDealBase;
   typedef FIX::Stk::TRfReqErOrd       TFixRfReqErOrd;
   typedef FIX::Stk::TRfReqErDeal      TFixRfReqErDeal;

   TSysOrds*           SysOrds_;
   TOrdCenter*         OrdCenter_;
   TFixOrdFlow*        FixOrdFlow_;
   TFixRptFlow*        FixRptFlow_;
   const std::string&  TandSesID_;
   TFixTradeLog*       FixTradeLog_;

public:
   K_ctor TTwseDrProc (TTwseFstInfo*);
   
   K_mf(void) Start (TTwseSesApaSmp*);

private:
   K_mf(TFixSysOrdID) AddFixNewOrd (TOrdCenter::TOrdHnd&);
   K_mf(void)         AddFixDeal   (const TFixSysOrdID&, const TStkOrder*, const TStkDeals*);
   K_mf(void)         AddFixFinal  (const TFixSysOrdID&, const TStkOrder*);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseDrProc_h_)
