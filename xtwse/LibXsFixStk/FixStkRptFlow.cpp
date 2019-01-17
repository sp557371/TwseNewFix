/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FixStkRptFlow.h"
#include "Nsa_UtilComm.h"
#include "src/Code/Values.h"
#include "FixStkVer.h"
#include "FixStkIntf.h"
#include "FixStkOrdTrack.h"
#include "FixStkRptTrack.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::FIX;
using namespace Kway::FIX::Stk;

/////////////////////////////////////////////////////////////////////////////
// class TRptFlow impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TRptFlow::TRptFlow (TTradeInfo* ti)
   : TradeInfo_(ti)
   , OrdCenter_(ti->GetFixOrdCenter())
   , OrdTrack_ (ti->GetFixOrdTrack ())
   , RptCenter_(ti->GetFixRptCenter())
   //**********************************************
   #if defined(_FixStkVer_RptTrack_)
   , RptTrack_ (ti->GetFixRptTrack ())
   #endif
   //**********************************************
   , TradeLog_ (ti->GetFixTradeLog ())
{
   ti->SetFixRptFlow(this);
}
//---------------------------------------------------------------------------

class TRptFlow::TFlowStepWorker
{
   TRfReqPtr  ReqPtr_;

public:
   inline K_ctor TFlowStepWorker (TRfReqBase& req)
      : ReqPtr_(&req)
   {}

   K_mf(EEvObjResult) Exec (TFlowStep* fs)
   {
      if (fs->OnRptReq(ReqPtr_) == false)
         return eor_Abort;

      return eor_Continue;
   }
};
//------------------------------------
K_mf(bool) TRptFlow::DoFlowSteps (TRfReqBase& req)
{
   char         msgbuf[1024];
   TFixRptCmdID rid       = req.GetRptCmdID();
   TFlowSteps&  fss       = FlowSteps_[rid];
   size_t       stepCount = fss.GetSize();

   if (stepCount == 0) {
      sprintf(msgbuf, "沒有設定:[%s]回報要求的服務!", rid.as_string().c_str());
      //req.Update(msgbuf, cid==fcid_New ? fof_AbortOrdClear : fof_AbortOrdUnknow);
      return false;
   }

   TFlowStepWorker worker(req);
   EEvObjResult    resu = fss.ForEachEv(worker);

   if (resu != eor_Finish)
      return false;

   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TRptFlow::DoReqBase (TRfReqBase& refBase)
{
   char logMsg[256];
   int  logLen;
   
   refBase.RptFlow_  = this;
   refBase.TradeLog_ = TradeLog_;

   //==============================================
   // 先從 OrdCenter 中尋找原始委託資料
   refBase.OrdHnd_ = OrdCenter_->GetOrdHnd(refBase.SysOrdID_, fuf_OrdFlow);
   if (refBase.OrdHnd_.is_invalid()) {
      TFixSysOrdID::orig_type fsid = refBase.SysOrdID_.get_orig();

      logLen = sprintf(logMsg, "<TRptFlow::DoReqBase> 找不到原始委託資料, FixSysOrdID[%d]", fsid);
      TradeLog_->LogMem(elt_Error, logMsg, logLen);
      
      sprintf(logMsg, "<FixRptFlow> 找不到原始委託資料, FixSysOrdID[%d]", fsid);
      TUtilComm::ConsMsg(logMsg);
      return false;
   }
   
   const TFsStkOrder* stkOrd = refBase.OrdHnd_.GetConst();
   if (!stkOrd) {
      TFixSysOrdID::orig_type fsid = refBase.SysOrdID_.get_orig();
      
      logLen = sprintf(logMsg, "<TRptFlow::DoReqBase> StkOrder = Null, FixSysOrdID[%d]", fsid);
      TradeLog_->LogMem(elt_Error, logMsg, logLen);

      sprintf(logMsg, "<FixRptFlow> StkOrder = Null, FixSysOrdID[%d]", fsid);
      TUtilComm::ConsMsg(logMsg);
      return false;
   }

   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TRptFlow::DoReqErBase (TRfReqErBase& refReqEr)
{
   if (DoReqBase(refReqEr) == false)
      return false;

   //==============================================
   // 將 OrdCenter 中的基本資料回填
   Message&           fixMsg = refReqEr.FixMsg_;
   const TFsStkOrder* stkOrd = refReqEr.OrdHnd_.GetConst();

   //------------------------------------
   // Required
   FixStk_SetFixAll_Required (fixMsg, Side,   stkOrd->Side_)
   FixStk_SetFixAll_Required (fixMsg, Symbol, stkOrd->Symbol_)
   
   //------------------------------------
   // Optional
   FixStk_SetFixStr_Optional (fixMsg, ClOrdID,     stkOrd->ClOrdID_)
   FixStk_SetFixStr_Optional (fixMsg, OrdType,     stkOrd->OrdType_)
   FixStk_SetFixStr_Optional (fixMsg, TimeInForce, stkOrd->TimeInForce_)
   
   //==============================================
   // 自動填入基本欄位
   SRfErField& erField = refReqEr.ErField_;
   
   //if (refReqEr.RfErFieldSet_.test(erf_Erf_OrderQty) == false)
   //   refReqEr.ErField_.OrderQty_ = stkOrd->OrderQty_;
   if (erField.OrderQty_.is_null())
      erField.OrderQty_ = stkOrd->OrderQty_;
   
   // 應該不需要填吧,直接取自己的 OrdHnd 就知道啦
   //erField.Price_ = stkOrd->Price_;

   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TRptFlow::DoReqErOrd (TRfReqErOrd req)
{
   TRfReqErOrdBase* refErOrd = req.get();

   if (DoReqErBase(*refErOrd) == false)
      return false;
   
   return DoFlowSteps(*refErOrd);
}
//---------------------------------------------------------------------------

K_mf(bool) TRptFlow::DoReqErReject (TRfReqErReject req)
{
   TRfReqErRejectBase* refErReject = req.get();

   if (DoReqErBase(*refErReject) == false)
      return false;
   
   //==============================================
   // OrdStatus
   const TFsStkOrder*  stkOrd    = refErReject->OrdHnd_.GetConst();
         SRfBaseField& baseField = refErReject->GetBaseField();

   if (stkOrd->OrdStatus_ == OrdStatus_PENDING_NEW)
      baseField.OrdStatus_.assignT(OrdStatus_REJECTED);
   else
      baseField.OrdStatus_ = stkOrd->OrdStatus_;

   return DoFlowSteps(*refErReject);
}
//---------------------------------------------------------------------------

K_mf(bool) TRptFlow::DoReqErDeal (TRfReqErDeal req)
{
   TRfReqErDealBase* refErDeal = req.get();

   if (DoReqErBase(*refErDeal) == false)
      return false;
   
   return DoFlowSteps(*refErDeal);
}
//---------------------------------------------------------------------------

K_mf(bool) TRptFlow::DoReqOcr (TRfReqOcr req)
{
   TRfReqOcrBase* refOcr = req.get();

   if (DoReqBase(*refOcr) == false) {
      return false;
   }
   
   //==============================================
   // 將 OrdCenter 中的基本資料回填
   Message&           fixMsg = refOcr->FixMsg_;
   const TFsStkOrder* stkOrd = refOcr->OrdHnd_.GetConst();

   //------------------------------------
   // Required
   SRfBaseField& baseField = refOcr->GetBaseField();
   
   // 如果此時沒有指定 OrdStatus 時, 就採用最後結果
   if (baseField.OrdStatus_.is_null())
      baseField.OrdStatus_ = stkOrd->OrdStatus_;

   return DoFlowSteps(*refOcr);
}
//---------------------------------------------------------------------------

K_mf(bool) TRptFlow::DoReqReject (TRfReqReject req)
{
   TRfReqRejectBase* refReject = req.get();

   if (refReject->OCmd_ == fcid_New || refReject->OCmd_ == fcid_Qry) {
      TRfReqErReject reqErReject = CreateReqErReject(refReject->SysOrdID_, refReject->OCmd_, refReject->RejectCode_);

      return DoReqErReject(reqErReject);
   }

   TRfReqOcr reqOcr = CreateReqOcr(refReject->SysOrdID_, refReject->OCmd_, refReject->RejectCode_);

   return DoReqOcr(reqOcr);
}
//---------------------------------------------------------------------------
