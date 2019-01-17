#ifndef OrdSchedulerH
#define OrdSchedulerH
//---------------------------------------------------------------------------
#include "TwStkOrdFlow.h"
#include "SenderBase.h"
#include "ORBase.h"
#include "SysInfo.h"
#include "SchedulerBase.h"
#include <queue>
//===========================================================================
using namespace Kway;
using namespace Kway::Tw;
//===========================================================================

// 委託排隊序列,每個市場都有三個不同等級的Queue

//===========================================================================

typedef Stk::TOrdFlow::TReqPtr     TReqPtr;
//typedef Stk::TOrdFlow::TReqPtrs    TReqPtrs;
typedef std::queue<TReqPtr>        TReqPtrs;
typedef Stk::TOrdCenter::TOrdHnd   TOrdHnd;
//===========================================================================
struct TQueue
{
   TReqPtrs     High_;          //High Priority
   TReqPtrs     Normal_;        //Normal Priority
   TReqPtrs     Low_;           //Low Priority
   TReqPtrs     Odd_;           //零股
   TReqPtrs     OddJam_;        //零股塞單
   TReqPtrs     Fix_;           //盤後
   TReqPtrs     Jam_;           //塞單
   TReqPtrs     Auct_;          //拍賣
   TReqPtrs     Lend_;          //標借
   TReqPtrs     Tend_;          //標購
};
//===========================================================================
enum eOrdPriority{
   eop_High   = 0,
   eop_Normal = 1,
   eop_Low    = 2,
   eop_Jam    = 3,
};
//===========================================================================
K_class TScheduler : public  TSchedulerBase
{
private:
   typedef  TOrdFlow::TReqPtr TReqPtr;
   typedef  Stk::TTrxSegment  TTrxSegment;
   TQueue          TseQue_;
   TQueue          OtcQue_;
   TQueue          OtcRQue_;
   TFile           Log_;
   TFile           QLog_;
   int             DebugLog_;
//   TSysInfoPage*   SysInfo_;
   TSegDefs        SegDefs_;
   bool            EnableAutoSwitch_;
   
   K_mf(void)      AddRec(TReqPtr& req,Stk::TMarket mkt,TOType, ETradeKind tk = tk_Normal, eOrdPriority eop = eop_Normal);
   K_mf(TReqPtr)   GetOrderFromQ(TQueue& que, Stk::TMarket mkt, ETradeKind tk = tk_Normal);

public:
   K_ctor          TScheduler(TOrdCenter&/*,TSysInfoPage**/);
   K_dtor          ~TScheduler();
   K_mf(bool)      OnOrdReq(TReqPtr req);
   K_mf(TReqPtr)   GetOrderByMkt(Stk::TMarket mkt, ETradeKind tk = tk_Normal);
   K_mf(bool)      SetMarketPh(TTrxSegment);
   K_mf(void)      SetMarketPh(TSegDefs*);
   K_mf(void)      SetLineState(Stk::TMarket mkt, ETradeKind tk,int sz);
   K_mf(void)      DoAutoReq(TReqPtr req, TMarket mkt, ETradeKind tk);
   K_mf(void)      DoReOrder(TReqPtr req, TMarket mkt, ETradeKind tk);
};
/////////////////////////////////////////////////////////////////////////////
#endif

