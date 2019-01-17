/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseFstAdptRpt.h"
// libExt
#include "ExtDef.h"
#include "MsgCenter.h"
// libxsAux
#include "Nsa_SeqNoGene.h"
#include "Nsa_UtilComm.h"
// libxsTwse
#include "TwseFsOrd.h"
#include "TwseRes.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TTwseFstAdpt::TFixRptPreSet impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseFstAdpt::TFixRptPreSet::TFixRptPreSet (TTwseFstAdpt* adpt)
   : Adpt_(adpt)
{
   TFixRptFlow* fixRptFlow = Adpt_->GetFixRptFlow();
   
   for (int i=FIX::Stk::frid_Start; i<FIX::Stk::frid_Total; ++i)
      fixRptFlow->AddFlowStep(TFixRptCmdID((FIX::Stk::EFixRptCmdID)i), this, TFixRptFlow::efp_PreSet);
}
//---------------------------------------------------------------------------

K_dtor TTwseFstAdpt::TFixRptPreSet::~TFixRptPreSet ()
{
   TFixRptFlow* fixRptFlow = Adpt_->GetFixRptFlow();

   for (int i=FIX::Stk::frid_Start; i<FIX::Stk::frid_Total; ++i)
      fixRptFlow->DelFlowStep(TFixRptCmdID((FIX::Stk::EFixRptCmdID)i), this);
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFstAdpt::TFixRptPreSet::OnRptReq (TFixRfReqPtr req)
{
   TFixRfReqBase*           fixRefBase   = req.get();
   TFixRptCmdID             frid         = fixRefBase->GetRptCmdID();
   FIX::Message&            fixMsg       = fixRefBase->GetFixMsg();
   FIX::Header&             fixHdr       = fixMsg.getHeader();
   SFixRfBaseField&         fixBaseField = fixRefBase->GetBaseField();
   TFixOrdCenter::TOrdHnd&  fixOrdHnd    = fixRefBase->GetOrdHnd();
   const TFixStkOrder*      fixStkOrd    = fixOrdHnd.GetConst();
   bool                     isEr         = frid.is_Er ();
   bool                     isOcr        = frid.is_Ocr();

   if (isEr) {
            TFixStkOrder&       fixOrdMdy   = fixOrdHnd.GetModify();
      const STwseFsOrdReseChar* fixBfRChar  = reinterpret_cast<const STwseFsOrdReseChar*>(fixBaseField.GetReseChar());
            STwseFsOrdReseChar* fixOrdRChar = reinterpret_cast<      STwseFsOrdReseChar*>(fixOrdMdy   .GetReseChar());      

      //===========================================
      // 檢查 tag 資料是否有異動
      
      // 10000.TwseIvacnoFlag
      if (!fixBfRChar->TwseIvacnoFlag_.is_null())
         fixOrdRChar->TwseIvacnoFlag_ = fixBfRChar->TwseIvacnoFlag_;
      // 10001.TwseOrdType
      if (!fixBfRChar->TwseOrdType_.is_null())
         fixOrdRChar->TwseOrdType_ = fixBfRChar->TwseOrdType_;
      //---------------------------------
      // 10002.TwseExCode
      // ==> 在[盤後定價]不會檢查 TwseExCode 資料正確性
      //     Tandem 會回覆正確的資料(Ex: FixCli 送 2, Tandem 回報為 0)
      //     所以要送出 Tandem 的狀態給 FixCli
      if (!fixBfRChar->TwseExCode_.is_null())
         fixOrdRChar->TwseExCode_ = fixBfRChar->TwseExCode_;

      if (!fixOrdRChar->TwseIvacnoFlag_.is_null())  fixMsg.setField(TwseIvacnoFlag(fixOrdRChar->TwseIvacnoFlag_.as_fix()));
      if (!fixOrdRChar->TwseOrdType_   .is_null())  fixMsg.setField(TwseOrdType   (fixOrdRChar->TwseOrdType_   .as_fix()));
      if (!fixOrdRChar->TwseExCode_    .is_null())  fixMsg.setField(TwseExCode    (fixOrdRChar->TwseExCode_    .as_fix()));

      // 10003 - TwseWarrantLP
      // ==> 並不是每次都有喔,必須要是這種委回的才有
      //     這邊我不知道委託時 Client 端是否有帶 10003
      //     所以交給 TTwseFsRptFlow::VirErOrd_FindOrdTrack 來處理囉
   }
   else if (isOcr) {
      //===========================================
      // 在 TWSE Fix 中, OrdStatus 沒有 OrdStatus_PENDING_NEW 的狀態, 應該是 AutoNew 產生的
      if (fixBaseField.OrdStatus_ == FIX::OrdStatus_PENDING_NEW)
         fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_REJECTED);
   }
  
   //==============================================
   // 以下欄位應該是不管那種回報都會用到的
   //--------------------
   // header
   // SenderSubID(050), TargetSubID(057) 這邊要互調一下
   if (fixStkOrd->TargetSubID_.empty() == false)  fixHdr.setField(FIX::SenderSubID(fixStkOrd->TargetSubID_.as_fix()));
   if (fixStkOrd->SenderSubID_.empty() == false)  fixHdr.setField(FIX::TargetSubID(fixStkOrd->SenderSubID_.as_fix()));

   // SendingTime (052) 有自己的固定格式
   fixHdr.setField(TwseSendingTime());

   //--------------------
   // body
   // TransactTime
   if (fixBaseField.TransactTime_.empty()) {
      fixBaseField.TransactTime_.assign(TwseTransactTime().getValue());
   }
   // OrderID
   fixBaseField.OrderID_ = fixStkOrd->OrderID_;

   //==============================================
   // 在 TWSE Fix 中, OrdStatus 沒有 OrdStatus_FILLED 的狀態
   if (isEr || frid == FIX::Stk::frid_Ocr) {
      if (fixBaseField.OrdStatus_ == FIX::OrdStatus_FILLED)
         fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_PARTIALLY_FILLED);
   }
   // 在 TWSE Fix 中, 原則上 OrdStatus 要等於 ExecType
   if (frid == FIX::Stk::frid_ErReject) {
      if (fixBaseField.OrdStatus_ != FIX::OrdStatus_REJECTED)
         fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_REJECTED);
   }

   //==============================================
   // 如果是 reject 訊息時, 將真正的 [RejectCode] [RejectText] 填入
   while (frid == FIX::Stk::frid_ErReject || frid == FIX::Stk::frid_Ocr) {
      int           rejCode      = FIX::Stk::wrc_None;
      int           twseCode     = 0;
      FIX::TFixInt* fixRejReason = NULL;

      if (frid == FIX::Stk::frid_ErReject) {
         TFixRfReqErRejectBase* refErReject = static_cast<TFixRfReqErRejectBase*>(fixRefBase);
         SFixRfErField&         fixErField  = refErReject->GetErField();
         
         rejCode = refErReject->GetRejectCode();
         if (rejCode != FIX::Stk::wrc_None) {
            // 代表是自訂錯誤
            fixRejReason = &(fixErField.OrdRejReason_);
         }
         else {
            if (fixErField.OrdRejReason_.is_null())
               break;

            twseCode = fixErField.OrdRejReason_.get_orig();
         }
      }
      else {
         TFixRfReqOcrBase* refOcr      = static_cast<TFixRfReqOcrBase*>(fixRefBase);
         SFixRfOcrField&   fixOcrField = refOcr->GetOcrField();
         
         rejCode = refOcr->GetRejectCode();
         if (rejCode != FIX::Stk::wrc_None) {
            // 代表是自訂錯誤
            fixRejReason = &(fixOcrField.CxlRejReason_);
         }
         else {
            if (fixOcrField.CxlRejReason_.is_null())
               break;
            twseCode = fixOcrField.CxlRejReason_.get_orig();
         }
      }

      //===========================================
      // 開始取得錯誤訊息
      TMarket     market = GetProcMkt();
      std::string code;

      //TUtilComm::ConsMsg("rejCode[%d], twseCode[%d]", rejCode, twseCode);
      
      if (rejCode != FIX::Stk::wrc_None) {
         // 代表是自訂錯誤
         code = TTwseRes::GetStsCodeByRejCode(rejCode, market, fixStkOrd->TargetSubID_);
      }
      else {
         code = TTwseRes::GetStsCodeByTwseCode(twseCode, market, fixStkOrd->TargetSubID_);
      }

      if (fixBaseField.Text_.empty())
         fixBaseField.Text_.assign(GetErrMsg(code.c_str()));
      
      if (fixRejReason)
         fixRejReason->assignT(atoi(code.c_str()));

      break;
   }

   return true;
}


/////////////////////////////////////////////////////////////////////////////
// class TTwseFstAdpt::TFixRptImpl::TExecIDGene
/////////////////////////////////////////////////////////////////////////////
class TTwseFstAdpt::TFixRptImpl::TExecIDGene
{
   Nsa::TSeqNoGene*  SeqNoGene_;

public:
   K_ctor  TExecIDGene (TTwseFstAdpt* adpt);
   K_dtor ~TExecIDGene ();

   K_mf(void) Execute (TFixRfReqErBase*);
};
/////////////////////////////////////////////////////////////////////////////
// class TTwseFstAdpt::TFixRptImpl::TExecIDGene impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseFstAdpt::TFixRptImpl::TExecIDGene::TExecIDGene (TTwseFstAdpt* adpt)
{
   bool isAutoFlush = adpt->GetFixTradeInfo()->GetFlushExecIDGene();
   char fullPath[256];

   sprintf(fullPath, "%sExecIDGene.txt", adpt->GetLogPath().c_str());
   SeqNoGene_ = new Nsa::TSeqNoGene(fullPath, false, isAutoFlush);
}
//---------------------------------------------------------------------------

K_dtor TTwseFstAdpt::TFixRptImpl::TExecIDGene::~TExecIDGene ()
{
   delete SeqNoGene_;
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFstAdpt::TFixRptImpl::TExecIDGene::Execute (TFixRfReqErBase* refEr)
{
   std::string    newSeqNo   = SeqNoGene_->GetNewStr(15);
   SFixRfErField& fixErField = refEr->GetErField();

   fixErField.ExecID_.assign(newSeqNo);
}
/////////////////////////////////////////////////////////////////////////////
// class TTwseFstAdpt::TFixRptImpl impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseFstAdpt::TFixRptImpl::TFixRptImpl (TTwseFstAdpt* adpt)
   : Adpt_      (adpt)
   , FixRptFlow_(adpt->GetFixRptFlow())
{
   ExecIDGene_ = new TExecIDGene(adpt);
   
   for (int i=FIX::Stk::frid_Start; i<FIX::Stk::frid_Total; ++i) {
      FIX::Stk::TFixRptCmdID rid((FIX::Stk::EFixRptCmdID)i);

      FixRptFlow_->AddFlowStep(rid, this);
   }
}
//---------------------------------------------------------------------------

K_dtor TTwseFstAdpt::TFixRptImpl::~TFixRptImpl ()
{
   for (int i=FIX::Stk::frid_Start; i<FIX::Stk::frid_Total; ++i) {
      FIX::Stk::TFixRptCmdID rid((FIX::Stk::EFixRptCmdID)i);

      FixRptFlow_->DelFlowStep(rid, this);
   }

   delete ExecIDGene_;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFstAdpt::TFixRptImpl::OnRptReq (TFixRfReqPtr req)
{
   TFixRfReqBase* refBase = req.get();
   EFixRptCmdID   frid    = refBase->GetRptCmdID().get_orig();

   //==============================================
   // 如果是委託回報的話, 先打上 ExecID
   if (frid == FIX::Stk::frid_ErOrd || frid == FIX::Stk::frid_ErReject) {
      TFixRfReqErBase* refErBase = static_cast<TFixRfReqErBase*>(refBase);
      
      ExecIDGene_->Execute(refErBase);
   }
   
   //==============================================
   // 後續處理
   switch (frid) {
   case FIX::Stk::frid_ErOrd:
      {
         TFixRfReqErOrdBase*  refErOrd  = static_cast<TFixRfReqErOrdBase*>(refBase);
         const TFixOrdCmdID&  foid      = refErOrd->GetOrdCmdID();
         const SFixRfErField& fixRfEfFd = refErOrd->GetErField();
         std::string          strMsg;
            
         switch (foid) {
            case FIX::Stk::fcid_New:     strMsg.assign("新單OK!");  break;
            case FIX::Stk::fcid_Qry:     strMsg.assign("查詢OK!");  break;
            case FIX::Stk::fcid_Del:
               {
                  if (fixRfEfFd.ExecType_.IsPending())  strMsg.assign("刪單已傳送!");
                  else                                  strMsg.assign("刪單OK!");
               }
               break;

            case FIX::Stk::fcid_ChgQty:
               {
                  if (fixRfEfFd.ExecType_.IsPending())  strMsg.assign("改量已傳送!");
                  else                                  strMsg.assign("改量OK!");
               }
               break;
         }

         refErOrd->Update(strMsg);
      }
      break;

   case FIX::Stk::frid_ErReject:
      {
         TFixRfReqErRejectBase* refErReject = static_cast<TFixRfReqErRejectBase*>(refBase);
         const TFixOrdCmdID&    foid        = refErReject->GetOrdCmdID();
         std::string            strMsg;

         switch (foid) {
            case FIX::Stk::fcid_New:  strMsg.assign("新單失敗!");  break;
            case FIX::Stk::fcid_Qry:  strMsg.assign("查詢失敗!");  break;
         }

         refErReject->Update(strMsg);
      }
      break;

   case FIX::Stk::frid_ErDeal:
      {
         refBase->Update(std::string());
      }
      break;
   
   case FIX::Stk::frid_Ocr:
      {
         TFixRfReqOcrBase* refOcr      = static_cast<TFixRfReqOcrBase*>(refBase);
         SFixRfOcrField&   fixOcrField = refOcr->GetOcrField();
         std::string       strMsg;

         switch (fixOcrField.CxlRejResponseTo_) {
            case FIX::CxlRejResponseTo_ORDER_CANCEL_REQUEST:          strMsg.assign("刪單失敗!");  break;
            case FIX::CxlRejResponseTo_ORDER_CANCEL_REPLACE_REQUEST:  strMsg.assign("改量失敗!");  break;
         }
         
         refOcr->Update(strMsg);
      }
      break;
   }

   return true;
}
//---------------------------------------------------------------------------
