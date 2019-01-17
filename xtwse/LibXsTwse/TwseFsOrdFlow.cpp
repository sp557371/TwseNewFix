/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseFsOrdFlow.h"
// libExt
#include "ExtDef.h"
#include "MsgCenter.h"
// Aux
#include "Nsa_UtilComm.h"
#include "Nsa_DbgInfo.h"
// Fix.Stk
#include "FixStkIntf.h"
// libXsTwse
#include "TwseFsOrd.h"
#include "TwseFstFact.h"
#include "TwseRes.h"
//---------------------------------------------------------------------------
#include "TwseFsOrdFlowAux.cpp"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TBrkOrdID impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TBrkOrdID::assign (const char* str)
{
   BrokerID_ = TBrokerID();
   TseOrdID_ = TTseOrdID();

   if (!str || strlen(str) == 0)
      return;

   const char* ptr = strchr(str, (int)' ');
   if (ptr) {
      BrokerID_.assign(std::string(str, ptr-str));
      TseOrdID_.assign(++ptr);
   }
   else {
      BrokerID_.assign(str);
   }
}
//---------------------------------------------------------------------------

K_mf(std::string) TBrkOrdID::as_string () const
{
   char buf[128];
   sprintf(buf, "%s %s", BrokerID_.as_string().c_str(), TseOrdID_.as_string().c_str());
   return std::string(buf);
}
//---------------------------------------------------------------------------

K_mf(bool) TBrkOrdID::operator < (const TBrkOrdID& a) const
{
   if (BrokerID_ < a.BrokerID_) {
      return true;
   }
   else if (BrokerID_ == a.BrokerID_) {
      if (TseOrdID_ < a.TseOrdID_)
         return true;
   }
   return false;
}


/////////////////////////////////////////////////////////////////////////////
// class TTwseFsOrdFlow impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseFsOrdFlow::TTwseFsOrdFlow (FIX::Stk::TTradeInfo* fti)
   : inherited(fti)
   , RsOrdID_ (NULL)
{
   //==============================================
   // 參數初始化
   TTwseFstInfo* tiTwse = static_cast<TTwseFstInfo*>(fti);
   size_t        i;

   TwOrdCenter_ = tiTwse->GetOrdCenter();
   
   inherited::SpecRule_.set(FIX::Stk::osr_ChgQty_UseDec);
   inherited::SpecRule_.set(FIX::Stk::osr_ChgPri_Disable);

   //==============================================
   // New Order Single
   const EFixOfOrdField nosFields[] = {
      FIX::Stk::eof_SenderSubID,
      FIX::Stk::eof_TargetSubID,
      FIX::Stk::eof_Account,
      FIX::Stk::eof_ClOrdID,
      FIX::Stk::eof_OrderID,
      FIX::Stk::eof_OrderQty,
      FIX::Stk::eof_OrdType,
      FIX::Stk::eof_Price,
      FIX::Stk::eof_Side,
      FIX::Stk::eof_Symbol,
   };

   for (i=0; i<numofele(nosFields); ++i)
      inherited::OfSetNos_.set(nosFields[i]);

   //==============================================
   // Order Cancel Request
   const EFixOfOrdField ocrFields[] = {
      FIX::Stk::eof_SenderSubID,
      FIX::Stk::eof_TargetSubID,
      FIX::Stk::eof_Account,
      FIX::Stk::eof_ClOrdID,
      FIX::Stk::eof_OrderID,
      FIX::Stk::eof_OrigClOrdID,
      FIX::Stk::eof_Price,
      FIX::Stk::eof_Side,
      FIX::Stk::eof_Symbol,
   };

   for (i=0; i<numofele(ocrFields); ++i)
      inherited::OfSetOcr_.set(ocrFields[i]);

   //==============================================
   // Order Cancel Replace Request
   const EFixOfOrdField ocrrFields[] = {
      FIX::Stk::eof_SenderSubID,
      FIX::Stk::eof_TargetSubID,
      FIX::Stk::eof_Account,
      FIX::Stk::eof_ClOrdID,
      FIX::Stk::eof_OrderID,
      FIX::Stk::eof_OrderQty,
      FIX::Stk::eof_OrdType,
      FIX::Stk::eof_OrigClOrdID,
      FIX::Stk::eof_Price,
      FIX::Stk::eof_Side,
      FIX::Stk::eof_Symbol,
   };

   for (i=0; i<numofele(ocrrFields); ++i)
      inherited::OfSetOcrr_.set(ocrrFields[i]);

   //==============================================
   // Order Status Request
   const EFixOfOrdField osrFields[] = {
      FIX::Stk::eof_SenderSubID,
      FIX::Stk::eof_TargetSubID,
      FIX::Stk::eof_ClOrdID,
      FIX::Stk::eof_OrderID,
      FIX::Stk::eof_Price,
      FIX::Stk::eof_Side,
      FIX::Stk::eof_Symbol,
   };

   for (i=0; i<numofele(osrFields); ++i)
      inherited::OfSetOsr_.set(osrFields[i]);

   //==============================================
   // Auto New Order
   const EFixOfOrdField anoFields[] = {
      FIX::Stk::eof_SenderSubID,
      FIX::Stk::eof_TargetSubID,
      FIX::Stk::eof_Account,
      FIX::Stk::eof_OrderID,
      FIX::Stk::eof_OrderQty,
      FIX::Stk::eof_OrdType,
      FIX::Stk::eof_OrigClOrdID,
      FIX::Stk::eof_Price,
      FIX::Stk::eof_Side,
      FIX::Stk::eof_Symbol,
   };

   for (i=0; i<numofele(anoFields); ++i)
      inherited::OfSetAno_.set(anoFields[i]);

   //==============================================
   // [委託書號]對照表
   char          dirPath[256];
   TViewAreaMgr& vaMgr = fti->GetVaMgr();
   
   TUtilComm::ConsMsg("Load Fix 委託書號對照表");

   sprintf(dirPath, "%sBrkOrdID", fti->GetLogPath().c_str());
   RsOrdID_ = new TRsOrdID(dirPath);
   Load_RsOrdID();

   vaMgr.Add(std::string("9.委託書號"), RsOrdID_->GetVaMgr());
}
//---------------------------------------------------------------------------

K_dtor TTwseFsOrdFlow::~TTwseFsOrdFlow ()
{
   if (RsOrdID_) {
      TViewAreaMgr& vaMgr = inherited::TradeInfo_->GetVaMgr();

      vaMgr.Del(RsOrdID_->GetVaMgr());
      delete RsOrdID_;
   }
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFsOrdFlow::Load_RsOrdID ()
{
   TFsSysOrds* sysOrds = inherited::OrdCenter_->GetSysOrds();

   for (TFsSysOrds::iterator iter=sysOrds->begin(); iter!=sysOrds->end(); ++iter) {
      const TFsSysOrdRef* ord = iter->get();
      if (ord) {
         VirInsNewOrd(iter.get_idx(), *ord);
      }
   }
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFsOrdFlow::VirInsNewOrd (const TFixSysOrdID& fsid, const TFsStkOrder& fixStkOrd)
{
   if (RsOrdID_) {
      TFixTwseTargetSubID twseTargetSubID(fixStkOrd.TargetSubID_);
      TTrxSegment         trxSegment     (twseTargetSubID.as_TrxSegment());
      TBrkOrdID           brkOrdID       (fixStkOrd.SenderSubID_.as_string(), fixStkOrd.OrderID_.as_string());

      RsOrdID_->Insert(trxSegment, brkOrdID, fsid);
   }
}
//---------------------------------------------------------------------------

K_mf(TTwseFsOrdFlow::EFixFindOrdResult) TTwseFsOrdFlow::VirFindOldOrd (TFixOfReqOldBase&             refOld,
                                                                       TTwseFsOrdFlow::TFixSysOrdID& fsid)
{
   const SFixOfOrdField& ordField = refOld.GetOrdField();
   const FixMessage&     fixMsg   = refOld.GetFixMsg  ();
   const FixHeader&      fixHdr   = fixMsg.getHeader  ();
   EFixOrdCmdID          foid     = refOld.GetOrdCmdID().get_orig();
   TBrkOrdID             brkOrdID;
   TTrxSegment           trxSegment;

   fsid.assign0();

   //==============================================
   // TargetSubID ==> TTrxSegment
   FIX::TargetSubID targetSubID;

   if (!fixHdr.findField(targetSubID)) {
      return FIX::Stk::for_OrderNotFound;
   }
   std::string         strTargetSubID (targetSubID.getString());
   TFixTwseTargetSubID twseTargetSubID(strTargetSubID);
   trxSegment = twseTargetSubID.as_TrxSegment();
   //==============================================
   // SenderSubID ==> TBrokerID
   FIX::SenderSubID senderSubID;

   if (!fixHdr.findField(senderSubID)) {
      return FIX::Stk::for_OrderNotFound;
   }
   std::string strSenderSubID(senderSubID.getString());
   brkOrdID.assign_BrokerID(strSenderSubID);
   //==============================================
   // OrderID ==> TTseOrdID
   FIX::OrderID orderID;

   if (!fixMsg.findField(orderID)) {
      return FIX::Stk::for_OrderNotFound;
   }
   std::string strOrderID(orderID.getString());
   brkOrdID.assign_TseOrdID(strOrderID);


   //==============================================
   // 使用 OrigClOrdID 尋找委託
   TFixClOrdID  fixCoid;
   // 查詢是使用 ClOrdID
   if (foid != FIX::Stk::fcid_Qry)  fixCoid = ordField.OrigClOrdID_;
   else                             fixCoid = ordField.ClOrdID_;

   while (fixCoid.empty() == false) {
      const SFixCoid* cpCoid = inherited::Coids_->FindDat(fixCoid);
      if (!cpCoid)
         break;
      //---------------------------------
      // 如果 OrigClOrdID = ClOrdID, 因為在 TFsOrdFlow::DoReqOld 已經將 ClOrdID 存進去了
      // 但是並還沒指定 SysOrdID, 所以需要判斷一下
      if (cpCoid->SysOrdID_.is_0())
         break;
      
      //---------------------------------
      // 取出此委託
      fsid = cpCoid->SysOrdID_;

      TFixOrdHnd fixOrdHnd = inherited::OrdCenter_->GetOrdHnd(fsid, FIX::Stk::fuf_OrdFlow);
      if (fixOrdHnd.is_invalid())
         return FIX::Stk::for_OrdContNotMatch;

      const TFsStkOrder* fixStkOrd = fixOrdHnd.GetConst();
      if (!fixStkOrd)
         return FIX::Stk::for_OrdContNotMatch;
      //---------------------------------
      // 比對此委託內容
      //
      // 比對 - 盤別
      if (fixStkOrd->TargetSubID_.as_string() != strTargetSubID)
         return FIX::Stk::for_OrdContNotMatch;
      // 比對 - 券商代號
      if (fixStkOrd->SenderSubID_.as_string() != strSenderSubID)
         return FIX::Stk::for_OrdContNotMatch;
      // 比對 - 委託書號
      if (fixStkOrd->OrderID_.as_string() != strOrderID)
         return FIX::Stk::for_OrdContNotMatch;
      // 比對 - 其它欄位
      TOrdFdChk ordFdChk(trxSegment, foid, *fixStkOrd, ordField);
         
      if (ordFdChk.DoChk() == false) {
         char logMsg[512];
         int  logLen;

         logLen = sprintf(logMsg, "單筆欄位檢核錯誤, FixSeqNum[%d], %s",
               ordField.MsgSeqNum_.get_orig(),
               ordFdChk.GetErrDesc(fsid, brkOrdID));

         inherited::TradeLog_->LogMem(elt_Info, logMsg, logLen);
         TUtilComm::ConsMsg(logMsg);

         return FIX::Stk::for_OrdContNotMatch;
      }
      
      return FIX::Stk::for_Success;
   }

   //==============================================
   // 使用 OrigClOrdID 找不到委託,在使用[委託書號尋找看看]
   TRsOrdID::TRsFixSoid* rsFixSoid = RsOrdID_->FindRsFixSoid(trxSegment, brkOrdID);
   if (rsFixSoid) {
      //---------------------------------
      // 全部繞過一遍看看是否有符合的
      for (TRsOrdID::TRsFixSoidI iter=rsFixSoid->begin(); iter!=rsFixSoid->end(); ++iter) {
         TFixOrdHnd fixOrdHnd = inherited::OrdCenter_->GetOrdHnd(iter->first, FIX::Stk::fuf_OrdFlow);
         if (fixOrdHnd.is_invalid())
            continue;

         const TFsStkOrder* fixStkOrd = fixOrdHnd.GetConst();
         if (!fixStkOrd)
            continue;
         //------------------------------
         // 比對 - 委託內容
         TOrdFdChk ordFdChk(trxSegment, foid, *fixStkOrd, ordField);

         if (ordFdChk.DoChk()) {
            //---------------------------
            // 目前只針對<DR切換重建資料>才可放行
            // ==> ClOrdID=NULL, 在一般情況下應該不會發生
            if (fixStkOrd->ClOrdID_.is_null()) {
               fsid = iter->first;
               return FIX::Stk::for_Success;
            }
            // 留下訊息
            char logMsg[512];
            int  logLen;
      
            logLen = sprintf(logMsg, "委託書號欄位檢核正確,但是 ClOrdID[%s] Not Null, FixSeqNum[%d]",
                  fixStkOrd->ClOrdID_.as_string().c_str(),
                  ordField.MsgSeqNum_.get_orig());

            inherited::TradeLog_->LogMem(elt_Info, logMsg, logLen);
            TUtilComm::ConsMsg(logMsg);
         }
         else {
            char logMsg[512];
            int  logLen;
      
            logLen = sprintf(logMsg, "重覆委託書號欄位檢核錯誤, FixSeqNum[%d], %s",
                  ordField.MsgSeqNum_.get_orig(),
                  ordFdChk.GetErrDesc(iter->first, brkOrdID));

            inherited::TradeLog_->LogMem(elt_Info, logMsg, logLen);
            TUtilComm::ConsMsg(logMsg);
         }
      }

      //---------------------------------
      // 全部都檢查過了,還是找不到符合的
      // ==> 為了可以正確回覆錯誤訊息(欄位內容)
      //     必須要新增一筆委託,然後回覆錯誤
      refOld.SetWorkRejCode(FIX::Stk::wrc_OrdContNotMatch);

      return FIX::Stk::for_OrderNotFound;
   }

   //==============================================
   // 使用<OrigClOrdID,委託書號>都找不到委託,應該是跨連線的委託
   // 處理邏輯: 使用新增新委託的方式
   return FIX::Stk::for_OrderNotFound;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsOrdFlow::CheckClOrdID (const TFixSysOrdID& fsid, EFixOrdCmdID foid, const TFixClOrdID& clOrdID,
                                         TFixRfReqReject&    reqReject)
{
   std::string str = clOrdID.as_string();
   size_t      len = str.length();
   //==============================================
   // 檢查長度
   if (len < efl_ClOrdID_Min || len > efl_ClOrdID_Max) {
      reqReject = TFixRptFlow::CreateReqReject(fsid, foid, tfrc_ClOrdIDErr);
      return false;
   }

   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsOrdFlow::CheckOrigClOrdID (const TFixSysOrdID&    fsid,        EFixOrdCmdID     foid,
                                             const TFixOrigClOrdID& origClOrdID, TFixRfReqReject& reqReject)
{
   std::string str = origClOrdID.as_string();
   size_t      len = str.length();
   //==============================================
   // 檢查長度
   if (len < efl_OrigClOrdID_Min || len > efl_OrigClOrdID_Max) {
      reqReject = TFixRptFlow::CreateReqReject(fsid, foid, tfrc_OrigClOrdIDErr);
      return false;
   }

   return true;
}
//---------------------------------------------------------------------------
// 檢查 - 40.OrdType
K_mf(bool) TTwseFsOrdFlow::CheckOrdType (const TFixSysOrdID& fsid,   EFixOrdCmdID     foid, const TFixOrdType& ordType,
                                         const TTrxSegment&  trxSeg, TFixRfReqReject& reqReject)
{
   char ch = ordType.get_orig();
   
   if (trxSeg == ts_Full || trxSeg == ts_Odd) {
      // 普通交易,零股
      // ==> 2: Limit Price
      if (ch == FIX::OrdType_LIMIT)
         return true;
   }
   else if (trxSeg == ts_Fix) {
      // 盤後定價
      // ==> Z: Limit after close
      if (ch == OrdType_LimitAfterClose)
         return true;
   }

   // 檢核失敗
   reqReject = TFixRptFlow::CreateReqReject(fsid, foid, tfrc_OrdTypeErr);
   return false;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsOrdFlow::CheckAccount (const TFixSysOrdID& fsid, EFixOrdCmdID foid, const TFixAccount& account, TFixRfReqReject& reqReject)
{
   std::string str = account.as_string();
   size_t      len = str.length();
   //==============================================
   // 檢查長度
   if (len != efl_Account) {
      reqReject = TFixRptFlow::CreateReqReject(fsid, foid, tfrc_IvacNoLenErr);
      return false;
   }

   //==============================================
   // 檢查內容
   // ==> 只允許數字
   const char* beg = str.c_str();
   const char* end = (beg + len);
   
   for (; beg<end; ++beg) {
      if (isdigit(*beg))
         continue;

      // 檢核失敗
      reqReject = TFixRptFlow::CreateReqReject(fsid, foid, tfrc_IvacNoLenErr);
      return false;
   }

   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsOrdFlow::CheckOrderID (const TFixSysOrdID& fsid, EFixOrdCmdID foid, const TFixOrderID& orderID, TFixRfReqReject& reqReject)
{
   //==============================================
   // 檢查長度
   std::string str = orderID.as_string();
   size_t      len = str.length();
   if (len != efl_OrderID) {
      reqReject = TFixRptFlow::CreateReqReject(fsid, foid, tfrc_SeqNoLenErr);
      return false;
   }

   const char* beg = str.c_str();
   const char* end = (beg + len);
   char  ch;
   
   //==============================================
   // 檢查內容: 1~5:文數字
   for (; beg<end; ++beg) {
      ch = *beg;
      if (isalpha(ch) || isdigit(ch))
         continue;
      
      // 檢核錯誤
      reqReject = TFixRptFlow::CreateReqReject(fsid, foid, tfrc_SeqNoLenErr);
      return false;
   }

   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsOrdFlow::CheckSymbol (const TFixSysOrdID& fsid, EFixOrdCmdID foid, const TFixSymbol& symbol, TFixRfReqReject& reqReject)
{
   std::string str = symbol.as_string();
   size_t      len = str.length();
   if (len < efl_Symbol_Min || len > efl_Symbol_Max) {
      reqReject = TFixRptFlow::CreateReqReject(fsid, foid, tfrc_StkNoLenErr);
      return false;
   }
   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsOrdFlow::CheckPrice (const TFixSysOrdID&  fsid, EFixOrdCmdID foid, TFixOrdType::orig_type oOrdType,
                                       const TFixStdString& str,  TFixRfReqReject& reqReject)
{
   //if (oOrdType != FIX::OrdType_NULL && oOrdType != OrdType_LimitAfterClose) {
   if (oOrdType != FIX::OrdType_NULL) {
      if (TTwseFsCnvt::CheckFixPri(str) == false) {
         reqReject = TFixRptFlow::CreateReqReject(fsid, foid, tfrc_PriLenErr);
         return false;
      }
   }
   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsOrdFlow::CheckQty (const TFixSysOrdID& fsid, EFixOrdCmdID foid, const TFixStdString& str, TFixRfReqReject& reqReject)
{
   if (TTwseFsCnvt::CheckFixQty(str) == false) {
      reqReject = TFixRptFlow::CreateReqReject(fsid, foid, tfrc_QtyLenErr);
      return false;
   }
   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsOrdFlow::CheckApCode (const TFixTargetSubID& str)
{
   char ch = ' ';
   
   if (!str.empty())
      ch = str[0];

   if (ch != TwseTargetSubID_Regular && ch != TwseTargetSubID_OddLots && ch != TwseTargetSubID_FixedPrice)
      return false;

   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsOrdFlow::CheckApCode (const TFixSysOrdID& fsid, EFixOrdCmdID foid, const TFixTargetSubID& str, TFixRfReqReject& reqReject)
{
   if (CheckApCode(str))
      return true;
   
   reqReject = TFixRptFlow::CreateReqReject(fsid, foid, tfrc_ApCodeErr);
   return false;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsOrdFlow::CheckBrokerID (const TFixSenderSubID& senderSubID)
{
   std::string brokerID = senderSubID.as_string();
   
   if (brokerID.length() != efl_BrokerID)
      return false;

   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsOrdFlow::CheckBrokerID (const TFixSysOrdID& fsid, EFixOrdCmdID foid, const TFixSenderSubID& senderSubID,
                                          TFixRfReqReject& reqReject)
{
   if (CheckBrokerID(senderSubID))
      return true;

   reqReject = TFixRptFlow::CreateReqReject(fsid, foid, tfrc_BrokerIDErr);
   return false;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsOrdFlow::CheckWarrantLP (TFixOfReqBase*   fixRefBase, const TFixTargetSubID& targetSubID,
                                           TFixRfReqReject& reqReject)
{
   // [定盤][零股]不允許 tag 10003
   // ==> TwseWarrantLP 資料是儲存在每筆的 OrdTrack 裡面
   char ch = targetSubID[0];

   if (ch == TwseTargetSubID_OddLots || ch == TwseTargetSubID_FixedPrice) {
      SFixOrdTrack*          fixOrdTrack = fixRefBase->GetOrdTrack();
      STwseOrdTrackReseChar* twseOtRChar = reinterpret_cast<STwseOrdTrackReseChar*>(fixOrdTrack->GetRChar());

      if (twseOtRChar->TwseWarrantLP_.get_orig() == TwseWarrantLP_Use) {
         reqReject = TFixRptFlow::CreateReqReject(fixRefBase->GetSysOrdID(),
                                                  fixRefBase->GetOrdCmdID().get_orig(),
                                                  tfrc_WarrantLPErr);
         return false;
      }
   }

   return true;
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFsOrdFlow::VirInitNewOrdField (const FixMessage& fixMsg, const SFixOfOrdField& fixOrdFd, TFsStkOrder& ord)
{
   STwseFsOrdReseChar* twseReseChar = reinterpret_cast<STwseFsOrdReseChar*>(ord.GetReseChar());

   //==============================================
   // 填入額外資訊
   /*
   FixStk_GetFixMsg(fixMsg, TwseIvacnoFlag, twseReseChar->TwseIvacnoFlag_)
   FixStk_GetFixMsg(fixMsg, TwseOrdType,    twseReseChar->TwseOrdType_)
   FixStk_GetFixMsg(fixMsg, TwseExCode,     twseReseChar->TwseExCode_)
   */

   twseReseChar->TwseIvacnoFlag_.assign_FixChar(fixOrdFd.TwseIvacnoFlag_);
   twseReseChar->TwseOrdType_   .assign_FixChar(fixOrdFd.TwseOrdType_);
   twseReseChar->TwseExCode_    .assign_FixChar(fixOrdFd.TwseExCode_);
}
//---------------------------------------------------------------------------
//
// New Order Single
//
K_mf(bool) TTwseFsOrdFlow::OnProcReqNos (TFixOfReqNosBase* refNos)
{
   TFixSysOrdID  fsid      = refNos->GetSysOrdID();
   EFixOrdCmdID  foid      = FIX::Stk::fcid_New;
   TFixOrdHnd&   fixOrdHnd = refNos->GetOrdHnd();   
   TFsStkOrder&  fixStkOrd = fixOrdHnd.GetModify();
   FIX::Message& fixMsg    = refNos->GetFixMsg();

   //==============================================
   // 檢查欄位長度
   int                    len;
   TFixRfReqReject        reqReject;
   const SFixOfOrdField&  ordField = refNos->GetOrdField();
   TFixOrdType::orig_type oOrdType = fixStkOrd.OrdType_.get_orig();
   
   do {
      // 檢查並更正AP-CODE
      if (CheckApCode(fsid, foid, ordField.TargetSubID_, reqReject) == false)
         break;
      // 轉換 - TTrxSegment
      TFixTwseTargetSubID twseTargetSubID(ordField.TargetSubID_);
      TTrxSegment         trxSegment     (twseTargetSubID.as_TrxSegment());
      // 券商代號長度不正確
      if (CheckBrokerID(fsid, foid, ordField.SenderSubID_, reqReject) == false)
         break;
      // 檢查 - 37.ClOrdID
      if (CheckClOrdID(fsid, foid, ordField.ClOrdID_, reqReject) == false)
         break;
      // 檢查 - 40.OrdType
      if (CheckOrdType(fsid, foid, ordField.OrdType_, trxSegment, reqReject) == false)
         break;
      // 投資人帳號長度不正確
      if (CheckAccount(fsid, foid, ordField.Account_, reqReject) == false)
         break;
      // 委託書編號長度不正確
      if (CheckOrderID(fsid, foid, ordField.OrderID_, reqReject) == false)
         break;
      // 股票代號長度不正確
      if (CheckSymbol(fsid, foid, ordField.Symbol_, reqReject) == false)
         break;
      // 單價長度不正確
      if (CheckPrice(fsid, foid, oOrdType, ordField.Price_, reqReject) == false)
         break;
      // 委託數量長度不正確
      if (CheckQty(fsid, foid, ordField.OrderQty_, reqReject) == false)
         break;
      // 檢查零股部份
      if (ordField.TargetSubID_[0] == TwseTargetSubID_OddLots) {
         const STwseFsOrdReseChar* twseRChar = reinterpret_cast<const STwseFsOrdReseChar*>(fixStkOrd.GetReseChar());
         
         // TwseIvacnoFlag != Normal(FIX)
         if (twseRChar->TwseIvacnoFlag_ != TwseIvacnoFlag_Normal_FIX) {
            reqReject = TFixRptFlow::CreateReqReject(fsid, foid, tfrc_IvacNoFlagErr);
            break;
         }
         // TwseOrdType != Normal
         if (twseRChar->TwseOrdType_ != TwseOrdType_Normal) {
            reqReject = TFixRptFlow::CreateReqReject(fsid, foid, tfrc_TwseOrdTypeErr);
            break;
         }
      }
      //---------------------------------
      // 檢查[卷商代號+委託書號]是否重覆
      TBrkOrdID brkOrdID(ordField.SenderSubID_.as_string(), ordField.OrderID_.as_string());

      if (RsOrdID_->CheckInsert(trxSegment, brkOrdID, fsid) == false) {
         // 代表[卷商代號+委託書號]重覆,但是不要這裡回覆,在 TFsOrdFlow::DoReqNos 最後才回覆錯誤
         refNos->SetInitRejCode(tfrc_TwseOrdIdDup);
         break;
      }

   } while (false);

   //==============================================
   // 看看有沒有錯誤發生
   if (reqReject.get()) {
      // 因為 OrdHnd 是新建立的, 所以先要 Update 一下, 這樣才會把資料 insert 到 SysOrds 裡面
      fixOrdHnd.FlushUpdate(FIX::Stk::fuf_OrdFlow);

      inherited::RptFlow_->DoReqReject(reqReject);
      return false;
   }

   return true;
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFsOrdFlow::InitOldOrdField (TFixOfReqBase* refBase)
{
   const FixMessage&         fixMsg    = refBase->GetFixMsg();
         TFsStkOrder&        fixOrd    = refBase->GetOrdHnd().GetModify();
         STwseFsOrdReseChar* twseRChar = reinterpret_cast<STwseFsOrdReseChar*>(fixOrd.GetReseChar());

   //==============================================
   // 填入額外資訊
   if (twseRChar->TwseIvacnoFlag_.is_null())
      FixStk_GetFixMsg(fixMsg, TwseIvacnoFlag, twseRChar->TwseIvacnoFlag_)

   if (twseRChar->TwseOrdType_.is_null())
      FixStk_GetFixMsg(fixMsg, TwseOrdType, twseRChar->TwseOrdType_)

   if (twseRChar->TwseExCode_.is_null())
      FixStk_GetFixMsg(fixMsg, TwseExCode, twseRChar->TwseExCode_)

   //==============================================
   // [刪/改]單時要檢查是否有 10003
   const TFixMsgType2& msgType = refBase->GetFixMsgType();

   if (msgType == FIX::MsgType_OrderCancelRequest || msgType == FIX::MsgType_OrderCancelReplaceRequest) {
      const FixMessage& fixMsg = refBase->GetFixMsg();
      TwseWarrantLP     twseWarrantLP;
   
      if (fixMsg.findField(twseWarrantLP)) {
         SFixOrdTrack* fixOrdTrack = refBase->GetOrdTrack();
         
         if (fixOrdTrack) {
            STwseOrdTrackReseChar* otRChar = reinterpret_cast<STwseOrdTrackReseChar*>(fixOrdTrack->GetRChar());
            
            otRChar->TwseWarrantLP_.assign(twseWarrantLP);
         }
         
         twseRChar->TwseWarrantLP_.assign(twseWarrantLP);
      }
   }
}
//---------------------------------------------------------------------------
//
// Order Cancel Request
//
K_mf(bool) TTwseFsOrdFlow::VirProcOcr (TFixOfReqOcrBase* refOcr)
{
   TFixSysOrdID       fsid      = refOcr->GetSysOrdID();
   EFixOrdCmdID       foid      = refOcr->GetOrdCmdID().get_orig();
   TFixOrdHnd&        fixOrdHnd = refOcr->GetOrdHnd();   
   const TFsStkOrder* fixStkOrd = fixOrdHnd.GetConst();

   //==============================================
   // 檢查欄位長度
   TFixRfReqReject        reqReject;
   const SFixOfOrdField&  ordField = refOcr->GetOrdField();
   TFixOrdType::orig_type oOrdType = fixStkOrd->OrdType_.get_orig();
   
   do {
      // 檢查並更正AP-CODE
      if (CheckApCode(fsid, foid, ordField.TargetSubID_, reqReject) == false)
         break;
      // 券商代號長度不正確
      if (CheckBrokerID(fsid, foid, ordField.SenderSubID_, reqReject) == false)
         break;
      // 檢查 - 37.ClOrdID
      if (CheckClOrdID(fsid, foid, ordField.ClOrdID_, reqReject) == false)
         break;
      // 檢查 - 41.OrigClOrdID
      if (CheckOrigClOrdID(fsid, foid, ordField.OrigClOrdID_, reqReject) == false)
         break;
      // 委託書編號長度不正確
      if (CheckOrderID(fsid, foid, ordField.OrderID_, reqReject) == false)
         break;
      // 投資人帳號長度不正確
      if (CheckAccount(fsid, foid, ordField.Account_, reqReject) == false)
         break;
      // 股票代號長度不正確
      if (CheckSymbol(fsid, foid, ordField.Symbol_, reqReject) == false)
         break;
      // 單價長度不正確
      if (CheckPrice(fsid, foid, oOrdType, ordField.Price_, reqReject) == false)
         break;
      // [定盤][零股]不允許 tag 10003
      if (CheckWarrantLP(refOcr, ordField.TargetSubID_, reqReject) == false)
         break;

   } while (false);

   if (reqReject.get()) {
      inherited::RptFlow_->DoReqReject(reqReject);
      return false;
   }

   return true;
}
//---------------------------------------------------------------------------
//
// Order Cancel Replace Request
//
K_mf(bool) TTwseFsOrdFlow::VirProcOcrr (TFixOfReqOcrrBase* refOcrr)
{
   TFixSysOrdID       fsid      = refOcrr->GetSysOrdID();
   EFixOrdCmdID       foid      = refOcrr->GetOrdCmdID().get_orig();
   TFixOrdHnd&        fixOrdHnd = refOcrr->GetOrdHnd();   
   const TFsStkOrder* fixStkOrd = fixOrdHnd.GetConst();

   //==============================================
   // 檢查欄位長度
   TFixRfReqReject        reqReject;
   const SFixOfOrdField&  ordField = refOcrr->GetOrdField();
   TFixOrdType::orig_type oOrdType = fixStkOrd->OrdType_.get_orig();
   
   do {
      // 檢查並更正AP-CODE
      if (CheckApCode(fsid, foid, ordField.TargetSubID_, reqReject) == false)
         break;
      // 轉換 - TTrxSegment
      TFixTwseTargetSubID twseTargetSubID(ordField.TargetSubID_);
      TTrxSegment         trxSegment     (twseTargetSubID.as_TrxSegment());
      // 券商代號長度不正確
      if (CheckBrokerID(fsid, foid, ordField.SenderSubID_, reqReject) == false)
         break;
      // 檢查 - 37.ClOrdID
      if (CheckClOrdID(fsid, foid, ordField.ClOrdID_, reqReject) == false)
         break;
      // 檢查 - 41.OrigClOrdID
      if (CheckOrigClOrdID(fsid, foid, ordField.OrigClOrdID_, reqReject) == false)
         break;
      // 檢查 - 40.OrdType
      if (CheckOrdType(fsid, foid, ordField.OrdType_, trxSegment, reqReject) == false)
         break;
      // 投資人帳號長度不正確
      if (CheckAccount(fsid, foid, ordField.Account_, reqReject) == false)
         break;
      // 委託書編號長度不正確
      if (CheckOrderID(fsid, foid, ordField.OrderID_, reqReject) == false)
         break;
      // 股票代號長度不正確
      if (CheckSymbol(fsid, foid, ordField.Symbol_, reqReject) == false)
         break;
      // 單價長度不正確
      if (CheckPrice(fsid, foid, oOrdType, ordField.Price_, reqReject) == false)
         break;
      // 委託數量長度不正確
      if (CheckQty(fsid, foid, ordField.OrderQty_, reqReject) == false)
         break;
      // [普通][定盤]不允需 OrderQty 為 0
      if (ordField.TargetSubID_[0] != TwseTargetSubID_OddLots) {
         int qty = atoi(ordField.OrderQty_.c_str());
         if (qty == 0) {
            reqReject = TFixRptFlow::CreateReqReject(fsid, foid, tfrc_QuantityError);
            break;
         }
      }
      // [定盤][零股]不允許 tag 10003
      if (CheckWarrantLP(refOcrr, ordField.TargetSubID_, reqReject) == false)
         break;

   } while (false);

   if (reqReject.get()) {
      inherited::RptFlow_->DoReqReject(reqReject);
      return false;
   }

   return true;
}
//---------------------------------------------------------------------------
//
// Order Status Request
//
K_mf(bool) TTwseFsOrdFlow::VirProcOsr (TFixOfReqOsrBase* refOsr)
{
   TFixSysOrdID       fsid      = refOsr->GetSysOrdID();
   EFixOrdCmdID       foid      = refOsr->GetOrdCmdID().get_orig();
   TFixOrdHnd&        fixOrdHnd = refOsr->GetOrdHnd();   
   const TFsStkOrder* fixStkOrd = fixOrdHnd.GetConst();

   //==============================================
   // 檢查欄位長度
   TFixRfReqReject        reqReject;
   const SFixOfOrdField&  ordField = refOsr->GetOrdField();
   TFixOrdType::orig_type oOrdType = fixStkOrd->OrdType_.get_orig();
   
   do {
      // 檢查並更正AP-CODE
      if (CheckApCode(fsid, foid, ordField.TargetSubID_, reqReject) == false)
         break;
      // 券商代號長度不正確
      if (CheckBrokerID(fsid, foid, ordField.SenderSubID_, reqReject) == false)
         break;
      // 檢查 - 37.ClOrdID
      if (CheckClOrdID(fsid, foid, ordField.ClOrdID_, reqReject) == false)
         break;
      // 委託書編號長度不正確
      if (CheckOrderID(fsid, foid, ordField.OrderID_, reqReject) == false)
         break;
      // 股票代號長度不正確
      if (CheckSymbol(fsid, foid, ordField.Symbol_, reqReject) == false)
         break;
      // 單價長度不正確
      if (CheckPrice(fsid, foid, oOrdType, ordField.Price_, reqReject) == false)
         break;
   } while (false);
   if (reqReject.get()) {
      inherited::RptFlow_->DoReqReject(reqReject);
      return false;
   }

   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsOrdFlow::VirProcOrderNotFound (TFixOfReqOldBase* fixRefOld)
{
   //==============================================
   // 原始委託找不到的時候,就直接新增一筆新委託囉
   // ==> 成交回報改成必須委託存在才會進入到 TwOrdCenter
   //     所以可以不用在去 TwOrdCenter 搜尋一次委託了

   //==============================================
   // 初步檢查欄位資料
   //
   // 找不到委託有可能是下列資料有問題
   // SenderSubID(50): BrokerID
   // TargetSubID(57): TrxSegment
   const SFixOfOrdField& fixOrdFd = fixRefOld->GetOrdField();
   int  workRejCode = FIX::Stk::wrc_None;

   // 一定要先檢查 TargetSubID, 這樣才有辦法組出錯誤訊息
   // 沒有 TargetSubID, 無法得知 [一般交易子系統] [盤後交易子系統] ...
   if (CheckApCode(fixOrdFd.TargetSubID_) == false) {
      workRejCode = tfrc_ApCodeErr;
   }
   else if (CheckBrokerID(fixOrdFd.SenderSubID_) == false) {
      workRejCode = tfrc_BrokerIDErr;
   }

   if (workRejCode != FIX::Stk::wrc_None)
      fixRefOld->SetWorkRejCode(workRejCode);
   else {
      // 順便檢查 <fixRefOld> 是否有錯誤
      workRejCode = fixRefOld->GetWorkRejCode();
   }

   //==============================================
   // 先在 FixStkOrd 中產生一筆新的委託
   FIX::TMessageRef  msgRef = fixRefOld->GetFixMsgRef();
   const FixMessage& fixMsg = *msgRef;
   bool       isValidOrd = (workRejCode == FIX::Stk::wrc_None);
   TSysOrdID  sid;
  
   TFixOfReqAno fixReqAno = TFsOrdFlow::CreateReqAno(msgRef, TFixRefOrdID(sid.get_orig()), isValidOrd);

   if (inherited::DoReqAno(fixReqAno) == false) {
      // 新增失敗, 暫時不知要如何處理
      return false;
   }

   //==============================================
   // 儲存新的委託的資訊
   TFixOfReqAnoBase*    fixRefAno = fixReqAno.get();
   TFsOrdFlow::TOrdHnd& fixOrdHnd = fixRefAno->GetOrdHnd();

   fixRefOld->SetOrdHnd(fixOrdHnd);

   return true;
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFsOrdFlow::VirOnGetOrdFiled (const FixMessage& fixMsg, SFixOfOrdField& ordFd)
{
   std::string tagVal;

   // 10000.TwseIvacnoFlag
   FixStk_FindTagVal (fixMsg, FIX::FIELD::TwseIvacnoFlag, ordFd.TwseIvacnoFlag_)
   // 10001.TwseOrdType
   FixStk_FindTagVal (fixMsg, FIX::FIELD::TwseOrdType,    ordFd.TwseOrdType_)
   // 10002.TwseExCode
   FixStk_FindTagVal (fixMsg, FIX::FIELD::TwseExCode,     ordFd.TwseExCode_)
}
//---------------------------------------------------------------------------
