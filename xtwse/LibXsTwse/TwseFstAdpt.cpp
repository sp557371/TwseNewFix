/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseFstAdpt.h"
#include "Nsa_UtilComm.h"
#include "ExtDef.h"
#include "MsgCenter.h"
#include "TwseFstFact.h"
#include "TwseFstAdptAux.h"
#include "TwseFstAdptOrd.h"
#include "TwseFstAdptRpt.h"
#include "TwseRes.h"
#include "TwseFsOrd.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw::Stk;

/////////////////////////////////////////////////////////////////////////////
// class TTwseFstAdpt impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseFstAdpt::TTwseFstAdpt (TFixTradeInfo* fti, TSysOrds* sos, TOrdCenter* oc, TOrdFlow* of)
   : inherited    (fti)
   , SysOrds_     (sos)
   , OrdCenter_   (oc)
   , OrdFlow_     (of)
   , OrdImpl_     (NULL)
   , RptImpl_     (NULL)
   , FixOrdImpl_  (NULL)
   , FixRptPreSet_(NULL)
   , FixRptImpl_  (NULL)
   , Timer_       (this)
   , IsReady_     (true)
{
   //==============================================
   // �Ѽƪ�l��
   TTwseFstInfo* tiTwse = static_cast<TTwseFstInfo*>(fti);
   TandSesID_ = tiTwse->GetTandSesID();

   // debug {
   TSysOrds* sysOrds = &(OrdCenter_->GetSysOrds());
   if (sysOrds != sos)
      TUtilComm::ConsMsg("<TTwseFstAdpt::TTwseFstAdpt> SysOrds pointer �첾�F!");
   // } debug
}
//---------------------------------------------------------------------------

K_dtor TTwseFstAdpt::~TTwseFstAdpt ()
{
   Timer_.Stop();

   if (FixOrdImpl_)    delete FixOrdImpl_;
   
   if (FixRptImpl_)    delete FixRptImpl_;
   if (FixRptPreSet_)  delete FixRptPreSet_;
   
   if (RptImpl_)       delete RptImpl_;
   if (OrdStepImpl_)   delete OrdStepImpl_;
   if (OrdImpl_)       delete OrdImpl_;
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFstAdpt::Startup ()
{
   OrdImpl_      = new TOrdImpl    (this);
   OrdStepImpl_  = new TOrdStepImpl(this);
   RptImpl_      = new TRptImpl    (this);
   
   FixOrdImpl_   = new TFixOrdImpl (this);

   FixRptPreSet_ = new TFixRptPreSet(this);
   FixRptImpl_   = new TFixRptImpl  (this);

   OrdImpl_->SetRptImpl(RptImpl_);
   
   PreChkOrdSts();
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFstAdpt::PreChkOrdSts ()
{
   TFixTradeLog* tradeLog = inherited::FixTradeInfo_->GetFixTradeLog();
   char          logMsg[512];
   int           logLen;

   logLen = sprintf(logMsg, "��B�ˬd Tw.Stk.OrdCenter, SysOrds[%d]", SysOrds_->size());
   tradeLog->LogMem(elt_Info, logMsg, logLen);
   TUtilComm::ConsMsg(logLen, logMsg);

   if (SysOrds_->empty())
      return;

   TSysOrds::iterator iter     = SysOrds_->begin();
   TSysOrds::iterator iterEnd  = SysOrds_->end  ();
   const TStkOrder*   cpSysOrd;
   EOrdFlowID         flowID;
   char               logBase[64];
   // ���ǰe�e�U�����~��
   Kway::byte lastTseCode = (Kway::byte)(atoi(TTwseRes::FrontEndSystemError));
   TMarket    market      = GetProcMkt();

   for (; iter!=iterEnd; ++iter) {
      TSysOrdID           soid(iter.get_idx());
      TOrdCenter::TOrdHnd ordHnd = OrdCenter_->GetOrdHnd(soid, uf_OrdFlow);
      
      sprintf(logBase, "PreChkOrdSts: SysOrdID[%d]", soid.get_orig());

      if (ordHnd.is_invalid()) {
         logLen = sprintf(logMsg, "%s, OrdHnd is invalid!", logBase);
         tradeLog->LogMem(elt_Warning, logMsg, logLen);
         continue;
      }

      cpSysOrd = ordHnd.GetConst();
      if (!cpSysOrd) {
         logLen = sprintf(logMsg, "%s, StkOrd is null!", logBase);
         tradeLog->LogMem(elt_Warning, logMsg, logLen);
         continue;
      }

      //===========================================
      // �ˬd FlowID �O�_�w�g�����F
      flowID = cpSysOrd->FlowID_.GetFlow();
      if (flowID >= of_Finish) {
         if (flowID == of_AbortOrdUnknow || flowID == of_LineBreakBulletin) {
            logLen = sprintf(logMsg, "%s, flowID[%d] ���B�z!", logBase, flowID);
            tradeLog->LogMem(elt_Warning, logMsg, logLen);
         }
         continue;
      }

      if (flowID < of_S_ing) {
         logLen = sprintf(logMsg, "%s, [%s] flowID[%d] �|���ǰe!",
                                    logBase,
                                    cpSysOrd->TrxSegment_.as_string(true).c_str(),
                                    flowID);
         tradeLog->LogMem(elt_Info, logMsg, logLen);
         
         NotSentList_.push(soid);
      }
      else {
         // �]�\�w�g�ǰe
         // ==> �ڥ����n���� TMPSes ���s�W�F, �åB�����d�߳������F, �ڤ~�i�H�A���ˬd
         logLen = sprintf(logMsg, "%s, FlowID[%d], ���A����,�y�ݦb�ˬd!", logBase, flowID);
         tradeLog->LogMem(elt_Info, logMsg, logLen);

         DlyChkList_.push(soid);
      }
   }

   // ��ܰT��
   logLen = sprintf(logMsg, "Tw.Stk.OrdCenter �|���e�X[%d],���A����[%d]",
                              NotSentList_.size(),
                              DlyChkList_ .size());
   tradeLog->LogMem(elt_Info, logMsg, logLen);
   TUtilComm::ConsMsg(logLen, logMsg);

   if (!NotSentList_.empty() || !DlyChkList_.empty()) {
      IsReady_ = false;
      Timer_.Start(10, &this_type::OnTimer);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFstAdpt::OnTimer (TTimerBase*)
{
   if (NotSentList_.empty() == false) {
      // �����n���ݥ���@�� TmpSes �s�W�u
      if (IsTMPReady() == false)
         return;

      // �ˬd[���e�X���e�U�O�_�w�g�B�z����]
      if (Proc_NotSent() == false)
         return;
   }

   if (DlyChkList_.empty() == false) {
      // �ڭn���ݩҦ��� TMPSes ���s�W�F�~�i�H�i��P�_
      if (IsAllTMPReady() == false)
         return;
         
      Proc_DlyChk();
   }

   Timer_.Stop();
   IsReady_ = true;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFstAdpt::Proc_NotSent ()
{
   if (NotSentList_.empty())
      return true;

   TFixTradeLog* tradeLog     = inherited::FixTradeInfo_->GetFixTradeLog();
   TFixOrdTrack* fixOrdTrack  = inherited::GetFixOrdTrack();
   int           cnt          = 0;
   char          logMsg[512], logBase[128];
   int           logLen;

   while (NotSentList_.empty() == false && cnt < 200) {
      TSysOrdID           soid(NotSentList_.front());
      TOrdCenter::TOrdHnd ordHnd   = OrdCenter_->GetOrdHnd(soid, uf_OrdFlow);
      const TStkOrder*    cdSysOrd = ordHnd.GetConst();

      NotSentList_.pop();

      //===========================================
      // ���� FixOrdCenter �̭��d�ߩe�U���{,�ݬݬO�����٨S���^��
      TFixSysOrdID            fsoid = TTwseFsCnvt::ToFixSysOrdID(cdSysOrd);
      TFixOrdTrack::TRecsHnd  rsh(fixOrdTrack, fsoid);
      TFixOrdTrack::TRsSeqNo* rsSeqNo = rsh.FindMap2();

      sprintf(logBase, "ProcNotSent: SysOrdID[%d] FixSysOrdID[%d]",
                           soid .get_orig(),
                           fsoid.get_orig());

      if (rsSeqNo == NULL) {
         logLen = sprintf(logMsg, "%s, Fix�e�U���{����[0]!", logBase);
         tradeLog->LogMem(elt_Warning, logMsg, logLen);
         continue;
      }

      for (TFixOrdTrack::TRsSeqNoI iter=rsSeqNo->begin(); iter!=rsSeqNo->end(); ++iter) {
               TFixSysSeqNo  fixOtKey(iter->first);
         const SFixOrdTrack& fixOtDat = iter->second;

         if (fixOtDat.FlowID_.IsFinish())
            continue;

         //========================================
         // �ˬd Fix OrdCmd �O�_�ŦX�ݨD
         if (((fixOtDat.OrdCmd_ == FIX::Stk::fcid_New) ||
              (fixOtDat.OrdCmd_ == FIX::Stk::fcid_Qry) ||
              (fixOtDat.OrdCmd_ == FIX::Stk::fcid_Del) ||
              (fixOtDat.OrdCmd_ == FIX::Stk::fcid_ChgQty)) == false)
         {
            logLen = sprintf(logMsg, "%s, ���B�z��Fix�e�U���{[%d], OrdCmd[%s]!",
                                       logBase,
                                       fixOtKey.get_orig(),
                                       fixOtDat.OrdCmd_.as_desc().c_str());

            tradeLog->LogMem(elt_Warning, logMsg, logLen);
            continue;
         }

         //========================================
         // log ����
         logLen = sprintf(logMsg, "%s, ���eFix�e�U���{[%d], OrdCmd[%s]!",
                                    logBase,
                                    fixOtKey.get_orig(),
                                    fixOtDat.OrdCmd_.as_desc().c_str());
         tradeLog->LogMem(elt_Info, logMsg, logLen);

         //========================================
         // �}�l�B�z���e����
         if (fixOtDat.OrdCmd_ == FIX::Stk::fcid_New) {
            OrdImpl_->ReReqNew(ordHnd);
         }
         else if (fixOtDat.OrdCmd_ == FIX::Stk::fcid_Del) {
            OrdImpl_->ReReqDel(ordHnd, fixOtDat);
         }
         else if (fixOtDat.OrdCmd_ == FIX::Stk::fcid_ChgQty) {
            OrdImpl_->ReReqChgQty(ordHnd, fixOtDat);
         }
         else if (fixOtDat.OrdCmd_ == FIX::Stk::fcid_Qry) {
            OrdImpl_->ReReqQry(soid);
         }
      }

      ++cnt;
   }

   //==============================================
   // �B�z�᪺ log ����
   logLen = sprintf(logMsg, "ProcNotSent: �Ѿl����[%d]", NotSentList_.size());
   tradeLog->LogMem(elt_Info, logMsg, logLen);

   return NotSentList_.empty();
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFstAdpt::Proc_DlyChk ()
{
   TFixTradeLog*    tradeLog = inherited::FixTradeInfo_->GetFixTradeLog();
   const TStkOrder* cpSysOrd;
   EOrdFlowID       flowID;
   char             logBase[64], logMsg[512];
   int              logLen;
   // ���ǰe�e�U�����~��
   Kway::byte lastTseCode = (Kway::byte)(atoi(TTwseRes::FrontEndSystemError));
   TMarket    market      = GetProcMkt();

   while (DlyChkList_.empty() == false) {
      TSysOrdID           soid(DlyChkList_.front());
      TOrdCenter::TOrdHnd ordHnd = OrdCenter_->GetOrdHnd(soid, uf_OrdFlow);
      
      sprintf(logBase, "OnTimer_DlyChk: SysOrdID[%d]", soid.get_orig());

      DlyChkList_.pop();
      cpSysOrd = ordHnd.GetConst();
      flowID   = cpSysOrd->FlowID_.GetFlow();

      if (flowID >= of_Finish) {
         logLen = sprintf(logMsg, "%s, flowID[%d] ���A�w�^�ɧ�s!", logBase, flowID);
         tradeLog->LogMem(elt_Info, logMsg, logLen);
         continue;
      }

      //===========================================
      // �����d�ߤ]�S�����G,�N�����^�п��~�o
      // ==> FlowID, LastTseCode_, LastMsg_
      TStkOrder&  udSysOrd = ordHnd.GetModify();
      std::string errCode  = TTwseRes::GetStsCodeByRejCode(tfrc_FrontEndSystemError, market, udSysOrd.TrxSegment_);
      std::string errMsg   = GetErrMsg(errCode.c_str());

      logLen = sprintf(logMsg, "%s, flowID[%d] ���A����,�����^�� %s!", logBase, flowID, errCode.c_str());
      tradeLog->LogMem(elt_Info, logMsg, logLen);

      flowID = (cpSysOrd->IsTseOK() || cpSysOrd->UnknowNew_ ? of_AbortOrdNoChange : of_AbortOrdClear);
      udSysOrd.FlowID_.SetFlow(flowID);
      
      udSysOrd.LastTseCode_ = lastTseCode;
      udSysOrd.LastMsg_.assign(errMsg);
   }
}
//---------------------------------------------------------------------------
