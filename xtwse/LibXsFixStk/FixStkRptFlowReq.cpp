/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FixStkRptFlowReq.h"
#include "FixStkRptFlow.h"
#include "FixStkIntf.h"
#include "FixStkOrdTrack.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;
using namespace Kway::FIX;
using namespace Kway::FIX::Stk;
//---------------------------------------------------------------------------
static const TFixQty2  Fix_Qty_0(0);

/////////////////////////////////////////////////////////////////////////////
// class TRfReqBase impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TRfReqBase::TRfReqBase (EFixRptCmdID rid, const char* msgType, const TFixSysOrdID& soid)
   : RCmd_    (rid)
   , MsgType_ (msgType)
   , SysOrdID_(soid)
   , RptFlow_ (NULL)
   , TradeLog_(NULL)
{
}
//---------------------------------------------------------------------------

K_dtor TRfReqBase::~TRfReqBase ()
{
}
//---------------------------------------------------------------------------

K_mf(void) TRfReqBase::Update (const std::string& strMsg)
{
   LastMsg_ = strMsg;

   //==============================================
   // 組合自己的 Fix 電文
   if (VirOnUpdate() == false)
      return;

   //==============================================
   // 異動委託資料
   TFsStkOrder& stkOrd = OrdHnd_.GetModify();
   
   stkOrd.LastMsg_.assign(LastMsg_);

   //==============================================
   // 打出所有的 Fix 電文
   std::string strMsgType = MsgType_.as_string();
   //std::string strMessage;
   SStkRptWsRef* datWsRef = new SStkRptWsRef;
   SStkRptWsPtr  datWsPtr(datWsRef);
   
   //==============================================
   // 將所有欄位的資料變成是 Message
   Header& header = FixMsg_.getHeader();
   
   header.setField(MsgType     (strMsgType));                        // 035
   //header.setField(TargetCompID(RptFlow_->TradeInfo_->GetSesID()));  // 056
   
   //FixMsg_.toString(strMessage);
   FixMsg_.toString1(datWsRef->Message_);

   //==============================================
   // 將資料儲存到 RptCenter 裡面
   //RptFlow_->RptCenter_->NewRpt(SysOrdID_, MsgType_, strMessage);
   datWsRef->SysOrdID_ = SysOrdID_;
   datWsRef->MsgType_  = MsgType_;
   
   RptFlow_->RptCenter_->NewRpt(datWsPtr);
}


/////////////////////////////////////////////////////////////////////////////
// class TRfReqErBase impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TRfReqErBase::TRfReqErBase (EFixRptCmdID rid, const TFixSysOrdID& soid)
   : inherited(rid, MsgType_ExecutionReport, soid)
{
   ErField_.ExecTransType_.assignT(ExecTransType_NEW);
}
//---------------------------------------------------------------------------

K_ctor TRfReqErBase::TRfReqErBase (EFixRptCmdID rid, const TFixSysOrdID& soid, EFixOrdCmdID oid)
   : inherited(rid, MsgType_ExecutionReport, soid)
   , OCmd_    (oid)
{
   ErField_.ExecTransType_.assignT(ExecTransType_NEW);
}
//---------------------------------------------------------------------------

K_mf(bool) TRfReqErBase::VirOnUpdate ()
{
   int fixVer = RptFlow_->TradeInfo_->GetFixVer();

   RptFlow_->VirBfOnUpdate_Er(*this);

   //==============================================
   // 異動 StkOrder
   TFsStkOrder& stkOrd = OrdHnd_.GetModify();

   stkOrd.ClOrdID_     = BaseField_.ClOrdID_;
   stkOrd.OrigClOrdID_ = BaseField_.OrigClOrdID_;
   stkOrd.Text_        = BaseField_.Text_;
   
   if (ErField_.OrderQty_ .is_null() == false)  stkOrd.OrderQty_  = ErField_.OrderQty_;
   if (ErField_.LeavesQty_.is_null() == false)  stkOrd.LeavesQty_ = ErField_.LeavesQty_;
   if (ErField_.CumQty_   .is_null() == false)  stkOrd.CumQty_    = ErField_.CumQty_;
   if (ErField_.Price_    .is_null() == false)  stkOrd.Price_     = ErField_.Price_;

   stkOrd.OrdStatus_ = BaseField_.OrdStatus_;
   stkOrd.ExecType_  = ErField_  .ExecType_;

   // test {
   //char buf[256];
   //sprintf(buf, "(%s_%s)", stkOrd.ExecType_.as_desc().c_str(), stkOrd.OrdStatus_.as_desc().c_str());
   //stkOrd.Text_.append(buf);
   // } test

   //==============================================
   // 組出 ExecutionReport 的電文
   Message&           fixMsg   = inherited::FixMsg_;
   const TRfSpecRule& specRule = inherited::RptFlow_->SpecRule_;
   bool               isErDeal = (inherited::RCmd_ == frid_ErDeal);

   //==============================================
   // header

   //==============================================
   // body - Required
   FixStk_SetFixAll_Required (fixMsg, AvgPx,               ErField_  .AvgPx_)          // 006
   FixStk_SetFixAll_Required (fixMsg, CumQty,              ErField_  .CumQty_)         // 014
   FixStk_SetFixAll_Required (fixMsg, ExecID,              ErField_  .ExecID_)         // 017

   // FIX.4.4 已經不使用 ExecTransType(020)
   if (fixVer == 42) {
      FixStk_SetFixAll_Required (fixMsg, ExecTransType, ErField_.ExecTransType_)  // 020
   }

   FixStk_SetFixAll_Required (fixMsg, OrderID,             BaseField_.OrderID_)        // 037
   FixStk_SetFixAll_Required (fixMsg, OrdStatus,           stkOrd    .OrdStatus_)      // 039
   FixStk_SetFixAll_Required (fixMsg, ExecType,            stkOrd    .ExecType_)       // 150
   // 151
   if (isErDeal && specRule.test(rsr_ErDeal_LeavesQty_Zero))
      FixStk_SetFixAll_Required (fixMsg, LeavesQty, Fix_Qty_0)
   else
      FixStk_SetFixAll_Required (fixMsg, LeavesQty, stkOrd.LeavesQty_)

   //==============================================
   // body - Optional
   FixStk_SetFixStr_Optional (fixMsg, Account,    stkOrd  .Account_)    // 001
   FixStk_SetFixStr_Optional (fixMsg, ClOrdID,    stkOrd  .ClOrdID_)    // 011
   FixStk_SetFixPri_Optional (fixMsg, LastPx,     ErField_.LastPx_)     // 031
   FixStk_SetFixQty_Optional (fixMsg, LastShares, ErField_.LastShares_) // 032

   // 038
   if (isErDeal && specRule.test(rsr_ErDeal_OrderQty_Zero))
      FixStk_SetFixQty_Optional (fixMsg, OrderQty, Fix_Qty_0)
   else
      FixStk_SetFixQty_Optional (fixMsg, OrderQty, stkOrd.OrderQty_)

   FixStk_SetFixStr_Optional (fixMsg, OrdType,               ErField_  .OrdType_)               // 040
   FixStk_SetFixStr_Optional (fixMsg, OrigClOrdID,           stkOrd    .OrigClOrdID_)           // 041
   FixStk_SetFixPri_Optional (fixMsg, Price,                 stkOrd    .Price_)                 // 044
   FixStk_SetFixStr_Optional (fixMsg, Text,                  stkOrd    .Text_)                  // 058
   FixStk_SetFixStr_Optional2(fixMsg, FIELD::TransactTime,   BaseField_.TransactTime_)          // 060
   FixStk_SetFixStr_Optional (fixMsg, OrdRejReason,          ErField_  .OrdRejReason_)          // 103
   FixStk_SetFixStr_Optional (fixMsg, ExecRestatementReason, ErField_  .ExecRestatementReason_) // 378

   //**********************************************
   #if defined(_FixStkVer_RptTrack_)
   //==============================================
   // 異動[回報歷程]
   TRptTrack::TRecsHnd rshRt(RptFlow_->RptTrack_, SysOrdID_);
   SRptTrack&          udRt = rshRt.BeginUpdate2_Inc();

   udRt.MsgType_.assign(MsgType_ExecutionReport);
   udRt.ClOrdID_     = stkOrd  .ClOrdID_;
   udRt.OrigClOrdID_ = stkOrd  .OrigClOrdID_;
   udRt.Text_        = stkOrd  .Text_;
   udRt.OrdStatus_   = stkOrd  .OrdStatus_;
   udRt.ExecType_    = stkOrd  .ExecType_;
   udRt.CumQty_      = ErField_.CumQty_;
   udRt.LastShares_  = ErField_.LastShares_;
   udRt.OrderQty_    = ErField_.OrderQty_;
   udRt.LeavesQty_   = stkOrd  .LeavesQty_;
   udRt.LastPx_      = ErField_.LastPx_;
   udRt.Price_       = ErField_.Price_;
   #endif
   //**********************************************
   
   return true;
}


/////////////////////////////////////////////////////////////////////////////
// class TRfReqErOrdBase impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TRfReqErOrdBase::TRfReqErOrdBase (const TFixSysOrdID& soid, EFixOrdCmdID oid)
   : inherited    (frid_ErOrd, soid, oid)
   , EnableResend_(false)
{
}
//---------------------------------------------------------------------------

K_mf(bool) TRfReqErOrdBase::VirOnUpdate ()
{
   bool ret = false;
   
   switch (OCmd_) {
      case fcid_New:        ret = UpdateNew    ();  break;
      case fcid_Del:        ret = UpdateDelete ();  break;
      case fcid_ChgQty:
      case fcid_ChgPri:
      case fcid_ChgPriQty:  ret = UpdateReplace();  break;
      case fcid_Qry:        ret = UpdateQuery  ();  break;
   }
   
   if (ret)
      inherited::VirOnUpdate();

   return ret;
}
//---------------------------------------------------------------------------

K_mf(bool) TRfReqErOrdBase::UpdateNew ()
{
   //==============================================
   // 尋找 OrdTrack 的狀態
   TOrdTrack::TRecsHnd             rshOt(RptFlow_->OrdTrack_, SysOrdID_);
   TOrdTrack::TRecsHnd::ENewStatus sts = rshOt.FindNewStatus();
   
   std::string strErrMsg;
   char        logMsg[128];
   int         logLen = 0;
   
   if (sts == TOrdTrack::TRecsHnd::ens_Sent) {
      if (!EnableResend_) {
         strErrMsg.assign("ERR:[New]回報已傳送!");

         logLen = sprintf(logMsg, "<UpdateNew> SysOrdID[%d] %s",
                                    SysOrdID_.get_orig(),
                                    strErrMsg.c_str());
      }
   }
   else if (sts == TOrdTrack::TRecsHnd::ens_NotFound) {
      strErrMsg.assign("ERR:找不到目前有效的[New]委託!");
      
      logLen = sprintf(logMsg, "<UpdateNew> SysOrdID[%d] %s",
                                 SysOrdID_.get_orig(),
                                 strErrMsg.c_str());
   }

   if (logLen != 0)
      TradeLog_->LogMem(elt_Error, logMsg, logLen);

   //==============================================
   // 看看有沒有錯誤訊息
   TFsStkOrder& stkOrd = OrdHnd_.GetModify();

   if (strErrMsg.empty() == false) {
      stkOrd.LastMsg_.assign(strErrMsg);
      return false;
   }

   //==============================================
   // 開始異動最後的委託狀態
   SOrdTrack& udOt = rshOt.BeginUpdate2();

   udOt.FlowID_.assignT(fof_Finish);

   //==============================================
   // 異動 StkOrd 的資料
   stkOrd.FlowID_ = udOt.FlowID_;

   //==============================================
   // 保留資料給[回報歷程]
   BaseField_.ClOrdID_ = udOt.ClOrdID_;
   
   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TRfReqErOrdBase::UpdateDelete ()
{
   //==============================================
   // 尋找 OrdTrack 的狀態
   TOrdTrack::TRecsHnd rshOt  (RptFlow_->OrdTrack_, SysOrdID_);
   TFixMsgType2        msgType(MsgType_OrderCancelRequest);
   std::string         strErrMsg;

   if (RptFlow_->VirErOrd_FindOrdTrack(*this, rshOt, msgType) == false) {
      char logMsg[512];
      int  logLen;

      strErrMsg.assign("ERR:找不到目前有效的[Cancel]委託!");
      logLen = sprintf(logMsg, "<TRptFlow::TReqErOrdBase::UpdateDelete> %s SysOrdID[%d]",
                                 strErrMsg.c_str(), SysOrdID_.get_orig());

      TradeLog_->LogMem(elt_Error, logMsg, logLen);
   }

   //==============================================
   // 看看有沒有錯誤訊息
   TFsStkOrder& stkOrd = OrdHnd_.GetModify();

   if (strErrMsg.empty() == false) {
      stkOrd.LastMsg_.assign(strErrMsg);
      return false;
   }

   //==============================================
   // 開始異動最後的委託狀態
   SOrdTrack& udOt = rshOt.BeginUpdate2();

   udOt.FlowID_.assignT(fof_Finish);
   
   //==============================================
   // 異動 StkOrd 的資料
   stkOrd.FlowID_ = udOt.FlowID_;

   //==============================================
   // 保留資料給[回報歷程]
   BaseField_.ClOrdID_     = udOt.ClOrdID_;
   BaseField_.OrigClOrdID_ = udOt.OrigClOrdID_;
   
   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TRfReqErOrdBase::UpdateReplace ()
{
   //==============================================
   // 尋找 OrdTrack 的狀態
   TOrdTrack::TRecsHnd rshOt  (RptFlow_->OrdTrack_, SysOrdID_);
   TFixMsgType2        msgType(MsgType_OrderCancelReplaceRequest);
   std::string         strErrMsg;

   if (RptFlow_->VirErOrd_FindOrdTrack(*this, rshOt, msgType) == false) {
      char logMsg[512];
      int  logLen;

      strErrMsg.assign("ERR:找不到目前有效的[Replace]委託!");
      logLen = sprintf(logMsg, "<TRptFlow::TReqErOrdBase::UpdateReplace> %s SysOrdID[%d]",
                                 strErrMsg.c_str(), SysOrdID_.get_orig());

      TradeLog_->LogMem(elt_Error, logMsg, logLen);
   }

   //==============================================
   // 看看有沒有錯誤訊息
   TFsStkOrder& stkOrd = OrdHnd_.GetModify();

   if (strErrMsg.empty() == false) {
      stkOrd.LastMsg_.assign(strErrMsg);
      return false;
   }

   //==============================================
   // 開始異動最後的委託狀態
   SOrdTrack& udOt = rshOt.BeginUpdate2();

   udOt.FlowID_.assignT(fof_Finish);
   
   //==============================================
   // 異動 StkOrd 的資料
   stkOrd.FlowID_ = udOt.FlowID_;

   //==============================================
   // 保留資料給[回報歷程]
   BaseField_.ClOrdID_     = udOt.ClOrdID_;
   BaseField_.OrigClOrdID_ = udOt.OrigClOrdID_;
   
   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TRfReqErOrdBase::UpdateQuery ()
{
   //==============================================
   // 尋找 OrdTrack 的狀態
   TOrdTrack::TRecsHnd rshOt  (RptFlow_->OrdTrack_, SysOrdID_);
   TFixMsgType2        msgType(MsgType_OrderStatusRequest);
   std::string         strErrMsg;

   if (rshOt.FindUnfinished(msgType) == false) {
      char logMsg[512];
      int  logLen;

      strErrMsg.assign("ERR:找不到目前有效的[Query]委託!");
      logLen = sprintf(logMsg, "<TRptFlow::TReqErOrdBase::UpdateReplace> %s SysOrdID[%d]",
                                 strErrMsg.c_str(), SysOrdID_.get_orig());
      
      TradeLog_->LogMem(elt_Error, logMsg, logLen);
   }

   //==============================================
   // 看看有沒有錯誤訊息
   TFsStkOrder& stkOrd = OrdHnd_.GetModify();

   if (strErrMsg.empty() == false) {
      stkOrd.LastMsg_.assign(strErrMsg);
      return false;
   }

   //==============================================
   // 開始異動最後的委託狀態
   SOrdTrack& udOt = rshOt.BeginUpdate2();

   udOt.FlowID_.assignT(fof_Finish);
   
   //==============================================
   // 異動 StkOrd 的資料
   stkOrd.FlowID_ = udOt.FlowID_;

   //==============================================
   // 保留資料給[回報歷程]
   BaseField_.ClOrdID_     = udOt.ClOrdID_;
   BaseField_.OrigClOrdID_ = udOt.OrigClOrdID_;
   
   return true;
}

/////////////////////////////////////////////////////////////////////////////
// class TRfReqErRejectBase impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TRfReqErRejectBase::TRfReqErRejectBase (const TFixSysOrdID& soid, EFixOrdCmdID oid, int rejCode)
   : inherited  (frid_ErReject, soid, oid)
   , RejectCode_(rejCode)
{
   //==============================================
   // 預設資料區
   ErField_.ExecType_.assignT(ExecType_REJECTED);
}
//---------------------------------------------------------------------------

K_mf(bool) TRfReqErRejectBase::VirOnUpdate ()
{
   //==============================================
   // 尋找 OrdTrack 的狀態
   TOrdTrack::TRecsHnd rshOt(RptFlow_->OrdTrack_, SysOrdID_);
   std::string         strErrMsg;

   if (inherited::OCmd_ == fcid_New) {
      TOrdTrack::TRecsHnd::ENewStatus sts = rshOt.FindNewStatus();

      if (sts != TOrdTrack::TRecsHnd::ens_Found) {
         char logMsg[128];
         int  len;

         if (sts == TOrdTrack::TRecsHnd::ens_Sent) {
            strErrMsg.assign("ERR:新單回報已傳送!");
            len = sprintf(logMsg, "<TRptFlow::TReqErRejectBase::DoUpdate> %s SysOrdID[%d]",
                                       strErrMsg.c_str(),
                                       SysOrdID_.get_orig());
         }
         else {
            strErrMsg.assign("ERR:找不到此筆有效委託!");
            len = sprintf(logMsg, "<TRptFlow::TReqErRejectBase::DoUpdate> %s SysOrdID[%d]",
                                       strErrMsg.c_str(),
                                       SysOrdID_.get_orig());
         }

         TradeLog_->LogMem(elt_Error, logMsg, len);
      }
   }
   else if (inherited::OCmd_ == fcid_Qry) {
      TFixMsgType2 msgType(MsgType_OrderStatusRequest);

      if (rshOt.FindUnfinished(msgType) == false) {
         char logMsg[512];
         int  logLen;

         strErrMsg.assign("ERR:找不到目前有效的[Query]委託!");
         logLen = sprintf(logMsg, "<TRptFlow::TReqErRejectBase::DoUpdate> %s SysOrdID[%d]",
                                    strErrMsg.c_str(), SysOrdID_.get_orig());

         TradeLog_->LogMem(elt_Error, logMsg, logLen);
      }
   }

   //==============================================
   // 看看有沒有錯誤訊息
   TFsStkOrder& stkOrd = OrdHnd_.GetModify();

   if (strErrMsg.empty() == false) {
      stkOrd.LastMsg_.assign(strErrMsg);
      return false;
   }

   //==============================================
   // 開始異動最後的委託狀態
   SOrdTrack& udOt = rshOt.BeginUpdate2();

        if (inherited::OCmd_ == fcid_New)  udOt.FlowID_.assignT(fof_AbortOrdClear);
   else if (inherited::OCmd_ == fcid_Qry)  udOt.FlowID_.assignT(fof_AbortOrdNoChange);
   
   //==============================================
   // 異動 StkOrd 的資料
   stkOrd.FlowID_ = udOt.FlowID_;

   //==============================================
   // 保留資料給[回報歷程]
   BaseField_.ClOrdID_     = udOt.ClOrdID_;
   BaseField_.OrigClOrdID_ = udOt.OrigClOrdID_;

   return inherited::VirOnUpdate();
}


/////////////////////////////////////////////////////////////////////////////
// class TRfReqErDealBase impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TRfReqErDealBase::TRfReqErDealBase (const TFixSysOrdID& soid)
   : inherited(frid_ErDeal, soid)
{
}
//---------------------------------------------------------------------------

K_mf(bool) TRfReqErDealBase::VirOnUpdate ()
{
   const TFsStkOrder* stkOrd = OrdHnd_.GetConst();

   //==============================================
   // 使用目前委託資料表的 ClOrdID
   inherited::BaseField_.ClOrdID_ = stkOrd->ClOrdID_;

   // test {
   char              logMsg[128];
   const SRfErField& erField = inherited::ErField_;

   sprintf(logMsg, "成交量[%d],成交價[%s]", erField.LastShares_.get_orig(), erField.LastPx_.as_str().c_str());
   LastMsg_.assign(logMsg);
   // } test

   return inherited::VirOnUpdate();
}


/////////////////////////////////////////////////////////////////////////////
// class TRfReqOcrBase impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TRfReqOcrBase::TRfReqOcrBase (const TFixSysOrdID& soid, EFixOrdCmdID oid, int rejCode)
   : inherited  (frid_Ocr, MsgType_OrderCancelReject, soid)
   , RejectCode_(rejCode)
{
   if (oid == fcid_Del)  OcrField_.CxlRejResponseTo_.assignT(CxlRejResponseTo_ORDER_CANCEL_REQUEST);
   else                  OcrField_.CxlRejResponseTo_.assignT(CxlRejResponseTo_ORDER_CANCEL_REPLACE_REQUEST);
}
//---------------------------------------------------------------------------

K_mf(bool) TRfReqOcrBase::VirOnUpdate ()
{
   //==============================================
   // 尋找 OrdTrack 的狀態
   TOrdTrack::TRecsHnd rshOt(RptFlow_->OrdTrack_, SysOrdID_);
   TFixMsgType2        msgType;

   if (OcrField_.CxlRejResponseTo_ == CxlRejResponseTo_ORDER_CANCEL_REQUEST) {
      msgType.assign(MsgType_OrderCancelRequest);
   }
   else if (OcrField_.CxlRejResponseTo_ == CxlRejResponseTo_ORDER_CANCEL_REPLACE_REQUEST) {
      msgType.assign(MsgType_OrderCancelReplaceRequest);
   }
   else {
      char logMsg[512];
      int  logLen = sprintf(logMsg, "<TRptFlow::TReqOcrBase::DoUpdate> unknown CxlRejResponseTo[%c]",
                                    OcrField_.CxlRejResponseTo_.get_orig());
      
      TradeLog_->LogMem(elt_Error, logMsg, logLen);
      return false;
   }

   std::string strErrMsg;

   if (rshOt.FindUnfinished(msgType) == false) {
      char logMsg[512], logTmp[128];
      int  logLen = sprintf(logTmp, "ERR:找不到目前有效委託[%s]", msgType.as_string().c_str());
      
      strErrMsg.assign(logTmp, logLen);
      logLen = sprintf(logMsg, "<TRptFlow::TReqOcrBase::DoUpdate> %s SysOrdID[%d]",
                                 logTmp, SysOrdID_.get_orig());

      TradeLog_->LogMem(elt_Error, logMsg, logLen);
   }

   //==============================================
   // 看看有沒有錯誤訊息
   TFsStkOrder& stkOrd = OrdHnd_.GetModify();

   if (strErrMsg.empty() == false) {
      stkOrd.LastMsg_.assign(strErrMsg);
      return false;
   }

   RptFlow_->VirBfOnUpdate_Ocr(*this);

   //==============================================
   // 開始異動 OrdTrack 的狀態
   SOrdTrack& udOt = rshOt.BeginUpdate2();

   udOt.FlowID_.assignT(fof_AbortOrdNoChange);

   //==============================================
   // 異動委託中心
   
   // 這是失敗的回報資料,所以不可更改 ClOrdID, OrigClOrdID
   // 但是原始的資料要是空的話,就填進去囉
   if (stkOrd.ClOrdID_.empty()) {
      stkOrd.ClOrdID_     = udOt.ClOrdID_;
      stkOrd.OrigClOrdID_ = udOt.OrigClOrdID_;
   }

   stkOrd.Text_        = BaseField_.Text_;
   stkOrd.OrdStatus_   = BaseField_.OrdStatus_;
   stkOrd.FlowID_      = udOt.FlowID_;

   //==============================================
   // 組出 OrderCancelRequest 的電文
   Message& fixMsg = inherited::FixMsg_;

   //==============================================
   // body
   FixStk_SetFixAll_Required (fixMsg, OrderID,             BaseField_.OrderID_)           // 037
   FixStk_SetFixAll_Required (fixMsg, OrdStatus,           stkOrd    .OrdStatus_)         // 039
   FixStk_SetFixAll_Required (fixMsg, ClOrdID,             udOt      .ClOrdID_)           // 011
   FixStk_SetFixAll_Required (fixMsg, OrigClOrdID,         udOt      .OrigClOrdID_)       // 041
   FixStk_SetFixAll_Required (fixMsg, CxlRejResponseTo,    OcrField_ .CxlRejResponseTo_)  // 434
   //------------------------------------
   // Optional
   FixStk_SetFixStr_Optional (fixMsg, Account,             stkOrd    .Account_)           // 001
   FixStk_SetFixStr_Optional (fixMsg, Text,                stkOrd    .Text_)              // 058
   FixStk_SetFixStr_Optional2(fixMsg, FIELD::TransactTime, BaseField_.TransactTime_)      // 060
   FixStk_SetFixStr_Optional (fixMsg, CxlRejReason,        OcrField_ .CxlRejReason_)      // 102

   //**********************************************
   #if defined(_FixStkVer_RptTrack_)
   //==============================================
   // 異動[回報歷程]
   TRptTrack::TRecsHnd rshRt(RptFlow_->RptTrack_, SysOrdID_);
   SRptTrack&          udRt = rshRt.BeginUpdate2_Inc();

   udRt.MsgType_.assign(MsgType_ExecutionReport);
   udRt.ClOrdID_     = stkOrd.ClOrdID_;
   udRt.OrigClOrdID_ = stkOrd.OrigClOrdID_;
   udRt.OrdStatus_   = stkOrd.OrdStatus_;
   udRt.Text_        = stkOrd.Text_;
   #endif
   //**********************************************
   
   return true;
}


/////////////////////////////////////////////////////////////////////////////
// class TRfReqRejectBase impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TRfReqRejectBase::TRfReqRejectBase (const TFixSysOrdID& soid, EFixOrdCmdID oid, int rejCode)
   : inherited  (frid_Unknown, MsgType_Null, soid)
   , OCmd_      (oid)
   , RejectCode_(rejCode)
{
}
//---------------------------------------------------------------------------
