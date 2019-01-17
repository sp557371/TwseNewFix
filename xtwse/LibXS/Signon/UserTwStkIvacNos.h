//
// TwStk - �N�ޱb��
//
// ����: $Id: UserTwStkIvacNos.h,v 1.5 2005/04/26 03:24:59 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef UserTwStkIvacNosH
#define UserTwStkIvacNosH
//---------------------------------------------------------------------------
#include "Signon/Policy/UPTwStkIvacNos.h"
#include "PmBase.h"
#include "inn_file.h"
#include "SimpleRecorder.h"
#include "VALogger.h"
#include "libsneTwStk.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
/////////////////////////////////////////////////////////////////////////////
using Kway::Signon::Policy::TTriGroup;
using Kway::Signon::Policy::TTriSeq;
using Kway::Signon::Policy::TUserTwStkIvac;
/////////////////////////////////////////////////////////////////////////////
// �w�q�s�ɤ覡(�ɮ׳̤j4G(dword),�C��Rec�̤j64K(word))
typedef Kstl::rr_inn_file<dword,word>     TUserTwStkIvacInn;
//===========================================================================
// �w�q��ƪ�
struct TUserTwStkIvac_PolicyKey : public TPolicyKey {};//���γo�ؤ覡�w�q Key,�|�y��BCB���ͪ��{���X�����D
//..............................
struct TUserTwStkIvacs : public TSimpleMap_AddRoomKey<Tw::Stk::TIvacKey,TUserTwStkIvac,TUserTwStkIvacInn::rr_key>::TEvRecs
{
   std::string mutable   PolicyResult_;
   K_mf(TPolicyBaseRef)  GetPolicy  () const  { return Kway::Signon::Policy::MakeRecsPolicy(*this, PolicyResult_, (TUserTwStkIvac*)0); }

   typedef TUserTwStkIvac_PolicyKey    TMasterKey;
   TMasterKey  MasterKey_;
   inline K_mf(const TMasterKey&)  get_master_key  () const                   { return MasterKey_; }
   inline K_mf(void)               set_master_key  (const TMasterKey& mkey)   { MasterKey_ = mkey; }
   inline K_mf(std::string)        GetMasterKeyStr () const                   { return MasterKey_.as_string(); }
};
typedef Kstl::ev_map<TUserTwStkIvac_PolicyKey,TUserTwStkIvacs>   TUserTwStkIvacPolicy;
//---------------------------------------------------------------------------
//typedef TSimpleRecorder_Detail<Kstl::map2d_maker_key_find<map2d_maker_base<TUserTwStkIvacPolicy> >,
//                               TUserTwStkIvacInn,
//                               sizeof(TUserTwStkIvac)>//�n�s�JInn����Ƥj�p
//                               TUserTwStkIvacPolicy_Recorder;
typedef Kstl::map2d_maker_key_find<map2d_maker_base<TUserTwStkIvacPolicy> >   TUserTwStkIvacPolicy_KeyFinder_t;
K_typedef(TUserTwStkIvacPolicy_KeyFinder_t, TUserTwStkIvacPolicy_KeyFinder);

K_typedef_TSimpleRecorder_Detail(TUserTwStkIvacPolicy_Recorder,
                                 TUserTwStkIvacPolicy_KeyFinder,
                                 TUserTwStkIvacInn,
                                 sizeof(TUserTwStkIvac));
//===========================================================================
class  TUserTwStkIvacMgr : protected TUserTwStkIvacPolicy//�D��ƪ�
                         , protected TUserTwStkIvacPolicy_Recorder//2d������,�s�ɥ�
                         , public    TPmBaseVA
{
public:
   typedef TUserTwStkIvacPolicy        TMasterRecs;
   typedef TUserTwStkIvacs             TDetailRecs;
   typedef TMasterRecs                 TRecs;
   K_mf(TMasterRecs*)  get_recs  ()    { return this; }

protected:
   typedef TUserTwStkIvacPolicy_Recorder  InheritedRecorder;
   typedef TMasterRecs::const_iterator    const_iterator;

public:
   K_ctor  TUserTwStkIvacMgr  (const std::string& defaultPath,
                               TTableLog* masterLog, TTableLog2d* detailLog);

   friend class TPmBase;
   K_mf(TPolicyBaseRef)  GetPolicy  (const TPolicyKey& key, bool forLocalUse) const
   { return GetPolicy_TMap(*this, static_cast<const key_type&>(key)); }

   //�����ܤ�k
   struct TFieldMaker : public TVAKeyFieldMaker<TRecs>
   {  //Master-����ܸs�դ���
      K_mf(const TFieldDesp&)  GetFieldDesp  (field_n_type n) const
      { static TFieldDesp fdesp = { "�N�޲էO", 12, Kway::ta_xLeft_yCenter, Kway::ta_xLeft_yCenter, Kway::tfs_Fixed, };
        return fdesp; }
   }; // class TFieldMaker

private:
   //Detail-�����ܤ�k
   struct TvacDetail : public TVACreater {
      typedef TDetailRecs     TRecs;
      struct  TFieldMaker
      {  //Detail-������y�z
         typedef TRecs                    TEvContainer;
         typedef TEvContainer::iterator   iterator;
         typedef size_t                   field_n_type;

         K_mf(field_n_type)       GetFieldCount  () const;
         K_mf(const TFieldDesp&)  GetFieldDesp   (field_n_type n) const;
         K_mf(std::string) Get  (const iterator& i, field_n_type) const;
         K_mf(void)        Put  (TEvContainer&, iterator&, field_n_type, const std::string&);
      }; // class TFieldMaker
      typedef TTVALogger2d<TFieldMaker>   TVALogger;
      TVALogger  VALog_;
      K_ctor            TvacDetail   (TTableLog2d* log)  : VALog_(log) {}
      K_mf(TaViewArea)  CreateNewVA  ();
   }; // struct TvacDetail
   typedef TvacDetail::TVALogger::TViewArea    TDetailVA;
   TvacDetail  Detail_;

   //�� ViewAreaMgr �޲z�Ϊ� members
   typedef TTVALogger<TFieldMaker>  TVALogger;
   typedef TVALogger::TViewArea     TMasterVA;
   TVALogger  VALog_;
   K_mf(TaViewArea)  CreateNewVA           ();
   K_mf(size_t)      GetDetails            (const char**&);
   K_mf(TVACreater*) GetDetailVACreater    (size_t);
   K_mf(TVACreater*) OnMasterViewRowMoved  (TViewArea& masterVA, TViewArea*& detailVA, size_t detailID);

public:
   typedef TvacDetail::TFieldMaker     TDetailFieldMaker;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
} // namespace Signon
//---------------------------------------------------------------------------
#endif

