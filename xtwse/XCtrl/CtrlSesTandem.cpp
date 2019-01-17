/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "CtrlSesTandem.h"
#include "ExtDef.h"
#include "Nsa_UtilComm.h"
#include "TwseExcCode.h"
#include "CtrlSesMgr.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// class TCtrlSesTandem impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TCtrlSesTandem::TCtrlSesTandem (TCtrlSesMgr* sesMgr)
   : inherited    ()
   , SesMgr_      (sesMgr)
   , RsReqResuGoh_(true)
   , Timer_       (this)
   , Interval_    (10000) // 10 ��
{
}
//---------------------------------------------------------------------------

K_dtor TCtrlSesTandem::~TCtrlSesTandem ()
{
   Timer_.Stop();
}
//---------------------------------------------------------------------------

K_mf(void) TCtrlSesTandem::OnMesLinkReady (TMesBase* mes)
{
   inherited::OnMesLinkReady(mes);
      
   ReqID_.clear();
   
   LinkIP_ = mes->GetUID();

   //==============================================
   // �}�Ҭ�����
   char filePath[256];
   
   sprintf(filePath, "%sSesCtrlTandem.log", GetProcLogPath().c_str());

   LogFile_.Open  (filePath);
   LogFile_.LogStr(elt_Info, "Link Ready! [%s]", LinkIP_.c_str());

   //==============================================
   // �o�e�ƥ�
   TExcMgrBase* excMgr = GetExcMgr();
   if (excMgr) {
      std::string::size_type pos = LinkIP_.find(':');
      if (pos != std::string::npos)
         LinkIP_.erase(pos);

      excMgr->Alert(TTwseExcCode::GetXCtrlCode(tec_TandemLinkReady), LinkIP_);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TCtrlSesTandem::OnMesLinkBroken (TMesBase* mes, const std::string& reason)
{
   inherited::OnMesLinkBroken(mes, reason);

   Timer_.Stop();

   LogFile_.LogStr(elt_Info, "Link Broken! [%s]", reason.c_str());

   //==============================================
   // �ˬd�u�@�O�_�w�g����,�Y�|���������ܻݭn�o�ƥ�
   if (MyReqIDs_.empty() == false) {
      //-----------------
      // �_�u�F, �n�h���ʸ�ƪ��A
      for (TMyReqIDsI iter=MyReqIDs_.begin(); iter!=MyReqIDs_.end(); ++iter) {
         TRsReqResuHnd      hnd(RsReqResuGoh_.GetPtr(), *iter);
         const SRsReqResu1* cpDat1 = hnd.FindDat1();
   
         if (cpDat1 && cpDat1->SentTime_.IsInvalid()) {
            SRsReqResu1& uDat1 = hnd.BeginUpdate1();
      
            uDat1.ProcResu_.assignT(epr_TandemLinkBroken);
         }
      }

      //-----------------
      // �o�e�ƥ� - �s�u���_
      TExcMgrBase* excMgr = GetExcMgr();
      if (excMgr) {
         char randNum[10];

         sprintf(randNum, "%04d", MyReqIDs_.begin()->RandNum_.get_orig());
         // �ƥ�ӷ�: LinkIP
         // �Ƶ�:     Tandem �R�O�y����
         excMgr->Alert(TTwseExcCode::GetXCtrlCode(tec_TandemLinkBroken), LinkIP_, std::string(randNum));
      }
   
      MyReqIDs_.clear();
   }
}
//---------------------------------------------------------------------------

K_mf(void) TCtrlSesTandem::OnMesLinkFail (TMesBase* mes, const std::string& reason)
{
   inherited::OnMesLinkFail(mes, reason);
}
//---------------------------------------------------------------------------

K_mf(int) TCtrlSesTandem::GetPacketSize (STandPkt* pkt, bool isSend)
{
   int funcCode = pkt->FuncCode_.as_int();
   
   if (funcCode == tfc_Heartbeat)
      return Pl_STandPktHdr;
      
   if (isSend)
      return Pl_STandPktResp_All;
   
   return Pl_STandPktReq_All;
}
//---------------------------------------------------------------------------

K_mf(void) TCtrlSesTandem::OnPktRecv (STandPkt& pkt, int pktSize)
{
   LogFile_.LogSize(elt_Recv, &pkt, pktSize);

   ETandFuncCode funcCode = pkt.FuncCode_.get_orig();

   if (funcCode == tfc_Heartbeat) {
      STandPkt newPkt(tfc_Heartbeat, emt_Request, pkt.Random_.as_int());
      inherited::SendPkt(newPkt);
   }
   else {
      AddReq(pkt);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TCtrlSesTandem::OnSendPkt (STandPkt& pkt, int pktSize)
{
   LogFile_.LogSize(elt_Send, &pkt, pktSize);
}
//---------------------------------------------------------------------------

K_mf(void) TCtrlSesTandem::AddReq (STandPkt& pkt)
{
   int randNum = pkt.Random_.as_int();

   //==============================================
   // �o�e�ƥ� - �������O
   TExcMgrBase* excMgr = GetExcMgr();
   if (excMgr) {
      char caRandNum[10];

      sprintf(caRandNum, "%04d", randNum);
      // �ƥ�ӷ�: LinkIP
      // �Ƶ�:     Tandem �R�O�y����
      excMgr->Alert(TTwseExcCode::GetXCtrlCode(tec_TandemRecvCmd), LinkIP_, std::string(caRandNum));
   }

   //==============================================
   // �x�s�ʥ]��T
   STandPktReq* pktFmt = pkt.GetReq();
   TReqID       reqID;
   SRsReqResu1  reqResu1;
   TRsFixSesID  rsFixSesID;
   int          tolRec   = pktFmt->TotalRecord_.as_int();

   reqID.Time_   .assign_str_hhmmssms_9(pkt.MsgTime_.as_orig());
   reqID.RandNum_.assign               (randNum);
   
   reqResu1.RecvTime_      = TTimeStamp::Now();
   reqResu1.FuncCode_      = pkt.FuncCode_;
   reqResu1.MsgType_       = pkt.MsgType_;
   reqResu1.MsgTime_       = pkt.MsgTime_;
   reqResu1.AllMarketFlag_ = pktFmt->AllMarketFlag_;
   reqResu1.NewSeqNum_     .assign (pktFmt->NewSeqNum_.as_int());
   reqResu1.ProcResu_      .assignT(epr_LinUp);

   for (int i=0; i<tolRec; ++i) {
      TFixSesID   fixSesID(pktFmt->Body_[i].BrokerID_, pktFmt->Body_[i].SocketID_);
      SRsReqResu2 reqResu2;

      reqResu2.TermNo_ = pktFmt->Body_[i].TermNo_;
      reqResu2.ReqIdx_ .assign(i);

      rsFixSesID.insert(TRsFixSesID::value_type(fixSesID, reqResu2));
   }

   //==============================================
   // �ʥ]���ˮ�
   EStatusCode statusCode;

   do {
      // �ˬd FuncCode 
      if (reqResu1.FuncCode_.is_invalid()) {
         reqResu1.ProcResu_.assignT(epr_Reject);
         statusCode = esc_FuncCodeError;
         break;
      }
   
      // �ˬd MsgType
      EMsgType msgType = reqResu1.MsgType_.get_orig();
      if (msgType != emt_Request) {
         reqResu1.ProcResu_.assignT(epr_Reject);
         statusCode = esc_MsgTypeError;
         break;
      }

      // �ˬd NewSeqNum �O�_����
      ETandFuncCode funcCode = reqResu1.FuncCode_.get_orig();

      if (funcCode == tfc_SequenceReset) {
         if (reqResu1.NewSeqNum_.get_orig() <= 0) {
            reqResu1.ProcResu_.assignT(epr_Reject);
            statusCode = esc_NewSeqNumError;
            break;
         }
      }
      
      // �ˬd Appia �s�u�O�_�s�b
      /*
      if (SesMgr_->GetAppiaSes() == NULL) {
         reqResu1.ProcResu_.set_error(epr_Finish);
         statusCode = esc_AppiaNotConn;
         break;
      }
      */

   } while (false);

   //==============================================
   // �B�z�����~���a��
   EProcResu orgResu = reqResu1.ProcResu_.get_orig();
   if (orgResu >= epr_Finish) {
      for (TRsFixSesID::iterator iter=rsFixSesID.begin(); iter!=rsFixSesID.end(); ++iter) {
         iter->second.StatusCode1_.assignT(statusCode);
      }
   }

   //==============================================
   // �s�W���ƪ�
   RsReqResuGoh_->AddReq(reqID, reqResu1, rsFixSesID);

   MyReqIDs_.insert(reqID);
   if (ReqID_.is_invalid()) {
      ReqID_ = reqID;
      Timer_.Start(10, &this_type::OnTimer);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TCtrlSesTandem::OnTimer (TTimerBase*)
{
   //==============================================
   // ���T�{ ReqID �O�_����
   if (ReqID_.is_invalid()) {
      if (MyReqIDs_.empty()) {
         // �ݰ_�Ӥu�@���w�����F
         Timer_.Stop();
         return;
      }
      ReqID_ = *(MyReqIDs_.begin());
   }

   TRsReqResuHnd      hnd(RsReqResuGoh_.GetPtr(), ReqID_);
   const SRsReqResu1* cpDat1 = hnd.FindDat1();
   
   EProcResu orgResu = cpDat1->ProcResu_.get_orig();
   if (orgResu < epr_Finish) {
      // �ˬd�O�_�� Timeout
      TTimeStamp ts   = TTimeStamp::Now();
      long       diff = (ts - cpDat1->RecvTime_);
      
      if (diff < Interval_) {
         // �٨S timeout, �~�򵥫��o
         return;
      }
      
      // timeout �����εo�ͤF, ���ʩ|�������� StatusCode
      SRsReqResu1&  udDat1     = hnd.BeginUpdate1();
      ETandFuncCode funcCode   = udDat1.FuncCode_.get_orig();
      TRsFixSesID*  rsFixSesID = hnd.GetMap2();

      udDat1.ProcResu_.assignT(epr_AppiaLinkTimeout);

      for (TRsFixSesID::iterator iter=rsFixSesID->begin(); iter!=rsFixSesID->end(); ++iter) {
         if (iter->second.IsFinish(funcCode))
            continue;

         if (iter->second.StatusCode1_.empty()) {
            TRsFixSesID::updater upd = rsFixSesID->begin_update(iter);
            upd->StatusCode1_.assignT(esc_AppiaTimeout);
         }
         else if (iter->second.StatusCode2_.empty()) {
            TRsFixSesID::updater upd = rsFixSesID->begin_update(iter);
            upd->StatusCode2_.assignT(esc_AppiaTimeout);
         }
      }
   }

   //==============================================
   // �ˬd�O�_�w�ǰe
   if (cpDat1->SentTime_.IsInvalid()) {
      TRsFixSesID* rsFixSesID = hnd.GetMap2();
      SRsReqResu1& udDat1     = hnd.BeginUpdate1();

      if (SendResp(ReqID_, udDat1, *rsFixSesID)) {
         udDat1.SentTime_ = TTimeStamp::Now();
      }
      else {
         udDat1.ProcResu_.assignT(epr_TandemLinkBroken);
      }
         
      // �o���u�@����,�����o
      MyReqIDs_.erase(ReqID_);
//      SesMgr_->ResetReqID();   
      TUtilComm::ConsMsg("Tandem �u�@���� [%s]", ReqID_.as_string().c_str());

      ReqID_.clear();
   }
}
//---------------------------------------------------------------------------

K_mf(bool) TCtrlSesTandem::SendResp (const TReqID& reqID, const SRsReqResu1& dat1, TRsFixSesID& rsFixSesID)
{
   STandPkt      pkt(dat1.FuncCode_.get_orig(), emt_Acknowledge, reqID.RandNum_.get_orig());
   STandPktResp* pktFmt   = pkt.GetResp();
   int           tolRec   = rsFixSesID.size();
   int           idx      = 0;
   EProcResu     origRequ = dat1.ProcResu_.get_orig();

   pktFmt->AllMarketFlag_ = dat1.AllMarketFlag_;
   pktFmt->TotalRecord_.assign((size_t)tolRec);
   pktFmt->clear();

   if (origRequ > epr_Finish) {
      // ���`�T���^��
      pkt.MsgType_.assignT(emt_Reject);
      pktFmt->TotalRecord_.assign((size_t)0);
   }
   else {
      for (TRsFixSesID::iterator iter=rsFixSesID.begin(); iter!=rsFixSesID.end(); ++iter) {
         const SRsReqResu2& dat2 = iter->second;
         // ���o�o��������
         idx = dat2.ReqIdx_.get_orig();
         if (idx < 0 || idx >= tolRec)
            idx = 0;
         // ���o�ʥ]��m
         STandPktResp::SBody* body = &(pktFmt->Body_[idx]);

         body->BrokerID_        = iter->first.GetBrokerID();
         body->SocketID_        = iter->first.GetSocketID();
         body->TermNo_          = dat2.TermNo_;
         body->ConnState_       = dat2.ConnState_;
         body->ServiceDownFlag_ = dat2.ServiceDownFlag_;
         body->SuspendFlag_     = dat2.SuspendFlag_;
         body->HoldFlag_        = dat2.HoldFlag_;
         body->InMsgSeqNum_     .assign((size_t)dat2.InMsgSeqNum_.get_orig());
         body->LastInMsgTime_   = dat2.LastInMsgTime_;
         body->OutMsgSeqNum_    .assign((size_t)dat2.OutMsgSeqNum_.get_orig());
         body->LastOutMsgTime_  = dat2.LastOutMsgTime_;
         body->StatusCode1_     = dat2.StatusCode1_;
         body->StatusCode2_     = dat2.StatusCode2_;
         body->Text_            = dat2.Text_;
      }
   }

   return inherited::SendPkt(pkt);
}
//---------------------------------------------------------------------------
