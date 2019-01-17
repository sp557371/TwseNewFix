/////////////////////////////////////////////////////////////////////////////
#if !defined(_TwseFstUnit_h_)
#define _TwseFstUnit_h_
//---------------------------------------------------------------------------
// TwStk
#include "TwStkOrdFlow.h"
// FixStk
#include "FixStkTradeMgr.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {

/////////////////////////////////////////////////////////////////////////////
// class TTwseFstUnit
/////////////////////////////////////////////////////////////////////////////
class TTwseFstUnit : public FIX::Stk::TTradeUnit
{
   // FixStk
   typedef FIX::Stk::TTradeUnit    inherited;
   typedef FIX::Stk::TFsOrdCenter  TFixOrdCenter;
   // TwStk
   typedef Tw::Stk::TSysOrds    TSysOrds;
   typedef Tw::Stk::TOrdCenter  TOrdCenter;
   typedef Tw::Stk::TOrdFlow    TOrdFlow;

   TSysOrds*    SysOrds_;
   TOrdCenter*  OrdCenter_;
   TOrdFlow*    OrdFlow_;

public:
   K_ctor TTwseFstUnit (const std::string& sesID, TSysOrds*, TOrdCenter*, TOrdFlow*);

   //==============================================
   // overwrite FIX::TStkTradeUnit virtual function
   K_mf(FIX::Stk::TTradeFact*) CreateTradeFact ();
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseFstUnit_h_)
