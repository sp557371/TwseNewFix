/////////////////////////////////////////////////////////////////////////////
#if !defined(_ApaSmpType_h_)
#define _ApaSmpType_h_
//---------------------------------------------------------------------------
#include <vector>
#include "kutility.h"
#include "fixed_num.h"
#include "KTime.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;

/////////////////////////////////////////////////////////////////////////////
// base type
/////////////////////////////////////////////////////////////////////////////
typedef std::string                     TSmpString;
typedef std::vector<TSmpString>         TSmpStringList;
typedef TSmpStringList::iterator        TSmpStringListI;
typedef TSmpStringList::const_iterator  TSmpStringListCi;

/////////////////////////////////////////////////////////////////////////////
// class TSmpBool
/////////////////////////////////////////////////////////////////////////////
K_class TSmpBool
{
   bool  Data_;

public:
   inline K_ctor TSmpBool () : Data_(false) {}

   K_mf(void)        assign    (const char*);
   K_mf(std::string) as_string () const;

   inline K_mf(void) assign  (const std::string& str)  { assign(str.c_str());    }
   inline K_mf(void) assign  (bool b)                  { Data_ = b;              }
   
   inline K_mf(bool) is_true () const  { return (Data_ == true); }
   inline K_mf(int)  as_int  () const  { return (Data_ ? 1 : 0); }
};

/////////////////////////////////////////////////////////////////////////////
// class TSmpInt
/////////////////////////////////////////////////////////////////////////////
class TSmpInt : public fixed_num<int>
{
   typedef fixed_num<int>  inherited;
   
public:
   using inherited::assign;

public:
   inline K_ctor TSmpInt (int n = 0)        : inherited(n)             {}
   inline K_ctor TSmpInt (const TSmpInt& a) : inherited(a.get_orig())  {}

   inline K_mf(std::string) as_string () const
      {
         if (inherited::is_0())
            return std::string("0");
         return inherited::as_string();
      }
};

/////////////////////////////////////////////////////////////////////////////
// class TSmpTime
/////////////////////////////////////////////////////////////////////////////
class TSmpTime : public TKDateTimeMS
{
   typedef TKDateTimeMS  inherited;

public:
   K_mf(void) assign_apafmt (const std::string&);
};

/////////////////////////////////////////////////////////////////////////////
// class TSmpEventType
/////////////////////////////////////////////////////////////////////////////
enum ESmpEventType
{
   set_Unknown,
      // Middleware connection closed.
      //  MIDDLEWARE_CLOSED            = -2,
      //  APPIA_SESSION_CLOSED         = MIDDLEWARE_CLOSED,
      //  MIDDLEWARE_OPENED            = -3,

      //===========================================
      // Outbound Acknowledgment Events
      set_MessageValidated          = 23010,
      set_MessageSent               = 23020,
      set_MessageSendConnectionDown = 23021,
      set_MessageCommitted          = 23030,

      //===========================================
      // Outbound Error Events
      set_MessageValidationError = 23011,
         // sample1
         //    SOCKET2.00000000416e230111100FIX_4280CODE: 0-10-0 INVALID TAG NUMBER: Invalid tag number. tag: <SOH><SOH><0x0a>
         //    CODE: 0-2-35 REQUIRED TAG MISSING: Required Field MsgType tag#: 35 not found in message type HeaderB<0x0a>
         //    CODE: 0-2-37 REQUIRED TAG MISSING: Required Field OrderID tag#: 37 not found in message type ExecutionReport1=98062416=011=1000114=017=283876520=038=100039=040=244=10054=155=110158=New::New150=0151=1000

      //  MESSAGE_SEND_UNKNOWN_TARGET  = 23022,
      
      //  MESSAGE_COMMIT_FAILURE       = 23031,
      //  OUTBOUND_MESSAGE_ERROR       = 23032,
      //  MESSAGE_EXPIRED              = 23034,

      //===========================================
      // Inbound message events
      set_SessionMessageReceived      = 23040,
      set_ApplicationeMessageReceived = 23041,

      set_IncomingMessageRestored     = 23050,
      set_EndOfRestore                = 23051,

      //  INBOUND_MESSAGE_ERROR        = 23060,

      //===========================================
      // Session Level Events
      set_SessionConnected           = 23210,
      set_SessionDisconnected        = 23211,
      set_SessionEndOfDay            = 23212,
      set_SessionConnectFailure      = 23213,
      set_SessionConnectTimeout      = 23214,
      set_SessionHold                = 23220,
      set_SessionRelease             = 23221,
      set_SessionSuspend             = 23222,
      set_SessionResume              = 23223,
      set_SessionServiceUp           = 23224,
      set_SessionServiceDown         = 23225,
      set_SessionChannelDisabled     = 23226,
      set_SessionChannelReady        = 23227,
      set_SessionChannelNotReady     = 23228,
      set_SessionStartEndOfDay       = 23230,
      set_SessionHaPrimaryStaleError = 23231,
      set_SessionCreated             = 23302,
      set_SessionDeleted             = 23304,
      set_NewSessionAdded            = set_SessionCreated,

      //===========================================
      // Appia server level events
      //  GLOBAL_MESSAGE               = 23303,
      set_EngineShutdown               = 23333,
      //  ENGINE_BEGIN_RELOAD_CONFIG   = 23430,
      //  ENGINE_END_RELOAD_CONFIG     = 23431,
      //  ENGINE_BECAME_PRIMARY        = 23440,
      //  ENGINE_BECAME_BACKUP         = 23441,
      //  RECOVERED_FROM_LOGFILE       = 23450,

      // Programming errors.
      //  MESSAGE_FORMAT_ERROR         = 23510,
      //  MIDDLEWARE_MESSAGE_FORMAT_ERROR = MESSAGE_FORMAT_ERROR,
      //  UNSUPPORTED                  = 23520
};
//---------------------------------------------------------------------------
K_class TSmpEventType : public EnumT<ESmpEventType>
{
   typedef EnumT<ESmpEventType>  inherited;

public:
   inline K_ctor TSmpEventType (ESmpEventType e = set_Unknown) : inherited(e)  {}

   K_mf(void) assign (const char*);
   
   K_mf(bool)        IsSessionLevelEvents           ()            const;
   K_mf(bool)        IsOutboundAcknowledgmentEvents ()            const;
   K_mf(std::string) as_string                      (bool incNum) const;
      
   inline K_mf(void) clear  ()                        { Data_ = set_Unknown; }
   inline K_mf(void) assign (const std::string& str)  { assign(str.c_str()); }
      
   inline K_mf(ESmpEventType) get_orig   () const  { return Data_;                }
   inline K_mf(bool)          is_invalid () const  { return Data_ == set_Unknown; }
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpCmd
/////////////////////////////////////////////////////////////////////////////
enum ESmpCmd
{
   esc_Unknown,
      // Param Size 0
      esc_Version,

      // Param Size 1
      esc_Stats,
      esc_Connect,
      esc_Disconnect,
      esc_ServiceDown,
      esc_ServiceUp,
      esc_EndOfDay,
      esc_Suspend,
      esc_Resume,
      esc_Hold,
      esc_Release,
      esc_RemoveMiddleware,

      // Param Size 2
      esc_SequenceReset,
      esc_OutMsgSeqNum,             // <session> <new seq no>
      esc_SuspendWithTerminate,     // <session> <text>

   esc_Total,

   esc_Param0 = esc_Version,
   esc_Param1 = esc_Stats,
   esc_Param2 = esc_SequenceReset,
};
//---------------------------------------------------------------------------
K_class TSmpCmd : public EnumT<ESmpCmd>
{
   typedef EnumT<ESmpCmd>  inherited;

public:
   inline K_ctor TSmpCmd (ESmpCmd e = esc_Unknown) : inherited(e)  {}

   K_mf(void) assign (const char*);
   
   K_mf(std::string) as_string () const;
      
   inline K_mf(void) clear  ()                        { Data_ = esc_Unknown; }
   inline K_mf(void) assign (const std::string& str)  { assign(str.c_str()); }
      
   inline K_mf(ESmpCmd) get_orig   () const  { return Data_;                }
   inline K_mf(bool)    is_invalid () const  { return Data_ == esc_Unknown; }
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpOperCmd
/////////////////////////////////////////////////////////////////////////////
K_class TSmpOperCmd
{
   TSmpCmd     SmpCmd_;
   TSmpString  Param1_;
   TSmpString  Param2_;

public:
   inline K_ctor TSmpOperCmd (ESmpCmd e = esc_Unknown)                               : SmpCmd_(e)                           {}
   inline K_ctor TSmpOperCmd (ESmpCmd e, const TSmpString& p1)                       : SmpCmd_(e), Param1_(p1)              {}
   inline K_ctor TSmpOperCmd (ESmpCmd e, const TSmpString& p1, const TSmpString& p2) : SmpCmd_(e), Param1_(p1), Param2_(p2) {}

   K_mf(TSmpString) Pack   () const;
   K_mf(bool)       Unpack (const TSmpString&);

   inline K_mf(void) SetSmpCmd (ESmpCmd e)            { SmpCmd_.assignT(e); }
   inline K_mf(void) SetParam1 (const TSmpString& p)  { Param1_ = p;        }
   inline K_mf(void) SetParam2 (const TSmpString& p)  { Param2_ = p;        }

   inline K_mf(const TSmpCmd&)    GetSmpCmd () const  { return SmpCmd_; }
   inline K_mf(const TSmpString&) GetParam1 () const  { return Param1_; }
   inline K_mf(const TSmpString&) GetParam2 () const  { return Param2_; }
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpOperCmdResu
/////////////////////////////////////////////////////////////////////////////
enum ESmpOperCmdResu
{
   socr_Success,
      socr_UnknownSessioID,
};
//---------------------------------------------------------------------------
class TSmpOperCmdResu : public EnumT<ESmpOperCmdResu>
{
   typedef EnumT<ESmpOperCmdResu>  inherited;

public:
   inline K_ctor TSmpOperCmdResu (ESmpOperCmdResu e = socr_Success) : inherited(e)  {}
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpConnectState  Page.58
/////////////////////////////////////////////////////////////////////////////
enum ESmpConnectState
{
   scs_Unknown,
      scs_Disconnected            = '0',
      scs_SocketConnection        = '1',
      scs_SentLogonConnection     = '2',
      scs_ReceivedLogonConnection = '3',
      scs_ApplicationConnection   = '4',
};
//---------------------------------------------------------------------------
K_class TSmpConnectState : public EnumT<ESmpConnectState>
{
   typedef EnumT<ESmpConnectState>  inherited;

public:
   inline K_ctor TSmpConnectState (ESmpConnectState e = scs_Unknown) : inherited(e)  {}
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpRejectReason
/////////////////////////////////////////////////////////////////////////////
enum ESmpRejectReason
{
   srr_Unknown,
      srr_NotLogon              = '0', // 尚未登入
      srr_AlreadyLogon          = '1', // 此 MiddlewareInterface 已經登入了
      srr_SesInfo_SesIDNotFound = '2', // 在查詢 SessionInfo 時輸入錯誤的 SessionID
};
//---------------------------------------------------------------------------
K_class TSmpRejectReason : public EnumT<ESmpRejectReason>
{
   typedef EnumT<ESmpRejectReason>  inherited;

public:
   inline K_ctor TSmpRejectReason (ESmpRejectReason e = srr_Unknown) : inherited(e)  {}
      
   inline K_mf(ESmpRejectReason) get_orig () const  { return inherited::Data_; }
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpInboundQueueInfo  Page.63
/////////////////////////////////////////////////////////////////////////////
struct SSmpInboundQueueInfo
{
   TSmpString  SessionID_;
   TSmpInt     NewSeqNum_;
   TSmpInt     QueueSize_;
};
typedef std::vector<SSmpInboundQueueInfo>      SSmpInboundQueueInfoA;
typedef SSmpInboundQueueInfoA::const_iterator  SSmpInboundQueueInfoACi;
//---------------------------------------------------------------------------
K_class TSmpInboundQueueInfo
{
   SSmpInboundQueueInfoA  Data_;

public:
   K_mf(const char*) Parse (const char*, int num);

   inline K_mf(const SSmpInboundQueueInfoA&) GetData () const  { return Data_; }
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpMiddlewareConfig  Page.39
/////////////////////////////////////////////////////////////////////////////
struct SSmpMiddlewareConfig
{
   TSmpInt         InMsgFormat_;
   TSmpBool        AppicationMessages_;
   TSmpBool        SessionMessages_;
   TSmpBool        MessageValidatedEvents_;
   TSmpBool        MessageCommitEvents_;
   TSmpBool        MessageSentEvents_;
   TSmpBool        SessionEvents_;
   TSmpBool        GlobalEvents_;
   TSmpStringList  CheckRemote_;
   TSmpStringList  MessageTypes_;
   TSmpStringList  SessionIDs_;
};
//---------------------------------------------------------------------------
K_class TSmpMiddlewareConfig
{
   SSmpMiddlewareConfig  Data_;

public:
   K_mf(const char*) Parse (const char*, bool hasEndOfMsg);
   
   inline K_mf(const SSmpMiddlewareConfig&) GetData () const  { return Data_; }
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpLastMsgClientIDs
/////////////////////////////////////////////////////////////////////////////
struct SSmpLastMsgClientID
{
   TSmpString  SessionID_;
   TSmpString  ClientMsgID_;
};
typedef std::vector<SSmpLastMsgClientID>      TSmpLastMsgClientIDA;
typedef TSmpLastMsgClientIDA::const_iterator  TSmpLastMsgClientIDACi;
//---------------------------------------------------------------------------
K_class TSmpLastMsgClientIDs
{
   TSmpLastMsgClientIDA  Data_;

public:
   K_mf(const char*) Parse (const char*);
   
   inline K_mf(const TSmpLastMsgClientIDA&) GetData () const  { return Data_; }
};


/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_ApaSmpType_h_)
