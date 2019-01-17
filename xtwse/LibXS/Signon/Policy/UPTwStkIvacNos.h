//
// �N�ޱb��
//
// ����: $Id: UPTwStkIvacNos.h,v 1.3 2003/12/03 03:00:33 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef UPTwStkIvacNosH
#define UPTwStkIvacNosH
//---------------------------------------------------------------------------
//
// �F���W�� = TUPTwStkIvacNos::PolicyName
// �F�����e = TUPTwStkIvacNos
//
#include "libnsTwStk.h"
#include "Signon/Policy/PolicyBase.h"
#include "fixed_num.h"
#include "TwStkIvacNo.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
namespace Policy {
using Kway::Tw::Stk::TIvacKey;
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
typedef Kstl::fixed_num<uint16>  TTriGroup;  //�պ�էO
typedef Kstl::fixed_num<uint16>  TTriSeq;    //�պⶶ��
//---------------------------------------------------------------------------
struct TUserTwStkIvac      //�ϥΪ�[�N�ޱb��]����@�b�����e
{
   TTriGroup   TriGroup_;  //�պ�էO
   bool        NoTrial_;   //���ѻP�պ�
   TTriSeq     TriSeq_;    //�պⶶ��

   inline K_ctor  TUserTwStkIvac  ()  : NoTrial_(false) {}
};
//---------------------------------------------------------------------------
struct TUserTwStkIvacRec : public TIvacKey,
                           public TUserTwStkIvac
{
   inline K_ctor  TUserTwStkIvacRec  (const TIvacKey& key)  : TIvacKey(key) { }
   inline K_ctor  TUserTwStkIvacRec  ()                     { }
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TUPTwStkIvacNos : public TTRecsPolicy<TUserTwStkIvacRec>
{
   typedef TTRecsPolicy<TUserTwStkIvacRec>   inherited;
public:
   inline K_ctor      TUPTwStkIvacNos  (const TPolicyBaseRef& ref)  : inherited(ref) { }
   static const char  PolicyName[];
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Policy
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
#endif
 
