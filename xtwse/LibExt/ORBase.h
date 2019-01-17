//---------------------------------------------------------------------------
#ifndef ORBaseH
#define ORBaseH
//---------------------------------------------------------------------------
#include "TwStkOrder.h"
#include "TwStkOrdFlow.h"
#include "ExtDef.h"
//---------------------------------------------------------------------------
class TORBase 
{
private:
protected:	// User declarations
   typedef Kway::Tw::Stk::TOrdCenter        TOrdCenter;
   typedef Kway::Tw::Stk::TOrdFlow          TOrdFlow;
   typedef Kway::Tw::Stk::TMarket           TMarket;
   typedef Kway::Tw::Stk::TOrdFlow::TReqPtr TReqPtr;
   TOrdCenter*             OrdCenter_;        // 繞單模組的OrdCenter
   TOrdFlow*               OrdFlow_;          // 繞單模組的OrdFlow
   TStkLines               LineState_;        // 所有可繞單主機的線路狀況加總
public:		
           K_ctor          TORBase      (){};
           K_dtor          ~TORBase     (){};
   virtual K_mf(bool)      SendOrd      (TReqPtr&, TMarket mkt, ETradeKind tk = tk_Normal, int sz = -1)
                                                                           {return false;} // 新委託到達後的作業
   virtual K_mf(void)      SetState     (TStkLines ls){};                 // 當本機的線路數量改變時所發出的通知
   virtual K_mf(Kway::word) GetLineState (TMarket m, ETradeKind tk){      // 取得所有繞單機制所回報的可用線路數量
   	                                   return LineState_.GetL(m,tk);}
   virtual K_mf(bool)      IsUseORD     (){return true;}                  // 是否進行分單作業
   virtual K_mf(bool)      IsUseORM     (){return true;}                  // 是否可進行斷線繞單作業
   virtual K_mf(void)      ResetScheduler() {};
};
/////////////////////////////////////////////////////////////////////////////
#endif
