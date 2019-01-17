//
// Policy Manager Base
//
// 版本: $Id: PmBase.h,v 1.5 2006/04/04 04:05:59 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef PmBaseH
#define PmBaseH
//---------------------------------------------------------------------------
#include "libnsSignonSvrDB.h"
#include "Signon/Policy/PolicyBase.h"
#include "ViewAreaMgr.h"
#include "char_ary.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
//---------------------------------------------------------------------------
using Kway::Signon::Policy::TPolicyBaseRef;
using Kway::Signon::Policy::TPolicyKey;
using Kway::Signon::Policy::TPmBase;
using Kway::Signon::Policy::TOnPmUpdate;

SIGDB_class TSignonSvr_DataCenter;
/////////////////////////////////////////////////////////////////////////////
//typedef Kstl::char_ary<char,12,0>   TPolicyName;
//typedef Kstl::char_ary<char,12,0>   TPolicyKey;
// 920623:原本應該用上述的定義方式就可以了,
// 可是BCB4(BCB5沒試過),不知在那兒有bug! 若使用上述的定義方式,會造成 std::map<> 的偏移值有錯!
// 所以改用繼承的方式定義:
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
class TPolicyName : public Kstl::char_ary<char,20,0> {
   typedef Kstl::char_ary<char,20,0>   inherited;
public:
   inline K_ctor  TPolicyName (const std::string& str)  : inherited(str) {}
   inline K_ctor  TPolicyName (const char* str)         : inherited(str) {}
   inline K_ctor  TPolicyName ()                        : inherited()    {}
};
#ifdef __BORLANDC__  //只有 BCB 才有能力在 [前置處理] 時, 知道資料的大小!
   #if sizeof(TPolicyName) != 20
   #  error "資料有定義方式有誤..."
   #endif
#endif
//---------------------------------------------------------------------------
class TPolicyKey0 : public Kstl::char_ary<char,12,0> {
   typedef Kstl::char_ary<char,12,0>   inherited;
public:
   inline K_ctor  TPolicyKey0  (const std::string& str)  : inherited() { assign(str); }
   inline K_ctor  TPolicyKey0  (const char* str)         : inherited() { assign(str); }
   inline K_ctor  TPolicyKey0  ()                        : inherited() {}

   inline K_mf(void)  assign  (const std::string& str)  { assign(str.c_str()); }
   inline K_mf(void)  assign  (const char* str)         { char buf[sizeof(*this)+1];
                                                          inherited::assign(TPolicyKey::assign(buf,str)); }
};
#ifdef __BORLANDC__  //只有 BCB 才有能力在 [前置處理] 時, 知道資料的大小!
   #if sizeof(TPolicyKey) != 12
   #  error "資料有定義方式有誤..."
   #endif
#endif
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
SIGDB_class TPmBaseVA : public TPmBase, public TVACreater
{
public:
   virtual K_mf(TPmBaseVA*) CreatePmBaseVA(TSignonSvr_DataCenter& owner, TSignonSvr_DataCenter* dcParentLimited) {
      return NULL;
   }
};
/////////////////////////////////////////////////////////////////////////////
SIGDB_class TPmBase_GenUpdEv : public TPmBaseVA
{
protected:
   K_ctor  TPmBase_GenUpdEv (Kstl::ev_container&);
   K_dtor ~TPmBase_GenUpdEv ();

   template <class TRecs>
   K_mf(bool) GetPolicyKeys (const TRecs& recs, const ev_iterator& istart, size_t count, TPolicyKey& keyBegin, TPolicyKey& keyEnd) const
      {  typename TRecs::const_iterator   itr = static_cast<const typename TRecs::iterator&>(istart);
         keyBegin = itr->first;
         typename TRecs::const_iterator   iend = recs.end();
         while(count-- > 0)
            if(++itr==iend) {
               --itr;
               keyEnd = itr->first;
               keyEnd.set(0, static_cast<char>(keyEnd[0]+1));
               return true;
            }
         keyEnd = itr->first;
         return true;
      }

public:
   virtual K_mf(bool) AddOnUpdate (const TPolicyKey&, TOnPmUpdate&);
   virtual K_mf(bool) DelOnUpdate (const TPolicyKey&, TOnPmUpdate&);

   virtual K_mf(const char*) GetPolicyName () const = 0;
   /// 取得 keyBegin = istart.key, keyEnd = (istart+count).key
   /// 作為[異動通知]的範圍: 若之前有呼叫 AddOnUpdate(key,updater) 則: if(keyBegin<=key && key < keyEnd) 就會收到此次的通知
   virtual K_mf(bool) GetPolicyKeys (const ev_iterator& istart, size_t count, TPolicyKey& keyBegin, TPolicyKey& keyEnd) const = 0;

private:
   class TImpl;
   TImpl* Impl_;
};
//---------------------------------------------------------------------------
SIGDB_class TPmMgr : protected TViewAreaMgr
{  //一堆[政策表]的管理
   //  管理 [PolicyName---TPmBase] 的集合
   typedef TViewAreaMgr inherited;

   TViewAreaMgr::TVAGetEditProp_GetNames  VAEditProp_GetNames_;
   TViewAreaMgr::TVAGetEditProp_GetKeys   VAEditProp_GetKeys_;
public:
   K_ctor  TPmMgr  ();

   inline K_mf(bool)  Add  (const std::string& policyName, TPmBaseVA& pm)  { return inherited::Add(policyName, pm); }
   inline K_mf(bool)  Del  (const std::string& policyName, TPmBaseVA& pm)  { return inherited::Add(policyName, pm); }
   inline K_mf(bool)  Del  (TPmBaseVA& pm)                                 { return inherited::Del(pm); }

   inline K_mf(bool) GetRegNames (Kway::TStrings& result) const {
      return inherited::GetRegNames(result);
   }

// inline K_mf(TPmBase*)         GetPolicy       (const std::string& policyName) const   { return static_cast<TPmBaseVA*>(GetReg(policyName)); }
// gcc不喜歡我把這樣的轉換放在這兒↑ 型別轉換會有問題: GetReg()=> TVACreater* => TPmBaseVA* => TPmBase*
   K_mf(TPmBase*) GetPolicy (const std::string& policyName) const;

   inline K_mf(TViewAreaMgr&)    GetViewAreaMgr  ()                                      { return *this; }
   inline K_mf(TVAGetEditProp&)  GetEditProp_PolicyNames  ()   { return VAEditProp_GetNames_; }
   inline K_mf(TVAGetEditProp&)  GetEditProp_PolicyKeys   ()   { return VAEditProp_GetKeys_;  }
};
//---------------------------------------------------------------------------
SIGDB_class TPmGroup : public TVACreater
{  //政策群管理
   // 使用 TPmGroupID 當作索引, 取得一組 [PolicyName--PolicyKey] 的集合
   class TImpl;
   TImpl* Impl_;
   //------------------------------------------------------------------------
   //給 ViewAreaMgr 管理用的 members
   K_mf(TaViewArea)  CreateNewVA           ();
   K_mf(size_t)      GetDetails            (const char**&);
   K_mf(TVACreater*) GetDetailVACreater    (size_t);
   K_mf(TVACreater*) OnMasterViewRowMoved  (TViewArea& masterVA, TViewArea*& detailVA, size_t detailID);

public:
   class TPmGroupID : public TPolicyKey0 {
      typedef TPolicyKey0  inherited;
   public:
      inline K_ctor  TPmGroupID  (const std::string& str)  : inherited(str) {}
      inline K_ctor  TPmGroupID  (const char* str)         : inherited(str) {}
      inline K_ctor  TPmGroupID  ()                        : inherited()    {}
   };

   //----- 建構/解構
   K_ctor  TPmGroup  (const std::string& fileName, TPmMgr*,
                      const std::string& mGroupName, const std::string& mTableName,
                      const std::string& dGroupName, const std::string& dTableName);
   K_dtor ~TPmGroup  ();

   //----- 查詢某 Policy 的設定
   K_mf(TPolicyBaseRef)  GetPolicy  (const TPmMgr&, const TPmGroupID&, const std::string& policyName, bool forLocalUse) const;

   //----- Policy異動通知.
   K_mf(bool) AddDelOnUpdate ( const TPmMgr&, const TPmGroupID&, const std::string& policyName, TOnPmUpdate&
                             , bool (K_FASTCALL TPmBase::*addDel) (const TPolicyKey&, TOnPmUpdate&) );

   //----- 修改某[PolicyGroup]-[PolicyName]-PolicyKey(若資料不存在則新增)
   K_mf(bool)  ModPolicy  (const TPmGroupID&, const std::string& policyName, const std::string& policyKey);

   //----- 新增某[PolicyGroup]-[PolicyName]-PolicyKey(若資料已存在則不變)
   K_mf(const TPolicyKey*)  AddPolicy  (const TPmGroupID&, const std::string& policyName, const std::string& policyKey);

   //取出全部的 Keys 當作輸入時的 [列舉]
   K_mf(TVAGetEditProp&)  GetVAEditProp_ForKeys  ();

   #ifdef __TransLnDB__ //如果需要從 KPTS的LnDB 轉入登入資料庫時,才需要開放底下的 members, 但您必須自己完成他
   K_mf(std::string)  LoadLnDB  (const std::string& pathLnDB);
   #endif
};
typedef TPmGroup::TPmGroupID  TPmGroupID;
/////////////////////////////////////////////////////////////////////////////
} // Signon
} // Kway
//---------------------------------------------------------------------------
#endif

