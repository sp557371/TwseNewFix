/////////////////////////////////////////////////////////////////////////////
#if !defined(_TwseFixField_h_)
#define _TwseFixField_h_
//---------------------------------------------------------------------------
#include "src/Code/Field.h"
#include "QFixExt/QFixTagType.h"
#include "TwStkOrder.h"
#include "FixStkRptFlow.h"
#include "Nsa_Time.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway  {
namespace FIX   {
namespace FIELD {

/////////////////////////////////////////////////////////////////////////////
// Twse Field Num
/////////////////////////////////////////////////////////////////////////////
enum ETwseFixField
{
   TwseIvacnoFlag = 10000,
   TwseOrdType    = 10001,
   TwseExCode     = 10002,
   TwseWarrantLP  = 10003,
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace FIELD
}; // namespace FIX
}; // namespace Kway


/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;
   using namespace Nsa;

/////////////////////////////////////////////////////////////////////////////
// value
/////////////////////////////////////////////////////////////////////////////
const char OrdType_LimitAfterClose       = 'Z'; // FixedPrice
const char OrdStatus_PartialNew          = 'P';
const char ExecType_PartialNew           = 'P';
//---------------------------------------------------------------------------
const char TwseTargetSubID_Regular       = '0'; // Regular Trading
const char TwseTargetSubID_OddLots       = '2'; // OddLots Trading
const char TwseTargetSubID_FixedPrice    = '7'; // Fixed Price Trading (AP Code)
//---------------------------------------------------------------------------
const char TwseIvacnoFlag_Normal         = ' '; // Normal
const char TwseIvacnoFlag_ATM            = 'A'; // ATM
const char TwseIvacnoFlag_DMA_Order      = 'D'; // DMA Order
const char TwseIvacnoFlag_Internet       = 'I'; // Internet
const char TwseIvacnoFlag_Voice          = 'V'; // Voice
const char TwseIvacnoFlag_Normal_FIX     = '1'; // Normal(FIX)
const char TwseIvacnoFlag_ATM_FIX        = '2'; // ATM(FIX)
const char TwseIvacnoFlag_DMA_Order_FIX  = '3'; // DMA Order(FIX)
const char TwseIvacnoFlag_Internet_FIX   = '4'; // Internet(FIX)
const char TwseIvacnoFlag_Voice_FIX      = '5'; // Voice(FIX)
//---------------------------------------------------------------------------
const char TwseOrdType_Normal            = '0'; // 一般(非融資融券)
const char TwseOrdType_FinaPurcOnMarg    = '1'; // 融資(代辦)
const char TwseOrdType_FinaShortSell     = '2'; // 融券(代辦)
const char TwseOrdType_FirmPurcOnMarg    = '3'; // 融資(自辦)
const char TwseOrdType_FirmShortSell     = '4'; // 融券(自辦)
const char TwseOrdType_SblShortSell5     = '5'; // 借券賣出
const char TwseOrdType_SblShortSell6     = '6'; // 避險套利借券賣出
//---------------------------------------------------------------------------
const char TwseExCode_RegularFixedPrice  = '0'; // Regular, FixedPrice
const char TwseExCode_OddLots            = '2'; // OddLots
const char TwseExCode_OverBelowLimit     = '3'; // 表示外國股票委託價格超過確認上/下界
//---------------------------------------------------------------------------
const char TwseHandlInst_Default         = '1'; // 
const char TwseTimeInForce_Default       = '0'; // Day
//---------------------------------------------------------------------------
// 10003
const char TwseWarrantLP_Use             = 'Y'; // use second type of order
const char TwseWarrantLP_NotUse          = 'N'; // (default), not use, (FixedPrice, OddLots not use)

/////////////////////////////////////////////////////////////////////////////
// class TwseUtcTimeStamp
/////////////////////////////////////////////////////////////////////////////
class TwseUtcTimeStamp : public FIX::StringField
{
   typedef FIX::StringField  inherited;

public:
   TwseUtcTimeStamp (int field);
   TwseUtcTimeStamp (int field, const TKTime&);
   TwseUtcTimeStamp (int field, const TKTime&, const TKTimeMS&);

private:
   void Assign (const TKDateTimeMS&);
};
/////////////////////////////////////////////////////////////////////////////
// class TwseSendingTime (052)
/////////////////////////////////////////////////////////////////////////////
class TwseSendingTime : public TwseUtcTimeStamp
{
   typedef TwseUtcTimeStamp  inherited;

public:
   inline TwseSendingTime () : inherited(FIX::FIELD::SendingTime)  {}
};
/////////////////////////////////////////////////////////////////////////////
// class TwseTransactTime  (60)
/////////////////////////////////////////////////////////////////////////////
class TwseTransactTime : public TwseUtcTimeStamp
{
   typedef TwseUtcTimeStamp  inherited;

public:
   inline TwseTransactTime ()                                     : inherited(FIX::FIELD::TransactTime)          {}
   inline TwseTransactTime (const TKTime& p1)                     : inherited(FIX::FIELD::TransactTime, p1)      {}
   inline TwseTransactTime (const TKTime& p1, const TKTimeMS& p2) : inherited(FIX::FIELD::TransactTime, p1, p2)  {}
};
/////////////////////////////////////////////////////////////////////////////
// class TwseIvacnoFlag  (10000)
/////////////////////////////////////////////////////////////////////////////
class TwseIvacnoFlag : public FIX::CharField
{
   typedef FIX::CharField  inherited;

public:
   TwseIvacnoFlag ()           : inherited(FIX::FIELD::TwseIvacnoFlag)         {}
   TwseIvacnoFlag (char value) : inherited(FIX::FIELD::TwseIvacnoFlag, value)  {}
};
/////////////////////////////////////////////////////////////////////////////
// class TwseOrdType  (10001)
/////////////////////////////////////////////////////////////////////////////
class TwseOrdType : public FIX::CharField
{
   typedef FIX::CharField  inherited;

public:
   TwseOrdType ()           : inherited(FIX::FIELD::TwseOrdType)         {}
   TwseOrdType (char value) : inherited(FIX::FIELD::TwseOrdType, value)  {}
};
/////////////////////////////////////////////////////////////////////////////
// class TwseExCode (10002)
/////////////////////////////////////////////////////////////////////////////
class TwseExCode : public FIX::CharField
{
   typedef FIX::CharField  inherited;

public:
   TwseExCode ()           : inherited(FIX::FIELD::TwseExCode)         {}
   TwseExCode (char value) : inherited(FIX::FIELD::TwseExCode, value)  {}
};
/////////////////////////////////////////////////////////////////////////////
// class TwseWarrantLP (10003)
/////////////////////////////////////////////////////////////////////////////
class TwseWarrantLP : public FIX::CharField
{
   typedef FIX::CharField  inherited;

public:
   TwseWarrantLP ()           : inherited(FIX::FIELD::TwseWarrantLP)         {}
   TwseWarrantLP (char value) : inherited(FIX::FIELD::TwseWarrantLP, value)  {}
};


/////////////////////////////////////////////////////////////////////////////
// class TFixTwseTargetSubID (56)
/////////////////////////////////////////////////////////////////////////////
class TFixTwseTargetSubID : public FIX::TFixTargetSubID
{
   typedef FIX::TFixTargetSubID  inherited;
   typedef Tw::Stk::TTrxSegment  TTrxSegment;

public:
   inline K_ctor TFixTwseTargetSubID (const std::string&          a)      : inherited(a)  {}
   inline K_ctor TFixTwseTargetSubID (const FIX::TFixTargetSubID& a)      : inherited(a)  {}
   inline K_ctor TFixTwseTargetSubID (const TTrxSegment&          trxSeg)                 { assign_TrxSegment(trxSeg); }

   K_mf(void) assign_TrxSegment (const TTrxSegment&);

   K_mf(TTrxSegment) as_TrxSegment () const;
};
/////////////////////////////////////////////////////////////////////////////
// class TFixTwseSide (54)
/////////////////////////////////////////////////////////////////////////////
class TFixTwseSide : public FIX::TFixSide
{
   typedef FIX::TFixSide     inherited;
   typedef Tw::Stk::TBSCode  TBSCode;

public:
   inline K_ctor TFixTwseSide (const FIX::TFixSide& a)      : inherited(a)  {}
   inline K_ctor TFixTwseSide (const TBSCode&       bsCode)                 { assign_BSCode(bsCode); }

   K_mf(void) assign_BSCode (const TBSCode&);
   
   K_mf(TBSCode) as_BSCode () const;
};
/////////////////////////////////////////////////////////////////////////////
// class TFixTwseExecType (150)
/////////////////////////////////////////////////////////////////////////////
class TFixTwseExecType : public FIX::TFixExecType
{
   typedef FIX::TFixExecType     inherited;
   typedef inherited::orig_type  orig_type;

public:
   inline K_ctor TFixTwseExecType (orig_type                ch = inherited::Null_Val) : inherited(ch)            {}
   inline K_ctor TFixTwseExecType (const FIX::TFixExecType& a)                        : inherited(a.get_orig())  {}

   K_mf(std::string) as_desc () const;
};
/////////////////////////////////////////////////////////////////////////////
// class TFixTwseIvacnoFlag (10000)
/////////////////////////////////////////////////////////////////////////////
// 新單  時使用 TseOrderLine_
// 刪改查時使用 SubAcc_[0]
class TFixTwseIvacnoFlag : public FIX::TFixChar
{
   typedef FIX::TFixChar                        inherited;
   typedef inherited::orig_type                 orig_type;
   typedef char_ary<char, 1, ' '>               TTseOrderLine;
   typedef Tw::Stk::TOrdFields_ForNew::TSubAcc  TSubAcc;

public:
   inline K_ctor TFixTwseIvacnoFlag (orig_type            ch = inherited::Null_Val) : inherited(ch) {}
   inline K_ctor TFixTwseIvacnoFlag (const FIX::TFixChar& a)                        : inherited(a)  {}

   // assign_TseOrderLine
   inline K_mf(void) assign_TseOrderLine (const TTseOrderLine& tseOl)
      { inherited::assignT(tseOl[0]); }

   // assign_SubAcc
   inline K_mf(void) assign_SubAcc (const TSubAcc& subAcc)
      { inherited::assignT(subAcc[0]); }

   // assign_FixChar
   inline K_mf(void) assign_FixChar (const FIX::TFixChar& a)
      { inherited::Data_ = a.get_orig(); }

   // as_desc
   K_mf(std::string) as_desc () const;
   
   // as_TseOrderLine
   inline K_mf(void) as_TseOrderLine (TTseOrderLine& tseOl) const
      { tseOl.set(0, inherited::Data_); }
   
   // as_SubAcc
   inline K_mf(void) as_SubAcc (TSubAcc& subAcc) const
      { subAcc.set(0, inherited::Data_); }
};
/////////////////////////////////////////////////////////////////////////////
// class TFixTwseOrdType (10001)
/////////////////////////////////////////////////////////////////////////////
class TFixTwseOrdType : public FIX::TFixChar
{
   typedef FIX::TFixChar         inherited;
   typedef inherited::orig_type  orig_type;
   typedef Tw::Stk::TOType       TOType;

public:
   inline K_ctor TFixTwseOrdType (orig_type            ch = inherited::Null_Val) : inherited(ch) {}
   inline K_ctor TFixTwseOrdType (const FIX::TFixChar& a)                        : inherited(a)  {}

   K_mf(void) assign_OType (const TOType&);
   
   K_mf(TOType) as_OType () const;

   inline K_mf(void) assign_FixChar (const FIX::TFixChar& a)
      { inherited::Data_ = a.get_orig(); }
};
/////////////////////////////////////////////////////////////////////////////
// class TFixTwseExCode (10002)
/////////////////////////////////////////////////////////////////////////////
class TFixTwseExCode : public FIX::TFixChar
{
public:
   typedef FIX::TFixChar         inherited;
   typedef inherited::orig_type  orig_type;
   typedef Tw::Stk::TSfGroup     TSfGroup;
   typedef Tw::Stk::TTrxSegment  TTrxSegment;

public:
   inline K_ctor TFixTwseExCode (orig_type            ch = inherited::Null_Val) : inherited(ch) {}
   inline K_ctor TFixTwseExCode (const FIX::TFixChar& a)                        : inherited(a)  {}

   K_mf(void) assign_SfGroup (const TSfGroup&, const TTrxSegment&);
   
   inline K_mf(void) as_SfGroup (TSfGroup& sfGroup) const
      { sfGroup.assign(inherited::get_orig()); }

   inline K_mf(void) assign_FixChar (const FIX::TFixChar& a)
      { inherited::Data_ = a.get_orig(); }
};
/////////////////////////////////////////////////////////////////////////////
// class TFixTwseWarrantLP (10003)
/////////////////////////////////////////////////////////////////////////////
typedef FIX::TFixChar  TFixTwseWarrantLP;

/////////////////////////////////////////////////////////////////////////////
} // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseFixField_h_)
