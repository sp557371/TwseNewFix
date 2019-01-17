//
// TwStk - 交易使用權限
//
// 版本: $Id: UserTwStkRights.h,v 1.4 2005/04/26 03:24:36 fonwin Exp $
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
// 使用權
//
namespace Kway {
namespace Signon {
/////////////////////////////////////////////////////////////////////////////
using Kway::Signon::Policy::TUserTwStkRightsItem;
using Kway::Signon::Policy::TUserTwStkRights;
/////////////////////////////////////////////////////////////////////////////
// 定義存檔方式(檔案最大4G(dword),每筆Rec最大64K(word))
typedef Kstl::rr_inn_file<dword,word>     TUserTwStkRightsInn;
/////////////////////////////////////////////////////////////////////////////
namespace for_bcb_bug {//不加上這段無用的定義,會造成 BCB compiler 說 TUserTwStkRightsRecorder 的定義有問題!!
struct TBCB_bug_filler_r {};
typedef TSimpleMap_AddRoomKey<int,TBCB_bug_filler_r, TUserTwStkRightsInn::rr_key>::TEvRecs  TUFGroups;
}
//===========================================================================
// 定義資料表
struct  TUserTwStkRights_PolicyKey : public TPolicyKey {};//不用這種方式定義 Key,會造成BCB產生的程式碼有問題
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


