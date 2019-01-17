//
// �ϥ��v/�s�Ѩϥ��v
//    class TUPTwStkRights;
//
// ����: $Id: UPTwStkRights.h,v 1.3 2003/12/03 02:58:59 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef UPTwStkRightsH
#define UPTwStkRightsH
//---------------------------------------------------------------------------
//
// �F���W�� = TUPTwStkRights::PolicyName
//            TUPTwStkRights::OddPolicyName
// �F�����e = TUPTwStkRights
//
#include "libnsTwStk.h"
#include "Signon/Policy/PolicyBase.h"
#include <bitset>
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
#ifdef K_EnumPacked_Bug
   typedef byte   TUserTwStkRightsItem;
   enum TUserTwStkRightsItem_t
#else
   enum TUserTwStkRightsItem
#endif
{  //�P ProgramTrade\LogonMgr\ExLogon.h �����w�q�����ۦP
         trLnMgr,       // �n�J��Ʈw�޲z��
         trLnMgrMCust,  // ���\���@�N�ޱb��
         trLnMgrMSale,  //         �N����~��

         trOrder,       // ���\�e�U�R�i��X
         trDelete,      // ���\����
         trChange,      // ���\��q
         trQuery,       // ���\�d��
         trCustNo,      // ���\��J�Ȥ�b��
         trSaleNo,      // ���\��J��~���N��
         trOrderNo,     // ���\��J�e�U�Ѹ�,������q����
         trAutoQty,     // ���\�۰ʭp��@�M�i��
         trSysMon,      // �t�κʬ�
         trChkMCust,    // �e�U��,�ˬd�N�ޱb��,�D�N�ޱb�����i�e�U
         trChkMSale,    // �e�U��,�ˬd�N����~��,�D�N����~�����i�e�U
         trQryBal,      // ���\�w�s�d��
         trQryCust,     // ���\�Ȥ��Ƭd��
         trRptMSale,    // �ݭn�^���N����~�����e�U��?
         trRevOrder,    // ���\�ϨR
         trTrialMul,    // ���\ * �պ�
         trTrialDiv,    // ���\ / �պ�
         trNoSc,        // �w�����Ѥ����\�e�U
         trErReport,    // ���\���b�ӳ�
         trReForce,     // ���\�j����L�ϥΪ̪��w���e�U��
         trMaExTabs,    // ���\�ϥΪ̺��@ Ex Tables, ����������ƪ�
         trMarkDeal,    // ���\�ϥΪ̤ĳ�
         trNoRptTermID, // �w�d�ϥΪ�,���̷��d���^���e�U
         trChkRevCrDb,  // ��R����.��歭��,�ˬd�O�_����R�ާ@
         trRptCanForce, // �Ȧ^����[�j���v��]��[�w���e�U��]
         trChkCfmForce, // �e�U�w���ˬd��,�ˬd CfmTForce �O�_�w�g�j���L
         trChkRvCustTotBSAmt, // �պ��,�ˬd�ϨR��,�O�_�|�W�L���W��
         trScChkDeal,         // �e�U�B���ˬd�e,���ˬd�w�����B��
         trCrossTerm,   //�i���d�d�ߧR���q
         trCrossGroup,  //�i��s�ըt�κʬ�

         trEnd,
} K_EnumPacked;
//---------------------------------------------------------------------------
struct TUserTwStkRights
{
   typedef std::bitset<trEnd>    TRightsFlag;
   TRightsFlag    Rights_;
   /*
   #ifdef __BORLANDC__
      #if sizeof(TRightsFlag) > ((trEnd+7)/8)
      #  error "���i�Nbitset<>�����s��!!"
      #endif
   #endif
   */
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TUPTwStkRights : public TTRecPolicy<TUserTwStkRights>
{
   typedef TTRecPolicy<TUserTwStkRights>  inherited;
public:
   inline K_ctor      TUPTwStkRights  (const TPolicyBaseRef& ref)  : inherited(ref) {}
   static const char  PolicyName[];
   static const char  OddPolicyName[];//�s�Ѩϥ��v
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Policy
} // namespace Kway
} // namespace Signon
//---------------------------------------------------------------------------
#endif

