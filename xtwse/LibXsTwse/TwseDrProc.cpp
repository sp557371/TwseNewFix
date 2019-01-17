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
   // �}�l���X�Ҧ����e�U���
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
      // �ˬd Fix.Stk ���e�U�O�_�s�b
      const TStkOrder* cpStkOrd = ordHnd.GetConst();
      if (cpStkOrd == NULL) {
         logLen = sprintf(logMsg, "<TwseDrProc> [%d]SysOrd = Null!", begSoid.get_orig());
         FixTradeLog_->LogMem(elt_Error, logMsg, logLen);
         continue;
      }

      TFixSysOrdID fsoid = TTwseFsCnvt::ToFixSysOrdID(cpStkOrd);
      if (!fsoid.is_0()) {
         logLen = sprintf(logMsg, "<TwseDrProc> SysOrdID[%d] �w�B�z!", begSoid.get_orig());
         FixTradeLog_->LogMem(elt_Error, logMsg, logLen);
         continue;
      }
      
      // �p�G Qty_ = 0, �N��O[����^��]���ͪ��e�U, �o��N���B�z�F
      if (cpStkOrd->Qty_.is_0()) {
         logLen = sprintf(logMsg, "<TwseDrProc> [Qty_=0][%d]�N��O����^�����ͪ��e�U,���B�z!", begSoid.get_orig());
         FixTradeLog_->LogMem(elt_Info, logMsg, logLen);
         continue;
      }

      // �s��
      fsoid = AddFixNewOrd(ordHnd);
      if (fsoid.is_0()) {
         logLen = sprintf(logMsg, "<TwseDrProc> �s�W�e�U����[%d]!", begSoid.get_orig());
         FixTradeLog_->LogMem(elt_Error, logMsg, logLen);
         continue;
      }
      
      // ����^��
      if (cpStkOrd->DealQty_.is_0() == false) {
         const TStkDeals* stkDeals = cpStkOrd->Deals_.get();
         if (stkDeals) {
            AddFixDeal(fsoid, cpStkOrd, stkDeals);
         }
      }
      
      // �̫᪬�A��s
      AddFixFinal(fsoid, cpStkOrd);

      // �p��,���F�קK�{���d��
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
   // �T�w�����
   // 021.HandlInst
   fixStkOrd.HandlInst_.assignT(TwseHandlInst_Default);
   // 059.TimeInForce
   fixStkOrd.TimeInForce_.assignT(TwseTimeInForce_Default);

   //==============================================
   // �ݭn�P�_�����
   // 1.�b DR site �Y�O�e�U���Ѫ��e�U�O���|�s�b Tw.Stk.OrdCenter �̭�
   // 2.�b DR site ���|�� PartialNew �����A, �]���⤣�X��

   fixStkOrd.OrdStatus_ .assignT(FIX::OrdStatus_NEW);
   fixStkOrd.ExecType_  .assignT(FIX::ExecType_NEW);
   fixStkOrd.LeavesQty_ = fixStkOrd.OrderQty_;

   //==============================================
   // ��l������
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
      // �N FixSysOrdID ��J
      fsoid = fixRefRno->GetSysOrdID();
      
      TTwseFsCnvt::FromFixSysOrdID(fsoid, stkOrd);

      //===========================================
      // update fixRefRno
      fixRefRno->Update(std::string("�^���s�e�UOK!"), FIX::Stk::fof_Sent);
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
      // ���泡��
      qtyDeal += stkDeal.Qty_;

      fixErField.LastPx_     = TTwseFsCnvt::ToFixPri(stkDeal.Pri_);
      fixErField.LastShares_ = TTwseFsCnvt::ToFixQty(stkDeal.Qty_);
      // �o�����Өϥβ֥[�� QtyDeal
      //fixErField.CumQty_     = TTwseFsCnvt::ToFixQty(stkOrd->DealQty_);
      fixErField.CumQty_     = TTwseFsCnvt::ToFixQty(qtyDeal);

      // �e�U����
      // ==> �o�����ӨS����k��X fixErField.LeavesQty_
      //     �]���S�����㪺���{

      // �e�U���A����
      fixBaseField.OrdStatus_.assignT(FIX::OrdStatus_PARTIALLY_FILLED);
      fixErField  .ExecType_ .assignT(FIX::ExecType_PARTIAL_FILL);

      // ���W ExecID
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

   // �]�����e���e�L New::New, �ҥH�n�j���b�e����
   refErOrd->SetEnableResend(true);

   FixRptFlow_->DoReqErOrd(reqErOrd);
}
//---------------------------------------------------------------------------
