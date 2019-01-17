//
// Singon Server 端的處理 基底 class
//
// 版本: $Id: SignonSvr.h,v 1.5 2006/04/04 03:57:24 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SignonSvrH
#define SignonSvrH
//---------------------------------------------------------------------------
#include "Signon/Policy/PolicyBase.h"
#include "pod_vector.h"
#include "KTime.h"
#include <set>
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
//---------------------------------------------------------------------------
using Kway::Signon::Policy::TOnPmUpdate;
using Kway::Signon::Policy::TPolicyKey;
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSignonSvr;
/////////////////////////////////////////////////////////////////////////////
struct TSignonSvrEvHandler
{
   virtual K_mf(void) OnSignonSvrDestroy (TSignonSvr*) = 0;
};
//---------------------------------------------------------------------------
struct TOnlineUserData
{
   std::string    UserID_;
   std::string    From_;      //登入來源
   std::string    ClientAP_;
   TKDateTimeMS   Time_;      //登入時間
};
//---------------------------------------------------------------------------
/** 2008/07/09 改用非同步 [登入結果通知]
      SignonSvr 可能同時支援 [同步] 及 [非同步]
      例如: 可根據 userid 的密碼檢查政策, 來決定使用 LDAP 檢查密碼, 或直接使用 SignonDB 檢查.
*/
LIBNS_class TSignonResult
{
protected:
   virtual K_dtor ~TSignonResult() {
   }
public:
   typedef Kway::Signon::Policy::TOnlineID   TOnlineID;
   virtual K_mf(void) OnSignonResult    (TSignonSvr*, int pendingID, const std::string& user, TOnlineID oid, const std::string& result) = 0;
   virtual K_mf(void) OnChgPassResult   (TSignonSvr*, int pendingID, const std::string& user, const std::string& result) = 0;
   virtual K_mf(void) OnCheckPassResult (TSignonSvr*, int pendingID, const std::string& user, bool pass) = 0;
};
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSignonSvr
{
public:
   //----- types.
   typedef Kway::Signon::Policy::TOnlineID         TOnlineID;
   typedef Kway::Signon::Policy::TPolicyBaseRef    TPolicyBaseRef;

   //----- constructor/destructor.
           K_ctor  TSignonSvr  ();
   virtual K_dtor ~TSignonSvr  ();

   //----- 工作站連線來源認證
   virtual K_mf(TPolicyBaseRef)  GetUFromPolicy  (const std::string& ufrom, const std::string& policyName, bool forLocalUse = true) const = 0;

   /* 2008/07/09 改用非同步 [登入結果通知]
   //----- 使用者身分認證 (目前尚未考慮非同步身分認證機制! 底下的介面不適合非同步認證!)
   virtual K_mf(std::string)     ChgPass        ( const std::string& user, const std::string& oldPass, const std::string& newPass
                                                , const std::string& ufrom) = 0;
   virtual K_mf(std::string)     SignonUser     ( const std::string& user,  const std::string& pass
                                                , const std::string& ufrom, const std::string& clientAP
                                                , TOnlineID&) = 0;
   ///> 僅檢查帳號密碼是否正確.
   virtual K_mf(bool)            CheckPass      ( const std::string& user,  const std::string& pass ) = 0;
   */
   /// 使用者身分認證, 改密碼...
   /// 傳回 pendingID( > 0) 做為 TSignonResult::OnSignonResult() 通知的依據,
   /// 若在返回前呼叫 TSignonResult::OnSignonResult(), 則此時 pendingID=0, 返回0
   virtual K_mf(int) ChgPass    ( const std::string& user,  const std::string& oldPass, const std::string& newPass
                                , const std::string& ufrom, TSignonResult&) = 0;
   virtual K_mf(int) SignonUser ( const std::string& user,  const std::string& pass
                                , const std::string& ufrom, const std::string& clientAP
                                , TSignonResult&) = 0;
   /// 僅檢查帳號密碼是否正確.
   virtual K_mf(int) CheckPass  ( const std::string& user,  const std::string& pass, TSignonResult& ) = 0;
   /// 取消尚未取得結果的登入通知, 若 pendingID<=0, 則取消全部的 sgnResult
   /// 若 pendingID > 0, 則取消指定的 pendingID, 且該 ID 對應的 TSignonResult 必須 = sgnResult
   virtual K_mf(bool) CancelSignonResult (int pendingID, TSignonResult* sgnResult) = 0;

   ///> 取得使用者權限政策
   virtual K_mf(TPolicyBaseRef)  GetUserPolicy  ( const std::string& user, const std::string& policyName, bool forLocalUse = true) const = 0;

   /** 2005/01/13增加:是否可重複登入機制,
       所以呼叫SignonUser()之後,若使用者斷線或登出,請務必呼叫SignoutUser().
       否則可能造成使用者無法登入的情況.
   */
   virtual K_mf(bool) SignoutUser (const std::string& user, const std::string& ufrom, const std::string& clientAP, TOnlineID&) = 0;

   /** 註冊當Policy有異動時通知.
   */
   virtual K_mf(bool) AddOnUpdate (const std::string& user, const std::string& policyName, TOnPmUpdate&) = 0;
   virtual K_mf(bool) DelOnUpdate (const std::string& user, const std::string& policyName, TOnPmUpdate&) = 0;

   /** 當SignonSvr死亡時通知.
   */
   inline K_mf(bool) AddSignonSvrEvHandler (TSignonSvrEvHandler* v)  { return SignonSvrEvHandlers_.add(v); }
   inline K_mf(bool) DelSignonSvrEvHandler (TSignonSvrEvHandler* v)  { return SignonSvrEvHandlers_.del(v); }

   /** 取得全部的 user-id.
   */
   virtual K_mf(bool) GetUsersList (std::set<std::string>&) = 0;

   /** 根據 OnlineID 取得線上使用者.
   */
   virtual K_mf(const TOnlineUserData*) GetOnlineUser (TOnlineID) = 0;

   /** 密碼支援哪種編碼方式
   */
   virtual K_mf(Kway::byte) GetPassEncMode () = 0;

   /** 一旦連線(LinkReady)就可以記錄log, 或有其他需要也可以記錄!
   */
   virtual K_mf(void) WriteLog (const std::string& user, const std::string& ufrom, const std::string& clientAP, TOnlineID oid, const char* msg) = 0;
private:
   typedef Kstl::pod_vector<TSignonSvrEvHandler*>  TSignonSvrEvHandlers;
   TSignonSvrEvHandlers SignonSvrEvHandlers_;
};
/////////////////////////////////////////////////////////////////////////////
/** 傳回設定前的 SignonSvr.
*/
LIBNS_fn(TSignonSvr*)  SetGlobalSignonSvr (TSignonSvr*);

/** 取得現在的 SignonSvr.
*/
LIBNS_fn(TSignonSvr*)  GetGlobalSignonSvr ();
/////////////////////////////////////////////////////////////////////////////
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
#endif
