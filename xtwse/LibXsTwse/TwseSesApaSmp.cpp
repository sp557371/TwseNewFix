/////////////////////////////////////////////////////////////////////////////
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseSesApaSmp.h"
#include "ExcMgrBase.h"
#include "ExtDef.h"
#include "MsgCenter.h"
#include "Nsa_UtilComm.h"
#include "src/Code/fix42/Logon.h"
#include "src/Code/fix42/Logout.h"
#include "src/Code/fix42/BusinessMessageReject.h"
#include "TwseRes.h"
#include "TwseExcCode.h"
#include "TwseFstUnit.h"
#include "TwseIniFile.h"
#include "TwseDrProc.h"
#include "TwseType.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// class TTwseSesApaSmp::TRptHndEv impl
/////////////////////////////////////////////////////////////////////////////
K_mf(EecshSendResu) TTwseSesApaSmp::TFixRptHndEv::OnSendData (const TFixSysRptID& key, const SFixStkRptWsPtr& datWsPtr)
{
   const SFixStkRptWsRef* datWsRef = datWsPtr.get();
   //if (!datWsRef || datWsRef->SentTime_.IsInvalid() == false)
   //   return esr_MoveNext;
   if (!datWsRef)
      return esr_MoveNext;

   //==============================================
   // ���b�o�䥴�Ӯɶ�, �N�� appia �w�g����F
   // ���H��b�ק�
   //TFixRptCenter::TRsRptU upd;
   //inherited::GetUpd(key, upd);
   
   //SFixStkRpt& updDat = *(*upd);
   //updDat.SentTime_ = TTimeStamp::Now();
   
   if (datWsRef->MsgType_ == MsgType_Logon || datWsRef->MsgType_ == MsgType_Logout)
      return esr_MoveNext;

   //==============================================
   // ��� func call
   //Ses_->SmpOutboundMessageDelivery(key.get_orig(), datWsRef->MsgType_.as_string(), datWsRef->Message_);
   TSmpMsgOutboundMessage  smpReq;
   SSmpMsgOutboundMessage& data = smpReq.GetData();
   
   data.ClientMsgID_ = key.as_string();
   data.SessionID_   = ApaSesID_;
   data.MessageType_ = datWsRef->MsgType_.as_string();
   data.MessageData_ = datWsRef->Message_;

   Ses_->SendMsg(smpReq);

   return esr_MoveNext;
}


/////////////////////////////////////////////////////////////////////////////
// class TTwseSesApaSmp impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseSesApaSmp::TTwseSesApaSmp (TOrdFlow* of)
   : inherited ()
   , OrdFlow_  (of)
   , FixStatus_(efs_NotLink)
{
   inherited::SoleMode_.set(esm_UseMySessionID);

   //==============================================
   // �ϥΰѼƪ� SessionID
   std::string apaSesID = GetProcName();

   TandSesID_ = apaSesID;
   // �ݭn�N Tandem ���N���ন Appia ��
   TTwseSesIDTand tandSesID(apaSesID);
   apaSesID = tandSesID.as_string_Apa();
   
   inherited::ApaSesID_        .assign(apaSesID);
   inherited::FixTargetCompID_ = TandSesID_;

   TUtilComm::ConsMsg("ApaConnSesID: %s", inherited::ApaSesID_.c_str());

   //==============================================
   // �]�w�Ѽ�
   inherited::ExcCode_LinkReady_  = TTwseExcCode::GetXTwseCode(tec_AppiaMwLinkReady);
   inherited::ExcCode_LinkBroken_ = TTwseExcCode::GetXTwseCode(tec_AppiaMwLinkBroken);
   inherited::ExcCode_LinkFail_   = TTwseExcCode::GetXTwseCode(tec_AppiaMwLinkFail);
   inherited::ExcCode_LinkFails_  = TTwseExcCode::GetXTwseCode(tec_AppiaMwLinkFails);
}
//---------------------------------------------------------------------------

K_dtor TTwseSesApaSmp::~TTwseSesApaSmp ()
{
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::VirAfSettingApply (const std::string& str)
{
   //==============================================
   // �]�w Debug Level
   byte dbgLevel = GetDebugLevel();

   inherited::PktFileHnd_.UseQueue(false);
   //inherited::LogFileHnd_.UseQueue(true);
   inherited::LogFileHnd_.UseQueue(false);

   inherited::SesProp_.reset();
   inherited::SesProp_.set(esp_Disable_PktLog);

   // �����ʥ] off
   // �ǰe�ʥ] off
   // �����ʥ] off
   if (dbgLevel == 0) {
      inherited::SesProp_.set(esp_Disable_RecvLog);
      inherited::SesProp_.set(esp_Disable_SendLog);
   }
   // �����ʥ] on, �ϥ� thread
   // �ǰe�ʥ] on, �ϥ� thread
   // �����ʥ] off
   else if (dbgLevel == 1) {
   }
   // �����ʥ] on, �ϥ� thread
   // �ǰe�ʥ] on, �ϥ� thread
   // �����ʥ] off
   else if (dbgLevel == 2) {
   }
   // �����ʥ] on, �����g��
   // �ǰe�ʥ] on, �����g��
   // �����ʥ] off
   else if (dbgLevel == 3) {
      inherited::LogFileHnd_.UseQueue(false);
   }
   // �����ʥ] on, �����g��
   // �ǰe�ʥ] on, �����g��
   // �����ʥ] on, �����g��
   else if (dbgLevel == 4) {
      inherited::SesProp_.reset(esp_Disable_PktLog);

      inherited::LogFileHnd_.UseQueue(false);
      inherited::PktFileHnd_.UseQueue(false);
   }
   
   // debug {
   //inherited::LogFileHnd_.UseQueue(true);
   //inherited::SesProp_.set(esp_Disable_RecvLog);
   //inherited::SesProp_.set(esp_Disable_SendLog);
   // } debug
   
   //==============================================
   //
   inherited::VirAfSettingApply(str);
      
   //==============================================
   // �]�w IP/Port list
   TTwseMainIni    iniMain;
   TStrings        servPorts;
   char            logMsg[512];
   int             logLen;
   std::string     servIP  = iniMain.GetAppiaSvrIP();
   TFileWriterHnd& logFile = inherited::LogFileHnd_;

   IpPortList_.clear();
   if (iniMain.GetAppiaSvrPortList(servPorts)) {
      for (TStrings::iterator iter=servPorts.begin(); iter!=servPorts.end(); ++iter) {
         char buf[128];
         sprintf(buf, "%s:%s,M0R%d", servIP.c_str(), iter->c_str(), iniMain.GetSocketRetryInterval());
         IpPortList_.push_back(std::string(buf));
            
         logFile.WriteStr(elt_Info, "IpPortList: %s", buf);
      }
   }

   //==============================================
   // IsAutoFlush
   inherited::IsAutoFlush_ = iniMain.IsAutoFlushFixSeqNo();

   logLen = sprintf(logMsg, "AutoFlushFixSeqNo: %c", (inherited::IsAutoFlush_ ? 'Y' : 'N'));
   logFile.WriteMem(elt_Info, logMsg, logLen);

   //==============================================
   // MaxOrderCount
   MaxOrderCount_ = (size_t)iniMain.GetMaxOrderCount();

   logLen = sprintf(logMsg, "MaxOrderCount: %d", MaxOrderCount_);
   logFile.WriteMem(elt_Info, logMsg, logLen);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnMesLinkReady (Kway::TMesBase* mes)
{
   inherited::OnMesLinkReady(mes);

   SmpGetMwInfoTimer_.reset();
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnMesLinkBroken (Kway::TMesBase* mes, const std::string& reason)
{
   inherited::OnMesLinkBroken(mes, reason);

   SmpGetMwInfoTimer_.reset();
   FixRptHndEv_      .reset();
   FixTradeUnit_     .reset();
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnMesLinkFail (TMesBase* mes, const std::string& reason)
{
   inherited::OnMesLinkFail(mes, reason);

   ChangeMesSetting(mes);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::ChangeMesSetting (TMesBase* mes)
{
   if (IpPortList_.empty())
      return;

   std::string  newSetting;
   std::string  oldSetting = mes->GetSetting();
   TIpPortListI iter       = IpPortList_.begin();

   for (; iter!=IpPortList_.end(); ++iter) {
      if (*iter == oldSetting) {
         ++iter;
         break;
      }
   }

   if (iter != IpPortList_.end())  newSetting = *iter;
   else                            newSetting = *(IpPortList_.begin());

   mes->SettingApply(newSetting);

   char logMsg[256];
   sprintf(logMsg, "ChangeMesSetting Old[%s] New[%s]", oldSetting.c_str(), newSetting.c_str());
   
   TUtilComm::ConsMsg(logMsg);
   LogFileHnd_.WriteStr(elt_Info, "<TTwseSesApaSmp::ChangeMesSetting> %s", logMsg);
}
//---------------------------------------------------------------------------

K_mf(std::string) TTwseSesApaSmp::GetLogPath ()
{
   return GetProcLogPath();
}
//---------------------------------------------------------------------------

K_mf(std::string) TTwseSesApaSmp::GetLogName ()
{
   char logName[256];
   
   sprintf(logName, "SesApaSmp_%s", TandSesID_.c_str());
   return std::string(logName);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnSmpLogon ()
{
   //==============================================
   // �إ� FixTradeUnit ����
   FIX::Stk::TTradeMgr& fixTradeMgr = FIX::Stk::TTradeMgr::GetInst();
   
   // ConnSesID    = T5U3U8U0UXU1U
   // TargetCompID = T5380X1
   // �ҥH�o��n�ϥ� TandSesID
   FixTradeUnit_ = fixTradeMgr.QueryUnit(TandSesID_);
   
   // ���o�ثe�w�e�U������
   TFixOrdFlow* fixOrdFlow = FixTradeUnit_->GetFixOrdFlow();

   CurOrderCount_ = fixOrdFlow->GetOrdCenter()->GetOrdCount();

   //==============================================
   // �M Appia �߰ݧڪ� MwConfig
   //inherited::OnSmpLogon();
   inherited::SmpGetMiddlewareConfig();
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnSmpReject (TSmpMsgReject* smpMsg)
{
   const SSmpMsgReject& data      = smpMsg->GetData();
   ESmpRejectReason     rejReason = data.RejReason_.get_orig();

   //==============================================
   // �ˬd�O�_�O [SessionInfo does not exist for xxxx]
   if (rejReason == srr_SesInfo_SesIDNotFound) {
      if (data.RejParam1_ == inherited::ApaSesID_) {
         std::string reason("AppiaMw: SessionInfo does not exist");
         
         inherited::Reopen(reason, inherited::ApaMwInfo_.ReopenItvl_);
      }
   }
}
//---------------------------------------------------------------------------
// �o�Ӥ�k�椣�q, �o��U RemoveMiddleware ���O, �u�|��ۤv���s�u�_��
/*
K_mf(void) TTwseSesApaSmp::OnSmpReject_AlreadyLogon ()
{
   //==============================================
   // �ǰe cmd:remove_middleware ���O
   TSmpOperCmd operCmd(esc_RemoveMiddleware, inherited::ApaSesID_);
   SendCmd(operCmd);

   //==============================================
   // ���s logon
   inherited::SmpLogon();
}
*/
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnSmpGetMiddlewareConfigResponse (TSmpMsgGetMiddlewareConfigResponse* smpMsg)
{
   inherited::OnSmpGetMiddlewareConfigResponse(smpMsg);

   //==============================================
   // �ˬd SessionID �O�_����
   if (inherited::ApaSesID_.empty())
      return;

   //==============================================
   // �ˬd MW ���]�w
   const SSmpMiddlewareConfig& mwConf    = smpMsg->GetData();
   const TSmpStringList&       fixSesIDs = mwConf.SessionIDs_;

   if (!fixSesIDs.empty()) {
      // �ոլݥi���i�H���ڪ� SessionID
      bool found = false;
      
      for (TSmpStringList::const_iterator iter=fixSesIDs.begin(); iter!=fixSesIDs.end(); ++iter) {
         if (inherited::ApaSesID_ == *iter) {
            found = true;
            break;
         }
      }
         
      if (!found) {
         // �䤣��ڪ� SessionID
         char logMsg[256];
         int  logLen = sprintf(logMsg, "<TTwseSesApaSmp::OnSmpGetMiddlewareInfoResponse> MW �]�w�䤣��ڪ� SessionID[%s]!",
                                             inherited::ApaSesID_.c_str());
      
         inherited::LogFileHnd_.WriteMem(elt_Warning, logMsg, logLen);
            
         TExcMgrBase* excMgr = GetExcMgr();
         if (excMgr)
            excMgr->Alert(TTwseExcCode::GetXTwseCode(tec_AppiaMwSessionIDNotMatch), TTwseExcCode::LinkState);
               
         Reopen("TMP SessionID �P AppiaMW ����", inherited::ApaMwInfo_.ReopenItvl_);
      }
   }
   else {
      // SessionIDs �O�ťժ�, ��ڪ� SessionID �]�w�i�h�a

      // log {
      char logMsg[256];
      int  logLen = sprintf(logMsg, "<TTwseSesApaSmp::OnSmpGetMiddlewareInfoResponse> MW �S���]�w SessionID, �۰ʳ]�w�ۤv�� SessionID[%s]",
                                          inherited::ApaSesID_.c_str());
      
      inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
      // } log

      SSmpMiddlewareConfig mwConfNew(mwConf);

      mwConfNew.SessionIDs_.push_back(inherited::ApaSesID_);
      inherited::SmpSetMiddlewareConfigRequest(mwConfNew);
   }

   //==============================================
   // �߰� appia �ثe������ SessionID
   //inherited::SmpGetMiddlewareInfoRequest();
   SmpGetMwInfoTimer_.reset(new TTwseTimerType(this));
   SmpGetMwInfoTimer_->Start(0, &this_type::OnTwseTimer);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnSmpGetMiddlewareInfoResponse (TSmpMsgGetMiddlewareInfoResponse* smpMsg)
{
   inherited::OnSmpGetMiddlewareInfoResponse(smpMsg);

   //==============================================
   // �ˬd MW ���]�w
   const SSmpMsgGetMiddlewareInfoResponse& data      = smpMsg->GetData();
   const SSmpMiddlewareConfig&             mwConf    = data.MiddlewareConfig_.GetData();
   const TSmpStringList&                   fixSesIDs = mwConf.SessionIDs_;

   //==============================================
   // ���o�����O
   Market_ = GetProcMkt();

   //==============================================
   // �إ� FixTradeUnit ����
   FIX::Stk::TTradeMgr& fixTradeMgr = FIX::Stk::TTradeMgr::GetInst();
   TFixRptCenter*       fixRptCenter;
   
   // �b OnSmpLogon �ɤw�g���L�F, �ҥH�o��i�H���ݭn
   //FixTradeUnit_ = fixTradeMgr.QueryUnit(TandSesID_);
   
   fixRptCenter = FixTradeUnit_->GetFixRptCenter();

   //==============================================
   // ���ݬݫʥ]�̭��� ClientMsgID �e��h�֤F
   const TSmpLastMsgClientIDA& lmcis = data.LastMsgClientIDs_.GetData();
         bool found     = true;
         int  obNextSno = 0;
   
   for (TSmpLastMsgClientIDACi iter=lmcis.begin(); iter!=lmcis.end(); ++iter) {
      if (iter->SessionID_ != inherited::ApaSesID_)
         continue;
      
      int tmpSeqNum = fixRptCenter->GetLastRptID().get_orig();
      int mwSeqNum  = atoi(iter->ClientMsgID_.c_str());
      
      //-------------------------------------
      // �����T��
      char     logMsg[256], logInfo[128];
      int      logLen;
      TErrCode excCode;
      
      sprintf(logInfo, "Tmp[%d], Mw[%d]", tmpSeqNum, mwSeqNum);

      logLen = sprintf(logMsg, "<TTwseSesApaSmp::OnSmpGetMiddlewareInfoResponse> ClientMsgID, %s", logInfo);
      inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);

      if (tmpSeqNum > mwSeqNum) {
         // ���`���A
         obNextSno = mwSeqNum;
         excCode   = TTwseExcCode::GetXTwseCode(tec_OutMsgSeqNumAbove);
      }
      else if (tmpSeqNum == mwSeqNum) {
         // ���`���A
         obNextSno = mwSeqNum;
         excCode   = TTwseExcCode::GetXTwseCode(tec_OutMsgSeqNumEqual);
      }
      else {
         // �����`���A, ���i��O eod ��, Appia �ݪ� CliMsgID �S���M
         obNextSno = 0;
         excCode   = TTwseExcCode::GetXTwseCode(tec_OutMsgSeqNumLess);
      }
      
      //-------------------------------------
      // �o�e�ƥ�
      TExcMgrBase* excMgr = GetExcMgr();
      if (excMgr) {
         excMgr->Alert(excCode, TTwseExcCode::OutMsgSeqNum, std::string(logInfo));
      }

      found = true;
      break;
   }

   //==============================================
   // �p�G SessionID ���ŦX,�N�_�u���s�o
   if (!found) {
      TExcMgrBase* excMgr = GetExcMgr();
      if (excMgr)
         excMgr->Alert(TTwseExcCode::GetXTwseCode(tec_AppiaMwSessionIDNotMatch), TTwseExcCode::LinkState);

      Reopen("TMP SessionID �P AppiaMW ����", inherited::ApaMwInfo_.ReopenItvl_);
      return;
   }

   //==============================================
   // �إ� FixRptHndEv ����
   // �u�n�M Appia �s�W, �N�i�H��[�^���q��]�����˵� Appia �o
   FixRptHndEv_.reset(new TFixRptHndEvRef(fixRptCenter->GetRsRpt(), this, obNextSno));

   //==============================================
   // �o�X���O�h�d�ߥثe Fix Session �����A
   inherited::SmpGetSessionInfoRequest();

   //==============================================
   // �}�l���� Fix ���
   inherited::SmpStartMiddlwareEvents();
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnSmpGetSessionInfoResponse (TSmpMsgGetSessionInfoResponse* smpMsg)
{
   const SSmpMsgGetSessionInfoResponse& data = smpMsg->GetData();
   if (data.SessionID_ != inherited::ApaSesID_)
      return;

   char logMsg[256];
   int  logLen;

   //==============================================
   // ��� Fix ������
   logLen = sprintf(logMsg, "ProtocolVersion = %s", data.ProtocolVersion_.c_str());
   TUtilComm::ConsMsg(logLen, logMsg);

   //==============================================
   // �˵����A
   // ConnectState
   switch (data.ConnectState_) {
      case scs_Disconnected:           FixStatus_ = efs_NotLink;           break;
      case scs_SocketConnection:       FixStatus_ = set_SessionConnected;  break;
      case scs_ApplicationConnection:  FixStatus_ = efs_Logon;             break;
      case scs_ReceivedLogonConnection:
         {
            // Client �ݪ��s�u�|�]���o�˦ӥd��,�ҥH�s�����s�L�a
            logLen = sprintf(logMsg, "<SessionInfoResponse> ConnectState[Received Logon], �o���_�u���O!");
            inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);

            TSmpOperCmd operCmd(esc_Disconnect, data.SessionID_);
            inherited::SendCmd(operCmd);
         }
         break;
   }

   //==============================================
   // DR �ݪ��Ѽ�
   bool isDr = IsDr();
   bool isDrFinish;

   if (isDr)
      isDrFinish = IsDrFinish();

   //==============================================
   // �p�G�o�{���A�O hold ��,�n�⥦�Ѱ�
   // �]�� appia �Ұʮ�,���i��O������ hold
   if (data.HoldFlag_.is_true()) {
      if (isDr && isDrFinish == false) {
         // �Y�N�Ұ� DR ��Ʀ^��,���Ѱ� Hold ���A
         logLen = sprintf(logMsg, "<SessionInfoResponse> DR �e���^��ƳB�z��, ���Ѱ� Hold ���A!");
         inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
         TUtilComm::ConsMsg(logLen, logMsg);

         inherited::SoleFlag_.set(esf_Hold);
      }
      else if (IsFirstRestore_.IsTrue()) {
         // ���ɥ��b�i��Ĥ@�����^�ɤu�@
         logLen = sprintf(logMsg, "<SessionInfoResponse> �Ĥ@���^�ɶi�椤, ���Ѱ� Hold ���A!");
         inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
         TUtilComm::ConsMsg(logLen, logMsg);

         inherited::SoleFlag_.set(esf_Hold);
      }
      else if (MaxOrderCount_ != 0 && CurOrderCount_ > MaxOrderCount_) {
         // �e�U���Ƥw�W�L�W��,���Ѱ� Hold ���A
         logLen = sprintf(logMsg, "<SessionInfoResponse> �e�U����(%u)�w�W�L�W��,���Ѱ� Hold ���A!", CurOrderCount_);
         inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
         TUtilComm::ConsMsg(logLen, logMsg);

         inherited::SoleFlag_.set(esf_Hold);
      }
      else {
         logLen = sprintf(logMsg, "<SessionInfoResponse> HoldFlag[Y], �o�ʫ��O�Ѱ�!");
         inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);

         TSmpOperCmd operCmd(esc_Release, data.SessionID_);
         inherited::SendCmd(operCmd);
      }
   }
   else {
      // �ˬd�O�_���b�i��Ĥ@�����^�ɤu�@
      // �p�G�O���ܭn���o�e Hold ���O,�� Fix Client �Ȯɤ���U��
      // ==> �p�G�b�o��o�e Hold ���O�|�ӺC�F,��b BfFirstRestore �B�z
   }

   //==============================================
   // ��l���A
        if (data.ServiceDownFlag_.is_true())  FixStatus_ = set_SessionServiceDown;
   else if (data.SuspendFlag_    .is_true())  FixStatus_ = set_SessionSuspend;

   inherited::ShowState();

   //==============================================
   // DR ����ƳB�z
   if (isDr) {
      if (isDrFinish) {
         logLen = sprintf(logMsg, "<SessionInfoResponse> DR �e���^��Ƥw�B�z�L�F,���b�B�z!");
         LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
         TUtilComm::ConsMsg(logLen, logMsg);
      }
      else {
         DrProc();

         if (inherited::SoleFlag_.test(esf_Hold)) {
            logLen = sprintf(logMsg, "<SessionInfoResponse> DR �e���^��ƳB�z����, �o�ʫ��O�Ѱ� Hold!");
            LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
            TUtilComm::ConsMsg(logLen, logMsg);

            TSmpOperCmd operCmd(esc_Release, inherited::ApaSesID_);
            inherited::SendCmd(operCmd);
               
            inherited::SoleFlag_.reset(esf_Hold);
         }
      }
   }
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnSmpMwEv_EndOfRestore (TSmpMsgMiddlewareEvent* smpMsg)
{
   inherited::OnSmpMwEv_EndOfRestore(smpMsg);

   char logMsg[256];
   int  logLen;

   //==============================================
   // �P�_�O�_�O�{���Ұʫ�Ĥ@�����^�ɵ���
   if (IsFirstRestore_.IsTrue()) {
      // �ˬd���e�O�_���e�L Hold ���O
      if (inherited::SoleFlag_.test(esf_Hold)) {
         if (MaxOrderCount_ != 0 && CurOrderCount_ > MaxOrderCount_) {
            // �e�U���Ƥw�W�L�W��,���Ѱ� Hold ���A
            logLen = sprintf(logMsg, "<EndOfRestore> �e�U����(%u)�w�W�L�W��,���Ѱ� Hold ���A!", CurOrderCount_);
            inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
         }
         else {
            logLen = sprintf(logMsg, "<EndOfRestore> �����Ĥ@�����^��, �o�ʫ��O�Ѱ� Hold!");
            LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);   

            TSmpOperCmd operCmd(esc_Release, inherited::ApaSesID_);
            inherited::SendCmd(operCmd);
            
            inherited::SoleFlag_.reset(esf_Hold);
         }
      }
      else {
         logLen = sprintf(logMsg, "<EndOfRestore> �����Ĥ@�����^��!");
         LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
      }
      
      IsFirstRestore_.assign(false);

      TUtilComm::ConsMsg(logMsg);
   }

   //==============================================
   // �o�e�ƥ�
   TExcMgrBase* excMgr = GetExcMgr();
   if (excMgr) {
      const SSmpMsgMiddlewareEvent& data = smpMsg->GetData();
      
      logLen = sprintf(logMsg, "Mw[%d]", data.SequenceNumber_.get_orig());
      
      excMgr->Alert(TTwseExcCode::GetXTwseCode(tec_InMsgSeqNumRcvOK), TTwseExcCode::InMsgSeqNum, std::string(logMsg, logLen));
   }
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnSmpMwEv_SessionEvents (TSmpMsgMiddlewareEvent* smpMsg)
{
   inherited::OnSmpMwEv_SessionEvents(smpMsg);

   const SSmpMsgMiddlewareEvent& data = smpMsg->GetData();

   if (data.EventType_ == set_SessionConnected) {
      // �]�� efs_Logon �|�� set_SessionConnected �٥��o�{
      if (FixStatus_ != efs_Logon)
         FixStatus_ = set_SessionConnected;
   }
   else {
      FixStatus_ = data.EventType_;
   }
   
   inherited::ShowState();
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnSmpMwEv_MessageValidationError (TSmpMsgMiddlewareEvent* smpMsg)
{
   // SOCKET2.0|0000000119|e|23011|2|T5U3U8U0UXU1U|FIX_42_TWSE|8|0|CODE: 0-2-8-6 REQUIRED TAG MISSING: Required field AvgPx (tag#: 6) not found||$
   // �ϥ� ClientMsgID ���e�U�Ѹ�
   const SSmpMsgMiddlewareEvent& crMwEv = smpMsg->GetData();
         TFixSysRptID            fsrid(crMwEv.ClientMsgID_);
         std::string             orderID;
         char                    logMsg[512];
         int                     logLen;

   if (fsrid.is_0() == false) {
      TFixRptCenter* fixRptCenter = FixTradeUnit_->GetFixRptCenter();
      
      orderID = fixRptCenter->FindOrderID(fsrid);
   }

   if (orderID.empty() == false)  logLen = sprintf(logMsg, "(%s) %s", orderID.c_str(),  crMwEv.MetaData_.c_str());
   else                           logLen = sprintf(logMsg, "(%d) %s", fsrid.get_orig(), crMwEv.MetaData_.c_str());

   TExcMgrBase* excMgr = GetExcMgr();
   if (excMgr) {
      excMgr->Alert(TTwseExcCode::GetXTwseCode(tec_AppiaMwOutboundMsgReject),
                    TTwseExcCode::ObMsgRej,
                    std::string(logMsg, logLen));
   }
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::VirFireBfFirstRestore (const TSmpString& apaSesID)
{
   // �C���s�W�u��,�Ĥ@�����^�ɫe
   // �ˬd�O�_���b�i��Ĥ@�����^�ɤu�@
   // �p�G�O���ܭn���o�e Hold ���O,�� Fix Client �Ȯɤ���U��
   if (IsFirstRestore_.IsUnknow()) {
      char logMsg[256];
      int  logLen = sprintf(logMsg, "<BfFirstRestore> �ǳƱҰʲĤ@���^��, �o�� Hold ���O!");

      inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
      TUtilComm::ConsMsg(logLen, logMsg);

      TSmpOperCmd operCmd(esc_Hold, apaSesID);
      inherited::SendCmd(operCmd);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::VirFireFixInMsgSeqNumStatus (const TSmpInt& tmpSeqNum, const TSmpInt& mwSeqNum,
                                                              bool     isChkFirst,      bool     isRestore)
{
   char logMsg[256];
   int  logLen;

   //==============================================
   // �P�_�O�_�O�{���Ұʫ�Ĥ@�����^��
   if (isChkFirst) {
      if (IsFirstRestore_.IsUnknow()) {
         if (isRestore) {
            logLen = sprintf(logMsg, "<FixInMsgSeqNumStatus> �{���Ұʫ�,�}�l�Ĥ@�����^��!");
            IsFirstRestore_.assign(true);
         }
         else {
            logLen = sprintf(logMsg, "<FixInMsgSeqNumStatus> �{���Ұʫ�,���ݭn�ҰʲĤ@�����^��!");
            IsFirstRestore_.assign(false);
         }
   
         TUtilComm::ConsMsg(logMsg);
         LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
      }
      else if (IsFirstRestore_.IsTrue()) {
         if (isRestore) {
            logLen = sprintf(logMsg, "<FixInMsgSeqNumStatus> �{���Ұʫ�,����W�����Ĥ@���^��!");
         }
         else {
            logLen = sprintf(logMsg, "<FixInMsgSeqNumStatus> �{���Ұʫ�,����W�����Ĥ@���^��,���O���ݭn�Ұ�!");
            IsFirstRestore_.assign(false);
         }
   
         TUtilComm::ConsMsg(logMsg);
         LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
      }
   }

   //==============================================
   // �o�e�ƥ�
   TExcMgrBase* excMgr = GetExcMgr();
   TErrCode     excCode;

   if (mwSeqNum > tmpSeqNum) {
      // ���`���p
      excCode = TTwseExcCode::GetXTwseCode(tec_InMsgSeqNumLess);
   }
   else if (mwSeqNum == tmpSeqNum) {
      // ���`���p
      excCode = TTwseExcCode::GetXTwseCode(tec_InMsgSeqNumEqual);
   }
   else {
      // �����`���p
      excCode = TTwseExcCode::GetXTwseCode(tec_InMsgSeqNumAbove);
   }

   if (excMgr) {
      logLen = sprintf(logMsg, "Tmp[%d], Mw[%d]", tmpSeqNum.get_orig(), mwSeqNum.get_orig());

      excMgr->Alert(excCode,  TTwseExcCode::InMsgSeqNum, std::string(logMsg, logLen));
   }
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::VirOnShowState (std::string& str)
{
   char buf[128];
   
   switch (FixStatus_) {
      case efs_NotLink:      strcpy(buf, "Not_Link");      break;
      case efs_Logon:        strcpy(buf, "Logon");         break;
      case efs_LogonReject:  strcpy(buf, "Logon_Reject");  break;
      default:
         {
            TSmpEventType smpEt((ESmpEventType)FixStatus_);
            strcpy(buf, smpEt.as_string(false).c_str());
         }
         break;
   }

   str.append("  Fix:");
   str.append(buf);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnFixLogon (FIX::TMessageRef msgRef, bool isRt)
{
   if (!isRt)
      return;

   ETwseFixRejectCode ret = CheckLogon(msgRef);
   
   SendLogonEcho(msgRef, ret);
}
//---------------------------------------------------------------------------

K_mf(ETwseFixRejectCode) TTwseSesApaSmp::CheckLogon (FIX::TMessageRef msgRef)
{
         Message* fixMsg = msgRef.get();
   const Header&  fixHdr = fixMsg->getHeader();

   //==============================================
   // �ˬd SenderCompID, �T�{ Appia ���t����ƬO�_���T
   // Appia �� SenderCompID(49) �O TandSesID ��
   SenderCompID senderCompID;

   if (!fixHdr.findField(senderCompID) || senderCompID.getValue() != TandSesID_)
      return tfrc_SenderCompIDError;

   //==============================================
   // RawData
   RawData qfRawData;

   if (!fixMsg->findField(qfRawData))
      return tfrc_RawDataLenNotEqual5;
      
   std::string rawData = qfRawData.getValue();
   if (rawData.length() != 5)
      return tfrc_RawDataLenNotEqual5;

   //==============================================
   // RawDataLength
   RawDataLength qfRawDataLength;

   if (!fixMsg->findField(qfRawDataLength))
      return tfrc_RawDataLenNotEqual5;

   if (qfRawDataLength.getValue() != 5)
      return tfrc_RawDataLenNotEqual5;

   //==============================================
   // �ˮ� - AppendNo �O�_�� 0
   char_ary<char, 3, ' '> appendNo;
   int                    nAppendNo;

   appendNo.assign(rawData);
   nAppendNo = appendNo.as_int();
   if (nAppendNo == 0)
      return tfrc_AppendNoEqual0;

   //==============================================
   // �ˮ� - AppendNo �O�_����
   TTwseAppendNoIni iniAppendNo;
   std::string      strAppendNo = appendNo.as_string();

   if (iniAppendNo.IsRepeat(strAppendNo)) {
      // log {
      char logMsg[512];
      
      sprintf(logMsg, "AppendNo����[%s] [%s]", strAppendNo.c_str(), iniAppendNo.GetListStr().c_str());
      TUtilComm::ConsMsg(logMsg);
      inherited::LogFileHnd_.WriteStr(elt_Warning, "<TTwseSesApaSmp::CheckLogon> %s", logMsg);
      // } log
      return tfrc_AppendNoError;
   }

   FIX::MsgSeqNum qfMsgSeqNum;
   fixHdr.findField(qfMsgSeqNum);

   iniAppendNo.SetList(strAppendNo, qfMsgSeqNum.getValue());

   //==============================================
   // �Q�γo�� SessionID �� Main.ini �̭����o Password
   TTwseMainIni iniMain;
   // _TwseVer_NewSesID_
   //std::string  pwd = iniMain.GetFixPassword(inherited::ApaSesID_);
   std::string  pwd = iniMain.GetFixPassword(TandSesID_);
   char         buf[20];
   std::string  keyVal(rawData.substr(3));

   Password_ = atoi(pwd.c_str());

   sprintf(buf, "%02d", ((nAppendNo * Password_ / 100) % 100));
   if (strcmp(buf, keyVal.c_str()) != 0)
      return tfrc_KeyValueError;

   return tfrc_None;
}
//---------------------------------------------------------------------------
   
K_mf(void) TTwseSesApaSmp::SendLogonEcho (FIX::TMessageRef msgRef, ETwseFixRejectCode rejCode)
{
   Message*   srcMsg = msgRef.get();
   TSmpString msgData;

   if (rejCode == tfrc_None) {
      FIX::RawDataLength qfRawDataLength;    // 095
      FIX::RawData       qfRawData;          // 096
      FIX::EncryptMethod qfEncryptMethod;    // 098
      FIX::HeartBtInt    qfHeartBtInt;       // 108

      srcMsg->findField(qfRawDataLength);
      srcMsg->findField(qfRawData);
      srcMsg->findField(qfEncryptMethod);
      srcMsg->findField(qfHeartBtInt);

      //===========================================
      // �^�аT�� Logon
      FIX::MsgType qfMsgType(FIX::MsgType_Logon);
      FIX::Message dstMsg   (qfMsgType);

      dstMsg.setField(qfEncryptMethod);
      dstMsg.setField(qfHeartBtInt);

      dstMsg.setField(qfRawDataLength);
      dstMsg.setField(qfRawData);

      dstMsg.toString(msgData);

      FixStatus_ = efs_Logon;
      TUtilComm::ConsMsg("Fix[%s] Logon OK!", TandSesID_.c_str());
   }
   else {
      std::string code   = TTwseRes::GetStsCodeByRejCode(rejCode, Market_);
      std::string errMsg = GetErrMsg(code.c_str());
      FIX::Text   qfText(errMsg);

      //===========================================
      // �^�аT�� Logout
      FIX::MsgType qfMsgType(FIX::MsgType_Logout);
      FIX::Message dstMsg   (qfMsgType);

      dstMsg.setField(qfText);
      dstMsg.toString(msgData);

      FixStatus_ = efs_LogonReject;
      TUtilComm::ConsMsg("Fix[%s] Logon Failure!", TandSesID_.c_str());
   }

   //==============================================
   // �}�l�ǰe
   // �� ClientMsgID �C�J RptCenter ����, ���O���g�� RptCenter �ǰe��
   TFixRptCenter* fixRptCenter = FixTradeUnit_->GetFixRptCenter();
   TFixMsgType2   fixMsgType;
   TFixSysRptID   fixRptID;

   if (FixStatus_ == efs_Logon)  fixMsgType.assign(MsgType_Logon);
   else                          fixMsgType.assign(MsgType_Logout);

   fixRptID = fixRptCenter->NewRpt(TFixSysOrdID(), fixMsgType, msgData);

   if (rejCode != tfrc_SenderCompIDError) {
      inherited::SmpOutboundMessageDelivery(fixRptID.get_orig(), fixMsgType.as_string(), msgData, NULL);
   }
   else {
      //===========================================
      // �����n�����D�q���� Fix SessionID �e�L�Ӫ�, �n�����^�h
      const Header&      fixHdr = srcMsg->getHeader();
            SenderCompID senderCompID;
            TSmpString   sessionID;
            
      fixHdr.findField(senderCompID);
      sessionID = senderCompID.getValue();

      inherited::SmpOutboundMessageDelivery(fixRptID.get_orig(), fixMsgType.as_string(), msgData, &sessionID);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnFixNewOrderSingle (FIX::TMessageRef msgRef, bool isRt)
{
   TFixOfReqNos reqNos     = TFixOrdFlow::CreateReqNos(msgRef);
   TFixOrdFlow* fixOrdFlow = FixTradeUnit_->GetFixOrdFlow();

   fixOrdFlow->DoReqNos(reqNos);

   //==============================================
   // �ˬd�e�U���ƬO�_���W�L
   if (MaxOrderCount_ == 0)
      return;

   CurOrderCount_ = fixOrdFlow->GetOrdCenter()->GetOrdCount();
   if (CurOrderCount_ <= MaxOrderCount_)
      return;

   // �o�e�T��
   char logMsg[512];
   int  logLen = sprintf(logMsg, "�e�U����(%u)�W�L�W��(%u)", CurOrderCount_, MaxOrderCount_);
   
   TExcMgrBase* excMgr = GetExcMgr();
   if (excMgr) {
      excMgr->Alert(TTwseExcCode::GetXTwseCode(tec_AppiaMwOrderCountOver),
                    TTwseExcCode::OrdCnt,
                    std::string(logMsg, logLen));
   }

   // �ˬd�O�_�o�e Hold
   if (inherited::SoleFlag_.test(esf_Hold) == false) {
      char buf[512];
      
      logLen = sprintf(buf, "<OnFixNewOrderSingle> %s, �o�� Hold ���O!", logMsg);
      LogFileHnd_.WriteMem(elt_Info, buf, logLen);   

      TSmpOperCmd operCmd(esc_Hold, inherited::ApaSesID_);
      inherited::SendCmd(operCmd);
            
      inherited::SoleFlag_.set(esf_Hold);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnFixOrderCancelRequest (FIX::TMessageRef msgRef, bool isRt)
{
   TFixOfReqOcr reqOcr     = TFixOrdFlow::CreateReqOcr(msgRef);
   TFixOrdFlow* fixOrdFlow = FixTradeUnit_->GetFixOrdFlow();

   fixOrdFlow->DoReqOcr(reqOcr);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnFixOrderCancelReplaceRequest (FIX::TMessageRef msgRef, bool isRt)
{
   TFixOfReqOcrr reqOcrr    = TFixOrdFlow::CreateReqOcrr(msgRef);
   TFixOrdFlow*  fixOrdFlow = FixTradeUnit_->GetFixOrdFlow();

   fixOrdFlow->DoReqOcrr(reqOcrr);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnFixOrderStatusRequest (FIX::TMessageRef msgRef, bool isRt)
{
   TFixOfReqOsr reqOsr     = TFixOrdFlow::CreateReqOsr(msgRef);
   TFixOrdFlow* fixOrdFlow = FixTradeUnit_->GetFixOrdFlow();

   fixOrdFlow->DoReqOsr(reqOsr);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnFixAppOther (const FIX::TFixMsgType2& msgType, FIX::TMessageRef msgRef, bool)
{
   const FIX::Message* fixMsg = msgRef.get();
   const FIX::Header&  fixHdr = fixMsg->getHeader();

   //==============================================
   // 372.RefMsgType
   // 380.BusinessRejectReason
   FIX::RefMsgType           qfRefMsgType          (msgType.as_string());
   FIX::BusinessRejectReason qfBusinessRejectReason(FIX::BusinessRejectReason_UNSUPPORTED_MESSAGE_TYPE);

   //==============================================
   // 045.RefSeqNum
   FIX::MsgSeqNum qfMsgSeqNum;
   FIX::RefSeqNum qfRefSeqNum;

   if (fixHdr.findField(qfMsgSeqNum))
      qfRefSeqNum.setValue(qfMsgSeqNum.getValue());

   //==============================================
   // 058.Text
   //FIX::TargetSubID qfTargetSubID;
   //fixHdr.findField(qfTargetSubID);
   std::string code   = TTwseRes::GetStsCodeByRejCode(tfrc_MsgTypeError, Market_);
   std::string errMsg = GetErrMsg(code.c_str());
   FIX::Text   qfText(errMsg);

   //==============================================
   // �}�l�^��
   FIX::TFixMsgType2 newMsgType(FIX::MsgType_BusinessMessageReject);
   std::string       msg;

   // �^�аT�� BusinessMessageReject
   FIX::MsgType qfMsgType(FIX::MsgType_BusinessMessageReject);
   FIX::Message echoMsg  (qfMsgType);

   echoMsg.setField(qfRefMsgType);
   echoMsg.setField(qfBusinessRejectReason);

   echoMsg.setField(qfRefSeqNum);
   echoMsg.setField(qfText);

   echoMsg.toString(msg);

   TFixRptCenter* fixRptCenter = FixTradeUnit_->GetFixRptCenter();
   fixRptCenter->NewRpt(TFixSysOrdID(), newMsgType, msg);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnTwseTimer (TTimerBase* timerBase)
{
   if (SmpGetMwInfoTimer_.get() == timerBase) {
      inherited::SmpGetMiddlewareInfoRequest();
      SmpGetMwInfoTimer_.reset();
   }
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseSesApaSmp::TTwseSesApaSmp::IsDr ()
{
   TTwseDrIni iniDr;
   return iniDr.IsDR();
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseSesApaSmp::IsDrFinish ()
{
   TTwseDrInfoIni iniDrInfo;
   return iniDrInfo.IsProcFinish();
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::DrProc ()
{
   char logMsg[128];      
   int  logLen;

   if (FixTradeUnit_.get() == NULL) {
      FIX::Stk::TTradeMgr& fixTradeMgr = FIX::Stk::TTradeMgr::GetInst();

      FixTradeUnit_ = fixTradeMgr.QueryUnit(TandSesID_);
   }

   TTwseFstInfo*  fstInfo = static_cast<TTwseFstInfo*>(FixTradeUnit_->GetFixTradeInfo());
   TTwseDrProc    proc(fstInfo);
   TTwseDrInfoIni iniDrInfo;

   logLen = sprintf(logMsg, "<DrProc> �}�l�B�z!");
   LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
   TUtilComm::ConsMsg(logLen, logMsg);

   proc.Start(this);

   logLen = sprintf(logMsg, "<DrProc> �B�z����!");
   LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
   TUtilComm::ConsMsg(logLen, logMsg);
      
   iniDrInfo.SetProcFinish();
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::DrPolling ()
{
   // �b�B�z DR �e���^�ɷ|�y�� Heartbeat Timeout, �ҥH�Ӥ��_��s�ɶ�
   SApaMwInfo& apaMwInfo = inherited::ApaMwInfo_;

   if (apaMwInfo.TmChkHb_.IsEnable())
      apaMwInfo.TmChkHb_.SetTimeout(apaMwInfo.ItvlBfSendHb_, true);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnGetFixVer ()
{
   TFixTradeUnit* fixTradeUnit = FixTradeUnit_.get();

   if (fixTradeUnit) {
      TFixOrdFlow* fixOrdFlow = fixTradeUnit->GetFixOrdFlow();
      
      if (fixOrdFlow) {
         fixOrdFlow->SetFixVer(inherited::FixVer_);
         // �����T��
         char logMsg[128];
         int  logLen;
         
         logLen = sprintf(logMsg, "�]�w Fix ����(%d)", FixVer_);
         
         LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
         TUtilComm::ConsMsg(logMsg);
      }
   }
}
//---------------------------------------------------------------------------
