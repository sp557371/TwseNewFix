//
// Singon Server 端的處理 - 使用 SignonDB (DataCenter)
//
// 版本: $Id: SignonSvr_DC.h,v 1.7 2006/04/04 03:56:03 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SignonSvr_DCH
#define SignonSvr_DCH
//---------------------------------------------------------------------------
//
// SignOnSvr Data Center
//
//---------------------------------------------------------------------------
#include "SignonSvr.h"
#include "PmBase.h"
#include "PmUFrom.h"       //class TPmUFromGroup;
#include "PmUFromIPv4.h"   //class TPmUFromIPv4;
#include "PmUserDB.h"      //class TPmUserDB;
#include <deque>
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
/////////////////////////////////////////////////////////////////////////////
SIGDB_class TSignonSvr_DataCenter;
//---------------------------------------------------------------------------
// SignonSvr 擴充模組的寫法:
//    (1) 名稱: sneXXXXX.dll 或 sneXXXXX.bpl
//    (2) 提供 2 個 functions: (請務必在 function [名稱之前][傳回值之後] 加上 __export)
typedef void*  TsneStartObj;
 SIGDB_exportC(TsneStartObj)   sneStart    (TSignonSvr_DataCenter&);
 SIGDB_exportC(void)           sneFinish   (TSignonSvr_DataCenter&, TsneStartObj);
  typedef  TsneStartObj     (*TsneStart)   (TSignonSvr_DataCenter&);
  typedef  void             (*TsneFinish)  (TSignonSvr_DataCenter&, TsneStartObj);
/////////////////////////////////////////////////////////////////////////////
SIGDB_class TSignonSvr_DataCenter : public TSignonSvr
{
public:
   K_ctor  TSignonSvr_DataCenter  (const std::string& defaultPath, const std::string& signonLogFileName);

   inline K_mf(std::string)    GetPathDB     ()    { return PathDB_;     }
   inline K_mf(TViewAreaMgr&)  GetMainVAMgr  ()    { return MainVAMgr_;  }//簽入基本資料管理VAMgr
   inline K_mf(TPmMgr&)        GetUserPmMgr  ()    { return UserPmMgr_;  }//[使用者政策]管理
   inline K_mf(TPmMgr&)        GetUFromPmMgr ()    { return UFromPmMgr_; }//[使用者來源政策]管理
   inline K_mf(TPmGroup&)      GetPmUserGroup()    { return *(PmUserGroup_.get()); }//使用者政策管理
   inline K_mf(TPmUserDB&)     GetUserDB     ()    { return *(UserDB_.get()); }
   virtual K_mf(TViewAreaMgr*) GetSignonVAMgr()    { return NULL;  }

   //----- 身分檢查
   typedef Kway::Signon::Policy::TOnlineID         TOnlineID;
   typedef Kway::Signon::Policy::TPolicyBaseRef    TPolicyBaseRef;
   K_mf(TPolicyBaseRef)  GetUserPolicy   (const std::string& user,  const std::string& policyName, bool forLocalUse = true) const;
   K_mf(TPolicyBaseRef)  GetUFromPolicy  (const std::string& ufrom, const std::string& policyName, bool forLocalUse = true) const;
   K_mf(bool)            GetUsersList    (std::set<std::string>&);

   //2008/07/09 改用非同步 [登入結果通知]
   virtual K_mf(int) ChgPass    ( const std::string& user,  const std::string& oldPass, const std::string& newPass
                                , const std::string& ufrom, TSignonResult&);
   virtual K_mf(int) SignonUser ( const std::string& user,  const std::string& pass
                                , const std::string& ufrom, const std::string& clientAP
                                , TSignonResult&);
   virtual K_mf(int) CheckPass  ( const std::string& user,  const std::string& pass, TSignonResult& );
   virtual K_mf(bool) CancelSignonResult (int pendingID, TSignonResult* sgnResult);

   //----- Policy異動通知.
   K_mf(bool) AddOnUpdate (const std::string& user, const std::string& policyName, TOnPmUpdate&);
   K_mf(bool) DelOnUpdate (const std::string& user, const std::string& policyName, TOnPmUpdate&);

   /** 2005/01/13增加:是否可重複登入機制,
       所以呼叫SignonUser()之後,若使用者斷線或登出,請務必呼叫SignoutUser().
       否則可能造成使用者無法登入的情況.
   */
   K_mf(bool) SignoutUser (const std::string& user, const std::string& ufrom, const std::string& clientAP, TOnlineID&);

   ///取得線上使用者
   K_mf(const TOnlineUserData*) GetOnlineUser (TOnlineID);

   /** 密碼支援哪種編碼方式
   */
   virtual K_mf(Kway::byte) GetPassEncMode ();
   virtual K_mf(void) WriteLog (const std::string& user, const std::string& ufrom, const std::string& clientAP, TOnlineID onlineID, const char* msg);

   inline K_mf(void) SetPostSignonSvr(TSignonSvr* svr) {
      PostSignonSvr_ = svr;
   }
private:
   std::string    PathDB_;  //Signon資料庫預設路徑

protected:
   K_mf(std::string)  DoChgPass    (const std::string& user,  const std::string& oldPass, const std::string& newPass, const std::string& ufrom);
   K_mf(std::string)  DoSignonUser (const std::string& user,  const std::string& pass, const std::string& ufrom, const std::string& clientAP, TOnlineID&);
   K_mf(bool)         DoCheckPass  (const std::string& user,  const std::string& pass);

   //使用[來源管理],及[使用者基本資料]表:請注意建構順序
   std::auto_ptr<TPmUFromGroup>     PmUFromGroup_;    //使用者來源管理:群組分類管理,來源管理政策
   std::auto_ptr<TPmUFromIPv4>      UFromIPv4_;       //IPv4的工作站管理
   std::auto_ptr<TPmGroup>          PmUserGroup_;     //使用者政策管理
   std::auto_ptr<TPmUserDB>         UserDB_;          //使用者資料庫

   //建立 [政策管理] 相關資料表, 提供註冊用
   TPmMgr         UserPmMgr_;    //使用者管理政策表
   TPmMgr         UFromPmMgr_;   //工作站來源管理政策表

   // 跟 PmMgr_ 註冊的人, 必須自行取消註冊
   // (若您使用 new 的方式建立您自己的物件來註冊,
   //  則可 Handle TVACreater::OnBfVAMgrDestroy() 的事件,來將您建立的物件消滅)
   // 否則如果您的物件比 PmMgr_ 早死,而又沒有取消註冊,
   // 當 PmMgr_ 要發出 OnBfVAMgrDestroy() 的事件通知時, 那就當機了!!

   SIGDB_class TOnlineUserMgr : public TVACreater {
      typedef TVACreater   inherited;
   public:
      K_ctor  TOnlineUserMgr (const TPmUserDB&, const std::string& signonLogFileName);

      K_mf(void)      GetUserCount(const std::string& user, const std::string& ufrom, const std::string& clientAP, size_t& ufCount, size_t& dufCount) const;
      K_mf(TOnlineID) SignonUser  (const std::string& user, const std::string& ufrom, const std::string& clientAP);
      K_mf(bool)      SignoutUser (const std::string& user, const std::string& ufrom, const std::string& clientAP, TOnlineID);
      K_mf(void)      WriteErrLog (const std::string& user, const std::string& ufrom, const std::string& clientAP, const std::string& msg);
      K_mf(void)      WriteLog    (const TKDateTimeMS& time
                                  , const std::string& user
                                  , const std::string& aufrom
                                  , const std::string& clientAP
                                  , TOnlineID          onlineID
                                  , const char*        appMsg );
      ///取得線上使用者
      K_mf(const TOnlineUserData*) GetOnlineUser (TOnlineID);
      K_mf(int) GetUserCount() const {
         return (int)OnlineMap_.size();
      }
   private:

      TFile             Log_;
      TKDate            LastMsgDate_;
      const TPmUserDB*  UserDB_;    //查詢姓名用.
      typedef Kstl::ev_map<TOnlineID,TOnlineUserData> TOnlineUsers;
      typedef Kstl::ev_map<TUserID,TOnlineUsers>      TOnlineUsersMap;
      typedef std::map<TOnlineID,TOnlineUserData*>    TOnlineMap;
      TOnlineUsersMap         OnlineUsersMap_;
      TOnlineID               LastOnlineID_;
      std::deque<TOnlineID>   FreeOnlineIDs_;
      TOnlineMap              OnlineMap_;

      struct  TFieldMaker {
         typedef TOnlineUsersMap          TEvContainer;
         typedef TEvContainer::iterator   iterator;
         typedef size_t                   field_n_type;

         inline K_ctor  TFieldMaker  (const TPmUserDB* userDB) : UserDB_(userDB) {}
         static K_mf(field_n_type) GetFieldCount  ();
         K_mf(const TFieldDesp&)   GetFieldDesp   (field_n_type fieldNo) const;
         K_mf(std::string)         Get  (const iterator& i, field_n_type fieldNo) const;
         K_mf(void)                Put  (TEvContainer&, iterator&, field_n_type, const std::string&);
      private:
         const TPmUserDB*  UserDB_;
      }; // class TFieldMaker
      typedef TTViewArea<TOnlineUsersMap,TFieldMaker>  TOnlineVA;

      struct TvacOnlineUser : public TVACreater {
         typedef TOnlineVA       TMasterVA;
         typedef TOnlineUsersMap TMasterRecs;
         typedef TOnlineUsers    TRecs;
         struct  TFieldMaker {
            typedef TRecs                    TEvContainer;
            typedef TEvContainer::iterator   iterator;
            typedef size_t                   field_n_type;

            static K_mf(field_n_type) GetFieldCount  ();
            K_mf(const TFieldDesp&)   GetFieldDesp   (field_n_type fieldNo) const;
            K_mf(std::string)         Get  (const iterator& i, field_n_type fieldNo) const;
            K_mf(void)                Put  (TEvContainer&, iterator&, field_n_type, const std::string&);
         }; // class TFieldMaker
         typedef TTViewArea<TRecs,TFieldMaker>  TVADetail;
         K_mf(TaViewArea)  CreateNewVA  ();

         struct TDetailtMaker {
               inline K_mf(TRecs*) operator() (const TMasterRecs::iterator& mi, bool isCreateNeeded)
                        { return &(mi->second); }
         };
      };

      TvacOnlineUser    VACDetail_;
      K_mf(TVACreater*) GetDetailVACreater (size_t);
      K_mf(size_t)      GetDetails         (const char**& details);
      //繼承自 TVACreater, 給 ViewAreaMgr 管理用的 members
      K_mf(TaViewArea)  CreateNewVA        ();
   };
   TOnlineUserMgr OnlineUserMgr_;

   //以上資料表的管理員,
   //為了讓VAMgr死掉時通知以上的資料表,所以 MainVAMgr_ 必須放在上述資料表建構完成之後
   TViewAreaMgr   MainVAMgr_;

   TSignonSvr*    PostSignonSvr_;
   std::string    PrevSgnUserID_;

   K_mf(bool) CheckPrevSgnUserID(const std::string& user, std::string& user2) const;
};
/////////////////////////////////////////////////////////////////////////////
// K_fn(TSignonSvr_DataCenter&)  GetDefaultSignonSvr_DC  (); fonwin:921106:取消
/////////////////////////////////////////////////////////////////////////////
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
#endif
