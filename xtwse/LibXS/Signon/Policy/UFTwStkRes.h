//
// ���t�귽: [��ӥN��],[�d��],[�t�B��],[�պ�̦h����]...
//    class TUFTwStkRes;
//
// ����: $Id: UFTwStkRes.h,v 1.2 2005/12/28 05:45:55 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef UFTwStkResH
#define UFTwStkResH
//---------------------------------------------------------------------------
//
// �F���W�� = TUFTwStkRes::PolicyName
// �F�����e = TUFTwStkRes
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
   Kway::Tw::Stk::TBrokerID         BrokerID_;   //��ӥN��
   Kway::Tw::Stk::TTseOrdID::TTerm  TermNo_;     //�d��
   Kway::Tw::Stk::TSfGroup          SfGroup_;    //�H�ΰt�B��
   Kstl::fixed_num<word>            MaxOrdCount_;//�̦h�պⵧ��
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

