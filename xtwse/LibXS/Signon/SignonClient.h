//
// 客戶端維護登入狀態的 class:
//    class TSignonClient
//
// 版本: $Id: SignonClient.h,v 1.3 2006/04/04 03:57:58 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SignonClientH
#define SignonClientH
//---------------------------------------------------------------------------
#include <map>
#include <vector>
//---------------------------------------------------------------------------
#include "pod_vector.h"
#include "Ses.h"
#include "Signon/OnlineUser.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class  TSignonClient;
//---------------------------------------------------------------------------
enum ESignonClientState {
      sc_NoSes,             //尚未建立Session
      sc_WaitLinkReady,     //Session連線中
      sc_WaitConnected,     //連線要求傳送中,等Server回覆
      sc_WaitSignonInput,   //連線OK,等候使用者簽入
      sc_WaitSignonLink,    //等候重新連線後登入.  
      sc_WaitSignonResult,  //登入要求傳送中,等Server回覆
      sc_WaitChgPass,       //密碼正確,但請先改密碼後才可登入.
      sc_Ready,             //已登入完成
      sc_Signout,           //系統登出
};
//---------------------------------------------------------------------------
LIBNS_class  TSignonAckEv
{
   friend class TSignonClient;
   //if(result.empty()) => success
   //else  result is error message!
   virtual K_mf(void)  OnSignonResult  (TSignonClient&, const std::string& result) = 0;
   virtual K_mf(void)  OnSignonChgPass (TSignonClient&, const std::string& result) {}
   virtual K_mf(void)  OnSignout       (TSignonClient&) {}
   virtual K_mf(void)  OnStateChanged  (TSignonClient&, ESignonClientState oldState) {}
   virtual K_mf(void)  OnPolicyChanged (TSignonClient&, const std::string& policyName) {}
   virtual K_mf(void)  OnChkTableRightResult (TSignonClient&, const std::string& user, const std::string& tableName, const std::string& result) {}
};
/////////////////////////////////////////////////////////////////////////////
LIBNS_class  TSignonClient : public TOnlineUser
{  //若多個Ses要共用同一個登入使用者,則只要共用此class即可
public:
   typedef ESignonClientState    EState;
   //----- 當登入狀態改變時負責通知全部的 Session
   struct TSesInfo {
      TSesBase*      Ses_;
      std::string    ServerName_;
      bool           IsReady_;   //is ready for Signon
      inline K_ctor      TSesInfo    (TSesBase& ses)  : Ses_(&ses), ServerName_(), IsReady_(false)  {}
      inline K_mf(bool)  operator==  (const TSesInfo& i) const  { return(Ses_==i.Ses_); }
      inline K_mf(bool)  operator!=  (const TSesInfo& i) const  { return(Ses_!=i.Ses_); }
   }; //此處不適用 pod_vector<> 因為 ServerName_ 為 std::string
   typedef std::vector<TSesInfo>    TSesClients;
   typedef TSesClients::iterator    TSesClienti;

private:
   //----- 負責掌握現在狀態
   EState   State_;

protected:
   //----- 負責登入事件通知
   typedef Kstl::pod_vector<TSignonAckEv*>   TEvs;
   typedef TEvs::iterator                    TEvi;
   TEvs  evs_;

   //----- 負責收集 Policy -----
   typedef std::string  TPolicyName;
   typedef std::string  TPolicyData;
   typedef std::map<TPolicyName,TPolicyData> TPolicys;
   typedef TPolicys::iterator                TPolicyi;
   TPolicys    UFromPolicys_;
   TPolicys    UserPolicys_;

protected:
   TSesClients    Ses_;

   K_mf(void)  AddSes    (TSesBase&);
   K_mf(void)  DelSes    (TSesBase&);
   K_mf(void)  SetState  (EState);
   typedef void (K_FASTCALL TSignonAckEv::*TSignonResultEv) (TSignonClient&, const std::string&);
   K_mf(void)  OnSignonResultEv      (TSignonResultEv fnEv, const std::string& result);
   K_mf(void)  OnSignonResult        (TSesBase*, const std::string& result);
   K_mf(void)  OnPolicyChanged       (const std::string& policyName);
   K_mf(void)  OnChkTableRightResult (const std::string& user, const std::string& tableName, const std::string& result);

   inline K_mf(void) OnChgPassResult (const std::string& result)  { OnSignonResultEv(&TSignonAckEv::OnSignonChgPass, result); }
public:
   K_ctor  TSignonClient  ();

   //提供的基本服務, 若有失敗,則應呼叫 evs_ 的相關事件
   virtual K_mf(void)  Open     ();//開啟全部的Ses,準備登入作業
   virtual K_mf(void)  Signon   (const std::string& user, const std::string& pass)=0;
   virtual K_mf(void)  ChgPass  (const std::string& user, const std::string& oldPass, const std::string& newPass)=0;
   virtual K_mf(void)  Signout  ();//系統登出:Close all Ses 

   //檢查[審核者]是否有權限設定 tableName !
   virtual K_mf(void)  CheckTableRight (const std::string& user, const std::string& pass, const std::string& tableName)=0;

   //Ses的管理: 1.設定某個Ses連線的[主機名稱],及是否為Ready狀態(is ready for Signon?)
   K_mf(void)  SetServerName  (TSesBase&, const std::string& serverName, bool IsReady=true);
   K_mf(void)  SetSesReady    (TSesBase&, bool IsReady);
   //           2.用[主機名稱]找到對應的Ses,若有相同的名稱,則使用第一個LinkReady的Ses
   K_mf(TSesBase*)  GetSes  (const std::string& serverName) const;
   //           3.取出全部的[主機名稱]
   K_mf(const TSesClients&)  GetSes  () const  { return Ses_; }

   //事件處理
   inline K_mf(void)  Add  (TSignonAckEv& ev)   { evs_.add(&ev);  }
   inline K_mf(void)  Del  (TSignonAckEv& ev)   { evs_.del(&ev);  }

   //取得登入元件的狀態
   K_mf(EState)  GetState  () const    { return State_; }

   //取得 [使用者/工作站] 管理政策
   typedef Kway::Signon::Policy::TPolicyBaseRef    TPolicyBaseRef;
   K_mf(TPolicyBaseRef)  GetUserPolicy   (const TPolicyName&) const;
   K_mf(TPolicyBaseRef)  GetUFromPolicy  (const TPolicyName&) const;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
#endif

