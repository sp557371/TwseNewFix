//
// TwStk - 代管營業員
//
// 版本: $Id: UserTwStkSaleNos.h,v 1.5 2005/04/26 03:24:23 fonwin Exp $
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
// 代管營業員
//
namespace Kway {
namespace Signon {
/////////////////////////////////////////////////////////////////////////////
using Kway::Tw::Stk::TSaleKey;
using Kway::Signon::Policy::TUserTwStkSale;
/////////////////////////////////////////////////////////////////////////////
// 定義存檔方式(檔案最大4G(dword),每筆Rec最大64K(word))
typedef Kstl::rr_inn_file<dword,word>     TUserTwStkSaleInn;
//===========================================================================
// 定義資料表
struct  TUserTwStkSale_PolicyKey: public TPolicyKey {};//不用這種方式定義 Key,會造成BCB產生的程式碼有問題
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
//                               sizeof(TUserTwStkSale)>//要存入Inn的資料大小
//                               TUserTwStkSalePolicy_Recorder;
typedef Kstl::map2d_maker_key_find<map2d_maker_base<TUserTwStkSalePolicy> >   TUserTwStkSalePolicy_KeyFinder_t;
K_typedef(TUserTwStkSalePolicy_KeyFinder_t, TUserTwStkSalePolicy_KeyFinder);

K_typedef_TSimpleRecorder_Detail(TUserTwStkSalePolicy_Recorder,
                                 TUserTwStkSalePolicy_KeyFinder,
                                 TUserTwStkSaleInn,
                                 sizeof(TUserTwStkSale));
//===========================================================================
class  TUserTwStkSaleMgr : protected TUserTwStkSalePolicy//主資料表
                         , protected TUserTwStkSalePolicy_Recorder//2d對應表,存檔用
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

   //建構
   K_ctor  TUserTwStkSaleMgr  (const std::string& defaultPath, TTableLog*, TTableLog2d*);

   friend class TPmBase;
   K_mf(TPolicyBaseRef)  GetPolicy  (const TPolicyKey& key, bool forLocalUse) const
   { return GetPolicy_TMap(*this, static_cast<const key_type&>(key)); }

   //資料顯示方法
   struct TFieldMaker : public TVAKeyFieldMaker<TRecs>
   {  //Master-僅顯示群組分類
      K_mf(const TFieldDesp&)  GetFieldDesp   (field_n_type n) const
      { static TFieldDesp fdesp = { "代管組別", 12, Kway::ta_xLeft_yCenter, Kway::ta_xLeft_yCenter, Kway::tfs_Null, };
        return fdesp; }
   }; // class TFieldMaker

private:
   //Detail-資料顯示方法
   struct TvacDetail : public TVACreater {
      typedef TDetailRecs     TRecs;
      struct  TFieldMaker
      {  //Detail-顯示欄位描述
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

   //給 ViewAreaMgr 管理用的 members
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
