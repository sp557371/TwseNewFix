//
// 簽入管理 - [簽入政策]基礎 classes
//
// 版本: $Id: PolicyBase.h,v 1.4 2006/04/04 04:02:11 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef PolicyBaseH
#define PolicyBaseH
//---------------------------------------------------------------------------
#include <utility>
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include "fixed_num.h"
#include "char_ary.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
namespace Policy {
/////////////////////////////////////////////////////////////////////////////
//
// 使用者登入管理,基本類別定義
// 在 Signon/Policy/* 的目錄下存放的是 [政策的名稱] 及 政策的[內容]
// 例 Signon/Policy/UFGroups.h
//
//---------------------------------------------------------------------------
typedef std::pair<const void*, word>   TPolicyBaseRef;
typedef Kstl::fixed_num<dword>         TOnlineID;
/////////////////////////////////////////////////////////////////////////////
template <class T>
class TTRecsPolicy
{
   const T*    Recs_;
   word        Count_;
public:
   typedef const T*  iterator;

   inline K_ctor  TTRecsPolicy  (const TPolicyBaseRef& ref)
            : Recs_(static_cast<const T*>(ref.first)),
              Count_(static_cast<word>(ref.second/sizeof(T)))
            { }

   inline K_mf(iterator)  begin  () const  { return Recs_; }
   inline K_mf(iterator)  end    () const  { return Recs_ + Count_; }
   inline K_mf(bool)      empty  () const  { return Count_==0; }
   inline K_mf(word)      size   () const  { return Count_; }
};
//---------------------------------------------------------------------------
template <class T>
class TTRecPolicy
{
   const T* Rec_;
public:
   typedef T   TRecType;

   inline K_ctor  TTRecPolicy  (const TPolicyBaseRef& ref)
            : Rec_(ref.second==sizeof(T) ? static_cast<const T*>(ref.first) : 0)
            { }

   inline K_mf(bool)      empty       ()     const  { return Rec_==0; }
   inline K_mf(const T*)  operator->  ()     const  { return Rec_;    }
   inline K_mf(const T&)  operator*   ()     const  { return *Rec_;   }
   inline K_mf(const T*)  get         ()     const  { return Rec_;    }
   inline K_mf(bool)      assign_to   (T& a) const  { if(Rec_==0)  { a = T(); return false; }
                                                      a = *Rec_;   return true; }
};
//---------------------------------------------------------------------------
template <class TRecs, class TRec>
K_mf(TPolicyBaseRef)  MakeRecsPolicy  (const TRecs& recs, std::string& result, const TRec*)
{
   result.clear();
   typename TRecs::const_iterator  iend = recs.end();
   for(typename TRecs::const_iterator i = recs.begin();  i != iend;  ++i) {
      const char* elem = reinterpret_cast<const char*>(&(i->first));
      result.insert(result.end(), elem, elem+sizeof(typename TRecs::key_type));
      elem = reinterpret_cast<const char*>(static_cast<const TRec*>(&(i->second)));
      result.insert(result.end(), elem, elem+sizeof(TRec));
   }
   return TPolicyBaseRef(result.data(), static_cast<TPolicyBaseRef::second_type>(result.length()));
}
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TPmBase;
//---------------------------------------------------------------------------
//請在使用時才用繼承的方式,例如: struct TPolicyKey_UserRights : public TPolicyKey {};
//以避開 BCB 的 bug!
LIBNS_class TPolicyKey : public Kstl::char_ary<char,12,' '>
{
   typedef Kstl::char_ary<char,12,' '>   inherited;
public:
   inline K_ctor  TPolicyKey  (const std::string& str)  : inherited() { assign(str); }
   inline K_ctor  TPolicyKey  (const char* str)         : inherited() { assign(str); }
   inline K_ctor  TPolicyKey  ()                        : inherited() {}

   inline K_mf(void)  assign  (const std::string& str)  { assign(str.c_str()); }
   inline K_mf(void)  assign  (const char* str)         { char buf[sizeof(*this)+1];
                                                          inherited::assign(assign(buf,str)); }

   static K_mf(const char*)  assign  (char* buf, const char* str);
};
//---------------------------------------------------------------------------
LIBNS_class TOnPmUpdate
{
protected:
   /** 解構放在 protected, 表示您必須自行刪除此物件, TPmBase_GenUpdEv不會把他刪除.
   */
   virtual K_dtor ~TOnPmUpdate  ()   {}
public:
   inline  K_ctor  TOnPmUpdate  ()   {}

   virtual K_mf(void) OnPmUpdate (TPmBase&, const TPolicyKey&, bool erased) = 0;
};
//---------------------------------------------------------------------------
LIBNS_class TPmBase
{  //基本政策設定資料表
   //例:TPmUserTwStkRights  [使用權]政策表   "PmUserTwStkRights.h"
   //   TPmUserTwStkScItems [風控項目]政策表 "PmUserTwStkScItems.h"
        TPmBase  (const TPmBase&);//禁止複製.
   void operator=(const TPmBase&);//禁止複製.
public:
   inline  K_ctor  TPmBase ()  {}
   virtual K_dtor ~TPmBase ()  {}

   virtual K_mf(TPolicyBaseRef) GetPolicy  (const TPolicyKey&, bool forLocalUse) const = 0;

   /** 提供異動通知註冊機制:
       針對某 TPolicyKey 註冊異動通知.
   */
   virtual K_mf(bool)        AddOnUpdate   (const TPolicyKey&, TOnPmUpdate&)  { return false; }
   virtual K_mf(bool)        DelOnUpdate   (const TPolicyKey&, TOnPmUpdate&)  { return false; }
   virtual K_mf(const char*) GetPolicyName () const                           { return "";    }

protected:
   template <class TRecs,
             class TRec>//使用 SimpleRecorder 之後, TRecs::mapped_type 已不是期望的型態, 所以需要加上此參數
                        //請參考使用範例 UserTwStkRes.h
   static K_mf(TPolicyBaseRef)  GetPolicy_TRec  (const TRecs& recs, const typename TRecs::key_type& key, const TRec*)
      {
         typename TRecs::const_iterator  i = recs.find(key);
         if(i==recs.end())
            return TPolicyBaseRef(0,0);
         return TPolicyBaseRef(static_cast<const TRec*>(&(i->second)), sizeof(TRec));
      }
   template <class TRecs>
   static K_mf(TPolicyBaseRef)  GetPolicy_TMap  (const TRecs& recs, const typename TRecs::key_type& key)
      {
         typename TRecs::const_iterator  i = recs.find(key);
         if(i==recs.end())
            return TPolicyBaseRef(0,0);
         return i->second.GetPolicy();
      }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Policy
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
#endif

