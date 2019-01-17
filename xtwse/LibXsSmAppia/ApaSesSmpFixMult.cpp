/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "ApaSesSmpFixMult.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// class TApaSesSmpFixMult impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TApaSesSmpFixMult::TApaSesSmpFixMult ()
   : inherited     ()
   , EnableRecover_(true)
{
}
//---------------------------------------------------------------------------

K_dtor TApaSesSmpFixMult::~TApaSesSmpFixMult ()
{
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpFixMult::OnMesLinkBroken (TMesBase* mes, const std::string& reason)
{
   inherited::OnMesLinkBroken(mes, reason);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpFixMult::OnSmpGetMiddlewareInfoResponse (TSmpMsgGetMiddlewareInfoResponse* smpMsg)
{
   //const SSmpMsgGetMiddlewareInfoResponse& data   = smpMsg->GetData();
   //const SSmpMiddlewareConfig&             mwConf = data.MiddlewareConfig_.GetData();
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpFixMult::OnSmpMwEv_SesMsgRecv (TSmpMsgRef smpMsgRef)
{
         TSmpMsgMiddlewareEvent* smpMsg = static_cast<TSmpMsgMiddlewareEvent*>(smpMsgRef.get());
   const SSmpMsgMiddlewareEvent& data   = smpMsg->GetData();
         TMessageRef             msgRef(new Message);
         TFixMsgType2            msgType = inherited::GetFixMsgType(data, msgRef, "OnSmpMwEv_SesMsgRecv");

   if (msgType.is_invalid())
      return;

   if (EnableRecover_) {
      //TSmpInt seqNo(data.SequenceNumber_);

      //if (Recover_->CheckRt(seqNo, msgType, msgRef) == false)
      //   return;
   }

   SDispFixMsgInfo dfmInfo(data.SessionID_, msgType, msgRef, true, smpMsgRef);
   DispFixMessage(dfmInfo);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpFixMult::OnSmpMwEv_AppMsgRecv (TSmpMsgMiddlewareEvent* smpMsg)
{
   const SSmpMsgMiddlewareEvent& data = smpMsg->GetData();
   TMessageRef                   msgRef(new Message);
   TFixMsgType2                  msgType = inherited::GetFixMsgType(data, msgRef, "OnSmpMwEv_AppMsgRecv");

   if (msgType.is_invalid())
      return;

   if (EnableRecover_) {
      //TSmpInt seqNo(data.SequenceNumber_);
      
      //if (Recover_->CheckRt(seqNo, msgType, msgRef) == false)
      //   return;
   }

   SDispFixMsgInfo dfmInfo(data.SessionID_, msgType, msgRef, true, TSmpMsgRef());
   DispFixMessage(dfmInfo);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpFixMult::OnSmpMwEv_IncomingMessageRestored (TSmpMsgMiddlewareEvent* smpMsg)
{
   const SSmpMsgMiddlewareEvent& data = smpMsg->GetData();
   TMessageRef                   msgRef(new Message);
   TFixMsgType2                  msgType = inherited::GetFixMsgType(data, msgRef, "OnSmpMwEv_IncomingMessageRestored");

   if (msgType.is_invalid())
      return;

   if (EnableRecover_) {
      //TSmpInt seqNo(data.SequenceNumber_);

      //if (Recover_->CheckRcv(seqNo, msgType, msgRef) == false)
      //   return;
   }

   SDispFixMsgInfo dfmInfo(data.SessionID_, msgType, msgRef, false, TSmpMsgRef());
   DispFixMessage(dfmInfo);
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpFixMult::OnSmpMwEv_EndOfRestore (TSmpMsgMiddlewareEvent* smpMsg)
{
   if (EnableRecover_) {
      //const SSmpMsgMiddlewareEvent& data = smpMsg->GetData();
      //      TSmpInt                 seqNo(data.SequenceNumber_);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TApaSesSmpFixMult::DispFixMessage (SDispFixMsgInfo& dfmInfo)
{
   const TSmpString&   sesID   = dfmInfo.SesID_;
   const TFixMsgType2& msgType = dfmInfo.MsgType_;
         TMessageRef&  msgRef  = dfmInfo.FixMsgRef_;
         bool          isRt    = dfmInfo.IsRt_;

   if (msgType.is_app()) {
           if (msgType == MsgType_NewOrderSingle)             OnFixNewOrderSingle           (sesID, msgRef, isRt);
      else if (msgType == MsgType_OrderCancelRequest)         OnFixOrderCancelRequest       (sesID, msgRef, isRt);
      else if (msgType == MsgType_OrderCancelReplaceRequest)  OnFixOrderCancelReplaceRequest(sesID, msgRef, isRt);
      else if (msgType == MsgType_OrderStatusRequest)         OnFixOrderStatusRequest       (sesID, msgRef, isRt);
      else                                                    OnFixAppOther                 (sesID, msgRef, isRt);
   }
   else {
           if (msgType == MsgType_Heartbeat)      OnFixHeartbeat    (sesID, msgRef, isRt);
      else if (msgType == MsgType_Logon)          OnFixLogon        (sesID, msgRef, isRt, dfmInfo.SmpMsgRef_);
      else if (msgType == MsgType_Logout)         OnFixLogout       (sesID, msgRef, isRt);
      else if (msgType == MsgType_ResendRequest)  OnFixResendRequest(sesID, msgRef, isRt);
      else if (msgType == MsgType_SequenceReset)  OnFixSequenceReset(sesID, msgRef, isRt);
   }
}
//---------------------------------------------------------------------------
