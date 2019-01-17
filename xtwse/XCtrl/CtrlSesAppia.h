/////////////////////////////////////////////////////////////////////////////
#if !defined(_CtrlSesAppia_h_)
#define _CtrlSesAppia_h_
//---------------------------------------------------------------------------
#include "ApaSesSmp.h"
#include "TwseSesApaBase.h"
#include "CtrlRsReqResu.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   class TCtrlSesMgr;

/////////////////////////////////////////////////////////////////////////////
// class TCtrlSesAppia
/////////////////////////////////////////////////////////////////////////////
class TCtrlSesAppia : public TTwseSesApaBase<TApaSesSmp>
{
   typedef TTwseSesApaBase<TApaSesSmp>  inherited;
   typedef TCtrlSesAppia                this_type;
   typedef TTimer<this_type>            TTimerType;
   typedef TCtrlRsReqResu::TRsFixSesID  TRsFixSesID;
   typedef TCtrlRsReqResu::TRecsHnd     TRsReqResuHnd;

   TCtrlSesMgr*       SesMgr_;
   TCtrlRsReqResuGoh  RsReqResuGoh_;
   TReqID             ReqID_;
   TTimerType         Timer_;
   //std::string        MyName_;
   char               MarketChar_;
   TStrings           FIXSesIDs_;

public:
   K_ctor  TCtrlSesAppia (TCtrlSesMgr*);
   K_dtor ~TCtrlSesAppia ();

protected:
   //==============================================
   // overwrite TSesBase virtual function
   K_mf(void) OnMesLinkReady  (TMesBase*);
   K_mf(void) OnMesLinkBroken (TMesBase*, const std::string&);
   K_mf(void) OnMesLinkFail   (TMesBase*, const std::string&);
   
   //==============================================
   // overwrite TApaSesSmpBase virtual function
   K_mf(bool)        VirBfSettingApply (std::string&);
   K_mf(std::string) GetLogPath        ();
   K_mf(std::string) GetLogName        ();

   //==============================================
   // overwrite TApaSesSmp virtual function
   Declare_ApaSesSmp_OnSmpMwEv_SesMsgRecv_Default
   Declare_ApaSesSmp_OnSmpMwEv_AppMsgRecv_Default
   Declare_ApaSesSmp_OnSmpMwEv_IncomingMessageRestored_Default
   Declare_ApaSesSmp_OnSmpMwEv_EndOfRestore_Default
   Declare_ApaSesSmp_OnSmpMwEv_OutboundAcknowledgmentEvents_Default
   Declare_ApaSesSmp_OnSmpMwEv_MessageValidationError_Default
   Declare_ApaSesSmp_OnSmpGetMiddlewareInfoResponse_Default
   Declare_ApaSesSmp_OnSmpGetMiddlewareConfigResponse_Default
   Declare_ApaSesSmp_OnSmpGetSessionListResponse_Default

   K_mf(void) OnSmpLogon                  ();
   K_mf(void) OnSmpGetSessionInfoResponse (TSmpMsgGetSessionInfoResponse*);
   K_mf(void) OnSmpOperCmdResponse        (TSmpMsgOperatorCommandResponse*);
   K_mf(void) OnSmpReject                 (TSmpMsgReject*);
   K_mf(void) OnSmpMwEv_SessionEvents     (TSmpMsgMiddlewareEvent*);

   //==============================================
   // TCtrlSesAppia function
   K_mf(void)      OnTimer          (TTimerBase*);
   K_mf(void)      SendQuery        (const TFixSesID&);
   K_mf(EProcResu) CheckStatus      (TRsReqResuHnd&);
   K_mf(void)      CheckSend        (TRsReqResuHnd&);
   K_mf(ESmpCmd)   FuncCodeToSmpCmd (ETandFuncCode);
   K_mf(bool)      CheckMarket      (const TSmpString&);
   K_mf(bool)      IsMyFIXSes       (const std::string);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_CtrlSesAppia_h_)
