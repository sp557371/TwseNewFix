//
// TwStk �� [�H�ΰt�B���], >>> �|������!!
//
// ����: $Id: TwStkCredit.h,v 1.2 2003/11/11 05:15:29 fonwin Exp $
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
//���F²��Client�d�߮ɪ��B�z:�N���TStkCredit�����ǰe
//�ҥH�ϥ� #pragma pack ���ﶵ
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
struct TStkCredit //�����H�Υ��(����B����B����...)
{
   typedef Kstl::fixed_dec<byte,2>  TRate;   // 0.00�� 1.00

   TStkCreditState   State_;     //����,����...
   TRate             CrRate_;    //�ĸꦨ��
   TRate             DbRate_;    //�Ĩ馨��
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
