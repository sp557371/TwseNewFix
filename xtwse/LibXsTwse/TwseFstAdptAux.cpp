/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseFstAdptAux.h"
#include "ExtDef.h"
#include "Nsa_SeqNoGene.h"
#include "Nsa_DbgInfo.h"
#include "Nsa_UtilComm.h"
#include "MsgCenter.h"
#include "TwseVer.h"
#include "TwseFsOrd.h"
#include "TwseRes.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;
using namespace Kway::Tw::Stk;

/////////////////////////////////////////////////////////////////////////////
// class TTwseFstAdpt::TOrdImpl impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseFstAdpt::TOrdImpl::TOrdImpl (TTwseFstAdpt* adpt)
   : Adpt_       (adpt)
   , OrdFlow_    (adpt->GetOrdFlow  ())
   , OrdCenter_  (adpt->GetOrdCenter())
   , SysOrds_    (adpt->GetSysOrds  ())
   , RptImpl_    (NULL)
   , FixTradeLog_(adpt->GetFixTradeInfo()->GetFixTradeLog())
{
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFstAdpt::TOrdImpl::InitOrdForNew (const TFixSysOrdID& fsid, const TFixStkOrder& fixStkOrd,
                                                  TOrdFields_ForNew& ord)
{
   TBrokerID brokerID(fixStkOrd.SenderSubID_.as_string());
   TIvacNo   ivacNo  (fixStkOrd.Account_    .as_string());

   // IVacKey
   ord.IvacKey_.Set(brokerID, ivacNo);
   // StkNo
   ord.StkNo_.assign(fixStkOrd.Symbol_.as_string());
   // BSCode
   TFixTwseSide twseSide(fixStkOrd.Side_);
   ord.BSCode_ = twseSide.as_BSCode();
   // Qty
   ord.Qty_.assign(fixStkOrd.OrderQty_.get_orig());
   // Pri
   ord.Pri_.assign(fixStkOrd.Price_.as_double());
   // 交易時段
   TFixTwseTargetSubID twseTargetSubID(fixStkOrd.TargetSubID_);
   ord.TrxSegment_ = twseTargetSubID.as_TrxSegment();
   // TseOrdID
   ord.TseOrdID_.assign(fixStkOrd.OrderID_.as_string());

   //==============================================
   // TwseOrdRChar
   const STwseFsOrdReseChar* twseOrdRChar = reinterpret_cast<const STwseFsOrdReseChar*>(fixStkOrd.GetReseChar());
   // OType
   ord.OType_.assignT(twseOrdRChar->TwseOrdType_.as_OType());
   
   // 11.04.18 改成只有在新單時才需填入,所以這邊不用處理了 {
   //// TseOrderLine
   //twseOrdRChar->TwseIvacnoFlag_.as_TseOrderLine(ord.TseOrderLine_);
   // } 11.04.18

   //==============================================
   // other
   // OrigSource::ClOrdID_
   TTwseFsCnvt::FromFixSysOrdID(fsid, ord);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFstAdpt::TOrdImpl::InitOrdForRec (const TFixStkOrder& fixStkOrd, TStkOrder_Rec& ord)
{
   const STwseFsOrdReseChar* twseReseChar = reinterpret_cast<const STwseFsOrdReseChar*>(fixStkOrd.GetReseChar());

   // Market
   ord.Market_ = GetProcMkt();

   // TwseExCode.10002
   twseReseChar->TwseExCode_.as_SfGroup(ord.SfGroup_);
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFstAdpt::TOrdImpl::DoReqNew (TFixOfReqPtr fixReq)
{
   TFixOfReqNosBase*           fixRefBase = static_cast<TFixOfReqNosBase*>(fixReq.get());
   TFixStkOrder&               fixStkOrd  = fixRefBase->GetOrdHnd().GetModify();
   TFixSysOrdID                fsid       = fixRefBase->GetSysOrdID();
   TOrdFlow::TReqNew           reqNew     = TOrdFlow::CreateReqNew(static_cast<TTrProfile&>(*this));
   TOrdFlow::TReqNew::ref_obj* refNew     = reqNew.get();
   TOrdFields_ForNew&          ord        = refNew->Ord_;

   //==============================================
   // 組合新單欄位
   InitOrdForNew(fsid, fixStkOrd, ord);

   const STwseFsOrdReseChar* twseOrdRChar = reinterpret_cast<const STwseFsOrdReseChar*>(fixStkOrd.GetReseChar());

   // TseOrderLine
   twseOrdRChar->TwseIvacnoFlag_.as_TseOrderLine(ord.TseOrderLine_);
   // 順便填入 SubAcc 裡面
   // ==> SubAcc 在[刪改查]時在 TTwseFstAdpt::TOrdStepImpl::OnOrdReq 填入
   //     Tandem 回報時並不會異動, 所以這邊要先填入, 新單失敗時才不會空的
   twseOrdRChar->TwseIvacnoFlag_.as_SubAcc(ord.SubAcc_);

   // OrigSource_.UserID_
   ord.OrigSource_.UserID_.assign(Adpt_->GetTandSesID());

   //==============================================
   // 先將 req 資料儲存起來, 不然呼叫 DoReq 後就太慢了
   FixReq_[FIX::Stk::fcid_New] = fixReq;
      
   //==============================================
   // 開始執行囉
   bool bResu = OrdFlow_->DoReq(reqNew);
   if (bResu == false) {
      // 這邊要處理失敗訊息
   }

   return bResu;
}
//---------------------------------------------------------------------------
// 重送[新單]
K_mf(bool) TTwseFstAdpt::TOrdImpl::ReReqNew (TOrdCenter::TOrdHnd& ordHnd)
{
   TOrdFlow::TReqNew           reqNew = TOrdFlow::CreateReqNew(static_cast<TTrProfile&>(*this), ordHnd.GetSysOrdID());
   TOrdFlow::TReqNew::ref_obj* refNew = reqNew.get();
   TOrdFields_ForNew&          ord    = refNew->Ord_;

   ord = *(ordHnd.GetConst());

   //==============================================
   // 先將 req 資料儲存起來, 不然呼叫 DoReq 後就太慢了
   FixReq_[FIX::Stk::fcid_New] = TFixOfReqPtr();
      
   //==============================================
   // 開始執行囉
   bool bResu = OrdFlow_->DoReq(reqNew);
   if (bResu == false) {
      // 這邊要處理失敗訊息
      char logMsg[128];
      int  logLen = sprintf(logMsg, "<ReReqNew> OrdFlow DoReqNew error!");

      TUtilComm::ConsMsg(logLen, logMsg);
   }

   return bResu;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFstAdpt::TOrdImpl::DoReqAutoNew (TFixOfReqPtr fixReq)
{
   TFixOfReqAnoBase* fixRefAno = static_cast<TFixOfReqAnoBase*>(fixReq.get());
   TFixStkOrder&     fixStkOrd = fixRefAno->GetOrdHnd().GetModify();
   TFixRefOrdID      froid     = fixRefAno->GetRefOrdID();

   fixStkOrd.RefOrdID_ = VerifyOrder(fixReq, froid);
   fixRefAno->SetRefOrdID(fixStkOrd.RefOrdID_);

   return true;
}
//---------------------------------------------------------------------------
   
K_mf(TSysOrdID) TTwseFstAdpt::TOrdImpl::VerifyOrder (TFixOfReqPtr fixReq, const TFixRefOrdID& froid)
{
   TFixOfReqBase*      fixRefBase = fixReq.get();
   TFixStkOrder&       fixStkOrd  = fixRefBase->GetOrdHnd().GetModify();
   TFixSysOrdID        fsid       = fixRefBase->GetSysOrdID();
   bool                isOrdExist = (froid.is_0() ? false : true);
   TOrdCenter::TOrdHnd ordHnd;

   if (!isOrdExist)  ordHnd = SysOrds_  ->begin_insert(uf_OrdFlow);
   else              ordHnd = OrdCenter_->GetOrdHnd(TSysOrdID(froid.get_orig()), uf_OrdFlow);

   if (ordHnd.is_invalid())
      return TSysOrdID();
      
   TStkOrder& stkOrd = ordHnd.GetModify();
   
   if (!isOrdExist) {
      InitOrdForNew(fsid, fixStkOrd, stkOrd);
      InitOrdForRec(      fixStkOrd, stkOrd);
   }
   else {
      // Qty
      //if (stkOrd.Qty_.is_0())
      //   stkOrd.Qty_ = TTwseFsCnvt::FromFixQty(fixStkOrd.OrderQty_);
      // Pri
      if (stkOrd.Pri_.is_0())
         stkOrd.Pri_ = TTwseFsCnvt::FromFixPri(fixStkOrd.Price_);
      // 交易時段
      TFixTwseTargetSubID twseTargetSubID(fixStkOrd.TargetSubID_);
      stkOrd.TrxSegment_ = twseTargetSubID.as_TrxSegment();

      // 11.04.18 改成只有在新單時才需填入,所以這邊不用處理了 {
      ////===========================================
      //// TwseOrdRChar
      //const STwseFsOrdReseChar* twseOrdRChar = reinterpret_cast<const STwseFsOrdReseChar*>(fixStkOrd.GetReseChar());
      //// TseOrderLine
      //twseOrdRChar->TwseIvacnoFlag_.as_TseOrderLine(stkOrd.TseOrderLine_);
      // } 11.04.18
      
      //===========================================
      // other
      // OrigSource::ClOrdID_
      TTwseFsCnvt::FromFixSysOrdID(fsid, stkOrd);

      InitOrdForRec(fixStkOrd, stkOrd);
   }

   // FlowID_.Flow_ 預設是 of_Finish, 所以要改一下, 避免在 OnOrdAfEndUpdate 時當作已經完成了
   stkOrd.FlowID_.SetFlow(of_P_ing);

   // 02.修改成必須要從 Fix 下的新單,成交回報才需要收
   //// 01.填入 UserID,代表之後的成交回報我都要收
   //stkOrd.OrigSource_.UserID_.assign(Adpt_->GetTandSesID());

   return ordHnd.GetSysOrdID();
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFstAdpt::TOrdImpl::DoReqDel (TFixOfReqPtr fixReq)
{
   TFixOfReqOcrBase*      fixRefOcr   = static_cast<TFixOfReqOcrBase*>(fixReq.get());
   TFixStkOrder&          fixStkOrd   = fixRefOcr->GetOrdHnd().GetModify();
   SFixOrdTrack*          fixOrdTrack = fixRefOcr->GetOrdTrack();
   STwseOrdTrackReseChar* twseOtRChar = reinterpret_cast<STwseOrdTrackReseChar*>(fixOrdTrack->GetRChar());
   
   TSysOrdID         sysOrdID(fixStkOrd.RefOrdID_);
   TOrdFlow::TReqDel reqDel = TOrdFlow::CreateReqDel(static_cast<TTrProfile&>(*this), sysOrdID);
   
   // TwseWarrantLP(10003)
   // ==> sysOrd.WrntMktMkr_ = ? 延後在 OnOrdReq 指定

   FixReq_[FIX::Stk::fcid_Del] = fixReq;

   //==============================================
   // 開始執行囉
   bool bResu = OrdFlow_->DoReq(reqDel);
   if (bResu == false) {
      // 這邊要處理失敗訊息
   }

   return bResu;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFstAdpt::TOrdImpl::ReReqDel (TOrdCenter::TOrdHnd& ordHnd, const SFixOrdTrack& cdFixOt)
{
   const STwseOrdTrackReseChar* twseOtRChar = reinterpret_cast<const STwseOrdTrackReseChar*>(cdFixOt.GetRChar());
         TOrdFlow::TReqDel      reqDel      = TOrdFlow::CreateReqDel(static_cast<TTrProfile&>(*this), ordHnd.GetSysOrdID());
         TStkOrder&             udSysOrd    = ordHnd.GetModify();   
   
   // TwseWarrantLP(10003)
   // ==> sysOrd.WrntMktMkr_ = ? 因為在 OnOrdReq 會無法取得 FixOrdTrack 的資料, 所以必須在這邊處理
   if (twseOtRChar->TwseWarrantLP_.get_orig() == TwseWarrantLP_Use) {
      udSysOrd.WrntMktMkr_ = 1;
   }
   else {
      udSysOrd.WrntMktMkr_ = 0;
   }

   FixReq_[FIX::Stk::fcid_Del] = TFixOfReqPtr();

   //==============================================
   // 開始執行囉
   bool bResu = OrdFlow_->DoReq(reqDel);
   if (bResu == false) {
      // 這邊要處理失敗訊息
   }

   return bResu;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFstAdpt::TOrdImpl::DoReqChgQty (TFixOfReqPtr fixReq)
{
   TFixOfReqOcrrBase*   fixRefOcrr = static_cast<TFixOfReqOcrrBase*>(fixReq.get());
   TFixStkOrder&        fixStkOrd  = fixRefOcrr->GetOrdHnd().GetModify();
   TSysOrdID            sysOrdID(fixStkOrd.RefOrdID_);
   TStkQty              reqQty    = TTwseFsCnvt::FromFixQty(fixRefOcrr->GetNewOrderQty());
   TOrdFlow::TReqChgQty reqChgQty = TOrdFlow::CreateReqChgQty(static_cast<TTrProfile&>(*this), sysOrdID, reqQty);

   // TwseWarrantLP(10003)
   // ==> sysOrd.WrntMktMkr_ = ? 延後在 OnOrdReq 指定

   FixReq_[FIX::Stk::fcid_ChgQty] = fixReq;

   //==============================================
   // 開始執行囉
   bool bResu = OrdFlow_->DoReq(reqChgQty);
   if (bResu == false) {
      // 這邊要處理失敗訊息
   }

   return bResu;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFstAdpt::TOrdImpl::ReReqChgQty (TOrdCenter::TOrdHnd& ordHnd, const SFixOrdTrack& cdFixOt)
{
   const STwseOrdTrackReseChar* twseOtRChar = reinterpret_cast<const STwseOrdTrackReseChar*>(cdFixOt.GetRChar());
         TStkQty                reqQty      = TTwseFsCnvt::FromFixQty(cdFixOt.OrderQty_);
         TOrdFlow::TReqChgQty   reqChgQty   = TOrdFlow::CreateReqChgQty(static_cast<TTrProfile&>(*this),
                                                                        ordHnd.GetSysOrdID(),
                                                                        reqQty);
         TStkOrder&             udSysOrd    = ordHnd.GetModify();

   // TwseWarrantLP(10003)
   // ==> sysOrd.WrntMktMkr_ = ? 因為在 OnOrdReq 會無法取得 FixOrdTrack 的資料, 所以必須在這邊處理
   if (twseOtRChar->TwseWarrantLP_.get_orig() == TwseWarrantLP_Use) {
      udSysOrd.WrntMktMkr_ = 1;
   }
   else {
      udSysOrd.WrntMktMkr_ = 0;
   }

   FixReq_[FIX::Stk::fcid_ChgQty] = TFixOfReqPtr();

   //==============================================
   // 開始執行囉
   bool bResu = OrdFlow_->DoReq(reqChgQty);
   if (bResu == false) {
      // 這邊要處理失敗訊息
   }

   return bResu;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFstAdpt::TOrdImpl::DoReqQry (TFixOfReqPtr fixReq)
{
   TFixOfReqOsrBase* fixRefOsr = static_cast<TFixOfReqOsrBase*>(fixReq.get());
   TFixStkOrder&     fixStkOrd = fixRefOsr->GetOrdHnd().GetModify();
   TSysOrdID         sysOrdID(fixStkOrd.RefOrdID_);
   TOrdFlow::TReqQry reqQry = TOrdFlow::CreateReqQry(static_cast<TTrProfile&>(*this), sysOrdID);

   FixReq_[FIX::Stk::fcid_Qry] = fixReq;

   //==============================================
   // 開始執行囉
   bool bResu = OrdFlow_->DoReq(reqQry);
   if (bResu == false) {
      // 這邊要處理失敗訊息
   }

   return bResu;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFstAdpt::TOrdImpl::ReReqQry (const TSysOrdID& soid)
{
   TOrdFlow::TReqQry reqQry = TOrdFlow::CreateReqQry(static_cast<TTrProfile&>(*this), soid);

   FixReq_[FIX::Stk::fcid_Qry] = TFixOfReqPtr();

   //==============================================
   // 開始執行囉
   bool bResu = OrdFlow_->DoReq(reqQry);
   if (bResu == false) {
      // 這邊要處理失敗訊息
   }

   return bResu;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFstAdpt::TOrdImpl::OnOrdReqUpdate (TOrdFlow::TReqPtr req)
{
   inherited::OnOrdReqUpdate(req);

   TOrdFlow::TReqBase* refBase = req.get();

   //==============================================
   // 當委託流程結束後,會依照 LastOkOCmd_ 來產生不同的回報
   // LastOkOCmd_ 是必須要收到交易所回報後才會異動
   // 假設委託流程是在 TmpSes 裡面就結束了(Ex:委託時間超過)
   // 此時並不會異動 LastOkOCmd_
   // 就會造成 LastOCmd=刪單, LastOkOCmd=新單
   // ==> 所以在這邊動一下手腳,讓我的判斷可以正確無誤
   if (refBase->IsFinish()) {
      TOrdCenter::TOrdHnd& ordHnd   = refBase->GetOrdHnd();
      TStkOrder&           udSysOrd = ordHnd.GetModify();
         
      udSysOrd.LastOkOCmd_ = refBase->GetOrdCmdID();
   }

   return true;
}


/////////////////////////////////////////////////////////////////////////////
// class TTwseFstAdpt::TOrdImpl impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseFstAdpt::TOrdStepImpl::TOrdStepImpl (TTwseFstAdpt* adpt)
   : Adpt_   (adpt)
   , OrdImpl_(Adpt_->GetOrdImpl())
{
   TOrdFlow* ordFlow = Adpt_->GetOrdFlow();
   
   ordFlow->AddFlowStep(cid_New,    *this, 0);
   ordFlow->AddFlowStep(cid_Del,    *this, 0);
   ordFlow->AddFlowStep(cid_Qry,    *this, 0);
   ordFlow->AddFlowStep(cid_ChgQty, *this, 0);
}
//---------------------------------------------------------------------------

K_dtor TTwseFstAdpt::TOrdStepImpl::~TOrdStepImpl ()
{
   TOrdFlow* ordFlow = Adpt_->GetOrdFlow();

   ordFlow->DelFlowStep(cid_New,    *this);
   ordFlow->DelFlowStep(cid_Del,    *this);
   ordFlow->DelFlowStep(cid_Qry,    *this);
   ordFlow->DelFlowStep(cid_ChgQty, *this);
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFstAdpt::TOrdStepImpl::OnOrdReq (TOrdFlow::TReqPtr req)
{
   TOrdFlow::TReqBase*  refBase = static_cast<TOrdFlow::TReqBase*>(req.get());
   TOrdCenter::TOrdHnd& ordHnd  = refBase->GetOrdHnd();

   if (ordHnd.is_invalid())
      return true;

   //==============================================
   // 以下可以不考慮,原因如下
   // 01.目前 XTwse 只會服務一條 Fix 連線,所以不會有多條 Fix 連線的問題
   // 02.目前在跨連線刪改查時不會填入 OrigSource_.UserID_, 因為這些委託是不回報成交的
   /*
   //==============================================
   // 假設此時如果有服務多條 Fix 連線時
   // 因為處理 Fix 連線都是各自處理的
   // 但是 Tw::Stk::TOrdFlow 只有一個
   // 所以這個事件有可能是別人的
   // 這裡需要過濾一下
   const TStkOrder*   cdOrd     = ordHnd.GetConst();
         std::string  strUserID = cdOrd->OrigSource_.UserID_.as_string();
   const std::string& strSesID  = Adpt_->GetTandSesID();

   if (strUserID != strSesID) {
      char logMsg[256];
      
      sprintf(logMsg, "<TTwseFstAdpt::TOrdStepImpl 316> OrigSource.UserID[%s] != Adpt.SesID[%s]",
                        strUserID.c_str(), strSesID.c_str());

      TUtilComm::ConsMsg(logMsg);
      
      return true;
   }
   */

   //==============================================
   // 開始處理囉
   TStkOrder&     urOrd      = ordHnd.GetModify();
   EOrdCmdID      ecid       = (EOrdCmdID)refBase->GetOrdCmdID();
   TFixOfReqPtr*  fixReq     = NULL;
   TFixOfReqBase* fixRefBase = NULL;
   
        if (ecid == cid_New)     fixReq = OrdImpl_->GetFixReq(FIX::Stk::fcid_New);
   else if (ecid == cid_Del)     fixReq = OrdImpl_->GetFixReq(FIX::Stk::fcid_Del);
   else if (ecid == cid_ChgQty)  fixReq = OrdImpl_->GetFixReq(FIX::Stk::fcid_ChgQty);
   else if (ecid == cid_Qry)     fixReq = OrdImpl_->GetFixReq(FIX::Stk::fcid_Qry);

   if (fixReq)
      fixRefBase = fixReq->get();

   if (fixRefBase) {
      // SendingTime
      urOrd.SendingTime_ = fixRefBase->GetSendingTime();
   }
   else {
      // fixRefBase = NULL, 應該是從 ReReqxxx 來的吧
      refBase->Update(std::string(), of_P_ing);
   
      return true;
   }

   //==============================================
   // 為了落實 FixClinet 送的資料要完整送到 Tandem
   const SFixOfOrdField& fixOrdField = fixRefBase->GetOrdField();

   if (ecid != cid_New) {
      // 10000.TwseIvacnoFlag
      TFixTwseIvacnoFlag twseIvacnoFlag(fixOrdField.TwseIvacnoFlag_);
      twseIvacnoFlag.as_SubAcc(urOrd.SubAcc_);
      // 10001.TwseOrdType
      TFixTwseOrdType twseOrdType(fixOrdField.TwseOrdType_);
      urOrd.OType_.assignT(twseOrdType.as_OType());
      // 10002.TwseExCode
      TFixTwseExCode twseExCode(fixOrdField.TwseExCode_);
      twseExCode.as_SfGroup(urOrd.SfGroup_);
   }
   
   //==============================================
   // 判斷是否啟用 TwseWarrantLP(10003)
   if (ecid == cid_Del || ecid == cid_ChgQty) {
      SFixOrdTrack*          fixOrdTrack = fixRefBase->GetOrdTrack();
      STwseOrdTrackReseChar* twseOtRChar = reinterpret_cast<STwseOrdTrackReseChar*>(fixOrdTrack->GetRChar());
   
      if (twseOtRChar->TwseWarrantLP_.get_orig() == TwseWarrantLP_Use) {
         // TwseWarrantLP(10003) 啟用
         urOrd.WrntMktMkr_ = 1;
      }
      else {
         urOrd.WrntMktMkr_ = 0;
      }
   }

   if (ecid == cid_New) {
      //===========================================
      // 異動 StkOrder
      TFixOfReqNosBase* fixRefNos = static_cast<TFixOfReqNosBase*>(fixRefBase);
      TFixOrdHnd&       fixOrdHnd = fixRefNos->GetOrdHnd();
      TFixStkOrder&     fixStkOrd = fixOrdHnd.GetModify();
      
      OrdImpl_->InitOrdForRec(fixStkOrd, urOrd);
      
      //===========================================
      // 異動 FixStkOrder
      // 在這邊新增 TwStk 和 FixStk 序號對應
      fixStkOrd.RefOrdID_ = ordHnd.GetSysOrdID();

      fixRefNos->Update(std::string("委託處理中..."), FIX::Stk::fof_Sent);
      fixReq->reset();
   }
   else if (ecid == cid_Del) {
      TFixOfReqOcrBase* fixRefOcr = static_cast<TFixOfReqOcrBase*>(fixRefBase);

      fixRefOcr->Update(std::string("刪單處理中..."), FIX::Stk::fof_Sent);
      fixReq->reset();
   }
   else if (ecid == cid_ChgQty) {
      TFixOfReqOcrrBase* fixRefOcrr = static_cast<TFixOfReqOcrrBase*>(fixRefBase);
      
      fixRefOcrr->Update(std::string("改量處理中..."), FIX::Stk::fof_Sent);
      fixReq->reset();
   }
   else if (ecid == cid_Qry) {
      TFixOfReqOsrBase* fixRefOsr = static_cast<TFixOfReqOsrBase*>(fixRefBase);
      
      fixRefOsr->Update(std::string("委託查詢中..."), FIX::Stk::fof_Sent);
      fixReq->reset();
   }
   
   refBase->Update(std::string(), of_P_ing);
   
   return true;
}


/////////////////////////////////////////////////////////////////////////////
// class TTwseFstAdpt::TRptImpl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseFstAdpt::TRptImpl::TRptImpl (TTwseFstAdpt* adpt)
   : Adpt_       (adpt)
   , SysOrds_    (adpt->GetSysOrds     ())
   , FixRptFlow_ (adpt->GetFixTradeInfo()->GetFixRptFlow ())
   , FixTradeLog_(adpt->GetFixTradeInfo()->GetFixTradeLog())
   , FixVer_     (adpt->GetFixTradeInfo()->GetFixVer     ())
{
   SysOrds_->AddEvHandler(static_cast<TSysOrdsEvHandler*>(this));
}
//---------------------------------------------------------------------------

K_dtor TTwseFstAdpt::TRptImpl::~TRptImpl ()
{
   SysOrds_->DelEvHandler(static_cast<TSysOrdsEvHandler*>(this));
}
//---------------------------------------------------------------------------

K_mf(TSysOrdsEvHandler::TEvResult) TTwseFstAdpt::TRptImpl::OnOrdAfEndUpdate (TSysOrds::TUpdaterImpl& uimpl)
{
   TUpdFrom         updFrom = uimpl.GetUpdFrom();
   const TStkOrder* stkOrd  = uimpl.GetConst();
   
   if (updFrom == uf_RptDeal) {
      Exec_Deal(stkOrd, uimpl.GetLastDeal(), uimpl);
   }
   else if (updFrom == uf_OrdFlow) {
      // dlog {
      //char logMsg[512];
      //
      //sprintf(logMsg, "SysOrdID[%d], FlowID[%d]", uimpl.GetSysOrdID().get_orig(), stkOrd->FlowID_.GetFlow());
      //TUtilComm::ConsMsg(logMsg);
      // } dlog
      SRptInfo rptInfo;

      rptInfo.FlowID_      = stkOrd->FlowID_.GetFlow();
      rptInfo.CmdID_       = (EOrdCmdID)stkOrd->LastOkOCmd_;
      rptInfo.StkOrd_      = stkOrd;
      rptInfo.FixSysOrdID_ = TTwseFsCnvt::ToFixSysOrdID(stkOrd);

      Execute(rptInfo);
   }

   return er_Continue;
}
//---------------------------------------------------------------------------
// 成交回報
K_mf(void) TTwseFstAdpt::TRptImpl::Exec_Deal (const TStkOrder*        stkOrd, const TStkDeals::const_iterator& iterDeal,
                                              TSysOrds::TUpdaterImpl& uimpl)
{
   //==============================================
   // 先檢查這筆新單是不是從這邊出去的, 如果不是的話,
   // 就不傳成交回報
   const std::string  strUserID = stkOrd->OrigSource_.UserID_.as_string();
   const std::string& strSesID  = Adpt_->GetTandSesID();

   if (strUserID != strSesID) {
      //char logMsg[512];
      //sprintf(logMsg, "<TTwseFstAdpt::TRptImpl 462> OrigSource.UserID[%s] != Adpt.SesID[%s]",
      //                  strUserID.c_str(), strSesID.c_str());
      //TUtilComm::ConsMsg(logMsg);
      return;
   }

   //==============================================
   // 先取得 FixSysOrdID
   TFixSysOrdID         fixSysOrdID    = TTwseFsCnvt::ToFixSysOrdID(stkOrd);
   TFixRfReqErDeal      reqErDeal      = TFixRptFlow::CreateReqErDeal(fixSysOrdID);
   TFixRfReqErDealBase* refErDeal      = reqErDeal.get();
   SFixRfBaseField&     fixBaseField   = refErDeal->GetBaseField();
   STwseFsOrdReseChar*  fixBaseFdRChar = reinterpret_cast<STwseFsOrdReseChar*>(fixBaseField.GetReseChar());
   SFixRfErField&       fixErField     = refErDeal->GetErField  ();
   const TStkDeal&      stkDeal        = iterDeal->second;

   //==============================================
   // 001.Account
   // ==> 成交回報要使用最原始的資料
   TIvacNo ivacNo(stkOrd->IvacKey_.GetIvacNo());
   
   if (ivacNo.ToInt() != 0)
      fixBaseField.Account_.assign(ivacNo.as_string(0, true));
   // 060.TransactTime
   fixBaseField.TransactTime_.assign(TwseTransactTime(stkDeal.Time_).getValue());
   // 10000.TwseIvacnoFlag
   // ==> 成交回報要使用最原始的資料
   fixBaseFdRChar->TwseIvacnoFlag_.assign_TseOrderLine(stkOrd->TseOrderLine_);

   // 成交部分
   fixErField.LastPx_     = TTwseFsCnvt::ToFixPri(stkDeal.Pri_);
   fixErField.LastShares_ = TTwseFsCnvt::ToFixQty(stkDeal.Qty_);
   fixErField.CumQty_     = TTwseFsCnvt::ToFixQty(stkOrd->DealQty_);

   // 151.LeavesQty
   // ==> 雖然成交回報不揭示[剩餘量],但是在 TTwseFsOrdFlow::VirFindOldOrd 會使用到
   //     所以還是要給正確的資料
   //fixErField.LeavesQty_ = TTwseFsCnvt::ToFixQty(stkOrd->TMPAfQty_);
   fixErField.LeavesQty_ = TTwseFsCnvt::ToFixQty(stkOrd->DiffQty(stkOrd->TMPAfQty_, stkOrd->DealQty_));

   //==============================================
   // 委託狀態部分 (OrdStatus, ExecType)
   if (FixVer_ == 42) {
      fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_PARTIALLY_FILLED);
      fixErField  .ExecType_ .assignT(FIX::ExecType_PARTIAL_FILL);
   }
   else if (FixVer_ == 44) {
      fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_PARTIALLY_FILLED);
      fixErField  .ExecType_ .assignT(FIX::ExecType_TRADE);
   }

   // 打上 ExecID
   TTwseFsCnvt::MakeDealExexID(iterDeal, fixErField.ExecID_);

   FixRptFlow_->DoReqErDeal(reqErDeal);
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFstAdpt::TRptImpl::Execute (SRptInfo& rptInfo)
{
   switch (rptInfo.FlowID_) {
      case of_Finish:            Exec_Finish          (rptInfo);  break;
      case of_AbortOrdClear:     Exec_AbortOrdClear   (rptInfo);  break;
      case of_AbortOrdNoChange:  Exec_AbortOrdNoChange(rptInfo);  break;
      default:  return false;
   }
   return true;
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFstAdpt::TRptImpl::Exec_Finish (SRptInfo& rptInfo)
{
   switch (rptInfo.CmdID_) {
      case cid_New:     Exec_Finish_New   (rptInfo);  break;
      case cid_Del:     Exec_Finish_Del   (rptInfo);  break;
      case cid_ChgQty:  Exec_Finish_ChgQty(rptInfo);  break;
      case cid_Qry:     Exec_Finish_Qry   (rptInfo);  break;
   }
}
//---------------------------------------------------------------------------
// 新單失敗
K_mf(void) TTwseFstAdpt::TRptImpl::Exec_AbortOrdClear (SRptInfo& rptInfo)
{
   const TStkOrder*       stkOrd       = rptInfo.StkOrd_;
   TFixRfReqErReject      reqErReject  = TFixRptFlow::CreateReqErReject(rptInfo.FixSysOrdID_, FIX::Stk::fcid_New);
   TFixRfReqErRejectBase* refErReject  = reqErReject.get();
   SFixRfBaseField&       fixBaseField = refErReject->GetBaseField();
   SFixRfErField&         fixErField   = refErReject->GetErField  ();

   Set_FixBaseField(stkOrd, fixBaseField, true);
   Set_FixErField  (stkOrd, fixErField);

   fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_REJECTED);
   
   fixErField.LeavesQty_ = TTwseFsCnvt::ToFixQty(stkOrd->LeftQty());
   fixErField.ExecType_  .assignT(FIX::ExecType_REJECTED);

   // Text        ( 58)
   // OrdRejReason(103)
   // ==> LastTseCode_ 裡面只存放後兩碼的錯誤,所以要在去取前兩碼的資料
   //                  這邊會無法知道 10 or 30 開頭的訊息,所以直接使用 LastMsg_ 的好了 (1005-SYSTEM NOT READY)
   std::string text    = stkOrd->LastMsg_.as_string();
   int         stsCode = atoi(text.c_str());

   fixBaseField.Text_        .assign (text);
   fixErField  .OrdRejReason_.assignT(stsCode);

   FixRptFlow_->DoReqErReject(reqErReject);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFstAdpt::TRptImpl::Exec_AbortOrdNoChange (SRptInfo& rptInfo)
{
   switch (rptInfo.CmdID_) {
      case cid_Del:     Exec_AbortOrdNoChange_Del   (rptInfo);  break;
      case cid_ChgQty:  Exec_AbortOrdNoChange_ChgQty(rptInfo);  break;
      case cid_Qry:     Exec_AbortOrdNoChange_Qry   (rptInfo);  break;
   }
}
//---------------------------------------------------------------------------
// 新單回報
K_mf(void) TTwseFstAdpt::TRptImpl::Exec_Finish_New (SRptInfo& rptInfo)
{
   TFixRfReqErOrd      reqErOrd     = TFixRptFlow::CreateReqErOrd(rptInfo.FixSysOrdID_, FIX::Stk::fcid_New);
   TFixRfReqErOrdBase* refErOrd     = reqErOrd.get();
   SFixRfBaseField&    fixBaseField = refErOrd->GetBaseField();
   SFixRfErField&      fixErField   = refErOrd->GetErField  ();
   const TStkOrder*    stkOrd       = rptInfo.StkOrd_;
   int                 lastTseCode  = (int)stkOrd->LastTseCode_;
   TStkQty             leftQty(stkOrd->LeftQty());
   
   Set_FixBaseField(stkOrd, fixBaseField, false);
   Set_FixErField  (stkOrd, fixErField);

   // 部分有效委託
   // ==>            OrderQty(38)  LeavesQty(151)  OrdRejReason(103)  Text(58)
   //     New Order        10            n/a                n/a        n/a
   //     Execution         8              8               3031       3031...

   // LastMsg_ 裡面的訊息是 "委託OK!" 不會有 StatusCode, 所以要自己去取得
   // 委託成功應該不會有 10xx 的 StatusCode, 所以可以使用 lastTseCode 取得 StatusCode

   //==============================================
   // 判斷是否是[外資買進或借券賣委託數量被刪減],要判斷(普通,盤後,零股)喔
   bool isQuantityWasCut = false;

   if (lastTseCode != 0) {
      if ((stkOrd->TrxSegment_ == ts_Full || stkOrd->TrxSegment_ == ts_Fix) && lastTseCode == TTwseRes::IntQuantityWasCut)
         isQuantityWasCut = true;

      if (stkOrd->TrxSegment_ == ts_Odd && lastTseCode == TTwseRes::IntQuantityWasCutOdd)
         isQuantityWasCut = true;
   }

   //==============================================
   // 開始處理
   if (isQuantityWasCut == false) {
      fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_NEW);
      fixErField  .ExecType_ .assignT(FIX::ExecType_NEW);
   }
   else {
      fixBaseField.OrdStatus_.assignT(OrdStatus_PartialNew);
      fixErField  .ExecType_ .assignT(ExecType_PartialNew);
         
      fixErField.OrderQty_ = TTwseFsCnvt::ToFixQty(leftQty);
   
      std::string code = TTwseRes::GetStsCodeByTwseCode(lastTseCode, stkOrd->Market_, stkOrd->TrxSegment_);
      if (!code.empty()) {
         fixBaseField.Text_        .assign(GetErrMsg(code.c_str()));
         fixErField  .OrdRejReason_.assign(code);
      }
   }

   fixErField.LeavesQty_ = TTwseFsCnvt::ToFixQty(leftQty);
   
   FixRptFlow_->DoReqErOrd(reqErOrd);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFstAdpt::TRptImpl::Exec_Finish_Del (SRptInfo& rptInfo)
{
   Exec_Finish_Del_ChgQty(rptInfo, FIX::Stk::fcid_Del);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFstAdpt::TRptImpl::Exec_Finish_ChgQty (SRptInfo& rptInfo)
{
   Exec_Finish_Del_ChgQty(rptInfo, FIX::Stk::fcid_ChgQty);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFstAdpt::TRptImpl::Exec_Finish_Del_ChgQty (SRptInfo& rptInfo, EFixOrdCmdID foid)
{
   const TStkOrder*    stkOrd       = rptInfo.StkOrd_;
   TFixRfReqErOrd      reqErOrd     = TFixRptFlow::CreateReqErOrd(rptInfo.FixSysOrdID_, foid);
   TFixRfReqErOrdBase* refErOrd     = reqErOrd.get();
   SFixRfBaseField&    fixBaseField = refErOrd->GetBaseField();
   SFixRfErField&      fixErField   = refErOrd->GetErField  ();
   bool                isWrntMktMkr = (stkOrd->LastTseCode_ == TseCode_WrntMktMkr ? true : false);
   char                cExecType, cOrdStatus;

   Set_FixBaseField(stkOrd, fixBaseField, false);
   Set_FixErField  (stkOrd, fixErField);

   if (isWrntMktMkr) {
      // 這邊只收到Tandem回的已接收,但是不確定是否成功,所以一律給0
      fixErField.OrderQty_.assign0();
   }
   else {
      TStkQty qtyDec;

      // OrderQty
      // ==> 普通,盤後: 存放[刪/改]成功數量
      if (stkOrd->TrxSegment_ != ts_Odd) {
         qtyDec = stkOrd->LastDecQty();
      }
      // ==> 零股: 存放最後有效量(包含成交)
      // ==> 零股: 存放[刪/改]成功數量並取絕對值
      else {
         //       BfQty  AfQty
         // 減量    100     50
         // 增量     50    100
         if (stkOrd->BfQty_ > stkOrd->AfQty_)  qtyDec = TStkQty(stkOrd->BfQty_ - stkOrd->AfQty_);
         else                                  qtyDec = TStkQty(stkOrd->AfQty_ - stkOrd->BfQty_);
      }
      
      fixErField.OrderQty_ = TTwseFsCnvt::ToFixQty(qtyDec);
   }

   fixErField.LeavesQty_ = TTwseFsCnvt::ToFixQty(stkOrd->TMPAfQty_);
   fixErField.CumQty_    = TTwseFsCnvt::ToFixQty(stkOrd->DealQty_);
   
   //==============================================
   // OrdStatus ExecType
   if (FixVer_ == 42) {
      if (foid == FIX::Stk::fcid_Del) {
         cExecType  = FIX::ExecType_CANCELED;
         cOrdStatus = FIX::OrdStatus_CANCELED;
      }
      else {
         cExecType  = FIX::ExecType_REPLACE;
         cOrdStatus = FIX::OrdStatus_REPLACED;
      }
   }
   else if (FixVer_ == 44) {
      if (foid == FIX::Stk::fcid_Del) {
         cExecType  = FIX::ExecType_CANCELED;
         cOrdStatus = FIX::OrdStatus_CANCELED;
      }
      else {
         cExecType = FIX::ExecType_REPLACE;
         
         if (stkOrd->DealQty_.is_0()) {
            // 尚未有成交
            cOrdStatus = FIX::OrdStatus_NEW;
         }
         else {
            cOrdStatus = FIX::OrdStatus_PARTIALLY_FILLED;
         }
      }
   }
   
   //==============================================
   // 有可能發生
   // case 1: 30    => 255 => 凱衛自訂Code,[權證流動量提供者新式刪單減量]回報
   // case 2: 30,32 =>  32 => 取消數量超過原有數量
   //         LastTseCode_ 裡面只存放後兩碼的錯誤,所以要在去取前兩碼的資料
   if (stkOrd->LastTseCode_ == TseCode_WrntMktMkr) {
      // 10003 - TwseWarrantLP
      STwseFsOrdReseChar* twseOrdRChar = reinterpret_cast<STwseFsOrdReseChar*>(fixBaseField.GetReseChar());

      twseOrdRChar->TwseWarrantLP_.assignT(TwseWarrantLP_Use);

      // 變更 OrdStatus ExecType
      if (foid == FIX::Stk::fcid_Del) {
         cExecType  = FIX::ExecType_PENDING_CANCEL;
         cOrdStatus = FIX::OrdStatus_PENDING_CANCEL;
      }
      else {
         cExecType  = FIX::ExecType_PENDING_REPLACE;
         cOrdStatus = FIX::OrdStatus_PENDING_REPLACE;
      }
   }
   else if (stkOrd->LastTseCode_ != 0) {
      std::string errCode = TTwseRes::GetStsCodeByTwseCode((int)stkOrd->LastTseCode_, stkOrd->Market_, stkOrd->TrxSegment_);
      if (!errCode.empty()) {
         // Text        (058)
         // OrdRejReason(103)
         fixBaseField.Text_.assign(GetErrMsg(errCode.c_str()));
         
         fixErField.OrdRejReason_.assign(errCode);
      }
   }

   fixBaseField.OrdStatus_.assignT(cOrdStatus);
   fixErField  .ExecType_ .assignT(cExecType);

   FixRptFlow_->DoReqErOrd(reqErOrd);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFstAdpt::TRptImpl::Exec_Finish_Qry (SRptInfo& rptInfo)
{
   TFixRfReqErOrd      reqErOrd     = TFixRptFlow::CreateReqErOrd(rptInfo.FixSysOrdID_, FIX::Stk::fcid_Qry);
   TFixRfReqErOrdBase* refErOrd     = reqErOrd.get();
   SFixRfBaseField&    fixBaseField = refErOrd->GetBaseField();
   SFixRfErField&      fixErField   = refErOrd->GetErField  ();
   const TStkOrder*    stkOrd       = rptInfo.StkOrd_;
   TStkQty             leftQty(stkOrd->TMPAfQty_);

   Set_FixBaseField(stkOrd, fixBaseField, false);
   Set_FixErField  (stkOrd, fixErField);

   fixErField.ExecTransType_.assignT(FIX::ExecTransType_STATUS);

   //==============================================
   // 看看是否有 [成交量]
   if (FixVer_ == 42) {
      if (stkOrd->DealQty_.is_0()) {
         fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_NEW);
         fixErField  .ExecType_ .assignT(FIX::ExecType_NEW);
      }
      else {
         fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_PARTIALLY_FILLED);
         fixErField  .ExecType_ .assignT(FIX::ExecType_PARTIAL_FILL);
         fixErField  .CumQty_   = TTwseFsCnvt::ToFixQty(stkOrd->DealQty_);
      }
   }
   else if (FixVer_ == 44) {
      if (stkOrd->DealQty_.is_0()) {
         fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_NEW);
         fixErField  .ExecType_ .assignT(FIX::ExecType_ORDER_STATUS);
      }
      else {
         fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_PARTIALLY_FILLED);
         fixErField  .ExecType_ .assignT(FIX::ExecType_ORDER_STATUS);
         fixErField  .CumQty_   = TTwseFsCnvt::ToFixQty(stkOrd->DealQty_);
      }
   }
   
   TFixQty fixQty = TTwseFsCnvt::ToFixQty(leftQty);
   
   // OrderQty 存放和 LeavesQty 相同
   fixErField.OrderQty_  = fixQty;
   fixErField.LeavesQty_ = fixQty;

   FixRptFlow_->DoReqErOrd(reqErOrd);
}
//---------------------------------------------------------------------------
// 刪單失敗
K_mf(void) TTwseFstAdpt::TRptImpl::Exec_AbortOrdNoChange_Del (SRptInfo& rptInfo)
{
   TFixRfReqOcr      reqOcr       = TFixRptFlow::CreateReqOcr(rptInfo.FixSysOrdID_, FIX::Stk::fcid_Del);
   TFixRfReqOcrBase* refOcr       = reqOcr.get();
   SFixRfBaseField&  fixBaseField = refOcr->GetBaseField();
   SFixRfOcrField&   fixOcrField  = refOcr->GetOcrField ();
   const TStkOrder*  stkOrd       = rptInfo.StkOrd_;

   Set_FixBaseField(stkOrd, fixBaseField, true);

   // 決定 OrdStatus
   if (stkOrd->DealQty_.is_0()) {
      // BfQty_ = AfQty_ = 0
      // ==> 如果新單是從這邊下的,應該不會有這種組合
      //     應該是自動產生的委託,然後在利用此筆委託做刪單
      if (stkOrd->BfQty_.is_0() && stkOrd->AfQty_.is_0())
         fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_REJECTED);
      else
         fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_NEW);
   }
   else {
      fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_PARTIALLY_FILLED);
   }

   fixOcrField.CxlRejResponseTo_.assignT(FIX::CxlRejResponseTo_ORDER_CANCEL_REQUEST);

   // Text        ( 58)
   // CxlRejReason(102)
   // ==> LastTseCode_ 裡面只存放後兩碼的錯誤,所以要在去取前兩碼的資料
   //                  這邊會無法知道 10 or 30 開頭的訊息,所以直接使用 LastMsg_ 的好了 (1005-SYSTEM NOT READY)
   std::string text    = stkOrd->LastMsg_.as_string();
   int         stsCode = atoi(text.c_str());

   fixBaseField.Text_        .assign (text);
   fixOcrField .CxlRejReason_.assignT(stsCode);

   FixRptFlow_->DoReqOcr(reqOcr);
}
//---------------------------------------------------------------------------
// 減量失敗
K_mf(void) TTwseFstAdpt::TRptImpl::Exec_AbortOrdNoChange_ChgQty (SRptInfo& rptInfo)
{
   TFixRfReqOcr      reqOcr       = TFixRptFlow::CreateReqOcr(rptInfo.FixSysOrdID_, FIX::Stk::fcid_ChgQty);
   TFixRfReqOcrBase* refOcr       = reqOcr.get();
   SFixRfBaseField&  fixBaseField = refOcr->GetBaseField();
   SFixRfOcrField&   fixOcrField  = refOcr->GetOcrField ();
   const TStkOrder*  stkOrd       = rptInfo.StkOrd_;

   Set_FixBaseField(stkOrd, fixBaseField, true);

   // 決定 OrdStatus
   if (stkOrd->DealQty_.is_0()) {
      // BfQty_ = AfQty_ = 0
      // ==> 如果新單是從這邊下的,應該不會有這種組合
      //     應該是自動產生的委託,然後在利用此筆委託做減量
      if (stkOrd->BfQty_.is_0() && stkOrd->AfQty_.is_0())
         fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_REJECTED);
      else
         fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_NEW);
   }
   else {
      fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_PARTIALLY_FILLED);
   }

   fixOcrField.CxlRejResponseTo_.assignT( FIX::CxlRejResponseTo_ORDER_CANCEL_REPLACE_REQUEST);

   // Text        ( 58)
   // CxlRejReason(102)
   // ==> LastTseCode_ 裡面只存放後兩碼的錯誤,所以要在去取前兩碼的資料
   //                  這邊會無法知道 10 or 30 開頭的訊息,所以直接使用 LastMsg_ 的好了 (1005-SYSTEM NOT READY)
   std::string text    = stkOrd->LastMsg_.as_string();
   int         stsCode = atoi(text.c_str());

   fixBaseField.Text_        .assign (text);
   fixOcrField .CxlRejReason_.assignT(stsCode);

   FixRptFlow_->DoReqOcr(reqOcr);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFstAdpt::TRptImpl::Exec_AbortOrdNoChange_Qry (SRptInfo& rptInfo)
{
   const TStkOrder*       stkOrd       = rptInfo.StkOrd_;
   TFixRfReqErReject      reqErReject  = TFixRptFlow::CreateReqErReject(rptInfo.FixSysOrdID_, FIX::Stk::fcid_Qry);
   TFixRfReqErRejectBase* refErReject  = reqErReject.get();
   SFixRfBaseField&       fixBaseField = refErReject->GetBaseField();
   SFixRfErField&         fixErField   = refErReject->GetErField  ();

   Set_FixBaseField(stkOrd, fixBaseField, true);
   Set_FixErField  (stkOrd, fixErField);

   // 決定 OrdStatus
   //fixBaseField->OrdStatus_.assignT(FIX::OrdStatus_REJECTED);
   if (stkOrd->DealQty_.is_0()) {
      // BfQty_ = AfQty_ = 0
      // ==> 如果新單是從這邊下的,應該不會有這種組合
      //     應該是自動產生的委託,然後在利用此筆委託做查詢
      if (stkOrd->BfQty_.is_0() && stkOrd->AfQty_.is_0())
         fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_REJECTED);
      else
         fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_NEW);
   }
   else {
      fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_PARTIALLY_FILLED);
   }
   
   fixErField.LeavesQty_ = TTwseFsCnvt::ToFixQty(stkOrd->TMPAfQty_);
   fixErField.CumQty_    = TTwseFsCnvt::ToFixQty(stkOrd->DealQty_);

   fixErField.ExecTransType_.assignT(FIX::ExecTransType_STATUS);

   //==============================================
   // ExecType
   if (FixVer_ == 42) {
      fixErField.ExecType_.assignT(FIX::ExecType_REJECTED);
   }
   else if (FixVer_ == 44) {
      fixErField.ExecType_.assignT(FIX::ExecType_ORDER_STATUS);
   }

   // Text        ( 58)
   // CxlRejReason(102)
   // ==> LastTseCode_ 裡面只存放後兩碼的錯誤,所以要在去取前兩碼的資料
   //                  這邊會無法知道 10 or 30 開頭的訊息,所以直接使用 LastMsg_ 的好了 (1005-SYSTEM NOT READY)
   std::string text    = stkOrd->LastMsg_.as_string();
   int         stsCode = atoi(text.c_str());

   fixBaseField.Text_        .assign (text);
   fixErField  .OrdRejReason_.assignT(stsCode);

   FixRptFlow_->DoReqErReject(reqErReject);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFstAdpt::TRptImpl::Set_FixBaseField (const TStkOrder* stkOrd, SFixRfBaseField& fixBaseField, bool hasErr)
{
   //==============================================
   // 001.Account_
   // ==> 在[刪單][查詢]時沒有 Account 的欄位, 但收到交易所委回時就有資料了
   TIvacNo ivacNo(stkOrd->TMPIvacNo_);
   
   if (ivacNo.ToInt() == 0)
      ivacNo = stkOrd->IvacKey_.GetIvacNo();

   if (ivacNo.ToInt() != 0)
      fixBaseField.Account_.assign(ivacNo.as_string(0, true));

   // 060
   fixBaseField.TransactTime_.assign(TwseTransactTime(stkOrd->TseLastTime_).getValue());

   //==============================================
   // 如果不是錯誤狀態,就會得到 Tandem 的狀態
   if (hasErr)
      return;

   STwseFsOrdReseChar* twseOrdRChar = reinterpret_cast<STwseFsOrdReseChar*>(fixBaseField.GetReseChar());

   // 10000.TwseIvacnoFlag
   twseOrdRChar->TwseIvacnoFlag_.assign_SubAcc(stkOrd->SubAcc_);
   // 10001.TwseOrdType
   EOType otype = (EOType)stkOrd->OType_;

   if (otype != ot_Unknow) {
      twseOrdRChar->TwseOrdType_.assign_OType(stkOrd->OType_);
   }
   // 10002.TwseExCode
   twseOrdRChar->TwseExCode_.assign_SfGroup(stkOrd->SfGroup_, stkOrd->TrxSegment_);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFstAdpt::TRptImpl::Set_FixErField (const TStkOrder* stkOrd, SFixRfErField& fixErFd)
{
   // 044 - Price
   // 在定盤時, Cli端有可能是填入 0 or 不正確的價格, 所以在回報時回填正確的資料
   if (stkOrd->TrxSegment_ == ts_Fix) {
      fixErFd.Price_ = TTwseFsCnvt::ToFixPri(stkOrd->Pri_);
   }
}
//---------------------------------------------------------------------------
