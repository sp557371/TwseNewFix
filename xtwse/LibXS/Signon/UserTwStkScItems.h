//
// TwStk - 風控項目設定
//
// 版本: $Id: UserTwStkScItems.h,v 1.4 2005/04/26 03:23:06 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef UserTwStkScItemsH
#define UserTwStkScItemsH
//---------------------------------------------------------------------------
#include "Signon/Policy/UPTwStkScItems.h"
#include "PmBase.h"
#include "inn_file.h"
#include "SimpleRecorder.h"
#include "VALogger.h"
#include "libsneTwStk.h"
//---------------------------------------------------------------------------
//
// 風控項目
//
namespace Kway {
namespace Signon {
/////////////////////////////////////////////////////////////////////////////
using Kway::Signon::Policy::TScLevel;
using Kway::Signon::Policy::TUserTwStkScItems;
/////////////////////////////////////////////////////////////////////////////
// 定義存檔方式(檔案最大4G(dword),每筆Rec最大64K(word))
typedef Kstl::rr_inn_file<dword,word>     TUserTwStkScItemsInn;
/////////////////////////////////////////////////////////////////////////////
namespace for_bcb_bug_scitems {//不加上這段無用的定義,會造成 BCB compiler 說 TUserTwStkScItemsRecorder 的定義有問題!!
struct TBCB_bug_filler {};
typedef TSimpleMap_AddRoomKey<int,TBCB_bug_filler, TUserTwStkScItemsInn::rr_key>::TEvRecs  TUFGroups;
}
//===========================================================================
// 定義資料表
struct  TUserTwStkScItems_PolicyKey : public TPolicyKey {};//不用這種方式定義 Key,會造成BCB產生的程式碼有問題
//typedef TSimpleRecorder_AddRoomKey<TUserTwStkScItems_PolicyKey, TUserTwStkScItems, TUserTwStkScItemsInn, sizeof(TUserTwStkScItems)>
//        TUserTwStkScItemsRecorder;
K_typedef_TSimpleRecorder_AddRoomKey(TUserTwStkScItemsRecorder,
                                     TUserTwStkScItems_PolicyKey,
                                     TUserTwStkScItems,
                                     TUserTwStkScItemsInn,
                                     static_cast<word>(sizeof(TUserTwStkScItems)) );
//===========================================================================
class  TUserTwStkScItemsMgr : protected TUserTwStkScItemsRecorder
                            , public    TPmBaseVA
{
   typedef TUserTwStkScItemsRecorder    InheritedRecorder;

public:
   using InheritedRecorder::TRecs;
   using InheritedRecorder::get_recs;

   K_ctor  TUserTwStkScItemsMgr  (const std::string& defaultPath, TTableLog*);

   K_mf(TPolicyBaseRef)  GetPolicy  (const TPolicyKey& key, bool forLocalUse) const
                { return GetPolicy_TRec(Recs_, static_cast<const TRecs::key_type&>(key), (TUserTwStkScItems*)0); }

   //資料顯示方法
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
   //給 ViewAreaMgr 管理用的 members
   K_mf(TaViewArea)  CreateNewVA  ();
   typedef TTVALogger<TFieldMaker>  TVALogger;
   TVALogger  VALog_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
} // namespace Signon
//---------------------------------------------------------------------------
#endif

