//
// 依據 [使用者來源] 之可登入 [群組]
//    class TUFGroups;
//
// 版本: $Id: UFGroups.h,v 1.1.1.1 2003/10/24 15:51:17 cvsroot Exp $
//
//---------------------------------------------------------------------------
#ifndef UFGroupsH
#define UFGroupsH
//---------------------------------------------------------------------------
//
// 政策名稱 = TUFGroups::PolicyName
// 政策內容 = TUFGroups
//
#include "Signon/libnsSignonPolicy.h"
#include "Signon/Policy/PolicyBase.h"
#include "char_ary.h"
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
typedef Kstl::char_ary<char, 4, ' '>   TUFGroup;
//---------------------------------------------------------------------------
SIGPO_class TUFGroups : public TTRecsPolicy<TUFGroup>
{
   typedef TTRecsPolicy<TUFGroup>   inherited;
public:
   inline K_ctor      TUFGroups   (const TPolicyBaseRef& ref)  : inherited(ref) { }
   static const char  PolicyName[];
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
} // namespace Policy
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
#endif
