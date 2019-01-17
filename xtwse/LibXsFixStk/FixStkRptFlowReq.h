/////////////////////////////////////////////////////////////////////////////
#if !defined(_FixStkRptFlowReq_h_)
#define _FixStkRptFlowReq_h_
//---------------------------------------------------------------------------
// ns
#include "wise_ptr.h"
// nsQFix
#include "src/Code/Message.h"
// nsQFixExt
//#include "QFixExt/QFixTagType.h"
// Fix.Stk
//#include "FixStkCoids.h"
#include "FixStkOrdCenter.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;

namespace FIX {
namespace Stk {
   class TTradeLog;
   class TRptFlow;

/////////////////////////////////////////////////////////////////////////////
// struct SRfBaseField
/////////////////////////////////////////////////////////////////////////////
struct SRfBaseField
{
   //==============================================
   // Required
   TFixOrderID    OrderID_;         // 037
   TFixOrdStatus  OrdStatus_;       // 039
   //==============================================
   // Optional
   TFixAccount      Account_;       // 001
   TFixClOrdID      ClOrdID_;       // 011
   TFixOrigClOrdID  OrigClOrdID_;   // 041
   TFixText         Text_;          // 058
   TFixStdString1   TransactTime_;  // 060
   //==============================================
   // Reserve
   TFixChar       ReseChar_[TFsStkOrderRec::Reserve_DataType_Size];

   inline K_mf(TFixChar*) GetReseChar ()  { return &(ReseChar_[0]); }
};
/////////////////////////////////////////////////////////////////////////////
// struct SRfErField
/////////////////////////////////////////////////////////////////////////////
struct SRfErField
{
   //==============================================
   // Required
   TFixPri2           AvgPx_;          // 006
   TFixQty2           CumQty_;         // 014
   TFixExecID         ExecID_;         // 017
   TFixExecTransType  ExecTransType_;  // 020
   //TFixSide           Side_;           // 054
   //TFixSymbol         Symbol_;         // 055
   TFixExecType       ExecType_;       // 150
   TFixQty2           LeavesQty_;      // 151
   //==============================================
   // Optional
   TFixPri2                   LastPx_;                // 031
   TFixQty2                   LastShares_;            // 032
   TFixQty2                   OrderQty_;              // 038
   TFixOrdType                OrdType_;               // 040
   TFixPri2                   Price_;                 // 044
   TFixOrdRejReason           OrdRejReason_;          // 103
   TFixExecRestatementReason  ExecRestatementReason_; // 378
};
/////////////////////////////////////////////////////////////////////////////
// struct SRfOcrField
/////////////////////////////////////////////////////////////////////////////
struct SRfOcrField
{
   //------------------------------
   // Required
   TFixCxlRejResponseTo  CxlRejResponseTo_;  // 434
   //------------------------------
   // Optional
   TFixCxlRejReason      CxlRejReason_;      // 102
};


/////////////////////////////////////////////////////////////////////////////
// class TRfReqBase  (回報要求物件基底)
/////////////////////////////////////////////////////////////////////////////
K_class TRfReqBase : public ref_counter<>
{
   friend class TRptFlow;

public:
   typedef TFsOrdCenter::TOrdHnd  TOrdHnd;

protected:
   TRptFlow*     RptFlow_;
   TFixRptCmdID  RCmd_;
   TFixMsgType2  MsgType_;
   TOrdHnd       OrdHnd_;
   TFixSysOrdID  SysOrdID_;
   Message       FixMsg_;
   std::string   LastMsg_;
   SRfBaseField  BaseField_;
   TTradeLog*    TradeLog_;

protected:
           K_ctor  TRfReqBase (EFixRptCmdID, const char* msgType, const TFixSysOrdID&);
   virtual K_dtor ~TRfReqBase ();

   //===========================================
   // TRfReqBase virtual function
   virtual K_mf(bool) VirOnUpdate () = 0;

public:
   K_mf(void) Update (const std::string&);

   inline K_mf(Message&)      GetFixMsg    ()  { return FixMsg_;    }
   inline K_mf(TOrdHnd&)      GetOrdHnd    ()  { return OrdHnd_;    }
   inline K_mf(SRfBaseField&) GetBaseField ()  { return BaseField_; }
   
   inline K_mf(const TFixSysOrdID&) GetSysOrdID  () const  { return SysOrdID_;  }
   inline K_mf(const TFixRptCmdID&) GetRptCmdID  () const  { return RCmd_;      }
   inline K_mf(const SRfBaseField&) GetBaseField () const  { return BaseField_; }
};

/////////////////////////////////////////////////////////////////////////////
// class TRfReqEr (ExecutionReport)
/////////////////////////////////////////////////////////////////////////////
K_class TRfReqErBase : public TRfReqBase
{
   typedef TRfReqBase  inherited;

   friend class TRptFlow;

protected:
   TFixOrdCmdID   OCmd_;
   SRfErField     ErField_;
   //TRfErFieldSet  RfErFieldSet_;
   
protected:
   K_ctor TRfReqErBase (EFixRptCmdID, const TFixSysOrdID&);
   K_ctor TRfReqErBase (EFixRptCmdID, const TFixSysOrdID&, EFixOrdCmdID);

   //===========================================
   // overwrite TRfReqBase virtual function
   K_mf(bool) VirOnUpdate ();

public:
   //inline K_mf(void) SetOrderQty  (const TFixQty& fixQty)  { ErField_.OrderQty_  = fixQty;  RfErFieldSet_.set(erf_Erf_OrderQty);  }
   inline K_mf(SRfErField&) GetErField ()  { return ErField_; }

   inline K_mf(const TFixOrdCmdID&) GetOrdCmdID () const  { return OCmd_;    }
   inline K_mf(const SRfErField&)   GetErField  () const  { return ErField_; }
};

/////////////////////////////////////////////////////////////////////////////
// class TRfReqErOrd (ExecutionReport Order)
/////////////////////////////////////////////////////////////////////////////
K_class TRfReqErOrdBase : public TRfReqErBase
{
   typedef TRfReqErBase  inherited;

   friend class TRptFlow;

   // 是否是修正狀態
   // ==> TwseFix 在切換到 DR-Side 時, 除了送 New::New 之外, 還會在送ㄧ次最後的結果
   //     所以使用此參數來達到重覆送的機制
   bool  EnableResend_;

protected:
   K_ctor TRfReqErOrdBase (const TFixSysOrdID&, EFixOrdCmdID);

   //===========================================
   // overwrite TRfReqErBase virtual function
   K_mf(bool) VirOnUpdate ();

   //===========================================
   // TRfReqErOrdBase function
   K_mf(bool) UpdateNew     ();
   K_mf(bool) UpdateDelete  ();
   K_mf(bool) UpdateReplace ();
   K_mf(bool) UpdateQuery   ();

public:
   inline K_mf(void) SetEnableResend (bool b)  { EnableResend_ = b; }
};

/////////////////////////////////////////////////////////////////////////////
// class TRfReqErReject (ExecutionReport Reject)
/////////////////////////////////////////////////////////////////////////////
K_class TRfReqErRejectBase : public TRfReqErBase
{
   typedef TRfReqErBase  inherited;

   friend class TRptFlow;

protected:
   int  RejectCode_;

public:
   inline K_mf(int) GetRejectCode () const  { return RejectCode_; }

protected:
   K_ctor TRfReqErRejectBase (const TFixSysOrdID&, EFixOrdCmdID oid, int rejCode);

   //===========================================
   // overwrite TRfReqErBase virtual function
   K_mf(bool) VirOnUpdate ();
};

/////////////////////////////////////////////////////////////////////////////
// class TRfReqErDeal (ExecutionReport Deal)
/////////////////////////////////////////////////////////////////////////////
K_class TRfReqErDealBase : public TRfReqErBase
{
   typedef TRfReqErBase  inherited;

   friend class TRptFlow;

protected:
   K_ctor TRfReqErDealBase (const TFixSysOrdID&);

   //===========================================
   // overwrite TRfReqErBase virtual function
   K_mf(bool) VirOnUpdate ();
};
   
/////////////////////////////////////////////////////////////////////////////
// class TRfReqOcr (Order Cancel Reject)
/////////////////////////////////////////////////////////////////////////////
K_class TRfReqOcrBase : public TRfReqBase
{
   typedef TRfReqBase  inherited;

   friend class TRptFlow;
   
   SRfOcrField  OcrField_;
   int          RejectCode_;

public:
   inline K_mf(SRfOcrField&) GetOcrField   ()        { return OcrField_;   }
   inline K_mf(int)          GetRejectCode () const  { return RejectCode_; }

protected:
   K_ctor TRfReqOcrBase (const TFixSysOrdID&, EFixOrdCmdID, int rejCode);

   //===========================================
   // overwrite TRfReqBase virtual function
   K_mf(bool) VirOnUpdate ();
};

/////////////////////////////////////////////////////////////////////////////
// class TRfReqReject
/////////////////////////////////////////////////////////////////////////////
K_class TRfReqRejectBase : public TRfReqBase
{
   typedef TRfReqBase  inherited;

   friend class TRptFlow;
   
   EFixOrdCmdID  OCmd_;
   int           RejectCode_;
   SRfErField    ErField_;
   SRfOcrField   OcrField_;

public:
   inline K_mf(SRfErField*)  GetErField  ()  { return &ErField_;  }
   inline K_mf(SRfOcrField*) GetOcrField ()  { return &OcrField_; }

protected:
   K_ctor TRfReqRejectBase (const TFixSysOrdID&, EFixOrdCmdID, int rejCode);

   //===========================================
   // overwrite TRfReqBase virtual function
   inline K_mf(bool) VirOnUpdate ()  { return true; }
};

/////////////////////////////////////////////////////////////////////////////
// wise_ref 資料型態
/////////////////////////////////////////////////////////////////////////////
typedef wise_ref<TRfReqBase>          TRfReqPtr;
typedef wise_ref<TRfReqErOrdBase>     TRfReqErOrd;
typedef wise_ref<TRfReqErRejectBase>  TRfReqErReject;
typedef wise_ref<TRfReqErDealBase>    TRfReqErDeal;
typedef wise_ref<TRfReqOcrBase>       TRfReqOcr;
typedef wise_ref<TRfReqRejectBase>    TRfReqReject;

/////////////////////////////////////////////////////////////////////////////
}; // namespace Stk
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_FixStkRptFlowReq_h_)
