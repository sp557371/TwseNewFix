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
#define  GFIX42        "FIX_42"
#define  GFIX43        "FIX_43"
#define  GFIX44        "FIX_44"
#define  GTwse         "XTAI"
#define  GOtc          "ROCO"

#define  GMainIni      "Main.ini"
#define  GTradeSegIni  "TradeSegment.ini"

//XTester.ini
#define  GSummary      "Summary"
#define  GSesID        "SesID"

//Main.ini - [Main]
#define  GAppiaPath    "AppiaPath"
#define  GTesterPath   "TesterPath"
#define  GNextDate     "NextDate"
#define  GBroker       "Broker"
#define  GBeginTime    "BTime"
#define  GEndTime      "ETime"
#define  GWorkCode     "WorkCode"
#define  GFixSes       "FixSes"
#define  GAppiaMWPort  "appiamwport"
#define  GType         "Type"
#define  GMWBegPort    "mw_begin_port"
#define  GK05Ready     "K05Ready"
#define  GTK05Path     "TK05Path"
#define  GOK05Path     "OK05Path"
#define  GTK05File     "TK05File"
#define  GOK05File     "OK05File"

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
#define  GSourceIP     "SourceIP"
#define  GApplyDate    "ApplyDate"
//[T109901]
#define  GTMPID        "TMPID"

//Appia-[Main]
#define  GAConnection  "connection"

//Appia-[Middleware]
//#define  GMidTesterB   "[mTester]"
#define  GMidTester    "mTester"
#define  GMidSesID     "middleware_session_id"
#define  GSocket       "SOCKET"

#define  GMidType      "middleware_type"
#define  GSocketPort   "socket_port"
#define  GMidGlobalMsg "middleware_global_messages"
#define  GMidMsgAckPayload  "middleware_message_ack_payload"
#define  GMidSesEvent       "middleware_session_events"

//Appia-[connection]
#define  GConFixVer    "connection_type"
#define  GNetAddress   "net_address"
#define  GLocalFirmID  "local_firm_id"
#define  GBrokerFirmID "remote_firm_id"
#define  GHeartbeatInt "heartbeat_interval"
#define  GStartSeqNum  "start_seq_num"
#define  GTradingSes   "trading_session"
#define  GTradingSesTB "[TRADING_SCHEDULE_TEST]"
#define  GTradingSesB  "[TRADING_SCHEDULE]"


//----------------------------------------------------------------------------
//};  // FT
//};  // Bx
//};  // Tw
};  // Kway
//---------------------------------------------------------------------------
#endif
