/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "QFixTagType.h"
#include "src/Code/Values.h"
#include "src/Code/DeprecatedValues.h"
#include "src/Code/Fields.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::FIX;
//---------------------------------------------------------------------------
static const char  Null_Char   = 0;
static const char* Null_String = "";

/////////////////////////////////////////////////////////////////////////////
// class TFixBeginStringInfo (8) (String) impl
/////////////////////////////////////////////////////////////////////////////
K_mf(const char**) TFixBeginStringInfo::GetItemList ()
{
   static const char* Item_List[] = {
      Null_String,
      BeginString_FIX40,
      BeginString_FIX41,
      BeginString_FIX42,
      BeginString_FIX43,
      BeginString_FIX44,
   };

   return Item_List;
};


/////////////////////////////////////////////////////////////////////////////
// class TFixMsgType2 (35) impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TFixMsgType2::assign (const MsgType& msgType)
{
   assign(msgType.getValue().c_str());
}
//---------------------------------------------------------------------------

K_mf(bool) TFixMsgType2::is_admin () const
{
   const char* str = Data_.begin();
   
   if (strcmp(str, MsgType_Heartbeat)     == 0 ||
       strcmp(str, MsgType_Logon)         == 0 ||
       strcmp(str, MsgType_TestRequest)   == 0 ||
       strcmp(str, MsgType_ResendRequest) == 0 ||
       strcmp(str, MsgType_Reject)        == 0 ||
       strcmp(str, MsgType_SequenceReset) == 0 ||
       strcmp(str, MsgType_Logout)        == 0    )
   {
      return true;
   }
   return false;
}
//---------------------------------------------------------------------------

K_mf(bool) TFixMsgType2::is_app () const
{
   return (!Data_.empty() && !is_admin());
}


/////////////////////////////////////////////////////////////////////////////
// class TFixMsgType1 (35) impl
/////////////////////////////////////////////////////////////////////////////
static const char* MsgType_List1[] = {
   MsgType_Unknown,
   MsgType_Heartbeat,
   MsgType_Logon,
   MsgType_TestRequest,
   MsgType_ResendRequest,
   MsgType_Reject,
   MsgType_SequenceReset,
   MsgType_Logout,
   MsgType_BusinessMessageReject,
   MsgType_NewOrderSingle,
   MsgType_ExecutionReport,
   MsgType_DontKnowTrade,
   MsgType_OrderCancelReplaceRequest,
   MsgType_OrderCancelRequest,
   MsgType_OrderCancelReject,
   MsgType_OrderStatusRequest,
   MsgType_TradingSessionStatusRequest,
   MsgType_TradingSessionStatus,
   MsgType_UserRequest,
   MsgType_UserResponse,
   MsgType_Advertisement,
   MsgType_IndicationOfInterest,
   MsgType_News,
   MsgType_Email,
   MsgType_QuoteRequest,
   MsgType_QuoteResponse,
   MsgType_QuoteRequestReject,
   MsgType_RFQRequest,
   MsgType_Quote,
   MsgType_QuoteCancel,
   MsgType_QuoteStatusRequest,
   MsgType_QuoteStatusReport,
   MsgType_MassQuote,
   MsgType_MassQuoteAcknowledgement,
   MsgType_MarketDataRequest,
   MsgType_MarketDataSnapshotFullRefresh,
   MsgType_MarketDataIncrementalRefresh,
   MsgType_MarketDataRequestReject,
   MsgType_SecurityDefinitionRequest,
   MsgType_SecurityDefinition,
   MsgType_SecurityTypeRequest,
   MsgType_SecurityTypes,
   MsgType_SecurityListRequest,
   MsgType_SecurityList,
   MsgType_DerivativeSecurityListRequest,
   MsgType_DerivativeSecurityList,
   MsgType_SecurityStatusRequest,
   MsgType_SecurityStatus,
   MsgType_OrderMassCancelRequest,
   MsgType_OrderMassCancelReport,
   MsgType_OrderMassStatusRequest,
   MsgType_NewOrderCross,
   MsgType_CrossOrderCancelReplaceRequest,
   MsgType_CrossOrderCancelRequest,
   MsgType_NewOrderMultileg,
   MsgType_MultilegOrderCancelReplaceRequest,
   MsgType_BidRequest,
   MsgType_BidResponse,
   MsgType_NewOrderList,
   MsgType_ListStrikePrice,
   MsgType_ListStatus,
   MsgType_ListExecute,
   MsgType_ListCancelRequest,
   MsgType_ListStatusRequest,
   MsgType_AllocationInstruction,
   MsgType_AllocationInstructionAck,
   MsgType_AllocationReport,
   MsgType_AllocationReportAck,
   MsgType_Confirmation,
   MsgType_ConfirmationAck,
   MsgType_ConfirmationRequest,
   MsgType_SettlementInstructions,
   MsgType_SettlementInstructionRequest,
   MsgType_TradeCaptureReportRequest,
   MsgType_TradeCaptureReportRequestAck,
   MsgType_TradeCaptureReport,
   MsgType_TradeCaptureReportAck,
   MsgType_RegistrationInstructions,
   MsgType_RegistrationInstructionsResponse,
   MsgType_PositionMaintenanceRequest,
   MsgType_PositionMaintenanceReport,
   MsgType_RequestForPositions,
   MsgType_RequestForPositionsAck,
   MsgType_PositionReport,
   MsgType_AssignmentReport,
   MsgType_CollateralRequest,
   MsgType_CollateralAssignment,
   MsgType_CollateralResponse,
   MsgType_CollateralReport,
   MsgType_CollateralInquiry,
   MsgType_NetworkStatusRequest,
   MsgType_NetworkStatusResponse,
   MsgType_CollateralInquiryAck,
};
//---------------------------------------------------------------------------

static const char* MsgType_Desc1[] = {
   Null_String,
   "0.Heartbeat",
   "A.Logon",
   "1.TestRequest",
   "2.ResendRequest",
   "3.Reject",
   "4.SequenceReset",
   "5.Logout",
   "j.BusinessMessageReject",
   "D.NewOrderSingle",
   "8.ExecutionReport",
   "Q.DontKnowTrade",
   "G.OrderCancelReplaceRequest",
   "F.OrderCancelRequest",
   "9.OrderCancelReject",
   "H.OrderStatusRequest",
   "g.TradingSessionStatusRequest",
   "h.TradingSessionStatus",
   "BE",
   "BF",
   "7",
   "6",
   "B",
   "C",
   "R",
   "AJ",
   "AG",
   "AH",
   "S",
   "Z",
   "a",
   "AI",
   "i",
   "b",
   "V",
   "W",
   "X",
   "Y",
   "c",
   "d",
   "v",
   "w",
   "x",
   "y",
   "z",
   "AA",
   "e",
   "f",
   "q",
   "r",
   "AF",
   "s",
   "t",
   "u",
   "AB",
   "AC",
   "k",
   "l",
   "E",
   "m",
   "N",
   "L",
   "K",
   "M",
   "J",
   "P",
   "AS",
   "AT",
   "AK",
   "AU",
   "BH",
   "T",
   "AV",
   "AD",
   "AQ",
   "AE",
   "AR",
   "o",
   "p",
   "AL",
   "AM",
   "AN",
   "AO",
   "AP",
   "AW",
   "AX",
   "AY",
   "AZ",
   "BA",
   "BB",
   "BC",
   "BD",
   "BG",
};
//---------------------------------------------------------------------------

K_ctor TFixMsgType1::TFixMsgType1 (const FIX::MsgType& msgType)
{
   assign(msgType.getValue().c_str());
}
//---------------------------------------------------------------------------

K_mf(void) TFixMsgType1::assign (const char* str)
{
   Data_ = emt_Unknown;
   if (str == NULL || strlen(str) == 0) {
      return;
   }

   for (int i=emt_Start; i<emt_Total; ++i) {
      //if (str[0] == MsgType_List[i][0]) {
      if (strcmp(str, MsgType_List1[i]) == 0) {
         Data_ = (EFixMsgType1)i;
         return;
      }
   }
}
//---------------------------------------------------------------------------

K_mf(std::string) TFixMsgType1::as_string () const
{
   std::string str;

   if (Data_ >= emt_Unknown && Data_ < emt_Total)
      //str.append(1, MsgType_List[Data_][0]);
      str.assign(MsgType_List1[Data_]);

   return str;
}
//---------------------------------------------------------------------------

K_mf(std::string) TFixMsgType1::as_desc () const
{
   if (Data_ > emt_Unknown && Data_ < emt_Total) {
      return std::string(MsgType_Desc1[Data_]);
   }
   return std::string();
}


/////////////////////////////////////////////////////////////////////////////
// class TFixExecTransType impl
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TFixExecTransType::as_desc () const
{
   switch (inherited::Data_) {
   //switch (inherited::data_[0]) {
      case ExecTransType_NEW:      return std::string("0.New");
      case ExecTransType_CANCEL:   return std::string("1.Cancel");
      case ExecTransType_CORRECT:  return std::string("2.Correct");
      case ExecTransType_STATUS:   return std::string("3.Status");
   }
   return inherited::as_string();
}


/////////////////////////////////////////////////////////////////////////////
// class TFixHandlInst impl
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TFixHandlInst::as_desc () const
{
   switch (inherited::Data_) {
   //switch (inherited::data_[0]) {
      case HandlInst_AUTOMATED_EXECUTION_ORDER_PRIVATE:  return std::string("1.AutomatedExecutionOrderPrivate");
      case HandlInst_AUTOMATED_EXECUTION_ORDER_PUBLIC:   return std::string("2.AutomatedExecutionOrderPublic");
      case HandlInst_MANUAL_ORDER:                       return std::string("3.ManualOrder");
   }
   return inherited::as_string();
}


/////////////////////////////////////////////////////////////////////////////
// class TFixOrdStatus impl
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TFixOrdStatus::as_desc () const
{
   switch (inherited::Data_) {
   //switch (inherited::data_[0]) {
      case OrdStatus_NEW:                   return std::string("0.New");
      case OrdStatus_PARTIALLY_FILLED:      return std::string("1.PartiallyFilled");
      case OrdStatus_FILLED:                return std::string("2.Filled");
      case OrdStatus_DONE_FOR_DAY:          return std::string("3.DoneForDay");
      case OrdStatus_CANCELED:              return std::string("4.Canceled");
      case OrdStatus_REPLACED:              return std::string("5.Replaced");
      case OrdStatus_PENDING_CANCEL:        return std::string("6.PendingCancel");
      case OrdStatus_STOPPED:               return std::string("7.Stopped");
      case OrdStatus_REJECTED:              return std::string("8.Rejected");
      case OrdStatus_SUSPENDED:             return std::string("9.Suspended");
      case OrdStatus_PENDING_NEW:           return std::string("A.PendingNew");
      case OrdStatus_CALCULATED:            return std::string("B.Calculated");
      case OrdStatus_EXPIRED:               return std::string("C.Expired");
      case OrdStatus_ACCEPTED_FOR_BIDDING:  return std::string("D.AcceptedForBidding");
      case OrdStatus_PENDING_REPLACE:       return std::string("E.PendingReplace");
   }
   return inherited::as_string();
}


/////////////////////////////////////////////////////////////////////////////
// class TFixOrdType impl
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TFixOrdType::as_desc () const
{
   switch (inherited::Data_) {
   //switch (inherited::data_[0]) {
      case OrdType_MARKET:                 return std::string("1.Market");
      case OrdType_LIMIT:                  return std::string("2.Limit");
      case OrdType_STOP:                   return std::string("3.Stop");
      case OrdType_STOP_LIMIT:             return std::string("4.StopLimit");
      case OrdType_MARKET_ON_CLOSE:        return std::string("5.MarketOnClose");
      case OrdType_WITH_OR_WITHOUT:        return std::string("6.WithOrWithout");
      case OrdType_LIMIT_OR_BETTER:        return std::string("7.LimitOrBetter");
      case OrdType_LIMIT_WITH_OR_WITHOUT:  return std::string("8.LimitWithOrWithout");
   }
   return inherited::as_string();
}


/////////////////////////////////////////////////////////////////////////////
// class TFixSide impl
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TFixSide::as_desc () const
{
   switch (inherited::Data_) {
   //switch (inherited::data_[0]) {
      case Side_BUY:                 return std::string("1.Buy");
      case Side_SELL:                return std::string("2.Sell");
      case Side_BUY_MINUS:           return std::string("3.BuyMinus");
      case Side_SELL_PLUS:           return std::string("4.SellPlus");
      case Side_SELL_SHORT:          return std::string("5.SellShort");
      case Side_SELL_SHORT_EXEMPT:   return std::string("6.SellShortExempt");
      case Side_UNDISCLOSED:         return std::string("7.Undisclosed");
      case Side_CROSS:               return std::string("8.Cross");
      case Side_CROSS_SHORT:         return std::string("9.CrossShort");
      case Side_CROSS_SHORT_EXEMPT:  return std::string("A.CrossShortExempt");
      case Side_AS_DEFINED:          return std::string("B.AsDefined");
      case Side_OPPOSITE:            return std::string("C.Opposite");
      case Side_SUBSCRIBE:           return std::string("D.Subscribe");
      case Side_REDEEM:              return std::string("E.Redeem");
      case Side_LEND:                return std::string("F.Lend");
      case Side_BORROW:              return std::string("G.Borrow");
   }
   return inherited::as_string();
}


/////////////////////////////////////////////////////////////////////////////
// class TFixTimeInForce impl
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TFixTimeInForce::as_desc () const
{
   switch (inherited::Data_) {
   //switch (inherited::data_[0]) {
      case TimeInForce_DAY:                  return std::string("0.Day");
      case TimeInForce_GOOD_TILL_CANCEL:     return std::string("1.GoodTillCancel");
      case TimeInForce_AT_THE_OPENING:       return std::string("2.AtTheOpening");
      case TimeInForce_IMMEDIATE_OR_CANCEL:  return std::string("3.ImmediateOrCancel");
      case TimeInForce_FILL_OR_KILL:         return std::string("4.FillOrKill");
      case TimeInForce_GOOD_TILL_CROSSING:   return std::string("5.GoodTillCrossing");
      case TimeInForce_GOOD_TILL_DATE:       return std::string("6.GoodTillDate");
      case TimeInForce_AT_THE_CLOSE:         return std::string("7.AtTheClose");
   }
   return inherited::as_string();
}


/////////////////////////////////////////////////////////////////////////////
// class TFixCxlRejReason impl  (102) (int)
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TFixCxlRejReason::as_desc () const
{
   switch (inherited::Data_) {
      case CxlRejReason_TOO_LATE_TO_CANCEL:                                         return std::string("0.TooLateToCancel");
      case CxlRejReason_UNKNOWN_ORDER:                                              return std::string("1.UnknownOrder");
      case CxlRejReason_BROKER_EXCHANGE_OPTION:                                     return std::string("2.BrokerExchangeOption");
      case CxlRejReason_ORDER_ALREADY_IN_PENDING_CANCEL_OR_PENDING_REPLACE_STATUS:  return std::string("3.OrderAlreadyInPendingCancelOrPendingReplaceStatus");
      case CxlRejReason_UNABLE_TO_PROCESS_ORDER_MASS_CANCEL_REQUEST:                return std::string("4.UnableToProcessOrderMassCancelRequest");
      case CxlRejReason_ORIGORDMODTIME_DID_NOT_MATCH_LAST_TRANSACTTIME_OF_ORDER:    return std::string("5.OrigordmodtimeDidNotMatchLastTransacttimeOfOrder");
      case CxlRejReason_DUPLICATE_CLORDID_RECEIVED:                                 return std::string("6.DuplicateClOrdIDReceived");
      case CxlRejReason_OTHER:                                                      return std::string("99.Other");
   }
   return inherited::as_string();
}


/////////////////////////////////////////////////////////////////////////////
// class TFixOrdRejReason impl  (103) (int)
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TFixOrdRejReason::as_desc () const
{
   switch (inherited::Data_) {
      case OrdRejReason_BROKER_EXCHANGE_OPTION:                      return std::string("0.Broker Exchange Option");
      case OrdRejReason_UNKNOWN_SYMBOL:                              return std::string("1.Unknown Symbol");
      case OrdRejReason_EXCHANGE_CLOSED:                             return std::string("2.Exchange Closed");
      case OrdRejReason_ORDER_EXCEEDS_LIMIT:                         return std::string("3.Order Exceeds Limit");
      case OrdRejReason_TOO_LATE_TO_ENTER:                           return std::string("4.Too Late To Enter");
      case OrdRejReason_UNKNOWN_ORDER:                               return std::string("5.Unknown Order");
      case OrdRejReason_DUPLICATE_ORDER:                             return std::string("6.Duplicate Order");
      case OrdRejReason_DUPLICATE_OF_A_VERBALLY_COMMUNICATED_ORDER:  return std::string("7.Duplicate Of A Verbally Communicated Order");
      case OrdRejReason_STALE_ORDER:                                 return std::string("8.Stale Order");
      case OrdRejReason_TRADE_ALONG_REQUIRED:                        return std::string("9.Trade Along Required");
      case OrdRejReason_INVALID_INVESTOR_ID:                         return std::string("10.Invalid Investor ID");
      case OrdRejReason_UNSUPPORTED_ORDER_CHARACTERISTIC:            return std::string("11.Unsupported Order Characteristic");
      case OrdRejReason_SURVEILLENCE_OPTION:                         return std::string("12.Surveillence Option");
      case OrdRejReason_INCORRECT_QUANTITY:                          return std::string("13.Incorrect Quantity");
      case OrdRejReason_INCORRECT_ALLOCATED_QUANTITY:                return std::string("14.Incorrect Allocated Quantity");
      case OrdRejReason_UNKNOWN_ACCOUNT:                             return std::string("15.Unknown Account");
      case OrdRejReason_OTHER:                                       return std::string("99.Other");
   }
   return inherited::as_string();
}


/////////////////////////////////////////////////////////////////////////////
// class TFixExecType impl
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TFixExecType::as_desc () const
{
   switch (inherited::Data_) {
   //switch (inherited::data_[0]) {
      case ExecType_NEW:              return std::string("0.New");
      case ExecType_PARTIAL_FILL:     return std::string("1.PartialFill");
      case ExecType_FILL:             return std::string("2.Fill");
      case ExecType_DONE_FOR_DAY:     return std::string("3.DoneForDay");
      case ExecType_CANCELED:         return std::string("4.Canceled");
      case ExecType_REPLACE:          return std::string("5.Replace");
      case ExecType_PENDING_CANCEL:   return std::string("6.PendingCancel");
      case ExecType_STOPPED:          return std::string("7.Stopped");
      case ExecType_REJECTED:         return std::string("8.Rejected");
      case ExecType_SUSPENDED:        return std::string("9.Suspended");
      case ExecType_PENDING_NEW:      return std::string("A.PendingNew");
      case ExecType_CALCULATED:       return std::string("B.Calculated");
      case ExecType_EXPIRED:          return std::string("C.Expired");
      case ExecType_RESTATED:         return std::string("D.Restated");
      case ExecType_PENDING_REPLACE:  return std::string("E.PendingReplace");
      case ExecType_TRADE:            return std::string("F.Trade");
      case ExecType_TRADE_CORRECT:    return std::string("G.TradeCorrect");
      case ExecType_TRADE_CANCEL:     return std::string("H.TradeCancel");
      case ExecType_ORDER_STATUS:     return std::string("I.OrderStatus");
   }
   return inherited::as_string();
}


/////////////////////////////////////////////////////////////////////////////
// class TFixExecRestatementReason impl
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TFixExecRestatementReason::as_desc () const
{
   switch (inherited::Data_) {
      case ExecRestatementReason_GT_CORPORATE_ACTION:          return std::string("0.GtCorporateAction");
      case ExecRestatementReason_GT_RENEWAL_RESTATEMENT:       return std::string("1.GtRenewalRestatement");
      case ExecRestatementReason_VERBAL_CHANGE:                return std::string("2.VerbalChange");
      case ExecRestatementReason_REPRICING_OF_ORDER:           return std::string("3.RepricingOfOrder");
      case ExecRestatementReason_BROKER_OPTION:                return std::string("4.BrokerOption");
      case ExecRestatementReason_PARTIAL_DECLINE_OF_ORDERQTY:  return std::string("5.PartialDeclineOfOrderQty");
      case ExecRestatementReason_CANCEL_ON_TRADING_HALT:       return std::string("6.CancelOnTradingHalt");
      case ExecRestatementReason_CANCEL_ON_SYSTEM_FAILURE:     return std::string("7.CancelOnSystemFailure");
      case ExecRestatementReason_MARKET_OPTION:                return std::string("8.MarketOption");
      case ExecRestatementReason_CANCELED_NOT_BEST:            return std::string("9.CanceledNotBest");
  }
   return inherited::as_string();
}


/////////////////////////////////////////////////////////////////////////////
// class TFixCxlRejResponseTo impl
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TFixCxlRejResponseTo::as_desc () const
{
   switch (inherited::Data_) {
   //switch (inherited::data_[0]) {
      case CxlRejResponseTo_ORDER_CANCEL_REQUEST:          return std::string("1.OrderCancelRequest");
      case CxlRejResponseTo_ORDER_CANCEL_REPLACE_REQUEST:  return std::string("2.OrderCancelReplaceRequest");
   };
   return inherited::as_string();
}
//---------------------------------------------------------------------------
