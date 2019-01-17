//
// UserPass.�ϥΪ̱K�X�޲z�F��
//
// ����: $Id: UserPassPolicy.h,v 1.3 2005/04/26 03:25:19 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef UserPassPolicyH
#define UserPassPolicyH
//---------------------------------------------------------------------------
#include "./Signon/Policy/UPPass.h"
#include "PmBase.h"
#include "inn_file.h"
#include "SimpleRecorder.h"
#include "VALogger.h"
#include "SesExtab.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
/////////////////////////////////////////////////////////////////////////////
using Kway::Signon::Policy::TUserPassPolicy;
typedef Kstl::rr_inn_file<dword,word>  TUserPPInn;
/////////////////////////////////////////////////////////////////////////////
/*namespace for_bcb_bug_PP {//���[�W�o�q�L�Ϊ��w�q,�|�y�� BCB compiler �� TUserTwStkResRecorder ���w�q�����D!!
struct TBCB_bug_filler {};
typedef TSimpleMap_AddRoomKey<int,TBCB_bug_filler,TUserTwStkResInn::rr_key>::TEvRecs  TUFGroups;
}*/
//===========================================================================
// �w�q��ƪ�
struct  TUserPP_PolicyKey : public TPolicyKey {};//���γo�ؤ覡�w�q Key,�|�y��BCB���ͪ��{���X�����D
K_typedef_TSimpleRecorder_AddRoomKey(TUserPPRecorder,
                                     TUserPP_PolicyKey,
                                     TUserPassPolicy,
                                     TUserPPInn,
                                     sizeof(TUserPassPolicy));
//===========================================================================
class  TUserPPMgr : protected TUserPPRecorder
                  , public    TPmBaseVA
{
   typedef TUserPPRecorder    InheritedRecorder;

public:
   using InheritedRecorder::TRecs;
   using InheritedRecorder::get_recs;

   K_ctor  TUserPPMgr  (const std::string& defaultPath);

   K_mf(TPolicyBaseRef)  GetPolicy  (const TPolicyKey& key, bool forLocalUse) const
                { return forLocalUse ? GetPolicy_TRec(Recs_, static_cast<const TRecs::key_type&>(key), (TUserPassPolicy*)0)
                                     : TPolicyBaseRef(); }

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
   typedef TExtabLogUpdater<TExtabLog,TTExtabUpdater<TFieldMaker> >  TLogger;
   TLogger     Logger_;
   //�� ViewAreaMgr �޲z�Ϊ� members
   K_mf(TaViewArea)  CreateNewVA  ();
   typedef TTVALogger<TFieldMaker>  TVALogger;
   TVALogger  VALog_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
#endif


