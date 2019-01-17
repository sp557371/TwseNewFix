//---------------------------------------------------------------------------
#ifndef bxOrderH
#define bxOrderH
//---------------------------------------------------------------------------
#include <map>
#include "char_ary.h"
#include "bxDef.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace bx {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
K_class TOrdTx: public TTseControlHead
{
   typedef TTseControlHead     inherited;
public:
   Kstl::char_ary<char,4,' '>  BrokerId_;
   TPvcID                      PvcId_;
   Kstl::char_ary<char,5,' '>  OrderNo_;
   Kstl::char_ary<char,7,' '>  IvacNo_;
};
//---------------------------------------------------------------------------
K_class TT010 : public TOrdTx
{
   typedef TOrdTx               inherited;
   typedef TT010                this_type;
public:
   Kstl::char_ary<char,1,' '>  IvacNo_Flag_;
   Kstl::char_ary<char,6,' '>  StkNo_;
   Kstl::char_ary<char,6,' '>  Pri_;
   Kstl::char_ary<char,3,' '>  Qty_;
   Kstl::char_ary<char,1,' '>  BSCode_;
   Kstl::char_ary<char,1,' '>  ExchageCode_;
   Kstl::char_ary<char,1,' '>  OrderType_;
};
static const TSize      szT010 = sizeof(TT010);
//---------------------------------------------------------------------------
K_class TT020 : public TT010
{
   typedef TT010                inherited;
   typedef TT020                this_type;
public:
   Kstl::char_ary<char,6,' '>  OrderDate_;
   Kstl::char_ary<char,6,' '>  OrderTime_;
   Kstl::char_ary<char,3,' '>  BfQty_;
   Kstl::char_ary<char,3,' '>  AfQty_;
};
static const TSize      szT020 = sizeof(TT020);
//---------------------------------------------------------------------------
K_class TT030: public TTseControlHead
{
   typedef TTseControlHead      inherited;
   typedef TT030                this_type;
};
//---------------------------------------------------------------------------
K_class TT070: public TTseControlHead
{
   typedef TTseControlHead      inherited;
   typedef TT070                this_type;
public:
   Kstl::char_ary<char,4,' '>  BrokerId_;
   TPvcID                      PvcId_;
};
//---------------------------------------------------------------------------
K_class TO010: public TOrdTx
{
   typedef TOrdTx               inherited;
   typedef TO010                this_type;
public:
   Kstl::char_ary<char,6,' '>  StkNo_;
   Kstl::char_ary<char,6,' '>  Pri_;
   Kstl::char_ary<char,8,' '>  Qty_;
   Kstl::char_ary<char,1,' '>  BSCode_;
   Kstl::char_ary<char,1,' '>  TradeKind_;   // 1=錯帳違約,2=一般
   Kstl::char_ary<char,4,' '>  EBrokerId_;    // 錯帳之對方券商代號
};
static const TSize      szO010 = sizeof(TO010);
//---------------------------------------------------------------------------
K_class TO020: public TO010
{
   typedef TO010                inherited;
   typedef TO020                this_type;
public:
   Kstl::char_ary<char,6,' '>  OrderDate_;
   Kstl::char_ary<char,6,' '>  OrderTime_;
   Kstl::char_ary<char,8,' '>  BfQty_;
   Kstl::char_ary<char,8,' '>  AfQty_;
};
static const TSize      szO020 = sizeof(TO020);
//---------------------------------------------------------------------------
K_class TA010: public TOrdTx      // 拍賣委託
{
   typedef TOrdTx               inherited;
   typedef TA010                this_type;
public:
   Kstl::char_ary<char, 6,' '>  StkNo_;
   Kstl::char_ary<char, 6,' '>  Pri_;
   Kstl::char_ary<char,12,' '>  Qty_;
};
//---------------------------------------------------------------------------
K_class TA020: public TA010      // 拍賣委託回報
{
   typedef TA010                inherited;
   typedef TA020                this_type;
public:
   Kstl::char_ary<char, 6,' '>  OrderDate_;
   Kstl::char_ary<char, 6,' '>  OrderTime_;
   Kstl::char_ary<char,12,' '>  BfQty_;
   Kstl::char_ary<char,12,' '>  AfQty_;
   Kstl::char_ary<char, 6,' '>  BfPri_;
   Kstl::char_ary<char, 6,' '>  AfPri_;
};
//---------------------------------------------------------------------------
K_class TV010: public TOrdTx      // 標借委託
{
   typedef TOrdTx               inherited;
   typedef TV010                this_type;
public:
   Kstl::char_ary<char, 6,' '>  StkNo_;
   Kstl::char_ary<char, 7,' '>  Pri_;
   Kstl::char_ary<char, 6,' '>  Qty_;
   Kstl::char_ary<char, 1,' '>  Keep_;     //集中保管碼
};
//---------------------------------------------------------------------------
K_class TV020: public TV010      // 標借委託回報
{
   typedef TV010                inherited;
   typedef TV020                this_type;
public:
   Kstl::char_ary<char, 6,' '>  OrderDate_;
   Kstl::char_ary<char, 6,' '>  OrderTime_;
   Kstl::char_ary<char, 6,' '>  BfQty_;
   Kstl::char_ary<char, 6,' '>  AfQty_;
};
//---------------------------------------------------------------------------
K_class TE010: public TOrdTx      // 標購委託
{
   typedef TOrdTx               inherited;
   typedef TE010                this_type;
public:
   Kstl::char_ary<char, 6,' '>  StkNo_;
   Kstl::char_ary<char, 2,' '>  StkSeqNo_;    //標購股票之序號
   Kstl::char_ary<char, 6,' '>  Pri_;
   Kstl::char_ary<char,12,' '>  Qty_;
};
//---------------------------------------------------------------------------
K_class TE020: public TE010      // 標購委託回報
{
   typedef TE010                inherited;
   typedef TE020                this_type;
public:
   Kstl::char_ary<char, 6,' '>  OrderDate_;
   Kstl::char_ary<char, 6,' '>  OrderTime_;
   Kstl::char_ary<char,12,' '>  BfQty_;
   Kstl::char_ary<char,12,' '>  AfQty_;
   Kstl::char_ary<char, 6,' '>  BfPri_;
   Kstl::char_ary<char, 6,' '>  AfPri_;
};
//---------------------------------------------------------------------------
K_class TE110: public TOrdTx      // 證金標購委託
{
   typedef TOrdTx               inherited;
   typedef TE110                this_type;
public:
   Kstl::char_ary<char, 6,' '>  StkNo_;
   Kstl::char_ary<char, 2,' '>  StkSeqNo_;    //標購股票之序號
   Kstl::char_ary<char, 6,' '>  Pri_;
   Kstl::char_ary<char,12,' '>  Qty_;
};
//---------------------------------------------------------------------------
K_class TE120: public TE110      // 證金標購委託回報
{
   typedef TE110                inherited;
   typedef TE120                this_type;
public:
   Kstl::char_ary<char, 8,' '>  OrderDate_;
   Kstl::char_ary<char, 6,' '>  OrderTime_;
   Kstl::char_ary<char, 2,' '>  OrderTimeMS_;
   Kstl::char_ary<char,12,' '>  BfQty_;
   Kstl::char_ary<char,12,' '>  AfQty_;
   Kstl::char_ary<char, 6,' '>  BfPri_;
   Kstl::char_ary<char, 6,' '>  AfPri_;
};
//---------------------------------------------------------------------------
/*
K_class TS010: public TTseControlHead
{
   typedef TTseControlHead     inherited;
   typedef TS010               this_type;
public:
   Kstl::char_ary<char,4,' '>  BrokerId_;
   Kstl::char_ary<char,5,' '>  OrderNo_;
   Kstl::char_ary<char,6,' '>  StkNo_;
   Kstl::char_ary<char,4,' '>  Qty_;
   Kstl::char_ary<char,6,' '>  Pri_;
   Kstl::char_ary<char,1,' '>  BSCode_;
};
//---------------------------------------------------------------------------
K_class TS020: public TS010
{
   typedef TS010               inherited;
   typedef TS020               this_type;
};
//---------------------------------------------------------------------------
K_class TS030: public TTseControlHead
{
   typedef TTseControlHead     inherited;
   typedef TS030               this_type;
public:
   Kstl::char_ary<char,4,' '>  BrokerId_;
   Kstl::char_ary<char,5,' '>  OrderNo_;
   Kstl::char_ary<char,6,' '>  StkNo_;
   Kstl::char_ary<char,4,' '>  AccBrkId_;
   Kstl::char_ary<char,7,' '>  IvacNo_;
   Kstl::char_ary<char,4,' '>  ErrBrkId_;
   Kstl::char_ary<char,1,' '>  BSCode_;
   Kstl::char_ary<char,6,' '>  Pri_;
   Kstl::char_ary<char,4,' '>  Qty_;
};
//---------------------------------------------------------------------------
K_class TS040: public TTseControlHead
{
   typedef TTseControlHead     inherited;
   typedef TS040               this_type;
public:
   Kstl::char_ary<char,4,' '>  BrokerId_;
   Kstl::char_ary<char,5,' '>  OrderNo_;
   Kstl::char_ary<char,6,' '>  StkNo_;
   Kstl::char_ary<char,6,' '>  StkName_;
   Kstl::char_ary<char,4,' '>  AccBroker_;
   Kstl::char_ary<char,7,' '>  IvacNo_;
   Kstl::char_ary<char,4,' '>  ErrBroker_;
   Kstl::char_ary<char,1,' '>  BSCode_;
   Kstl::char_ary<char,6,' '>  Pri_;
   Kstl::char_ary<char,4,' '>  Qty_;
   Kstl::char_ary<char,6,' '>  InputTime_;
};
//---------------------------------------------------------------------------
K_class TS050: public TTseControlHead
{
   typedef TTseControlHead     inherited;
   typedef TS050               this_type;
public:
   Kstl::char_ary<char,4,' '>  BrokerId_;
   Kstl::char_ary<char,5,' '>  OrderNo_;
   Kstl::char_ary<char,6,' '>  StkNo_;
   Kstl::char_ary<char,6,' '>  Pri_;
   Kstl::char_ary<char,4,' '>  Qty_;
   Kstl::char_ary<char,4,' '>  BuyBroker_;
};
//---------------------------------------------------------------------------
K_class TS060: public TTseControlHead
{
   typedef TTseControlHead     inherited;
   typedef TS060               this_type;
public:
   Kstl::char_ary<char,4,' '>  BrokerId_;
   Kstl::char_ary<char,5,' '>  OrderNo_;
   Kstl::char_ary<char,6,' '>  StkNo_;
   Kstl::char_ary<char,6,' '>  StkName_;
   Kstl::char_ary<char,6,' '>  Pri_;
   Kstl::char_ary<char,4,' '>  Qty_;
   Kstl::char_ary<char,4,' '>  BuyBroker_;
   Kstl::char_ary<char,6,' '>  InputTime_;
   Kstl::char_ary<char,6,' '>  ConfirmTime_;
};
//---------------------------------------------------------------------------
K_class TS070: public TTseControlHead
{
   typedef TTseControlHead     inherited;
   typedef TS070               this_type;
public:
   Kstl::char_ary<char,4,' '>  BrokerId_;
   Kstl::char_ary<char,4,' '>  SellBroker_;
   Kstl::char_ary<char,5,' '>  OrderNoSell_;
   Kstl::char_ary<char,5,' '>  OrderNoBuy_;
};
//---------------------------------------------------------------------------
K_class TS080: public TS070
{
   typedef TS070               inherited;
   typedef TS080               this_type;
public:
   Kstl::char_ary<char,6,' '>  StkNo_;
   Kstl::char_ary<char,6,' '>  StkName_;
   Kstl::char_ary<char,6,' '>  Pri_;
   Kstl::char_ary<char,4,' '>  Qty_;
   Kstl::char_ary<char,6,' '>  InputTime_;
   Kstl::char_ary<char,6,' '>  ConfirmTime_;
};
//---------------------------------------------------------------------------
K_class TS090: public TTseControlHead
{
   typedef TTseControlHead     inherited;
   typedef TS090               this_type;
public:
   Kstl::char_ary<char,6,' '>  StkNo_;
};
//---------------------------------------------------------------------------
struct TS100Body
{
   Kstl::char_ary<char,6,' '>  StkNo_;
   Kstl::char_ary<char,6,' '>  StkName_;
   Kstl::char_ary<char,1,' '>  HighUd_;
   Kstl::char_ary<char,6,'0'>  HighPrice_;
   Kstl::char_ary<char,1,' '>  LowUd_;
   Kstl::char_ary<char,6,'0'>  LowPrice_;
   Kstl::char_ary<char,1,' '>  LastUd_;
   Kstl::char_ary<char,6,'0'>  LastPrice_;
   Kstl::char_ary<char,1,' '>  UpDown_;
   Kstl::char_ary<char,5,'0'>  UdPrice_;
   Kstl::char_ary<char,5,'0'>  MthRecord_;
   Kstl::char_ary<char,6,'0'>  MthQty_;
   Kstl::char_ary<char,8,'0'>  MthAmount_;
};
//---------------------------------------------------------------------------
K_class TS100: public TTseControlHead
{
   typedef TTseControlHead     inherited;
   typedef TS100               this_type;
public:
   Kstl::char_ary<char,2,' '>  RecCount_;
   TS100Body                   Body_[10];
};
//---------------------------------------------------------------------------
K_class TS110: public TTseControlHead
{
   typedef TTseControlHead     inherited;
   typedef TS110               this_type;
public:
   Kstl::char_ary<char,6,' '>  StkNo_;
   Kstl::char_ary<char,1,' '>  BSCode_;
};
//---------------------------------------------------------------------------
struct TS120Body
{
   Kstl::char_ary<char,4,' '>  DecBrkId_;
   Kstl::char_ary<char,8,' '>  DecBrkName_;
   Kstl::char_ary<char,1,' '>  BSCode_;
   Kstl::char_ary<char,6,' '>  Pri_;
   Kstl::char_ary<char,4,' '>  Qty_;
};
//---------------------------------------------------------------------------
K_class TS120: public TTseControlHead
{
   typedef TTseControlHead     inherited;
   typedef TS120               this_type;
public:
   Kstl::char_ary<char,2,' '>  RecCount_;
   Kstl::char_ary<char,6,' '>  StkNo_;
   TS120Body                   Body_[10];
};
*/
//---------------------------------------------------------------------------
enum TOrderErrorCode{
   oec_Success           = 0,
   oec_TimeIsOver        = 1,
   oec_TimeIsEarly       = 2,
   oec_QueryLaterOn      = 3,
   oec_WaitForMatch      = 4,
   oec_OrderNotFound     = 5,
   oec_FunCodeErr        = 11,
   oec_BrokerNoErr       = 12,
   oec_BranchNoErr       = 13,
   oec_IvacNoErr         = 14,
   oec_PvcIdErr          = 15,
   oec_TermIdErr         = 16,
   oec_SeqNoErr          = 18,
   oec_FundTradeOnly     = 19,
   oec_StkNoErr          = 20,
   oec_PriErr            = 21,
   oec_QtyErr            = 22,
   oec_BSCodeErr         = 24,
   oec_OrderTypeErr      = 25,
   oec_ExchangeCodeErr   = 26,
   oec_ForNotAllowed     = 28,
   oec_TrustTradeViolate = 29,
   oec_QtyOverLimit      = 30,
   oec_QtyWasCut         = 31,
   oec_DelOverQty        = 32,
   oec_ChgDelQryOnly     = 33,
   oec_TradeSuspended    = 34,
   oec_BuyQtyOverLimit   = 35,
   oec_SellQtyOverLimit  = 36,
   oec_FrnPriceOver      = 37,
   oec_ErrOverLimit      = 89,
   oec_CallTse           = 99
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
   #pragma pack(pop)
#else
   #pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
} // namespace bx
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif
