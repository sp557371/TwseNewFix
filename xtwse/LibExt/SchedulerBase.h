//---------------------------------------------------------------------------
#ifndef SchedulerBaseH
#define SchedulerBaseH
//---------------------------------------------------------------------------
#include "TwStkOrdFlow.h"
#include "TwStkOrdCenter.h"
#include "SenderBase.h"
#include "ExtDef.h"
#include "ORBase.h"
//---------------------------------------------------------------------------
K_class TSchedulerBase :public  Kway::Tw::Stk::TOrdFlow::TFlowStep
{
protected:
   typedef Kway::Tw::Stk::TOrdCenter            TOrdCenter;
   typedef Kway::Tw::Stk::TMarket               TMarket;
   typedef Kway::Tw::Stk::TTrxSegment           TTrxSegment;
   typedef Kway::Tw::Stk::TOrdFlow::TReqPtr     TReqPtr;
   TOrdCenter*                  OrdCenter_;
   TSenderBase*                 Sender_;
   TORBase*                     ORM_;
   TStkLines                    LineState_;
   TTrxSegment                  MarketPh_;
public:
   K_mf(TOrdCenter*)            GetOrdCenter()  {return OrdCenter_;}
   K_mf(bool)                   SetSender(TSenderBase* sb) {
                                     if(Sender_ && sb) 
                                        return false;
                                     Sender_ = sb;
                                     if(Sender_)
                                        Sender_->SetMarketPh(MarketPh_);
                                     return true;}
   K_mf(bool)                   SetORM(TORBase* orb) {
                                     if(ORM_ && orb) 
                                        return false;
                                     ORM_ = orb;
                                     if(ORM_)
                                        ORM_->SetState(LineState_);
                                     return true;}
   virtual K_mf(TReqPtr)        GetOrderByMkt(TMarket mkt, ETradeKind tk = tk_Normal){return TReqPtr(0);}
   virtual K_mf(TTrxSegment)    GetMarketPh(){return MarketPh_;}
   virtual K_mf(bool)           SetMarketPh(TTrxSegment ts){MarketPh_ = ts;}
   virtual K_mf(void)           SetMarketPh(TSegDefs*){};
   virtual K_mf(void)           SetLineState(TMarket mkt, ETradeKind, int) = 0;
   virtual K_mf(void)           SetLS(TMarket mkt, ETradeKind tk, Kway::word value){
                                         LineState_.SetL(mkt,tk,value);}
   virtual K_mf(void)           SetAll(Kway::word value){
                                         LineState_.SetAll(value);}
   virtual K_mf(void)           SetQS(TMarket mkt, ETradeKind tk, Kway::word value){
                                         LineState_.SetQ(mkt,tk,value);}
   virtual K_mf(void)           DoAutoReq(TReqPtr, TMarket mkt, ETradeKind tk) = 0;
   virtual K_mf(TSenderBase*)   GetSender(){return Sender_;}
   virtual K_mf(TORBase*)       GetORM()   {return ORM_;}
   virtual K_mf(void)           DoReOrder(TReqPtr, TMarket mkt, ETradeKind tk) = 0;
};
/////////////////////////////////////////////////////////////////////////////
#endif
