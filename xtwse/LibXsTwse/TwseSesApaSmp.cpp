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
   // 先在這邊打個時間, 代表 appia 已經收到了
   // 等以後在修改
   //TFixRptCenter::TRsRptU upd;
   //inherited::GetUpd(key, upd);
   
   //SFixStkRpt& updDat = *(*upd);
   //updDat.SentTime_ = TTimeStamp::Now();
   
   if (datWsRef->MsgType_ == MsgType_Logon || datWsRef->MsgType_ == MsgType_Logout)
      return esr_MoveNext;

   //==============================================
   // 減少 func call
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
   // 使用參數的 SessionID
   std::string apaSesID = GetProcName();

   TandSesID_ = apaSesID;
   // 需要將 Tandem 的代號轉成 Appia 的
   TTwseSesIDTand tandSesID(apaSesID);
   apaSesID = tandSesID.as_string_Apa();
   
   inherited::ApaSesID_        .assign(apaSesID);
   inherited::FixTargetCompID_ = TandSesID_;

   TUtilComm::ConsMsg("ApaConnSesID: %s", inherited::ApaSesID_.c_str());

   //==============================================
   // 設定參數
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
   // 設定 Debug Level
   byte dbgLevel = GetDebugLevel();

   inherited::PktFileHnd_.UseQueue(false);
   //inherited::LogFileHnd_.UseQueue(true);
   inherited::LogFileHnd_.UseQueue(false);

   inherited::SesProp_.reset();
   inherited::SesProp_.set(esp_Disable_PktLog);

   // 接收封包 off
   // 傳送封包 off
   // 全部封包 off
   if (dbgLevel == 0) {
      inherited::SesProp_.set(esp_Disable_RecvLog);
      inherited::SesProp_.set(esp_Disable_SendLog);
   }
   // 接收封包 on, 使用 thread
   // 傳送封包 on, 使用 thread
   // 全部封包 off
   else if (dbgLevel == 1) {
   }
   // 接收封包 on, 使用 thread
   // 傳送封包 on, 使用 thread
   // 全部封包 off
   else if (dbgLevel == 2) {
   }
   // 接收封包 on, 直接寫檔
   // 傳送封包 on, 直接寫檔
   // 全部封包 off
   else if (dbgLevel == 3) {
      inherited::LogFileHnd_.UseQueue(false);
   }
   // 接收封包 on, 直接寫檔
   // 傳送封包 on, 直接寫檔
   // 全部封包 on, 直接寫檔
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
   // 設定 IP/Port list
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
   // 建立 FixTradeUnit 物件
   FIX::Stk::TTradeMgr& fixTradeMgr = FIX::Stk::TTradeMgr::GetInst();
   
   // ConnSesID    = T5U3U8U0UXU1U
   // TargetCompID = T5380X1
   // 所以這邊要使用 TandSesID
   FixTradeUnit_ = fixTradeMgr.QueryUnit(TandSesID_);
   
   // 取得目前已委託的筆數
   TFixOrdFlow* fixOrdFlow = FixTradeUnit_->GetFixOrdFlow();

   CurOrderCount_ = fixOrdFlow->GetOrdCenter()->GetOrdCount();

   //==============================================
   // 和 Appia 詢問我的 MwConfig
   //inherited::OnSmpLogon();
   inherited::SmpGetMiddlewareConfig();
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnSmpReject (TSmpMsgReject* smpMsg)
{
   const SSmpMsgReject& data      = smpMsg->GetData();
   ESmpRejectReason     rejReason = data.RejReason_.get_orig();

   //==============================================
   // 檢查是否是 [SessionInfo does not exist for xxxx]
   if (rejReason == srr_SesInfo_SesIDNotFound) {
      if (data.RejParam1_ == inherited::ApaSesID_) {
         std::string reason("AppiaMw: SessionInfo does not exist");
         
         inherited::Reopen(reason, inherited::ApaMwInfo_.ReopenItvl_);
      }
   }
}
//---------------------------------------------------------------------------
// 這個方法行不通, 這邊下 RemoveMiddleware 指令, 只會把自己的連線斷掉
/*
K_mf(void) TTwseSesApaSmp::OnSmpReject_AlreadyLogon ()
{
   //==============================================
   // 傳送 cmd:remove_middleware 指令
   TSmpOperCmd operCmd(esc_RemoveMiddleware, inherited::ApaSesID_);
   SendCmd(operCmd);

   //==============================================
   // 重新 logon
   inherited::SmpLogon();
}
*/
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnSmpGetMiddlewareConfigResponse (TSmpMsgGetMiddlewareConfigResponse* smpMsg)
{
   inherited::OnSmpGetMiddlewareConfigResponse(smpMsg);

   //==============================================
   // 檢查 SessionID 是否有效
   if (inherited::ApaSesID_.empty())
      return;

   //==============================================
   // 檢查 MW 的設定
   const SSmpMiddlewareConfig& mwConf    = smpMsg->GetData();
   const TSmpStringList&       fixSesIDs = mwConf.SessionIDs_;

   if (!fixSesIDs.empty()) {
      // 試試看可不可以找到我的 SessionID
      bool found = false;
      
      for (TSmpStringList::const_iterator iter=fixSesIDs.begin(); iter!=fixSesIDs.end(); ++iter) {
         if (inherited::ApaSesID_ == *iter) {
            found = true;
            break;
         }
      }
         
      if (!found) {
         // 找不到我的 SessionID
         char logMsg[256];
         int  logLen = sprintf(logMsg, "<TTwseSesApaSmp::OnSmpGetMiddlewareInfoResponse> MW 設定找不到我的 SessionID[%s]!",
                                             inherited::ApaSesID_.c_str());
      
         inherited::LogFileHnd_.WriteMem(elt_Warning, logMsg, logLen);
            
         TExcMgrBase* excMgr = GetExcMgr();
         if (excMgr)
            excMgr->Alert(TTwseExcCode::GetXTwseCode(tec_AppiaMwSessionIDNotMatch), TTwseExcCode::LinkState);
               
         Reopen("TMP SessionID 與 AppiaMW 不符", inherited::ApaMwInfo_.ReopenItvl_);
      }
   }
   else {
      // SessionIDs 是空白的, 把我的 SessionID 設定進去吧

      // log {
      char logMsg[256];
      int  logLen = sprintf(logMsg, "<TTwseSesApaSmp::OnSmpGetMiddlewareInfoResponse> MW 沒有設定 SessionID, 自動設定自己的 SessionID[%s]",
                                          inherited::ApaSesID_.c_str());
      
      inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
      // } log

      SSmpMiddlewareConfig mwConfNew(mwConf);

      mwConfNew.SessionIDs_.push_back(inherited::ApaSesID_);
      inherited::SmpSetMiddlewareConfigRequest(mwConfNew);
   }

   //==============================================
   // 詢問 appia 目前有那些 SessionID
   //inherited::SmpGetMiddlewareInfoRequest();
   SmpGetMwInfoTimer_.reset(new TTwseTimerType(this));
   SmpGetMwInfoTimer_->Start(0, &this_type::OnTwseTimer);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::OnSmpGetMiddlewareInfoResponse (TSmpMsgGetMiddlewareInfoResponse* smpMsg)
{
   inherited::OnSmpGetMiddlewareInfoResponse(smpMsg);

   //==============================================
   // 檢查 MW 的設定
   const SSmpMsgGetMiddlewareInfoResponse& data      = smpMsg->GetData();
   const SSmpMiddlewareConfig&             mwConf    = data.MiddlewareConfig_.GetData();
   const TSmpStringList&                   fixSesIDs = mwConf.SessionIDs_;

   //==============================================
   // 取得市場別
   Market_ = GetProcMkt();

   //==============================================
   // 建立 FixTradeUnit 物件
   FIX::Stk::TTradeMgr& fixTradeMgr = FIX::Stk::TTradeMgr::GetInst();
   TFixRptCenter*       fixRptCenter;
   
   // 在 OnSmpLogon 時已經做過了, 所以這邊可以不需要
   //FixTradeUnit_ = fixTradeMgr.QueryUnit(TandSesID_);
   
   fixRptCenter = FixTradeUnit_->GetFixRptCenter();

   //==============================================
   // 先看看封包裡面的 ClientMsgID 送到多少了
   const TSmpLastMsgClientIDA& lmcis = data.LastMsgClientIDs_.GetData();
         bool found     = true;
         int  obNextSno = 0;
   
   for (TSmpLastMsgClientIDACi iter=lmcis.begin(); iter!=lmcis.end(); ++iter) {
      if (iter->SessionID_ != inherited::ApaSesID_)
         continue;
      
      int tmpSeqNum = fixRptCenter->GetLastRptID().get_orig();
      int mwSeqNum  = atoi(iter->ClientMsgID_.c_str());
      
      //-------------------------------------
      // 紀錄訊息
      char     logMsg[256], logInfo[128];
      int      logLen;
      TErrCode excCode;
      
      sprintf(logInfo, "Tmp[%d], Mw[%d]", tmpSeqNum, mwSeqNum);

      logLen = sprintf(logMsg, "<TTwseSesApaSmp::OnSmpGetMiddlewareInfoResponse> ClientMsgID, %s", logInfo);
      inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);

      if (tmpSeqNum > mwSeqNum) {
         // 正常狀態
         obNextSno = mwSeqNum;
         excCode   = TTwseExcCode::GetXTwseCode(tec_OutMsgSeqNumAbove);
      }
      else if (tmpSeqNum == mwSeqNum) {
         // 正常狀態
         obNextSno = mwSeqNum;
         excCode   = TTwseExcCode::GetXTwseCode(tec_OutMsgSeqNumEqual);
      }
      else {
         // 不正常狀態, 有可能是 eod 後, Appia 端的 CliMsgID 沒有清
         obNextSno = 0;
         excCode   = TTwseExcCode::GetXTwseCode(tec_OutMsgSeqNumLess);
      }
      
      //-------------------------------------
      // 發送事件
      TExcMgrBase* excMgr = GetExcMgr();
      if (excMgr) {
         excMgr->Alert(excCode, TTwseExcCode::OutMsgSeqNum, std::string(logInfo));
      }

      found = true;
      break;
   }

   //==============================================
   // 如果 SessionID 不符合,就斷線重連囉
   if (!found) {
      TExcMgrBase* excMgr = GetExcMgr();
      if (excMgr)
         excMgr->Alert(TTwseExcCode::GetXTwseCode(tec_AppiaMwSessionIDNotMatch), TTwseExcCode::LinkState);

      Reopen("TMP SessionID 與 AppiaMW 不符", inherited::ApaMwInfo_.ReopenItvl_);
      return;
   }

   //==============================================
   // 建立 FixRptHndEv 物件
   // 只要和 Appia 連上, 就可以把[回報電文]全部倒給 Appia 囉
   FixRptHndEv_.reset(new TFixRptHndEvRef(fixRptCenter->GetRsRpt(), this, obNextSno));

   //==============================================
   // 發出指令去查詢目前 Fix Session 的狀態
   inherited::SmpGetSessionInfoRequest();

   //==============================================
   // 開始接收 Fix 資料
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
   // 顯示 Fix 版本號
   logLen = sprintf(logMsg, "ProtocolVersion = %s", data.ProtocolVersion_.c_str());
   TUtilComm::ConsMsg(logLen, logMsg);

   //==============================================
   // 檢視狀態
   // ConnectState
   switch (data.ConnectState_) {
      case scs_Disconnected:           FixStatus_ = efs_NotLink;           break;
      case scs_SocketConnection:       FixStatus_ = set_SessionConnected;  break;
      case scs_ApplicationConnection:  FixStatus_ = efs_Logon;             break;
      case scs_ReceivedLogonConnection:
         {
            // Client 端的連線會因為這樣而卡住,所以叫它重連過吧
            logLen = sprintf(logMsg, "<SessionInfoResponse> ConnectState[Received Logon], 發動斷線指令!");
            inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);

            TSmpOperCmd operCmd(esc_Disconnect, data.SessionID_);
            inherited::SendCmd(operCmd);
         }
         break;
   }

   //==============================================
   // DR 端的參數
   bool isDr = IsDr();
   bool isDrFinish;

   if (isDr)
      isDrFinish = IsDrFinish();

   //==============================================
   // 如果發現狀態是 hold 時,要把它解除
   // 因為 appia 啟動時,有可能是全部為 hold
   if (data.HoldFlag_.is_true()) {
      if (isDr && isDrFinish == false) {
         // 即將啟動 DR 資料回補,不解除 Hold 狀態
         logLen = sprintf(logMsg, "<SessionInfoResponse> DR 委成回資料處理中, 不解除 Hold 狀態!");
         inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
         TUtilComm::ConsMsg(logLen, logMsg);

         inherited::SoleFlag_.set(esf_Hold);
      }
      else if (IsFirstRestore_.IsTrue()) {
         // 此時正在進行第一次的回補工作
         logLen = sprintf(logMsg, "<SessionInfoResponse> 第一次回補進行中, 不解除 Hold 狀態!");
         inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
         TUtilComm::ConsMsg(logLen, logMsg);

         inherited::SoleFlag_.set(esf_Hold);
      }
      else if (MaxOrderCount_ != 0 && CurOrderCount_ > MaxOrderCount_) {
         // 委託筆數已超過上限,不解除 Hold 狀態
         logLen = sprintf(logMsg, "<SessionInfoResponse> 委託筆數(%u)已超過上限,不解除 Hold 狀態!", CurOrderCount_);
         inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
         TUtilComm::ConsMsg(logLen, logMsg);

         inherited::SoleFlag_.set(esf_Hold);
      }
      else {
         logLen = sprintf(logMsg, "<SessionInfoResponse> HoldFlag[Y], 發動指令解除!");
         inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);

         TSmpOperCmd operCmd(esc_Release, data.SessionID_);
         inherited::SendCmd(operCmd);
      }
   }
   else {
      // 檢查是否正在進行第一次的回補工作
      // 如果是的話要先發送 Hold 指令,讓 Fix Client 暫時不能下單
      // ==> 如果在這邊發送 Hold 指令會太慢了,改在 BfFirstRestore 處理
   }

   //==============================================
   // 其餘狀態
        if (data.ServiceDownFlag_.is_true())  FixStatus_ = set_SessionServiceDown;
   else if (data.SuspendFlag_    .is_true())  FixStatus_ = set_SessionSuspend;

   inherited::ShowState();

   //==============================================
   // DR 的資料處理
   if (isDr) {
      if (isDrFinish) {
         logLen = sprintf(logMsg, "<SessionInfoResponse> DR 委成回資料已處理過了,不在處理!");
         LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
         TUtilComm::ConsMsg(logLen, logMsg);
      }
      else {
         DrProc();

         if (inherited::SoleFlag_.test(esf_Hold)) {
            logLen = sprintf(logMsg, "<SessionInfoResponse> DR 委成回資料處理完畢, 發動指令解除 Hold!");
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
   // 判斷是否是程式啟動後第一次的回補結束
   if (IsFirstRestore_.IsTrue()) {
      // 檢查之前是否有送過 Hold 指令
      if (inherited::SoleFlag_.test(esf_Hold)) {
         if (MaxOrderCount_ != 0 && CurOrderCount_ > MaxOrderCount_) {
            // 委託筆數已超過上限,不解除 Hold 狀態
            logLen = sprintf(logMsg, "<EndOfRestore> 委託筆數(%u)已超過上限,不解除 Hold 狀態!", CurOrderCount_);
            inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
         }
         else {
            logLen = sprintf(logMsg, "<EndOfRestore> 結束第一次的回補, 發動指令解除 Hold!");
            LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);   

            TSmpOperCmd operCmd(esc_Release, inherited::ApaSesID_);
            inherited::SendCmd(operCmd);
            
            inherited::SoleFlag_.reset(esf_Hold);
         }
      }
      else {
         logLen = sprintf(logMsg, "<EndOfRestore> 結束第一次的回補!");
         LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
      }
      
      IsFirstRestore_.assign(false);

      TUtilComm::ConsMsg(logMsg);
   }

   //==============================================
   // 發送事件
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
      // 因為 efs_Logon 會比 set_SessionConnected 還先發現
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
   // 使用 ClientMsgID 找到委託書號
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
   // 每次連上線後,第一次的回補前
   // 檢查是否正在進行第一次的回補工作
   // 如果是的話要先發送 Hold 指令,讓 Fix Client 暫時不能下單
   if (IsFirstRestore_.IsUnknow()) {
      char logMsg[256];
      int  logLen = sprintf(logMsg, "<BfFirstRestore> 準備啟動第一次回補, 發動 Hold 指令!");

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
   // 判斷是否是程式啟動後第一次的回補
   if (isChkFirst) {
      if (IsFirstRestore_.IsUnknow()) {
         if (isRestore) {
            logLen = sprintf(logMsg, "<FixInMsgSeqNumStatus> 程式啟動後,開始第一次的回補!");
            IsFirstRestore_.assign(true);
         }
         else {
            logLen = sprintf(logMsg, "<FixInMsgSeqNumStatus> 程式啟動後,不需要啟動第一次的回補!");
            IsFirstRestore_.assign(false);
         }
   
         TUtilComm::ConsMsg(logMsg);
         LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
      }
      else if (IsFirstRestore_.IsTrue()) {
         if (isRestore) {
            logLen = sprintf(logMsg, "<FixInMsgSeqNumStatus> 程式啟動後,接續上次的第一次回補!");
         }
         else {
            logLen = sprintf(logMsg, "<FixInMsgSeqNumStatus> 程式啟動後,接續上次的第一次回補,但是不需要啟動!");
            IsFirstRestore_.assign(false);
         }
   
         TUtilComm::ConsMsg(logMsg);
         LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
      }
   }

   //==============================================
   // 發送事件
   TExcMgrBase* excMgr = GetExcMgr();
   TErrCode     excCode;

   if (mwSeqNum > tmpSeqNum) {
      // 正常情況
      excCode = TTwseExcCode::GetXTwseCode(tec_InMsgSeqNumLess);
   }
   else if (mwSeqNum == tmpSeqNum) {
      // 正常情況
      excCode = TTwseExcCode::GetXTwseCode(tec_InMsgSeqNumEqual);
   }
   else {
      // 不正常情況
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
   // 檢查 SenderCompID, 確認 Appia 分配的資料是否正確
   // Appia 的 SenderCompID(49) 是 TandSesID 喔
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
   // 檢核 - AppendNo 是否為 0
   char_ary<char, 3, ' '> appendNo;
   int                    nAppendNo;

   appendNo.assign(rawData);
   nAppendNo = appendNo.as_int();
   if (nAppendNo == 0)
      return tfrc_AppendNoEqual0;

   //==============================================
   // 檢核 - AppendNo 是否重複
   TTwseAppendNoIni iniAppendNo;
   std::string      strAppendNo = appendNo.as_string();

   if (iniAppendNo.IsRepeat(strAppendNo)) {
      // log {
      char logMsg[512];
      
      sprintf(logMsg, "AppendNo重複[%s] [%s]", strAppendNo.c_str(), iniAppendNo.GetListStr().c_str());
      TUtilComm::ConsMsg(logMsg);
      inherited::LogFileHnd_.WriteStr(elt_Warning, "<TTwseSesApaSmp::CheckLogon> %s", logMsg);
      // } log
      return tfrc_AppendNoError;
   }

   FIX::MsgSeqNum qfMsgSeqNum;
   fixHdr.findField(qfMsgSeqNum);

   iniAppendNo.SetList(strAppendNo, qfMsgSeqNum.getValue());

   //==============================================
   // 利用這個 SessionID 到 Main.ini 裡面取得 Password
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
      // 回覆訊息 Logon
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
      // 回覆訊息 Logout
      FIX::MsgType qfMsgType(FIX::MsgType_Logout);
      FIX::Message dstMsg   (qfMsgType);

      dstMsg.setField(qfText);
      dstMsg.toString(msgData);

      FixStatus_ = efs_LogonReject;
      TUtilComm::ConsMsg("Fix[%s] Logon Failure!", TandSesID_.c_str());
   }

   //==============================================
   // 開始傳送
   // 把 ClientMsgID 列入 RptCenter 控管, 但是不經由 RptCenter 傳送喔
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
      // 必須要先知道從那個 Fix SessionID 送過來的, 好對應回去
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
   // 檢查委託筆數是否有超過
   if (MaxOrderCount_ == 0)
      return;

   CurOrderCount_ = fixOrdFlow->GetOrdCenter()->GetOrdCount();
   if (CurOrderCount_ <= MaxOrderCount_)
      return;

   // 發送訊息
   char logMsg[512];
   int  logLen = sprintf(logMsg, "委託筆數(%u)超過上限(%u)", CurOrderCount_, MaxOrderCount_);
   
   TExcMgrBase* excMgr = GetExcMgr();
   if (excMgr) {
      excMgr->Alert(TTwseExcCode::GetXTwseCode(tec_AppiaMwOrderCountOver),
                    TTwseExcCode::OrdCnt,
                    std::string(logMsg, logLen));
   }

   // 檢查是否發送 Hold
   if (inherited::SoleFlag_.test(esf_Hold) == false) {
      char buf[512];
      
      logLen = sprintf(buf, "<OnFixNewOrderSingle> %s, 發動 Hold 指令!", logMsg);
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
   // 開始回覆
   FIX::TFixMsgType2 newMsgType(FIX::MsgType_BusinessMessageReject);
   std::string       msg;

   // 回覆訊息 BusinessMessageReject
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

   logLen = sprintf(logMsg, "<DrProc> 開始處理!");
   LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
   TUtilComm::ConsMsg(logLen, logMsg);

   proc.Start(this);

   logLen = sprintf(logMsg, "<DrProc> 處理完畢!");
   LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
   TUtilComm::ConsMsg(logLen, logMsg);
      
   iniDrInfo.SetProcFinish();
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesApaSmp::DrPolling ()
{
   // 在處理 DR 委成回時會造成 Heartbeat Timeout, 所以來不斷更新時間
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
         // 紀錄訊息
         char logMsg[128];
         int  logLen;
         
         logLen = sprintf(logMsg, "設定 Fix 版本(%d)", FixVer_);
         
         LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
         TUtilComm::ConsMsg(logMsg);
      }
   }
}
//---------------------------------------------------------------------------
