#ifndef FixAdminMsgH
#define FixAdminMsgH
/////////////////////////////////////////////////////////////////////////////
// Heartbeat / Test Request
#define fMsgType_Heartbeat_s        "0"
#define fMsgType_TestRequest_s      "1"
#define fTag_TestReqID              112

// Resend Request
#define fMsgType_ResendRequest_s    "2"
#define fTag_BeginSeqNo               7
#define fTag_EndSeqNo                16

// Sequence Reset
#define fMsgType_SequenceReset_s    "4"
#define fTag_GapFillFlag            123
#define fTag_NewSeqNo                36

// Session Reject
#define fMsgType_Reject_s           "3"
#define fTag_RefSeqNum               45
#define fTag_RefTagID               371
#define fTag_RefMsgType             372
#define fTag_SessionRejectReason    373
#define fTag_Text                    58

// Logon
#define fMsgType_Logon_s            "A"
#define fTag_EncryptMethod           98
#define fTag_HeartBtInt             108
#define fTag_RawDataLength           95
#define fTag_RawData                 96
#define fTag_ResetSeqNumFlag        141

// Logout
#define fMsgType_Logout_s           "5"
/////////////////////////////////////////////////////////////////////////////
#endif
