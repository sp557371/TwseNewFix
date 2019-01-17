/////////////////////////////////////////////////////////////////////////////
#if !defined(_ApaSesSmpFixMult_h_)
#define _ApaSesSmpFixMult_h_
//---------------------------------------------------------------------------
#include "ApaSesSmpFixUtil.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace FIX;

/////////////////////////////////////////////////////////////////////////////
// class TApaSesSmpFixMult
/////////////////////////////////////////////////////////////////////////////
K_class TApaSesSmpFixMult : public TApaSesSmpFix
{
   typedef TApaSesSmpFix  inherited;

protected:
   bool  EnableRecover_;

public:
   K_ctor  TApaSesSmpFixMult ();
   K_dtor ~TApaSesSmpFixMult ();

   //==============================================
   // overwrite TApaSesSmpFix virtual function
   K_mf(void) DispFixMessage (SDispFixMsgInfo&);

protected:
   //==============================================
   // overwrite TSesBase virtual function
   K_mf(void) OnMesLinkBroken (TMesBase*, const std::string&);

   //==============================================
   // overwrite TApaSesSmp virtual function
   K_mf(void) OnSmpGetMiddlewareInfoResponse    (TSmpMsgGetMiddlewareInfoResponse*);
   K_mf(void) OnSmpMwEv_SesMsgRecv              (TSmpMsgRef);
   K_mf(void) OnSmpMwEv_AppMsgRecv              (TSmpMsgMiddlewareEvent*);
   K_mf(void) OnSmpMwEv_IncomingMessageRestored (TSmpMsgMiddlewareEvent*);
   K_mf(void) OnSmpMwEv_EndOfRestore            (TSmpMsgMiddlewareEvent*);

   //==============================================
   // TApaSesSmpFixMult virtual function
   // ==> (const TSmpString& sesID, TMessageRef msgRef, bool isRt)
   virtual K_mf(void) OnFixLogon                     (const TSmpString&, TMessageRef, bool isRt, TSmpMsgRef) = 0;
   virtual K_mf(void) OnFixLogout                    (const TSmpString&, TMessageRef, bool isRt) = 0;
   virtual K_mf(void) OnFixHeartbeat                 (const TSmpString&, TMessageRef, bool isRt) = 0;
   virtual K_mf(void) OnFixResendRequest             (const TSmpString&, TMessageRef, bool isRt) = 0;
   virtual K_mf(void) OnFixSequenceReset             (const TSmpString&, TMessageRef, bool isRt) = 0;
   //----------------------------------------------
   virtual K_mf(void) OnFixNewOrderSingle            (const TSmpString&, TMessageRef, bool isRt) = 0;
   virtual K_mf(void) OnFixOrderCancelRequest        (const TSmpString&, TMessageRef, bool isRt) = 0;
   virtual K_mf(void) OnFixOrderCancelReplaceRequest (const TSmpString&, TMessageRef, bool isRt) = 0;
   virtual K_mf(void) OnFixOrderStatusRequest        (const TSmpString&, TMessageRef, bool isRt) = 0;
   virtual K_mf(void) OnFixAppOther                  (const TSmpString&, TMessageRef, bool isRt) = 0;
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////////////////
// TApaSesSmpFixMult virtual function default
/////////////////////////////////////////////////////////////////////////////////////////
#define Declare_ApaSesSmpFixMult_OnFixLogon_Default                                          \
   inline K_mf(void) OnFixLogon (const TSmpString&, TMessageRef, bool, TSmpMsgRef) {}        \
//---------------------------------------------------------------------------------------------
#define Declare_ApaSesSmpFixMult_OnFixLogout_Default                                         \
   inline K_mf(void) OnFixLogout (const TSmpString&, TMessageRef, bool) {}                   \
//---------------------------------------------------------------------------------------------
#define Declare_ApaSesSmpFixMult_OnFixHeartbeat_Default                                      \
   inline K_mf(void) OnFixHeartbeat (const TSmpString&, TMessageRef, bool) {}                \
//---------------------------------------------------------------------------------------------
#define Declare_ApaSesSmpFixMult_OnFixResendRequest_Default                                  \
   inline K_mf(void) OnFixResendRequest (const TSmpString&, TMessageRef, bool) {}            \
//---------------------------------------------------------------------------------------------
#define Declare_ApaSesSmpFixMult_OnFixSequenceReset_Default                                  \
   inline K_mf(void) OnFixSequenceReset (const TSmpString&, TMessageRef, bool) {}            \
//---------------------------------------------------------------------------------------------
#define Declare_ApaSesSmpFixMult_OnFixAdminOther_Default                                     \
   inline K_mf(void) OnFixAdminOther (const TSmpString&, TMessageRef, bool) {}               \
//---------------------------------------------------------------------------------------------
#define Declare_ApaSesSmpFixMult_OnFixNewOrderSingle_Default                                 \
   inline K_mf(void) OnFixNewOrderSingle (const TSmpString&, TMessageRef, bool) {}           \
//---------------------------------------------------------------------------------------------
#define Declare_ApaSesSmpFixMult_OnFixOrderCancelRequest_Default                             \
   inline K_mf(void) OnFixOrderCancelRequest (const TSmpString&, TMessageRef, bool) {}       \
//---------------------------------------------------------------------------------------------
#define Declare_ApaSesSmpFixMult_OnFixOrderCancelReplaceRequest_Default                      \
   inline K_mf(void) OnFixOrderCancelReplaceRequest (const TSmpString&, TMessageRef, bool) {}\
//---------------------------------------------------------------------------------------------
#define Declare_ApaSesSmpFixMult_OnFixOrderStatusRequest_Default                             \
   inline K_mf(void) OnFixOrderStatusRequest (const TSmpString&, TMessageRef, bool) {}       \
//---------------------------------------------------------------------------------------------
#define Declare_ApaSesSmpFixMult_OnFixAppOther_Default                                       \
   inline K_mf(void) OnFixAppOther (const TSmpString&, TMessageRef, bool) {}                 \
//---------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_ApaSesSmpFixMult_h_)
