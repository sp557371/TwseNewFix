//
// TwStk - 交易資源
//
// 版本: $Id: UserTwStkRes.h,v 1.4 2005/04/26 03:24:48 fonwin Exp $
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
// 分配資源: [券商代號],[櫃號],[配額組],[試算最多筆數]...
//
namespace Kway {
namespace Signon {
/////////////////////////////////////////////////////////////////////////////
using Kway::Signon::Policy::TUserTwStkRes;
typedef Kstl::rr_inn_file<dword,word>  TUserTwStkResInn;
/////////////////////////////////////////////////////////////////////////////
namespace for_bcb_bug_TwStkRes {//不加上這段無用的定義,會造成 BCB compiler 說 TUserTwStkResRecorder 的定義有問題!!
struct TBCB_bug_filler {};
typedef TSimpleMap_AddRoomKey<int,TBCB_bug_filler,TUserTwStkResInn::rr_key>::TEvRecs  TUFGroups;
}
//===========================================================================
// 定義資料表
struct  TUserTwStkRes_PolicyKey : public TPolicyKey {};//不用這種方式定義 Key,會造成BCB產生的程式碼有問題
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


