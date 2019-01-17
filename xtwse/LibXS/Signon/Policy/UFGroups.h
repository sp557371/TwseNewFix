//
// �̾� [�ϥΪ̨ӷ�] ���i�n�J [�s��]
//    class TUFGroups;
//
// ����: $Id: UFGroups.h,v 1.1.1.1 2003/10/24 15:51:17 cvsroot Exp $
//
//---------------------------------------------------------------------------
#ifndef UFGroupsH
#define UFGroupsH
//---------------------------------------------------------------------------
//
// �F���W�� = TUFGroups::PolicyName
// �F�����e = TUFGroups
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
