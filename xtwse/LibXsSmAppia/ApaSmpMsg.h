/////////////////////////////////////////////////////////////////////////////
#if !defined(_ApaSmpMsg_h_)
#define _ApaSmpMsg_h_
//---------------------------------------------------------------------------
#include "wise_ptr.h"
#include "ApaSmpType.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;

/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgType
/////////////////////////////////////////////////////////////////////////////
enum ESmpMsgType
{
   smt_Unknown = 0x00,

      // 'C', OperatorCommand request, sent by the client application
      smt_OperatorCommandRequest = 'C',
      
      // 'c', Appia response to the OperatorCommand request
      smt_OperatorCommandResponse = 'c',
      
      // 'e', Appia MiddlewareEvent, sent in response to Outbound Message delivery and
      //      RestoreMessage requests from the client application, as well as configured message event subscriptions
      // Page.7
      smt_MiddlewareEvent = 'e',
                                    
      // 'N', SMP Logon message request, sent by the client application
      smt_Logon = 'N',

      // 'O', Outbound message delivery request, sent by the client application
      smt_OutboundMessage = 'O',

      // 'U', ShutdownMiddleware request, sent by the client application
      smt_ShutdownMiddleware = 'U',

      // 'W', StartMiddlwareEvents request, sent by the client application, Page.22
      smt_StartMiddlwareEvents = 'W',

      // 'X', StopMiddlwareEvents request, sent by the client application
      smt_StopMiddlwareEvents = 'X',
      
      // 'I', GetSessionInfo request, sent by the client application
      smt_GetSessionInfoRequest = 'I',
      
      // 'i', Appia response to the GetSessionInfo request
      smt_GetSessionInfoResponse = 'i',
      
      // 'K', GetMiddlewareInfo request, sent by the client application, p62
      smt_GetMiddlewareInfoRequest = 'K',
      
      // 'k', Appia response to the GetMiddlewareInfo request, Page.63
      smt_GetMiddlewareInfoResponse = 'k',
      
      // 'L', GetSessionList request, sent by the client application, Page.56
      smt_GetSessionListRequest = 'L',
      
      // 'l', Appia response to the GetSessionList request, Page.56
      smt_GetSessionListResponse = 'l',
   
      // 'G', GetMiddlewareConfig request, sent by the client application, Page.39
      smt_GetMiddlewareConfigRequest = 'G',
      
      // 'g', Appia response to the GetMiddlewareConfig request, Page.39
      smt_GetMiddlewareConfigResponse = 'g',
      
      // 'R', RestoreMessage request, sent by the client application, Page.53
      smt_RestoreMessageRequest = 'R',
      
      // 'r', Appia Server Reject message, in response to an unknown or unexpected request
      //         (such as an operator command received before a Logon message)
      smt_Reject = 'r',
      
      // 'M', setMiddlewareConfig request, sent by the client application
      smt_SetMiddlewareConfigRequest = 'M',
      
   smt_Total
};
//---------------------------------------------------------------------------
K_class TSmpMsgType
{
   ESmpMsgType  Data_;

public:
   inline K_ctor TSmpMsgType (ESmpMsgType e = smt_Unknown) : Data_(e)  {}

   K_mf(void) assign      (const char*);
   K_mf(void) assign_char (char);

   K_mf(char)        as_char   () const;
   K_mf(std::string) as_string () const;

   inline K_mf(void) assign  (const std::string& str)  { assign(str.c_str()); }
   inline K_mf(void) assignT (ESmpMsgType e)           { Data_ = e;           }
   
   inline K_mf(bool)        is_valid () const  { return (Data_ != smt_Unknown); }
   inline K_mf(ESmpMsgType) get_orig () const  { return Data_;                  }

   inline operator ESmpMsgType () const  { return Data_; }
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsg
/////////////////////////////////////////////////////////////////////////////
K_class TSmpMsg : public Kstl::ref_counter<>
{
   size_t       MsgDataLen_;
   TSmpMsgType  MsgType_;

public:
   typedef Kstl::wise_ref<TSmpMsg>  TSmpMsgRef;

public:
   inline K_ctor TSmpMsg ()                 : MsgDataLen_(0),             MsgType_(smt_Unknown)  {}
   inline K_ctor TSmpMsg (ESmpMsgType e)    : MsgDataLen_(0),             MsgType_(e)            {}
   inline K_ctor TSmpMsg (const TSmpMsg& a) : MsgDataLen_(a.MsgDataLen_), MsgType_(a.MsgType_)   {}

   K_mf(int)        Pack   (std::string&);
   K_mf(TSmpMsgRef) Unpack (const char*);

   K_mf(int)  GetMsgData (std::string&) const;
   K_mf(bool) SetMsgData (const std::string&);

   inline K_mf(const TSmpMsgType&) GetMsgType () const  { return MsgType_; }

public:
   static const char   VersionInfo[];
   static const char   Delimiter;
   static const char   EndOfMsg;
   static const size_t HeaderLength;

protected:
   //==============================================
   // TSmpMsg virtual function
   virtual K_mf(int)  OnGetMsgData (std::string&) const  { return 0;    }
   virtual K_mf(bool) OnSetMsgData (const std::string&)  { return true; }
};
//---------------------------------------------------------------------------
typedef TSmpMsg::TSmpMsgRef  TSmpMsgRef;


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgLogon
/////////////////////////////////////////////////////////////////////////////
class TSmpMsgLogon : public TSmpMsg
{
   typedef TSmpMsg  inherited;

   std::string  MiddlewareID_;
   int          ReliableMode_;

public:
   inline K_ctor TSmpMsgLogon (const std::string& strMiddlewareID, int nReliableMode)
      : inherited    (smt_Logon)
      , MiddlewareID_(strMiddlewareID)
      , ReliableMode_(nReliableMode)
   {}

protected:
   // overwrite TSmpMsgData virtual function
   K_mf(int) OnGetMsgData (std::string&) const;
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgReject
/////////////////////////////////////////////////////////////////////////////
struct SSmpMsgReject
{
   TSmpString        RejMsg_;
   TSmpRejectReason  RejReason_;
   TSmpString        RejParam1_;
};
//---------------------------------------------------------------------------
class TSmpMsgReject : public TSmpMsg
{
   typedef TSmpMsg  inherited;

   SSmpMsgReject  Data_;

public:
   inline K_ctor TSmpMsgReject (const TSmpMsg& a) : inherited(a)  {}

   inline K_mf(const SSmpMsgReject&) GetData () const  { return Data_; }

protected:
   //==============================================
   // overwrite TSmpMsg virtual function
   K_mf(bool) OnSetMsgData (const std::string&);
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgOperatorCommandRequest
/////////////////////////////////////////////////////////////////////////////
class TSmpMsgOperatorCommandRequest : public TSmpMsg
{
   typedef TSmpMsg  inherited;

   TSmpString  Command_;

public:
   inline K_ctor TSmpMsgOperatorCommandRequest (const TSmpString& strCmd)
      : inherited(smt_OperatorCommandRequest)
      , Command_ (strCmd)
   {}

protected:
   //==============================================
   // overwrite TSmpMsg virtual function
   K_mf(int) OnGetMsgData (std::string&) const;
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgOperatorCommandResponse
/////////////////////////////////////////////////////////////////////////////
struct SSmpMsgOperatorCommandResponse
{
   TSmpString       FullCommand_;
   TSmpString       Status_;
   TSmpOperCmd      SmpOperCmd_;
   TSmpOperCmdResu  OperCmdResu_;
};
//---------------------------------------------------------------------------
class TSmpMsgOperatorCommandResponse : public TSmpMsg
{
   typedef TSmpMsg  inherited;

   SSmpMsgOperatorCommandResponse  Data_;

public:
   inline K_ctor TSmpMsgOperatorCommandResponse (const TSmpMsg& a) : inherited(a)  {}

   inline K_mf(const SSmpMsgOperatorCommandResponse&) GetData () const  { return Data_; }

protected:
   //==============================================
   // overwrite TSmpMsg virtual function
   K_mf(bool) OnSetMsgData (const std::string& str);
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgMiddlewareEvent
/////////////////////////////////////////////////////////////////////////////
struct SSmpMsgMiddlewareEvent
{
   TSmpEventType  EventType_;
   TSmpString     ClientMsgID_;
   TSmpString     SessionID_;
   TSmpString     Protocol_;
   TSmpString     MessageType_;
   TSmpInt        SequenceNumber_;
   TSmpString     MetaData_;
   TSmpString     EventData_;
   // 延伸的資料
   TSmpBool       IsPossResend_;
   TSmpString     RemoteIP_;
   TSmpString     RemotePort_;

   K_mf(void) Parse_PossResend ();
   K_mf(void) Parse_MsgType_A  ();
};
//---------------------------------------------------------------------------
class TSmpMsgMiddlewareEvent : public TSmpMsg
{
   typedef TSmpMsg  inherited;

   SSmpMsgMiddlewareEvent  Data_;

public:
   inline K_ctor TSmpMsgMiddlewareEvent (const TSmpMsg& a) : inherited(a)  {}

   inline K_mf(const SSmpMsgMiddlewareEvent&) GetData () const  { return Data_; }

protected:
   //==============================================
   // overwrite TSmpMsg virtual function
   K_mf(bool) OnSetMsgData (const std::string& str);
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgGetSessionInfoResponse
/////////////////////////////////////////////////////////////////////////////
struct SSmpMsgGetSessionInfoResponse
{
   TSmpString        SessionID_;
   TSmpString        RemoteFirmID_;
   TSmpString        LocalFirmID_;
   TSmpString        LocalTraderID_;
   TSmpString        Group_;
   TSmpString        Description_;
   TSmpString        Contact_;
   TSmpString        Protocol_;
   TSmpString        ProtocolVersion_;
   TSmpString        Version_;
   TSmpString        NetAddresses_;
   TSmpString        Port_;
   TSmpString        EncryptionType_;
   TSmpString        HeartbeatInterval_;
   TSmpConnectState  ConnectState_;
   TSmpString        LastEODTime_;
   TSmpBool          HoldFlag_;
   TSmpString        HoldRejReason_;
   TSmpBool          ServiceDownFlag_;
   TSmpString        ServiceDownRejReason_;
   TSmpBool          SuspendFlag_;
   TSmpString        SuspendRejReason_;
   TSmpString        ResendActiveFlag_;
   TSmpString        NormalLogoutFlag_;
   TSmpString        InMsgCount_;
   TSmpString        OutMsgCount_;
   TSmpString        InByteCountNumber_;
   TSmpString        OutByteCount_;
   TSmpTime          LastInMsgTime_;
   TSmpTime          LastOutMsgTime_;
   TSmpString        LastOutMsgSeqNum_;
   TSmpInt           InMsgSeqNum_;
   TSmpInt           OutMsgSeqNum_;
   TSmpString        AllocationCount_;
   TSmpString        CancelCount_;
   TSmpString        CorrectCount_;
   TSmpString        ExecutionCount_;
   TSmpString        ExecutionAckCount_;
   TSmpString        HeartbeatCount_;
   TSmpString        IOICount_;
   TSmpString        LogonCount_;
   TSmpString        OrderCount_;
   TSmpString        RejectCount_;
};
//---------------------------------------------------------------------------
K_class TSmpMsgGetSessionInfoResponse : public TSmpMsg
{
   typedef TSmpMsg  inherited;
   
   SSmpMsgGetSessionInfoResponse  Data_;

public:
   inline K_ctor TSmpMsgGetSessionInfoResponse (const TSmpMsg& a) : inherited(a)  {}
      
   inline K_mf(const SSmpMsgGetSessionInfoResponse&) GetData () const  { return Data_; }

protected:
   //==============================================
   // overwrite TSmpMsg virtual function
   K_mf(bool) OnSetMsgData (const std::string& str);
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgGetMiddlewareInfoResponse
/////////////////////////////////////////////////////////////////////////////
struct SSmpMsgGetMiddlewareInfoResponse
{
   std::string           MiddlewareID_;
   std::string           MiddlewareType_;
   TSmpBool              ReliableModesStatus_;
   TSmpInt               NumberOfInboundQueueInfoBlocks_;
   TSmpInboundQueueInfo  InboundQueueInfo_;
   TSmpBool              ConnectionStatus_;
   TSmpMiddlewareConfig  MiddlewareConfig_;
   TSmpLastMsgClientIDs  LastMsgClientIDs_;
};
//---------------------------------------------------------------------------
K_class TSmpMsgGetMiddlewareInfoResponse : public TSmpMsg
{
   typedef TSmpMsg  inherited;

   SSmpMsgGetMiddlewareInfoResponse  Data_;

public:
   inline K_ctor TSmpMsgGetMiddlewareInfoResponse (const TSmpMsg& a) : inherited(a)  {}

   //==============================================
   // inline
   inline K_mf(const SSmpMsgGetMiddlewareInfoResponse&) GetData () const  { return Data_; }

protected:
   //==============================================
   // overwrite TSmpMsg virtual function
   K_mf(bool) OnSetMsgData (const std::string& str);
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgOutboundMessage
/////////////////////////////////////////////////////////////////////////////
enum ESmpMsgOmdFmt
{
   omd_RawData              = 0,
   omd_XML                  = 1,
   omd_AttributeValueObject = 7,
};
//---------------------------------------------------------------------------
struct SSmpMsgOutboundMessage
{
   TSmpString     ClientMsgID_;
   TSmpString     SessionID_;
   TSmpString     Protocol_;
   ESmpMsgOmdFmt  Format_;
   TSmpString     MessageType_;
   TSmpString     MessageData_;  // MessageData_ 結尾必須要有 <SOH> 喔

   inline K_ctor SSmpMsgOutboundMessage ()
      : Protocol_("FIX")
      , Format_  (omd_RawData)
   {}
};
//---------------------------------------------------------------------------
K_class TSmpMsgOutboundMessage : public TSmpMsg
{
   typedef TSmpMsg  inherited;

   SSmpMsgOutboundMessage  Data_;

public:
   inline K_ctor TSmpMsgOutboundMessage ()
      : inherited(smt_OutboundMessage)
   {}
   
   inline K_mf(SSmpMsgOutboundMessage&) GetData ()  { return Data_; }

protected:
   // overwrite TSmpMsgData virtual function
   K_mf(int) OnGetMsgData (std::string&) const;
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgStartMiddlwareEvents
/////////////////////////////////////////////////////////////////////////////
class TSmpMsgStartMiddlwareEvents : public TSmpMsg
{
   typedef TSmpMsg  inherited;

public:
   inline K_ctor TSmpMsgStartMiddlwareEvents () : inherited(smt_StartMiddlwareEvents)  {}
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgGetMiddlewareConfigRequest
/////////////////////////////////////////////////////////////////////////////
class TSmpMsgGetMiddlewareConfigRequest : public TSmpMsg
{
   typedef TSmpMsg  inherited;

public:
   inline K_ctor TSmpMsgGetMiddlewareConfigRequest () : inherited(smt_GetMiddlewareConfigRequest)  {}
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgGetSessionInfoRequest
/////////////////////////////////////////////////////////////////////////////
K_class TSmpMsgGetSessionInfoRequest : public TSmpMsg
{
   typedef TSmpMsg  inherited;

   TSmpString  SessionID_;

public:
   inline K_ctor TSmpMsgGetSessionInfoRequest (const TSmpString& sessionID)
      : inherited (smt_GetSessionInfoRequest)
      , SessionID_(sessionID)
   {}

protected:
   //==============================================
   // overwrite TSmpMsg virtual function
   K_mf(int) OnGetMsgData (std::string&) const;
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgGetMiddlewareInfoRequest
/////////////////////////////////////////////////////////////////////////////
K_class TSmpMsgGetMiddlewareInfoRequest : public TSmpMsg
{
   typedef TSmpMsg  inherited;

   std::string  MiddlewareID_;

public:
   inline K_ctor TSmpMsgGetMiddlewareInfoRequest (const std::string& middlewareID)
      : inherited    (smt_GetMiddlewareInfoRequest)
      , MiddlewareID_(middlewareID)
   {}

protected:
   //==============================================
   // overwrite TSmpMsg virtual function
   K_mf(int) OnGetMsgData (std::string&) const;
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgGetMiddlewareConfigResponse
/////////////////////////////////////////////////////////////////////////////
K_class TSmpMsgGetMiddlewareConfigResponse : public TSmpMsg
{
   typedef TSmpMsg  inherited;

   TSmpMiddlewareConfig  Data_;

public:
   inline K_ctor TSmpMsgGetMiddlewareConfigResponse (const TSmpMsg& a) : inherited(a)  {}

   //==============================================
   // inline
   inline K_mf(const SSmpMiddlewareConfig&) GetData () const  { return Data_.GetData(); }

protected:
   //==============================================
   // overwrite TSmpMsg virtual function
   K_mf(bool) OnSetMsgData (const std::string& str);
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgGetSessionListRequest
/////////////////////////////////////////////////////////////////////////////
class TSmpMsgGetSessionListRequest : public TSmpMsg
{
   typedef TSmpMsg  inherited;

public:
   inline K_ctor TSmpMsgGetSessionListRequest () : inherited(smt_GetSessionListRequest)  {}
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgGetSessionListResponse
/////////////////////////////////////////////////////////////////////////////
K_class TSmpMsgGetSessionListResponse : public TSmpMsg
{
   typedef TSmpMsg  inherited;

   TSmpStringList  SessionIDs_;

public:
   inline K_ctor TSmpMsgGetSessionListResponse (const TSmpMsg& a) : inherited(a)  {}

   //==============================================
   // inline
   inline K_mf(const TSmpStringList&) GetSessionIDs () const  { return SessionIDs_; }

protected:
   //==============================================
   // overwrite TSmpMsg virtual function
   K_mf(bool) OnSetMsgData (const std::string&);
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgRestoreMessageRequest
/////////////////////////////////////////////////////////////////////////////
K_class TSmpMsgRestoreMessageRequest : public TSmpMsg
{
   typedef TSmpMsg  inherited;

   TSmpString  SessionID_;
   TSmpInt     BeginSeqNo_;
   TSmpInt     EndSeqNo_;

public:
   inline K_ctor TSmpMsgRestoreMessageRequest (const std::string& sessionID, const TSmpInt& begSeqNo, const TSmpInt& endSeqNo)
      : inherited  (smt_RestoreMessageRequest)
      , SessionID_ (sessionID)
      , BeginSeqNo_(begSeqNo)
      , EndSeqNo_  (endSeqNo)
   {}

protected:
   //==============================================
   // overwrite TSmpMsg virtual function
   K_mf(int) OnGetMsgData (std::string&) const;
};


/////////////////////////////////////////////////////////////////////////////
// class TSmpSetMiddlewareConfigRequest
/////////////////////////////////////////////////////////////////////////////
K_class TSmpSetMiddlewareConfigRequest : public TSmpMsg
{
   typedef TSmpMsg  inherited;

   SSmpMiddlewareConfig  MwConf_;

public:
   inline K_ctor TSmpSetMiddlewareConfigRequest (const SSmpMiddlewareConfig& mwConf)
      : inherited(smt_SetMiddlewareConfigRequest)
      , MwConf_  (mwConf)
   {}

protected:
   //==============================================
   // overwrite TSmpMsg virtual function
   K_mf(int) OnGetMsgData (std::string&) const;
};


/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_ApaSmpMsg_h_)
