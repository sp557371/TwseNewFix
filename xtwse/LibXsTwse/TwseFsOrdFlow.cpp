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
   // �Ѽƪ�l��
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
   // [�e�U�Ѹ�]��Ӫ�
   char          dirPath[256];
   TViewAreaMgr& vaMgr = fti->GetVaMgr();
   
   TUtilComm::ConsMsg("Load Fix �e�U�Ѹ���Ӫ�");

   sprintf(dirPath, "%sBrkOrdID", fti->GetLogPath().c_str());
   RsOrdID_ = new TRsOrdID(dirPath);
   Load_RsOrdID();

   vaMgr.Add(std::string("9.�e�U�Ѹ�"), RsOrdID_->GetVaMgr());
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
   // �ϥ� OrigClOrdID �M��e�U
   TFixClOrdID  fixCoid;
   // �d�߬O�ϥ� ClOrdID
   if (foid != FIX::Stk::fcid_Qry)  fixCoid = ordField.OrigClOrdID_;
   else                             fixCoid = ordField.ClOrdID_;

   while (fixCoid.empty() == false) {
      const SFixCoid* cpCoid = inherited::Coids_->FindDat(fixCoid);
      if (!cpCoid)
         break;
      //---------------------------------
      // �p�G OrigClOrdID = ClOrdID, �]���b TFsOrdFlow::DoReqOld �w�g�N ClOrdID �s�i�h�F
      // ���O���٨S���w SysOrdID, �ҥH�ݭn�P�_�@�U
      if (cpCoid->SysOrdID_.is_0())
         break;
      
      //---------------------------------
      // ���X���e�U
      fsid = cpCoid->SysOrdID_;

      TFixOrdHnd fixOrdHnd = inherited::OrdCenter_->GetOrdHnd(fsid, FIX::Stk::fuf_OrdFlow);
      if (fixOrdHnd.is_invalid())
         return FIX::Stk::for_OrdContNotMatch;

      const TFsStkOrder* fixStkOrd = fixOrdHnd.GetConst();
      if (!fixStkOrd)
         return FIX::Stk::for_OrdContNotMatch;
      //---------------------------------
      // ��惡�e�U���e
      //
      // ��� - �L�O
      if (fixStkOrd->TargetSubID_.as_string() != strTargetSubID)
         return FIX::Stk::for_OrdContNotMatch;
      // ��� - ��ӥN��
      if (fixStkOrd->SenderSubID_.as_string() != strSenderSubID)
         return FIX::Stk::for_OrdContNotMatch;
      // ��� - �e�U�Ѹ�
      if (fixStkOrd->OrderID_.as_string() != strOrderID)
         return FIX::Stk::for_OrdContNotMatch;
      // ��� - �䥦���
      TOrdFdChk ordFdChk(trxSegment, foid, *fixStkOrd, ordField);
         
      if (ordFdChk.DoChk() == false) {
         char logMsg[512];
         int  logLen;

         logLen = sprintf(logMsg, "�浧����ˮֿ��~, FixSeqNum[%d], %s",
               ordField.MsgSeqNum_.get_orig(),
               ordFdChk.GetErrDesc(fsid, brkOrdID));

         inherited::TradeLog_->LogMem(elt_Info, logMsg, logLen);
         TUtilComm::ConsMsg(logMsg);

         return FIX::Stk::for_OrdContNotMatch;
      }
      
      return FIX::Stk::for_Success;
   }

   //==============================================
   // �ϥ� OrigClOrdID �䤣��e�U,�b�ϥ�[�e�U�Ѹ��M��ݬ�]
   TRsOrdID::TRsFixSoid* rsFixSoid = RsOrdID_->FindRsFixSoid(trxSegment, brkOrdID);
   if (rsFixSoid) {
      //---------------------------------
      // ����¶�L�@�M�ݬݬO�_���ŦX��
      for (TRsOrdID::TRsFixSoidI iter=rsFixSoid->begin(); iter!=rsFixSoid->end(); ++iter) {
         TFixOrdHnd fixOrdHnd = inherited::OrdCenter_->GetOrdHnd(iter->first, FIX::Stk::fuf_OrdFlow);
         if (fixOrdHnd.is_invalid())
            continue;

         const TFsStkOrder* fixStkOrd = fixOrdHnd.GetConst();
         if (!fixStkOrd)
            continue;
         //------------------------------
         // ��� - �e�U���e
         TOrdFdChk ordFdChk(trxSegment, foid, *fixStkOrd, ordField);

         if (ordFdChk.DoChk()) {
            //---------------------------
            // �ثe�u�w��<DR�������ظ��>�~�i���
            // ==> ClOrdID=NULL, �b�@�뱡�p�U���Ӥ��|�o��
            if (fixStkOrd->ClOrdID_.is_null()) {
               fsid = iter->first;
               return FIX::Stk::for_Success;
            }
            // �d�U�T��
            char logMsg[512];
            int  logLen;
      
            logLen = sprintf(logMsg, "�e�U�Ѹ�����ˮ֥��T,���O ClOrdID[%s] Not Null, FixSeqNum[%d]",
                  fixStkOrd->ClOrdID_.as_string().c_str(),
                  ordField.MsgSeqNum_.get_orig());

            inherited::TradeLog_->LogMem(elt_Info, logMsg, logLen);
            TUtilComm::ConsMsg(logMsg);
         }
         else {
            char logMsg[512];
            int  logLen;
      
            logLen = sprintf(logMsg, "���Щe�U�Ѹ�����ˮֿ��~, FixSeqNum[%d], %s",
                  ordField.MsgSeqNum_.get_orig(),
                  ordFdChk.GetErrDesc(iter->first, brkOrdID));

            inherited::TradeLog_->LogMem(elt_Info, logMsg, logLen);
            TUtilComm::ConsMsg(logMsg);
         }
      }

      //---------------------------------
      // �������ˬd�L�F,�٬O�䤣��ŦX��
      // ==> ���F�i�H���T�^�п��~�T��(��줺�e)
      //     �����n�s�W�@���e�U,�M��^�п��~
      refOld.SetWorkRejCode(FIX::Stk::wrc_OrdContNotMatch);

      return FIX::Stk::for_OrderNotFound;
   }

   //==============================================
   // �ϥ�<OrigClOrdID,�e�U�Ѹ�>���䤣��e�U,���ӬO��s�u���e�U
   // �B�z�޿�: �ϥηs�W�s�e�U���覡
   return FIX::Stk::for_OrderNotFound;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsOrdFlow::CheckClOrdID (const TFixSysOrdID& fsid, EFixOrdCmdID foid, const TFixClOrdID& clOrdID,
                                         TFixRfReqReject&    reqReject)
{
   std::string str = clOrdID.as_string();
   size_t      len = str.length();
   //==============================================
   // �ˬd����
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
   // �ˬd����
   if (len < efl_OrigClOrdID_Min || len > efl_OrigClOrdID_Max) {
      reqReject = TFixRptFlow::CreateReqReject(fsid, foid, tfrc_OrigClOrdIDErr);
      return false;
   }

   return true;
}
//---------------------------------------------------------------------------
// �ˬd - 40.OrdType
K_mf(bool) TTwseFsOrdFlow::CheckOrdType (const TFixSysOrdID& fsid,   EFixOrdCmdID     foid, const TFixOrdType& ordType,
                                         const TTrxSegment&  trxSeg, TFixRfReqReject& reqReject)
{
   char ch = ordType.get_orig();
   
   if (trxSeg == ts_Full || trxSeg == ts_Odd) {
      // ���q���,�s��
      // ==> 2: Limit Price
      if (ch == FIX::OrdType_LIMIT)
         return true;
   }
   else if (trxSeg == ts_Fix) {
      // �L��w��
      // ==> Z: Limit after close
      if (ch == OrdType_LimitAfterClose)
         return true;
   }

   // �ˮ֥���
   reqReject = TFixRptFlow::CreateReqReject(fsid, foid, tfrc_OrdTypeErr);
   return false;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsOrdFlow::CheckAccount (const TFixSysOrdID& fsid, EFixOrdCmdID foid, const TFixAccount& account, TFixRfReqReject& reqReject)
{
   std::string str = account.as_string();
   size_t      len = str.length();
   //==============================================
   // �ˬd����
   if (len != efl_Account) {
      reqReject = TFixRptFlow::CreateReqReject(fsid, foid, tfrc_IvacNoLenErr);
      return false;
   }

   //==============================================
   // �ˬd���e
   // ==> �u���\�Ʀr
   const char* beg = str.c_str();
   const char* end = (beg + len);
   
   for (; beg<end; ++beg) {
      if (isdigit(*beg))
         continue;

      // �ˮ֥���
      reqReject = TFixRptFlow::CreateReqReject(fsid, foid, tfrc_IvacNoLenErr);
      return false;
   }

   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsOrdFlow::CheckOrderID (const TFixSysOrdID& fsid, EFixOrdCmdID foid, const TFixOrderID& orderID, TFixRfReqReject& reqReject)
{
   //==============================================
   // �ˬd����
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
   // �ˬd���e: 1~5:��Ʀr
   for (; beg<end; ++beg) {
      ch = *beg;
      if (isalpha(ch) || isdigit(ch))
         continue;
      
      // �ˮֿ��~
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
   // [�w�L][�s��]�����\ tag 10003
   // ==> TwseWarrantLP ��ƬO�x�s�b�C���� OrdTrack �̭�
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
   // ��J�B�~��T
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
   // �ˬd������
   int                    len;
   TFixRfReqReject        reqReject;
   const SFixOfOrdField&  ordField = refNos->GetOrdField();
   TFixOrdType::orig_type oOrdType = fixStkOrd.OrdType_.get_orig();
   
   do {
      // �ˬd�ç�AP-CODE
      if (CheckApCode(fsid, foid, ordField.TargetSubID_, reqReject) == false)
         break;
      // �ഫ - TTrxSegment
      TFixTwseTargetSubID twseTargetSubID(ordField.TargetSubID_);
      TTrxSegment         trxSegment     (twseTargetSubID.as_TrxSegment());
      // ��ӥN�����פ����T
      if (CheckBrokerID(fsid, foid, ordField.SenderSubID_, reqReject) == false)
         break;
      // �ˬd - 37.ClOrdID
      if (CheckClOrdID(fsid, foid, ordField.ClOrdID_, reqReject) == false)
         break;
      // �ˬd - 40.OrdType
      if (CheckOrdType(fsid, foid, ordField.OrdType_, trxSegment, reqReject) == false)
         break;
      // ���H�b�����פ����T
      if (CheckAccount(fsid, foid, ordField.Account_, reqReject) == false)
         break;
      // �e�U�ѽs�����פ����T
      if (CheckOrderID(fsid, foid, ordField.OrderID_, reqReject) == false)
         break;
      // �Ѳ��N�����פ����T
      if (CheckSymbol(fsid, foid, ordField.Symbol_, reqReject) == false)
         break;
      // ������פ����T
      if (CheckPrice(fsid, foid, oOrdType, ordField.Price_, reqReject) == false)
         break;
      // �e�U�ƶq���פ����T
      if (CheckQty(fsid, foid, ordField.OrderQty_, reqReject) == false)
         break;
      // �ˬd�s�ѳ���
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
      // �ˬd[���ӥN��+�e�U�Ѹ�]�O�_����
      TBrkOrdID brkOrdID(ordField.SenderSubID_.as_string(), ordField.OrderID_.as_string());

      if (RsOrdID_->CheckInsert(trxSegment, brkOrdID, fsid) == false) {
         // �N��[���ӥN��+�e�U�Ѹ�]����,���O���n�o�̦^��,�b TFsOrdFlow::DoReqNos �̫�~�^�п��~
         refNos->SetInitRejCode(tfrc_TwseOrdIdDup);
         break;
      }

   } while (false);

   //==============================================
   // �ݬݦ��S�����~�o��
   if (reqReject.get()) {
      // �]�� OrdHnd �O�s�إߪ�, �ҥH���n Update �@�U, �o�ˤ~�|���� insert �� SysOrds �̭�
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
   // ��J�B�~��T
   if (twseRChar->TwseIvacnoFlag_.is_null())
      FixStk_GetFixMsg(fixMsg, TwseIvacnoFlag, twseRChar->TwseIvacnoFlag_)

   if (twseRChar->TwseOrdType_.is_null())
      FixStk_GetFixMsg(fixMsg, TwseOrdType, twseRChar->TwseOrdType_)

   if (twseRChar->TwseExCode_.is_null())
      FixStk_GetFixMsg(fixMsg, TwseExCode, twseRChar->TwseExCode_)

   //==============================================
   // [�R/��]��ɭn�ˬd�O�_�� 10003
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
   // �ˬd������
   TFixRfReqReject        reqReject;
   const SFixOfOrdField&  ordField = refOcr->GetOrdField();
   TFixOrdType::orig_type oOrdType = fixStkOrd->OrdType_.get_orig();
   
   do {
      // �ˬd�ç�AP-CODE
      if (CheckApCode(fsid, foid, ordField.TargetSubID_, reqReject) == false)
         break;
      // ��ӥN�����פ����T
      if (CheckBrokerID(fsid, foid, ordField.SenderSubID_, reqReject) == false)
         break;
      // �ˬd - 37.ClOrdID
      if (CheckClOrdID(fsid, foid, ordField.ClOrdID_, reqReject) == false)
         break;
      // �ˬd - 41.OrigClOrdID
      if (CheckOrigClOrdID(fsid, foid, ordField.OrigClOrdID_, reqReject) == false)
         break;
      // �e�U�ѽs�����פ����T
      if (CheckOrderID(fsid, foid, ordField.OrderID_, reqReject) == false)
         break;
      // ���H�b�����פ����T
      if (CheckAccount(fsid, foid, ordField.Account_, reqReject) == false)
         break;
      // �Ѳ��N�����פ����T
      if (CheckSymbol(fsid, foid, ordField.Symbol_, reqReject) == false)
         break;
      // ������פ����T
      if (CheckPrice(fsid, foid, oOrdType, ordField.Price_, reqReject) == false)
         break;
      // [�w�L][�s��]�����\ tag 10003
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
   // �ˬd������
   TFixRfReqReject        reqReject;
   const SFixOfOrdField&  ordField = refOcrr->GetOrdField();
   TFixOrdType::orig_type oOrdType = fixStkOrd->OrdType_.get_orig();
   
   do {
      // �ˬd�ç�AP-CODE
      if (CheckApCode(fsid, foid, ordField.TargetSubID_, reqReject) == false)
         break;
      // �ഫ - TTrxSegment
      TFixTwseTargetSubID twseTargetSubID(ordField.TargetSubID_);
      TTrxSegment         trxSegment     (twseTargetSubID.as_TrxSegment());
      // ��ӥN�����פ����T
      if (CheckBrokerID(fsid, foid, ordField.SenderSubID_, reqReject) == false)
         break;
      // �ˬd - 37.ClOrdID
      if (CheckClOrdID(fsid, foid, ordField.ClOrdID_, reqReject) == false)
         break;
      // �ˬd - 41.OrigClOrdID
      if (CheckOrigClOrdID(fsid, foid, ordField.OrigClOrdID_, reqReject) == false)
         break;
      // �ˬd - 40.OrdType
      if (CheckOrdType(fsid, foid, ordField.OrdType_, trxSegment, reqReject) == false)
         break;
      // ���H�b�����פ����T
      if (CheckAccount(fsid, foid, ordField.Account_, reqReject) == false)
         break;
      // �e�U�ѽs�����פ����T
      if (CheckOrderID(fsid, foid, ordField.OrderID_, reqReject) == false)
         break;
      // �Ѳ��N�����פ����T
      if (CheckSymbol(fsid, foid, ordField.Symbol_, reqReject) == false)
         break;
      // ������פ����T
      if (CheckPrice(fsid, foid, oOrdType, ordField.Price_, reqReject) == false)
         break;
      // �e�U�ƶq���פ����T
      if (CheckQty(fsid, foid, ordField.OrderQty_, reqReject) == false)
         break;
      // [���q][�w�L]������ OrderQty �� 0
      if (ordField.TargetSubID_[0] != TwseTargetSubID_OddLots) {
         int qty = atoi(ordField.OrderQty_.c_str());
         if (qty == 0) {
            reqReject = TFixRptFlow::CreateReqReject(fsid, foid, tfrc_QuantityError);
            break;
         }
      }
      // [�w�L][�s��]�����\ tag 10003
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
   // �ˬd������
   TFixRfReqReject        reqReject;
   const SFixOfOrdField&  ordField = refOsr->GetOrdField();
   TFixOrdType::orig_type oOrdType = fixStkOrd->OrdType_.get_orig();
   
   do {
      // �ˬd�ç�AP-CODE
      if (CheckApCode(fsid, foid, ordField.TargetSubID_, reqReject) == false)
         break;
      // ��ӥN�����פ����T
      if (CheckBrokerID(fsid, foid, ordField.SenderSubID_, reqReject) == false)
         break;
      // �ˬd - 37.ClOrdID
      if (CheckClOrdID(fsid, foid, ordField.ClOrdID_, reqReject) == false)
         break;
      // �e�U�ѽs�����פ����T
      if (CheckOrderID(fsid, foid, ordField.OrderID_, reqReject) == false)
         break;
      // �Ѳ��N�����פ����T
      if (CheckSymbol(fsid, foid, ordField.Symbol_, reqReject) == false)
         break;
      // ������פ����T
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
   // ��l�e�U�䤣�쪺�ɭ�,�N�����s�W�@���s�e�U�o
   // ==> ����^���令�����e�U�s�b�~�|�i�J�� TwOrdCenter
   //     �ҥH�i�H���Φb�h TwOrdCenter �j�M�@���e�U�F

   //==============================================
   // ��B�ˬd�����
   //
   // �䤣��e�U���i��O�U�C��Ʀ����D
   // SenderSubID(50): BrokerID
   // TargetSubID(57): TrxSegment
   const SFixOfOrdField& fixOrdFd = fixRefOld->GetOrdField();
   int  workRejCode = FIX::Stk::wrc_None;

   // �@�w�n���ˬd TargetSubID, �o�ˤ~����k�եX���~�T��
   // �S�� TargetSubID, �L�k�o�� [�@�����l�t��] [�L�����l�t��] ...
   if (CheckApCode(fixOrdFd.TargetSubID_) == false) {
      workRejCode = tfrc_ApCodeErr;
   }
   else if (CheckBrokerID(fixOrdFd.SenderSubID_) == false) {
      workRejCode = tfrc_BrokerIDErr;
   }

   if (workRejCode != FIX::Stk::wrc_None)
      fixRefOld->SetWorkRejCode(workRejCode);
   else {
      // ���K�ˬd <fixRefOld> �O�_�����~
      workRejCode = fixRefOld->GetWorkRejCode();
   }

   //==============================================
   // ���b FixStkOrd �����ͤ@���s���e�U
   FIX::TMessageRef  msgRef = fixRefOld->GetFixMsgRef();
   const FixMessage& fixMsg = *msgRef;
   bool       isValidOrd = (workRejCode == FIX::Stk::wrc_None);
   TSysOrdID  sid;
  
   TFixOfReqAno fixReqAno = TFsOrdFlow::CreateReqAno(msgRef, TFixRefOrdID(sid.get_orig()), isValidOrd);

   if (inherited::DoReqAno(fixReqAno) == false) {
      // �s�W����, �Ȯɤ����n�p��B�z
      return false;
   }

   //==============================================
   // �x�s�s���e�U����T
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
