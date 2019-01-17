//
// 代管營業員
//
// 版本: $Id: UPTwStkSaleNos.h,v 1.2 2003/12/03 03:00:33 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef UPTwStkSaleNosH
#define UPTwStkSaleNosH
//---------------------------------------------------------------------------
//
// 政策名稱 = TUPTwStkSaleNos::PolicyName
// 政策內容 = TUPTwStkSaleNos
//
#include "Signon/Policy/PolicyBase.h"
#include "TwStkSaleNo.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
namespace Policy {
//---------------------------------------------------------------------------
using Kway::Tw::Stk::TSaleKey;
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
struct TUserTwStkSale
{
   bool  DefaultSaleNo_;

   K_ctor  TUserTwStkSale  ()  : DefaultSaleNo_(false)  {}
};
//---------------------------------------------------------------------------
struct TUserTwStkSaleRec : public TSaleKey,
                           public TUserTwStkSale
{
   inline K_ctor  TUserTwStkSaleRec  (const TSaleKey& key)  : TSaleKey(key) { }
   inline K_ctor  TUserTwStkSaleRec  ()                     { }
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TUPTwStkSaleNos : public TTRecsPolicy<TUserTwStkSaleRec>
{
   typedef TTRecsPolicy<TUserTwStkSaleRec>   inherited;
public:
   inline K_ctor      TUPTwStkSaleNos  (const TPolicyBaseRef& ref)   : inherited(ref) { }
   static const char  PolicyName[];
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Policy
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
#endif
