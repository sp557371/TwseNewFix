//
// 使用者,遠端管理權限政策.
//    class TUPRemoteMgr;
//
// 版本: $Id: UPRemoteMgr.h,v 1.2 2005/04/26 05:57:37 fonwin Exp $
//
// Note:此Policy不接受Client端的查詢(因為包含了一個ev_map<>).
//---------------------------------------------------------------------------
#ifndef UPRemoteMgrH
#define UPRemoteMgrH
//---------------------------------------------------------------------------
//
// 政策名稱 = TUPRemoteMgr::PolicyName
// 政策內容 = TUPRemoteMgr
//
#include "Signon/Policy/PolicyBase.h"
#include "ev_map.h"
#include "char_ary.h"
#include "inn_file.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
namespace Policy {
/////////////////////////////////////////////////////////////////////////////
typedef Kstl::rr_inn_file< ::Kway::dword, ::Kway::word>   TURM_Inn;
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
LIBNS_struct TUserRemoteMgrPolicyBase
{
   union {
      byte     Flags_;
      struct {
         byte  AllowVAC_  : 1,
               ReadOnly_  : 1,   //當AllTables_==true時,則參考此欄位設定是否可修改.
               AllTables_ : 1;   //允許全部的資料表,若為true,則不再參考明細的設定.
      };
   };
   inline K_ctor  TUserRemoteMgrPolicyBase  ()
      : Flags_(0)
      { }
};
//---------------------------------------------------------------------------
#ifdef _MSC_VER//K_EnumPacked_Bug 移植到VC時才發現沒加,為了檔案能相容,所以改判斷 _MSC_VER
   typedef byte   ETableRight;
   enum ETableRight_t
#else
   enum ETableRight
#endif
{
   etr_ReadOnly        = 0,   //不能改.
   etr_WriteSettingAll = 1,   //可修改 + 可設定套用(不需審核),可審核他人.
   etr_Disabled        = 2,   //無權限.
   etr_WriteNoSetting  = 3,   //可修改 + 設定套用時,需要有權限者強迫.
   etr_WriteSetting1   = 4,   //可修改 + 可設定套用,需他人審核,可審核他人.
/*
   etr_Disabled        = 0,   //無權限.
   etr_ReadOnly        = 1,   //不能改.
   etr_WriteNoSetting  = 2,   //可修改 + 設定套用時,需要有權限者強迫.
   etr_WriteSetting1   = 3,   //可修改 + 可設定套用,需他人審核,可審核他人.
   etr_WriteSettingAll = 4,   //可修改 + 可設定套用(不需審核),可審核他人.
*/
};
//---------------------------------------------------------------------------
struct TTableRight
{
   ETableRight    Right_;

   inline K_ctor  TTableRight  (ETableRight r = etr_Disabled)
      : Right_(r)
      , RoomKey_()
      { }

   //20090213:避免VA複製造成存檔錯誤
   inline K_mf(void) operator= (const TTableRight& r) {
      Right_ = r.Right_;
   }

   TURM_Inn::rr_key  mutable RoomKey_;

#ifdef _MSC_VER
   K_mf(bool) operator==(const TTableRight& r) const {
	   return Right_ == r.Right_;
   }
#endif
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TTableRights : public Kstl::ev_map<std::string,TTableRight>
{
   typedef Kstl::ev_map<std::string,TTableRight>   inherited;
public:
   typedef Kstl::char_ary<char,12,' '>    TMasterKey;

#ifdef _MSC_VER
   inline K_ctor  TTableRights ()  {}
#endif

   inline explicit K_ctor  TTableRights  (const TMasterKey& mkey)
      : MasterKey_(mkey)
      { }

   inline K_mf(const TMasterKey&)  GetMasterKey    () const { return MasterKey_; }
   inline K_mf(std::string)        GetMasterKeyStr () const { return MasterKey_.as_string(); }

   inline K_mf(void) operator= (const TTableRights& r) {
      inherited::operator=(r);
   }

private:
   TMasterKey  MasterKey_;
};
//---------------------------------------------------------------------------
LIBNS_struct TUserRemoteMgrPolicy : public TUserRemoteMgrPolicyBase
{
#ifdef _MSC_VER
   inline K_ctor TUserRemoteMgrPolicy() {}
#endif

   inline K_ctor  TUserRemoteMgrPolicy  (const TTableRights::TMasterKey& mkey)
      : RoomKey_()
      , TableRights_(mkey)
      { }

   inline K_mf(TUserRemoteMgrPolicy&) operator= (const TUserRemoteMgrPolicyBase& r)
            { static_cast<TUserRemoteMgrPolicyBase&>(*this) = r;
              return *this; }

   TURM_Inn::rr_key  mutable RoomKey_;
   TTableRights      mutable TableRights_;

   inline void operator=(const TUserRemoteMgrPolicy& r) {
      //避免ViewArea執行[複製]操作,所造成的問題.
      static_cast<TUserRemoteMgrPolicyBase&>(*this) = static_cast<const TUserRemoteMgrPolicyBase&>(r);
      TableRights_ = r.TableRights_;
   }

   //配合 TTExtabUpdater2d<> 的新增:
   inline K_mf(TTableRight&)  operator[] (const std::string& dkey)    { return TableRights_[dkey]; }
   inline K_mf(size_t)        erase      (const std::string& dkey)    { return TableRights_.erase(dkey); }
   inline                     operator TTableRights& ()               { return TableRights_; }

   K_mf(ETableRight) CheckTableRight (std::string tabName) const;
};
//---------------------------------------------------------------------------
LIBNS_class TUPRemoteMgr : public TTRecPolicy<TUserRemoteMgrPolicy>
{
   typedef TTRecPolicy<TUserRemoteMgrPolicy>   inherited;
public:
   inline K_ctor      TUPRemoteMgr  (const TPolicyBaseRef& ref)  : inherited(ref) {}
   static const char  PolicyName[];
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Policy
} // namespace Kway
} // namespace Signon
//---------------------------------------------------------------------------
#endif

