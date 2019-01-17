//
// �q�Tclasses��¦: Session-Messager
//    class TSesClient; �Ȥ�ݪ� PkSes
//
// ����: $Id: SesClient.h,v 1.7 2006/04/04 03:59:25 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SesClientH
#define SesClientH
//---------------------------------------------------------------------------
#include "SesPk.h"
#include "Pk/PkConnected.h"
#include "Pk/PkSignon.h"
//---------------------------------------------------------------------------
#include "Signon/SignonClient.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Pk {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class  TSesClient;
LIBNS_class  TOnlineClient;
//---------------------------------------------------------------------------
LIBNS_class  TSesClient : public TSesPk
{
   typedef TSesPk   inherited;

   TVer           ClientVer_;
   std::string    ClientApName_;
   byte           HeartbeatTimes_;
   THiLoTransRef  ServerHiLoRef_;//�b���� TConnectedAck �ɳ]�w!
   std::string    ServerName_;
   TVer           ServerVer_;
   TOnlineClient* OnlineClient_; //�h��Session�i��@�ΦP�@��OnlineClient
   bool           HbSended_;     //�bHeartbeat �p�ɾ��ɶ���,�O�_���e�X���!
   Kway::byte     PassEnc_;      //�K�X�ǰe�ɪ��[�K�覡
   Kway::byte     PassXor_;

   K_mf(std::string) EncodePass (const std::string& pass);

protected:
   friend class TOnlineClient;
   virtual K_mf(void) OnOnlineClientDestroy (TOnlineClient* onlineClient);

   virtual K_mf(void) OnSignon    (const std::string& result);//SetState(result.empty() ? sp_Ready : sp_WaitSignonInput)
   virtual K_mf(void) SendSignon  (const char** policyNamesBegin, size_t policyNamesCount);
   virtual K_mf(void) SendChgPass (const std::string& user, const std::string& oldpass, const std::string& newpass);
   virtual K_mf(void) SendChkTableRight (const std::string& user, const std::string& pass, const std::string& tableName);

   K_mf(void)    MesLinkReady    (TMesBase*, const char** policyNamesBegin, size_t policyNamesCount);
   K_mf(void)  OnMesLinkBroken   (TMesBase*, const std::string& reason);

   K_mf(void)  OnTimer           (TTimerBase*); //�eHeartbeat
   K_mf(void)  OnPkWorkFinished  (const THead&);//�M�� HeartbeatTimes_

public:
   K_ctor  TSesClient  (TWorkersBase&, TVer, const std::string& apName, TOnlineClient&);
   K_dtor ~TSesClient  ();

           K_mf(bool)  SendPk          (const THead&);
   virtual K_mf(void)  OnConnectedAck  (const TConnectedAck&);

   inline K_mf(THiLoTransRef)  GetServerHiLoRef  () const  { return ServerHiLoRef_; }
   inline K_mf(std::string)    GetServerName     () const  { return ServerName_;    }
   inline K_mf(TVer)           GetServerVer      () const  { return ServerVer_;     }
   inline K_mf(TOnlineClient*) GetOnlineClient   () const  { return OnlineClient_;  }
 //inline K_mf(const TGroups&) GetGroups         () const  { return Groups_;        }
   inline K_mf(const std::string&) GetClientApName () const { return ClientApName_; }
};
/////////////////////////////////////////////////////////////////////////////
LIBNS_class  TOnlineClient : public Signon::TSignonClient, public TWorker
{
   typedef Signon::TSignonClient    inheritedSignon;
   //------ data members
   std::string    Pass_;
   typedef std::vector<const char*> TReqPolicys;
   TReqPolicys    ReqPolicys_;
   bool           AutoSignon_;
   //---------- for Pk::TWorker ------------
   typedef TWorker   inheritedWorker;
   K_mf(TCheckResult)  OnPkCheck       (TSesPk&, const THead&, TSize recvSize);
   K_mf(void)          OnPkRecv        (TSesPk&, const THead&);
   K_mf(void)          OnConnectedAck  (TSesPk&, const TConnectedAck&);
   K_mf(bool)          OnSignonAck     (TSesPk&, const TSignonAck::TSignonResult&);
   K_mf(TSesClient*)   GetWaitSignonInput  (bool IsExcludeReady);

public:
   K_ctor  TOnlineClient  (TWorkersBase&);
   K_dtor ~TOnlineClient  ();

   inline K_mf(void)  AddSes  (TSesClient& ses)    { inheritedSignon::AddSes(ses); }
   inline K_mf(void)  DelSes  (TSesClient& ses)    { inheritedSignon::DelSes(ses); }

   //���Ѱ򥻪��A��:
   /** autoSignon�Y��true,��ܷ�Ses�s�u���\��,�۰ʵn�J(�w�]��).
                     false,��ܦ����I�s�Y�S��Ses�i�n�J,�h�|�i�J�n�J���Ѫ����A.
       ��mf�ȧ@�]�w,�����ܥثe��ñ�J���A.
   */
   K_mf(void)  SetAutoSignon (bool autoSignon);

   K_mf(void)  Signon   (const std::string& user, const std::string& pass);
   K_mf(void)  ChgPass  (const std::string& user, const std::string& oldPass, const std::string& newPass);
   K_mf(void)  Signout  ();
   K_mf(void)  CheckTableRight (const std::string& user, const std::string& pass, const std::string& tableName);

   //�� State >= sc_WaitSignonResult ��, GetPass() �~���N�q
   K_mf(const std::string&) GetPass () const   { return Pass_; }

   //�W��n�J�ɻݭn�� ReqPolicys.
   K_mf(void)  AddSignonPolicy  (const char** policys, int count);
   K_mf(void)  DelSignonPolicy  (const char** policys, int count);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Pk
} // namespace Kway
//---------------------------------------------------------------------------
#endif

