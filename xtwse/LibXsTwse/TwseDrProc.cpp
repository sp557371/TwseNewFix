/////////////////////////////////////////////////////////////////////////////
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseDrProc.h"
#include "TwseSesApaSmp.h"
#include "TwseFsOrd.h"
#include "Nsa_UtilComm.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// class TTwseDrProc impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseDrProc::TTwseDrProc (TTwseFstInfo* tti)
   : SysOrds_    (tti->GetSysOrds    ())
   , OrdCenter_  (tti->GetOrdCenter  ())
   , FixOrdFlow_ (tti->GetFixOrdFlow ())
   , FixRptFlow_ (tti->GetFixRptFlow ())
   , TandSesID_  (tti->GetTandSesID  ())
   , FixTradeLog_(tti->GetFixTradeLog())
{
}
//---------------------------------------------------------------------------

K_mf(void) TTwseDrProc::Start (TTwseSesApaSmp* ses)
{
   size_t size = SysOrds_->size();
   char   logMsg[512];
   int    logLen;

   logLen = sprintf(logMsg, "<TwseDrProc> Start SysOrds Size[%d]", size);
   FixTradeLog_->LogMem(elt_Info, logMsg, logLen);
   TUtilComm::ConsMsg(logLen, logMsg);

   if (size == 0)
      return;

   //==============================================
   // 開始拜訪所有的委託資料
   TSysOrdID    begSoid(1);
   TSysOrdID    endSoid(size);
   Kway::uint64 msBeg = TTimerBase::GetClockMS();
   Kway::uint64 msCur;
   int          cntTimeout = 0;

   for (; begSoid<=endSoid; ++begSoid) {
      TOrdCenter::TOrdHnd ordHnd = OrdCenter_->GetOrdHnd(begSoid, Tw::Stk::uf_OrdFlow);
      if (ordHnd.is_invalid()) {
         logLen = sprintf(logMsg, "<TwseDrProc> ordHnd[%d] is_invalid!", begSoid.get_orig());
         FixTradeLog_->LogMem(elt_Error, logMsg, logLen);
         continue;
      }

      //===========================================
      // 檢查 Fix.Stk 的委託是否存在
      const TStkOrder* cpStkOrd = ordHnd.GetConst();
      if (cpStkOrd == NULL) {
         logLen = sprintf(logMsg, "<TwseDrProc> [%d]SysOrd = Null!", begSoid.get_orig());
         FixTradeLog_->LogMem(elt_Error, logMsg, logLen);
         continue;
      }

      TFixSysOrdID fsoid = TTwseFsCnvt::ToFixSysOrdID(cpStkOrd);
      if (!fsoid.is_0()) {
         logLen = sprintf(logMsg, "<TwseDrProc> SysOrdID[%d] 已處理!", begSoid.get_orig());
         FixTradeLog_->LogMem(elt_Error, logMsg, logLen);
         continue;
      }
      
      // 如果 Qty_ = 0, 代表是[成交回報]產生的委託, 這邊就不處理了
      if (cpStkOrd->Qty_.is_0()) {
         logLen = sprintf(logMsg, "<TwseDrProc> [Qty_=0][%d]代表是成交回報產生的委託,不處理!", begSoid.get_orig());
         FixTradeLog_->LogMem(elt_Info, logMsg, logLen);
         continue;
      }

      // 新單
      fsoid = AddFixNewOrd(ordHnd);
      if (fsoid.is_0()) {
         logLen = sprintf(logMsg, "<TwseDrProc> 新增委託失敗[%d]!", begSoid.get_orig());
         FixTradeLog_->LogMem(elt_Error, logMsg, logLen);
         continue;
      }
      
      // 成交回報
      if (cpStkOrd->DealQty_.is_0() == false) {
         const TStkDeals* stkDeals = cpStkOrd->Deals_.get();
         if (stkDeals) {
            AddFixDeal(fsoid, cpStkOrd, stkDeals);
         }
      }
      
      // 最後狀態更新
      AddFixFinal(fsoid, cpStkOrd);

      // 計時,為了避免程式卡死
      msCur = TTimerBase::GetClockMS();
      if ((msCur - msBeg) > 100) {
         ses->DrPolling();
         TSyncObj::CheckSync();

         ++cntTimeout;
         if (cntTimeout >= 5) {
            cntTimeout = 0;
         
            logLen = sprintf(logMsg, "<TwseDrProc> Curr SysOrdID[%d]", begSoid.get_orig());
            FixTradeLog_->LogMem(elt_Info, logMsg, logLen);
            TUtilComm::ConsMsg(logLen, logMsg);
         }

         msBeg = TTimerBase::GetClockMS();
      }
   }

   logLen = sprintf(logMsg, "<TwseDrProc> Finish!");
   FixTradeLog_->LogMem(elt_Info, logMsg, logLen);
   TUtilComm::ConsMsg(logLen, logMsg);
}
//---------------------------------------------------------------------------

K_mf(TTwseDrProc::TFixSysOrdID) TTwseDrProc::AddFixNewOrd (TOrdCenter::TOrdHnd& ordHnd)
{
   TFixOfReqRno       fixReqRno  = TFixOrdFlow::CreateReqRno();
   TFixOfReqRnoBase*  fixRefRno  = fixReqRno.get();
   TFsStkOrder&       fixStkOrd  = fixRefRno->GetStkOrd  ();
   SFixOfRnoField&    fixOfRnoFd = fixRefRno->GetRnoField();
   TStkOrder&         stkOrd     = ordHnd.GetModify();
   TSysOrdID          soid       = ordHnd.GetSysOrdID();
   char               logMsg[512];
   int                logLen;
   
   //logLen = sprintf(logMsg, "<TTwseDrProc::AddFixNewOrd> enter, soid[%d]", soid.get_orig());
   //FixTradeLog_->LogMem(elt_Info, logMsg, logLen);
   
   // 050.SenderSubID
   fixStkOrd.SenderSubID_.assign(stkOrd.IvacKey_.GetBrokerID().as_string());
   // 057.TargetSubID
   TFixTwseTargetSubID twseTargetSubID(stkOrd.TrxSegment_);
   fixStkOrd.TargetSubID_.assign(twseTargetSubID.as_string());
   // 001.Account
   fixStkOrd.Account_.assign(stkOrd.IvacKey_.GetIvacNo().as_string(0, true));
   // 037.OrderID
   fixStkOrd.OrderID_.assign(stkOrd.TseOrdID_.as_string(true));
   // 038.OrderQty
   fixStkOrd.OrderQty_ = TTwseFsCnvt::ToFixQty(stkOrd.Qty_);
   // 044.Price
   fixStkOrd.Price_ = TTwseFsCnvt::ToFixPri(stkOrd.Pri_);
   // 054.Side
   TFixTwseSide twseSide(stkOrd.BSCode_);
   fixStkOrd.Side_.assign(twseSide.as_string());
   // 055.Symbol
   fixStkOrd.Symbol_.assign(stkOrd.StkNo_.as_string());
   // TwseOrdRChar
   STwseFsOrdReseChar* twseOrdRChar = reinterpret_cast<STwseFsOrdReseChar*>(fixStkOrd.GetReseChar());
   // 10000.TwseIvacnoFlag
   twseOrdRChar->TwseIvacnoFlag_.assign_TseOrderLine(stkOrd.TseOrderLine_);
   // 10001.TwseOrdType
   twseOrdRChar->TwseOrdType_.assign_OType(stkOrd.OType_);
   // 10002.TwseExCode
   twseOrdRChar->TwseExCode_.assign_SfGroup(stkOrd.SfGroup_, stkOrd.TrxSegment_);

   //==============================================
   // 固定的資料
   // 021.HandlInst
   fixStkOrd.HandlInst_.assignT(TwseHandlInst_Default);
   // 059.TimeInForce
   fixStkOrd.TimeInForce_.assignT(TwseTimeInForce_Default);

   //==============================================
   // 需要判斷的資料
   // 1.在 DR site 若是委託失敗的委託是不會存在 Tw.Stk.OrdCenter 裡面
   // 2.在 DR site 不會有 PartialNew 的狀態, 因為算不出來

   fixStkOrd.OrdStatus_ .assignT(FIX::OrdStatus_NEW);
   fixStkOrd.ExecType_  .assignT(FIX::ExecType_NEW);
   fixStkOrd.LeavesQty_ = fixStkOrd.OrderQty_;

   //==============================================
   // 其餘的部分
   // RefOrdID_
   fixStkOrd.RefOrdID_ = ordHnd.GetSysOrdID();

   // UserID_
   stkOrd.OrigSource_.UserID_.assign(TandSesID_);

   // 060.TransactTime
   fixOfRnoFd.TransactTime_.assign(TwseTransactTime(stkOrd.TseOrigTime_).getValue());

   bool         ret = FixOrdFlow_->DoReqRno(fixReqRno);
   TFixSysOrdID fsoid;
   
   if (ret) {
      //===========================================
      // 將 FixSysOrdID 填入
      fsoid = fixRefRno->GetSysOrdID();
      
      TTwseFsCnvt::FromFixSysOrdID(fsoid, stkOrd);

      //===========================================
      // update fixRefRno
      fixRefRno->Update(std::string("回報新委託OK!"), FIX::Stk::fof_Sent);
   }
   
   return fsoid;
}
//---------------------------------------------------------------------------

K_mf(void) TTwseDrProc::AddFixDeal (const TFixSysOrdID& fsoid, const TStkOrder* stkOrd, const TStkDeals* stkDeals)
{
   TStkQty qtyDeal;

   for (TStkDeals::const_iterator iterDeal=stkDeals->begin(); iterDeal!=stkDeals->end(); ++iterDeal) {
      TFixRfReqErDeal      reqErDeal    = TFixRptFlow::CreateReqErDeal(fsoid);
      TFixRfReqErDealBase* refErDeal    = reqErDeal.get();
      SFixRfBaseField&     fixBaseField = refErDeal->GetBaseField();
      SFixRfErField&       fixErField   = refErDeal->GetErField  ();
      const TStkDeal&      stkDeal      = iterDeal->second;

      // 060.TransactTime
      fixBaseField.TransactTime_.assign(TwseTransactTime(stkDeal.Time_).getValue());

      //===========================================
      // 成交部份
      qtyDeal += stkDeal.Qty_;

      fixErField.LastPx_     = TTwseFsCnvt::ToFixPri(stkDeal.Pri_);
      fixErField.LastShares_ = TTwseFsCnvt::ToFixQty(stkDeal.Qty_);
      // 這邊應該使用累加的 QtyDeal
      //fixErField.CumQty_     = TTwseFsCnvt::ToFixQty(stkOrd->DealQty_);
      fixErField.CumQty_     = TTwseFsCnvt::ToFixQty(qtyDeal);

      // 委託部分
      // ==> 這邊應該沒有辦法算出 fixErField.LeavesQty_
      //     因為沒有完整的歷程

      // 委託狀態部分
      fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_PARTIALLY_FILLED);
      fixErField  .ExecType_ .assignT(FIX::ExecType_PARTIAL_FILL);

      // 打上 ExecID
      TTwseFsCnvt::MakeDealExexID(iterDeal, fixErField.ExecID_);

      FixRptFlow_->DoReqErDeal(reqErDeal);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TTwseDrProc::AddFixFinal (const TFixSysOrdID& fsoid, const TStkOrder* stkOrd)
{
   TFixRfReqErOrd      reqErOrd = TFixRptFlow::CreateReqErOrd(fsoid, FIX::Stk::fcid_New);
   TFixRfReqErOrdBase* refErOrd = reqErOrd.get();
   SFixRfBaseField&    fixBaseField = refErOrd->GetBaseField();
   SFixRfErField&      fixErField   = refErOrd->GetErField  ();
   bool                hasDeal(stkOrd->DealQty_.is_0() ? false : true);

   if (hasDeal)
      fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_PARTIALLY_FILLED);
   else
      fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_NEW);
   
   // 060.TransactTime
   fixBaseField.TransactTime_.assign(TwseTransactTime(stkOrd->TseLastTime_).getValue());

   fixErField.LeavesQty_ = TTwseFsCnvt::ToFixQty(stkOrd->LeftQty());
   fixErField.CumQty_    = TTwseFsCnvt::ToFixQty(stkOrd->DealQty_);
   fixErField.ExecType_             .assignT(FIX::ExecType_RESTATED);
   fixErField.ExecRestatementReason_.assignT(FIX::ExecRestatementReason_MARKET_OPTION);

   // 因為之前有送過 New::New, 所以要強迫在送ㄧ次
   refErOrd->SetEnableResend(true);

   FixRptFlow_->DoReqErOrd(reqErOrd);
}
//---------------------------------------------------------------------------
