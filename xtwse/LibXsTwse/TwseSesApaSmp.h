/////////////////////////////////////////////////////////////////////////////
#if !defined(_TwseSesApaSmp_h_)
#define _TwseSesApaSmp_h_
//---------------------------------------------------------------------------
#include "Bool3.h"
#include "tplns/Nsa_EvCtn_SeqHnd.h"
#include "ApaSesSmpFixSole.h"
#include "TwStkOrdFlow.h"
#include "FixStkOrdFlow.h"
#include "FixStkRptCenter.h"
#include "FixStkTradeMgr.h"
#include "TwseRes.h"
#include "TwseSesApaBase.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;
   using namespace Nsa;

   class TTwseDrProc;

/////////////////////////////////////////////////////////////////////////////
// class TTwseSesApaSmp
/////////////////////////////////////////////////////////////////////////////
class TTwseSesApaSmp : public TTwseSesApaBase<TApaSesSmpFixSole>
{
   typedef TTwseSesApaBase<TApaSesSmpFixSole>  inherited;
   typedef TTwseSesApaSmp                      this_type;

   typedef inherited::SApaMwInfo          SApaMwInfo;
   typedef TTimer<this_type>              TTwseTimerType;
   typedef std::auto_ptr<TTwseTimerType>  TTwseTimerTypePtr;

   friend class TTwseDrProc;

   enum EFixStatus
   {
      efs_NotLink,
         efs_Logon,
         efs_LogonReject,
         // 以下的狀態就利用 ESmpEventType - Session Level Events
   };

public:
   // Fix.Stk
   typedef FIX::Stk::SStkRpt         SFixStkRpt;
   typedef FIX::Stk::SStkRptWsPtr    SFixStkRptWsPtr;
   typedef FIX::Stk::SStkRptWsRef    SFixStkRptWsRef;
   typedef FIX::Stk::TFixSysOrdID    TFixSysOrdID;
   typedef FIX::Stk::TFixSysRptID    TFixSysRptID;
   typedef FIX::Stk::TOfReqNosBase   TFixOfReqNosBase;
   typedef FIX::Stk::TOfReqOcrBase   TFixOfReqOcrBase;
   typedef FIX::Stk::TOfReqOcrrBase  TFixOfReqOcrrBase;
   typedef FIX::Stk::TOfReqOsrBase   TFixOfReqOsrBase;
   typedef FIX::Stk::TOfReqNos       TFixOfReqNos;
   typedef FIX::Stk::TOfReqOcr       TFixOfReqOcr;
   typedef FIX::Stk::TOfReqOcrr      TFixOfReqOcrr;
   typedef FIX::Stk::TOfReqOsr       TFixOfReqOsr;
   typedef FIX::Stk::TFsOrdFlow      TFixOrdFlow;
   typedef FIX::Stk::TRptCenter      TFixRptCenter;
   typedef FIX::Stk::TTradeUnit      TFixTradeUnit;
   typedef FIX::Stk::TTradeUnitPtr   TFixTradeUnitPtr;
   // TwStk
   typedef Tw::Stk::TMarket          TMarket;
   typedef Tw::Stk::TOrdFlow         TOrdFlow;
   // Other
   typedef std::vector<std::string>  TIpPortList;
   typedef TIpPortList::iterator     TIpPortListI;

protected:
   //==============================================
   // class TFixRptHndEv
   class TFixRptHndEv : public TEvArySeqHnd<TFixRptCenter::TRsRpt>
   {
      typedef TEvArySeqHnd<TFixRptCenter::TRsRpt>  inherited;

      TTwseSesApaSmp*  Ses_;
      TSmpString       ApaSesID_;

   public:
      inline K_ctor TFixRptHndEv (TFixRptCenter::TRsRpt* rs, TTwseSesApaSmp* ses, int nextSno)
         : inherited(rs)
         , Ses_     (ses)
         , ApaSesID_(Ses_->GetApaSesID())
      {
         inherited::SetMoveNextCount(200);

         // 把位置位移到最後的資料上
         TFixSysRptID key(++nextSno);
         
         inherited::SendCurr(key);
      }
      
      //===========================================
      // overwrite TEvMapSeqHnd virtual function
      K_mf(EecshSendResu) OnSendData (const TFixSysRptID&, const SFixStkRptWsPtr&);
   };
   typedef wise_ptr<TFixRptHndEv>    TFixRptHndEvPtr;
   typedef TFixRptHndEvPtr::ref_obj  TFixRptHndEvRef;

   TOrdFlow*          OrdFlow_;
   TFixTradeUnitPtr   FixTradeUnit_;
   TFixRptHndEvPtr    FixRptHndEv_;
   TMarket            Market_;
   int                Password_;  // FIX 登入時的檢核密碼
   int                FixStatus_;
   TIpPortList        IpPortList_;
   TTwseTimerTypePtr  SmpGetMwInfoTimer_;
   std::string        TandSesID_;

   // 是否是第一次回補
   // 因為目前是使用 TCPClient, 所以當 IsFirstRestore_ = true 時
   // 可以解讀成程式啟動後第一次回補
   TBool3  IsFirstRestore_;

   // 委託筆數上限值
   size_t  MaxOrderCount_;
   size_t  CurOrderCount_;

public:
   K_ctor  TTwseSesApaSmp (TOrdFlow*);
   K_dtor ~TTwseSesApaSmp ();

protected:
   //==============================================
   // overwrite TSesBase virtual function
   K_mf(void) OnMesLinkReady  (TMesBase*);
   K_mf(void) OnMesLinkBroken (TMesBase*, const std::string&);
   K_mf(void) OnMesLinkFail   (TMesBase*, const std::string&);

   //==============================================
   // overwrite TApaSesSmpBase virtual function
   K_mf(void)        VirAfSettingApply (const std::string&);
   K_mf(std::string) GetLogPath        ();
   K_mf(std::string) GetLogName        ();

   //==============================================
   // overwrite TApaSesSmp virtual function
   K_mf(void) OnSmpLogon                       ();
   K_mf(void) OnSmpReject                      (TSmpMsgReject*);
   K_mf(void) OnSmpGetMiddlewareConfigResponse (TSmpMsgGetMiddlewareConfigResponse*);
   K_mf(void) OnSmpGetMiddlewareInfoResponse   (TSmpMsgGetMiddlewareInfoResponse*);
   K_mf(void) OnSmpGetSessionInfoResponse      (TSmpMsgGetSessionInfoResponse*);
   K_mf(void) OnSmpMwEv_EndOfRestore           (TSmpMsgMiddlewareEvent*);
   K_mf(void) OnSmpMwEv_SessionEvents          (TSmpMsgMiddlewareEvent*);
   K_mf(void) OnSmpMwEv_MessageValidationError (TSmpMsgMiddlewareEvent*);
   
   //K_mf(void) OnSmpReject_AlreadyLogon ();

   K_mf(void) VirFireBfFirstRestore       (const TSmpString& apaSesID);
   K_mf(void) VirFireFixInMsgSeqNumStatus (const TSmpInt& mySeqNum, const TSmpInt& mwSeqNum, bool isChkFirst, bool isRestore);

   K_mf(void) VirOnShowState              (std::string&);
   K_mf(void) VirOnSesTimer               ();

   K_mf(void) VirBfSmpRestoreMessage (const TSmpString&, const TSmpInt&, const TSmpInt&);

   Declare_ApaSesSmp_OnSmpGetSessionListResponse_Default
   Declare_ApaSesSmp_OnSmpOperCmdResponse_Default
   Declare_ApaSesSmp_OnSmpMwEv_OutboundAcknowledgmentEvents_Default

   //==============================================
   // overwrite TApaSesSmpFixSole virtual function
   Declare_ApaSesSmpFixSole_OnFixHeartbeat_Default
   Declare_ApaSesSmpFixSole_OnFixLogout_Default
   Declare_ApaSesSmpFixSole_OnFixResendRequest_Default
   Declare_ApaSesSmpFixSole_OnFixSequenceReset_Default
   
   K_mf(void) OnGetFixVer ();
   //------------------------------------
   K_mf(void) OnFixLogon                     (FIX::TMessageRef, bool);
   //------------------------------------
   K_mf(void) OnFixNewOrderSingle            (FIX::TMessageRef, bool);
   K_mf(void) OnFixOrderCancelRequest        (FIX::TMessageRef, bool);
   K_mf(void) OnFixOrderCancelReplaceRequest (FIX::TMessageRef, bool);
   K_mf(void) OnFixOrderStatusRequest        (FIX::TMessageRef, bool);
   K_mf(void) OnFixAppOther                  (const FIX::TFixMsgType2&, FIX::TMessageRef, bool);

   //==============================================
   // TTwseSesApaSmp function
   K_mf(void)               OnTwseTimer      (TTimerBase*);
   K_mf(ETwseFixRejectCode) CheckLogon       (FIX::TMessageRef);
   K_mf(void)               SendLogonEcho    (FIX::TMessageRef, ETwseFixRejectCode);
   K_mf(void)               ChangeMesSetting (TMesBase*);

   // for TTwseDrProc
   K_mf(void) DrPolling ();

private:
   K_mf(bool) IsDr       ();
   K_mf(bool) IsDrFinish ();
   K_mf(void) DrProc     ();
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseSesApaSmp_h_)
