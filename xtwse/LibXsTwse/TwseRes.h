/////////////////////////////////////////////////////////////////////////////
#if !defined(_TwseRes_h_)
#define _TwseRes_h_
//---------------------------------------------------------------------------
#include "QFixExt/QFixTagDef.h"
#include "TwStkOrder.h"
#include "FixStkRes.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Tw::Stk;

/////////////////////////////////////////////////////////////////////////////
// enum
/////////////////////////////////////////////////////////////////////////////
enum ETwseFixRejectCode
{
   tfrc_None = FIX::Stk::wrc_None,

   // 請注意, tfrc_Start 必須 >= FIX::Stk::EWorkRejCode::wrc_UserBegin
   tfrc_Start = FIX::Stk::wrc_UserBegin,
      // 連線子系統
      tfrc_Conn_Start,
         tfrc_AppendNoError,        // 01.檢查並更正APPEND-NO
         tfrc_KeyValueError,        // 04.檢查並更正KEY-VALUE          2013/12/17 miguel change to 02.
         tfrc_AppendNoEqual0,       // 70.檢查並更正APPEND NO          2013/12/17 miguel change to 03.
         tfrc_RawDataLenNotEqual5,  // 71.檢查並更正RAWDATA LENGTH     2013/12/17 miguel change to 08.
         tfrc_ApCodeErr,            // 72.檢查並更正AP-CODE  (FIX欄位TargetSubID)
         tfrc_SenderCompIDError,    // 73.錯誤的SenderCompID (FIX欄位SenderCompID)
         tfrc_MsgTypeError,         // 78.不支援的訊息類別   (FIX欄位MsgType)
         tfrc_TradeSuspended,       // 86.此PVC停止作業，進入離線狀態
      tfrc_Conn_End,

      // 交易子系統
      tfrc_Trade_Start,
         tfrc_BrokerIDErr,          // 12.檢查並更正證券商代號          (FIX欄位SenderSubID)
         /* tfrc_SeqNoError,           // 18.檢查並更正委託書編號          (FIX欄位OrderID後四碼) */
         tfrc_QuantityError,        // 22.檢查並更正委託數量            (FIX欄位OrderQty)
         tfrc_QuantityWasCut,       // 31.外資買進或借券賣委託數量被刪減
         // for fix
                                    // 70.檢查FIX ClOrdID是否重複
         tfrc_IvacNoLenErr,         // 71.檢查並更正投資人帳號          (FIX欄位Account)
         tfrc_SeqNoLenErr,          // 72.檢查並更正委託書編號          (FIX欄位OrderID)
         tfrc_StkNoLenErr,          // 73.檢查並更正股票代號            (FIX欄位Symbol)
         tfrc_PriLenErr,            // 74.檢查並更正單價
         tfrc_QtyLenErr,            // 75.檢查並更正委託數量            (FIX欄位OrderQty)
         tfrc_ErrorOverLimit,       // 77.錯誤次數超過限制,請通知證交所
         tfrc_WarrantLPErr,         // 78.檢查並更正WARRANT-LP          (FIX欄位TwseWarrantLP)
         tfrc_IvacNoFlagErr,        // 79.檢查並更正IVACNO-FLAG         (FIX欄位TwseIvacnoFlag)
         tfrc_TwseOrdTypeErr,       // 80.檢查並更正ORDER-TYPE          (FIX欄位TwseOrdType)
         tfrc_OrigClOrdIDErr,       // 81.檢查並更正OrigClOrdID         (FIX欄位OrigClOrdID)
         tfrc_ClOrdIDErr,           // 82.檢查並更正ClOrdID             (FIX欄位ClOrdID)
         tfrc_OrdTypeErr,           // 83.檢查並更正OrdType             (FIX欄位OrdType)
                                    // 84.刪改查與原委託內容不符 (FIX.Stk 裡面已有定義 wrc_OrdContNotMatch)
         tfrc_TwseOrdIdDup,         // 85.委託書編號重覆(券商代號+委託書號)
   
         tfrc_FrontEndSystemError,  // 98.前端系統異常，請重新委託
         tfrc_CallCenter,           // 99.查詢委託是否已輸入成功或打電話到證交所詢問

         //tfrc_ApCodeErr,      // 76.檢查並更正AP-CODE             (FIX欄位TargetSubID)
         //tfrc_MsgTypeError,   // 78.不支援的訊息類別              (FIX欄位MsgType)
      tfrc_Trade_End,

   tfrc_Total
};

/////////////////////////////////////////////////////////////////////////////
// class TTwseRes
/////////////////////////////////////////////////////////////////////////////
class TTwseRes
{
   typedef FIX::TFixTargetSubID  TFixTargetSubID;

   static K_mf(std::string) GetErrCodeHdr (int rejCode, const TMarket&, const TTrxSegment&);

public:
   // 錯誤代碼 - 使用 ETwseFixRejectCode 取得 StatusCode
   // ==> 如果只是要取 [連線子系統], 只要傳入 (rejCode, TMarket) 即可
   static K_mf(std::string) GetStsCodeByRejCode (int rejCode, const TMarket&, const TTrxSegment& seg = TTrxSegment(ts_Early));

   // 錯誤代碼 - 使用 ETwseFixRejectCode 取得 StatusCode
   static K_mf(std::string) GetStsCodeByRejCode (int rejCode, const TMarket&, const TFixTargetSubID&);

   // 錯誤代碼 - 使用 ETwseFixRejectCode 取得 StatusCode
   static K_mf(std::string) GetStsCodeByTwseCode (int twseCode, const TMarket&, const TTrxSegment&);

   // 錯誤代碼 - 使用 ETwseFixRejectCode 取得 StatusCode
   static K_mf(std::string) GetStsCodeByTwseCode (int twseCode, const TMarket&, const TFixTargetSubID&);

   //==============================================
   // 子系統代號
   static const char*  TseConn;
   static const char*  TseFull;
   static const char*  TseFixed;
   static const char*  TseOdd;
   //--------------------
   static const char*  OtcConn;
   static const char*  OtcFull;
   static const char*  OtcFixed;
   static const char*  OtcOdd;
   
   //==============================================
   // 錯誤碼
   //------------------------------------
   // 連線子系統
   static const char*  AppendNoError;        // 01.檢查並更正APPEND-NO
   static const char*  KeyValueError;        // 04.檢查並更正KEY-VALUE          2013/12/17 miguel change to 02.
   static const char*  AppendNoEqual0;       // 70.檢查並更正APPEND NO          2013/12/17 miguel change to 03.
   static const char*  RawDataLenNotEqual5;  // 71.檢查並更正RAWDATA LENGTH     2013/12/17 miguel change to 08.
   static const char*  ApCodeErr;            // 72.檢查並更正AP-CODE  (FIX欄位TargetSubID)
   static const char*  SenderCompIDError;    // 73.錯誤的SenderCompID (FIX欄位SenderCompID)
   static const char*  MsgTypeError;         // 78.不支援的訊息類別   (FIX欄位MsgType)
   static const char*  TradeSuspended;       // 86.此PVC停止作業，進入離線狀態
   //------------------------------------
   // 交易子系統
   //--------------------------
   
   /*
   // 18.檢查並更正委託書編號(FIX欄位OrderID後四碼)(普通交易,盤後定價)
   // 17.檢查並更正委託書編號(FIX欄位OrderID後四碼)(零股)
   static const char*  SeqNoError;
   static const char*  SeqNoErrorOdd;
   */

   //--------------------------
   // 22.檢查並更正委託數量(FIX欄位OrderQty)
   static const char*  QuantityError;        
   //--------------------------
   // 外資買進或借券賣委託數量被刪減
   static const char*  QuantityWasCut;       // 31:普通,盤後
   static const char*  QuantityWasCutOdd;    // 32:零股

   static const char*  ClOrdIDRepeat;        // 70.檢查FIX ClOrdID是否重複
   static const char*  IvacNoLenErr;         // 71.檢查並更正投資人帳號
   static const char*  SeqNoLenErr;          // 72.檢查並更正委託書編號
   static const char*  StkNoLenErr;          // 73.檢查並更正股票代號
   static const char*  PriLenErr;            // 74.檢查並更正單價
   static const char*  QtyLenErr;            // 75.檢查並更正委託數量
   static const char*  BrokerIDErr;          // 76.檢查並更正BrokerID            (FIX欄位SenderSubID)
   static const char*  ErrorOverLimit;       // 77.錯誤次數超過限制,請通知證交所
   static const char*  WarrantLPErr;         // 78.檢查並更正WARRANT-LP          (FIX欄位TwseWarrantLP)
   static const char*  IvacNoFlagErr;        // 79.檢查並更正IVACNO-FLAG         (FIX欄位TwseIvacnoFlag)
   static const char*  TwseOrdTypeErr;       // 80.檢查並更正ORDER-TYPE          (FIX欄位TwseOrdType)
   static const char*  OrigClOrdIDErr;       // 81.檢查並更正OrigClOrdID         (FIX欄位OrigClOrdID)
   static const char*  ClOrdIDErr;           // 82.檢查並更正ClOrdID             (FIX欄位ClOrdID)
   static const char*  OrdTypeErr;           // 83.檢查並更正OrdType             (FIX欄位OrdType)
   static const char*  OrdContNotMatch;      // 84.刪改查與原委託內容不符
   static const char*  TwseOrdIdDup;         // 85.委託書編號重覆(券商代號+委託書號)
   static const char*  FrontEndSystemError;  // 98.前端系統異常，請重新委託
   static const char*  CallCenter;           // 99.查詢委託是否已輸入成功或打電話到證交所詢問
   //----------------------------------------------
   static const int    IntQuantityWasCut;    // 31.外資買進或借券賣委託數量被刪減
   static const int    IntQuantityWasCutOdd; // 32.外資買進委託數量被刪減

   //static const char*  ApCodeErr;      // 76.檢查並更正AP-CODE
   //static const char*  MsgTypeError;   // 78.不支援的訊息類別              (FIX欄位MsgType)
};

/////////////////////////////////////////////////////////////////////////////
} // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseRes_h_)
