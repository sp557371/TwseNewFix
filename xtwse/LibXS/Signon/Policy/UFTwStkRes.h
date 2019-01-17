//
// 分配資源: [券商代號],[櫃號],[配額組],[試算最多筆數]...
//    class TUFTwStkRes;
//
// 版本: $Id: UFTwStkRes.h,v 1.2 2005/12/28 05:45:55 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef UFTwStkResH
#define UFTwStkResH
//---------------------------------------------------------------------------
//
// 政策名稱 = TUFTwStkRes::PolicyName
// 政策內容 = TUFTwStkRes
//
#include "Signon/Policy/PolicyBase.h"
#include "TwStk.h"
#include "TwStkTseOrdKey.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
namespace Policy {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
struct TUserTwStkRes
{
   Kway::Tw::Stk::TBrokerID         BrokerID_;   //券商代號
   Kway::Tw::Stk::TTseOrdID::TTerm  TermNo_;     //櫃號
   Kway::Tw::Stk::TSfGroup          SfGroup_;    //信用配額組
   Kstl::fixed_num<word>            MaxOrdCount_;//最多試算筆數
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TUFTwStkRes : public TTRecPolicy<TUserTwStkRes>
{
   typedef TTRecPolicy<TUserTwStkRes>  inherited;
public:
   inline K_ctor      TUFTwStkRes  (const TPolicyBaseRef& ref)  : inherited(ref) {}
   static const char  PolicyName[];
};
//---------------------------------------------------------------------------
typedef TUFTwStkRes  TUPTwStkRes;
/////////////////////////////////////////////////////////////////////////////
} // namespace Policy
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
#endif

