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
      sprintf(msgbuf, "�S���]�w:[%s]�e�U�n�D���A��!", cid.as_desc().c_str());
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
   // �N Fix �q���ƶ�J OrdField
   GetOrdFiled(OfSetNos_, refNos->GetFixMsg(), ordField);

   //==============================================
   // �ˬd ClOrdID �O�_����
   const TFixMsgSeqNum& msgSeqNum = ordField.MsgSeqNum_;
   const TFixClOrdID&   clOrdID   = ordField.ClOrdID_;
   ECoidStatus          coidSts   = (clOrdID.empty() ? ecs_Empty : ecs_Valid);

   if (coidSts == ecs_Valid) {
      if (Coids_->AddCoid(clOrdID, refNos->CoidU_) == false) {
         // ClOrdID ���ФF
         coidSts = ecs_Duplicate;
      }
   }
   else {
      // ClOrdID �O�ťժ�, �Ӧp��B�z�O?
   }

   //==============================================
   // �ˬd�O�_�� PossDupFlag
   if (ordField.PossDupFlag_.is_true()) {
      if (coidSts != ecs_Valid) {
         logLen = sprintf(logMsg, "<TFsOrdFlow::DoReqNos> PossDupFlag=Y, SeqNum[%d], ClOrdID[%s] ���� or �ť�!",
                                       msgSeqNum.get_orig(), clOrdID.as_string().c_str());

         TradeLog_->LogMem(elt_Info, logMsg, logLen);
         return true;
      }
   }

   //==============================================
   // �N�e�U��Ƭ����� OrdCenter
   refNos->OrdHnd_ = OrdCenter_->GetSysOrds()->begin_insert(fuf_OrdFlow);

   TFixSysOrdID sid = refNos->GetSysOrdID();
   TFsStkOrder& ord = refNos->OrdHnd_.GetModify();

   InitOrdField(refNos->GetFixMsg(), refNos->OrdField_, ord);

   //==============================================
   // ���� ClOrdID --> SysOrdID �����Y
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
   // 02.�o��|�x�s[���ӥN��+�e�U�Ѹ�]�P FixSysOrdID �����Y,���� OnProcReqNos �̭��B�z
   //    �]���n�ˬd[���ӥN��+�e�U�Ѹ�]�O�_�O���Ъ�
   //// 01.�}���~�Ӫ̦��S��[�e�U����]�W�h
   //VirInsNewOrd(sid, ord);

   //==============================================
   // �����e�U���{
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
   // �}��[�~�Ӫ�]�ݬݬO�_���B�~����T�n�x�s
   if (OnProcReqNos(refNos) == false) {
      return false;
   }

   //==============================================
   // �B�z ClOrdID ���ƪ����p
   if (coidSts == ecs_Duplicate) {
      // �]�� OrdHnd �O�s�إߪ�, �ҥH���n Update �@�U, �o�ˤ~�|���� insert �� SysOrds �̭�
      refNos->OrdHnd_.FlushUpdate(fuf_OrdFlow);
      
      ProcClOrdIDTheSame(fcid_New, sid);
      return false;
   }

   //==============================================
   // �̫��ˬd�O�_�� RejCode
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
   // �N Fix �q���ƶ�J OrdField
   GetOrdFiled(OfSetAno_, refAno->GetFixMsg(), ordField);

   //==============================================
   // �N�e�U��Ƭ����� OrdCenter
   refAno->OrdHnd_ = OrdCenter_->GetSysOrds()->begin_insert(fuf_OrdFlow);
   
   TFixSysOrdID sid = refAno->GetSysOrdID();
   TFsStkOrder& ord = refAno->OrdHnd_.GetModify();

   InitOrdField(refAno->GetFixMsg(), ordField, ord);

   //==============================================
   // 2010.06.29 {
   // ���ɪ� OrigClOrdID ���i��O���~��
   // ���i��e�U���O�q Fix �X�h��, �� OrigClOrdID ���i��O�ö�, �ҥH�S��k��@�̾�
   //    ���� ClOrdID --> SysOrdID �����Y
   //    ���ث��O�O�q�O���e�U�l�ͥX�Ӫ�, �ҥH�o�̪� OrigClOrdID �N�O�� SysOrdID ������
   //    ���ݭn���� ClOrdID --> SysOrdID �����Y, �]�����򪺳B�z�|����
   //if (ordField.OrigClOrdID_.empty() == false) {
   //   Coids_->AddCoid(ordField.OrigClOrdID_, refAno->CoidU_, sid);
   //}
   // } 2010.06.29

   //==============================================
   // �}���~�Ӫ̦��S��[�e�U����]�W�h
   VirInsNewOrd(sid, ord);

   //==============================================
   // �����e�U���{
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
   // �N�e�U��Ƭ����� OrdCenter
   refRno->OrdHnd_ = OrdCenter_->GetSysOrds()->begin_insert(fuf_OrdFlow);
   
   TFixSysOrdID sid = refRno->GetSysOrdID();
   TFsStkOrder& ord = refRno->OrdHnd_.GetModify();

   ord = refRno->StkOrd_;
   // �O�d��l Account ���
   ord.OrigAccount_ = ord.Account_;
   
   //==============================================
   // ���� ClOrdID --> SysOrdID �����Y
   // ���ث��O�O�q�O���e�U�l�ͥX�Ӫ�, �ҥH�o�̪� OrigClOrdID �N�O�� SysOrdID ������
   //if (ordField.OrigClOrdID_.empty() == false) {
   //   Coids_->AddDat1(ordField.OrigClOrdID_, sid);
   //}

   //==============================================
   // �}���~�Ӫ̦��S��[�e�U����]�W�h
   VirInsNewOrd(sid, ord);

   //==============================================
   // �����e�U���{
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
      // �䤣���l�e�U
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
   // �ˬd ClOrdID �O�_����
   if (coidSts == ecs_Valid) {
      // 1.�@�륿�`��[�R/��]��u�|����@�� DoReqOld
      // 2.��s�u[�R/��]��,�]���s�e�U���s�b,�ҥH�|��[�۰ʷs�W]�e�U
      //   �M��b����@�� DoReqOld,�o�ɴN�i�H���Φb�ˬd�F
      if (refOld.CoidU_.is_invalid() && Coids_->AddCoid(clOrdID, refOld.CoidU_) == false) {
         // ClOrdID ���ФF
         coidSts = ecs_Duplicate;
      }
   }
   else {
      // ClOrdID �O�ťժ�, �Ӧp��B�z�O?
   }
   
   //==============================================
   // �ˬd�O�_�� PossDupFlag
   if (oid != fcid_Qry && ordField.PossDupFlag_.is_true()) {
      if (coidSts != ecs_Valid) {
         logLen = sprintf(logMsg, "<TFsOrdFlow::DoReqOld> PossDupFlag=Y, SeqNum[%d], ClOrdID[%s] ���� or �ť�!",
                                       msgSeqNum.get_orig(), clOrdID.as_string().c_str());

         TradeLog_->LogMem(elt_Info, logMsg, logLen);
         return for_OrderDup;
      }
   }

   //==============================================
   // �T�{ OrdHnd �O�_�s�b
   if (refOld.OrdHnd_.is_invalid()) {
      //=================================
      // �M���l�e�U�� SysOrdID
      retVal = VirFindOldOrd(refOld, sid);
      if (retVal == for_OrderNotFound) {
         logLen = sprintf(logMsg, "<TFsOrdFlow::DoReqOld> �䤣���l�e�U! FixSeqNum[%d], ClOrdID[%s] OrigClOrdID[%s]",
                                       msgSeqNum            .get_orig(),
                                       ordField.ClOrdID_    .as_string().c_str(),
                                       ordField.OrigClOrdID_.as_string().c_str());
      
         TradeLog_->LogMem(elt_Error, logMsg, logLen);

         return for_OrderNotFound;
      }

      //=================================
      // �Q�� SysOrdID �Ө��o OrdHnd
      refOld.OrdHnd_ = OrdCenter_->GetOrdHnd(sid, fuf_OrdFlow);
      if (refOld.OrdHnd_.is_invalid()) {
         logLen = sprintf(logMsg, "<TFsOrdFlow::DoReqOld> �䤣��OrdHnd! ClOrdID[%s] OrigClOrdID[%s] SysOrdID[%d]",
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
   // �����e�U���{
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
   // ���� ClOrdID --> SysOrdID �����Y
   // �ˬd ClOrdID �O�_����
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
   // �N Fix �q���ƶ�J OrdField
   GetOrdFiled(OfSetOcr_, fixMsg, refOcr->OrdField_);

   //==============================================
   // ��B�ˬd
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
   // �H�U�O�� OrdHnd �@�w�s�b
   otrackTmp.OrdCmd_.assignT(fcid_Del);
   
   //==============================================
   // �s�J OrdTrack
   TFixSysOrdID sid = refOcr->GetSysOrdID();

   refOcr->OTrackU_ = OrdTrack_->GetNewTrack(sid, otrackTmp);

   //==============================================
   // �B�z�e���ǳ�
   VirBfProcOcr(refOcr);

   //==============================================
   // ��B�ˬd�O�_�� RejCode
   if (refOld.WorkRejCode_ != wrc_None) {
      ProcReqError(refOld, refOld.WorkRejCode_);
      return false;
   }

   //==============================================
   // �B�z ClOrdID ���ƪ����A
   if (retVal == for_ClOrdIDDup) {
      ProcClOrdIDTheSame(fcid_Del, sid);
      return false;
   }
   // �e�U���e����
   else if (retVal == for_OrdContNotMatch) {
      ProcReqError(refOld, wrc_OrdContNotMatch);
      return false;
   }

   //==============================================
   // �}��[�~�Ӫ�]�ݬݬO�_���B�~����T�n�x�s
   if (VirProcOcr(refOcr) == false) {
      return false;
   }

   //==============================================
   // �̫��ˬd�O�_�� RejCode
   if (refOcr->InitRejCode_ != wrc_None) {
      ProcReqError(*refOcr, refOcr->InitRejCode_);
      return false;
   }

   //==============================================
   // �}�l����
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
   // �N Fix �q���ƶ�J OrdField
   GetOrdFiled(OfSetOcrr_, refOcrr->GetFixMsg(), refOcrr->OrdField_);

   //==============================================
   // ��B�ˬd
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
   // �H�U�O�� OrdHnd �@�w�s�b

   //==============================================
   // �s�J OrdTrack
   TFixSysOrdID sid = refOcrr->GetSysOrdID();
   SOrdTrack*   otrack;

   refOcrr->OTrackU_ = OrdTrack_->GetNewTrack(sid, otrackTmp);
   otrack            = refOcrr->GetOrdTrack();

   //==============================================
   // �ˬd�O fcid_ChgQty or fcid_ChgPri or fcid_ChgPriQty
         EFixOrdCmdID oid   = fcid_Unknown;
   const TFsStkOrder* cpOrd = refOcrr->OrdHnd_.GetConst();

   //==============================================
   // �P�_�O [��q] [���] [��q���]
   bool hasOrderQty = (otrack->OrderQty_.is_null() == false);
   bool hasPrice    = (otrack->Price_   .is_null() == false);
   
   if (hasOrderQty)  refOcrr->NewOrderQty_ = otrack->OrderQty_;
   if (hasPrice)     refOcrr->NewPrice_    = otrack->Price_;

   if (hasOrderQty) {
      // �ҥ�Ҩϥ�[�R��q]
      if (SpecRule_.test(osr_ChgQty_UseDec)) {
         oid = fcid_ChgQty;
      }
      // �@�� Fix �ϥ�[�Ѿl�q]
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
   // �x�s oid
   if (oid == fcid_Unknown) {
      char logMsg[128];
      int  logLen = sprintf(logMsg, "<TFixOrdFlow::DoReqOcrr> oid = fcid_Unknown");
      
      TradeLog_->LogMem(elt_Error, logMsg, logLen);
      return false;
   }
   otrack ->OrdCmd_.assignT(oid);
   refOcrr->OCmd_  .assignT(oid);
   
   //==============================================
   // �B�z�e���ǳ�
   VirBfProcOcrr(refOcrr);

   //==============================================
   // ��B�ˬd�O�_�� RejCode
   if (refOld.WorkRejCode_ != wrc_None) {
      ProcReqError(refOld, refOld.WorkRejCode_);
      return false;
   }

   //==============================================
   // �B�z ClOrdID ���ƪ����A
   if (retVal == for_ClOrdIDDup) {
      ProcClOrdIDTheSame(oid, sid);
      return false;
   }
   // �e�U���e����
   else if (retVal == for_OrdContNotMatch) {
      ProcReqError(refOld, wrc_OrdContNotMatch);
      return false;
   }

   //==============================================
   // �}��[�~�Ӫ�]�ݬݬO�_���B�~����T�n�x�s
   if (VirProcOcrr(refOcrr) == false) {
      return false;
   }

   //==============================================
   // �̫��ˬd�O�_�� RejCode
   if (refOcrr->InitRejCode_ != wrc_None) {
      ProcReqError(*refOcrr, refOcrr->InitRejCode_);
      return false;
   }

   //==============================================
   // �}�l����
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
   // �N Fix �q���ƶ�J OrdField
   GetOrdFiled(OfSetOsr_, refOsr->GetFixMsg(), ordField);
   
   // �o��d�߬O�O�H ClOrdID ��@ key �ӴM���l�e�U
   ordField.OrigClOrdID_ = ordField.ClOrdID_;

   //==============================================
   // ��B�ˬd
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
   // �H�U�O�� OrdHnd �@�w�s�b

   //==============================================
   // �s�J OrdTrack
   TFixSysOrdID sid = refOsr->GetSysOrdID();

   refOsr->OTrackU_ = OrdTrack_->GetNewTrack(sid, otrackTmp);

   //==============================================
   // �B�z�e���ǳ�
   VirBfProcOsr(refOsr);

   //==============================================
   // ��B�ˬd�O�_�� RejCode
   if (refOld.WorkRejCode_ != wrc_None) {
      ProcReqError(refOld, refOld.WorkRejCode_);
      return false;
   }

   //==============================================
   // OrderStatusRequest ���|�� ClOrdID ���ƪ����A
   //if (ret == for_ClOrdIDDup) {
   //   ProcClOrdIDTheSame(fcid_Qry, sid);
   //   return false;
   //}

   // �e�U���e����
   if (retVal == for_OrdContNotMatch) {
      ProcReqError(refOld, wrc_OrdContNotMatch);
      return false;
   }
      
   //==============================================
   // �}��[�~�Ӫ�]�ݬݬO�_���B�~����T�n�x�s
   if (VirProcOsr(refOsr) == false) {
      return false;
   }

   //==============================================
   // �̫��ˬd�O�_�� RejCode
   if (refOsr->InitRejCode_ != wrc_None) {
      ProcReqError(*refOsr, refOsr->InitRejCode_);
      return false;
   }

   //==============================================
   // �}�l����
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

   // �����ʤ@�U OrdHnd
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
   // �ۭq tag ����
   VirOnGetOrdFiled(fixMsg, field);
}
//---------------------------------------------------------------------------
// �]�w fix ������
K_mf(void) TFsOrdFlow::SetFixVer (int n)
{
   TradeInfo_->SetFixVer(n); 
}
//---------------------------------------------------------------------------
