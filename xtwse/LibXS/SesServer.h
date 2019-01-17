//
// 通訊classes基礎: Session-Messager
//    class TSesServer; 主機端的 PkSes
//
// 版本: $Id: SesServer.h,v 1.7 2005/05/18 03:27:28 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SesServerH
#define SesServerH
//---------------------------------------------------------------------------
#include "SesPk.h"
#include "Signon/SignonSvr.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Pk {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class  TSesServer : public TSesPk
                        , public Kway::Signon::TSignonSvrEvHandler 
{
   typedef TSesPk   inherited;

   std::string      ServerName_;
   std::string      UserID_;
   std::string      ClientAp_;
   std::string      UFrom_;
   TVer             ClientVer_;
   std::string      ServerAp_;

protected:
   K_mf(void) OnBfDestroy        ();
   K_mf(void) OnMesLinkReady     (TMesBase*);
   K_mf(void) OnMesLeave         (TMesBase*);
   K_mf(void) OnHeartbeat        (const THeartbeat&);
   K_mf(void) OnPkWorkFinished   (const THead&);
   K_mf(void) OnTimer            (TTimerBase*);
   K_mf(void) OnSignonSvrDestroy (Kway::Signon::TSignonSvr*);

   typedef Kway::Signon::TSignonSvr TSignonSvr;
   typedef TSignonSvr::TOnlineID    TOnlineID;

private:
   TSignonSvr*      SignonSvr_;
   TOnlineID        OnlineID_;

public:
   K_ctor  TSesServer  (TWorkersBase&);
   K_dtor ~TSesServer  ();

   /** SetUserReady(): svr==0表示,登入失敗.
   */
   virtual K_mf(std::string) SetUserReady (TSignonSvr* svr, const std::string& user, TOnlineID);
   virtual K_mf(bool)        Signout      ();

          K_mf(bool)               SettingApply  (const std::string& serverName);
   inline K_mf(const std::string&) GetServerName () const    { return ServerName_; }///< = SettingApply(serverName)
   inline K_mf(void)               SetServerAp   (const std::string& v) { ServerAp_ = v; }
   inline K_mf(const std::string&) GetServerAp   () const    { return ServerAp_;   }///< = TWorker_OnConnectedReq: ServerName + Ver
   inline K_mf(const std::string&) GetUserID     () const    { return UserID_;     }
   inline K_mf(TOnlineID)          GetOnlineID   () const    { return OnlineID_;   }
   inline K_mf(const std::string&) GetClientAp   () const    { return ClientAp_;   }
   inline K_mf(const TVer&)        GetClientVer  () const    { return ClientVer_;  }
   virtual K_mf(void)             OnCliApConnect (EState newState, const std::string& ap, const TVer&);

   inline static K_mf(std::string) GetLastOpUser ()                     { return LastOpUser(); }
   inline static K_mf(void)        ClrLastOpUser ()                     { LastOpUser().clear();}
   inline static K_mf(void)        SetLastOpUser (const std::string& u) { LastOpUser() = u;    }

   inline K_mf(TSignonSvr*) GetSignonSvr () const  { return SignonSvr_; }

private:
   static K_mf(std::string&)  LastOpUser  ();
};
/////////////////////////////////////////////////////////////////////////////
class TConnectedReq;   // "Pk/PkConnected.h"
class TConnectedAck;
//-------------------
LIBNS_class TWorker_OnConnectedReq : public TWorker
{
   typedef TWorker   inherited;

   virtual K_mf(void)  OnConnectedRecv  (TSesPk&, const TConnectedReq&, TConnectedAck&) {}
protected:
   K_mf(TCheckResult)  OnPkCheck (TSesPk&, const THead&, TSize recvSize);
   K_mf(void)          OnPkRecv  (TSesPk&, const THead&);

   TVer           ServerVer_;
   std::string    ServerName_;
public:
   K_ctor  TWorker_OnConnectedReq  (TWorker* regs[], const TVer& serverVer, const std::string& serverName);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Pk
} // namespace Kway
//---------------------------------------------------------------------------
#endif

