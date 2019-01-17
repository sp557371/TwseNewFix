#ifndef FFixEvHandH
#define FFixEvHandH
//---------------------------------------------------------------------------
#include "ev_handler.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Fix  {
namespace Stk  {
//---------------------------------------------------------------------------
K_class TStkSesFix;
K_class TFixMsg;
/////////////////////////////////////////////////////////////////////////////
enum TFixEvMask {
    // session level
    fem_Logon,      // Logon
    fem_Heartbeat,  // Heart beat
    fem_TestReq,    // Test Request
    fem_ResendReq,  // Resend Request
    fem_SeqReset,   // Sequence Reset
    fem_SessReject, // Session Reject
    fem_Logout,     // Logout
    // applicition level
    fem_Order,      // Single Order Request
    fem_Cancel,     // Single Order Cancel Request
    fem_Replace,    // Single Order Cancel/Replace Request
    fem_MOrder,     // Multileg Order - Request
    fem_MReplace,   // Multileg Order Cancel/Replace Request
    fem_MCancel,    // MultiLeg Order Cancel Request
    fem_SecuDef,    // Security Definition Request
    fem_Quote,      // Quote
    fem_End
};
//---------------------------------------------------------------------------
K_class TFixEv : public Kstl::ev_handler_base<0>
{
private:
  virtual K_mf(void) DoEvent(const TFixMsg&) = 0;//NULL;
public:
          K_mf(void) operator()(const TFixMsg& msg) { DoEvent(msg); }
};
//---------------------------------------------------------------------------
K_class TFixMsgEv : public Kstl::ev_handler_base<fem_End>
{
private:
  // session level
  virtual K_mf(void) OnLogon        (TStkSesFix*, const TFixMsg&) { ev_mask_.reset(fem_Logon);         }
  virtual K_mf(void) OnHeartbeat    (TStkSesFix*, const TFixMsg&) { ev_mask_.reset(fem_Heartbeat);     }
  virtual K_mf(void) OnTestReq      (TStkSesFix*, const TFixMsg&) { ev_mask_.reset(fem_TestReq);       }
  virtual K_mf(void) OnResendReq    (TStkSesFix*, const TFixMsg&) { ev_mask_.reset(fem_ResendReq);     }
  virtual K_mf(void) OnSeqReset     (TStkSesFix*, const TFixMsg&) { ev_mask_.reset(fem_SeqReset);      }
  virtual K_mf(void) OnSessReject   (TStkSesFix*, const TFixMsg&) { ev_mask_.reset(fem_SessReject);    }
  virtual K_mf(void) OnLogout       (TStkSesFix*, const TFixMsg&) { ev_mask_.reset(fem_Logout);        }
  // appliction level
  virtual K_mf(void) OnOrder        (TStkSesFix*, const TFixMsg&) { ev_mask_.reset(fem_Order);         }
  virtual K_mf(void) OnCancel       (TStkSesFix*, const TFixMsg&) { ev_mask_.reset(fem_Cancel);        }
  virtual K_mf(void) OnReplace      (TStkSesFix*, const TFixMsg&) { ev_mask_.reset(fem_Replace);       }
  virtual K_mf(void) OnMOrder       (TStkSesFix*, const TFixMsg&) { ev_mask_.reset(fem_MOrder);        }
  virtual K_mf(void) OnMCancel      (TStkSesFix*, const TFixMsg&) { ev_mask_.reset(fem_MCancel);       }
  virtual K_mf(void) OnMReplace     (TStkSesFix*, const TFixMsg&) { ev_mask_.reset(fem_MReplace);      }
  virtual K_mf(void) OnSecuDef      (TStkSesFix*, const TFixMsg&) { ev_mask_.reset(fem_SecuDef);       }
  virtual K_mf(void) OnQuote        (TStkSesFix*, const TFixMsg&) { ev_mask_.reset(fem_Quote);         }
public:
    K_mf(void) operator()(TStkSesFix* ses, const TFixMsg& msg, TFixEvMask evm)
    {
      if(!is_need_ev(evm))
        return;
      switch(evm)
      {
        // session level
        case fem_Logon:      	OnLogon    	(ses, msg);     break;  // Logon
        case fem_Heartbeat:  	OnHeartbeat	(ses, msg);     break;  // Heart beat
        case fem_TestReq:    	OnTestReq  	(ses, msg);     break;  // Test Request
        case fem_ResendReq:  	OnResendReq	(ses, msg);     break;  // Resend Request
        case fem_SeqReset:   	OnSeqReset 	(ses, msg);     break;  // Sequence Reset
        case fem_SessReject: 	OnSessReject(ses, msg);	    break;  // Session Reject
        case fem_Logout:     	OnLogout   	(ses, msg);     break;  // Logout
        // appliction level
        case fem_Order:      	OnOrder    	(ses, msg);     break;  // Single Order
        case fem_Cancel:     	OnCancel   	(ses, msg);     break;  // Single Order Cancel Request
        case fem_Replace:    	OnReplace  	(ses, msg);     break;  // Single Order Cancel/Replace Request
        case fem_MOrder:      	OnMOrder   	(ses, msg);     break;  // Multileg Order
        case fem_MCancel:     	OnMCancel  	(ses, msg);     break;  // Multileg Order Cancel Request
        case fem_MReplace:    	OnMReplace 	(ses, msg);     break;  // Multileg Order Cancel/Replace Request
        case fem_SecuDef:    	OnSecuDef  	(ses, msg);     break;  // Security Definition Request
        case fem_Quote:    	    OnQuote  	(ses, msg);     break;  // Quote
      }
    }
};
//-------------------------------------------------------
}; // Stk
}; // Fix
}; // Tw
}; // Kway
#endif
