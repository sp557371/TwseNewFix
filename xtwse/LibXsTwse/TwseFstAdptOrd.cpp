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
   // ���ˬd������ Tw.Stk.TStkOrder �s���s�b
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
            // �e�U����
         }
      }
      break;

   case FIX::Stk::fcid_Del:
      {
         if (OrdImpl_->DoReqDel(fixReq) == false) {
            // �R�楢��
         }
      }
      break;
      
   case FIX::Stk::fcid_ChgQty:
      {
         if (OrdImpl_->DoReqChgQty(fixReq) == false) {
            // ��q����
         }
      }
      break;
      
   case FIX::Stk::fcid_Qry:
      {
         if (OrdImpl_->DoReqQry(fixReq) == false) {
            // �d�ߥ���
         }
      }
      break;

   case FIX::Stk::fcid_AutoNew:
      {
         //========================================
         // �o�Өƥ�O�q���b TwStkOrder ���s�W�@���e�U
         // �s�W��Y�N���q���ȧ����F
         TFixOfReqAnoBase* fixRefAno = static_cast<TFixOfReqAnoBase*>(fixRefBase);
            
         if (fixRefAno->IsValidOrd()) {
            // ��쳣�S���D�b�[�� OrdCenter �̭�
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
