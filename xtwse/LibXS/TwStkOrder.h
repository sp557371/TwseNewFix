//
// TwStk 的 [單一委託書]Record 及 相關的 types、classes 定義
//
// 版本: $Id: TwStkOrder.h,v 1.13 2006/04/04 03:46:03 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkOrderH
#define TwStkOrderH
//---------------------------------------------------------------------------
#include "wise_ptr.h"
//---------------------------------------------------------------------------
#include "KTime.h"
#include "TwStk.h"
#include "TwStkIvacNo.h"
#include "TwStkSaleNo.h"
#include "TwStkDeal.h"
#include "Signon/Policy/UPTwStkScItems.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace Stk {
using namespace Kstl;
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
/////////////////////////////////////////////////////////////////////////////
#ifdef K_EnumPacked_Bug
   typedef byte   EOrdCmdID;
   enum EOrdCmdID_t
#else
   enum EOrdCmdID
#endif
{
      cid_New,
      cid_Qry,
      cid_Del,
      cid_ChgQty,
      cid_ChgPri,
      cid_TriMul,
      cid_TriDiv,
      cid_End,//TOrdCmdID的定義結束
      cid_Bulletin = cid_End  //公告
} K_EnumPacked;
TWSTK_class TOrdCmdID : public EnumT<EOrdCmdID>
{
   typedef EnumT<EOrdCmdID>   inherited;
public:
   inline K_ctor      TOrdCmdID  (EOrdCmdID a = cid_New)  : inherited(a) {}
   K_mf(std::string)  as_string  (bool longStr=false) const;
   K_mf(void)         assign     (const std::string&);
};
//-----------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   EBulletinItem;
   enum EBulletinItem_t
#else
   enum EBulletinItem
#endif
{  // 公告項目
   bi_TrafficStock,   // 塞單股票
   bi_General,        // 一般公告
   bi_TrxSegment,     // 交易時段變更公告

//交易時段變更公告,須配合 FlowID_:
// of_P_ing = 啟動與交易所的連線介面,若啟動連線失敗,則應持續嘗試與交易所連線
// of_Q_ing = 若啟動連線成功,則進入此狀態,可開始接收委託,但 <不表示> 委託時間已到
//            of_P_ing,of_Q_ing,應放在同一筆委託公告
// of_S_ing = 已有收到交易所的委託成功訊息,表示確定交易時間已到
//            為選項,此狀態可以不公告,視 [委託傳送流程模組] 實作而定
// of_Finish  收盤公告
// of_LineBreakBulletin   斷線公告
//
// 以上的 [公告時機] 說明,僅為建議,應視 [委託傳送流程模組] 實作而定
// 其餘針對各種公告的產生時機,及收到公告時的處理,應交給各個模組自行處理
};
//-----------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   ETrxSegment;
   enum ETrxSegment_t
#else
   enum ETrxSegment
#endif
{  // 交易時段
   ts_Early,   //今日尚未開始.

   ts_Full,    //整股.
   ts_FullLate,//整股收盤.

   ts_Odd,     //零股.     94/12/19:零股委託時間改為: pm1:40-pm2:30, 當日成交.
   ts_Fix,     //盤後定價.

   ts_OddLate, //零股收盤.
   ts_FixLate, //定價收盤.

   ts_Late,    //今日交易結束.
};
TWSTK_class TTrxSegment : public EnumT<ETrxSegment>
{
   typedef EnumT<ETrxSegment>   inherited;
public:
   inline K_ctor      TTrxSegment (ETrxSegment a = ts_Full)  : inherited(a) {}
   K_mf(std::string)  as_string   (bool longStr=false) const;
   K_mf(void)         assign      (const std::string&);
};
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef char   EBSCode;
   enum EBSCode_t
#else
   enum EBSCode
#endif
{
   bsc_Buy    = 'B',
   bsc_Sell   = 'S',
} K_EnumPacked;
struct TBSCode : public EnumT<EBSCode>
{
 //K_mf(std::string)  as_string  () const;                 使用 EnumT<> 的預設
 //K_mf(void)         assign     (const std::string&);     使用 EnumT<> 的預設
};
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef char   EOType;
   enum EOType_t
#else
   enum EOType
#endif
{
   ot_Unknow = 0,
   ot_Gn   = '0',  //現股/集保
   ot_CrA  = '1',  //代辦融資
   ot_DbA  = '2',  //代辦融券
   ot_CrS  = '3',  //自辦融資
   ot_DbS  = '4',  //自辦融券
   ot_Tse5 = '5',  //交易所-借券賣出        (送交易所時,交易類別為'5')
   ot_Tse6 = '6',  //交易所-避險套利借券賣出(送交易所時,交易類別為'6')

   //交易所沒有這類的定義,所以使用特殊符號
   ot_CrC  = '%',  //沖資
   ot_DbC  = '^',  //沖券
   ot_GnC  = '&',  //現沖

   ot_Auct = 'A',  //拍賣
   ot_Lend = 'L',  //標借
   ot_Tend = 'T',  //標購
} K_EnumPacked;
TWSTK_class TOType : public EnumT<EOType>
{
   typedef EnumT<EOType>   inherited;
public:
   inline K_ctor  TOType  (EOType a = ot_Unknow)   : inherited(a)  {}

   K_mf(std::string)  as_string  () const;  // "0","1","2","3","4","C5","C6","C7","T5","T6"
   K_mf(void)         assign     (const std::string&);
   K_mf(TOType)       GetLegal   () const;

   K_mf(bool)  IsGn  () const  { return(Data_==ot_Gn   ||  Data_==ot_GnC); }
   K_mf(bool)  IsCr  () const  { return(Data_==ot_CrA  ||  Data_==ot_CrS  ||  Data_==ot_CrC); }
   K_mf(bool)  IsDb  () const  { return(Data_==ot_DbA  ||  Data_==ot_DbS  ||  Data_==ot_DbC); }
};
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   EOrdFlowID;
   enum EOrdFlowID_t
#else
   enum EOrdFlowID
#endif
{
   of_P_ing,            //處理中.
   of_Q_ing,            //排隊中.
   of_S_ing,            //傳送中.
   of_Sent,             //適用於[委託要求]與[委託成功]從不同線路告知時.
   of_Finish,           //正常結束.
   of_AbortOrdClear,    //委託流程中斷-[新增]委託無效.
   of_AbortOrdUnknow,   //委託流程中斷-[新增/刪單/改量]狀況不明.
   of_AbortOrdNoChange, //委託流程中斷-[刪單/改量]委託狀況不變.
   of_LineBreakBulletin,//交易斷線公告.
} K_EnumPacked;
TWSTK_class TOrdFlowID
{
   union {
      struct {
         byte  Flow_ : 4,  // of_P_ing, of_Q_ing, of_S_ing, of_Finish...
               Step_ : 4;  // 0 = #Finish#, 1-15
      };
      byte     UData_;
   };
public:
   inline explicit K_ctor  TOrdFlowID  (int stepn=0, EOrdFlowID of=of_Finish)
                              : Flow_(of), Step_(static_cast<byte>(stepn))
                              { }

   K_mf(std::string)  as_string  () const;   //1P,1Q,1S,1F,1A...2P,2Q,2S...
   K_mf(void)         assign     (const std::string&);

   inline K_mf(byte)       GetStep    ()             const   { return Step_;                 }
   inline K_mf(EOrdFlowID) GetFlow    ()             const   { return EOrdFlowID(Flow_);     }
   inline K_mf(void)       SetStep    (int n)                { Step_ = static_cast<byte>(n); }
   inline K_mf(void)       SetFlow    (EOrdFlowID a)         { Flow_ = a;                    }
   /** 委託流程結束:可能全部完成,或在某步驟中斷.
   */
   inline K_mf(bool)       IsAllFinish () const              { return( (Step_ == 0  &&  Flow_ >= of_Finish)
                                                                     || Flow_ > of_Finish); }
   /** 委託流程全部正常執行完畢.
   */
   inline K_mf(bool)       IsNormalFinish () const           { return(Step_ == 0  &&  Flow_ >= of_Finish); }

   inline K_mf(bool)       operator== (TOrdFlowID a) const   { return UData_==a.UData_;  }
   inline K_mf(bool)       operator!= (TOrdFlowID a) const   { return UData_!=a.UData_;  }
   inline K_mf(bool)       operator<  (TOrdFlowID a) const   { return Step_ == a.Step_
                                                                      ? Flow_     < a.Flow_
                                                                      : (Step_-1) < (a.Step_-1); }
   inline K_mf(bool)       operator>  (TOrdFlowID a) const   { return(a < *this); }
};
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   EScCode;
   enum EScCode_t
#else
   enum EScCode
#endif
{  //風控管制項目代碼

   sc_None         =  0,
   sc_NoT30        =  1,  // T30無資料         ┌ sc_NoT30 及 sc_PriceLimit
   sc_PriceLimit   =  2,  // 單價超過漲跌停    └ 為了在T30資料異常時,仍可強迫輸入
   sc_FullT        =  3,  // 全額交割

   sc_Dep1BQty     =  4,  // 單筆B/S張數超過 _Dep1BQty  則預收款券
   sc_Dep1SQty     =  sc_Dep1BQty,
   sc_DepNBQty     =  5,  // 多筆B/S張數超過 _DepNBQty  則預收款券
   sc_DepNSQty     =  sc_DepNBQty,
   sc_DepNBSQty    =  6,  // 多筆B+S張數超過 _DepNBSQty 則預收款券
   sc_DepAutoQty   =  7,  // KPTC → KPTS 張數超過預收時,自動調整成不要預收的數量
   sc_EchoNone     =  7,  // KPTS → KPTC

   sc_IvacQtyB     =  8,            // 每日單一帳號[現買進][資買進]合計張數限額,(不含券買)
   sc_IvacAmtB     =  9,            // 每日單一帳號[現買進][資買進]合計價金限額,(不含券買)
   sc_Qty1B        = 10,            // 單筆        [現買進][資買進]張數限額,(不含券買)
   sc_Amt1B        = 11,            // 單筆        [現買進][資買進]價金限額,(不含券買)
   sc_IvacQtyS     = sc_IvacQtyB,   // 每日單一帳號[現賣出][券賣出]合計張數限額,(不含資賣)
   sc_IvacAmtS     = sc_IvacAmtB,   // 每日單一帳號[現賣出][券賣出]合計價金限額,(不含資賣)
   sc_Qty1S        = sc_Qty1B,      // 單筆        [現賣出][券賣出]張數限額,(不含資賣)
   sc_Amt1S        = sc_Amt1B,      // 單筆        [現賣出][券賣出]價金限額,(不含資賣)
   sc_IvacQtyBS    = 12,            // 每日單一帳號[現B/S][券S][資B]合計張數限額
   sc_IvacAmtBS    = 13,            // 每日單一帳號[現B/S][券S][資B]合計價金限額
   sc_CtrlBS       = 14,            // BS管制
   sc_CtrlCD       = 15,            // 信用管制
   sc_CpAmtB       = 16,            // 公司(單一營業據點),超過買進限額
   sc_CpAmtS       = 17,            // 公司(單一營業據點),超過賣出限額
   sc_CpQtyB       = 18,            // 公司(單一營業據點),超過買進限張
   sc_CpQtyS       = 19,            // 公司(單一營業據點),超過賣出限張
   sc_CpAmtBS      = 20,            // 公司(單一營業據點),超過BS限額
   sc_CpQtyBS      = 21,            // 公司(單一營業據點),超過BS限張

   sc_CrDbStop     = 22, // 停資券強迫旗標
   sc_CrDbQty      = 23, // 資券配額不足
   sc_RvDbStop     = 24, // 資買委託,但是今日停券
   sc_RvDbOver     = 25, // 資買委託,超過今日可能券賣

   sc_BalQty       = 26, // 庫存股數不足
   sc_BalAmt       = 27, // 超過信用級數上限
   sc_CrDb1StkAmt  = 28, // 超過信用單股上限
   sc_TotBSAmt     = 29, // 整戶BS投資上限(授信額度)
   sc_NoFinaStk    = 30, // 證金配額未建檔,請改成[現股]後強迫

   sc_CrDbQtyDeal     = 31, // 資券配額不足    (本委託+已成交 > 配額)
   sc_BalQtyDeal      = 32, // 庫存股數不足    (本委託+已成交 > 庫存)
   sc_BalAmtDeal      = 33, // 超過信用級數上限(本委託+已成交 > 信用級數)
   sc_CrDb1StkAmtDeal = 34, // 超過信用單股上限(本委託+已成交 > 單股上限)
   sc_TotBSAmtDeal    = 35, // 整戶BS投資上限  (本委託+已成交 > 授信額度)

   sc_BalAmtSum       = 36, // 超過信用級數上限(本委託+總歸戶委託 > 信用級數)
   sc_CrDb1StkAmtSum  = 37, // 超過信用單股上限(本委託+總歸戶委託 > 單股上限)
   sc_TotBSAmtSum     = 38, // 總歸戶投資上限  (本委託+總歸戶委託 > 授信額度)

   sc_AmtBOtc2        = 39, // OTC 2類股 買進超額
   sc_AmtLevel1       = 40, // 委託金額控管第一級
   sc_AmtLevel2       = 41, // 委託金額控管第二級
   sc_AmtLevel3       = 42, // 委託金額控管第三級
   sc_Dormant         = 43, // 靜止戶
   sc_GroupAmt        = 44, // 20070820群組投資額度控管
   sc_NoKYC           = 45, // 20080220檢查KYC狀態
   sc_IvacStk         = 46, // 20090324個人個股安控

   sc_End     = 8*8,//預留0-63,共64種管制項目
   sc_AllPass = 0x7f,
} K_EnumPacked;

//typedef Kway::Signon::Policy::TScLevel    TScLevel;
using ::Kway::Signon::Policy::TScLevel;

TWSTK_class TScCode
{
   byte     ScCode_;
   TScLevel ScLevel_;
public:
   inline K_ctor  TScCode  ()  : ScCode_(sc_AllPass) { }

   inline K_mf(void)      Set        (EScCode scCode, TScLevel scLevel)   { ScCode_ = scCode;  ScLevel_ = scLevel; }
   inline K_mf(EScCode)   Get        () const  { return (EScCode)ScCode_;      }
   inline K_mf(bool)      IsPass     () const  { return ScCode_ == sc_AllPass; }
   inline K_mf(TScLevel)  GetScLevel () const  { return ScLevel_;              }
   /** 風控檢查通過.
   */
   inline K_mf(void)      SetPass    ()        { ScCode_ = sc_AllPass;         }
};
typedef std::bitset<sc_End>  TScForces;   //指定強迫旗標: 指定強迫某些[風控管制項目代碼]

TWSTK_gd(TScForces)     SC_CanChgOType;    // 風控管制,但,KPTC可改類別
TWSTK_gd(TScForces)     SC_CannotTri;      // 試算時,一旦有此管制,則停止試算
TWSTK_gd(TScForces)     SC_Deposit;        // 預收
TWSTK_gd(TScForces)     SC_TriErrMsg;      // 試算失敗,直接傳回風控訊息
TWSTK_gd(TScForces)     SC_QtyAmt_All;     // 單股風控失敗(sc_IvacQtyB～sc_CpQtyBS)
TWSTK_gd(TScForces)     SC_CpAll;          // 單股風控失敗(sc_CpXXXX)單一營業據點
TWSTK_gd(TScForces)     SC_CrDb;           //sc_NoFinaStk|sc_CrDbStop|sc_CrDbQty
TWSTK_gd(TScForces)     SC_TotBSAmt_All;   //sc_TotBSAmt|sc_TotBSAmtDeal|sc_TotBSAmtSum
TWSTK_gd(TScForces)     SC_CrDbQty_All;    //sc_CrDbQty|sc_CrDbQtyDeal
TWSTK_gd(TScForces)     SC_BalQty_All;     //sc_BalQty|sc_BalQtyDeal
TWSTK_gd(TScForces)     SC_BalAmt_All;     //sc_BalAmt|sc_BalAmtDeal|sc_BalAmtSum
TWSTK_gd(TScForces)     SC_CrDb1StkAmt_All;//sc_CrDb1StkAmt|sc_CrDb1StkAmtDeal|sc_CrDb1StkAmtSum
//---------------------------------------------------------------------------
struct TOrdSource //委託來源
{
   typedef char_ary<char,12,0>   TUserID;
   typedef char_ary<char,40,0>   TClOrdID;
   typedef char_ary<char,1,' '>  TOrderLine;
   TUserID     UserID_;
   TKTime      Time_;      //主機收到委託要求的時間
   TClOrdID    ClOrdID_;   //要求端自行編號,主機端不作任何的修改,您要填入字串或binary資料皆可!
   TOrderLine  OrderLine_;
};
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   EOrdEmergency;
   enum EOrdEmergency_t
#else
   enum EOrdEmergency //委託緊急程度
#endif
{
   oe_Lowest  =   0,
   oe_Lower   =  50,
   oe_Normal  = 100,
   oe_Higher  = 150,
   oe_Highest = 255,
} K_EnumPacked;
TWSTK_class TOrdEmergency : public EnumT<EOrdEmergency>
{
   typedef EnumT<EOrdEmergency>   inherited;
public:
   inline K_ctor      TOrdEmergency  (EOrdEmergency a = oe_Normal)  : inherited(a) {}
   inline K_mf(std::string) as_string () const                 { char buf[128]; ltoa(Data_,buf,10); return std::string(buf); }
   inline K_mf(void)        assign    (const std::string& str) { Data_ = static_cast<EOrdEmergency>(atol(str.c_str())); }
};
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   ETseOrderLine;
   enum ETseOrderLine_t
#else
   enum ETseOrderLine           //交易所使用的委託管道別20080901 eric: 20081006起委託封包需加註委託來源
#endif
{
   to_Normal    = 0,             //一般或未設定
   to_Auto      = 1,             //自動化服務設備委託下單
   to_DMA       = 2,             //電子式專屬線路(DMA)委託下單
   to_Internet  = 3,             //網際網路委託下單
   to_Voice     = 4,             //語音委託下單
   to_F_Normal  = 5,             //FIX一般或未設定
   to_F_Auto    = 6,             //FIX自動化服務設備委託下單
   to_F_DMA     = 7,             //FIX電子式專屬線路(DMA)委託下單
   to_F_Internet= 8,             //FIX網際網路委託下單
   to_F_Voice   = 9,             //FIX語音委託下單
};
//---------------------------------------------------------------------------
TWSTK_class TTseOrderLine : public EnumT<ETseOrderLine>
{
   typedef EnumT<ETseOrderLine>   inherited;
public:
   inline K_ctor      TTseOrderLine   (ETseOrderLine a = to_Normal)  : inherited(a) {}
   K_mf(std::string)  as_string  (bool longStr = true) const;
   K_mf(void)         assign     (const std::string&);
};
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
typedef fixed_num<dword>   TSysOrdID;   //每個交易平台,最多4億筆(1-0xffffffff)委託!
typedef fixed_num<byte>    THostID;     //每個交易平台,最多255台(1-0xff)交易主機.
struct TOrigSysOrdID {
   THostID     HostID_;
   TSysOrdID   SysOrdID_;
};
//---------------------------------------------------------------------------
struct TOrdFields_ForNew
{  //委託基本欄位-新增委託用
   TIvacKey       IvacKey_;   //投資人帳號(券商代號+客戶帳號)
   TOType         OType_;
   TStkNo         StkNo_;
   TStkQty        Qty_;       //原始委託(最小交易單位 = 一般為股數),若==0,表示尚未收到新增委託的回報
   TStkPri        Pri_;       //委託價,若==0,表示只收到成交回報,尚未收到委託回報
   TBSCode        BSCode_;
   TSfGroup       SfGroup_;      //信用配額組別
   TSaleNo        SaleNo_;
   TOrdSource     OrigSource_;   //原始委託來源
   TTrxSegment    TrxSegment_;   //交易時段
   Kway::byte     Attr_;         //配合OMS的: 委託屬性(TOrderAttr): OmsOrder.h
   Kstl::char_ary<char,1,' '>  TseOrderLine_; //配合20081006委託封包加註來源別, 20100712 修改為直接使用文字類別

   typedef Kstl::char_ary<char,8,' '>  TSubAcc;
   TSubAcc  SubAcc_; ///< 當後端連到券商主機時,有些券商會要求[子帳號].

   TTseOrdID      TseOrdID_;     //委託書號(櫃號+序號)
   TKDateTimeMS   SendingTime_;

};
//---------------------------------------------------------------------------
struct TStkOrder_Rec : public TOrdFields_ForNew
{  //此結構不包含指標性資料(如:明細表),可直接儲存
   //TSysOrdID    SysOrdID_;  Key值,由iterator取得,在此處不必保留此欄位

   inline K_ctor  TStkOrder_Rec  ()  : LastTseCode_(0)
                                     , Flags_(0)
                                     , LastOkOCmd_(cid_End)
                                     { }

   //這部分的 data members 一旦確定後(交易所回報OK後), [不會]隨著委託狀態而變化
   TOType         TseOType_;     //只有在交易所[回覆成功後]才填入正確的OType,之後就再也不會改變!
   TKTime         TseOrigTime_;  //交易所回覆的原始委託時間
   TBrokerID      DstBrkID_;     //興櫃委託:委託目的地推薦券商, 上市/上櫃:委託線路的券商代號
   TBrokerID      SrcBrkID_;     //若為錯帳委託, IvacKey_ 為[總公司錯帳帳號], 此時 SrcBrkID_ 為申報錯帳之[分公司代號]
   TMarket        Market_;
   //接下來的 data members 會隨著 成交、刪單、減量... 而有所改變
   TKTime         TseLastTime_;  //最後一次交易所回覆的時間(刪單.減量.查詢...不含成交)
   TOrdCmdID      LastOCmd_;     //最後一次要求交易所執行的操作
   TOrdCmdID      LastOkOCmd_;   //最後一次要求交易所[成功後]的操作,只可能包含: cid_End, cid_New, cid_Del, cid_ChgQty
                                 //刪改失敗,斷線,Timeout,<不>變動此欄位; 查詢成功後<不>變動此欄位.
                                 //此欄位由[交易所連線程序]or[委託回報連線相關程序]維護.

   //93.05.12 取消 TseBfQty_, TseAfQty_ 改成與 KPTS 相同的定義
   static inline K_mf(TStkQty)  DiffQty  (TStkQty q1, TStkQty q2)  { return q1 < q2 ? TStkQty(0) : TStkQty(q1-q2); }
   //交易所改前量:
//   inline K_mf(TStkQty)  TseBfQty () const   { return DiffQty(BfQty_, MatchQty_); }
   inline K_mf(TStkQty)  TseBfQty () const   { return TMPBfQty_; }
   //交易所改後量:
//   inline K_mf(TStkQty)  TseAfQty () const   { return DiffQty(AfQty_, MatchQty_); }
   inline K_mf(TStkQty)  TseAfQty () const   { return TMPAfQty_; }
   //交易所剩餘量:   or = AfQty_ - max(DealQty_,MatchQty_);
   inline K_mf(TStkQty)  LeftQty  () const   { return DiffQty(AfQty_, DealQty_); }
   //最後一次刪減量:
   inline K_mf(TStkQty)  LastDecQty () const { return DiffQty(BfQty_, AfQty_); }
   //累計刪減量:
   inline K_mf(TStkQty)  TotDecQty () const  { return DiffQty(Qty_, AfQty_); }
   // 如此一來對Qty的操作就可以參考KPTS的源碼: bxSesApTx.cpp RptDef.cpp

   //跟交易所最後一次溝通(新增委託,查詢,刪單,減量)的結果(含MatchQty_)
   TStkQty        BfQty_;
   TStkQty        AfQty_;
   //MatchQty_: 1.根據[刪單.減量.查詢]結果,得到的成交數量(含DealQty_)
   //           2.當收到其他主機的[刪改回報]時: MatchQty_ = DealQty_
   TStkQty        MatchQty_;
   TStkQty        LostQty_;      //刪單減量時斷線,無法判斷是否成功的數量(93.05.12)
   TStkQty        DealQty_;      //已收到的成交回報[成交數量]
   TStkAmt        DealAmt_;      //        成交回報[累計價金]
   inline K_mf(TStkPri)  GetDealPri () const  { return DealQty_.is_0() ? TStkPri() : TStkPri(DealAmt_ / DealQty_); }

   TOrdFlowID     FlowID_;
   TScCode        ScCode_;       //最後一次[風控管制]之代碼
   TScForces      CfmForces_;    //已強迫過的風控項目
   TScForces      HisForces_;    //曾經被管制過的風控項目
   union {
      byte           LastTseCode_;  //交易所回覆 StatusCode
      EBulletinItem  BuItem_;       //公告項目
   };
   TOrdSource     LastSource_;      //最後一次委託要求的來源
   TOrdEmergency  Emergency_;       //緊急程度.委託流程進行中(TFlowStep),應判斷此欄位決定委託的優先權.
   TOrigSysOrdID  OrigSysOrdID_;    //原始[系統序號]: HostID+SysOrdID. 填入時機: (1)OrdFlow-NewOrd, (2)委託書同步時.

   typedef char_ary<char,80,0>   TMsg;
   TMsg           LastMsg_;
   TIvacNo        TMPIvacNo_;       //20110407eric Tandem 端回覆的客戶帳號未必為原委客戶帳號, FIX 回覆委回時應以此欄位為準
   TStkQty        TMPBfQty_;        //20110408eric Tandem 端回覆的改前量&改後量
   TStkQty        TMPAfQty_;

   //增加flag時,記得在建構時設定初始值.
   union {
      byte        Flags_;
      struct {
         byte     IsErOrder_ : 1,   // 錯帳申報
                  UnknowNew_ : 1,   // 新增委託時斷線,狀況不明.
                  ChgPriNew_ : 1,   // 因改價而產生的新委託.
                  NewOrdFlow_: 1,   // [新委託要求]流程進行中,UpdateFrom==uf_OrdFlow時,此旗標才有效.
                  IsBooked_  : 1,   // 自動圈存之委託己完成圈存動作(款/券)
                  Component_ : 1,   // 成份股旗標, 在新委託時或第一次收到時填入
                  ExceedBnd_ : 1,   // 外國股票價格超過上下界
                  WrntMktMkr_: 1;   // 20100623 權證流動量提供者新式刪單減量
      };
   };

   //IsTseOK()並不表示iord為本機新增的委託
   //         (例如:有可能先收到其他主機的委託回報,然後在本機作查詢,然後交易所回覆OK!)
   //         僅能確定此筆委託曾經與交易所溝通過,且交易所回覆OK
   inline K_mf(bool)  IsTseOK  () const   { return(TseOType_ != ot_Unknow); }
};
//---------------------------------------------------------------------------
struct TStkOrder : public TStkOrder_Rec
{
   //明細表,為了避免複製Ord時,會將明細表複製一份,所以使用 wise_ptr<>, 不用 wise_container<>
   // 1.成交明細,使用mutable,表示您修改Deals_,不會改變 DealQty_ 與 DealAmt_
   //            您必須使用 TOrdCenter 提供的 [成交回報]機制 來處理 Deals_
   mutable wise_ptr<TStkDeals>  Deals_;
   // 2.待處理的委託要求
   //   PendingReq_;
};
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
//---------------------------------------------------------------------------
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif

