/////////////////////////////////////////////////////////////////////////////
#if !defined(_FixStkOrder_h_)
#define _FixStkOrder_h_
//---------------------------------------------------------------------------
#include "QFixExt/QFixTagDef.h"
#include "FixStk.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace FIX  {
namespace Stk  {

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// struct TFsStkOrderRec
/////////////////////////////////////////////////////////////////////////////
// 修改此資料結構時, 需要連帶修改 FixStkOrdCenter_Aux.cpp 的 peek_room, into_room
struct TFsStkOrderRec
{
   #if defined(_FixTag_Twse_Ver2_)
      typedef Kstl::char_ary<char, 80, 0>  TLastMsg;
   #else
      typedef std::string                  TLastMsg;
   #endif
   
   enum {
      Reserve_DataType_Size = 8,
   };

   //==============================================
   // header
   TFixSenderSubID    SenderSubID_;       // 050
   TFixTargetSubID    TargetSubID_;       // 057
   //==============================================
   // Body
   TFixAccount        Account_;           // 001
   TFixClOrdID        ClOrdID_;           // 011
   TFixQty2           CumQty_;            // 014
   TFixHandlInst      HandlInst_;         // 021
   TFixOrderID        OrderID_;           // 037
   TFixQty2           OrderQty_;          // 038
   TFixOrdStatus      OrdStatus_;         // 039
   TFixOrdType        OrdType_;           // 040
   TFixOrigClOrdID    OrigClOrdID_;       // 041
   TFixPri2           Price_;             // 044
   TFixSymbol         Symbol_;            // 055
   TFixSide           Side_;              // 054
   TFixText           Text_;              // 058
   TFixTimeInForce    TimeInForce_;       // 059
   TFixExecType       ExecType_;          // 150
   TFixQty2           LeavesQty_;         // 151
   //==============================================
   // ext
   TFixRefOrdID       RefOrdID_;
   TFixOrdFlowID      FlowID_;
   TLastMsg           LastMsg_;
   //==============================================
   // Reserve
   TFixChar           ReseChar_[Reserve_DataType_Size];
   TFixQty2           ReseQty_ [Reserve_DataType_Size];
   TFixPri2           ResePri_ [Reserve_DataType_Size];
   //==============================================
   // 001.Account  (最原始的 Account 資料)
   TFixAccount        OrigAccount_;

   inline K_mf(TFixChar*) GetReseChar ()  { return &(ReseChar_[0]); }
   inline K_mf(TFixQty2*) GetReseQty  ()  { return &(ReseQty_ [0]); }
   inline K_mf(TFixPri2*) GetResePri  ()  { return &(ResePri_ [0]); }

   inline K_mf(const TFixChar*) GetReseChar () const  { return &(ReseChar_[0]); }
   inline K_mf(const TFixQty2*) GetReseQty  () const  { return &(ReseQty_ [0]); }
   inline K_mf(const TFixPri2*) GetResePri  () const  { return &(ResePri_ [0]); }
};
/////////////////////////////////////////////////////////////////////////////
// struct TFsStkOrder
/////////////////////////////////////////////////////////////////////////////
struct TFsStkOrder : public TFsStkOrderRec
{
};


/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif

/////////////////////////////////////////////////////////////////////////////
}; // namespace Stk
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_FixStkOrder_h_)
