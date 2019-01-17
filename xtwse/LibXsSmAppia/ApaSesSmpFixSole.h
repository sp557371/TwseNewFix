/////////////////////////////////////////////////////////////////////////////
#if !defined(_ApaSesSmpFixSole_h_)
#define _ApaSesSmpFixSole_h_
//---------------------------------------------------------------------------
#include "ApaSesSmpFixUtil.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace FIX;

/////////////////////////////////////////////////////////////////////////////
// class TApaSesSmpFixSole
/////////////////////////////////////////////////////////////////////////////
K_class TApaSesSmpFixSole : public TApaSesSmpFix
{
protected:
   typedef TApaSesSmpFix  inherited;

   enum ESoleMode
   {
      // 使用自行指定的 SessionID
      esm_UseMySessionID,

      esm_Total
   };
   typedef std::bitset<esm_Total>  TSoleMode;

   enum ESoleFlag
   {
      esf_Hold,   // 目前指定的 Session 是否 HoldFlag=true
      esf_Total
   };
   typedef std::bitset<esf_Total>  TSoleFlag;

protected:
   TSmpString       ApaSesID_;         // connection = [T5U3U8U0UXU1U]
   TSmpString       FixTargetCompID_;  // [T5U3U8U0UXU1U]
                                       // remote_firm_id = T5380X1
   TSoleMode        SoleMode_;
   TSoleFlag        SoleFlag_;
   TApaFixRecover*  Recover_;
   
   // 傳送端的 Fix 版本(數字)
   int  FixVer_;

public:
   K_ctor  TApaSesSmpFixSole ();
   K_dtor ~TApaSesSmpFixSole ();

   //==============================================
   // overwrite TApaSesSmpFix virtual function
   K_mf(void) DispFixMessage (SDispFixMsgInfo&);

   //==============================================
   // smp request
   K_mf(void) SmpOutboundMessageDelivery (int cliMsgID, const TSmpString& msgType, const TSmpString& msgData,
                                          const TSmpString* sessionID = NULL);

   inline K_mf(const TSmpString&) GetApaSesID        () const  { return ApaSesID_;        }
   inline K_mf(const TSmpString&) GetFixTargetCompID () const  { return FixTargetCompID_; }

protected:
   //==============================================
   // overwrite TSesBase virtual function
   K_mf(void) OnMesLinkReady  (TMesBase*);
   K_mf(void) OnMesLinkBroken (TMesBase*, const std::string&);

   //==============================================
   // overwrite TApaSesSmp virtual function
   K_mf(void) OnSmpGetMiddlewareConfigResponse  (TSmpMsgGetMiddlewareConfigResponse*);
   K_mf(void) OnSmpGetMiddlewareInfoResponse    (TSmpMsgGetMiddlewareInfoResponse*);
   K_mf(void) OnSmpMwEv_SesMsgRecv              (TSmpMsgRef);
   K_mf(void) OnSmpMwEv_AppMsgRecv              (TSmpMsgMiddlewareEvent*);
   K_mf(void) OnSmpMwEv_IncomingMessageRestored (TSmpMsgMiddlewareEvent*);
   K_mf(void) OnSmpMwEv_EndOfRestore            (TSmpMsgMiddlewareEvent*);
   K_mf(void) OnSmpMwEv_SessionEvents           (TSmpMsgMiddlewareEvent*);

   //==============================================
   // TApaSesSmpFixSole virtual function
   virtual K_mf(void) OnGetFixVer () = 0;
   //------------------------------------
   virtual K_mf(void) OnFixLogon                     (TMessageRef, bool isRt) = 0;
   virtual K_mf(void) OnFixLogout                    (TMessageRef, bool isRt) = 0;
   virtual K_mf(void) OnFixHeartbeat                 (TMessageRef, bool isRt) = 0;
   virtual K_mf(void) OnFixResendRequest             (TMessageRef, bool isRt) = 0;
   virtual K_mf(void) OnFixSequenceReset             (TMessageRef, bool isRt) = 0;
   //------------------------------------
   virtual K_mf(void) OnFixNewOrderSingle            (TMessageRef, bool isRt) = 0;
   virtual K_mf(void) OnFixOrderCancelRequest        (TMessageRef, bool isRt) = 0;
   virtual K_mf(void) OnFixOrderCancelReplaceRequest (TMessageRef, bool isRt) = 0;
   virtual K_mf(void) OnFixOrderStatusRequest        (TMessageRef, bool isRt) = 0;
   virtual K_mf(void) OnFixAppOther                  (const TFixMsgType2&, TMessageRef, bool isRt) = 0;

   //==============================================
   // TApaSesSmpFixSole function
   inline K_mf(void) SmpGetSessionInfoRequest ()  { inherited::SmpGetSessionInfoRequest(ApaSesID_); }
};

/////////////////////////////////////////////////////////////////////////////
} // namespace Kway

/////////////////////////////////////////////////////////////////////////////
// TApaSesSmpFixSole virtual function default
/////////////////////////////////////////////////////////////////////////////
#define Declare_ApaSesSmpFixSole_OnFixHeartbeat_Default                    \
   inline K_mf(void) OnFixHeartbeat (TMessageRef, bool) {}                 \
//---------------------------------------------------------------------------
#define Declare_ApaSesSmpFixSole_OnFixLogout_Default                       \
   inline K_mf(void) OnFixLogout (TMessageRef, bool) {}                    \
//---------------------------------------------------------------------------
#define Declare_ApaSesSmpFixSole_OnFixResendRequest_Default                \
   inline K_mf(void) OnFixResendRequest (TMessageRef, bool) {}             \
//---------------------------------------------------------------------------
#define Declare_ApaSesSmpFixSole_OnFixSequenceReset_Default                \
   inline K_mf(void) OnFixSequenceReset (TMessageRef, bool) {}             \
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_ApaSesSmpFixSole_h_)
