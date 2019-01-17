/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FixStkOrdFlowReq.h"
#include "QFixExt/QFixUtil.h"
#include "FixStkRes.h"
#include "FixStkOrdFlow.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;
using namespace Kway::FIX;
using namespace Kway::FIX::Stk;

/////////////////////////////////////////////////////////////////////////////
// class TOfReqBase impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TOfReqBase::TOfReqBase (EFixOrdCmdID oid)
   : OCmd_       (oid)
   , OrdFlow_    (NULL)
   , InitRejCode_(wrc_None)
{
}
//---------------------------------------------------------------------------

K_ctor TOfReqBase::TOfReqBase (EFixOrdCmdID oid, const char* msgType, const TMessageRef& msgRef)
   : OCmd_       (oid)
   , FixMsgType_ (msgType)
   , FixMsgRef_  (msgRef)
   , OrdFlow_    (NULL)
   , InitRejCode_(wrc_None)
{
   const Header&     fixHdr = msgRef->getHeader();
         SendingTime sendingTime;

   if (fixHdr.findField(sendingTime)) {
      TQFixUtil::UtcTimeStamp_To_KDateTimeMS(sendingTime, SendingTime_);
   }
}
//---------------------------------------------------------------------------

K_dtor TOfReqBase::~TOfReqBase ()
{
}
//---------------------------------------------------------------------------

K_mf(void) TOfReqBase::Update (const std::string& msg, EFixOrdFlowID flowID)
{
   LastMsg_.assign(msg);
   FlowID_.assignT(flowID);
   
   //==============================================
   // 異動 FixStkOrder
   if (!OrdHnd_.is_invalid()) {
      TFsStkOrder& ord = OrdHnd_.GetModify();

      if (!msg.empty())
         ord.LastMsg_.assign(msg);
      
      ord.FlowID_ = FlowID_;
   }

   //==============================================
   // 異動 FixOrdTrack
   SOrdTrack* otrack = GetOrdTrack();

   otrack->FlowID_ = FlowID_;

   //==============================================
   // 檢查 OrdTrack 是否已經新增
   //if (ReqState_.test(ers_OrdTrackAdd) == false) {
   //   TFixSysOrdID sid      = GetSysOrdID();
   //   TOrdTrack*   ordTrack = OrdFlow_->GetOrdTrack();
   //   
   //   ordTrack->AddTrack(sid, OTrack_);
   //   ReqState_.set(ers_OrdTrackAdd);
   //}

   //==============================================
   // 異動 OrdHnd
   OrdHnd_.FlushUpdate(fuf_OrdFlow);
}


/////////////////////////////////////////////////////////////////////////////
// class TOfReqOldBase impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TOfReqOldBase::TOfReqOldBase (EFixOrdCmdID cid, const char* msgType, const TMessageRef& msgRef)
   : inherited   (cid, msgType, msgRef)
   , WorkRejCode_(wrc_None)
{
}


/////////////////////////////////////////////////////////////////////////////
// class TOfReqOcrBase impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TOfReqOcrBase::TOfReqOcrBase (const TMessageRef& msgRef)
   : inherited(fcid_Del, MsgType_OrderCancelRequest, msgRef)
{
}
//---------------------------------------------------------------------------

K_mf(void) TOfReqOcrBase::Update (const std::string& msg, EFixOrdFlowID flowID)
{
   inherited::Update(msg, flowID);

   /*
   if (flowID == of_Sent) {
      //===========================================
      // 發送 Pending Cancel 回報
      TSysOrdID                sysOrdID    = inherited::GetSysOrdID();
      TRptFlow::TReqErOrd      reqErOrd    = TRptFlow::CreateReqErOrd(sysOrdID, OCmd_.get_orig());
      TRptFlow::TReqErOrdBase* refErOrd    = reqErOrd.get();
      SRfBaseField*            rfBaseField = refErOrd->GetBaseField();
      SRfErField*              rfErField   = refErOrd->GetErField  ();
      const TStkOrder*         stkOrd      = inherited::OrdHnd_.GetConst();

      rfBaseField->OrdStatus_.assignT(OrdStatus_PENDING_CANCEL);
      rfErField  ->ExecType_ .assignT(ExecType_PENDING_CANCEL);
      
      rfErField->LeavesQty_ = stkOrd->LeavesQty_;

      RptFlow_->DoReq(reqErOrd);
   }
   */
}


/////////////////////////////////////////////////////////////////////////////
// class TOfReqOcrrBase impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TOfReqOcrrBase::TOfReqOcrrBase (const TMessageRef& msgRef)
   : inherited(fcid_ChgPriQty, MsgType_OrderCancelReplaceRequest, msgRef)
{
}
//---------------------------------------------------------------------------

K_mf(void) TOfReqOcrrBase::Update (const std::string& msg, EFixOrdFlowID flowID)
{
   inherited::Update(msg, flowID);
   /*
   if (flowID == of_Sent) {
      //===========================================
      // 發送 Pending Replace 回報
      TSysOrdID                sysOrdID    = inherited::GetSysOrdID();
      TRptFlow::TReqErOrd      reqErOrd    = TRptFlow::CreateReqErOrd(sysOrdID, OCmd_.get_orig());
      TRptFlow::TReqErOrdBase* refErOrd    = reqErOrd.get();
      SRfBaseField*            rfBaseField = refErOrd->GetBaseField();
      SRfErField*              rfErField   = refErOrd->GetErField  ();
      const TStkOrder*         stkOrd      = inherited::OrdHnd_.GetConst();

      rfBaseField->OrdStatus_.assignT(OrdStatus_PENDING_REPLACE);
      rfErField  ->ExecType_ .assignT(ExecType_PENDING_REPLACE);
      
      rfErField->LeavesQty_ = stkOrd->LeavesQty_;

      RptFlow_->DoReq(reqErOrd);
   }
   */
}


/////////////////////////////////////////////////////////////////////////////
// class TOfReqOsrBase::TOfReqOsrBase impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TOfReqOsrBase::TOfReqOsrBase (const TMessageRef& msgRef)
   : inherited(fcid_Qry, MsgType_OrderStatusRequest, msgRef)
{
}
//---------------------------------------------------------------------------

K_mf(void) TOfReqOsrBase::Update (const std::string& msg, EFixOrdFlowID flowID)
{
   inherited::Update(msg, flowID);
}


/////////////////////////////////////////////////////////////////////////////
// class TOfReqAnoBase impl  (Auto New Order)
/////////////////////////////////////////////////////////////////////////////
K_ctor TOfReqAnoBase::TOfReqAnoBase (const TMessageRef& msgRef, TFixRefOrdID roid, bool isValidOrd)
   : inherited  (fcid_AutoNew, MsgType_Null, msgRef)
   , RefOrdID_  (roid)
   , IsValidOrd_(isValidOrd)
{
}
//---------------------------------------------------------------------------

K_mf(void) TOfReqAnoBase::Update (const std::string& msg, EFixOrdFlowID flowID)
{
   TFsStkOrder& ord = inherited::OrdHnd_.GetModify();

   if (flowID == fof_Finish) {
      // 我是自動新增的,應該沒辦法決定這些狀態
      //ord.ExecType_ .assignT(ExecType_NEW);
      //ord.OrdStatus_.assignT(OrdStatus_NEW);
   }
   else {
      ord.ExecType_ .assignT(ExecType_REJECTED);
      ord.OrdStatus_.assignT(OrdStatus_REJECTED);
   }

   inherited::Update(msg, flowID);
}


/////////////////////////////////////////////////////////////////////////////
// class TOfReqRnoBase impl (Rpt New Order)
/////////////////////////////////////////////////////////////////////////////
K_ctor TOfReqRnoBase::TOfReqRnoBase ()
   : inherited(fcid_RptNew)
{
}
//---------------------------------------------------------------------------

K_mf(void) TOfReqRnoBase::Update (const std::string& msg, EFixOrdFlowID flowID)
{
   inherited::Update(msg, flowID);

   //==============================================
   // 發出[委託回報]
   TFixSysOrdID       soid         = inherited::GetSysOrdID();
   const TFsStkOrder* ord          = inherited::OrdHnd_.GetConst();
   TRfReqErOrd        reqErOrd     = TRptFlow::CreateReqErOrd(soid, fcid_New);
   TRfReqErOrdBase*   refErOrd     = reqErOrd.get();
   SRfBaseField&      fixBaseField = refErOrd->GetBaseField();
   SRfErField&        fixErField   = refErOrd->GetErField  ();

   fixBaseField.OrdStatus_ = ord->OrdStatus_;
   if (RnoField_.TransactTime_.is_null() == false)
      fixBaseField.TransactTime_ = RnoField_.TransactTime_;
   
   fixErField.ExecType_  = ord->ExecType_;
   fixErField.LeavesQty_ = ord->LeavesQty_;

   inherited::OrdFlow_->GetRptFlow()->DoReqErOrd(reqErOrd);
}
//---------------------------------------------------------------------------
