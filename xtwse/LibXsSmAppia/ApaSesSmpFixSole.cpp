/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "ApaSesSmpFixSole.h"
#include "Nsa_UtilComm.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// class TApaSesSmpFixSole impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TApaSesSmpFixSole::TApaSesSmpFixSole ()
   : inherited()
   , Recover_ (NULL)
{
}
//---------------------------------------------------------------------------

K_dtor TApaSesSmpFixSole::~TApaSesSmpFixSole ()
{
   if (Recover_)
      delete Recover_;
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpFixSole::OnMesLinkReady (TMesBase* mes)
{
   inherited::OnMesLinkReady(mes);

   SoleFlag_.reset();
   FixVer_ = 0;
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpFixSole::OnMesLinkBroken (TMesBase* mes, const std::string& reason)
{
   inherited::OnMesLinkBroken(mes, reason);
      
   if (Recover_) {
      delete Recover_;
      Recover_ = NULL;
   }
   FixVer_ = 0;
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpFixSole::SmpOutboundMessageDelivery (int cliMsgID, const TSmpString& msgType, const TSmpString& msgData,
                                                          const TSmpString* sessionID)
{
   if (!sessionID)
      sessionID = &ApaSesID_;

   if (cliMsgID <= 0) {
      inherited::SmpOutboundMessageDelivery(TSmpString(), *sessionID, msgType, msgData);
   }
   else {
      char buf[50];

      sprintf(buf, "%d", cliMsgID);
      inherited::SmpOutboundMessageDelivery(TSmpString(buf), *sessionID, msgType, msgData);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpFixSole::OnSmpGetMiddlewareConfigResponse (TSmpMsgGetMiddlewareConfigResponse* smpMsg)
{
   const SSmpMiddlewareConfig& mwConf = smpMsg->GetData();

   //==============================================
   // 取得 SessionID
   if (SoleMode_.test(esm_UseMySessionID) == false) {
      ApaSesID_.clear();
      
      if (!mwConf.SessionIDs_.empty())
         ApaSesID_ = mwConf.SessionIDs_[0];
   }
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpFixSole::OnSmpGetMiddlewareInfoResponse (TSmpMsgGetMiddlewareInfoResponse* smpMsg)
{
   const SSmpMsgGetMiddlewareInfoResponse& data = smpMsg->GetData();

   //==============================================
   // 建立回補元件, 並看看可不可以在封包裡面找到 NewSeqNum
   const SSmpInboundQueueInfoA& ibqis = data.InboundQueueInfo_.GetData();
         bool                   found = false;
   
   Recover_ = new TApaFixRecover(this, ApaSesID_, FixTargetCompID_);
   
   for (SSmpInboundQueueInfoACi iter=ibqis.begin(); iter!=ibqis.end(); ++iter) {
      if (ApaSesID_ == iter->SessionID_) {
         found = true;
         Recover_->CheckFirst(iter->NewSeqNum_);
         break;
      }
   }

   if (!found) {
      Recover_->CheckFirst(TSmpInt(-1));
   }
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpFixSole::OnSmpMwEv_SesMsgRecv (TSmpMsgRef smpMsgRef)
{
         TSmpMsgMiddlewareEvent* smpMsg = static_cast<TSmpMsgMiddlewareEvent*>(smpMsgRef.get());
   const SSmpMsgMiddlewareEvent& data   = smpMsg->GetData();
         TMessageRef             msgRef(new Message);
         TFixMsgType2            msgType = inherited::GetFixMsgType(data, msgRef, "OnSmpMwEv_SesMsgRecv");

   if (msgType.is_invalid())
      return;

   TSmpInt seqNo(data.SequenceNumber_);
   // 有發現在 Logon 時會發生 SequenceNumber 為 0 的狀態
   // 所以自己動手來取囉
   if (seqNo.is_0()) {
      Header&   header = msgRef->getHeader();
      MsgSeqNum qfMsgSeqNum;

      if (header.findField(qfMsgSeqNum)) {
         seqNo.assign(qfMsgSeqNum.getValue());
      }
   }

   if (Recover_->CheckRt(seqNo, msgType, msgRef) == false) {
      // 如果是 logon 訊息時, 應該可以不用排隊了
      if (msgType != MsgType_Logon)
         return;
   }

   SDispFixMsgInfo dfmInfo(data.SessionID_, msgType, msgRef, true, smpMsgRef);
   DispFixMessage(dfmInfo);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpFixSole::OnSmpMwEv_AppMsgRecv (TSmpMsgMiddlewareEvent* smpMsg)
{
   const SSmpMsgMiddlewareEvent& data = smpMsg->GetData();
   TMessageRef                   msgRef(new Message);
   TFixMsgType2                  msgType = inherited::GetFixMsgType(data, msgRef, "OnSmpMwEv_AppMsgRecv");

   if (msgType.is_invalid())
      return;

   TSmpInt seqNo(data.SequenceNumber_);

   if (Recover_->CheckRt(seqNo, msgType, msgRef) == false)
      return;

   SDispFixMsgInfo dfmInfo(data.SessionID_, msgType, msgRef, true, TSmpMsgRef());
   DispFixMessage(dfmInfo);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpFixSole::OnSmpMwEv_IncomingMessageRestored (TSmpMsgMiddlewareEvent* smpMsg)
{
   const SSmpMsgMiddlewareEvent& data = smpMsg->GetData();
   TMessageRef                   msgRef(new Message);
   TFixMsgType2                  msgType = inherited::GetFixMsgType(data, msgRef, "OnSmpMwEv_IncomingMessageRestored");

   if (msgType.is_invalid())
      return;

   TSmpInt seqNo(data.SequenceNumber_);

   if (Recover_->CheckRcv(seqNo, msgType, msgRef) == false)
      return;

   SDispFixMsgInfo dfmInfo(data.SessionID_, msgType, msgRef, false, TSmpMsgRef());
   DispFixMessage(dfmInfo);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpFixSole::OnSmpMwEv_EndOfRestore (TSmpMsgMiddlewareEvent* smpMsg)
{
   const SSmpMsgMiddlewareEvent& data = smpMsg->GetData();
         TSmpInt                 seqNo(data.SequenceNumber_);

   Recover_->CheckRcvEnd(seqNo);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpFixSole::OnSmpMwEv_SessionEvents (TSmpMsgMiddlewareEvent* smpMsg)
{
   const SSmpMsgMiddlewareEvent& data = smpMsg->GetData();

   if (data.EventType_ == set_SessionEndOfDay) {
      Recover_->Init();
   }
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpFixSole::DispFixMessage (SDispFixMsgInfo& dfmInfo)
{
   const TSmpString&   sesID   = dfmInfo.SesID_;
   const TFixMsgType2& msgType = dfmInfo.MsgType_;
         TMessageRef&  msgRef  = dfmInfo.FixMsgRef_;
         bool          isRt    = dfmInfo.IsRt_;

   if (SoleMode_.test(esm_UseMySessionID) == false) {
      if (ApaSesID_ != sesID)
         return;
   }

   // 檢查是否已知道 Fix 版本
   if (FixVer_ == 0) {
      FixVer_ = msgRef->getVersion();
      OnGetFixVer();
   }

   if (msgType.is_app()) {
           if (msgType == MsgType_NewOrderSingle)             OnFixNewOrderSingle           (msgRef, isRt);
      else if (msgType == MsgType_OrderCancelRequest)         OnFixOrderCancelRequest       (msgRef, isRt);
      else if (msgType == MsgType_OrderCancelReplaceRequest)  OnFixOrderCancelReplaceRequest(msgRef, isRt);
      else if (msgType == MsgType_OrderStatusRequest)         OnFixOrderStatusRequest       (msgRef, isRt);
      else                                                    OnFixAppOther                 (msgType, msgRef, isRt);
   }
   else {
           if (msgType == MsgType_Heartbeat)      OnFixHeartbeat    (msgRef, isRt);
      else if (msgType == MsgType_Logon)          OnFixLogon        (msgRef, isRt);
      else if (msgType == MsgType_Logout)         OnFixLogout       (msgRef, isRt);
      else if (msgType == MsgType_ResendRequest)  OnFixResendRequest(msgRef, isRt);
      else if (msgType == MsgType_SequenceReset)  OnFixSequenceReset(msgRef, isRt);
   }
}
//---------------------------------------------------------------------------
