/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "ApaSmpType.h"
#include "Nsa_Base.h"   // stricmp
#include "Nsa_UtilStr.h"
#include "Nsa_LineBuf.h"
#include "Nsa_UtilComm.h"
#include "ApaSmpMsg.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TSmpTime impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TSmpTime::assign_apafmt (const std::string& str)
{
   // 從 1970 年開始到現在總共的 milliseconds
   // 1256696131967 ==> 2009/10/28 10:15:31.967
   inherited::SetDateTime(TKDateTime());
   inherited::SetMS      (TKTimeMS  ());

   size_t len = str.length();
   if (len <= 3)
      return;

   std::string strDt(str.substr(0, len-3));
   std::string strMs(str.substr(len-3));
      
   time_t     t = atoi(strDt.c_str());
   struct tm* s = localtime(&t);

   TKDate     date(static_cast<word>(s->tm_year + 1900),
                   static_cast<byte>(s->tm_mon  + 1),
                   static_cast<byte>(s->tm_mday));

   TKTime     time(static_cast<byte>(s->tm_hour),
                   static_cast<byte>(s->tm_min),
                   static_cast<byte>(s->tm_sec));
      
   TKTimeMS   ms(atoi(strMs.c_str()));
   
   inherited::SetDateTime(date, time);
   inherited::SetMS      (ms);
}


/////////////////////////////////////////////////////////////////////////////
// class TSmpEventType impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TSmpEventType::assign (const char* str)
{
   Data_ = (ESmpEventType)(atoi(str));
}
//---------------------------------------------------------------------------

K_mf(bool) TSmpEventType::IsOutboundAcknowledgmentEvents () const
{
   if (inherited::Data_ == set_MessageValidated           ||
       inherited::Data_ == set_MessageSent                ||
       inherited::Data_ == set_MessageSendConnectionDown  ||
       inherited::Data_ == set_MessageCommitted             )
   {
      return true;
   }
   return false;
}
//---------------------------------------------------------------------------

K_mf(bool) TSmpEventType::IsSessionLevelEvents () const
{
   if (inherited::Data_ == set_SessionConnected           ||
       inherited::Data_ == set_SessionDisconnected        ||
       inherited::Data_ == set_SessionEndOfDay            ||
       inherited::Data_ == set_SessionConnectFailure      ||
       inherited::Data_ == set_SessionConnectTimeout      ||
       inherited::Data_ == set_SessionHold                ||
       inherited::Data_ == set_SessionRelease             ||
       inherited::Data_ == set_SessionSuspend             ||
       inherited::Data_ == set_SessionResume              ||
       inherited::Data_ == set_SessionServiceUp           ||
       inherited::Data_ == set_SessionServiceDown         ||
       inherited::Data_ == set_SessionChannelDisabled     ||
       inherited::Data_ == set_SessionChannelReady        ||
       inherited::Data_ == set_SessionChannelNotReady     ||
       inherited::Data_ == set_SessionStartEndOfDay       ||
       inherited::Data_ == set_SessionHaPrimaryStaleError ||
       inherited::Data_ == set_SessionCreated             ||
       inherited::Data_ == set_SessionDeleted               )
   {
      return true;
   }
   return false;
}
//---------------------------------------------------------------------------

K_mf(std::string) TSmpEventType::as_string (bool incNum) const
{
   char buf[128];

   buf[0] = '\0';
   if (incNum)
      sprintf(buf, "%d.", inherited::Data_);

   switch (inherited::Data_) {
      //===========================================
      // Outbound Acknowledgment Events
      case set_MessageValidated:             strcat(buf, "Message_Validated");               break;   // 23010
      case set_MessageSent:                  strcat(buf, "Message_Sent");                    break;   // 23020
      case set_MessageSendConnectionDown:    strcat(buf, "Message_Send_Connection_Down");    break;   // 23021
      case set_MessageCommitted:             strcat(buf, "Message_Committed");               break;   // 23030
      //===========================================
      // Outbound Error Events
      case set_MessageValidationError:       strcat(buf, "Message_Validation_Error");        break; // 23011
      case set_SessionMessageReceived:       strcat(buf, "Session_Message_Received");        break; // 23040
      case set_ApplicationeMessageReceived:  strcat(buf, "Applicatione_Message_Received");   break; // 23041
      case set_IncomingMessageRestored:      strcat(buf, "Incoming_Message_Restored");       break; // 23050
      case set_EndOfRestore:                 strcat(buf, "End_Of_Restore");                  break; // 23051
      case set_SessionConnected:             strcat(buf, "Session_Connected");               break; // 23210
      case set_SessionDisconnected:          strcat(buf, "Session_Disconnected");            break; // 23211
      case set_SessionEndOfDay:              strcat(buf, "Session_End_Of_Day");              break; // 23212
      case set_SessionConnectFailure:        strcat(buf, "Session_Connect_Failure");         break; // 23213
      case set_SessionConnectTimeout:        strcat(buf, "Session_Connect_Timeout");         break; // 23214
      case set_SessionHold:                  strcat(buf, "Session_Hold");                    break; // 23220
      case set_SessionRelease:               strcat(buf, "Session_Release");                 break; // 23221
      case set_SessionSuspend:               strcat(buf, "Session_Suspend");                 break; // 23222
      case set_SessionResume:                strcat(buf, "Session_Resume");                  break; // 23223
      case set_SessionServiceUp:             strcat(buf, "Session_Service_Up");              break; // 23224
      case set_SessionServiceDown:           strcat(buf, "Session_Service_Down");            break; // 23225
      case set_SessionChannelDisabled:       strcat(buf, "Session_Channel_Disabled");        break; // 23226
      case set_SessionChannelReady:          strcat(buf, "Session_Channel_Ready");           break; // 23227
      case set_SessionChannelNotReady:       strcat(buf, "Session_Channel_Not_Ready");       break; // 23228
      case set_SessionStartEndOfDay:         strcat(buf, "Session_Start_End_Of_Day");        break; // 23230
      case set_SessionHaPrimaryStaleError:   strcat(buf, "Session_HA_Primary_Stale_Error");  break; // 23231
      case set_SessionCreated:               strcat(buf, "Session_Created");                 break; // 23302
      case set_SessionDeleted:               strcat(buf, "Session_Deleted");                 break; // 23304
      //===========================================
      // Appia Server Level Events
      case set_EngineShutdown:               strcat(buf, "Engine_Shutdown");                 break; // 23333
   }

   if (buf[0] == '\0')
      sprintf(buf, "%d", inherited::Data_);

   return std::string(buf);
}


/////////////////////////////////////////////////////////////////////////////
// class TSmpBool impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TSmpBool::assign (const char* str)
{
   Data_ = false;
   
   if (str == NULL || strlen(str) == 0)
      return;

   if (stricmp(str, "true") == 0 || strcmp(str, "1") == 0)
      Data_ = true;
}
//---------------------------------------------------------------------------

K_mf(std::string) TSmpBool::as_string () const
{
   if (Data_ == true)
      return std::string("true");
   
   return std::string("false");
}


/////////////////////////////////////////////////////////////////////////////
// class TSmpCmd impl
/////////////////////////////////////////////////////////////////////////////
static const char*  SmpCmd_Str[] = {
   "",
   "version",
   "stats",
   "connect",
   "disconnect",
   "service_down",
   "service_up",
   "eod",
   "suspend",
   "resume",
   "hold",
   "release",
   "remove_middleware",
   "seq_reset",
   "out_msg_seq_num",
   "suspend_with_terminate",
};
//---------------------------------------------------------------------------

K_mf(void) TSmpCmd::assign (const char* str)
{
   for (int i=(int)esc_Unknown+1; i<esc_Total; ++i) {
      if (strcmp(str, SmpCmd_Str[i]) == 0) {
         Data_ = (ESmpCmd)i;
         return;
      }
   }
   Data_ = esc_Unknown;
}
//---------------------------------------------------------------------------

K_mf(std::string) TSmpCmd::as_string () const
{
   if (Data_ > esc_Unknown && Data_ < esc_Total)
      return std::string(SmpCmd_Str[Data_]);
   return std::string();
}


/////////////////////////////////////////////////////////////////////////////
// class TSmpOperCmd impl
/////////////////////////////////////////////////////////////////////////////
K_mf(TSmpString) TSmpOperCmd::Pack () const
{
   if (SmpCmd_.is_invalid())
      return TSmpString();

   TLineBuf line(256);
   
   line.Append(SmpCmd_.as_string());
   
   if (Param1_.empty() == false) {
      line.Append(' ');
      line.Append(Param1_);
   }

   if (Param2_.empty() == false) {
      line.Append(' ');
      line.Append(Param2_);
   }

   return TSmpString(line.GetCStr());
}
//---------------------------------------------------------------------------

K_mf(bool) TSmpOperCmd::Unpack (const TSmpString& str)
{
   SmpCmd_.clear();
   Param1_.clear();
   Param2_.clear();

   //==============================================
   // 格式: <cmd> <param1> <param2> ...
   TSmpString::size_type pos = str.find(' ');
   if (pos == TSmpString::npos)
      SmpCmd_.assign(str);
   else
      SmpCmd_.assign(str.substr(0, pos));

   if (SmpCmd_.is_invalid())
      return false;

   ESmpCmd eSmpCmd = SmpCmd_.get_orig();

   if (eSmpCmd >= esc_Param2) {
      TSmpString::size_type pos2 = str.rfind(' ');
      if (pos2 == TSmpString::npos)
         return false;

      Param1_.assign(str.substr(pos+1, pos2-pos-1));
      Param2_.assign(str.substr(pos2+1));

      if (Param1_.empty() || Param2_.empty())
         return false;
   }
   else if (eSmpCmd >= esc_Param1) {
      Param1_.assign(str.substr(pos+1));
      if (Param1_.empty())
         return false;
   }

   return true;
}


/////////////////////////////////////////////////////////////////////////////
// class TSmpInboundQueueInfo impl
/////////////////////////////////////////////////////////////////////////////
K_mf(const char*) TSmpInboundQueueInfo::Parse (const char* str, int num)
{
   // "Global|-1|0|$1100|43|0|$true|0|1|1|0|0|0|1|0|||1100|$$$"
   // ==> "Global|-1|0|$1100|43|0|$"
   const char* beg = str;
   const char* end;

   while (num > 0) {
      end = strchr(beg, TSmpMsg::EndOfMsg);
      if (!end)
         return NULL;
         
      std::string strUnit(beg, end-beg);
      TStrSplit   sp(strUnit.c_str(), TSmpMsg::Delimiter);
      size_t      size = sp.GetSize();

      if (size != 3)
         break;

      SSmpInboundQueueInfo one;

      one.SessionID_ = sp.GetStr(0);
      one.NewSeqNum_.assign(sp.GetInt(1));
      one.QueueSize_.assign(sp.GetInt(2));
      
      Data_.push_back(one);

      beg = (end + 1);
      --num;
   }

   return beg;
}


/////////////////////////////////////////////////////////////////////////////
// class TSmpMiddlewareConfig impl
/////////////////////////////////////////////////////////////////////////////
K_mf(const char*) TSmpMiddlewareConfig::Parse (const char* str, bool hasEndOfMsg)
{
   //==============================================
   // 會有兩種格式:
   // 'g': GetMiddlewareConfigResponse (hasEndOfMsg = false)
   //    SOCKET2.0|0000000025|g|0|1|0|0|0|0|1|1|||SBI,INET|$
   //    ==>                    0|1|0|0|0|0|1|1|||SBI,INET|
   //
   // 'k': GetMiddlewareInfoResponse (hasEndOfMsg = true)
   //    ....0|1|1|0|0|0|1|0|||1100|$$$
   //    ==> 0|1|1|0|0|0|1|0|||1100|$
   std::string strUnit;
   const char* beg = str;
   const char* end = NULL;

   if (hasEndOfMsg) {
      end = strchr(beg, TSmpMsg::EndOfMsg);
      if (!end) {
         return NULL;
      }
      strUnit.assign(beg, end-beg);
   }
   else {
      strUnit.assign(str);
   }

   TStrSplit   sp(strUnit.c_str(), TSmpMsg::Delimiter);
   std::string strTmp;

   Data_.InMsgFormat_           .assign(sp.GetStr(0));
   Data_.AppicationMessages_    .assign(sp.GetStr(1));
   Data_.SessionMessages_       .assign(sp.GetStr(2));
   Data_.MessageValidatedEvents_.assign(sp.GetStr(3));
   Data_.MessageCommitEvents_   .assign(sp.GetStr(4));
   Data_.MessageSentEvents_     .assign(sp.GetStr(5));
   Data_.SessionEvents_         .assign(sp.GetStr(6));
   Data_.GlobalEvents_          .assign(sp.GetStr(7));
   
   strTmp = sp.GetStr(8);
   if (!strTmp.empty()) {
      Data_.CheckRemote_.push_back(strTmp);
   }

   strTmp = sp.GetStr(9);
   if (!strTmp.empty()) {
      Data_.MessageTypes_.push_back(strTmp);
   }

   strTmp = sp.GetStr(10);
   if (!strTmp.empty()) {
      TStrSplit sp2(strTmp.c_str(), ',');
      size_t    size = sp2.GetSize();
      
      for (size_t i=0; i<size; ++i) {
         Data_.SessionIDs_.push_back(sp2.GetStr(i));
      }
   }

   if (end)
      return (end + 1);

   return end;
}


/////////////////////////////////////////////////////////////////////////////
// class TSmpLastMsgClientIDs impl
/////////////////////////////////////////////////////////////////////////////
K_mf(const char*) TSmpLastMsgClientIDs::Parse (const char* str)
{
   // "SEND41=ORD41A|SEND42=CANCL42B|SEND43=ORD43Z|SEND44=ORD44P$"
   const char* beg = str;
   const char* end = strchr(beg, TSmpMsg::EndOfMsg);
   
   if (!end) {
      return NULL;
   }

   std::string strUnit(beg, end-beg);
   TStrSplit   sp(strUnit.c_str(), TSmpMsg::Delimiter);
   size_t      size = sp.GetSize();
   
   for (size_t i=0; i<size; ++i) {
      std::string            strOne = sp.GetStr(i);
      std::string::size_type pos    = strOne.find('=');
         
      if (pos == std::string::npos)
         continue;

      SSmpLastMsgClientID one;
      
      one.SessionID_   = strOne.substr(0, pos);
      one.ClientMsgID_ = strOne.substr(pos+1);
      
      Data_.push_back(one);
   }

   return (end + 1);
}
