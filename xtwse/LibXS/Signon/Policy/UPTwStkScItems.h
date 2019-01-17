//
// ��������/�s�ѭ�������
//
// ����: $Id: UPTwStkScItems.h,v 1.3 2005/04/26 03:23:06 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef UPTwStkScItemsH
#define UPTwStkScItemsH
//---------------------------------------------------------------------------
//
// �F���W�� = TUPTwStkScItems::PolicyName
//            TUPTwStkScItems::OddPolicyName
// �F�����e = TUPTwStkScItems
//
#include "libnsTwStk.h"
#include "Signon/Policy/PolicyBase.h"
#include <string.h>
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
   typedef char   TScLevel;
   enum TScLevel_t
#else
   enum TScLevel
#endif
{
   slCanForce     = '0',   //�i�j��
   slCannotForce  = '1',   //����j��
   slDontCheck    = '2',   //���ˬd
   slSend2Mgr     = '3',   //�e�D��
   slMgrPass      = '4',   //���K�X(Keyin�ݿ�J���K�X��j��)
} K_EnumPacked;//�]�� gcc3.2-3.3�� bug,�[�W __attribute__((packed)); �|��compile�����~
//---------------------------------------------------------------------------
struct TUserTwStkScItems
{
   inline K_ctor  TUserTwStkScItems  ()  { memset(this, slCannotForce, sizeof(*this)); }

   union {
      TScLevel    ScItems_[80];
      #ifdef __BORLANDC__
         #if sizeof(TScLevel) != 1
         #  error "�Э��s�w�q ScItems_[] �]�\�A�����b�M����(.bpr)�W�[ -b- ���ﶵ"
         #endif
      #endif
      struct {
         TScLevel    Fc0;
         TScLevel    FcNoT30;       // T30�L���
         TScLevel    FcPriceLimit;  // �W�L���^��
         TScLevel    FcFullT;       // ���B���
         TScLevel    FcNoPriLmt;    // �L���^�T�Ѳ��������e�U����
         TScLevel    Fc5;
         TScLevel    Fc6;
         TScLevel    Fc7;
         TScLevel    Fc8;
         TScLevel    Fc9;

         TScLevel    FcStkDeposit;  //�����ĵ�ܪ�(�w���ڨ�)
         TScLevel    FcStkSC;       //��Ӥ�����(�b��+�Ѳ�)
         TScLevel    FcStkCpSC;     //(��@��~���I)+�Ѳ�.�w��
         TScLevel    FcIvacStkSC;   //�ӤH�ӪѦw��(�S�w�b��+�Ѳ�)
         TScLevel    Fc14;
         TScLevel    Fc15;
         TScLevel    Fc16;
         TScLevel    Fc17;
         TScLevel    Fc18;
         TScLevel    Fc19;

         TScLevel    FcCrStop;      // ����ĸ�
         TScLevel    FcDbStop;      // ����Ĩ�
         TScLevel    FcCrQty;       // �ĸ�t�B����
         TScLevel    FcDbQty;       // �Ĩ�t�B����
         TScLevel    FcToBal;       // �t�B��0�ΰ����,�����\��R
         TScLevel    FcNoFinaStk;   // �Ҫ��t�B������
         TScLevel    FcCrCQty;      // ��t�B����(��������O�ϥ� ot_CrC)
         TScLevel    FcDbCQty;      // ��t�B����(��������O�ϥ� ot_DbC)
         TScLevel    Fc28;
         TScLevel    FcBalSBLQty;   // 20090324 �]���H�Υ���Nsbl �w�s���ޥѭ�FcBalDbQty�W�ߥX��

         TScLevel    FcBalGnQty; //�w�s.���W��.�ż�
         TScLevel    FcBalCrQty;
         TScLevel    FcBalDbQty;
         TScLevel    FcTotBSAmt; //�R��X�p���W��
         TScLevel    FcBalCrAmt;
         TScLevel    FcBalDbAmt;
         TScLevel    FcCr1StkAmt;//��ѿĸ�.�Ĩ�W�L
         TScLevel    FcDb1StkAmt;
         TScLevel    FcWarnAmt;  //���I��浧ĵ��
         TScLevel    FcUpLmtAmt; //���I��浧�̰�

         TScLevel    FcCtrlBS; //���ި�
         TScLevel    FcCtrlCD; //�H�κި�
         TScLevel    FcRisk;   //���I��ި�
         TScLevel    FcAmtLevel1;   //�b���e�U���B���h�ި�Ĥ@��
         TScLevel    FcAmtLevel2;   //�b���e�U���B���h�ި�Ĥ@��
         TScLevel    FcAmtLevel3;   //�b���e�U���B���h�ި�Ĥ@��
         TScLevel    FcDormant;     //�R���(�Ĥ@���e�U�j����, �Y�Ѱ��R��᪬�A)
         TScLevel    FcGroupAmt;    // 20070820�s�է���B��
         TScLevel    FcNoKYC;       // 20080220�LKYC
         TScLevel    FcExceedBound; // 20090617 �~��Ѳ��W�L�W�U��
      };
   };
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TUPTwStkScItems : public TTRecPolicy<TUserTwStkScItems>
{
   typedef TTRecPolicy<TUserTwStkScItems>  inherited;
public:
   inline K_ctor      TUPTwStkScItems  (const TPolicyBaseRef& ref)  : inherited(ref) {}
   static const char  PolicyName[];
   static const char  OddPolicyName[];//�s�ѭ�������
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Policy
} // namespace Kway
} // namespace Signon
//---------------------------------------------------------------------------
#endif
