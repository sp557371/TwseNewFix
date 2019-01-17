#ifndef FixAppMsgH
#define FixAppMsgH
namespace Kway {
namespace Tw   {
namespace Fix  {
namespace Stk  {
/////////////////////////////////////////////////////////////////////////////
// New Order - Single
#define fMsgType_NewOrderSingle_s        "D"

#define fTag_ClOrdID                      11
#define fTag_OrderID                      37
#define fTag_Account                       1
#define fTag_HandlInst                    21
//#define fTag_NoTradingSessions           386
//#define fTag_TradingSessionID            336
#define fTag_Symbol                       55
#define fTag_Side                         54
#define fTag_TimeInForce                  59
#define fTag_TransactTime                 60
#define fTag_OrderQty                     38
#define fTag_OrdType                      40
#define fTag_Price                        44
#define fTag_Text                         58
//#define fTag_BrokerID                  10000
#define fTag_TwseIvacnoFlag            10000
#define fTag_TwseOrdType               10001
#define fTag_Tag10002                  10002
#define fTag_Tag10003                  10003

#define fTag_SecurityExchange            207
#define fTag_OrderCapacity                47

// Order Cancel/Replace
#define fTag_OrigClOrdID                  41

// Execution Report
//#define fTag_SecondaryOrderID            198
#define fTag_ExecID                       17
#define fTag_ExecTransType                20
#define fTag_ExecType                    150
#define fTag_OrdStatus                    39
#define fTag_OrdRejReason                103
#define fTag_ExecRestatmentReason        378
#define fTag_LastShares                   32
#define fTag_LastPx                       31
#define fTag_LeavesQty                   151
#define fTag_CumQty                       14
#define fTag_AvgPx                         6
//#define fTag_TwseStatusCode            10002

// Order Cancel Reject
#define fTag_CxlRejResponseTo            434
#define fTag_CxlRejReason                102

// Trading Session Status Request
#define fTag_TradSesReqID                335
#define fTag_SubscriptionRequestType     263
//#define fTag_SwitchTradSes             10003
//#define fTag_APCode                    10004
//#define fTag_MarketType                10005

// Trading Session Status
#define fTag_TradSesStatus               340
/////////////////////////////////////////////////////////////////////////////
}; // Stk
}; // Fix
}; // Tw
}; // Kway
#endif

