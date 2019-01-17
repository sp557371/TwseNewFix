/////////////////////////////////////////////////////////////////////////////
#if !defined(_ApaSesSmp_h_)
#define _ApaSesSmp_h_
//---------------------------------------------------------------------------
// libns
#include "Ses.h"
#include "TimerThread.h"
// libnsAux
#include "Nsa_FileWriter.h"
#include "Nsa_UtilTime.h"
// SmAppia
#include "ApaSmpMsg.h"
#include "ApaSesConf.h"
#include <memory>
/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TApaSesSmpBase
/////////////////////////////////////////////////////////////////////////////
K_class TApaSesSmpBase : public TSesBase
{
   typedef TSesBase  inherited;

protected:
   enum ESesProp
   {
      esp_Disable_PktLog,
      esp_Disable_RecvLog,
      esp_Disable_SendLog,
      esp_Total
   };
   typedef std::bitset<esp_Total>  TSesProp;

protected:
   class TPktParse;

   TPktParse*      PktParse_;
   SAppiaConf      Conf_;
   TFileWriterHnd  PktFileHnd_;
   TFileWriterHnd  LogFileHnd_;
   TSesProp        SesProp_;
   std::string     AppiaGrp_;

public:
           K_ctor  TApaSesSmpBase ();
   virtual K_dtor ~TApaSesSmpBase ();

   //==============================================
   // TApaSesSmpBase virtual function
   virtual K_mf(std::string) GetLogPath ();
   virtual K_mf(std::string) GetLogName ();

   //==============================================
   // TApaSesSmpBase function
   K_mf(int) Send (const void*, size_t);

protected:
   //==============================================
   // overwrite TSesBase virtual function
   K_mf(void) OnMesLinkReady  (TMesBase*);
   K_mf(void) OnMesLinkBroken (TMesBase*, const std::string&);
   K_mf(void) OnMesLinkFail   (TMesBase*, const std::string&);
   K_mf(void) OnMesRecvReady  (TMesBase*, size_t size);
   K_mf(void) OnMesSendEmpty  (TMesBase*);

   //==============================================
   // TApaSesSmpBase virtual function
   virtual K_mf(void) OnPktRecv         (const std::string&, const TKDateTimeMS*) = 0;
   virtual K_mf(bool) VirBfSettingApply (      std::string&) { return true; }
   virtual K_mf(void) VirAfSettingApply (const std::string&) {}

   //==============================================
   // TApaSesSmpBase function
   inline K_mf(const std::string&) GetMweID () const  { return Conf_.MiddlewareID_; }

private:
   //==============================================
   // overwrite TSesBase virtual function
   K_mf(bool) SettingApply (const std::string&);
};


/////////////////////////////////////////////////////////////////////////////
// class TApaSesSmp
/////////////////////////////////////////////////////////////////////////////
K_class TApaSesSmp : public TApaSesSmpBase
{
protected:
   typedef TApaSesSmpBase             inherited;
   typedef TApaSesSmp                 this_type;
   typedef TTimer<this_type>          TTimerType;
   typedef std::auto_ptr<TTimerType>  TTimerTypePtr;

   enum EApaMwStatus
   {
      ams_LinkError,
      ams_RequestLogon,
      ams_Logon,
      ams_NotLogon,
      ams_AlreadyLogon,
   };

   struct SApaMwInfo
   {
      EApaMwStatus  Status_;         // 目前 Ses <-> ApaMw 的狀態
      TTimerCheck   TmChkLogon_;     // 檢查 Ses  -> ApaMw 是否 LogonTimeout
      int           LogonInterval_;  // 等待 Ses  -> ApaMw LogonTimeout 的 ms

      TTimerCheck   TmChkHb_;        // 檢查 Ses <-> ApaMw 之間的連線狀態
      int           ItvlBfSendHb_;   // 傳送 HB 的     Interval ms
      int           ItvlAfSendHb_;   // 等待 HB 回覆的 Interval ms

      int           ReopenItvl_;     // 因某種原因自動斷線後,重連的秒數

      inline K_ctor SApaMwInfo ()
         : Status_       (ams_LinkError)
         , LogonInterval_(5000)  // 預設 5 秒
         , TmChkHb_      (false) // 預設不啟動
         , ItvlBfSendHb_ (0)
         , ItvlAfSendHb_ (0)
         , ReopenItvl_   (2)     // 預設 2 秒
      {}

      // 傳入單位是秒喔
      K_mf(void) SetHbItvl (int itvlBf, int itvlAf)
      {
         TmChkHb_.SetEnable(false);
         ItvlBfSendHb_ = itvlBf * 1000;
         ItvlAfSendHb_ = itvlAf * 1000;
         
         if (ItvlBfSendHb_ != 0)
            TmChkHb_.SetEnable(true);
      }
   };

protected:
   // Timer
   TTimerTypePtr  SesTimer_;
   TTimerTypePtr  ReopenTimer_;
   //
   std::string    LastState_;
   SApaMwInfo     ApaMwInfo_;

public:
   K_ctor  TApaSesSmp ();
   K_dtor ~TApaSesSmp ();

   //==============================================
   // TApaSesSmp virtual function
   //==============================================
   // 用途:每次登入 MW 後,如果有需要回補時,事先通知的機制
   // 使用:在回補之前,我想要先 Hold,等回補在在解 Hold
   virtual K_mf(void) VirFireBfFirstRestore (const TSmpString& apaSesID) {}

   virtual K_mf(void) VirFireFixInMsgSeqNumStatus (const TSmpInt& mySeqNum,   const TSmpInt& mwSeqNum,
                                                         bool     isChkFirst,       bool     isRestore) {}

   //==============================================
   // TApaSesSmp function
   K_mf(void) SendMsg (TSmpMsg&);
   K_mf(void) SendCmd (const TSmpOperCmd&);

   //==============================================
   // smp request
   K_mf(void) SmpRestoreMessage          (const TSmpString&, const TSmpInt&, const TSmpInt&);
   K_mf(void) SmpOutboundMessageDelivery (const TSmpString&, const TSmpString&, const TSmpString&, const TSmpString&);

   //==============================================
   // TApaSesSmp inline
   inline K_mf(TFileWriterHnd&) GetLogFileHnd ()  { return LogFileHnd_; }

protected:
   //==============================================
   // overwrite TApaSesSmpBase virtual function
   K_mf(void) OnMesLinkReady  (TMesBase*);
   K_mf(void) OnMesLinkBroken (TMesBase*, const std::string&);
   K_mf(void) OnMesLinkFail   (TMesBase*, const std::string&);
   //----------------------------------------------
   K_mf(void) OnPktRecv       (const std::string&, const TKDateTimeMS*);

   //==============================================
   // TApaSesSmp virtual function
   virtual K_mf(void) OnSmpLogon                             ()                                    = 0;
   virtual K_mf(void) OnSmpMwEv_SesMsgRecv                   (TSmpMsgRef)                          = 0;
   virtual K_mf(void) OnSmpMwEv_AppMsgRecv                   (TSmpMsgMiddlewareEvent*)             = 0;
   virtual K_mf(void) OnSmpMwEv_IncomingMessageRestored      (TSmpMsgMiddlewareEvent*)             = 0;
   virtual K_mf(void) OnSmpMwEv_EndOfRestore                 (TSmpMsgMiddlewareEvent*)             = 0;
   virtual K_mf(void) OnSmpMwEv_SessionEvents                (TSmpMsgMiddlewareEvent*)             = 0;
   virtual K_mf(void) OnSmpMwEv_OutboundAcknowledgmentEvents (TSmpMsgMiddlewareEvent*)             = 0;
   virtual K_mf(void) OnSmpMwEv_MessageValidationError       (TSmpMsgMiddlewareEvent*)             = 0;
   virtual K_mf(void) OnSmpGetMiddlewareInfoResponse         (TSmpMsgGetMiddlewareInfoResponse*)   = 0;
   virtual K_mf(void) OnSmpGetMiddlewareConfigResponse       (TSmpMsgGetMiddlewareConfigResponse*) = 0;
   virtual K_mf(void) OnSmpGetSessionListResponse            (TSmpMsgGetSessionListResponse*)      = 0;
   virtual K_mf(void) OnSmpGetSessionInfoResponse            (TSmpMsgGetSessionInfoResponse*)      = 0;
   virtual K_mf(void) OnSmpOperCmdResponse                   (TSmpMsgOperatorCommandResponse*)     = 0;
   virtual K_mf(void) OnSmpReject                            (TSmpMsgReject*)                      = 0;
   //----------------------------------------------
   virtual K_mf(void) OnSmpReject_AlreadyLogon ();
   //----------------------------------------------
   virtual K_mf(void) VirOnShowState (std::string&)  {}

   //==============================================
   // TSesAppiaSmp function
   K_mf(void) SmpLogon                      ();
   K_mf(void) SmpStartMiddlwareEvents       ();
   K_mf(void) SmpGetSessionList             ();
   K_mf(void) SmpGetMiddlewareConfig        ();
   K_mf(void) SmpGetMiddlewareInfoRequest   ();
   K_mf(void) SmpGetSessionInfoRequest      (const TSmpString& sessionID);
   K_mf(void) SmpSetMiddlewareConfigRequest (const SSmpMiddlewareConfig&);
   //----------------------------------------------
   K_mf(void) DoSmpLogon           ();
   K_mf(void) DoSmpReject          (TSmpMsgReject*);
   K_mf(void) DoSmpMwEv            (TSmpMsgRef);
   K_mf(void) DoSmpMwEv_SesMsgRecv (TSmpMsgRef);
   K_mf(void) DoSmpMwEv_AppMsgRecv (TSmpMsgMiddlewareEvent*);
   //----------------------------------------------
   K_mf(void) Reopen         (const std::string&, int itvlSec = 2); //預設2秒
   K_mf(void) ShowState      ();
   K_mf(void) OnTimer        (TTimerBase*);
   K_mf(bool) CheckHeartbeat ();
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

//////////////////////////////////////////////////////////////////////////////////////////////////
// TApaSesSmp virtual function default
//////////////////////////////////////////////////////////////////////////////////////////////////
/*
   Declare_ApaSesSmp_OnSmpLogon_Default
   Declare_ApaSesSmp_OnSmpMwEv_SesMsgRecv_Default
   Declare_ApaSesSmp_OnSmpMwEv_AppMsgRecv_Default
   Declare_ApaSesSmp_OnSmpMwEv_IncomingMessageRestored_Default
   Declare_ApaSesSmp_OnSmpMwEv_EndOfRestore_Default
   Declare_ApaSesSmp_OnSmpMwEv_MessageValidationError_Default
   Declare_ApaSesSmp_OnSmpGetMiddlewareInfoResponse_Default
   Declare_ApaSesSmp_OnSmpGetMiddlewareConfigResponse_Default
   Declare_ApaSesSmp_OnSmpGetSessionListResponse_Default
   Declare_ApaSesSmp_OnSmpGetSessionInfoResponse_Default
   Declare_ApaSesSmp_OnSmpOperCmdResponse_Default
   Declare_ApaSesSmp_OnSmpReject_Default
*/   
//------------------------------------------------------------------------------------------------
#define Declare_ApaSesSmp_OnSmpLogon_Default                                                    \
   inline K_mf(void) OnSmpLogon () {}                                                           \
//------------------------------------------------------------------------------------------------
#define Declare_ApaSesSmp_OnSmpMwEv_SesMsgRecv_Default                                          \
   inline K_mf(void) OnSmpMwEv_SesMsgRecv (TSmpMsgRef) {}                                       \
//------------------------------------------------------------------------------------------------
#define Declare_ApaSesSmp_OnSmpMwEv_AppMsgRecv_Default                                          \
   inline K_mf(void) OnSmpMwEv_AppMsgRecv (TSmpMsgMiddlewareEvent*) {}                          \
//------------------------------------------------------------------------------------------------
#define Declare_ApaSesSmp_OnSmpMwEv_IncomingMessageRestored_Default                             \
   inline K_mf(void) OnSmpMwEv_IncomingMessageRestored (TSmpMsgMiddlewareEvent*) {}             \
//------------------------------------------------------------------------------------------------
#define Declare_ApaSesSmp_OnSmpMwEv_EndOfRestore_Default                                        \
   inline K_mf(void) OnSmpMwEv_EndOfRestore (TSmpMsgMiddlewareEvent*) {}                        \
//------------------------------------------------------------------------------------------------
#define Declare_ApaSesSmp_OnSmpMwEv_SessionEvents_Default                                       \
   inline K_mf(void) OnSmpMwEv_SessionEvents (TSmpMsgMiddlewareEvent*) {}                       \
//------------------------------------------------------------------------------------------------
#define Declare_ApaSesSmp_OnSmpMwEv_OutboundAcknowledgmentEvents_Default                        \
   inline K_mf(void) OnSmpMwEv_OutboundAcknowledgmentEvents (TSmpMsgMiddlewareEvent*) {}        \
//------------------------------------------------------------------------------------------------
#define Declare_ApaSesSmp_OnSmpMwEv_MessageValidationError_Default                              \
   inline K_mf(void) OnSmpMwEv_MessageValidationError (TSmpMsgMiddlewareEvent*) {}              \
//------------------------------------------------------------------------------------------------
#define Declare_ApaSesSmp_OnSmpGetMiddlewareInfoResponse_Default                                \
   inline K_mf(void) OnSmpGetMiddlewareInfoResponse (TSmpMsgGetMiddlewareInfoResponse*) {}      \
//------------------------------------------------------------------------------------------------
#define Declare_ApaSesSmp_OnSmpGetMiddlewareConfigResponse_Default                              \
   inline K_mf(void) OnSmpGetMiddlewareConfigResponse (TSmpMsgGetMiddlewareConfigResponse*) {}  \
//------------------------------------------------------------------------------------------------
#define Declare_ApaSesSmp_OnSmpGetSessionListResponse_Default                                   \
   inline K_mf(void) OnSmpGetSessionListResponse (TSmpMsgGetSessionListResponse*) {}            \
//------------------------------------------------------------------------------------------------
#define Declare_ApaSesSmp_OnSmpGetSessionInfoResponse_Default                                   \
   inline K_mf(void) OnSmpGetSessionInfoResponse (TSmpMsgGetSessionInfoResponse*) {}
//------------------------------------------------------------------------------------------------
#define Declare_ApaSesSmp_OnSmpOperCmdResponse_Default                                          \
   inline K_mf(void) OnSmpOperCmdResponse (TSmpMsgOperatorCommandResponse*) {}                  \
//------------------------------------------------------------------------------------------------
#define Declare_ApaSesSmp_OnSmpReject_Default                                                   \
   inline K_mf(void) OnSmpReject (TSmpMsgReject*) {}                                            \
//------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_ApaSesSmp_h_)
