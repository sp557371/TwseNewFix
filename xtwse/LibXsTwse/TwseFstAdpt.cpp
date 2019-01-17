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
   // 參數初始化
   TTwseFstInfo* tiTwse = static_cast<TTwseFstInfo*>(fti);
   TandSesID_ = tiTwse->GetTandSesID();

   // debug {
   TSysOrds* sysOrds = &(OrdCenter_->GetSysOrds());
   if (sysOrds != sos)
      TUtilComm::ConsMsg("<TTwseFstAdpt::TTwseFstAdpt> SysOrds pointer 位移了!");
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

   logLen = sprintf(logMsg, "初步檢查 Tw.Stk.OrdCenter, SysOrds[%d]", SysOrds_->size());
   tradeLog->LogMem(elt_Info, logMsg, logLen);
   TUtilComm::ConsMsg(logLen, logMsg);

   if (SysOrds_->empty())
      return;

   TSysOrds::iterator iter     = SysOrds_->begin();
   TSysOrds::iterator iterEnd  = SysOrds_->end  ();
   const TStkOrder*   cpSysOrd;
   EOrdFlowID         flowID;
   char               logBase[64];
   // 未傳送委託的錯誤瑪
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
      // 檢查 FlowID 是否已經完成了
      flowID = cpSysOrd->FlowID_.GetFlow();
      if (flowID >= of_Finish) {
         if (flowID == of_AbortOrdUnknow || flowID == of_LineBreakBulletin) {
            logLen = sprintf(logMsg, "%s, flowID[%d] 未處理!", logBase, flowID);
            tradeLog->LogMem(elt_Warning, logMsg, logLen);
         }
         continue;
      }

      if (flowID < of_S_ing) {
         logLen = sprintf(logMsg, "%s, [%s] flowID[%d] 尚未傳送!",
                                    logBase,
                                    cpSysOrd->TrxSegment_.as_string(true).c_str(),
                                    flowID);
         tradeLog->LogMem(elt_Info, logMsg, logLen);
         
         NotSentList_.push(soid);
      }
      else {
         // 也許已經傳送
         // ==> 我必須要等到 TMPSes 都連上了, 並且末筆查詢都結束了, 我才可以再次檢查
         logLen = sprintf(logMsg, "%s, FlowID[%d], 狀態不明,稍待在檢查!", logBase, flowID);
         tradeLog->LogMem(elt_Info, logMsg, logLen);

         DlyChkList_.push(soid);
      }
   }

   // 顯示訊息
   logLen = sprintf(logMsg, "Tw.Stk.OrdCenter 尚未送出[%d],狀態不明[%d]",
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
      // 必須要等待任何一條 TmpSes 連上線
      if (IsTMPReady() == false)
         return;

      // 檢查[未送出的委託是否已經處理完畢]
      if (Proc_NotSent() == false)
         return;
   }

   if (DlyChkList_.empty() == false) {
      // 我要等待所有的 TMPSes 都連上了才可以進行判斷
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
      // 先到 FixOrdCenter 裡面查詢委託歷程,看看是什麼還沒有回報
      TFixSysOrdID            fsoid = TTwseFsCnvt::ToFixSysOrdID(cdSysOrd);
      TFixOrdTrack::TRecsHnd  rsh(fixOrdTrack, fsoid);
      TFixOrdTrack::TRsSeqNo* rsSeqNo = rsh.FindMap2();

      sprintf(logBase, "ProcNotSent: SysOrdID[%d] FixSysOrdID[%d]",
                           soid .get_orig(),
                           fsoid.get_orig());

      if (rsSeqNo == NULL) {
         logLen = sprintf(logMsg, "%s, Fix委託歷程筆數[0]!", logBase);
         tradeLog->LogMem(elt_Warning, logMsg, logLen);
         continue;
      }

      for (TFixOrdTrack::TRsSeqNoI iter=rsSeqNo->begin(); iter!=rsSeqNo->end(); ++iter) {
               TFixSysSeqNo  fixOtKey(iter->first);
         const SFixOrdTrack& fixOtDat = iter->second;

         if (fixOtDat.FlowID_.IsFinish())
            continue;

         //========================================
         // 檢查 Fix OrdCmd 是否符合需求
         if (((fixOtDat.OrdCmd_ == FIX::Stk::fcid_New) ||
              (fixOtDat.OrdCmd_ == FIX::Stk::fcid_Qry) ||
              (fixOtDat.OrdCmd_ == FIX::Stk::fcid_Del) ||
              (fixOtDat.OrdCmd_ == FIX::Stk::fcid_ChgQty)) == false)
         {
            logLen = sprintf(logMsg, "%s, 未處理的Fix委託歷程[%d], OrdCmd[%s]!",
                                       logBase,
                                       fixOtKey.get_orig(),
                                       fixOtDat.OrdCmd_.as_desc().c_str());

            tradeLog->LogMem(elt_Warning, logMsg, logLen);
            continue;
         }

         //========================================
         // log 紀錄
         logLen = sprintf(logMsg, "%s, 重送Fix委託歷程[%d], OrdCmd[%s]!",
                                    logBase,
                                    fixOtKey.get_orig(),
                                    fixOtDat.OrdCmd_.as_desc().c_str());
         tradeLog->LogMem(elt_Info, logMsg, logLen);

         //========================================
         // 開始處理重送部份
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
   // 處理後的 log 紀錄
   logLen = sprintf(logMsg, "ProcNotSent: 剩餘筆數[%d]", NotSentList_.size());
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
   // 未傳送委託的錯誤瑪
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
         logLen = sprintf(logMsg, "%s, flowID[%d] 狀態已回補更新!", logBase, flowID);
         tradeLog->LogMem(elt_Info, logMsg, logLen);
         continue;
      }

      //===========================================
      // 末筆查詢也沒有結果,就直接回覆錯誤囉
      // ==> FlowID, LastTseCode_, LastMsg_
      TStkOrder&  udSysOrd = ordHnd.GetModify();
      std::string errCode  = TTwseRes::GetStsCodeByRejCode(tfrc_FrontEndSystemError, market, udSysOrd.TrxSegment_);
      std::string errMsg   = GetErrMsg(errCode.c_str());

      logLen = sprintf(logMsg, "%s, flowID[%d] 狀態不明,直接回覆 %s!", logBase, flowID, errCode.c_str());
      tradeLog->LogMem(elt_Info, logMsg, logLen);

      flowID = (cpSysOrd->IsTseOK() || cpSysOrd->UnknowNew_ ? of_AbortOrdNoChange : of_AbortOrdClear);
      udSysOrd.FlowID_.SetFlow(flowID);
      
      udSysOrd.LastTseCode_ = lastTseCode;
      udSysOrd.LastMsg_.assign(errMsg);
   }
}
//---------------------------------------------------------------------------
