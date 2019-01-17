/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FixStkOrdFlow.h"
// libnsAux
#include "Nsa_UtilComm.h"
#include "Nsa_DbgInfo.h"
// FixStk
#include "FixStkVer.h"
#include "FixStkIntf.h"
#include "FixStkIntf.h"
#include "FixStkCoids.h"
#include "FixStkRptFlow.h"
#include "FixStkOrdTrack.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;
using namespace Kway::FIX;
using namespace Kway::FIX::Stk;

/////////////////////////////////////////////////////////////////////////////
// class TFsOrdFlow impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TFsOrdFlow::TFsOrdFlow (TTradeInfo* ti)
   : TradeInfo_(ti)
   , OrdCenter_(ti->GetFixOrdCenter())
   , Coids_    (ti->GetFixCoids    ())
   , OrdTrack_ (ti->GetFixOrdTrack ())
   , TradeLog_ (ti->GetFixTradeLog ())
   , RptFlow_  (NULL)
{
   ti->SetFixOrdFlow(this);
}
//---------------------------------------------------------------------------

class TFsOrdFlow::TFlowStepWorker
{
   TOfReqPtr  ReqPtr_;

public:
   inline K_ctor TFlowStepWorker (TOfReqBase& req)
      : ReqPtr_(&req)
   {
      // debug {
      //fprintf(stderr, "<TOrdFlow::TFlowStepWorker> SysOrdID[%d]\n",
      //                     ReqPtr_->GetSysOrdID().get_orig());
      // } debug
   }

   K_mf(EEvObjResult) Exec (TFlowStep* fs)
   {
      // debug {
      //fprintf(stderr, "<TOrdFlow::Exec> ptr[%p], SysOrdID[%d]\n",
      //                     ReqPtr_.get(),
      //                     ReqPtr_->GetSysOrdID().get_orig());
      // } debug

      if (fs->OnFsOrdReq(ReqPtr_) == false)
         return eor_Abort;

      return eor_Continue;
   }
};
//------------------------------------
K_mf(bool) TFsOrdFlow::DoFlowSteps (TOfReqBase& refBase)
{
   char         msgbuf[1024];
   TFixOrdCmdID cid       = refBase.GetOrdCmdID();
   TFlowSteps&  fss       = FlowSteps_[cid];
   size_t       stepCount = fss.GetSize();

   if (stepCount == 0) {
      sprintf(msgbuf, "沒有設定:[%s]委託要求的服務!", cid.as_desc().c_str());
      //req.Update(msgbuf, cid==fcid_New ? of_AbortOrdClear : of_AbortOrdUnknow);
      return false;
   }

   TFlowStepWorker worker(refBase);
   EEvObjResult    resu = fss.ForEachEv(worker);
   
   if (resu != eor_Finish)
      return false;
      
   return true;
}
//---------------------------------------------------------------------------

K_mf(void) TFsOrdFlow::InitOrdField (const Message& fixMsg, const SOfOrdField& ordField, TFsStkOrder& ord)
{
   const Header& fixHdr = fixMsg.getHeader();

   //==============================================
   // FixMsg
   // header
   FixStk_GetFixMsg(fixHdr, SenderSubID, ord.SenderSubID_)
   FixStk_GetFixMsg(fixHdr, TargetSubID, ord.TargetSubID_)
   // body
   FixStk_GetFixMsg(fixMsg, HandlInst,   ord.HandlInst_)
   FixStk_GetFixMsg(fixMsg, OrdType,     ord.OrdType_)
   FixStk_GetFixMsg(fixMsg, TimeInForce, ord.TimeInForce_)

   //==============================================
   // OrdField
   ord.ClOrdID_ = ordField.ClOrdID_;
   ord.Account_ = ordField.Account_;
   ord.OrderID_ = ordField.OrderID_;
   ord.Side_    = ordField.Side_;
   ord.Symbol_  = ordField.Symbol_;
   //
   ord.OrderQty_.assign(ordField.OrderQty_.as_string());
   ord.Price_   .assign(ordField.Price_   .as_string());
   //
   ord.OrigAccount_ = ord.Account_;

   VirInitNewOrdField(fixMsg, ordField, ord);
}
//---------------------------------------------------------------------------
//
// New Order Single
//
K_mf(bool) TFsOrdFlow::DoReqNos (TOfReqNos reqNos)
{
   TOfReqNosBase* refNos           = reqNos.get();
                  refNos->OrdFlow_ = this;
   SOfOrdField&   ordField         = refNos->OrdField_;
   char           logMsg[256];
   size_t         logLen;

   //==============================================
   // 將 Fix 電文資料填入 OrdField
   GetOrdFiled(OfSetNos_, refNos->GetFixMsg(), ordField);

   //==============================================
   // 檢查 ClOrdID 是否重覆
   const TFixMsgSeqNum& msgSeqNum = ordField.MsgSeqNum_;
   const TFixClOrdID&   clOrdID   = ordField.ClOrdID_;
   ECoidStatus          coidSts   = (clOrdID.empty() ? ecs_Empty : ecs_Valid);

   if (coidSts == ecs_Valid) {
      if (Coids_->AddCoid(clOrdID, refNos->CoidU_) == false) {
         // ClOrdID 重覆了
         coidSts = ecs_Duplicate;
      }
   }
   else {
      // ClOrdID 是空白的, 該如何處理呢?
   }

   //==============================================
   // 檢查是否為 PossDupFlag
   if (ordField.PossDupFlag_.is_true()) {
      if (coidSts != ecs_Valid) {
         logLen = sprintf(logMsg, "<TFsOrdFlow::DoReqNos> PossDupFlag=Y, SeqNum[%d], ClOrdID[%s] 重複 or 空白!",
                                       msgSeqNum.get_orig(), clOrdID.as_string().c_str());

         TradeLog_->LogMem(elt_Info, logMsg, logLen);
         return true;
      }
   }

   //==============================================
   // 將委託資料紀錄到 OrdCenter
   refNos->OrdHnd_ = OrdCenter_->GetSysOrds()->begin_insert(fuf_OrdFlow);

   TFixSysOrdID sid = refNos->GetSysOrdID();
   TFsStkOrder& ord = refNos->OrdHnd_.GetModify();

   InitOrdField(refNos->GetFixMsg(), refNos->OrdField_, ord);

   //==============================================
   // 紀錄 ClOrdID --> SysOrdID 的關係
   if (coidSts == ecs_Valid) {
      SCoid* coidDat = refNos->GetCoid();
      if (coidDat) {
         coidDat->SysOrdID_ = sid;
      }
   }
   else if (coidSts == ecs_Duplicate) {
      logLen = sprintf(logMsg, "<TFsOrdFlow::DoReqNos> SeqNum[%d], ClOrdID[%s] Duplicate",
                                    msgSeqNum.get_orig(), clOrdID.as_string().c_str());

      TradeLog_->LogMem(elt_Warning, logMsg, logLen);
   }

   //==============================================
   // 02.這邊會儲存[卷商代號+委託書號]與 FixSysOrdID 的關係,移到 OnProcReqNos 裡面處理
   //    因為要檢查[卷商代號+委託書號]是否是重覆的
   //// 01.開放給繼承者有特殊的[委託對應]規則
   //VirInsNewOrd(sid, ord);

   //==============================================
   // 紀錄委託歷程
   SOrdTrack* otrack;

   refNos->OTrackU_ = OrdTrack_->GetNewTrack(sid, SOrdTrack());
   otrack           = refNos   ->GetOrdTrack();

   otrack->OrdCmd_    .assignT(fcid_New);
   otrack->MsgSeqNum_ = msgSeqNum;
   otrack->MsgType_   .assign (MsgType_NewOrderSingle);
   otrack->ClOrdID_   = clOrdID;
   otrack->OrderQty_  = ord.OrderQty_;
   otrack->Price_     = ord.Price_;
   otrack->FlowID_    .assignT(fof_P_ing);

   //==============================================
   // FixStkOrder
   ord.ExecType_.assignT(ExecType_PENDING_NEW);

   //==============================================
   // 開放給[繼承者]看看是否有額外的資訊要儲存
   if (OnProcReqNos(refNos) == false) {
      return false;
   }

   //==============================================
   // 處理 ClOrdID 重複的狀況
   if (coidSts == ecs_Duplicate) {
      // 因為 OrdHnd 是新建立的, 所以先要 Update 一下, 這樣才會把資料 insert 到 SysOrds 裡面
      refNos->OrdHnd_.FlushUpdate(fuf_OrdFlow);
      
      ProcClOrdIDTheSame(fcid_New, sid);
      return false;
   }

   //==============================================
   // 最後檢查是否有 RejCode
   if (refNos->InitRejCode_ != wrc_None) {
      ProcReqError(*refNos, refNos->InitRejCode_);
      return false;
   }

   return DoFlowSteps(*refNos);
}
//---------------------------------------------------------------------------
//
// Auto New Order
//
K_mf(bool) TFsOrdFlow::DoReqAno (TOfReqAno reqAno)
{
   TOfReqAnoBase* refAno           = reqAno.get();
   SOfOrdField&   ordField         = refAno->OrdField_;
                  refAno->OrdFlow_ = this;

   //==============================================
   // 將 Fix 電文資料填入 OrdField
   GetOrdFiled(OfSetAno_, refAno->GetFixMsg(), ordField);

   //==============================================
   // 將委託資料紀錄到 OrdCenter
   refAno->OrdHnd_ = OrdCenter_->GetSysOrds()->begin_insert(fuf_OrdFlow);
   
   TFixSysOrdID sid = refAno->GetSysOrdID();
   TFsStkOrder& ord = refAno->OrdHnd_.GetModify();

   InitOrdField(refAno->GetFixMsg(), ordField, ord);

   //==============================================
   // 2010.06.29 {
   // 此時的 OrigClOrdID 有可能是錯誤的
   // 有可能委託不是從 Fix 出去的, 那 OrigClOrdID 有可能是亂填的, 所以沒辦法當作依據
   //    紀錄 ClOrdID --> SysOrdID 的關係
   //    此種型別是從別的委託衍生出來的, 所以這裡的 OrigClOrdID 就是此 SysOrdID 的對應
   //    不需要紀錄 ClOrdID --> SysOrdID 的關係, 因為後續的處理會紀錄
   //if (ordField.OrigClOrdID_.empty() == false) {
   //   Coids_->AddCoid(ordField.OrigClOrdID_, refAno->CoidU_, sid);
   //}
   // } 2010.06.29

   //==============================================
   // 開放給繼承者有特殊的[委託對應]規則
   VirInsNewOrd(sid, ord);

   //==============================================
   // 紀錄委託歷程
   SOrdTrack* otrack;

   refAno->OTrackU_ = OrdTrack_->GetNewTrack(sid, SOrdTrack());
   otrack           = refAno   ->GetOrdTrack();
   
   otrack->MsgSeqNum_ = ordField.MsgSeqNum_;
   otrack->OrdCmd_    .assignT(fcid_AutoNew);
   otrack->FlowID_    .assignT(fof_P_ing);
   otrack->OrderQty_  = ord.OrderQty_;
   otrack->Price_     = ord.Price_;

   //==============================================
   // FixStkOrder
   ord.ExecType_ .assignT(ExecType_PENDING_NEW);
   ord.OrdStatus_.assignT(OrdStatus_PENDING_NEW);

   return DoFlowSteps(*refAno);
}
//---------------------------------------------------------------------------
//
// Rpt New Order
//
K_mf(bool) TFsOrdFlow::DoReqRno (TOfReqRno reqRno)
{
   TOfReqRnoBase* refRno = reqRno.get();

   refRno->OrdFlow_ = this;

   //==============================================
   // 將委託資料紀錄到 OrdCenter
   refRno->OrdHnd_ = OrdCenter_->GetSysOrds()->begin_insert(fuf_OrdFlow);
   
   TFixSysOrdID sid = refRno->GetSysOrdID();
   TFsStkOrder& ord = refRno->OrdHnd_.GetModify();

   ord = refRno->StkOrd_;
   // 保留原始 Account 資料
   ord.OrigAccount_ = ord.Account_;
   
   //==============================================
   // 紀錄 ClOrdID --> SysOrdID 的關係
   // 此種型別是從別的委託衍生出來的, 所以這裡的 OrigClOrdID 就是此 SysOrdID 的對應
   //if (ordField.OrigClOrdID_.empty() == false) {
   //   Coids_->AddDat1(ordField.OrigClOrdID_, sid);
   //}

   //==============================================
   // 開放給繼承者有特殊的[委託對應]規則
   VirInsNewOrd(sid, ord);

   //==============================================
   // 紀錄委託歷程
   SOrdTrack* otrack;

   refRno->OTrackU_ = OrdTrack_->GetNewTrack(sid, SOrdTrack());
   otrack           = refRno   ->GetOrdTrack();
   
   otrack->OrdCmd_ .assignT(fcid_RptNew);
   otrack->FlowID_ .assignT(fof_P_ing);
   otrack->OrderQty_ = ord.OrderQty_;
   otrack->Price_    = ord.Price_;

   //==============================================
   // FixStkOrder
   //ord.ExecType_ .assignT(ExecType_PENDING_NEW);
   //ord.OrdStatus_.assignT(OrdStatus_PENDING_NEW);

   return DoFlowSteps(*refRno);
}
//---------------------------------------------------------------------------

K_mf(EFindOrdResult) TFsOrdFlow::VirFindOldOrd (TOfReqOldBase& refOld, TFixSysOrdID& fsoid)
{
   const SOfOrdField& ordField = refOld.GetOrdField();
   const SCoid*       cpCoid   = Coids_->FindDat(ordField.OrigClOrdID_);

   if (cpCoid == NULL) {
      // 找不到原始委託
      fsoid.assign0();
      return for_OrderNotFound;
   }
   
   fsoid = cpCoid->SysOrdID_;
   return for_Success;
}
//---------------------------------------------------------------------------

K_mf(EFindOrdResult) TFsOrdFlow::DoReqOld (TOfReqOldBase& refOld, SOrdTrack& track)
{
                        refOld.OrdFlow_ = this;
   SOfOrdField&         ordField        = refOld.OrdField_;
   const TFixOrdCmdID&  oid             = refOld.GetOrdCmdID();
   const TFixMsgSeqNum& msgSeqNum       = ordField.MsgSeqNum_;
   const TFixClOrdID&   clOrdID         = ordField.ClOrdID_;
   ECoidStatus          coidSts         = (clOrdID.empty() ? ecs_Empty : ecs_Valid);
   EFindOrdResult       retVal          = for_Success;
   TFixSysOrdID         sid;
   char                 logMsg[256];
   size_t               logLen;

   //==============================================
   // 檢查 ClOrdID 是否重覆
   if (coidSts == ecs_Valid) {
      // 1.一般正常的[刪/改]單只會執行一次 DoReqOld
      // 2.跨連線[刪/改]單,因為新委託不存在,所以會先[自動新增]委託
      //   然後在執行一次 DoReqOld,這時就可以不用在檢查了
      if (refOld.CoidU_.is_invalid() && Coids_->AddCoid(clOrdID, refOld.CoidU_) == false) {
         // ClOrdID 重覆了
         coidSts = ecs_Duplicate;
      }
   }
   else {
      // ClOrdID 是空白的, 該如何處理呢?
   }
   
   //==============================================
   // 檢查是否為 PossDupFlag
   if (oid != fcid_Qry && ordField.PossDupFlag_.is_true()) {
      if (coidSts != ecs_Valid) {
         logLen = sprintf(logMsg, "<TFsOrdFlow::DoReqOld> PossDupFlag=Y, SeqNum[%d], ClOrdID[%s] 重複 or 空白!",
                                       msgSeqNum.get_orig(), clOrdID.as_string().c_str());

         TradeLog_->LogMem(elt_Info, logMsg, logLen);
         return for_OrderDup;
      }
   }

   //==============================================
   // 確認 OrdHnd 是否存在
   if (refOld.OrdHnd_.is_invalid()) {
      //=================================
      // 尋找原始委託的 SysOrdID
      retVal = VirFindOldOrd(refOld, sid);
      if (retVal == for_OrderNotFound) {
         logLen = sprintf(logMsg, "<TFsOrdFlow::DoReqOld> 找不到原始委託! FixSeqNum[%d], ClOrdID[%s] OrigClOrdID[%s]",
                                       msgSeqNum            .get_orig(),
                                       ordField.ClOrdID_    .as_string().c_str(),
                                       ordField.OrigClOrdID_.as_string().c_str());
      
         TradeLog_->LogMem(elt_Error, logMsg, logLen);

         return for_OrderNotFound;
      }

      //=================================
      // 利用 SysOrdID 來取得 OrdHnd
      refOld.OrdHnd_ = OrdCenter_->GetOrdHnd(sid, fuf_OrdFlow);
      if (refOld.OrdHnd_.is_invalid()) {
         logLen = sprintf(logMsg, "<TFsOrdFlow::DoReqOld> 找不到OrdHnd! ClOrdID[%s] OrigClOrdID[%s] SysOrdID[%d]",
                                       ordField.ClOrdID_    .as_string().c_str(),
                                       ordField.OrigClOrdID_.as_string().c_str(),
                                       sid                  .get_orig());
      
         TradeLog_->LogMem(elt_Error, logMsg, logLen);

         return for_OrderNotFound;
      }
   }
   else {
      sid = refOld.GetSysOrdID();
   }

   //==============================================
   // 紀錄委託歷程
   track.MsgSeqNum_   = msgSeqNum;
   track.MsgType_     = refOld.FixMsgType_;
   track.ClOrdID_     = clOrdID;
   track.OrigClOrdID_ = ordField.OrigClOrdID_;

   // OrderQty
   if (ordField.OrderQty_.empty() == false) {
      //track.Flag_    .set   (otf_Has_OrderQty);
      track.OrderQty_.assign(ordField.OrderQty_.as_string());
   }
   // Price
   if (ordField.Price_.empty() == false) {
      //track.Flag_ .set   (otf_Has_Price);
      track.Price_.assign(ordField.Price_.as_string());
   }

   track.FlowID_.assignT(fof_P_ing);

   //==============================================
   // 紀錄 ClOrdID --> SysOrdID 的關係
   // 檢查 ClOrdID 是否重複
   if (coidSts == ecs_Valid) {
      SCoid* coidDat = refOld.GetCoid();
      if (coidDat) {
         coidDat->SysOrdID_    = sid;
         coidDat->OrigClOrdID_ = track.OrigClOrdID_;
      }
   }
   else if (coidSts == ecs_Duplicate) {
      return for_ClOrdIDDup;
   }

   return retVal;
}
//---------------------------------------------------------------------------
//
// Order Cancel Request
//
K_mf(bool) TFsOrdFlow::DoReqOcr (TOfReqOcr reqOcr)
{
   TOfReqOcrBase* refOcr = reqOcr.get();
   TOfReqOldBase& refOld = *refOcr;
   const Message& fixMsg = refOcr->GetFixMsg();

   //==============================================
   // 將 Fix 電文資料填入 OrdField
   GetOrdFiled(OfSetOcr_, fixMsg, refOcr->OrdField_);

   //==============================================
   // 初步檢查
   SOrdTrack      otrackTmp;
   EFindOrdResult retVal = DoReqOld(refOld, otrackTmp);

   if (retVal == for_OrderNotFound) {
      if (VirProcOrderNotFound(refOcr) == false)
         return false;

      otrackTmp.clear();
      retVal = DoReqOld(refOld, otrackTmp);
   }
   else if (retVal == for_OrderDup) {
      return true;
   }

   //==============================================
   // 以下保證 OrdHnd 一定存在
   otrackTmp.OrdCmd_.assignT(fcid_Del);
   
   //==============================================
   // 存入 OrdTrack
   TFixSysOrdID sid = refOcr->GetSysOrdID();

   refOcr->OTrackU_ = OrdTrack_->GetNewTrack(sid, otrackTmp);

   //==============================================
   // 處理前的準備
   VirBfProcOcr(refOcr);

   //==============================================
   // 初步檢查是否有 RejCode
   if (refOld.WorkRejCode_ != wrc_None) {
      ProcReqError(refOld, refOld.WorkRejCode_);
      return false;
   }

   //==============================================
   // 處理 ClOrdID 重複的狀態
   if (retVal == for_ClOrdIDDup) {
      ProcClOrdIDTheSame(fcid_Del, sid);
      return false;
   }
   // 委託內容不符
   else if (retVal == for_OrdContNotMatch) {
      ProcReqError(refOld, wrc_OrdContNotMatch);
      return false;
   }

   //==============================================
   // 開放給[繼承者]看看是否有額外的資訊要儲存
   if (VirProcOcr(refOcr) == false) {
      return false;
   }

   //==============================================
   // 最後檢查是否有 RejCode
   if (refOcr->InitRejCode_ != wrc_None) {
      ProcReqError(*refOcr, refOcr->InitRejCode_);
      return false;
   }

   //==============================================
   // 開始執行
   return DoFlowSteps(*reqOcr);
}
//---------------------------------------------------------------------------
//
// Order Cancel Replace Request
//
K_mf(bool) TFsOrdFlow::DoReqOcrr (TOfReqOcrr reqOcrr)
{
   TOfReqOcrrBase* refOcrr = reqOcrr.get();
   TOfReqOldBase&  refOld  = *refOcrr;
   SOrdTrack       otrackTmp;

   //==============================================
   // 將 Fix 電文資料填入 OrdField
   GetOrdFiled(OfSetOcrr_, refOcrr->GetFixMsg(), refOcrr->OrdField_);

   //==============================================
   // 初步檢查
   EFindOrdResult retVal = DoReqOld(refOld, otrackTmp);

   if (retVal == for_OrderNotFound) {
      if (VirProcOrderNotFound(refOcrr) == false)
         return false;

      otrackTmp.clear();
      retVal = DoReqOld(refOld, otrackTmp);
   }
   else if (retVal == for_OrderDup) {
      return true;
   }
      
   //==============================================
   // 以下保證 OrdHnd 一定存在

   //==============================================
   // 存入 OrdTrack
   TFixSysOrdID sid = refOcrr->GetSysOrdID();
   SOrdTrack*   otrack;

   refOcrr->OTrackU_ = OrdTrack_->GetNewTrack(sid, otrackTmp);
   otrack            = refOcrr->GetOrdTrack();

   //==============================================
   // 檢查是 fcid_ChgQty or fcid_ChgPri or fcid_ChgPriQty
         EFixOrdCmdID oid   = fcid_Unknown;
   const TFsStkOrder* cpOrd = refOcrr->OrdHnd_.GetConst();

   //==============================================
   // 判斷是 [改量] [改價] [改量改價]
   bool hasOrderQty = (otrack->OrderQty_.is_null() == false);
   bool hasPrice    = (otrack->Price_   .is_null() == false);
   
   if (hasOrderQty)  refOcrr->NewOrderQty_ = otrack->OrderQty_;
   if (hasPrice)     refOcrr->NewPrice_    = otrack->Price_;

   if (hasOrderQty) {
      // 證交所使用[刪減量]
      if (SpecRule_.test(osr_ChgQty_UseDec)) {
         oid = fcid_ChgQty;
      }
      // 一般 Fix 使用[剩餘量]
      else {
         if (cpOrd->OrderQty_ != refOcrr->NewOrderQty_)
            oid = fcid_ChgQty;
      }
   }

   if (hasPrice && SpecRule_.test(osr_ChgPri_Disable) == false) {
      if (cpOrd->Price_ != refOcrr->NewPrice_) {
         if (oid == fcid_ChgQty)  oid = fcid_ChgPriQty;
         else                     oid = fcid_ChgPri;
      }
   }

   //==============================================
   // 儲存 oid
   if (oid == fcid_Unknown) {
      char logMsg[128];
      int  logLen = sprintf(logMsg, "<TFixOrdFlow::DoReqOcrr> oid = fcid_Unknown");
      
      TradeLog_->LogMem(elt_Error, logMsg, logLen);
      return false;
   }
   otrack ->OrdCmd_.assignT(oid);
   refOcrr->OCmd_  .assignT(oid);
   
   //==============================================
   // 處理前的準備
   VirBfProcOcrr(refOcrr);

   //==============================================
   // 初步檢查是否有 RejCode
   if (refOld.WorkRejCode_ != wrc_None) {
      ProcReqError(refOld, refOld.WorkRejCode_);
      return false;
   }

   //==============================================
   // 處理 ClOrdID 重複的狀態
   if (retVal == for_ClOrdIDDup) {
      ProcClOrdIDTheSame(oid, sid);
      return false;
   }
   // 委託內容不符
   else if (retVal == for_OrdContNotMatch) {
      ProcReqError(refOld, wrc_OrdContNotMatch);
      return false;
   }

   //==============================================
   // 開放給[繼承者]看看是否有額外的資訊要儲存
   if (VirProcOcrr(refOcrr) == false) {
      return false;
   }

   //==============================================
   // 最後檢查是否有 RejCode
   if (refOcrr->InitRejCode_ != wrc_None) {
      ProcReqError(*refOcrr, refOcrr->InitRejCode_);
      return false;
   }

   //==============================================
   // 開始執行
   return DoFlowSteps(*refOcrr);
}
//---------------------------------------------------------------------------
//
// Order Status Request
//
K_mf(bool) TFsOrdFlow::DoReqOsr (TOfReqOsr reqOsr)
{
   TOfReqOsrBase* refOsr   = reqOsr.get();
   TOfReqOldBase& refOld   = *refOsr;
   SOfOrdField&   ordField = refOsr->OrdField_;

   //==============================================
   // 將 Fix 電文資料填入 OrdField
   GetOrdFiled(OfSetOsr_, refOsr->GetFixMsg(), ordField);
   
   // 這邊查詢是是以 ClOrdID 當作 key 來尋找原始委託
   ordField.OrigClOrdID_ = ordField.ClOrdID_;

   //==============================================
   // 初步檢查
   SOrdTrack      otrackTmp;
   EFindOrdResult retVal = DoReqOld(refOld, otrackTmp);

   if (retVal == for_OrderNotFound) {
      if (VirProcOrderNotFound(refOsr) == false)
         return false;

      otrackTmp.clear();
      retVal = DoReqOld(refOld, otrackTmp);
   }

   otrackTmp.OrdCmd_     .assignT(fcid_Qry);
   otrackTmp.OrigClOrdID_.clear  ();

   //==============================================
   // 以下保證 OrdHnd 一定存在

   //==============================================
   // 存入 OrdTrack
   TFixSysOrdID sid = refOsr->GetSysOrdID();

   refOsr->OTrackU_ = OrdTrack_->GetNewTrack(sid, otrackTmp);

   //==============================================
   // 處理前的準備
   VirBfProcOsr(refOsr);

   //==============================================
   // 初步檢查是否有 RejCode
   if (refOld.WorkRejCode_ != wrc_None) {
      ProcReqError(refOld, refOld.WorkRejCode_);
      return false;
   }

   //==============================================
   // OrderStatusRequest 不會有 ClOrdID 重複的狀態
   //if (ret == for_ClOrdIDDup) {
   //   ProcClOrdIDTheSame(fcid_Qry, sid);
   //   return false;
   //}

   // 委託內容不符
   if (retVal == for_OrdContNotMatch) {
      ProcReqError(refOld, wrc_OrdContNotMatch);
      return false;
   }
      
   //==============================================
   // 開放給[繼承者]看看是否有額外的資訊要儲存
   if (VirProcOsr(refOsr) == false) {
      return false;
   }

   //==============================================
   // 最後檢查是否有 RejCode
   if (refOsr->InitRejCode_ != wrc_None) {
      ProcReqError(*refOsr, refOsr->InitRejCode_);
      return false;
   }

   //==============================================
   // 開始執行
   return DoFlowSteps(*refOsr);
}
//---------------------------------------------------------------------------

K_mf(void) TFsOrdFlow::ProcClOrdIDTheSame (EFixOrdCmdID foid, TFixSysOrdID fsid)
{
   TRfReqReject reqReject = TRptFlow::CreateReqReject(fsid, foid, wrc_ClOrdIDTheSame);

   RptFlow_->DoReqReject(reqReject);
}
//---------------------------------------------------------------------------

K_mf(void) TFsOrdFlow::ProcReqError (TOfReqBase& refBase, int rejCode)
{
   TFixSysOrdID fsid      = refBase.GetSysOrdID();
   EFixOrdCmdID foid      = refBase.OCmd_.get_orig();
   TRfReqReject reqReject = TRptFlow::CreateReqReject(fsid, foid, rejCode);

   // 先異動一下 OrdHnd
   refBase.OrdHnd_.FlushUpdate(fuf_OrdFlow);

   RptFlow_->DoReqReject(reqReject);
}
//---------------------------------------------------------------------------

K_mf(void) TFsOrdFlow::GetOrdFiled (const TOfOrdFieldSet& fieldSet, const Message& fixMsg, SOfOrdField& field)
{
   const Header& fixHdr = fixMsg.getHeader();

   //==============================================
   // header
   FixStk_GetFixMsg (fixHdr, MsgSeqNum,   field.MsgSeqNum_)
   FixStk_GetFixMsg (fixHdr, PossDupFlag, field.PossDupFlag_)

   if (fieldSet.test(eof_SenderSubID))  FixStk_GetFixMsg(fixHdr, SenderSubID, field.SenderSubID_)
   if (fieldSet.test(eof_TargetSubID))  FixStk_GetFixMsg(fixHdr, TargetSubID, field.TargetSubID_)

   //==============================================
   // body
   if (fieldSet.test(eof_Account))      FixStk_GetFixMsg(fixMsg, Account,     field.Account_)
   if (fieldSet.test(eof_ClOrdID))      FixStk_GetFixMsg(fixMsg, ClOrdID,     field.ClOrdID_)
   if (fieldSet.test(eof_OrderID))      FixStk_GetFixMsg(fixMsg, OrderID,     field.OrderID_)
   if (fieldSet.test(eof_OrderQty))     FixStk_GetFixMsg(fixMsg, OrderQty,    field.OrderQty_)
   if (fieldSet.test(eof_OrdType))      FixStk_GetFixMsg(fixMsg, OrdType,     field.OrdType_)
   if (fieldSet.test(eof_OrigClOrdID))  FixStk_GetFixMsg(fixMsg, OrigClOrdID, field.OrigClOrdID_)
   if (fieldSet.test(eof_Price))        FixStk_GetFixMsg(fixMsg, Price,       field.Price_)
   if (fieldSet.test(eof_Side))         FixStk_GetFixMsg(fixMsg, Side,        field.Side_)
   if (fieldSet.test(eof_Symbol))       FixStk_GetFixMsg(fixMsg, Symbol,      field.Symbol_)
      
   //==============================================
   // 自訂 tag 部分
   VirOnGetOrdFiled(fixMsg, field);
}
//---------------------------------------------------------------------------
// 設定 fix 版本號
K_mf(void) TFsOrdFlow::SetFixVer (int n)
{
   TradeInfo_->SetFixVer(n); 
}
//---------------------------------------------------------------------------
