/////////////////////////////////////////////////////////////////////////////
#if !defined(_QFixTagType_h_)
#define _QFixTagType_h_
//---------------------------------------------------------------------------
#include <string>
#include "QFixTagTypeBase.h"
#include "src/Code/Values.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;

namespace FIX  {
//---------------------------------------------------------------------------
class MsgType;

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// ©T©wªø«×
/////////////////////////////////////////////////////////////////////////////
// header
typedef TFixInt   TFixMsgSeqNum;    // 034
typedef TFixBool  TFixPossDupFlag;  // 043
//---------------------------------------------------------------------------
// body
//---------------------------------------------------------------------------
/*
// header
typedef TFixCaString< 20>  TFixOnBehalfOfCompID;   // 115
typedef TFixCaString< 20>  TFixDeliverToCompID;    // 128
// body
typedef TFixCaString< 30>  TFixAccount;      // 001
typedef TFixCaString< 40>  TFixClOrdID;      // 011
typedef TFixCaString<100>  TFixExecID;       // 017
typedef TFixCaString< 50>  TFixOrderID;      // 037
typedef TFixCaString< 40>  TFixOrigClOrdID;  // 041
typedef TFixCaString< 30>  TFixSymbol;       // 055
typedef TFixCaString<100>  TFixText;         // 058
typedef TFixCaString< 50>  TFixRawData;      // 096
*/
//---------------------------------------------------------------------------
//typedef TFixQty            TFixCumQty;       // 014
//typedef TFixQty            TFixLastShares;   // 032
//typedef TFixQty            TFixOrderQty;     // 038
//typedef TFixQty            TFixLeavesQty;    // 151
//---------------------------------------------------------------------------
//typedef TFixPri            TFixPrice;        // 044


/////////////////////////////////////////////////////////////////////////////
// class TFixBeginString (8) (String)
/////////////////////////////////////////////////////////////////////////////
enum EFixBeginString
{
   ebs_Unknown,
      ebs_Fix40,
      ebs_Fix41,
      ebs_Fix42,
      ebs_Fix43,
      ebs_Fix44,
   ebs_Total,
   
   ebs_Start = (ebs_Unknown + 1),
};
//---------------------------------------------------------------------------
K_class TFixBeginStringInfo
{
public:
   static K_mf(const char**) GetItemList ();

   static inline K_mf(EFixBeginString) GetEUnknown ()  { return ebs_Unknown; }
   static inline K_mf(EFixBeginString) GetEStart   ()  { return ebs_Start;   }
   static inline K_mf(EFixBeginString) GetETotal   ()  { return ebs_Total;   }
};
//---------------------------------------------------------------------------
class TFixBeginString : public TFixFmtStr<EFixBeginString, TFixBeginStringInfo>
{
   typedef TFixFmtStr<EFixBeginString, TFixBeginStringInfo>  inherited;

public:
   inline explicit K_ctor TFixBeginString (EFixBeginString e = ebs_Unknown) : inherited(e)    {}
   inline explicit K_ctor TFixBeginString (int             val)                               { assign_int(val); }

   K_mf(void) assign_int (int val)
   {
      Data_ = ebs_Unknown;
      switch (val) {
         case 42:  Data_ = ebs_Fix42;  break;
         case 44:  Data_ = ebs_Fix44;  break;
         case 43:  Data_ = ebs_Fix43;  break;
         case 41:  Data_ = ebs_Fix41;  break;
         case 40:  Data_ = ebs_Fix40;  break;
      }
   }
};


/////////////////////////////////////////////////////////////////////////////
// class TFixMsgType2 (35)
/////////////////////////////////////////////////////////////////////////////
// A.MsgType_Logon
// 5.MsgType_Logout
// D.MsgType_NewOrderSingle
// F.MsgType_OrderCancelRequest
// G.MsgType_OrderCancelReplaceRequest
// H.MsgType_OrderStatusRequest
// 8.MsgType_ExecutionReport
// 9.MsgType_OrderCancelReject
// j.MsgType_BusinessMessageReject
K_class TFixMsgType2
{
   typedef TFixMsgType2          this_type;
   typedef char_ary<char, 3, 0>  TData;

   TData  Data_;

public:
   inline          K_ctor TFixMsgType2 ()                            {}
   inline explicit K_ctor TFixMsgType2 (const char*        str)      { assign(str);     }
   inline explicit K_ctor TFixMsgType2 (const std::string& str)      { assign(str);     }
   inline          K_ctor TFixMsgType2 (const MsgType&     msgType)  { assign(msgType); }

   K_mf(void) assign (const MsgType&);
   
   K_mf(bool) is_admin () const;
   K_mf(bool) is_app   () const;

   inline K_mf(void) assign (const char*        str)  { Data_.assign(str); }
   inline K_mf(void) assign (const std::string& str)  { Data_.assign(str); }
   inline K_mf(void) clear  ()                        { Data_.clear();     }

   inline K_mf(std::string) as_string  ()                const  { return Data_.as_string(); }
   inline K_mf(bool)        is_invalid ()                const  { return Data_.empty    (); }
   //inline K_mf(bool)        is_equal   (const char* str) const  { return (strcmp(Data_.begin(), str) == 0); }

   inline K_mf(bool) operator <  (const this_type& a) const  { return (Data_ <  a.Data_); }
   inline K_mf(bool) operator == (const this_type& a) const  { return (Data_ == a.Data_); }
   inline K_mf(bool) operator != (const this_type& a) const  { return (Data_ != a.Data_); }

   inline K_mf(bool) operator == (const char* str) const  { return (strcmp(Data_.begin(), str) == 0); }
   inline K_mf(bool) operator != (const char* str) const  { return (strcmp(Data_.begin(), str) != 0); }
};


/////////////////////////////////////////////////////////////////////////////
// class TFixMsgType1 (35)
/////////////////////////////////////////////////////////////////////////////
enum EFixMsgType1
{
   emt_Unknown,
      emt_Heartbeat,                            // "0"
      emt_Logon,                                // "A"
      emt_TestRequest,                          // "1"
      emt_ResendRequest,                        // "2"
      emt_Reject,                               // "3"
      emt_SequenceReset,                        // "4"
      emt_Logout,                               // "5"
      emt_BusinessMessageReject,                // "j"
      emt_NewOrderSingle,                       // "D"
      emt_ExecutionReport,                      // "8"
      emt_DontKnowTrade,                        // "Q"
      emt_OrderCancelReplaceRequest,            // "G"
      emt_OrderCancelRequest,                   // "F"
      emt_OrderCancelReject,                    // "9"
      emt_OrderStatusRequest,                   // "H"
      emt_TradingSessionStatusRequest,          // "g"
      emt_TradingSessionStatus,                 // "h"
      emt_UserRequest,                          // "BE"
      emt_UserResponse,                         // "BF"
      emt_Advertisement,                        // "7"
      emt_IndicationOfInterest,                 // "6"
      emt_News,                                 // "B"
      emt_Email,                                // "C"
      emt_QuoteRequest,                         // "R"
      emt_QuoteResponse,                        // "AJ"
      emt_QuoteRequestReject,                   // "AG"
      emt_RFQRequest,                           // "AH"
      emt_Quote,                                // "S"
      emt_QuoteCancel,                          // "Z"
      emt_QuoteStatusRequest,                   // "a"
      emt_QuoteStatusReport,                    // "AI"
      emt_MassQuote,                            // "i"
      emt_MassQuoteAcknowledgement,             // "b"
      emt_MarketDataRequest,                    // "V"
      emt_MarketDataSnapshotFullRefresh,        // "W"
      emt_MarketDataIncrementalRefresh,         // "X"
      emt_MarketDataRequestReject,              // "Y"
      emt_SecurityDefinitionRequest,            // "c"
      emt_SecurityDefinition,                   // "d"
      emt_SecurityTypeRequest,                  // "v"
      emt_SecurityTypes,                        // "w"
      emt_SecurityListRequest,                  // "x"
      emt_SecurityList,                         // "y"
      emt_DerivativeSecurityListRequest,        // "z"
      emt_DerivativeSecurityList,               // "AA"
      emt_SecurityStatusRequest,                // "e"
      emt_SecurityStatus,                       // "f"
      emt_OrderMassCancelRequest,               // "q"
      emt_OrderMassCancelReport,                // "r"
      emt_OrderMassStatusRequest,               // "AF"
      emt_NewOrderCross,                        // "s"
      emt_CrossOrderCancelReplaceRequest,       // "t"
      emt_CrossOrderCancelRequest,              // "u"
      emt_NewOrderMultileg,                     // "AB"
      emt_MultilegOrderCancelReplaceRequest,    // "AC"
      emt_BidRequest,                           // "k"
      emt_BidResponse,                          // "l"
      emt_NewOrderList,                         // "E"
      emt_ListStrikePrice,                      // "m"
      emt_ListStatus,                           // "N"
      emt_ListExecute,                          // "L"
      emt_ListCancelRequest,                    // "K"
      emt_ListStatusRequest,                    // "M"
      emt_AllocationInstruction,                // "J"
      emt_AllocationInstructionAck,             // "P"
      emt_AllocationReport,                     // "AS"
      emt_AllocationReportAck,                  // "AT"
      emt_Confirmation,                         // "AK"
      emt_ConfirmationAck,                      // "AU"
      emt_ConfirmationRequest,                  // "BH"
      emt_SettlementInstructions,               // "T"
      emt_SettlementInstructionRequest,         // "AV"
      emt_TradeCaptureReportRequest,            // "AD"
      emt_TradeCaptureReportRequestAck,         // "AQ"
      emt_TradeCaptureReport,                   // "AE"
      emt_TradeCaptureReportAck,                // "AR"
      emt_RegistrationInstructions,             // "o"
      emt_RegistrationInstructionsResponse,     // "p"
      emt_PositionMaintenanceRequest,           // "AL"
      emt_PositionMaintenanceReport,            // "AM"
      emt_RequestForPositions,                  // "AN"
      emt_RequestForPositionsAck,               // "AO"
      emt_PositionReport,                       // "AP"
      emt_AssignmentReport,                     // "AW"
      emt_CollateralRequest,                    // "AX"
      emt_CollateralAssignment,                 // "AY"
      emt_CollateralResponse,                   // "AZ"
      emt_CollateralReport,                     // "BA"
      emt_CollateralInquiry,                    // "BB"
      emt_NetworkStatusRequest,                 // "BC"
      emt_NetworkStatusResponse,                // "BD"
      emt_CollateralInquiryAck,                 // "BG"
   emt_Total,

   emt_Start = (emt_Unknown + 1),
};
//---------------------------------------------------------------------------
K_class TFixMsgType1
{
   EFixMsgType1  Data_;

public:
   inline          K_ctor TFixMsgType1 (EFixMsgType1 e=emt_Unknown) : Data_(e)  {}
   inline explicit K_ctor TFixMsgType1 (const std::string& str)                 { assign(str); }
                   K_ctor TFixMsgType1 (const FIX::MsgType&);

   K_mf(void) assign (const char*);

   K_mf(std::string) as_string () const;
   K_mf(std::string) as_desc   () const;

   inline K_mf(void) assign  (const std::string& str)  { assign(str.c_str()); }
   inline K_mf(void) assignT (EFixMsgType1 e)          { Data_ = e;           }
   inline K_mf(void) clear   ()                        { Data_ = emt_Unknown; }

   inline K_mf(EFixMsgType1) get_orig   () const  { return Data_;                  }
   inline K_mf(std::string)  as_fix     () const  { return as_string();            }
   inline K_mf(bool)         is_invalid () const  { return (Data_ == emt_Unknown); }

   inline K_mf(bool) operator <  (const TFixMsgType1& a) const  { return (Data_ <  a.Data_); }
   inline K_mf(bool) operator == (const TFixMsgType1& a) const  { return (Data_ == a.Data_); }

   inline K_mf(bool) operator ==           (EFixMsgType1 e) const  { return (Data_ == e); }
   inline K_mf(bool) operator !=           (EFixMsgType1 e) const  { return (Data_ != e); }
   inline            operator EFixMsgType1 ()               const  { return Data_;        }
};


/////////////////////////////////////////////////////////////////////////////
// class TFixExecTransType (20) (char)
/////////////////////////////////////////////////////////////////////////////
// 0.ExecTransType_NEW
// 1.ExecTransType_CANCEL
// 2.ExecTransType_CORRECT
// 3.ExecTransType_STATUS
//---------------------------------------------------------------------------
K_class TFixExecTransType : public TFixChar
{
public:
   typedef TFixChar              inherited;
   typedef inherited::orig_type  orig_type;

public:
   inline K_ctor TFixExecTransType (orig_type val = inherited::Null_Val) : inherited(val)  {}

   K_mf(std::string) as_desc () const;
};


/////////////////////////////////////////////////////////////////////////////
// class TFixHandlInst (21) (char)
/////////////////////////////////////////////////////////////////////////////
// 1.HandlInst_AUTOMATED_EXECUTION_ORDER_PRIVATE
// 2.HandlInst_AUTOMATED_EXECUTION_ORDER_PUBLIC
// 3.HandlInst_MANUAL_ORDER
//---------------------------------------------------------------------------
K_class TFixHandlInst : public TFixChar
{
public:
   typedef TFixChar              inherited;
   typedef inherited::orig_type  orig_type;

public:
   inline K_ctor TFixHandlInst (orig_type val = inherited::Null_Val) : inherited(val)  {}

   K_mf(std::string) as_desc () const;
};


/////////////////////////////////////////////////////////////////////////////
// class TFixOrdStatus (39) (char)
/////////////////////////////////////////////////////////////////////////////
// 0.OrdStatus_NEW
// 1.OrdStatus_PARTIALLY_FILLED
// 2.OrdStatus_FILLED
// 3.OrdStatus_DONE_FOR_DAY
// 4.OrdStatus_CANCELED
// 5.OrdStatus_REPLACED
// 6.OrdStatus_PENDING_CANCEL
// 7.OrdStatus_STOPPED
// 8.OrdStatus_REJECTED
// 9.OrdStatus_SUSPENDED
// A.OrdStatus_PENDING_NEW
// B.OrdStatus_CALCULATED
// C.OrdStatus_EXPIRED
// D.OrdStatus_ACCEPTED_FOR_BIDDING
// E.OrdStatus_PENDING_REPLACE
//---------------------------------------------------------------------------
K_class TFixOrdStatus : public TFixChar
{
public:
   typedef TFixChar              inherited;
   typedef inherited::orig_type  orig_type;

public:
   inline K_ctor TFixOrdStatus (orig_type val = inherited::Null_Val) : inherited(val)  {}

   K_mf(std::string) as_desc () const;
};


/////////////////////////////////////////////////////////////////////////////
// class TFixOrdType (40) (char)
/////////////////////////////////////////////////////////////////////////////
// 1.OrdType_MARKET
// 2.OrdType_LIMIT
// 3.OrdType_STOP
// 4.OrdType_STOP_LIMIT
// 5.OrdType_MARKET_ON_CLOSE
// 6.OrdType_WITH_OR_WITHOUT
// 7.OrdType_LIMIT_OR_BETTER
// 8.OrdType_LIMIT_WITH_OR_WITHOUT
//---------------------------------------------------------------------------
K_class TFixOrdType : public TFixChar
{
public:
   typedef TFixChar              inherited;
   typedef inherited::orig_type  orig_type;

public:
   inline K_ctor TFixOrdType (orig_type val = inherited::Null_Val) : inherited(val)  {}

   K_mf(std::string) as_desc () const;
};


/////////////////////////////////////////////////////////////////////////////
// class TFixSide (54) (char)
/////////////////////////////////////////////////////////////////////////////
// 1.Side_BUY
// 2.Side_SELL
// 3.Side_BUY_MINUS
// 4.Side_SELL_PLUS
// 5.Side_SELL_SHORT
// 6.Side_SELL_SHORT_EXEMPT
// 7.Side_UNDISCLOSED
// 8.Side_CROSS
// 9.Side_CROSS_SHORT
// A.Side_CROSS_SHORT_EXEMPT
// B.Side_AS_DEFINED
// C.Side_OPPOSITE
// D.Side_SUBSCRIBE
// E.Side_REDEEM
// F.Side_LEND
// G.Side_BORROW
//---------------------------------------------------------------------------
K_class TFixSide : public TFixChar
{
public:
   typedef TFixChar              inherited;
   typedef inherited::orig_type  orig_type;

public:
   inline K_ctor TFixSide (orig_type val = inherited::Null_Val) : inherited(val)  {}

   K_mf(std::string) as_desc () const;
};


/////////////////////////////////////////////////////////////////////////////
// class TFixTimeInForce (59) (char)
/////////////////////////////////////////////////////////////////////////////
// 0.TimeInForce_DAY
// 1.TimeInForce_GOOD_TILL_CANCEL
// 2.TimeInForce_AT_THE_OPENING
// 3.TimeInForce_IMMEDIATE_OR_CANCEL
// 4.TimeInForce_FILL_OR_KILL
// 5.TimeInForce_GOOD_TILL_CROSSING
// 6.TimeInForce_GOOD_TILL_DATE
// 7.TimeInForce_AT_THE_CLOSE
//---------------------------------------------------------------------------
K_class TFixTimeInForce : public TFixChar
{
public:
   typedef TFixChar              inherited;
   typedef inherited::orig_type  orig_type;

public:
   inline K_ctor TFixTimeInForce (orig_type val = inherited::Null_Val) : inherited(val)  {}

   K_mf(std::string) as_desc () const;
};


/////////////////////////////////////////////////////////////////////////////
// class TFixCxlRejReason (102) (int)
/////////////////////////////////////////////////////////////////////////////
//  0.CxlRejReason_TOO_LATE_TO_CANCEL
//  1.CxlRejReason_UNKNOWN_ORDER
//  2.CxlRejReason_BROKER_EXCHANGE_OPTION
//  3.CxlRejReason_ORDER_ALREADY_IN_PENDING_CANCEL_OR_PENDING_REPLACE_STATUS
//  4.CxlRejReason_UNABLE_TO_PROCESS_ORDER_MASS_CANCEL_REQUEST
//  5.CxlRejReason_ORIGORDMODTIME_DID_NOT_MATCH_LAST_TRANSACTTIME_OF_ORDER
//  6.CxlRejReason_DUPLICATE_CLORDID_RECEIVED
// 99.CxlRejReason_OTHER
//---------------------------------------------------------------------------
K_class TFixCxlRejReason : public TFixInt
{
public:
   typedef TFixInt               inherited;
   typedef inherited::orig_type  orig_type;

public:
   inline K_ctor TFixCxlRejReason (orig_type val = inherited::Null_Val) : inherited(val)  {}

   K_mf(std::string) as_desc () const;
};


/////////////////////////////////////////////////////////////////////////////
// class TFixOrdRejReason (103) (int)
/////////////////////////////////////////////////////////////////////////////
//  0.OrdRejReason_BROKER_EXCHANGE_OPTION
//  1.OrdRejReason_UNKNOWN_SYMBOL
//  2.OrdRejReason_EXCHANGE_CLOSED
//  3.OrdRejReason_ORDER_EXCEEDS_LIMIT
//  4.OrdRejReason_TOO_LATE_TO_ENTER
//  5.OrdRejReason_UNKNOWN_ORDER
//  6.OrdRejReason_DUPLICATE_ORDER
//  7.OrdRejReason_DUPLICATE_OF_A_VERBALLY_COMMUNICATED_ORDER
//  8.OrdRejReason_STALE_ORDER
//  9.OrdRejReason_TRADE_ALONG_REQUIRED
// 10.OrdRejReason_INVALID_INVESTOR_ID
// 11.OrdRejReason_UNSUPPORTED_ORDER_CHARACTERISTIC
// 12.OrdRejReason_SURVEILLENCE_OPTION
// 13.OrdRejReason_INCORRECT_QUANTITY
// 14.OrdRejReason_INCORRECT_ALLOCATED_QUANTITY
// 15.OrdRejReason_UNKNOWN_ACCOUNT
// 99.OrdRejReason_OTHER
//---------------------------------------------------------------------------
K_class TFixOrdRejReason : public TFixInt
{
public:
   typedef TFixInt               inherited;
   typedef inherited::orig_type  orig_type;

public:
   inline K_ctor TFixOrdRejReason (orig_type val = inherited::Null_Val) : inherited(val)  {}
      
   K_mf(std::string) as_desc () const;
};


/////////////////////////////////////////////////////////////////////////////
// class TFixExecType (150) (char)
/////////////////////////////////////////////////////////////////////////////
// 0.ExecType_NEW
// 1.ExecType_PARTIAL_FILL
// 2.ExecType_FILL
// 3.ExecType_DONE_FOR_DAY
// 4.ExecType_CANCELED
// 5.ExecType_REPLACE
// 6.ExecType_PENDING_CANCEL
// 7.ExecType_STOPPED
// 8.ExecType_REJECTED
// 9.ExecType_SUSPENDED
// A.ExecType_PENDING_NEW
// B.ExecType_CALCULATED
// C.ExecType_EXPIRED
// D.ExecType_RESTATED
// E.ExecType_PENDING_REPLACE
// F.ExecType_TRADE
// G.ExecType_TRADE_CORRECT
// H.ExecType_TRADE_CANCEL
// I.ExecType_ORDER_STATUS
//---------------------------------------------------------------------------
K_class TFixExecType : public TFixChar
{
public:
   typedef TFixChar              inherited;
   typedef inherited::orig_type  orig_type;

public:
   inline K_ctor TFixExecType (orig_type ch = inherited::Null_Val) : inherited(ch)  {}

   K_mf(std::string) as_desc () const;

   inline K_mf(bool) IsPending () const
      {
         return ((inherited::Data_ == ExecType_PENDING_CANCEL ||
                  inherited::Data_ == ExecType_PENDING_NEW    ||
                  inherited::Data_ == ExecType_PENDING_REPLACE) ? true : false);
      }
};


/////////////////////////////////////////////////////////////////////////////
// class TFixExecRestatementReason (378) (int)
/////////////////////////////////////////////////////////////////////////////
// 0.ExecRestatementReason_GT_CORPORATE_ACTION
// 1.ExecRestatementReason_GT_RENEWAL_RESTATEMENT
// 2.ExecRestatementReason_VERBAL_CHANGE
// 3.ExecRestatementReason_REPRICING_OF_ORDER
// 4.ExecRestatementReason_BROKER_OPTION
// 5.ExecRestatementReason_PARTIAL_DECLINE_OF_ORDERQTY
// 6.ExecRestatementReason_CANCEL_ON_TRADING_HALT
// 7.ExecRestatementReason_CANCEL_ON_SYSTEM_FAILURE
// 8.ExecRestatementReason_MARKET_OPTION
// 9.ExecRestatementReason_CANCELED_NOT_BEST
//---------------------------------------------------------------------------
K_class TFixExecRestatementReason : public TFixInt
{
public:
   typedef TFixInt               inherited;
   typedef inherited::orig_type  orig_type;

public:
   inline K_ctor TFixExecRestatementReason (orig_type val = inherited::Null_Val) : inherited(val)  {}

   K_mf(std::string) as_desc () const;
};


/////////////////////////////////////////////////////////////////////////////
// class TFixCxlRejResponseTo (434) (char)
/////////////////////////////////////////////////////////////////////////////
// 1.CxlRejResponseTo_ORDER_CANCEL_REQUEST
// 2.CxlRejResponseTo_ORDER_CANCEL_REPLACE_REQUEST
//---------------------------------------------------------------------------
K_class TFixCxlRejResponseTo : public TFixChar
{
public:
   typedef TFixChar              inherited;
   typedef inherited::orig_type  orig_type;

public:
   inline K_ctor TFixCxlRejResponseTo (orig_type ch = inherited::Null_Val) : inherited(ch)  {}

   K_mf(std::string) as_desc () const;
};


/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif

/////////////////////////////////////////////////////////////////////////////
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_QFixTagType_h_)
