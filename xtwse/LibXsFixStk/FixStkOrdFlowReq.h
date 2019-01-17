/////////////////////////////////////////////////////////////////////////////
#if !defined(_FixStkOrdFlowReq_h_)
#define _FixStkOrdFlowReq_h_
//---------------------------------------------------------------------------
// ns
#include "wise_ptr.h"
// nsQFix
#include "src/Code/Message.h"
// nsQFixExt
#include "QFixExt/QFixTagType.h"
// Fix.Stk
#include "FixStkCoids.h"
#include "FixStkOrdCenter.h"
#include "FixStkOrdTrack.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;

namespace FIX {
namespace Stk {
   class TFsOrdFlow;

/////////////////////////////////////////////////////////////////////////////
// struct SOfOrdField
/////////////////////////////////////////////////////////////////////////////
enum EOfOrdField
{
   eof_Start,
      eof_SenderSubID = eof_Start,
      eof_TargetSubID,
      eof_Account,
      eof_ClOrdID,
      eof_OrderID,
      eof_OrderQty,
      eof_OrdType,
      eof_OrigClOrdID,
      eof_Price,
      eof_Side,
      eof_Symbol,
   eof_Total
};
//---------------------------------------------------------------------------
typedef std::bitset<eof_Total>  TOfOrdFieldSet;
//---------------------------------------------------------------------------
struct SOfOrdField
{
   // header
   TFixMsgSeqNum    MsgSeqNum_;     // 034
   TFixPossDupFlag  PossDupFlag_;   // 043
   TFixSenderSubID  SenderSubID_;   // 050
   TFixTargetSubID  TargetSubID_;   // 057
   // body
   TFixAccount      Account_;       // 001
   TFixClOrdID      ClOrdID_;       // 011
   TFixOrderID      OrderID_;       // 037
   TFixStdString1   OrderQty_;      // 038
   TFixOrdType      OrdType_;       // 040
   TFixOrigClOrdID  OrigClOrdID_;   // 041
   TFixStdString1   Price_;         // 044
   TFixSide         Side_;          // 054
   TFixSymbol       Symbol_;        // 055
   // 自訂部份
   TFixChar         TwseIvacnoFlag_; // 10000
   TFixChar         TwseOrdType_;    // 10001
   TFixChar         TwseExCode_;     // 10002
};
/////////////////////////////////////////////////////////////////////////////
// struct SOfRnoField
/////////////////////////////////////////////////////////////////////////////
struct SOfRnoField
{
   TFixStdString1  TransactTime_;  // 060
};


/////////////////////////////////////////////////////////////////////////////
// class TOfReqBase  (委託要求物件基底)
/////////////////////////////////////////////////////////////////////////////
K_class TOfReqBase : public ref_counter<>
{
   friend class TFsOrdFlow;
      
public:
   typedef TFsOrdCenter::TOrdHnd  TOrdHnd;

protected:
   TFixOrdCmdID   OCmd_;
   TOrdHnd        OrdHnd_;
   TFixOrdFlowID  FlowID_;
   std::string    LastMsg_;
   TFixMsgType2   FixMsgType_;
   TMessageRef    FixMsgRef_;
   TFsOrdFlow*    OrdFlow_;
   
   // 保留原始的資料, 這樣在做資料檢核時就不需要在去 FixMsg 裡面去查了
   // 因為這些資料不是字串, 在塞進 TFsStkOrder 就沒辦法判斷了
   SOfOrdField  OrdField_;
   
   // 紀錄 ClOrdID 的資料表, 存放 updater 可以加快速度
   // 不需要先 find 在 insert
   TCoids::TRecsU  CoidU_;

   // 委託歷程資料, 在呼叫 Update 時會新增到 OrdTrack_ 裡面
   TOrdTrack::TRsSeqNoU  OTrackU_;

   // 在前置作業時,就發現這筆委託是失敗的
   // 但是不行在 OrdFlow 中就中斷,因為想要通知後端接收者
   // Ex:台灣證交所 - 在Failover收到回補委託,但這些委託要回覆錯誤
   int  InitRejCode_;

   // 將 Fix 電文裡面的 SendingTime(52) 放進來
   TKDateTimeMS  SendingTime_;

protected:
           K_ctor  TOfReqBase (EFixOrdCmdID);
           K_ctor  TOfReqBase (EFixOrdCmdID, const char*, const TMessageRef&);
   virtual K_dtor ~TOfReqBase ();

public:
   //===========================================
   // TOfReqBase virtual function
   virtual K_mf(void) Update (const std::string&, EFixOrdFlowID);

   //===========================================
   // TOfReqBase function
   
   //===========================================
   // TOfReqBase inline function
   inline K_mf(void) SetOrdHnd      (TOrdHnd& a)     { OrdHnd_      = a;    }
   inline K_mf(void) SetInitRejCode (int      code)  { InitRejCode_ = code; }
   
   // GetOrdTrack
   inline K_mf(SOrdTrack*) GetOrdTrack ()
      { return (OTrackU_.is_invalid() ? NULL : &(*OTrackU_)); }
   
   // GetCoid
   inline K_mf(SCoid*) GetCoid ()
      { return (CoidU_.is_invalid() ? NULL : &(*CoidU_)); }

   // GetSysOrdID
   inline K_mf(TFixSysOrdID) GetSysOrdID () const
      { return (OrdHnd_.is_invalid() ? TFixSysOrdID() : OrdHnd_.GetSysOrdID()); }

   inline K_mf(TOrdHnd&)    GetOrdHnd    ()  { return OrdHnd_;     }
   inline K_mf(Message&)    GetFixMsg    ()  { return *FixMsgRef_; }
   inline K_mf(TMessageRef) GetFixMsgRef ()  { return FixMsgRef_;  }

   inline K_mf(const TFixOrdCmdID&) GetOrdCmdID    () const  { return OCmd_;        }
   inline K_mf(const TFixMsgType2&) GetFixMsgType  () const  { return FixMsgType_;  }
   inline K_mf(const SOfOrdField&)  GetOrdField    () const  { return OrdField_;    }
   inline K_mf(const Message&)      GetFixMsg      () const  { return *FixMsgRef_;  }
   inline K_mf(int)                 GetInitRejCode () const  { return InitRejCode_; }
   inline K_mf(const TKDateTimeMS&) GetSendingTime () const  { return SendingTime_; }
};

/////////////////////////////////////////////////////////////////////////////
// class TOfReqNosBase  (NewOrderSingle)
/////////////////////////////////////////////////////////////////////////////
K_class TOfReqNosBase : public TOfReqBase
{
   typedef TOfReqBase  inherited;

   friend class TFsOrdFlow;

protected:
   inline K_ctor TOfReqNosBase (const TMessageRef& msgRef)
      : inherited(fcid_New, MsgType_NewOrderSingle, msgRef)
   {}
};

/////////////////////////////////////////////////////////////////////////////
// class TOfReqOldBase  (OrderCancelRequest, OrderCancelReplaceRequest, OrderStatusRequest)
/////////////////////////////////////////////////////////////////////////////
K_class TOfReqOldBase : public TOfReqBase
{
   typedef TOfReqBase  inherited;

   friend class TFsOrdFlow;

protected:
   // 在前置作業時,發現有些 tag 資料錯誤
   int  WorkRejCode_;

protected:
   K_ctor TOfReqOldBase (EFixOrdCmdID, const char* msgType, const TMessageRef&);

public:
   inline K_mf(void) SetWorkRejCode (int n)  { WorkRejCode_ = n; }

   inline K_mf(int) GetWorkRejCode () const  { return WorkRejCode_; }
};

/////////////////////////////////////////////////////////////////////////////
// class TOfReqOcrBase  (OrderCancelRequest)
/////////////////////////////////////////////////////////////////////////////
K_class TOfReqOcrBase : public TOfReqOldBase
{
   typedef TOfReqOldBase  inherited;

   friend class TFsOrdFlow;

public:
   //==============================================
   // overwrite TOfReqBase virtual function
   K_mf(void) Update (const std::string&, EFixOrdFlowID);

protected:
   K_ctor TOfReqOcrBase (const TMessageRef&);
};

/////////////////////////////////////////////////////////////////////////////
// class TOfReqOcrrBase (OrderCancelReplaceRequest)
/////////////////////////////////////////////////////////////////////////////
K_class TOfReqOcrrBase : public TOfReqOldBase
{
   typedef TOfReqOldBase  inherited;

   friend class TFsOrdFlow;

protected:
   TFixQty2  NewOrderQty_;
   TFixPri2  NewPrice_;

public:
   //==============================================
   // overwrite TOfReqBase virtual function
   K_mf(void) Update (const std::string&, EFixOrdFlowID);

   //==============================================
   // TReqOcrr function
   inline K_mf(const TFixQty2&) GetNewOrderQty () const  { return NewOrderQty_; }
   inline K_mf(const TFixPri2&) GetNewPrice    () const  { return NewPrice_;    }

protected:
   K_ctor TOfReqOcrrBase (const TMessageRef&);
};

/////////////////////////////////////////////////////////////////////////////
// class TOfReqOsrBase  (OrderStatusRequest)
/////////////////////////////////////////////////////////////////////////////
K_class TOfReqOsrBase : public TOfReqOldBase
{
   typedef TOfReqOldBase  inherited;

   friend class TFsOrdFlow;

public:
   //==============================================
   // overwrite TOfReqBase virtual function
   K_mf(void) Update (const std::string&, EFixOrdFlowID);

   //==============================================
   // TOfReqOsrBase function

protected:
   K_ctor TOfReqOsrBase (const TMessageRef&);
};
   
/////////////////////////////////////////////////////////////////////////////
// class TOfReqAnoBase  (AutoNewOrder)
/////////////////////////////////////////////////////////////////////////////
// 說明: 沒有接收過新單委託,若此時要可以處理該筆的刪/改單時
//       就必須先自動產生一筆新單委託,這樣流程才可以跑下去
K_class TOfReqAnoBase : public TOfReqBase
{
   typedef TOfReqBase  inherited;

   friend class TFsOrdFlow;

protected:
   TFixRefOrdID  RefOrdID_;
   
   // 是否是有效的委託
   // 如果不是的話, Adpt 裡面也許可以不用處理
   // Ex: Twse Fix
   // ==> 缺少 TargetSubID 時, 就可以不用新增到 TwStkOrdCenter 裡面
   bool          IsValidOrd_;

public:
   //===========================================
   // overwrite TOfReqBase virtual function
   K_mf(void) Update (const std::string&, EFixOrdFlowID);

   //===========================================
   // TOfReqAnoBase function
   inline K_mf(void) SetRefOrdID (const TFixRefOrdID& a)  { RefOrdID_ = a; }
   
   inline K_mf(const TFixRefOrdID&) GetRefOrdID () const  { return RefOrdID_;   }
   inline K_mf(bool)                IsValidOrd  () const  { return IsValidOrd_; }

protected:
   K_ctor TOfReqAnoBase (const TMessageRef&, TFixRefOrdID, bool isValidOrd);
};

/////////////////////////////////////////////////////////////////////////////
// class TOfReqRnoBase  (Rpt New Order)
/////////////////////////////////////////////////////////////////////////////
// 說明: 從外部新增一筆委託
//       目前的設計都是利用 Fix 電文來產生委託
//       如果想要從非 Fix 管道產生新委託時就要靠它了
K_class TOfReqRnoBase : public TOfReqBase
{
   typedef TOfReqBase  inherited;

   friend class TFsOrdFlow;

protected:
   TFsStkOrder  StkOrd_;
   SOfRnoField  RnoField_;

public:
   //===========================================
   // overwrite TOfReqBase virtual function
   K_mf(void) Update (const std::string&, EFixOrdFlowID);

   //===========================================
   // TOfReqRnoBase function
   inline K_mf(TFsStkOrder&) GetStkOrd   ()  { return StkOrd_;   }
   inline K_mf(SOfRnoField&) GetRnoField ()  { return RnoField_; }

protected:
   K_ctor TOfReqRnoBase ();
};


/////////////////////////////////////////////////////////////////////////////
// wise_ref 資料型態
/////////////////////////////////////////////////////////////////////////////
typedef wise_ref<TOfReqBase>      TOfReqPtr;
typedef wise_ref<TOfReqNosBase>   TOfReqNos;
typedef wise_ref<TOfReqOcrBase>   TOfReqOcr;
typedef wise_ref<TOfReqOcrrBase>  TOfReqOcrr;
typedef wise_ref<TOfReqOsrBase>   TOfReqOsr;
typedef wise_ref<TOfReqAnoBase>   TOfReqAno;
typedef wise_ref<TOfReqRnoBase>   TOfReqRno;
      
/////////////////////////////////////////////////////////////////////////////
}; // namespace Stk
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_FixStkOrdFlowReq_h_)
