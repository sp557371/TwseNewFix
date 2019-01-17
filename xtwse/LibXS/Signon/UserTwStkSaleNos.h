//
// TwStk - �N����~��
//
// ����: $Id: UserTwStkSaleNos.h,v 1.5 2005/04/26 03:24:23 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef UserTwStkSaleNosH
#define UserTwStkSaleNosH
//---------------------------------------------------------------------------
#include "Signon/Policy/UPTwStkSaleNos.h"
#include "PmBase.h"
#include "inn_file.h"
#include "SimpleRecorder.h"
#include "VALogger.h"
#include "libsneTwStk.h"
//---------------------------------------------------------------------------
//
// �N����~��
//
namespace Kway {
namespace Signon {
/////////////////////////////////////////////////////////////////////////////
using Kway::Tw::Stk::TSaleKey;
using Kway::Signon::Policy::TUserTwStkSale;
/////////////////////////////////////////////////////////////////////////////
// �w�q�s�ɤ覡(�ɮ׳̤j4G(dword),�C��Rec�̤j64K(word))
typedef Kstl::rr_inn_file<dword,word>     TUserTwStkSaleInn;
//===========================================================================
// �w�q��ƪ�
struct  TUserTwStkSale_PolicyKey: public TPolicyKey {};//���γo�ؤ覡�w�q Key,�|�y��BCB���ͪ��{���X�����D
//..............................
struct TUserTwStkSales : public TSimpleMap_AddRoomKey<TSaleKey,TUserTwStkSale,TUserTwStkSaleInn::rr_key>::TEvRecs
{
   std::string mutable   PolicyResult_;
   K_mf(TPolicyBaseRef)  GetPolicy  () const  { return Kway::Signon::Policy::MakeRecsPolicy(*this, PolicyResult_,(TUserTwStkSale*)0); }

   typedef TUserTwStkSale_PolicyKey    TMasterKey;
   TMasterKey  MasterKey_;
   inline K_mf(const TMasterKey&)  get_master_key  () const                   { return MasterKey_; }
   inline K_mf(void)               set_master_key  (const TMasterKey& mkey)   { MasterKey_ = mkey; }
   inline K_mf(std::string)        GetMasterKeyStr () const                   { return MasterKey_.as_string(); }
};
typedef Kstl::ev_map<TUserTwStkSale_PolicyKey,TUserTwStkSales> TUserTwStkSalePolicy;
//---------------------------------------------------------------------------
//typedef TSimpleRecorder_Detail<Kstl::map2d_maker_key_find<map2d_maker_base<TUserTwStkSalePolicy> >,
//                               TUserTwStkSaleInn,
//                               sizeof(TUserTwStkSale)>//�n�s�JInn����Ƥj�p
//                               TUserTwStkSalePolicy_Recorder;
typedef Kstl::map2d_maker_key_find<map2d_maker_base<TUserTwStkSalePolicy> >   TUserTwStkSalePolicy_KeyFinder_t;
K_typedef(TUserTwStkSalePolicy_KeyFinder_t, TUserTwStkSalePolicy_KeyFinder);

K_typedef_TSimpleRecorder_Detail(TUserTwStkSalePolicy_Recorder,
                                 TUserTwStkSalePolicy_KeyFinder,
                                 TUserTwStkSaleInn,
                                 sizeof(TUserTwStkSale));
//===========================================================================
class  TUserTwStkSaleMgr : protected TUserTwStkSalePolicy//�D��ƪ�
                         , protected TUserTwStkSalePolicy_Recorder//2d������,�s�ɥ�
                         , public  TPmBaseVA
{
protected:
   typedef TUserTwStkSalePolicy_Recorder  InheritedRecorder;
   typedef TMasterRecs::const_iterator    const_iterator;

public:
   typedef TUserTwStkSalePolicy  TMasterRecs;
   typedef TMasterRecs           TRecs;
   typedef TUserTwStkSales       TDetailRecs;
   inline K_mf(TMasterRecs*)     get_recs  ()   { return this; }

   //�غc
   K_ctor  TUserTwStkSaleMgr  (const std::string& defaultPath, TTableLog*, TTableLog2d*);

   friend class TPmBase;
   K_mf(TPolicyBaseRef)  GetPolicy  (const TPolicyKey& key, bool forLocalUse) const
   { return GetPolicy_TMap(*this, static_cast<const key_type&>(key)); }

   //�����ܤ�k
   struct TFieldMaker : public TVAKeyFieldMaker<TRecs>
   {  //Master-����ܸs�դ���
      K_mf(const TFieldDesp&)  GetFieldDesp   (field_n_type n) const
      { static TFieldDesp fdesp = { "�N�޲էO", 12, Kway::ta_xLeft_yCenter, Kway::ta_xLeft_yCenter, Kway::tfs_Null, };
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
   };
   typedef TvacDetail::TVALogger::TViewArea    TDetailVA;
   TvacDetail  Detail_;

   //�� ViewAreaMgr �޲z�Ϊ� members
   typedef TTVALogger<TFieldMaker>  TVALogger;
   typedef TVALogger::TViewArea     TMasterVA;
   TVALogger   VALog_;
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
