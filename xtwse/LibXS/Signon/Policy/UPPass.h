//
// �ϥΪ̱K�X�޲z�F��.
//    class TUPPass;
//
// ����: $Id: UPPass.h,v 1.2 2005/01/14 07:53:58 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef UPPassH
#define UPPassH
//---------------------------------------------------------------------------
//
// �F���W�� = TUPPass::PolicyName
// �F�����e = TUPPass
//
#include "Signon/libnsSignonPolicy.h"
#include "Signon/Policy/PolicyBase.h"
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
struct TUserPassPolicy
{
   byte  MaxErrCount_;     // �̦h���~����.
   word  ValidDays_;       // ���ĤѼ�,0��ܥΤ[����.
   byte  MinLength_;       // �̤֪���.
   byte  HistoryCount_;    // �±K�X�O�d����.
   union {
      byte     Flags_;
      struct {
         byte  NeedAlpha_  : 1,  ///< �K�X�����]�t: �r��.
               NeedNumber_ : 1,  ///< �K�X�����]�t: �Ʀr.
               NeedSymbol_ : 1,  ///< �K�X�����]�t: �Ÿ�.
               DiffUserID_ : 1,  ///< �K�X�����PUserID���P.
               NeedUpper_  : 1,  ///< �K�X�����]�t: �j�g�r��.
               NeedLower_  : 1;  ///< �K�X�����]�t: �p�g�r��.
      };
   };
   byte  MaxCountFromSameIP_;  // [�ۦP]�ӷ��i���Ƶn�J����.(0=����)
   byte  MaxCountFromDiffIP_;  // [���P]�ӷ��i���Ƶn�J����.
                               // ���M���W�٥�IP,��������ϥ�IP�ӷ�.

   byte  MinDiffChars_; ///< Require passwords to contain at least [n] different chars.
   byte  ChgHours_;     ///< ���w�ɶ���,
   byte  MaxChgTimes_;  ///< �̦h���\�����ʦ���.
   word  Durations_;    // �h�֤ѥ��n�J�Y���i�n�J,0��ܥΤ[����.
   byte  Repeat_;       // �ۦP���r���̦h�s��X�{�h�֦�
   byte  IncDecRepeat_; // ���W��r���̦h����

   inline K_ctor  TUserPassPolicy  ()
      : MaxErrCount_(3)
      , ValidDays_(90)
      , MinLength_(7)
      , HistoryCount_(6)
      , Flags_(0)
      , MaxCountFromSameIP_(0)
      , MaxCountFromDiffIP_(0)
      , MinDiffChars_(0)
      , ChgHours_(0)
      , MaxChgTimes_(0)
      , Durations_(0)
      , Repeat_(0)
      , IncDecRepeat_(0)
      { NeedAlpha_  = true;
        NeedUpper_  = true;
        NeedLower_  = true;
        NeedNumber_ = true;
        NeedSymbol_ = false;
        DiffUserID_ = true;
      }
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
SIGPO_class TUPPass : public TTRecPolicy<TUserPassPolicy>
{
   typedef TTRecPolicy<TUserPassPolicy>   inherited;
public:
   inline K_ctor      TUPPass  (const TPolicyBaseRef& ref)  : inherited(ref) {}
   static const char  PolicyName[];
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Policy
} // namespace Kway
} // namespace Signon
//---------------------------------------------------------------------------
#endif

