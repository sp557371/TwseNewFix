//
// TwStk - ����귽
//
// ����: $Id: UserTwStkRes.h,v 1.4 2005/04/26 03:24:48 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef UserTwStkResH
#define UserTwStkResH
//---------------------------------------------------------------------------
#include "Signon/Policy/UFTwStkRes.h"
#include "PmBase.h"
#include "inn_file.h"
#include "SimpleRecorder.h"
#include "VALogger.h"
#include "libsneTwStk.h"
//---------------------------------------------------------------------------
//
// ���t�귽: [��ӥN��],[�d��],[�t�B��],[�պ�̦h����]...
//
namespace Kway {
namespace Signon {
/////////////////////////////////////////////////////////////////////////////
using Kway::Signon::Policy::TUserTwStkRes;
typedef Kstl::rr_inn_file<dword,word>  TUserTwStkResInn;
/////////////////////////////////////////////////////////////////////////////
namespace for_bcb_bug_TwStkRes {//���[�W�o�q�L�Ϊ��w�q,�|�y�� BCB compiler �� TUserTwStkResRecorder ���w�q�����D!!
struct TBCB_bug_filler {};
typedef TSimpleMap_AddRoomKey<int,TBCB_bug_filler,TUserTwStkResInn::rr_key>::TEvRecs  TUFGroups;
}
//===========================================================================
// �w�q��ƪ�
struct  TUserTwStkRes_PolicyKey : public TPolicyKey {};//���γo�ؤ覡�w�q Key,�|�y��BCB���ͪ��{���X�����D
//typedef TSimpleRecorder_AddRoomKey<TUserTwStkRes_PolicyKey, TUserTwStkRes, TUserTwStkResInn, sizeof(TUserTwStkRes)>
//        TUserTwStkResRecorder;
K_typedef_TSimpleRecorder_AddRoomKey(TUserTwStkResRecorder,
                                     TUserTwStkRes_PolicyKey,
                                     TUserTwStkRes,
                                     TUserTwStkResInn,
                                     sizeof(TUserTwStkRes));
//===========================================================================
class  TUserTwStkResMgr : protected TUserTwStkResRecorder
                        , public    TPmBaseVA
{
   typedef TUserTwStkResRecorder    InheritedRecorder;

public:
   typedef InheritedRecorder::TRecs    TRecs;
   using InheritedRecorder::get_recs;

   K_ctor  TUserTwStkResMgr  (const std::string& defaultPath, TTableLog*);

   K_mf(TPolicyBaseRef)  GetPolicy  (const TPolicyKey& key, bool forLocalUse) const
                { return GetPolicy_TRec(Recs_, static_cast<const TRecs::key_type&>(key), (TUserTwStkRes*)0); }

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


