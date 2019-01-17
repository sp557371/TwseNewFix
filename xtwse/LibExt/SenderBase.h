//---------------------------------------------------------------------------
#ifndef SenderBaseH
#define SenderBaseH
//---------------------------------------------------------------------------
#include "TwStkOrder.h"
#include "TwStkOrdFlow.h"
#include "ExtDef.h"
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
class TSenderBase 
{
private:	// User declarations
protected:	// User declarations
   typedef Kway::Tw::Stk::TMarket           TMarket;
   typedef Kway::Tw::Stk::TTrxSegment       TTrxSegment;
   typedef Kway::Tw::Stk::TOrdFlow::TReqPtr TReqPtr;
public:		// User declarations
           K_ctor     TSenderBase(){};
           K_dtor     ~TSenderBase(){};
   virtual K_mf(bool) NewOrderAlert(TMarket mkt, TReqPtr& ReqPtr, ETradeKind tk = tk_Normal){return false;}
   virtual K_mf(void) SetMarketPh(TTrxSegment){};
   virtual K_mf(void) SetMarketPh(TSegDefs*){};
   virtual K_mf(bool) IsAutoStart(ETradeKind tk) {return false;}
   virtual K_mf(void) ResetScheduler() {};
};
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#endif
