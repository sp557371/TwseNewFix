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
   // ����ɬq
   TFixTwseTargetSubID twseTargetSubID(fixStkOrd.TargetSubID_);
   ord.TrxSegment_ = twseTargetSubID.as_TrxSegment();
   // TseOrdID
   ord.TseOrdID_.assign(fixStkOrd.OrderID_.as_string());

   //==============================================
   // TwseOrdRChar
   const STwseFsOrdReseChar* twseOrdRChar = reinterpret_cast<const STwseFsOrdReseChar*>(fixStkOrd.GetReseChar());
   // OType
   ord.OType_.assignT(twseOrdRChar->TwseOrdType_.as_OType());
   
   // 11.04.18 �令�u���b�s��ɤ~�ݶ�J,�ҥH�o�䤣�γB�z�F {
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
   // �զX�s�����
   InitOrdForNew(fsid, fixStkOrd, ord);

   const STwseFsOrdReseChar* twseOrdRChar = reinterpret_cast<const STwseFsOrdReseChar*>(fixStkOrd.GetReseChar());

   // TseOrderLine
   twseOrdRChar->TwseIvacnoFlag_.as_TseOrderLine(ord.TseOrderLine_);
   // ���K��J SubAcc �̭�
   // ==> SubAcc �b[�R��d]�ɦb TTwseFstAdpt::TOrdStepImpl::OnOrdReq ��J
   //     Tandem �^���ɨä��|����, �ҥH�o��n����J, �s�楢�Ѯɤ~���|�Ū�
   twseOrdRChar->TwseIvacnoFlag_.as_SubAcc(ord.SubAcc_);

   // OrigSource_.UserID_
   ord.OrigSource_.UserID_.assign(Adpt_->GetTandSesID());

   //==============================================
   // ���N req ����x�s�_��, ���M�I�s DoReq ��N�ӺC�F
   FixReq_[FIX::Stk::fcid_New] = fixReq;
      
   //==============================================
   // �}�l�����o
   bool bResu = OrdFlow_->DoReq(reqNew);
   if (bResu == false) {
      // �o��n�B�z���ѰT��
   }

   return bResu;
}
//---------------------------------------------------------------------------
// ���e[�s��]
K_mf(bool) TTwseFstAdpt::TOrdImpl::ReReqNew (TOrdCenter::TOrdHnd& ordHnd)
{
   TOrdFlow::TReqNew           reqNew = TOrdFlow::CreateReqNew(static_cast<TTrProfile&>(*this), ordHnd.GetSysOrdID());
   TOrdFlow::TReqNew::ref_obj* refNew = reqNew.get();
   TOrdFields_ForNew&          ord    = refNew->Ord_;

   ord = *(ordHnd.GetConst());

   //==============================================
   // ���N req ����x�s�_��, ���M�I�s DoReq ��N�ӺC�F
   FixReq_[FIX::Stk::fcid_New] = TFixOfReqPtr();
      
   //==============================================
   // �}�l�����o
   bool bResu = OrdFlow_->DoReq(reqNew);
   if (bResu == false) {
      // �o��n�B�z���ѰT��
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
      // ����ɬq
      TFixTwseTargetSubID twseTargetSubID(fixStkOrd.TargetSubID_);
      stkOrd.TrxSegment_ = twseTargetSubID.as_TrxSegment();

      // 11.04.18 �令�u���b�s��ɤ~�ݶ�J,�ҥH�o�䤣�γB�z�F {
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

   // FlowID_.Flow_ �w�]�O of_Finish, �ҥH�n��@�U, �קK�b OnOrdAfEndUpdate �ɷ�@�w�g�����F
   stkOrd.FlowID_.SetFlow(of_P_ing);

   // 02.�ק令�����n�q Fix �U���s��,����^���~�ݭn��
   //// 01.��J UserID,�N���᪺����^���ڳ��n��
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
   // ==> sysOrd.WrntMktMkr_ = ? ����b OnOrdReq ���w

   FixReq_[FIX::Stk::fcid_Del] = fixReq;

   //==============================================
   // �}�l�����o
   bool bResu = OrdFlow_->DoReq(reqDel);
   if (bResu == false) {
      // �o��n�B�z���ѰT��
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
   // ==> sysOrd.WrntMktMkr_ = ? �]���b OnOrdReq �|�L�k���o FixOrdTrack �����, �ҥH�����b�o��B�z
   if (twseOtRChar->TwseWarrantLP_.get_orig() == TwseWarrantLP_Use) {
      udSysOrd.WrntMktMkr_ = 1;
   }
   else {
      udSysOrd.WrntMktMkr_ = 0;
   }

   FixReq_[FIX::Stk::fcid_Del] = TFixOfReqPtr();

   //==============================================
   // �}�l�����o
   bool bResu = OrdFlow_->DoReq(reqDel);
   if (bResu == false) {
      // �o��n�B�z���ѰT��
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
   // ==> sysOrd.WrntMktMkr_ = ? ����b OnOrdReq ���w

   FixReq_[FIX::Stk::fcid_ChgQty] = fixReq;

   //==============================================
   // �}�l�����o
   bool bResu = OrdFlow_->DoReq(reqChgQty);
   if (bResu == false) {
      // �o��n�B�z���ѰT��
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
   // ==> sysOrd.WrntMktMkr_ = ? �]���b OnOrdReq �|�L�k���o FixOrdTrack �����, �ҥH�����b�o��B�z
   if (twseOtRChar->TwseWarrantLP_.get_orig() == TwseWarrantLP_Use) {
      udSysOrd.WrntMktMkr_ = 1;
   }
   else {
      udSysOrd.WrntMktMkr_ = 0;
   }

   FixReq_[FIX::Stk::fcid_ChgQty] = TFixOfReqPtr();

   //==============================================
   // �}�l�����o
   bool bResu = OrdFlow_->DoReq(reqChgQty);
   if (bResu == false) {
      // �o��n�B�z���ѰT��
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
   // �}�l�����o
   bool bResu = OrdFlow_->DoReq(reqQry);
   if (bResu == false) {
      // �o��n�B�z���ѰT��
   }

   return bResu;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFstAdpt::TOrdImpl::ReReqQry (const TSysOrdID& soid)
{
   TOrdFlow::TReqQry reqQry = TOrdFlow::CreateReqQry(static_cast<TTrProfile&>(*this), soid);

   FixReq_[FIX::Stk::fcid_Qry] = TFixOfReqPtr();

   //==============================================
   // �}�l�����o
   bool bResu = OrdFlow_->DoReq(reqQry);
   if (bResu == false) {
      // �o��n�B�z���ѰT��
   }

   return bResu;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFstAdpt::TOrdImpl::OnOrdReqUpdate (TOrdFlow::TReqPtr req)
{
   inherited::OnOrdReqUpdate(req);

   TOrdFlow::TReqBase* refBase = req.get();

   //==============================================
   // ��e�U�y�{������,�|�̷� LastOkOCmd_ �Ӳ��ͤ��P���^��
   // LastOkOCmd_ �O�����n�������Ҧ^����~�|����
   // ���]�e�U�y�{�O�b TmpSes �̭��N�����F(Ex:�e�U�ɶ��W�L)
   // ���ɨä��|���� LastOkOCmd_
   // �N�|�y�� LastOCmd=�R��, LastOkOCmd=�s��
   // ==> �ҥH�b�o��ʤ@�U��},���ڪ��P�_�i�H���T�L�~
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
   // �H�U�i�H���Ҽ{,��]�p�U
   // 01.�ثe XTwse �u�|�A�Ȥ@�� Fix �s�u,�ҥH���|���h�� Fix �s�u�����D
   // 02.�ثe�b��s�u�R��d�ɤ��|��J OrigSource_.UserID_, �]���o�ǩe�U�O���^�����檺
   /*
   //==============================================
   // ���]���ɦp�G���A�Ȧh�� Fix �s�u��
   // �]���B�z Fix �s�u���O�U�۳B�z��
   // ���O Tw::Stk::TOrdFlow �u���@��
   // �ҥH�o�Өƥ󦳥i��O�O�H��
   // �o�̻ݭn�L�o�@�U
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
   // �}�l�B�z�o
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
      // fixRefBase = NULL, ���ӬO�q ReReqxxx �Ӫ��a
      refBase->Update(std::string(), of_P_ing);
   
      return true;
   }

   //==============================================
   // ���F���� FixClinet �e����ƭn����e�� Tandem
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
   // �P�_�O�_�ҥ� TwseWarrantLP(10003)
   if (ecid == cid_Del || ecid == cid_ChgQty) {
      SFixOrdTrack*          fixOrdTrack = fixRefBase->GetOrdTrack();
      STwseOrdTrackReseChar* twseOtRChar = reinterpret_cast<STwseOrdTrackReseChar*>(fixOrdTrack->GetRChar());
   
      if (twseOtRChar->TwseWarrantLP_.get_orig() == TwseWarrantLP_Use) {
         // TwseWarrantLP(10003) �ҥ�
         urOrd.WrntMktMkr_ = 1;
      }
      else {
         urOrd.WrntMktMkr_ = 0;
      }
   }

   if (ecid == cid_New) {
      //===========================================
      // ���� StkOrder
      TFixOfReqNosBase* fixRefNos = static_cast<TFixOfReqNosBase*>(fixRefBase);
      TFixOrdHnd&       fixOrdHnd = fixRefNos->GetOrdHnd();
      TFixStkOrder&     fixStkOrd = fixOrdHnd.GetModify();
      
      OrdImpl_->InitOrdForRec(fixStkOrd, urOrd);
      
      //===========================================
      // ���� FixStkOrder
      // �b�o��s�W TwStk �M FixStk �Ǹ�����
      fixStkOrd.RefOrdID_ = ordHnd.GetSysOrdID();

      fixRefNos->Update(std::string("�e�U�B�z��..."), FIX::Stk::fof_Sent);
      fixReq->reset();
   }
   else if (ecid == cid_Del) {
      TFixOfReqOcrBase* fixRefOcr = static_cast<TFixOfReqOcrBase*>(fixRefBase);

      fixRefOcr->Update(std::string("�R��B�z��..."), FIX::Stk::fof_Sent);
      fixReq->reset();
   }
   else if (ecid == cid_ChgQty) {
      TFixOfReqOcrrBase* fixRefOcrr = static_cast<TFixOfReqOcrrBase*>(fixRefBase);
      
      fixRefOcrr->Update(std::string("��q�B�z��..."), FIX::Stk::fof_Sent);
      fixReq->reset();
   }
   else if (ecid == cid_Qry) {
      TFixOfReqOsrBase* fixRefOsr = static_cast<TFixOfReqOsrBase*>(fixRefBase);
      
      fixRefOsr->Update(std::string("�e�U�d�ߤ�..."), FIX::Stk::fof_Sent);
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
// ����^��
K_mf(void) TTwseFstAdpt::TRptImpl::Exec_Deal (const TStkOrder*        stkOrd, const TStkDeals::const_iterator& iterDeal,
                                              TSysOrds::TUpdaterImpl& uimpl)
{
   //==============================================
   // ���ˬd�o���s��O���O�q�o��X�h��, �p�G���O����,
   // �N���Ǧ���^��
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
   // �����o FixSysOrdID
   TFixSysOrdID         fixSysOrdID    = TTwseFsCnvt::ToFixSysOrdID(stkOrd);
   TFixRfReqErDeal      reqErDeal      = TFixRptFlow::CreateReqErDeal(fixSysOrdID);
   TFixRfReqErDealBase* refErDeal      = reqErDeal.get();
   SFixRfBaseField&     fixBaseField   = refErDeal->GetBaseField();
   STwseFsOrdReseChar*  fixBaseFdRChar = reinterpret_cast<STwseFsOrdReseChar*>(fixBaseField.GetReseChar());
   SFixRfErField&       fixErField     = refErDeal->GetErField  ();
   const TStkDeal&      stkDeal        = iterDeal->second;

   //==============================================
   // 001.Account
   // ==> ����^���n�ϥγ̭�l�����
   TIvacNo ivacNo(stkOrd->IvacKey_.GetIvacNo());
   
   if (ivacNo.ToInt() != 0)
      fixBaseField.Account_.assign(ivacNo.as_string(0, true));
   // 060.TransactTime
   fixBaseField.TransactTime_.assign(TwseTransactTime(stkDeal.Time_).getValue());
   // 10000.TwseIvacnoFlag
   // ==> ����^���n�ϥγ̭�l�����
   fixBaseFdRChar->TwseIvacnoFlag_.assign_TseOrderLine(stkOrd->TseOrderLine_);

   // ���泡��
   fixErField.LastPx_     = TTwseFsCnvt::ToFixPri(stkDeal.Pri_);
   fixErField.LastShares_ = TTwseFsCnvt::ToFixQty(stkDeal.Qty_);
   fixErField.CumQty_     = TTwseFsCnvt::ToFixQty(stkOrd->DealQty_);

   // 151.LeavesQty
   // ==> ���M����^��������[�Ѿl�q],���O�b TTwseFsOrdFlow::VirFindOldOrd �|�ϥΨ�
   //     �ҥH�٬O�n�����T�����
   //fixErField.LeavesQty_ = TTwseFsCnvt::ToFixQty(stkOrd->TMPAfQty_);
   fixErField.LeavesQty_ = TTwseFsCnvt::ToFixQty(stkOrd->DiffQty(stkOrd->TMPAfQty_, stkOrd->DealQty_));

   //==============================================
   // �e�U���A���� (OrdStatus, ExecType)
   if (FixVer_ == 42) {
      fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_PARTIALLY_FILLED);
      fixErField  .ExecType_ .assignT(FIX::ExecType_PARTIAL_FILL);
   }
   else if (FixVer_ == 44) {
      fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_PARTIALLY_FILLED);
      fixErField  .ExecType_ .assignT(FIX::ExecType_TRADE);
   }

   // ���W ExecID
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
// �s�楢��
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
   // ==> LastTseCode_ �̭��u�s����X�����~,�ҥH�n�b�h���e��X�����
   //                  �o��|�L�k���D 10 or 30 �}�Y���T��,�ҥH�����ϥ� LastMsg_ ���n�F (1005-SYSTEM NOT READY)
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
// �s��^��
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

   // �������ĩe�U
   // ==>            OrderQty(38)  LeavesQty(151)  OrdRejReason(103)  Text(58)
   //     New Order        10            n/a                n/a        n/a
   //     Execution         8              8               3031       3031...

   // LastMsg_ �̭����T���O "�e�UOK!" ���|�� StatusCode, �ҥH�n�ۤv�h���o
   // �e�U���\���Ӥ��|�� 10xx �� StatusCode, �ҥH�i�H�ϥ� lastTseCode ���o StatusCode

   //==============================================
   // �P�_�O�_�O[�~��R�i�έɨ��e�U�ƶq�Q�R��],�n�P�_(���q,�L��,�s��)��
   bool isQuantityWasCut = false;

   if (lastTseCode != 0) {
      if ((stkOrd->TrxSegment_ == ts_Full || stkOrd->TrxSegment_ == ts_Fix) && lastTseCode == TTwseRes::IntQuantityWasCut)
         isQuantityWasCut = true;

      if (stkOrd->TrxSegment_ == ts_Odd && lastTseCode == TTwseRes::IntQuantityWasCutOdd)
         isQuantityWasCut = true;
   }

   //==============================================
   // �}�l�B�z
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
      // �o��u����Tandem�^���w����,���O���T�w�O�_���\,�ҥH�@�ߵ�0
      fixErField.OrderQty_.assign0();
   }
   else {
      TStkQty qtyDec;

      // OrderQty
      // ==> ���q,�L��: �s��[�R/��]���\�ƶq
      if (stkOrd->TrxSegment_ != ts_Odd) {
         qtyDec = stkOrd->LastDecQty();
      }
      // ==> �s��: �s��̫ᦳ�Ķq(�]�t����)
      // ==> �s��: �s��[�R/��]���\�ƶq�è������
      else {
         //       BfQty  AfQty
         // ��q    100     50
         // �W�q     50    100
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
            // �|��������
            cOrdStatus = FIX::OrdStatus_NEW;
         }
         else {
            cOrdStatus = FIX::OrdStatus_PARTIALLY_FILLED;
         }
      }
   }
   
   //==============================================
   // ���i��o��
   // case 1: 30    => 255 => �ͽæۭqCode,[�v�Ҭy�ʶq���Ѫ̷s���R���q]�^��
   // case 2: 30,32 =>  32 => �����ƶq�W�L�즳�ƶq
   //         LastTseCode_ �̭��u�s����X�����~,�ҥH�n�b�h���e��X�����
   if (stkOrd->LastTseCode_ == TseCode_WrntMktMkr) {
      // 10003 - TwseWarrantLP
      STwseFsOrdReseChar* twseOrdRChar = reinterpret_cast<STwseFsOrdReseChar*>(fixBaseField.GetReseChar());

      twseOrdRChar->TwseWarrantLP_.assignT(TwseWarrantLP_Use);

      // �ܧ� OrdStatus ExecType
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
   // �ݬݬO�_�� [����q]
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
   
   // OrderQty �s��M LeavesQty �ۦP
   fixErField.OrderQty_  = fixQty;
   fixErField.LeavesQty_ = fixQty;

   FixRptFlow_->DoReqErOrd(reqErOrd);
}
//---------------------------------------------------------------------------
// �R�楢��
K_mf(void) TTwseFstAdpt::TRptImpl::Exec_AbortOrdNoChange_Del (SRptInfo& rptInfo)
{
   TFixRfReqOcr      reqOcr       = TFixRptFlow::CreateReqOcr(rptInfo.FixSysOrdID_, FIX::Stk::fcid_Del);
   TFixRfReqOcrBase* refOcr       = reqOcr.get();
   SFixRfBaseField&  fixBaseField = refOcr->GetBaseField();
   SFixRfOcrField&   fixOcrField  = refOcr->GetOcrField ();
   const TStkOrder*  stkOrd       = rptInfo.StkOrd_;

   Set_FixBaseField(stkOrd, fixBaseField, true);

   // �M�w OrdStatus
   if (stkOrd->DealQty_.is_0()) {
      // BfQty_ = AfQty_ = 0
      // ==> �p�G�s��O�q�o��U��,���Ӥ��|���o�زզX
      //     ���ӬO�۰ʲ��ͪ��e�U,�M��b�Q�Φ����e�U���R��
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
   // ==> LastTseCode_ �̭��u�s����X�����~,�ҥH�n�b�h���e��X�����
   //                  �o��|�L�k���D 10 or 30 �}�Y���T��,�ҥH�����ϥ� LastMsg_ ���n�F (1005-SYSTEM NOT READY)
   std::string text    = stkOrd->LastMsg_.as_string();
   int         stsCode = atoi(text.c_str());

   fixBaseField.Text_        .assign (text);
   fixOcrField .CxlRejReason_.assignT(stsCode);

   FixRptFlow_->DoReqOcr(reqOcr);
}
//---------------------------------------------------------------------------
// ��q����
K_mf(void) TTwseFstAdpt::TRptImpl::Exec_AbortOrdNoChange_ChgQty (SRptInfo& rptInfo)
{
   TFixRfReqOcr      reqOcr       = TFixRptFlow::CreateReqOcr(rptInfo.FixSysOrdID_, FIX::Stk::fcid_ChgQty);
   TFixRfReqOcrBase* refOcr       = reqOcr.get();
   SFixRfBaseField&  fixBaseField = refOcr->GetBaseField();
   SFixRfOcrField&   fixOcrField  = refOcr->GetOcrField ();
   const TStkOrder*  stkOrd       = rptInfo.StkOrd_;

   Set_FixBaseField(stkOrd, fixBaseField, true);

   // �M�w OrdStatus
   if (stkOrd->DealQty_.is_0()) {
      // BfQty_ = AfQty_ = 0
      // ==> �p�G�s��O�q�o��U��,���Ӥ��|���o�زզX
      //     ���ӬO�۰ʲ��ͪ��e�U,�M��b�Q�Φ����e�U����q
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
   // ==> LastTseCode_ �̭��u�s����X�����~,�ҥH�n�b�h���e��X�����
   //                  �o��|�L�k���D 10 or 30 �}�Y���T��,�ҥH�����ϥ� LastMsg_ ���n�F (1005-SYSTEM NOT READY)
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

   // �M�w OrdStatus
   //fixBaseField->OrdStatus_.assignT(FIX::OrdStatus_REJECTED);
   if (stkOrd->DealQty_.is_0()) {
      // BfQty_ = AfQty_ = 0
      // ==> �p�G�s��O�q�o��U��,���Ӥ��|���o�زզX
      //     ���ӬO�۰ʲ��ͪ��e�U,�M��b�Q�Φ����e�U���d��
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
   // ==> LastTseCode_ �̭��u�s����X�����~,�ҥH�n�b�h���e��X�����
   //                  �o��|�L�k���D 10 or 30 �}�Y���T��,�ҥH�����ϥ� LastMsg_ ���n�F (1005-SYSTEM NOT READY)
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
   // ==> �b[�R��][�d��]�ɨS�� Account �����, ���������ҩe�^�ɴN����ƤF
   TIvacNo ivacNo(stkOrd->TMPIvacNo_);
   
   if (ivacNo.ToInt() == 0)
      ivacNo = stkOrd->IvacKey_.GetIvacNo();

   if (ivacNo.ToInt() != 0)
      fixBaseField.Account_.assign(ivacNo.as_string(0, true));

   // 060
   fixBaseField.TransactTime_.assign(TwseTransactTime(stkOrd->TseLastTime_).getValue());

   //==============================================
   // �p�G���O���~���A,�N�|�o�� Tandem �����A
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
   // �b�w�L��, Cli�ݦ��i��O��J 0 or �����T������, �ҥH�b�^���ɦ^�񥿽T�����
   if (stkOrd->TrxSegment_ == ts_Fix) {
      fixErFd.Price_ = TTwseFsCnvt::ToFixPri(stkOrd->Pri_);
   }
}
//---------------------------------------------------------------------------
