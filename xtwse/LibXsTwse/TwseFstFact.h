/////////////////////////////////////////////////////////////////////////////
#if !defined(_TwseFstFact_h_)
#define _TwseFstFact_h_
//---------------------------------------------------------------------------
#include "FixStkIntf.h"
// TwStk
#include "TwStkOrdFlow.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {

/////////////////////////////////////////////////////////////////////////////
// class TTwseFstFact
/////////////////////////////////////////////////////////////////////////////
class TTwseFstFact : public FIX::Stk::TTradeFact
{
   typedef Tw::Stk::TSysOrds    TSysOrds;
   typedef Tw::Stk::TOrdCenter  TOrdCenter;
   typedef Tw::Stk::TOrdFlow    TOrdFlow;

   typedef FIX::Stk::TTradeInfo  TFixTradeInfo;
   typedef FIX::Stk::TFsOrdFlow  TFixOrdFlow;
   typedef FIX::Stk::TRptFlow    TFixRptFlow;
   typedef FIX::Stk::TTradeAdpt  TFixTradeAdpt;

   TSysOrds*    SysOrds_;
   TOrdCenter*  OrdCenter_;
   TOrdFlow*    OrdFlow_;

public:
   inline K_ctor TTwseFstFact (TSysOrds* sos, TOrdCenter* oc, TOrdFlow* of)
      : SysOrds_  (sos)
      , OrdCenter_(oc)
      , OrdFlow_  (of)
   {}

   //==============================================
   // overwrite FIX::TStkTradeFact virtual function
   K_mf(TFixTradeInfo*) CreateTradeInfo    (const std::string&);
   K_mf(TFixOrdFlow*)   CreateOrdFlow      (TFixTradeInfo*);
   K_mf(TFixRptFlow*)   CreateRptFlow      (TFixTradeInfo*);
   K_mf(TFixTradeAdpt*) CreateTradeAdpt    (TFixTradeInfo*);
   K_mf(TVACreater*)    CreateOrdCenetrVAC (TFixTradeInfo*);
};

/////////////////////////////////////////////////////////////////////////////
// class TTwseFstInfo
/////////////////////////////////////////////////////////////////////////////
class TTwseFstInfo : public FIX::Stk::TTradeInfo
{
   // Fix.Stk
   typedef FIX::Stk::TTradeInfo  inherited;
   // Tw.Stk
   typedef Tw::Stk::TSysOrds    TSysOrds;
   typedef Tw::Stk::TOrdCenter  TOrdCenter;

   TSysOrds*    SysOrds_;
   TOrdCenter*  OrdCenter_;

   // ApaSesID:  T5U3U8U0UXU1U
   // TandSesID: T5380X1
   // 因為 ApaSesID 長度(13)太長了, 比 TwStkOrd.OrigSource_.UserID_ 長度(12) 還要長
   // 所以只好用 TandSesID
   std::string  TandSesID_;

public:
   K_ctor TTwseFstInfo (const std::string& sesID, TSysOrds*, TOrdCenter*);
      
   inline K_mf(TSysOrds*)   GetSysOrds   ()  { return SysOrds_;   }
   inline K_mf(TOrdCenter*) GetOrdCenter ()  { return OrdCenter_; }

   inline K_mf(const std::string&) GetTandSesID () const  { return TandSesID_; }
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseFstFact_h_)
