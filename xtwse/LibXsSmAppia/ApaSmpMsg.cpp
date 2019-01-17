/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "ApaSmpMsg.h"
#include "tinyxml.h"
#include "Nsa_DbgInfo.h"
#include "Nsa_UtilStr.h"
#include "Nsa_UtilComm.h"
#include "Nsa_LineBuf.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgType impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TSmpMsgType::assign (const char* str)
{
   Data_ = smt_Unknown;

   if (str && strlen(str) >= 1)
      assign_char(str[0]);
}
//---------------------------------------------------------------------------

K_mf(void) TSmpMsgType::assign_char (char ch)
{
   Data_ = (ESmpMsgType)ch;
}
//---------------------------------------------------------------------------

K_mf(char) TSmpMsgType::as_char () const
{
   return (char)Data_;
}
//---------------------------------------------------------------------------

K_mf(std::string) TSmpMsgType::as_string () const
{
   switch (Data_) {
      case smt_OperatorCommandRequest:       return std::string("OperatorCommandRequest");
      case smt_OperatorCommandResponse:      return std::string("OperatorCommandResponse");
      case smt_MiddlewareEvent:              return std::string("MiddlewareEvent");
      case smt_Logon:                        return std::string("Logon");
      case smt_OutboundMessage:              return std::string("OutboundDelivery");
      case smt_Reject:                       return std::string("Reject");
      case smt_ShutdownMiddleware:           return std::string("ShutdownMiddleware");
      case smt_StartMiddlwareEvents:         return std::string("StartMiddlwareEvents");
      case smt_StopMiddlwareEvents:          return std::string("StopMiddlwareEvents");
      case smt_GetSessionInfoRequest:        return std::string("GetSessionInfoRequest");
      case smt_GetSessionInfoResponse:       return std::string("GetSessionInfoResponse");
      case smt_GetMiddlewareInfoRequest:     return std::string("GetMiddlewareInfoRequest");
      case smt_GetMiddlewareInfoResponse:    return std::string("GetMiddlewareInfoResponse");
      case smt_GetSessionListRequest:        return std::string("GetSessionListRequest");
      case smt_GetSessionListResponse:       return std::string("GetSessionListResponse");
      case smt_GetMiddlewareConfigRequest:   return std::string("GetMiddlewareConfigRequest");
      case smt_GetMiddlewareConfigResponse:  return std::string("GetMiddlewareConfigResponse");
      case smt_RestoreMessageRequest:        return std::string("RestoreMessageRequest");
      case smt_SetMiddlewareConfigRequest:   return std::string("SetMiddlewareConfigRequest");
   }
   
   return std::string();
}  
   
   
/////////////////////////////////////////////////////////////////////////////
// class TSmpMsg impl
/////////////////////////////////////////////////////////////////////////////
const char   TSmpMsg::VersionInfo[] = { "SOCKET2.0" };
const char   TSmpMsg::Delimiter     = 0x01;
const char   TSmpMsg::EndOfMsg      = 0x03;
const size_t TSmpMsg::HeaderLength  = 21;   // "SOCKET2.0|0000000228|"
//---------------------------------------------------------------------------
   
K_mf(int) TSmpMsg::Pack (std::string& strMsg)
{  
   char        buf[1024];
   int         len;
   std::string strMsgData;
      
   GetMsgData(strMsgData);
   
   //SOCKET2.0|0000000003|U|$
   MsgDataLen_ = strMsgData.length();
   len = sprintf(buf, "%s%c%010d%c%s%c%c",
                        VersionInfo,        TSmpMsg::Delimiter,
                        MsgDataLen_+2,      TSmpMsg::Delimiter,
                        strMsgData.c_str(), TSmpMsg::Delimiter,
                        TSmpMsg::EndOfMsg);
   
   strMsg.assign(buf, len);

   return len;
}  
//---------------------------------------------------------------------------
   
K_mf(int) TSmpMsg::GetMsgData (std::string& strMsgData) const
{
   // SOCKET2.0|0000000003|W|$
   //                      W
   // SOCKET2.0|0000000014|N|MyClient|0|$
   //                      N|MyClient|0
   char        buf[1024];
   int         len;
   std::string strMsgDataBody;
   char        ch = MsgType_.as_char();

   OnGetMsgData(strMsgDataBody);

   if (strMsgDataBody.empty())
      len = sprintf(buf, "%c", ch);
   else
      len = sprintf(buf, "%c%c%s", ch, TSmpMsg::Delimiter, strMsgDataBody.c_str());

   strMsgData.assign(buf, len);

   return len;
}
//---------------------------------------------------------------------------

K_mf(TSmpMsg::TSmpMsgRef) TSmpMsg::Unpack (const char* str)
{
   static int  nDelimiter = (int)TSmpMsg::Delimiter;

   TSmpMsgRef  msgRef;
   const char* curr = str;
   const char* ptr  = strchr(curr, nDelimiter);

   //==============================================
   // 'SOCKET2.0|0000000003|U|$'
   if (ptr == NULL) {
      return msgRef;
   }
   curr = (ptr+1);

   //==============================================
   // '0000000003|U|$'
   // MsgDataLen_ = 'U|$' = 3
   ptr = strchr(curr, nDelimiter);
   if (ptr == NULL) {
      return msgRef;
   }
   MsgDataLen_ = atoi(curr);
   if (MsgDataLen_ == 0) {
      return msgRef;
   }

   //==============================================
   // 'U|$'
   curr = (ptr+1);
   MsgType_.assign_char(*curr);
   if (MsgType_.is_valid() == false) {
      return msgRef;
   }
   if (*(++curr) != TSmpMsg::Delimiter) {
      return msgRef;
   }

   //==============================================
   // 這邊只可以 erase 結尾符號 '$', 不可多 erase 其餘的
   // 因為結尾有可能是 '|$' or '$$'
   ////==============================================
   //// 判斷結尾是否為 '|$'
   //size_t      currLen = MsgDataLen_ - 2;
   //std::string strMsgData(++curr, currLen);
   //
   //if (currLen > 1) {
   //   if (strMsgData[currLen-2] != TSmpMsg::Delimiter && strMsgData[currLen-1] != TSmpMsg::EndOfMsg) {
   //      return msgRef;
   //   }
   //   currLen -= 2;
   //}
   //else {
   //   if (strMsgData[currLen-1] != TSmpMsg::EndOfMsg) {
   //      return msgRef;
   //   }
   //   currLen -= 1;
   //}
   //strMsgData.erase(currLen);
   size_t      currLen = MsgDataLen_ - 2;
   std::string strMsgData(++curr, currLen);

   if (strMsgData[currLen-1] != TSmpMsg::EndOfMsg) {
      return msgRef;
   }
   currLen -= 1;
   strMsgData.erase(currLen);

   //==============================================
   // 開始分配
   TSmpMsg* msg = NULL;

   switch (MsgType_) {
   case smt_MiddlewareEvent:              msg = new TSmpMsgMiddlewareEvent            (*this);  break;
   case smt_OperatorCommandResponse:      msg = new TSmpMsgOperatorCommandResponse    (*this);  break;
   case smt_Reject:                       msg = new TSmpMsgReject                     (*this);  break;
   case smt_GetSessionInfoResponse:       msg = new TSmpMsgGetSessionInfoResponse     (*this);  break;
   case smt_GetMiddlewareInfoResponse:    msg = new TSmpMsgGetMiddlewareInfoResponse  (*this);  break;
   case smt_GetMiddlewareConfigResponse:  msg = new TSmpMsgGetMiddlewareConfigResponse(*this);  break;
   case smt_GetSessionListResponse:       msg = new TSmpMsgGetSessionListResponse     (*this);  break;
   default:  return msgRef;
   }

   // 解析失敗
   if (msg->SetMsgData(strMsgData) == false) {
      delete msg;
      return msgRef;
   }

   msgRef.reset(msg);

   return msgRef;
}
//---------------------------------------------------------------------------

K_mf(bool) TSmpMsg::SetMsgData (const std::string& strMsgData)
{
   MsgDataLen_ = strMsgData.length();

   return OnSetMsgData(strMsgData);
}


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgLogon impl
/////////////////////////////////////////////////////////////////////////////
K_mf(int) TSmpMsgLogon::OnGetMsgData (std::string& strMsgBody) const
{
   // SOCKET2.0|0000000014|N|MyClient|0|$
   //                        MyClient|0
   char buf[50];
   int  len;

   len = sprintf(buf, "%s%c%d", MiddlewareID_.c_str(), TSmpMsg::Delimiter, ReliableMode_);
   strMsgBody.assign(buf, len);

   return len;
}


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgReject impl
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TSmpMsgReject::OnSetMsgData (const std::string& str)
{
   /*
   // smt_Logon                 ==> SOCKET2.0|0000000047|r|MiddlewareInterface XCtrl already connected|$
   //                           ==> SOCKET2.0|0000000040|r|expected Logon message, received ‘W’|$
   // smt_GetSessionInfoRequest ==> SOCKET2.0|0000000039|r|SessionInfo does not exist for T1100$

   // SOCKET2.0|0000000040|r|expected Logon message, received ‘W’|$
   //                        expected Logon message, received ‘W’
   */
   std::string::size_type npos = std::string::npos;
   std::string::size_type pos1, pos2;

   Data_.RejMsg_ = str;

   do {
      //===========================================
      // smt_Logon: expected Logon message, received ‘W’
      const char logon1[] = { "expected Logon message, received" };

      if (Data_.RejMsg_.find(logon1) != npos) {
         Data_.RejReason_.assignT(srr_NotLogon);
         break;
      }

      //===========================================
      // smt_Logon: MiddlewareInterface XCtrl already connected
      const char logon2[] = { "MiddlewareInterface" };
      const char logon3[] = { "already connected"   };

      pos1 = Data_.RejMsg_.find(logon2);
      if (pos1 != npos) {
         pos2 = Data_.RejMsg_.find(logon3);
         if (pos2 != npos) {
            Data_.RejReason_.assignT(srr_AlreadyLogon);
            break;
         }
      }

      //===========================================
      // SessionInfo does not exist for T1100
      const char   sesinfo1[]   = { "SessionInfo does not exist for " };
      const size_t sesinfo1_len = strlen(sesinfo1);

      pos1 = Data_.RejMsg_.find(sesinfo1);
      if (pos1 != npos) {
         Data_.RejReason_.assignT(srr_SesInfo_SesIDNotFound);
         Data_.RejParam1_ = Data_.RejMsg_.substr(pos1+sesinfo1_len);
         break;
      }
   } while (false);

   return true;
}


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgOperatorCommandRequest impl
/////////////////////////////////////////////////////////////////////////////
K_mf(int) TSmpMsgOperatorCommandRequest::OnGetMsgData (std::string& strMsgBody) const
{
   // SOCKET2.0|0000000011|C|version|$
   //                        version
   strMsgBody = Command_;
   
   return strMsgBody.length();
}


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgOperatorCommandResponse impl
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TSmpMsgOperatorCommandResponse::OnSetMsgData (const std::string& str)
{
   // SOCKET2.0|0000000042|c|version|Appia2.0.0.5 / JVM: 1.3.1-b24|$
   //                        version|Appia2.0.0.5 / JVM: 1.3.1-b24

   // suspend T110001|Invalid argument for command 'suspend': argument 'T110001': Unknown connection ID.$

   std::string::size_type pos = str.find(TSmpMsg::Delimiter);
   if (pos == std::string::npos)
      return false;

   std::string            status = str.substr(pos+1);
   std::string::size_type npos   = std::string::npos;

   Data_.FullCommand_ = str.substr(0, pos);
   Data_.Status_      = status;
   Data_.SmpOperCmd_.Unpack(Data_.FullCommand_);

   do {
      //===========================================
      // socr_UnknownSessioID
      const char fmt1[] = { "Invalid argument for command" };
      const char fmt2[] = { "Unknown connection ID"        };

      if (status.find(fmt1) != npos && status.find(fmt2) != npos) {
         Data_.OperCmdResu_.assignT(socr_UnknownSessioID);
         break;
      }

   } while (false);

   return true;
}


/////////////////////////////////////////////////////////////////////////////
// struct SSmpMsgMiddlewareEvent
/////////////////////////////////////////////////////////////////////////////
K_mf(void) SSmpMsgMiddlewareEvent::Parse_PossResend ()
{
   std::string::size_type pos = MetaData_.find("PossResend");
   
   if (pos != std::string::npos)
      IsPossResend_.assign(true);
}
//---------------------------------------------------------------------------

K_mf(void) SSmpMsgMiddlewareEvent::Parse_MsgType_A  ()
{
   //==============================================
   // MsgType=A, <metadata><socket local="10.204.101.51:23201" remote="10.204.101.212:3586"/></metadata>
   TiXmlDocument xml;
   
   xml.Parse(MetaData_.c_str());
   while (!xml.Error()) {
      TiXmlElement* elemMetadata = xml.RootElement();
      if (!elemMetadata)
         break;
         
      TiXmlElement* elemSocket = elemMetadata->FirstChildElement("socket");
      if (!elemSocket)
         break;
         
      const char* ccpRemote = elemSocket->Attribute("remote");
      if (!ccpRemote)
         break;
               
      const char* ptr = strchr(ccpRemote, (int)':');
      if (ptr) {
         RemoteIP_  .assign(ccpRemote, ptr);
         RemotePort_.assign(ptr+1);
         
         TUtilComm::ConsMsg("RemoteIP[%s], RemotePort[%s]", RemoteIP_.c_str(), RemotePort_.c_str());
      }

      break;
   }
}
/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgMiddlewareEvent impl
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TSmpMsgMiddlewareEvent::OnSetMsgData (const std::string& str)
{
   // 23041||SBI|FIX_42|8|2||8=FIX.4.2|9=0140|35=8|
   //    34=2|49=SBI|56=SLGM|52=20011105-20:20:33|37=0|17=0|11=j0|55=IBM|
   //    54=1|38=5000|40=1|21=1|20=0|151=5000|14=0|31=0.0|32=0|6=0.0|
   //    150=0|39=0|10=226|
   std::string::size_type stpos, edpos, len;
   char                   caInt[20];

   //==============================================
   // EventType
   edpos = str.find(TSmpMsg::Delimiter);
   if (edpos == std::string::npos) {
      return false;
   }
   memcpy(caInt, str.c_str(), edpos);
   caInt[edpos] = '\0';
   Data_.EventType_.assign(caInt);

   //==============================================
   // ClientMsgID
   ++edpos;
   stpos = edpos;
   edpos = str.find(TSmpMsg::Delimiter, stpos);
   if (edpos == std::string::npos) {
      return false;
   }
   Data_.ClientMsgID_ = str.substr(stpos, edpos-stpos);

   //==============================================
   // SessionID
   ++edpos;
   stpos = edpos;
   edpos = str.find(TSmpMsg::Delimiter, stpos);
   if (edpos == std::string::npos) {
      return false;
   }
   Data_.SessionID_ = str.substr(stpos, edpos-stpos);

   //==============================================
   // Protocol
   ++edpos;
   stpos = edpos;
   edpos = str.find(TSmpMsg::Delimiter, stpos);
   if (edpos == std::string::npos) {
      return false;
   }
   Data_.Protocol_ = str.substr(stpos, edpos-stpos);

   //==============================================
   // MessageType
   ++edpos;
   stpos = edpos;
   edpos = str.find(TSmpMsg::Delimiter, stpos);
   if (edpos == std::string::npos) {
      return false;
   }
   Data_.MessageType_ = str.substr(stpos, edpos-stpos);

   //==============================================
   // SequenceNumber
   ++edpos;
   stpos = edpos;
   edpos = str.find(TSmpMsg::Delimiter, stpos);
   if (edpos == std::string::npos) {
      return false;
   }
   len = edpos-stpos;
   memcpy(caInt, str.c_str()+stpos, len);
   caInt[len] = '\0';
   Data_.SequenceNumber_.assign(caInt);

   //==============================================
   // MetaData
   ++edpos;
   stpos = edpos;
   edpos = str.find(TSmpMsg::Delimiter, stpos);
   if (edpos == std::string::npos) {
      return false;
   }
   Data_.MetaData_ = str.substr(stpos, edpos-stpos);
   if (Data_.MetaData_.empty() == false) {
      Data_.Parse_PossResend();
      
      if (Data_.MessageType_.size() == 1 && Data_.MessageType_[0] == 'A')
         Data_.Parse_MsgType_A();
   }

   //==============================================
   // EventData
   ++edpos;
   stpos            = edpos;
   Data_.EventData_ = str.substr(stpos);

   //==============================================
   // 這些 EventType 結尾會有兩個 '|', 所以要移除一個
   len = Data_.EventData_.length();
   if (len > 1) {
      if (Data_.EventType_ == set_SessionMessageReceived      ||
          Data_.EventType_ == set_ApplicationeMessageReceived ||
          Data_.EventType_ == set_IncomingMessageRestored        )
      {
         --len;
         if (Data_.EventData_[len] == TSmpMsg::Delimiter && Data_.EventData_[len-1] == TSmpMsg::Delimiter)
            Data_.EventData_.erase(len);
      }
   }

   return true;
}


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgOutboundMessage impl
/////////////////////////////////////////////////////////////////////////////
K_mf(int) TSmpMsgOutboundMessage::OnGetMsgData (std::string& strMsgBody) const
{
   // SOCKET2.0|0000000099|O|order0|SBI|FIX|0|D|35=D|49=SLGM|56=SBI|11=j0|21=1|55=IBM|54=1|38=5000|40=1|60=20010101-01:01:01||$
   char buf[1024];
   int  len;

   len = sprintf(buf, "%s%c%s%c%s%c%d%c%s%c%s",
                        Data_.ClientMsgID_.c_str(), TSmpMsg::Delimiter,
                        Data_.SessionID_  .c_str(), TSmpMsg::Delimiter,
                        Data_.Protocol_   .c_str(), TSmpMsg::Delimiter,
                        Data_.Format_,              TSmpMsg::Delimiter,
                        Data_.MessageType_.c_str(), TSmpMsg::Delimiter,
                        Data_.MessageData_.c_str());

   strMsgBody.assign(buf, len);
   
   return len;
}


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgGetSessionInfoRequest impl
/////////////////////////////////////////////////////////////////////////////
K_mf(int) TSmpMsgGetSessionInfoRequest::OnGetMsgData (std::string& strMsgBody) const
{
   // SOCKET2.0|0000000007|I|SBI|$
   //                        SBI
   strMsgBody = SessionID_;
   
   return strMsgBody.length();
}


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgGetSessionInfoResponse impl
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TSmpMsgGetSessionInfoResponse::OnSetMsgData (const std::string& str)
{
   // SOCKET2.0|0000000719|i|FIX|HeartbeatCount=1692|ExecutionAckCount=1|ResendActiveFlag=false|
   //    EncryptionType=0|LogonCount=2|SessionID=SBI0|Correct-Count=0|NetAddresses=127.0.0.1|CancelCount=0|
   //    NormalLogout-Flag=true|Port=7000|SuspendRejectReason=|ProtocolVersion=FIX_42|RemoteFirmID=SBI0|Group=|
   //    OutByteCount=126456|InMsgCount=1707|HoldFlag=false|LastOutMsgTime=0|Description=|LocalFirmID=SLGM0|
   //    LastOutMsgSeqNum=1697|AllocationCount=0|ConnectState=0|InMsgSeqNum=1708|SuspendFlag=false|
   //    ServiceDownFlag=false|ExecutionCount=13|OutMsgSeqNum=1698|OutMsgCount=1697|LastInMsgTime=0|Contact=|
   //    RejectCount=0|IOICount=0|LocalTraderID=|HeartbeatInterval=30|ServiceDownRejectReason=|OrderCount=4|
   //    HoldRejectRea-son=|LastEODTime=1071790076729|InByteCount=128889|$

   // SOCKET2.0|0000000719|i|FIX|HeartbeatCount=1692|...|$
   // ==> FIX|HeartbeatCount=1692|...
   TStrSplit sp(str.c_str(), TSmpMsg::Delimiter);
   char      chsp = '=';
   
   Data_.SessionID_            = sp.GetStr_ByKey("SessionID",            chsp);
   Data_.RemoteFirmID_         = sp.GetStr_ByKey("RemoteFirmID",         chsp);
   Data_.LocalFirmID_          = sp.GetStr_ByKey("LocalFirmID",          chsp);
   Data_.LocalTraderID_        = sp.GetStr_ByKey("LocalTraderID",        chsp);
   Data_.Group_                = sp.GetStr_ByKey("Group",                chsp);
   Data_.Description_          = sp.GetStr_ByKey("Description",          chsp);
   Data_.Contact_              = sp.GetStr_ByKey("Contact",              chsp);
   Data_.Protocol_             = sp.GetStr_ByKey("Protocol",             chsp);
   Data_.ProtocolVersion_      = sp.GetStr_ByKey("ProtocolVersion",      chsp);
   Data_.Version_              = sp.GetStr_ByKey("Version",              chsp);
   Data_.NetAddresses_         = sp.GetStr_ByKey("NetAddresses",         chsp);
   Data_.Port_                 = sp.GetStr_ByKey("Port",                 chsp);
   Data_.EncryptionType_       = sp.GetStr_ByKey("EncryptionType",       chsp);
   Data_.HeartbeatInterval_    = sp.GetStr_ByKey("HeartbeatInterval",    chsp);
   Data_.LastEODTime_          = sp.GetStr_ByKey("LastEODTime",          chsp);
   Data_.HoldRejReason_        = sp.GetStr_ByKey("HoldRejReason",        chsp);
   Data_.ServiceDownRejReason_ = sp.GetStr_ByKey("ServiceDownRejReason", chsp);
   Data_.SuspendRejReason_     = sp.GetStr_ByKey("SuspendRejReason",     chsp);
   Data_.ResendActiveFlag_     = sp.GetStr_ByKey("ResendActiveFlag",     chsp);
   Data_.NormalLogoutFlag_     = sp.GetStr_ByKey("NormalLogoutFlag",     chsp);
   Data_.InMsgCount_           = sp.GetStr_ByKey("InMsgCount",           chsp);
   Data_.OutMsgCount_          = sp.GetStr_ByKey("OutMsgCount",          chsp);
   Data_.InByteCountNumber_    = sp.GetStr_ByKey("InByteCountNumber",    chsp);
   Data_.OutByteCount_         = sp.GetStr_ByKey("OutByteCount",         chsp);
   Data_.LastOutMsgSeqNum_     = sp.GetStr_ByKey("LastOutMsgSeqNum",     chsp);
   Data_.AllocationCount_      = sp.GetStr_ByKey("AllocationCount",      chsp);
   Data_.CancelCount_          = sp.GetStr_ByKey("CancelCount",          chsp);
   Data_.CorrectCount_         = sp.GetStr_ByKey("CorrectCount",         chsp);
   Data_.ExecutionCount_       = sp.GetStr_ByKey("ExecutionCount",       chsp);
   Data_.ExecutionAckCount_    = sp.GetStr_ByKey("ExecutionAckCount",    chsp);
   Data_.HeartbeatCount_       = sp.GetStr_ByKey("HeartbeatCount",       chsp);
   Data_.IOICount_             = sp.GetStr_ByKey("IOICount",             chsp);
   Data_.LogonCount_           = sp.GetStr_ByKey("LogonCount",           chsp);
   Data_.OrderCount_           = sp.GetStr_ByKey("OrderCount",           chsp);
   Data_.RejectCount_          = sp.GetStr_ByKey("RejectCount",          chsp);

   Data_.ConnectState_   .assign(sp.GetStr_ByKey("ConnectState",    chsp));
   Data_.HoldFlag_       .assign(sp.GetStr_ByKey("HoldFlag",        chsp));
   Data_.ServiceDownFlag_.assign(sp.GetStr_ByKey("ServiceDownFlag", chsp));
   Data_.SuspendFlag_    .assign(sp.GetStr_ByKey("SuspendFlag",     chsp));
   Data_.InMsgSeqNum_    .assign(sp.GetStr_ByKey("InMsgSeqNum",     chsp));
   Data_.OutMsgSeqNum_   .assign(sp.GetStr_ByKey("OutMsgSeqNum",    chsp));

   Data_.LastInMsgTime_ .assign_apafmt(sp.GetStr_ByKey("LastInMsgTime",  chsp));
   Data_.LastOutMsgTime_.assign_apafmt(sp.GetStr_ByKey("LastOutMsgTime", chsp));

   return true;
}


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgGetMiddlewareInfoResponse impl
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TSmpMsgGetMiddlewareInfoResponse::OnSetMsgData (const std::string& str)
{
   // SOCKET2.0|0000000299|k|middleware1|SOCKET|false|14|Global|-
   //    1|0|$CTCI|1|0|$RECV|9345|0|$RECV40|905|0|$RECV41|905|0|$RECV42|905|0|$RE
   //    CV43|905|0|$RECV44|905|0|$SEND|9344|0|$SEND40|905|0|$SEND41|905|0|$SEND4
   //    2|905|0|$SEND43|905|0|$SEND44|905|0|$true|0|1|0|1|1|1|1|1||||$SEND41=ORD
   //    41A|SEND42=CANCL42B|SEND43=ORD43Z|SEND44=ORD44P$

   // SOCKET2.0|0000000299|k|middleware1|SOCKET|false|14|Global|-...$
   // ==> middleware1|SOCKET|false|14|Global|-...

         std::string strTmp;
   const char*       beg = str.c_str();
   const char*       end = strchr(beg, TSmpMsg::Delimiter);

   //==============================================
   // MiddlewareID
   //    ==> "middleware1"
   //==============================================
   if (!end) {
      return false;
   }
   Data_.MiddlewareID_.assign(beg, end-beg);
   beg = (end + 1);
   //==============================================
   // MiddlewareType
   //    ==> "SOCKET"
   //==============================================
   end = strchr(beg, TSmpMsg::Delimiter);
   if (!end) {
      return false;
   }
   Data_.MiddlewareType_.assign(beg, end-beg);
   beg = (end + 1);
   //==============================================
   // ReliableModesStatus
   //    ==> "false"
   //==============================================
   end = strchr(beg, TSmpMsg::Delimiter);
   if (!end) {
      return false;
   }
   strTmp.assign(beg, end-beg);
   Data_.ReliableModesStatus_.assign(strTmp);

   beg = (end + 1);
   //==============================================
   // NumberOfInboundQueueInfoBlocks
   //    ==> "14"
   //==============================================
   end = strchr(beg, TSmpMsg::Delimiter);
   if (!end) {
      return false;
   }
   strTmp.assign(beg, end-beg);
   Data_.NumberOfInboundQueueInfoBlocks_.assign(strTmp);
   beg = (end + 1);
   // InboundQueueInfo
   beg = Data_.InboundQueueInfo_.Parse(beg, Data_.NumberOfInboundQueueInfoBlocks_.get_orig());
   if (!beg) {
      return false;
   }
   // ConnectionStatus
   end = strchr(beg, TSmpMsg::Delimiter);
   if (!end) {
      return false;
   }
   strTmp.assign(beg, end-beg);
   Data_.ConnectionStatus_.assign(strTmp);
   
   beg = (end + 1);
   //==============================================
   // MiddlewareConfig
   //    ==> "0|1|0|1|1|1|1|1||||"
   //==============================================
   beg = Data_.MiddlewareConfig_.Parse(beg, true);
   if (!beg) {
      return false;
   }
   //==============================================
   // LastMsgClientIDs
   //    ==> "SEND41=ORD41A|SEND42=CANCL42B|SEND43=ORD43Z|SEND44=ORD44P"
   //==============================================
   beg = Data_.LastMsgClientIDs_.Parse(beg);
   if (!beg) {
      return false;
   }

   return true;
}


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgGetMiddlewareInfoRequest impl
/////////////////////////////////////////////////////////////////////////////
K_mf(int) TSmpMsgGetMiddlewareInfoRequest::OnGetMsgData (std::string& strMsgBody) const
{
   // SOCKET2.0|0000000007|K|SBI|$
   //                        SBI
   strMsgBody = MiddlewareID_;

   return strMsgBody.length();
}


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgGetMiddlewareConfigResponse impl
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TSmpMsgGetMiddlewareConfigResponse::OnSetMsgData (const std::string& str)
{
   // SOCKET2.0|0000000025|g|0|1|0|0|0|0|1|1|||SBI,INET|$
   // ==> 0|1|0|0|0|0|1|1|||SBI,INET|
   Data_.Parse(str.c_str(), false);
   
   return true;
//   // SOCKET2.0|0000000025|g|0|1|0|0|0|0|1|1|||SBI,INET|$
//   // ==> 0|1|0|0|0|0|1|1|||SBI,INET|
//   TStrSplit   sp(str.c_str(), TSmpMsg::Delimiter);
//   std::string strTmp;
//
//   Data_.InMsgFormat_           .assign(sp.GetStr(0));
//   Data_.AppicationMessages_    .assign(sp.GetStr(1));
//   Data_.SessionMessages_       .assign(sp.GetStr(2));
//   Data_.MessageValidatedEvents_.assign(sp.GetStr(3));
//   Data_.MessageCommitEvents_   .assign(sp.GetStr(4));
//   Data_.MessageSentEvents_     .assign(sp.GetStr(5));
//   Data_.SessionEvents_         .assign(sp.GetStr(6));
//   Data_.GlobalEvents_          .assign(sp.GetStr(7));
//
//   strTmp = sp.GetStr(8);
//   if (!strTmp.empty()) {
//      Data_.CheckRemote_.push_back(strTmp);
//   }
//
//   strTmp = sp.GetStr(9);
//   if (!strTmp.empty()) {
//      Data_.MessageTypes_.push_back(strTmp);
//   }
//
//   strTmp = sp.GetStr(10);
//   if (!strTmp.empty()) {
//      TStrSplit sp2(strTmp.c_str(), ',');
//      size_t    size = sp2.GetSize();
//      
//      for (size_t i=0; i<size; ++i) {
//         Data_.SessionIDs_.push_back(sp2.GetStr(i));
//      }
//   }
//
//   return true;
}


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgGetSessionListResponse impl
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TSmpMsgGetSessionListResponse::OnSetMsgData (const std::string& str)
{
   // SOCKET2.0|0000000012|l|SBI,INET|$
   // ==>                    SBI,INET|
   TStrSplit   sp(str.c_str(), TSmpMsg::Delimiter);
   std::string strTmp = sp.GetStr(0);

   if (!strTmp.empty()) {
      TStrSplit sp2(strTmp.c_str(), ',');
      size_t    size = sp2.GetSize();
      
      for (size_t i=0; i<size; ++i) {
         SessionIDs_.push_back(sp2.GetStr(i));
      }
   }

   return true;
}


/////////////////////////////////////////////////////////////////////////////
// class TSmpMsgRestoreMessageRequest impl
/////////////////////////////////////////////////////////////////////////////
K_mf(int) TSmpMsgRestoreMessageRequest::OnGetMsgData (std::string& strMsgBody) const
{
   // SOCKET2.0|0000000012|R|SBI|3|9|$
   // ==>                    SBI|3|9
   char buf[256];
   int  len;
   
   len = sprintf(buf, "%s%c%d%c%d",
                        SessionID_.c_str(),     TSmpMsg::Delimiter,
                        BeginSeqNo_.get_orig(), TSmpMsg::Delimiter,
                        EndSeqNo_  .get_orig());

   strMsgBody.assign(buf, len);
   
   return len;
}


/////////////////////////////////////////////////////////////////////////////
// class TSmpSetMiddlewareConfigRequest impl
/////////////////////////////////////////////////////////////////////////////
K_mf(int) TSmpSetMiddlewareConfigRequest::OnGetMsgData (std::string& strMsgBody) const
{
   // SOCKET2.0|0000000025|M|0|1|0|0|0|0|1|1|||SBI,INET|$
   // ==>                    0|1|0|0|0|0|1|1|||SBI,INET
   TLineBufNew line;
   char        buf[256];
   
   sprintf(buf, "%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c",
                     MwConf_.InMsgFormat_           .get_orig(), TSmpMsg::Delimiter,
                     MwConf_.AppicationMessages_    .as_int(),   TSmpMsg::Delimiter,
                     MwConf_.SessionMessages_       .as_int(),   TSmpMsg::Delimiter,
                     MwConf_.MessageValidatedEvents_.as_int(),   TSmpMsg::Delimiter,
                     MwConf_.MessageCommitEvents_   .as_int(),   TSmpMsg::Delimiter,
                     MwConf_.MessageSentEvents_     .as_int(),   TSmpMsg::Delimiter,
                     MwConf_.SessionEvents_         .as_int(),   TSmpMsg::Delimiter,
                     MwConf_.GlobalEvents_          .as_int(),   TSmpMsg::Delimiter);

   line.Append(buf);

   // CheckRemote
   if (MwConf_.CheckRemote_.empty()) {
      line.Append(TSmpMsg::Delimiter);
   }
   else {
      for (TSmpStringList::const_iterator iter=MwConf_.CheckRemote_.begin(); iter!=MwConf_.CheckRemote_.end(); ++iter) {
         line.Append(*iter);
         line.Append(',');
      }
      line.EraseBack(1);
   }
   // MessageTypes
   if (MwConf_.MessageTypes_.empty()) {
      line.Append(TSmpMsg::Delimiter);
   }
   else {
      for (TSmpStringList::const_iterator iter=MwConf_.MessageTypes_.begin(); iter!=MwConf_.MessageTypes_.end(); ++iter) {
         line.Append(*iter);
         line.Append(',');
      }
      line.EraseBack(1);
   }
   // SessionIDs
   if (!MwConf_.SessionIDs_.empty()) {
      for (TSmpStringList::const_iterator iter=MwConf_.SessionIDs_.begin(); iter!=MwConf_.SessionIDs_.end(); ++iter) {
         line.Append(*iter);
         line.Append(',');
      }
      line.EraseBack(1);
   }

   int len = (int)line.GetUsedSize();
   strMsgBody.assign(line.GetCStr(), len);
   
   return len;
}
//---------------------------------------------------------------------------
