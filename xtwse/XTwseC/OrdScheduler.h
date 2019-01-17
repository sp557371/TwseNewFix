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

// �e�U�ƶ��ǦC,�C�ӥ��������T�Ӥ��P���Ū�Queue

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
   TReqPtrs     Odd_;           //�s��
   TReqPtrs     OddJam_;        //�s�Ѷ��
   TReqPtrs     Fix_;           //�L��
   TReqPtrs     Jam_;           //���
   TReqPtrs     Auct_;          //���
   TReqPtrs     Lend_;          //�Э�
   TReqPtrs     Tend_;          //����
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

