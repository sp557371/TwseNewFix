//
// TwStk 的 [信用配額資料], >>> 尚未完成!!
//
// 版本: $Id: TwStkCredit.h,v 1.2 2003/11/11 05:15:29 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkCreditH
#define TwStkCreditH
//---------------------------------------------------------------------------
#include "fixed_num.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace Stk {
/////////////////////////////////////////////////////////////////////////////
#ifdef K_EnumPacked_Bug
   typedef byte   TStkCreditState;
   enum TStkCreditState_t
#else
   enum TStkCreditState
#endif
{
      scs_CrStop = 0x01,
      scs_DbStop = 0x02,
} K_EnumPacked;
//---------------------------------------------------------------------------
//為了簡化Client查詢時的處理:將整個TStkCredit直接傳送
//所以使用 #pragma pack 的選項
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
struct TStkCredit //市場信用交易(停資、停券、成數...)
{
   typedef Kstl::fixed_dec<byte,2>  TRate;   // 0.00～ 1.00

   TStkCreditState   State_;     //停資,停券...
   TRate             CrRate_;    //融資成數
   TRate             DbRate_;    //融券成數
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif
