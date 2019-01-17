/////////////////////////////////////////////////////////////////////////////
#if !defined(_TserSesApaSmp_h_)
#define _TserSesApaSmp_h_
//---------------------------------------------------------------------------
#include "ApaSesSmpFixMult.h"
#include "TwseRes.h"
#include "TwseSesApaBase.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   class TTserIni;

/////////////////////////////////////////////////////////////////////////////
// class TTserSesApaSmp
/////////////////////////////////////////////////////////////////////////////
class TTserSesApaSmp : public TTwseSesApaBase<TApaSesSmpFixMult>
{
   typedef TTwseSesApaBase<TApaSesSmpFixMult>  inherited;

   class TCliMsgID;
   
   TCliMsgID*  CliMsgID_;

public:
   K_ctor  TTserSesApaSmp ();
   K_dtor ~TTserSesApaSmp ();

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
   K_mf(void) OnSmpLogon                             ();
   K_mf(void) OnSmpGetMiddlewareInfoResponse         (TSmpMsgGetMiddlewareInfoResponse*);
   K_mf(void) OnSmpGetSessionInfoResponse            (TSmpMsgGetSessionInfoResponse*);
   K_mf(void) OnSmpMwEv_OutboundAcknowledgmentEvents (TSmpMsgMiddlewareEvent*);

   Declare_ApaSesSmp_OnSmpGetMiddlewareConfigResponse_Default
   Declare_ApaSesSmp_OnSmpGetSessionListResponse_Default
   Declare_ApaSesSmp_OnSmpOperCmdResponse_Default
   Declare_ApaSesSmp_OnSmpReject_Default
   Declare_ApaSesSmp_OnSmpMwEv_SessionEvents_Default
   Declare_ApaSesSmp_OnSmpMwEv_MessageValidationError_Default
   
   //==============================================
   // overwrite TApaSesSmpFixMult virtual function
   K_mf(void) OnFixLogon (const TSmpString& sesID, TMessageRef, bool isRt, TSmpMsgRef);

   Declare_ApaSesSmpFixMult_OnFixLogout_Default
   Declare_ApaSesSmpFixMult_OnFixHeartbeat_Default
   Declare_ApaSesSmpFixMult_OnFixResendRequest_Default
   Declare_ApaSesSmpFixMult_OnFixSequenceReset_Default
   Declare_ApaSesSmpFixMult_OnFixAdminOther_Default
   Declare_ApaSesSmpFixMult_OnFixNewOrderSingle_Default
   Declare_ApaSesSmpFixMult_OnFixOrderCancelRequest_Default
   Declare_ApaSesSmpFixMult_OnFixOrderCancelReplaceRequest_Default
   Declare_ApaSesSmpFixMult_OnFixOrderStatusRequest_Default
   Declare_ApaSesSmpFixMult_OnFixAppOther_Default
   
   //==============================================
   // TTserSesApaSmp function
   K_mf(ETwseFixRejectCode) CheckLogon     (                               const Message&, int pwd);
   K_mf(void)               SendLogoutEcho (const TSmpString& strApaSesID, const Message&, ETwseFixRejectCode);
   K_mf(std::string)        MakeFullIPAddr (const std::string);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TserSesApaSmp_h_)
