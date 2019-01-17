/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "ApaSesSmp.h"
#include <queue>
#include "Nsa_SysInfo.h"
#include "Nsa_UtilStr.h"
#include "Nsa_LineBuf.h"
#include "Nsa_DbgInfo.h"
#include "Nsa_UtilComm.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// class TApaSesSmpBase::TPktParse
/////////////////////////////////////////////////////////////////////////////
class TApaSesSmpBase::TPktParse
{
   TLineBufNew  LineBuf_;
   int          MsgDataLen_;

public:
   K_ctor TPktParse ();

   K_mf(void) Clear  ();
   K_mf(bool) GetMsg (std::string&);

   inline K_mf(void) Append (const void* buffer, size_t size)  { LineBuf_.Append(buffer, size); }

   inline K_mf(const char*) GetBuf    () const  { return LineBuf_.GetCStr();     }
   inline K_mf(size_t)      GetBufLen () const  { return LineBuf_.GetUsedSize(); }
};
/////////////////////////////////////////////////////////////////////////////
// class TApaSesSmpBase::TPktParse impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TApaSesSmpBase::TPktParse::TPktParse ()
   : LineBuf_   (1024)
   , MsgDataLen_(0)
{
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpBase::TPktParse::Clear ()
{
   LineBuf_.Clear();
   MsgDataLen_ = 0;
}
//---------------------------------------------------------------------------

K_mf(bool) TApaSesSmpBase::TPktParse::GetMsg (std::string& smpMsg)
{
   size_t                  usedSize;
   TLineBufBase::size_type pos;

   if (MsgDataLen_ == 0) {
      //===========================================
      // �N��O�s���ʥ]  "SOCKET2.0|0000000228|"
      // ��������� header ("SOCKET2.0")
      static const size_t smpVerLen = strlen(TSmpMsg::VersionInfo);
      
      pos = LineBuf_.FindStr(0, TSmpMsg::VersionInfo);
      if (pos == TLineBufBase::npos) {
         return false;
      }

      usedSize = LineBuf_.MoveFirst(pos);
      if (usedSize < TSmpMsg::HeaderLength) {
         return false;
      }
      //========================================
      // ��� header ��, �N�i�H�����פF
      pos = LineBuf_.FindChar(TSmpMsg::Delimiter, smpVerLen);
      if (pos == TLineBufBase::npos) {
         // ���׹�F, �o�䤣�� '|', ??? �L��
         return false;
      }
      MsgDataLen_ = LineBuf_.GetInt(pos+1);
      if (MsgDataLen_ == 0) {
         // �榡���k�X, ���׫o�� 0, ??? �L��
         return false;
      }
   }
   else {
      usedSize = LineBuf_.GetUsedSize();
   }

   //==============================================
   // �}�l��ʥ]�o
   size_t tolSize = (TSmpMsg::HeaderLength + MsgDataLen_);

   if (usedSize < tolSize) {
      return false;
   }

   smpMsg.assign(LineBuf_.GetCStr(), tolSize);

   if (usedSize == tolSize)  LineBuf_.Clear();
   else                      LineBuf_.MoveFirst(tolSize);

   MsgDataLen_ = 0;

   return true;
}


/////////////////////////////////////////////////////////////////////////////
// class TApaSesSmpBase impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TApaSesSmpBase::TApaSesSmpBase ()
   : inherited()
{
   PktParse_ = new TPktParse;
}
//---------------------------------------------------------------------------

K_dtor TApaSesSmpBase::~TApaSesSmpBase ()
{
   delete PktParse_;
}
//---------------------------------------------------------------------------

K_mf(std::string) TApaSesSmpBase::GetLogPath ()
{
   char filePath[256];

   sprintf(filePath, "./Logs/%s/", TSysInfo::GetTradeDateStr().c_str());
   return std::string(filePath);
}
//---------------------------------------------------------------------------

K_mf(std::string) TApaSesSmpBase::GetLogName ()
{
   char logName[256];
   
   sprintf(logName, "SesApaSmp_%s", Conf_.MiddlewareID_.c_str());
   return std::string(logName);
}
//---------------------------------------------------------------------------

K_mf(bool) TApaSesSmpBase::SettingApply (const std::string& str)
{
   std::string strCopy(str);

   PktFileHnd_.UseQueue(false);
   LogFileHnd_.UseQueue(false);
   
   //==============================================
   if (VirBfSettingApply(strCopy) == false)
      return false;

   //==============================================
   // [MiddlewareID],[ReliableMode]
   TStrSplit sp(strCopy.c_str(), ',');

   Conf_.MiddlewareID_ = sp.GetStr(0);
   Conf_.ReliableMode_.assign(sp.GetInt(1));

   //==============================================
   // �}�Ҭ�����
   char logMsg[256];
   int  logLen;

   sprintf(logMsg, "%s%s.log", GetLogPath().c_str(), GetLogName().c_str());
   LogFileHnd_.Reg(logMsg);

   logLen = sprintf(logMsg, "%c==============================================================================", 0x0a);
   LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);

   logLen = sprintf(logMsg, "SettingApply: %s", str.c_str());
   LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);

   //==============================================
   VirAfSettingApply(strCopy);

   return true;
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpBase::OnMesLinkReady (TMesBase* mes)
{
   PktParse_->Clear();

   char msgBuf[256];
   int  msgLen;

   //==============================================
   // �}�� PktFile
   if (!SesProp_.test(esp_Disable_PktLog)) {
      sprintf(msgBuf, "%s%s.pkt", GetLogPath().c_str(), GetLogName().c_str());
      PktFileHnd_.Reg(msgBuf);

      msgLen = sprintf(msgBuf, "%c------------------------------------------------------------------------------", 0x0a);
      PktFileHnd_.WriteMem(elt_Info, msgBuf, msgLen);
   }

   //==============================================
   // �}�� LogFile
   msgLen = sprintf(msgBuf, "%c------------------------------------------------------------------------------", 0x0a);
   LogFileHnd_.WriteMem(elt_Info, msgBuf, msgLen);
   
   msgLen = sprintf(msgBuf, "Link Ready! [%s]", mes->GetUID().c_str());
   LogFileHnd_.WriteMem(elt_Info, msgBuf, msgLen);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpBase::OnMesLinkBroken (TMesBase* mes, const std::string& reason)
{
   LogFileHnd_.WriteStr(elt_Info, "Link Broken! [%s] [%s]", mes->GetUID().c_str(), reason.c_str());

   PktFileHnd_.Unreg();
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpBase::OnMesLinkFail (TMesBase* mes, const std::string& reason)
{
   LogFileHnd_.WriteStr(elt_Error, "Link Fail! [%s] [%s]", mes->GetSetting().c_str(), reason.c_str());
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpBase::OnMesRecvReady (TMesBase* mes, size_t size)
{
   if (SesProp_.test(esp_Disable_RecvLog) == false) {
      LogFileHnd_.WriteStr(elt_MesIn, "OnMesRecvReady");
   }

   char buf[1024*20];
   int  bufLen;
   //char buf[300*10]; // �@���e�U�����פj�� 300, �C���B�z 10 ��
   //char buf[1024*4];
   int  rdsz = mes->Recv(buf, sizeof(buf));
   if (rdsz <= 0) {
      if (SesProp_.test(esp_Disable_RecvLog) == false) {
         int msgLen = sprintf(buf, "RecvLen[%d] <= 0", rdsz);
         LogFileHnd_.WriteMem(elt_Info, buf, msgLen);
      }

      return;
   }
   else {
   }

   if (!SesProp_.test(esp_Disable_PktLog))
      PktFileHnd_.WriteMem(elt_MesIn, buf, rdsz);

   PktParse_->Append(buf, rdsz);

   size_t cntProc = 0;
   // ���]���ɦ��ⵧ���ħ���q��
   // 1.SOCKET2.0 0000000047 r SessionInfo does not exist for T5U3U8U0UXU1U
   // 2.SOCKET2.0 0000000128 e 23051 T5U3U8U0UXU1U com.javtech.appia.protocols.SessionInfoRegistry$InvalidSessionException: T5U3U8U0UXU1U T5U3U8U0UXU1U
   //   End_Of_Restore, SequenceNumber[0]
   // ==> �B�z���Ĥ@���q���o�{���`,�|�I�s mes->CloseReason(),���ɬ�������N�|�Q�R��
   //     �p�G�b�~��B�z�ĤG���q�媺��,�N�|�y���{�����I
   //     �ҥH�o��n�P�_�s�u���A��
   while (true) {
      std::string strPkt;
      if (PktParse_->GetMsg(strPkt) == false)
         break;

      OnPktRecv(strPkt, NULL);
      ++cntProc;

      if (mes->IsLinkReady() == false) {
         size_t leftSize = PktParse_->GetBufLen();

         if (leftSize > 0) {
            bufLen = sprintf(buf, "<272> �B�z�ʥ]���e�ɵo�{�_�u�F,�Ѿl���[%u]�N���B�z�F!", leftSize);
            LogFileHnd_.WriteMem(elt_Info, buf, bufLen);

            LogFileHnd_.WriteMem(elt_Info, PktParse_->GetBuf(), leftSize);
         }
         
         break;
      }
   }

   //==============================================
   // �x�s�B�z�ƾ�
   if (SesProp_.test(esp_Disable_RecvLog) == false) {
      bufLen = sprintf(buf, "RecvLen[%d], BufSize[%d], ProcCnt[%d]", rdsz, sizeof(buf), cntProc);
      LogFileHnd_.WriteMem(elt_Info, buf, bufLen);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpBase::OnMesSendEmpty (TMesBase*)
{
   LogFileHnd_.WriteStr(elt_Info, "OnMesSendEmpty");
}
//---------------------------------------------------------------------------

K_mf(int) TApaSesSmpBase::Send (const void* buffer, size_t size)
{
   TMesBase* mes = inherited::GetMes();

   if (mes == NULL || mes->IsLinkReady() == false) {
      TLogMsg logMsg;

      logMsg.PutMem(elt_Send, "Send Error! ", 12, NULL, false);
      logMsg.Append(buffer, size, true);
      LogFileHnd_.WriteMsg(logMsg);

      return 0;
   }

   return mes->Send(buffer, size);
}


/////////////////////////////////////////////////////////////////////////////
// class TApaSesSmp impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TApaSesSmp::TApaSesSmp ()
   : inherited()
{
}
//---------------------------------------------------------------------------

K_dtor TApaSesSmp::~TApaSesSmp ()
{
   ReopenTimer_.reset();
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::OnMesLinkReady (Kway::TMesBase* mes)
{
   inherited::OnMesLinkReady(mes);

   //==============================================
   // �Ѽƪ�l��
   ReopenTimer_.reset();
   LastState_  .clear();

   SmpLogon();
   ShowState();

   SesTimer_.reset(new TTimerType(this));
   SesTimer_->Start(1000, &this_type::OnTimer);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::OnMesLinkBroken (Kway::TMesBase* mes, const std::string& str)
{
   inherited::OnMesLinkBroken(mes, str);

   ApaMwInfo_.Status_ = ams_LinkError;

   SesTimer_.reset();
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::OnMesLinkFail (Kway::TMesBase* mes, const std::string& str)
{
   inherited::OnMesLinkFail(mes, str);

   ApaMwInfo_.Status_ = ams_LinkError;
}
//---------------------------------------------------------------------------
K_mf(void) TApaSesSmp::OnPktRecv (const std::string& strPkt, const TKDateTimeMS* dtms)
{
   const char* cpMsg = strPkt.c_str();

   //==============================================
   // ���s�p��
   if (ApaMwInfo_.TmChkHb_.IsEnable())
      ApaMwInfo_.TmChkHb_.SetTimeout(ApaMwInfo_.ItvlBfSendHb_, true);

   //==============================================
   // �b�g[����]�ʥ]�� log �ɥ�����
   if (!inherited::SesProp_.test(esp_Disable_RecvLog)) {
      char   buf[1024*20];
      size_t len = strPkt.length();
   
      buf[0] = 0x0a;
      memcpy(buf+1, cpMsg, len);
      len += 1;
      buf[len] = '\0';

      LogFileHnd_.WriteMem(elt_Recv, buf, len, dtms);
   }

   //==============================================
   // �}�l�ѫʥ]

   TSmpMsg    msg;
   TSmpMsgRef msgRef = msg.Unpack(cpMsg);
   TSmpMsg*   msgPtr = msgRef.get();

   if (msgPtr == NULL) {
      return;
   }

   //==============================================
   // �����T��
   const TSmpMsgType& msgType = msg.GetMsgType();

   switch (msgType) {
   case smt_MiddlewareEvent:
      {
         DoSmpMwEv(msgRef);
      }
      break;

   case smt_Reject:
      {
         TSmpMsgReject* msgExt = static_cast<TSmpMsgReject*>(msgPtr);
         DoSmpReject(msgExt);
      }
      break;
      
   case smt_OperatorCommandResponse:
      {
         TSmpMsgOperatorCommandResponse*       msgExt = static_cast<TSmpMsgOperatorCommandResponse*>(msgPtr);
         const SSmpMsgOperatorCommandResponse& data   = msgExt->GetData();
         
         //========================================
         // �n�����\���P�_�̾�
         if (ApaMwInfo_.Status_ == ams_RequestLogon && data.SmpOperCmd_.GetSmpCmd().get_orig() == esc_Version) {
            DoSmpLogon();
         }
         else {
            OnSmpOperCmdResponse(msgExt);
         }
      }
      break;

   case smt_GetMiddlewareInfoResponse:
      {
         TSmpMsgGetMiddlewareInfoResponse* msgExt = static_cast<TSmpMsgGetMiddlewareInfoResponse*>(msgPtr);
         OnSmpGetMiddlewareInfoResponse(msgExt);
      }
      break;

   case smt_GetMiddlewareConfigResponse:
      {
         TSmpMsgGetMiddlewareConfigResponse* msgExt = static_cast<TSmpMsgGetMiddlewareConfigResponse*>(msgPtr);
         OnSmpGetMiddlewareConfigResponse(msgExt);
      }
      break;
      
   case smt_GetSessionListResponse:
      {
         TSmpMsgGetSessionListResponse* msgExt = static_cast<TSmpMsgGetSessionListResponse*>(msgPtr);
         OnSmpGetSessionListResponse(msgExt);
      }
      break;
      
   case smt_GetSessionInfoResponse:
      {
         TSmpMsgGetSessionInfoResponse* msgExt = static_cast<TSmpMsgGetSessionInfoResponse*>(msgPtr);
         OnSmpGetSessionInfoResponse(msgExt);
      }
      break;
   }
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::SendMsg (TSmpMsg& msg)
{
   std::string strData;
   int         size = msg.Pack(strData);
   const char* ptr  = strData.c_str();

   if (!inherited::SesProp_.test(esp_Disable_SendLog)) {
      //TLogMsg logMsg(2048);
      //logMsg.PutMem(elt_Send, "==> ", 4, NULL, false);
      //logMsg.Append(ptr, size, true);
      //LogFileHnd_.WriteMsg(logMsg);
      char logBuf[5120];

      strcpy(logBuf, "==> ");
      memcpy(logBuf+4, ptr, size);
      LogFileHnd_.WriteMem(elt_Send, logBuf, size+4);
   }

   // debug {
   /*
   if (msg.GetMsgType().get_orig() == smt_OutboundMessage) {
      TSmpMsgOutboundMessage& smpMsg = static_cast<TSmpMsgOutboundMessage&>(msg);
      SSmpMsgOutboundMessage& data   = smpMsg.GetData();
      
      if (data.MessageType_[0] != 'A')
         return;
   }
   */
   // } debug

   inherited::Send(ptr, size);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::SendCmd (const TSmpOperCmd& operCmd)
{
   TSmpMsgOperatorCommandRequest smpMsg(operCmd.Pack());
   SendMsg(smpMsg);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::SmpLogon ()
{
   ApaMwInfo_.Status_ = ams_RequestLogon;

   //==============================================
   // �ǰe logon ���O
   TSmpMsgLogon msg(Conf_.MiddlewareID_, Conf_.ReliableMode_.get_orig());

   SendMsg(msg);

   //==============================================
   // �ǰe cmd:version ���O
   // �]���e�X logon ���O��èS������^��
   // �ҥH�[�e�@�� cmd ��Ĳ�o logon ���ƥ�
   TSmpOperCmd operCmd(esc_Version);
   SendCmd(operCmd);

   ApaMwInfo_.TmChkLogon_.SetTimeout(ApaMwInfo_.LogonInterval_);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::SmpStartMiddlwareEvents ()
{
   LogFileHnd_.WriteStr(elt_Info, "SMP: StartMiddlwareEvents");

   TSmpMsgStartMiddlwareEvents msg;
   SendMsg(msg);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::SmpGetSessionList ()
{
   TSmpMsgGetSessionListRequest smpReq;
   
   SendMsg(smpReq);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::SmpGetMiddlewareConfig  ()
{
   TSmpMsgGetMiddlewareConfigRequest smpReq;
   
   SendMsg(smpReq);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::SmpRestoreMessage (const TSmpString& sesID, const TSmpInt& begNo, const TSmpInt& endNo)
{
   TSmpMsgRestoreMessageRequest smpReq(sesID, begNo, endNo);
   SendMsg(smpReq);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::SmpOutboundMessageDelivery (const TSmpString& clientMsgID, const TSmpString& sessionID,
                                                   const TSmpString& messageType, const TSmpString& messageData)
{
   TSmpMsgOutboundMessage  smpReq;
   SSmpMsgOutboundMessage& data = smpReq.GetData();
   
   data.ClientMsgID_ = clientMsgID;
   data.SessionID_   = sessionID;
   data.MessageType_ = messageType;
   data.MessageData_ = messageData;
   
   SendMsg(smpReq);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::SmpGetMiddlewareInfoRequest ()
{
   TSmpMsgGetMiddlewareInfoRequest smpReq(Conf_.MiddlewareID_);
   SendMsg(smpReq);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::SmpGetSessionInfoRequest (const TSmpString& sessionID)
{
   TSmpMsgGetSessionInfoRequest smpReq(sessionID);
   SendMsg(smpReq);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::SmpSetMiddlewareConfigRequest (const SSmpMiddlewareConfig& mwConf)
{
   LogFileHnd_.WriteStr(elt_Info, "SMP: SetMiddlewareConfigRequest");

   TSmpSetMiddlewareConfigRequest smpReq(mwConf);
   SendMsg(smpReq);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::DoSmpLogon ()
{
   //==============================================
   // �ˬd�O�_�n�Ұ�[�߸��]]����
   // ==> �b OnPktRecv �� TmChkHb_ ���|�ҥ� SetTimeout, �ҥH�o��i�H�K�F

   ApaMwInfo_.Status_ = ams_Logon;
   ShowState();

   OnSmpLogon();

   //SmpStartMiddlwareEvents();
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::DoSmpReject (TSmpMsgReject* smpMsg)
{
   OnSmpReject(smpMsg);

   const SSmpMsgReject& data      = smpMsg->GetData();
   ESmpRejectReason     rejReason = data.RejReason_.get_orig();

   if (rejReason == srr_NotLogon) {
      std::string reason("Appia Middleware Not Logon!");

      ApaMwInfo_.Status_ = ams_NotLogon;

      ShowState();
      
      Reopen(reason, ApaMwInfo_.ReopenItvl_);
   }
   else if (rejReason == srr_AlreadyLogon) {
      OnSmpReject_AlreadyLogon();
   }
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::OnSmpReject_AlreadyLogon ()
{
   std::string reason("Appia Middleware Already Logon!");

   ApaMwInfo_.Status_ = ams_AlreadyLogon;

   ShowState();
   Reopen(reason, ApaMwInfo_.ReopenItvl_);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::Reopen (const std::string& reason, int itvlSec)
{
   TMesBase* mes = inherited::GetMes();
   if (mes && mes->IsLinkReady())
      mes->CloseReason(reason);

   ReopenTimer_.reset(new TTimerType(this));
   ReopenTimer_->Start(itvlSec*1000, &this_type::OnTimer);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::OnTimer (TTimerBase* timerBase)
{
   if (timerBase == SesTimer_.get()) {
      //===========================================
      // �ˬd�O�_�w�g�n�J���\�F
      if (ApaMwInfo_.Status_ == ams_RequestLogon) {
         if (ApaMwInfo_.TmChkLogon_.IsTimeout()) {
            // �n�J�O��
            Reopen(std::string("Appia Middleware Logon Timeout"), ApaMwInfo_.ReopenItvl_);
            return;
         }
      }
      //===========================================
      // �ˬd�߸��]���A
      else if (ApaMwInfo_.Status_ == ams_Logon) {
         if (CheckHeartbeat() == false)
            return;
      }

      ShowState();
   }
   else if (timerBase == ReopenTimer_.get()) {
      ReopenTimer_.reset();
      
      inherited::Open();
   }
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::DoSmpMwEv (TSmpMsgRef msgRef)
{
         TSmpMsgMiddlewareEvent* msg  = static_cast<TSmpMsgMiddlewareEvent*>(msgRef.get());
   const SSmpMsgMiddlewareEvent& data = msg->GetData();

   if (data.EventType_ == set_ApplicationeMessageReceived) {
      //return;
      DoSmpMwEv_AppMsgRecv(msg);
   }
   else if (data.EventType_ == set_SessionMessageReceived) {
      DoSmpMwEv_SesMsgRecv(msgRef);
   }
   else if (data.EventType_ == set_IncomingMessageRestored) {
      OnSmpMwEv_IncomingMessageRestored(msg);
   }
   else if (data.EventType_ == set_EndOfRestore) {
      char msgBuf[128];
      int  msgLen = sprintf(msgBuf, "%s, SequenceNumber[%d]",
                                       data.EventType_     .as_string(false).c_str(),
                                       data.SequenceNumber_.get_orig());

      LogFileHnd_.WriteMem(elt_Info, msgBuf, msgLen);
      
      OnSmpMwEv_EndOfRestore(msg);
   }
   else if (data.EventType_.IsSessionLevelEvents()) {
      LogFileHnd_.WriteStr(elt_Info, "%s", data.EventType_.as_string(false).c_str());

      OnSmpMwEv_SessionEvents(msg);
   }
   else if (data.EventType_.IsOutboundAcknowledgmentEvents()) {
      LogFileHnd_.WriteStr(elt_Info, "%s", data.EventType_.as_string(false).c_str());

      OnSmpMwEv_OutboundAcknowledgmentEvents(msg);
   }
   // 23011.Message_Validation_Error
   else if (data.EventType_ == set_MessageValidationError) {
      LogFileHnd_.WriteStr(elt_Info, "23011.Message_Validation_Error");

      OnSmpMwEv_MessageValidationError(msg);
   }
   else {
      LogFileHnd_.WriteStr(elt_Warning, "<TApaSesSmp::DoSmpMwEv> ���B�z���ƥ�N��[%s]",
                                             data.EventType_.as_string(true).c_str());
   }
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::DoSmpMwEv_AppMsgRecv (TSmpMsgMiddlewareEvent* msg)
{
   const SSmpMsgMiddlewareEvent& data = msg->GetData();

   //if (Status_.AppMsgRecvSeqNum_ < data.SequenceNumber_)  Status_.AppMsgRecvSeqNum_ = data.SequenceNumber_;
   //Status_.AppMsgRecvCount_++;

   OnSmpMwEv_AppMsgRecv(msg);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::DoSmpMwEv_SesMsgRecv (TSmpMsgRef msgRef)
{
   //const SSmpMsgMiddlewareEvent& data = msg->GetData();
   //if (Status_.AppMsgRecvSeqNum_ < data.SequenceNumber_)  Status_.AppMsgRecvSeqNum_ = data.SequenceNumber_;
   //Status_.AppMsgRecvCount_++;

   OnSmpMwEv_SesMsgRecv(msgRef);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmp::ShowState ()
{
   std::string state("MW:");
   char        apaMw[50];
   
   switch (ApaMwInfo_.Status_) {
      case ams_RequestLogon:  strcpy(apaMw, "RequestLogon");  break;
      case ams_Logon:         strcpy(apaMw, "Logon");         break;
      case ams_NotLogon:      strcpy(apaMw, "NotLogon");      break;
      case ams_AlreadyLogon:  strcpy(apaMw, "AlreadyLogon");  break;
      default:                apaMw[0] = '\0';                break;
   }

   state.append(apaMw);
   VirOnShowState(state);

   if (LastState_ != state) {
      LastState_ = state;
      inherited::OnStateChanged(LastState_);
   }
}
//---------------------------------------------------------------------------

K_mf(bool) TApaSesSmp::CheckHeartbeat ()
{
   if (ApaMwInfo_.TmChkHb_.IsTimeout() == false)
      return true;

   // TimeoutCount: 1 ==> �e�X�߸��]
   // TimeoutCount: 2 ==> �e�X�߸��]�L�^��
   if (ApaMwInfo_.TmChkHb_.GetTimeoutCount() > 1) {
      // �N��e�X[Heartbeat]�ᤴ�M�S���^��
      // �_�u�ᰨ�W���s
      Reopen(std::string("AppiaMwSes: Heartbeat Timeout!"), 0);

      return false;
   }
   else {
      // �N��ǳƲĤ@���e�X[Heartbeat]
      // �ثe�ϥ� Cmd[Version] ��@�߸��]
      TSmpOperCmd operCmd(esc_Version);
      SendCmd(operCmd);

      ApaMwInfo_.TmChkHb_.SetTimeout(ApaMwInfo_.ItvlAfSendHb_, false);
   }

   return true;
}
//---------------------------------------------------------------------------
