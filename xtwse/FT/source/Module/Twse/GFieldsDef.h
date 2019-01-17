//---------------------------------------------------------------------------
#ifndef GFieldsDefH
#define GFieldsDefH
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
namespace Kway {
//namespace Tw   {
//namespace Bx   {
//namespace FT   {
//---------------------------------------------------------------------------
//Common
#define  GMain         "Main"
#define  GAppiaMain    "MAIN"
//#define  GFIX42        "FIX_42_TWSE"
#define  GFIX42        "FIX_42"
//#define  GFIX43        "FIX_43_TWSE"
//#define  GFIX44        "FIX_44_TWSE"
#define  GFIX44        "FIX_44"
#define  GTwse         "XTAI"
#define  GOtc          "ROCO"

#define  GMainIni      "Main.ini"
#define  GTradeSegIni  "TradeSegment.ini"
#define  GAppiaIni     "twse.ini"
#define  GPassWordsIni "passwords.ini"
#define  GPWD_Update   "passwords_update"
#define  GAppiaSmpIni  "appia_smp.ini"

//XTester.ini
#define  GSummary      "Summary"
#define  GSesID        "SesID"
#define  GAppiaMWName "AppiaMWName"

//Passwords.ini
#define  GPASSWORDS    "PASSWORDS"

//Appia-Smp.ini
#define  GSMPSESSION   "SMP_SESSION"

//Main.ini - [Main]
#define  GAppiaHomePath "AppiaHomePath"
#define  GAppiaPath     "AppiaPath"
#define  GTesterPath    "TesterPath"
#define  GTmpPath       "TmpPath"
#define  GBroker        "Broker"
#define  GBeginTime     "BTime"
#define  GEndTime       "ETime"
#define  GWorkCode      "WorkCode"
#define  GFixSes        "FixSes"
#define  GAppiaMWPort   "appiamwport"
#define  GType          "Type"
#define  GMWBegPort     "mw_begin_port"
#define  GTermID        "TermID"
#define  GAppiaGroup    "AppiaGroup"
#define  GFWGroup       "FWGroup"
#define  GListPort1     "ListPort1"
#define  GListPort2     "ListPort2"
#define  GTotalFlow     "TotalFlow"

//[XFT]
#define  GXFT          "XFT"
#define  GNextDate     "NextDate"
#define  GTF05Ready    "TF05Ready"
#define  GOF05Ready    "OF05Ready"
#define  GTF05File     "TF05File"
#define  GOF05File     "OF05File"
#define  GMultiplier   "Multiplier"
#define  GMultiplierS  "Multipliers"
//#define  GTF05Path     "TF05Path"
//#define  GOF05Path     "OF05Path"
#define  GTF03Ready    "TF03Ready"
#define  GOF03Ready    "OF03Ready"
#define  GTF11Ready    "TF11Ready"
#define  GOF11Ready    "OF11Ready"
#define  GTF11Done     "TF11Done"
#define  GOF11Done     "OF11Done"
#define  GTF13Ready    "TF13Ready"
#define  GOF13Ready    "OF13Ready"
#define  GTF13Done     "TF13Done"
#define  GOF13Done     "OF13Done"

//[Appia]
#define  GAppia        "APPIA"
#define  GIsLocked     "IsLocked"

#define  GSIDAG        "SESSIONID-APPIAGROUP"
#define  GAppiaFWGroup "APPIA-FWGROUP"

//[T1099]
#define  GMatPwd       "MatPassword"
#define  GMatSesID     "MatSesID"
#define  GMatSrcPort   "MatSrcPort"

//TMP
#define  GPwd          "Password"
#define  GSPort        "SourcePort"
#define  GSeqNo        "SeqNo"
#define  GAPCode       "APCode"
#define  GPortName     "Portname"
#define  GSourceIP1    "SourceIP1"
#define  GSourceIP2    "SourceIP2"
#define  GApplyDate    "ApplyDate"
#define  GIP           "IP"
#define  GLineNo       "LineNo"
//[T109901]
#define  GTMPID        "TMPID"

//Appia-[Main]
#define  GAConnection  "connection"
#define  GMiddleware   "middleware"
#define  GTestNo       "TestNo"

//Appia-[Middleware]
//#define  GMidTesterB   "[mTester]"
#define  GMidTester    "mTester"
#define  GMidSesID     "middleware_session_id"
#define  GSocket       "SOCKET"

#define  GMidType           "middleware_type"
#define  GSocketPort        "socket_port"
#define  GMidGlobalMsg      "middleware_global_messages"
#define  GMidMsgAckPayload  "middleware_message_ack_payload"
#define  GMidSesEvent       "middleware_session_events"

//Appia-[connection]
#define  GConFixVer    "connection_type"
#define  GNetAddress   "net_address"
#define  GRemotePort   "remote_port"  //[Add]20110523
#define  GLocalFirmID  "local_firm_id"
#define  GBrokerFirmID "remote_firm_id"
#define  GHeartbeatInt "heartbeat_interval"
#define  GStartSeqNum  "start_seq_num"
#define  GTradingSes   "trading_session"
#define  GTradingSesTB "[TRADING_SCHEDULE_TEST]"
#define  GTradingSesB  "[TRADING_SCHEDULE]"

//Appia-[KWAY]
#define  GKway         "KWAY"

//----------------------------------------------------------------------------
//};  // FT
//};  // Bx
//};  // Tw
};  // Kway
//---------------------------------------------------------------------------
#endif
