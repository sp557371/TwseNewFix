/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseFstAdptOrd.h"
#include "TwseFstAdptAux.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// class TTwseFstAdpt::TFixOrdImpl impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseFstAdpt::TFixOrdImpl::TFixOrdImpl (TTwseFstAdpt* adpt)
   //: Adpt_       (adpt)
   : OrdImpl_    (adpt->GetOrdImpl   ())
   , FixOrdFlow_ (adpt->GetFixOrdFlow())
   , FixTradeLog_(adpt->GetFixTradeInfo()->GetFixTradeLog())
{
   for (int i=FIX::Stk::fcid_Start; i<FIX::Stk::fcid_Total; ++i) {
      FIX::Stk::TFixOrdCmdID cid((EFixOrdCmdID)i);

      FixOrdFlow_->AddFlowStep(cid, this);
   }
}
//---------------------------------------------------------------------------

K_dtor TTwseFstAdpt::TFixOrdImpl::~TFixOrdImpl ()
{
   for (int i=FIX::Stk::fcid_Start; i<FIX::Stk::fcid_Total; ++i) {
      FIX::Stk::TFixOrdCmdID cid((EFixOrdCmdID)i);

      FixOrdFlow_->DelFlowStep(cid, this);
   }
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFstAdpt::TFixOrdImpl::OnFsOrdReq (TFixOfReqPtr fixReq)
{
   TFixOfReqBase* fixRefBase = fixReq.get();
   EFixOrdCmdID   foid       = fixRefBase->GetOrdCmdID().get_orig();

   //==============================================
   // 先檢查對應的 Tw.Stk.TStkOrder 存不存在
   if (foid == FIX::Stk::fcid_Del    ||
       foid == FIX::Stk::fcid_ChgQty ||
       foid == FIX::Stk::fcid_Qry)
   {
      TFixOrdHnd&   fixOrdHnd = fixRefBase->GetOrdHnd();
      TFixStkOrder& fixSysOrd = fixOrdHnd.GetModify();

      if (fixSysOrd.RefOrdID_.is_0()) {
         fixSysOrd.RefOrdID_ = OrdImpl_->VerifyOrder(fixReq, fixSysOrd.RefOrdID_);
      }
   }

   switch (foid) {
   case FIX::Stk::fcid_New:
      {
         if (OrdImpl_->DoReqNew(fixReq) == false) {
            // 委託失敗
         }
      }
      break;

   case FIX::Stk::fcid_Del:
      {
         if (OrdImpl_->DoReqDel(fixReq) == false) {
            // 刪單失敗
         }
      }
      break;
      
   case FIX::Stk::fcid_ChgQty:
      {
         if (OrdImpl_->DoReqChgQty(fixReq) == false) {
            // 改量失敗
         }
      }
      break;
      
   case FIX::Stk::fcid_Qry:
      {
         if (OrdImpl_->DoReqQry(fixReq) == false) {
            // 查詢失敗
         }
      }
      break;

   case FIX::Stk::fcid_AutoNew:
      {
         //========================================
         // 這個事件是通知在 TwStkOrder 中新增一筆委託
         // 新增後即代表階段任務完成了
         TFixOfReqAnoBase* fixRefAno = static_cast<TFixOfReqAnoBase*>(fixRefBase);
            
         if (fixRefAno->IsValidOrd()) {
            // 欄位都沒問題在加到 OrdCenter 裡面
            if (OrdImpl_->DoReqAutoNew(fixReq) == false) {
               fixRefAno->Update(std::string("Auto New Failure!"), FIX::Stk::fof_AbortOrdClear);
            }
            else {
               fixRefAno->Update(std::string("Auto New Success!"), FIX::Stk::fof_Finish);
            }
         }
         else {
            fixRefAno->Update(std::string("Auto New Success!"), FIX::Stk::fof_Finish);
         }
      }
      break;
   }

   return true;
}
//---------------------------------------------------------------------------
