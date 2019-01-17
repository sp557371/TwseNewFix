//
// TwStk - ����ϥ��v��
//
// ����: $Id: UserTwStkRights.h,v 1.4 2005/04/26 03:24:36 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef UserTwStkRightsH
#define UserTwStkRightsH
//---------------------------------------------------------------------------
#include "Signon/Policy/UPTwStkRights.h"
#include "PmBase.h"
#include "inn_file.h"
#include "SimpleRecorder.h"
#include "VALogger.h"
#include "libsneTwStk.h"
//---------------------------------------------------------------------------
//
// �ϥ��v
//
namespace Kway {
namespace Signon {
/////////////////////////////////////////////////////////////////////////////
using Kway::Signon::Policy::TUserTwStkRightsItem;
using Kway::Signon::Policy::TUserTwStkRights;
/////////////////////////////////////////////////////////////////////////////
// �w�q�s�ɤ覡(�ɮ׳̤j4G(dword),�C��Rec�̤j64K(word))
typedef Kstl::rr_inn_file<dword,word>     TUserTwStkRightsInn;
/////////////////////////////////////////////////////////////////////////////
namespace for_bcb_bug {//���[�W�o�q�L�Ϊ��w�q,�|�y�� BCB compiler �� TUserTwStkRightsRecorder ���w�q�����D!!
struct TBCB_bug_filler_r {};
typedef TSimpleMap_AddRoomKey<int,TBCB_bug_filler_r, TUserTwStkRightsInn::rr_key>::TEvRecs  TUFGroups;
}
//===========================================================================
// �w�q��ƪ�
struct  TUserTwStkRights_PolicyKey : public TPolicyKey {};//���γo�ؤ覡�w�q Key,�|�y��BCB���ͪ��{���X�����D
//typedef TSimpleRecorder_AddRoomKey<TUserTwStkRights_PolicyKey, TUserTwStkRights, TUserTwStkRightsInn, sizeof(TUserTwStkRights)>
//        TUserTwStkRightsRecorder;
K_typedef_TSimpleRecorder_AddRoomKey(TUserTwStkRightsRecorder,
                                     TUserTwStkRights_PolicyKey,
                                     TUserTwStkRights,
                                     TUserTwStkRightsInn,
                                     sizeof(TUserTwStkRights));
//===========================================================================
class  TUserTwStkRightsMgr : protected TUserTwStkRightsRecorder
                           , public    TPmBaseVA
{
   typedef TUserTwStkRightsRecorder    InheritedRecorder;

public:
   using InheritedRecorder::TRecs;
   using InheritedRecorder::get_recs;

   K_ctor  TUserTwStkRightsMgr  (const std::string& defaultPath, TTableLog*);

   K_mf(TPolicyBaseRef)  GetPolicy  (const TPolicyKey& key, bool forLocalUse) const
                { return GetPolicy_TRec(Recs_, static_cast<const TRecs::key_type&>(key), (TUserTwStkRights*)0); }

   //�����ܤ�k
   struct  TFieldMaker
   {
      typedef TRecs                    TEvContainer;
      typedef TEvContainer::iterator   iterator;
      typedef size_t                   field_n_type;

      K_mf(field_n_type)       GetFieldCount  () const;
      K_mf(const TFieldDesp&)  GetFieldDesp   (field_n_type n) const;
      K_mf(std::string) Get  (const iterator& i, field_n_type) const;
      K_mf(void)        Put  (TEvContainer&, iterator&, field_n_type, const std::string&);
   }; // class TFieldMaker

private:
   //�� ViewAreaMgr �޲z�Ϊ� members
   K_mf(TaViewArea)  CreateNewVA  ();
   typedef TTVALogger<TFieldMaker>  TVALogger;
   TVALogger  VALog_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
} // namespace Signon
//---------------------------------------------------------------------------
#endif


