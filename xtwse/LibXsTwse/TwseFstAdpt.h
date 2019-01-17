/////////////////////////////////////////////////////////////////////////////
#if !defined(_TwseFstAdpt_h_)
#define _TwseFstAdpt_h_
//---------------------------------------------------------------------------
#include <queue>
// Tw.Stk
#include "TwStkOrdFlow.h"
// Fix.Stk
#include "FixStkIntf.h"
#include "FixStkOrdFlow.h"
#include "FixStkRptFlow.h"
// Twse
#include "TwseVer.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {

/////////////////////////////////////////////////////////////////////////////
// class TTwseFstAdpt
/////////////////////////////////////////////////////////////////////////////
class TTwseFstAdpt : public FIX::Stk::TTradeAdpt
{
   typedef FIX::Stk::TTradeAdpt  inherited;
   typedef TTwseFstAdpt          this_type;
   typedef TTimer<this_type>     TTimerType;
   //==============================================
   // Fix.Stk
   typedef FIX::TFixQty2                    TFixQty;
   typedef FIX::Stk::EFixOrdCmdID           EFixOrdCmdID;
   typedef FIX::Stk::EFixRptCmdID           EFixRptCmdID;
   typedef FIX::Stk::SRfBaseField           SFixRfBaseField;
   typedef FIX::Stk::SRfErField             SFixRfErField;
   typedef FIX::Stk::SRfOcrField            SFixRfOcrField;
   typedef FIX::Stk::SOrdTrack              SFixOrdTrack;
   typedef FIX::Stk::TFixSysSeqNo           TFixSysSeqNo;
   typedef FIX::Stk::TFixSysOrdID           TFixSysOrdID;
   typedef FIX::Stk::TFixOrdCmdID           TFixOrdCmdID;
   typedef FIX::Stk::TFixRptCmdID           TFixRptCmdID;
   typedef FIX::Stk::TFsStkOrder            TFixStkOrder;
   typedef FIX::Stk::TFsOrdCenter           TFixOrdCenter;
   typedef FIX::Stk::TFsOrdCenter::TOrdHnd  TFixOrdHnd;
   typedef FIX::Stk::TOrdTrack              TFixOrdTrack;
   typedef FIX::Stk::TFsOrdFlow             TFixOrdFlow;
   typedef FIX::Stk::TRptFlow               TFixRptFlow;
   typedef FIX::Stk::TTradeInfo             TFixTradeInfo;
   typedef FIX::Stk::TTradeLog              TFixTradeLog;
   typedef FIX::Stk::TOfReqPtr              TFixOfReqPtr;
   typedef FIX::Stk::TOfReqBase             TFixOfReqBase;
   typedef FIX::Stk::TOfReqNosBase          TFixOfReqNosBase;
   typedef FIX::Stk::TOfReqOcrBase          TFixOfReqOcrBase;
   typedef FIX::Stk::TOfReqOcrrBase         TFixOfReqOcrrBase;
   typedef FIX::Stk::TOfReqOsrBase          TFixOfReqOsrBase;
   typedef FIX::Stk::TOfReqAnoBase          TFixOfReqAnoBase;
   //
   typedef FIX::Stk::TRfReqPtr              TFixRfReqPtr;
   typedef FIX::Stk::TRfReqBase             TFixRfReqBase;
   typedef FIX::Stk::TRfReqErBase           TFixRfReqErBase;
   typedef FIX::Stk::TRfReqErOrdBase        TFixRfReqErOrdBase;
   typedef FIX::Stk::TRfReqErRejectBase     TFixRfReqErRejectBase;
   typedef FIX::Stk::TRfReqErDealBase       TFixRfReqErDealBase;
   typedef FIX::Stk::TRfReqOcrBase          TFixRfReqOcrBase;
   typedef FIX::Stk::TRfReqErOrd            TFixRfReqErOrd;
   typedef FIX::Stk::TRfReqErDeal           TFixRfReqErDeal;
   typedef FIX::Stk::TRfReqErReject         TFixRfReqErReject;
   typedef FIX::Stk::TRfReqOcr              TFixRfReqOcr;
   //==============================================
   // TwStk
   typedef Tw::Stk::TMarket     TMarket;
   typedef Tw::Stk::TSysOrdID   TSysOrdID;
   typedef Tw::Stk::TStkOrder   TStkOrder;
   typedef Tw::Stk::TSysOrds    TSysOrds;
   typedef Tw::Stk::TOrdFlow    TOrdFlow;
   typedef Tw::Stk::TOrdCenter  TOrdCenter;

   class TOrdImpl;
   class TOrdStepImpl;
   class TRptImpl;
   
   class TFixOrdImpl;

   class TFixRptPreSet;
   class TFixRptImpl;

   friend class TFixOrdImpl;
   
   TSysOrds*    SysOrds_;
   TOrdCenter*  OrdCenter_;
   TOrdFlow*    OrdFlow_;
   
   TOrdImpl*         OrdImpl_;
   TOrdStepImpl*     OrdStepImpl_;
   TRptImpl*         RptImpl_;

   //TFixOrdPreCheck*  FixOrdPreCheck_;
   TFixOrdImpl*      FixOrdImpl_;
   
   TFixRptPreSet*  FixRptPreSet_;
   TFixRptImpl*    FixRptImpl_;
   
   // ApaSesID:  T5U3U8U0UXU1U
   // TandSesID: T5380X1
   // 因為 ApaSesID 長度(13)太長了, 比 TwStkOrd.OrigSource_.UserID_ 長度(12) 還要長
   // 所以只好用 TandSesID
   // 資料來源為 TwseFstInfo.TandSesID_ 資訊
   std::string  TandSesID_;

   // 在 Tw.Stk.OrdCenter 裡面有 [傳送中][已傳送的狀態], 但是沒有回報
   // 所以我必須要等待 TMPSes 都連上了, 並且末筆查詢都結束了, 我才可以再次檢查
   typedef std::queue<TSysOrdID>  TSysOrdIDQ;
   
   TTimerType  Timer_;
   TSysOrdIDQ  NotSentList_;
   TSysOrdIDQ  DlyChkList_;
   bool        IsReady_; // 是否在等待處理 [未送出委託] or [末筆查詢結果]

public:
   K_ctor  TTwseFstAdpt (TFixTradeInfo*, TSysOrds*, TOrdCenter*, TOrdFlow*);
   K_dtor ~TTwseFstAdpt ();

   //==============================================
   // overwrite FIX::TStkTradeAdpt virtual function
   K_mf(void) Startup ();

   //==============================================
   // TTwseFstAdpt function
   K_mf(void) PreChkOrdSts ();

   // TwStk Objs
   inline K_mf(TSysOrds*)   GetSysOrds   ()  { return SysOrds_;   }
   inline K_mf(TOrdCenter*) GetOrdCenter ()  { return OrdCenter_; }
   inline K_mf(TOrdFlow*)   GetOrdFlow   ()  { return OrdFlow_;   }

   // FixStk Impl Objs
   inline K_mf(TOrdImpl*) GetOrdImpl ()  { return OrdImpl_; }

   inline K_mf(const std::string&) GetTandSesID () const  { return TandSesID_; }
   inline K_mf(bool)               IsReady      () const  { return IsReady_;   }

protected:
   K_mf(void) OnTimer      (TTimerBase*);
   K_mf(bool) Proc_NotSent ();
   K_mf(void) Proc_DlyChk  ();
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseOrdAdpt_h_)
