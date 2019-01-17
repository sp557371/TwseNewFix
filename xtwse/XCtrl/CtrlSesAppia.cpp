/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "CtrlSesAppia.h"
#include "ExtDef.h"
#include "Nsa_UtilComm.h"
#include "TwseType.h"
#include "CtrlSesMgr.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// class TCtrlSesAppia impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TCtrlSesAppia::TCtrlSesAppia (TCtrlSesMgr* sesMgr)
   : inherited    ()
   , SesMgr_      (sesMgr)
   , RsReqResuGoh_(true)
   , Timer_       (this)
{
   /*
   // 使用[程式名稱_HostName]當作MiddlewareID
   char buf[512], hostName[256];

   if (gethostname(hostName, sizeof(hostName)) != 0)
      hostName[0] = '\0';

   sprintf(buf, "%s_%s", GetProcName().c_str(), hostName);
   MyName_.assign(buf);
   */

   Tw::Stk::TMarket mkt = GetProcMkt();
   switch (mkt) {
      case Tw::Stk::m_Tse:  MarketChar_ = 'T';   break;
      case Tw::Stk::m_Otc:  MarketChar_ = 'O';   break;
      default:              MarketChar_ = 0x00;  break;
   }

   //==============================================
   // 設定參數
   inherited::ExcCode_LinkReady_  = TTwseExcCode::GetXCtrlCode(tec_AppiaMwLinkReady);
   inherited::ExcCode_LinkBroken_ = TTwseExcCode::GetXCtrlCode(tec_AppiaMwLinkBroken);
   inherited::ExcCode_LinkFail_   = TTwseExcCode::GetXCtrlCode(tec_AppiaMwLinkFail);
   inherited::ExcCode_LinkFails_  = TTwseExcCode::GetXCtrlCode(tec_AppiaMwLinkFails);
}
//---------------------------------------------------------------------------

K_dtor TCtrlSesAppia::~TCtrlSesAppia ()
{
   Timer_.Stop();
   SesMgr_->SetAppiaSes(AppiaGrp_, 0);
}
//---------------------------------------------------------------------------

K_mf(bool) TCtrlSesAppia::VirBfSettingApply (std::string& str)
{
   //==============================================
   // 如果[處理程序參數]是空白的,就用程式名稱
   //if (str.empty())
   //   str = MyName_;

   inherited::MaxLinkFailCount_ = 2;
   inherited::LinkFailCount_    = 0;
   std::string tmp = str;
   const char* tmpc = tmp.c_str();
   char* ch = (char*)strchr(tmpc,'_');
   *ch = 0;
   AppiaGrp_ = std::string(tmpc);
   *ch = '_';
//20130220 eric 讀取所有這個appia group 的FIX session ID
   TKIniFile IniFile(std::string(GetConfPath()+"/Main.ini").c_str());
   int cnt = IniFile.ReadStrings("SESSIONID-APPIAGROUP",AppiaGrp_,FIXSesIDs_);
   return true;
}
//---------------------------------------------------------------------------

K_mf(std::string) TCtrlSesAppia::GetLogPath ()
{
   return GetProcLogPath();
}
//---------------------------------------------------------------------------

K_mf(std::string) TCtrlSesAppia::GetLogName ()
{
   // 預設是使用 MiddlewareID 當作檔名, 目前的 MiddlewareID 是使用 ProcName + HostName
   // 避免切換主機後開啟不ㄧ樣的 log
   // 所以使用 ProcName 當作檔名好了
   char logName[256];
   
   sprintf(logName, "SesApaSmp_%s", GetProcName().c_str());
   return std::string(logName);
}
//---------------------------------------------------------------------------

K_mf(void) TCtrlSesAppia::OnMesLinkReady (TMesBase* mes)
{
   inherited::OnMesLinkReady(mes);
}
//---------------------------------------------------------------------------

K_mf(void) TCtrlSesAppia::OnMesLinkBroken (TMesBase* mes, const std::string& reason)
{
   inherited::OnMesLinkBroken(mes, reason);

   SesMgr_->SetAppiaSes(AppiaGrp_, 0);
   
   Timer_.Stop();

   //==============================================
   // 斷線了, 要去異動資料表狀態
   //if (ReqID_.is_0() == false) {
   //   RsReqResuGoh_->SetProcResuError(ReqID_, epr_TandemLinkBroken);
   //}
}
//---------------------------------------------------------------------------

K_mf(void) TCtrlSesAppia::OnMesLinkFail (TMesBase* mes, const std::string& reason)
{
   inherited::OnMesLinkFail(mes, reason);
}
//---------------------------------------------------------------------------

K_mf(void) TCtrlSesAppia::OnSmpLogon ()
{

   SesMgr_->SetAppiaSes(AppiaGrp_,this);
//   RsReqResuGoh_->FindNewReq(ReqID_);
   Timer_.Start(100, &this_type::OnTimer);
      
   inherited::SmpStartMiddlwareEvents();
}
//---------------------------------------------------------------------------

K_mf(void) TCtrlSesAppia::OnTimer (TTimerBase*)
{
   //==============================================
   // 確認 ReqID 是存在的
/*
   if (ReqID_.is_invalid()) {
      if (RsReqResuGoh_->FindNewReq(ReqID_) == false)
         return;
      TUtilComm::ConsMsg("<TCtrlSesAppia::OnTimer> FindNewReq OK [%s]", ReqID_.as_string().c_str());
   }
*/
   if(ReqID_.is_invalid()){
      ReqID_ = SesMgr_->FindNewReq(AppiaGrp_);
      if(ReqID_.is_invalid())
         return;
      TUtilComm::ConsMsg("%s <TCtrlSesAppia::OnTimer> FindNewReq OK [%s]", AppiaGrp_.c_str(), ReqID_.as_string().c_str());
   }
   
      

   //==============================================
   // 開始檢核這筆 ReqID 的資料是否已經傳送過
   TRsReqResuHnd      rshRr(RsReqResuGoh_.GetPtr(), ReqID_);
   const SRsReqResu1* cpDat1 = rshRr.FindDat1();
   
   //==============================================
   // 檢查狀態是否為[排隊中]
   if (cpDat1->ProcResu_ == epr_LinUp) {
      SRsReqResu1& udDat1 = rshRr.BeginUpdate1();
      udDat1.ProcResu_.assignT(epr_Process);
      SesMgr_->SetAppGrpState(AppiaGrp_, TProcResu(epr_Process));
   }

   //==============================================
   // Appia 要傳送的部分已經 OK 了, 但是結果還沒有回來
/*
   if (cpDat1->ProcResu_ == epr_ApaSent)
      return;
*/
//   if(SesMgr_->GetAppGrpState(AppiaGrp_) == TProcResu(epr_ApaSent))
//      return;
   //==============================================
   // 檢查目前狀態
   EProcResu ret = CheckStatus(rshRr);
   SesMgr_->SetAppGrpState(AppiaGrp_, TProcResu(ret));

   if (ret == epr_Finish) {
      //rshRr.MoveNext1();
      //ReqID_ = rshRr.GetKey1();
      // 下次進 Timer 後在來找有效的 ReqID
      TUtilComm::ConsMsg("Appia 工作完成 [%s]", ReqID_.as_string().c_str());
      ReqID_.clear();
      return;
   }
   else if (ret == epr_ApaSent) {
      return;
   }

   //==============================================
   // 開始檢核這筆 ReqID 的資料那些還沒有傳送過的
   CheckSend(rshRr);
}
//---------------------------------------------------------------------------

K_mf(EProcResu) TCtrlSesAppia::CheckStatus (TRsReqResuHnd& hnd)
{
   //==============================================
   // 開始檢查
   const SRsReqResu1* cpDat1   = hnd.FindDat1();
   ETandFuncCode      funcCode = cpDat1->FuncCode_.get_orig();
//   EProcResu          orgResu  = cpDat1->ProcResu_.get_orig();
   EProcResu          orgResu  = SesMgr_->GetAppGrpState(AppiaGrp_).get_orig();
   EProcResu          defResu  = epr_Process;

   if (orgResu >= epr_Finish)
      return epr_Finish;

   //==============================================
   // 全市場線路操作, 沒有 body
   if (cpDat1->AllMarketFlag_.is_true()) {
      if (orgResu == epr_ApaSent)
         return epr_ApaSent;
      return defResu;
   }

   //==============================================
   // 檢核 body 資料, 沒有 body 就當作已經完成
   if (hnd.MoveFirst2() == false) {
//      SRsReqResu1& udDat1 = hnd.BeginUpdate1();
//      udDat1.ProcResu_.assignT(epr_Finish);
      return epr_Finish;
   }

   //==============================================
   // 拜訪所有 SessionID 的 StatusCode, 看看是不是已經完成了
   bool statusCodeOk = true;
   bool apaSentOk    = true;

   do {
      const SRsReqResu2* cpDat2 = hnd.FindDat2();
      if(!IsMyFIXSes(hnd.GetKey2().as_full()))
         continue;

      if (cpDat2->IsSentOk(funcCode) == false)
         apaSentOk = false;
         
      if (cpDat2->IsFinish(funcCode) == false)
         statusCodeOk = false;

   } while (hnd.MoveNext2());
   
   if (statusCodeOk) {
//      SRsReqResu1& udDat1 = hnd.BeginUpdate1();
         
//      udDat1.ProcResu_.assignT(epr_Finish);
      
      return epr_Finish;
   }
   else if (apaSentOk) {
      // 都已經傳送完畢了, 不需要在繼續下去了
//      SRsReqResu1& udDat1 = hnd.BeginUpdate1();
//      udDat1.ProcResu_.assignT(epr_ApaSent);
      return epr_ApaSent;
   }

   hnd.MoveFirst2();
   return defResu;
}
//---------------------------------------------------------------------------

K_mf(ESmpCmd) TCtrlSesAppia::FuncCodeToSmpCmd (ETandFuncCode funcCode)
{
   switch (funcCode) {
      case tfc_Suspend:               return esc_Suspend;
      case tfc_SuspendWithTerminate:  return esc_SuspendWithTerminate;
      case tfc_Resume:                return esc_Resume;
      case tfc_ServiceDown:           return esc_ServiceDown;
      case tfc_ServiceUp:             return esc_ServiceUp;
      case tfc_Hold:                  return esc_Hold;
      case tfc_Release:               return esc_Release;
      case tfc_SequenceReset:         return esc_OutMsgSeqNum;
   }
   return esc_Unknown;
}
//---------------------------------------------------------------------------

K_mf(void) TCtrlSesAppia::CheckSend (TRsReqResuHnd& hnd)
{
   //==============================================
   // 全市場線路操作
   const SRsReqResu1* cpDat1   = hnd.FindDat1();
   ETandFuncCode      funcCode = cpDat1->FuncCode_.get_orig();

   
   if (cpDat1->AllMarketFlag_.is_true()) {
      if(SesMgr_->GetAppGrpState(AppiaGrp_).get_orig() < epr_ApaSent){
         TSmpOperCmd operCmd(FuncCodeToSmpCmd(funcCode), TSmpString("*"));
         inherited::SendCmd(operCmd);
      }
         
//      if(cpDat1->ProcResu_.get_orig() < epr_ApaSent){
//         SRsReqResu1& udDat1 = hnd.BeginUpdate1();
//         udDat1.ProcResu_.assignT(epr_ApaSent);
//         udDat1.SentTime_ = TTimeStamp::Now();
//      }
      SesMgr_->SetAppGrpState(AppiaGrp_, TProcResu(epr_ApaSent));
      return;
   }

   //==============================================
   // 個別線路操作
   if (funcCode == tfc_StatusQuery) {
      do {
         const SRsReqResu2* cpDat2 = hnd.FindDat2();
         //20130220 eric 檢核FIX Session ID 是否屬於本連線要處理的範圍
         if(!IsMyFIXSes(hnd.GetKey2().as_full()))
            continue;
         if (cpDat2->ApaSentList_.test(asl_Query) == false) {
            SendQuery(hnd.GetKey2());

            SRsReqResu2& udDat2 = hnd.BeginUpdate2();
            udDat2.ApaSentList_.set(asl_Query);
            udDat2.AppiaGrp_.assign(AppiaGrp_);
         }

      } while (hnd.MoveNext2());
   }
   else {
      do {
         const SRsReqResu2* cpDat2 = hnd.FindDat2();
         //20130220 eric 檢核FIX Session ID 是否屬於本連線要處理的範圍
         if(!IsMyFIXSes(hnd.GetKey2().as_full()))
            continue;

         if (cpDat2->ApaSentList_.test(asl_Cmd) == false) {
            // 先將 FixSesID(TandSesID) 轉成 ApaSesID
            TTwseSesIDTand tandSesID  (hnd.GetKey2().as_full());
            std::string    strApaSesID(tandSesID.as_string_Apa());
            
            if (funcCode == tfc_SequenceReset) {
               TSmpOperCmd operCmd(FuncCodeToSmpCmd(funcCode), strApaSesID, cpDat1->NewSeqNum_.as_string());
               inherited::SendCmd(operCmd);
            }
            else {
               TSmpOperCmd operCmd(FuncCodeToSmpCmd(funcCode), strApaSesID);
               inherited::SendCmd(operCmd);
            }

            SRsReqResu2& udDat2 = hnd.BeginUpdate2();
            udDat2.ApaSentList_.set(asl_Cmd);
            udDat2.AppiaGrp_.assign(AppiaGrp_);
         }

      } while (hnd.MoveNext2());
   }
   SesMgr_->SetAppGrpState(AppiaGrp_, TProcResu(epr_ApaSent));
}
//---------------------------------------------------------------------------

K_mf(void) TCtrlSesAppia::SendQuery (const TFixSesID& fixSesID)
{
   // 要將 FixSesID(TandSesID) 轉成 ApaSesID
   TTwseSesIDTand tandSesID  (fixSesID.as_full());
   std::string    strApaSesID(tandSesID.as_string_Apa());

   TSmpMsgGetSessionInfoRequest smpMsg(strApaSesID);
   inherited::SendMsg(smpMsg);
}
//---------------------------------------------------------------------------

K_mf(void) TCtrlSesAppia::OnSmpOperCmdResponse (TSmpMsgOperatorCommandResponse* smpMsg)
{
   const SSmpMsgOperatorCommandResponse& data = smpMsg->GetData();
   const TSmpOperCmd& operCmd = data.SmpOperCmd_;
   TRsReqResuHnd      rshRr(RsReqResuGoh_.GetPtr(), ReqID_);
   const SRsReqResu1* cpDat1 = rshRr.FindDat1();

   if (!cpDat1)
      return;
      
   //==============================================
   // 檢查 funcCode 和 smpCmd 是否一樣
   ETandFuncCode funcCode = cpDat1->FuncCode_.get_orig();
   ESmpCmd       smpCmd   = FuncCodeToSmpCmd(funcCode);

   if (smpCmd != operCmd.GetSmpCmd().get_orig())
      return;

   //==============================================
   // 檢查是否為全市場線路操作
   std::string strApaSesID = operCmd.GetParam1();

   if (strApaSesID == TSmpString("*")) {
      if (cpDat1->AllMarketFlag_.is_true()) {
         SesMgr_->SetAppGrpState(AppiaGrp_, TProcResu(epr_Finish));
         ReqID_.clear();
//         SRsReqResu1& udDat1 = rshRr.BeginUpdate1();
//         udDat1.ProcResu_.assignT(epr_Finish);
      }

      return;
   }

   //==============================================
   // 找到 SessionID
         TTwseSesIDApa apaSesID    (strApaSesID);
         std::string   strTandSesID(apaSesID.as_string_Tand());
         TFixSesID     fixSesID    (strTandSesID, true);
   const SRsReqResu2*  cpDat2 = rshRr.FindDat2(fixSesID);
   
   if (!cpDat2)
      return;

   ESmpOperCmdResu operCmdResu = (ESmpOperCmdResu)data.OperCmdResu_;   
   SRsReqResu2&    udDat2      = rshRr.BeginUpdate2();

   if (operCmdResu == socr_Success) {
      // 指令執行完後就發查詢的指令
      SendQuery(fixSesID);
      udDat2.ApaSentList_.set(asl_Query);
      udDat2.StatusCode1_.assignT(esc_Normal);
   }
   else if (operCmdResu == socr_UnknownSessioID) {
      udDat2.StatusCode1_.assignT(esc_FixSesIDError);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TCtrlSesAppia::OnSmpGetSessionInfoResponse (TSmpMsgGetSessionInfoResponse* smpMsg)
{
   const SSmpMsgGetSessionInfoResponse& data = smpMsg->GetData();
         std::string    strApaSesID (data.SessionID_);
         TTwseSesIDApa  apaSesID    (strApaSesID);
         std::string    strTandSesID(apaSesID.as_string_Tand());
         TFixSesID      fixSesID(strTandSesID, true);
         TRsReqResuHnd  rshRr(RsReqResuGoh_.GetPtr(), ReqID_, fixSesID);

   if (rshRr.FindDat2() == NULL)
      return;

   const SRsReqResu1* cpDat1 = rshRr.FindDat1();
         SRsReqResu2& udDat2 = rshRr.BeginUpdate2();

   // ConnectState
   switch (data.ConnectState_) {
      case scs_Disconnected:             udDat2.ConnState_.assignT(ecs_Disconnected);             break;
      case scs_SocketConnection:         udDat2.ConnState_.assignT(ecs_SocketConnection);         break;
      case scs_SentLogonConnection:      udDat2.ConnState_.assignT(ecs_SentLogonConnection);      break;
      case scs_ReceivedLogonConnection:  udDat2.ConnState_.assignT(ecs_ReceivedLogonConnection);  break;
      case scs_ApplicationConnection:    udDat2.ConnState_.assignT(ecs_ApplicationConnection);    break;
   }
   // ServiceDownFlag
   if (data.ServiceDownFlag_.is_true())  udDat2.ServiceDownFlag_.assignT(true);
   else                                  udDat2.ServiceDownFlag_.assignT(false);
   // SuspendFlag
   if (data.SuspendFlag_.is_true())  udDat2.SuspendFlag_.assignT(true);
   else                              udDat2.SuspendFlag_.assignT(false);
   // HoldFlag
   if (data.HoldFlag_.is_true())  udDat2.HoldFlag_.assignT(true);
   else                           udDat2.HoldFlag_.assignT(false);
   // InMsgSeqNum
   udDat2.InMsgSeqNum_.assign(data.InMsgSeqNum_.get_orig());
   // LastInMsgTime
   udDat2.LastInMsgTime_.assign(data.LastInMsgTime_);
   // OutMsgSeqNum
   udDat2.OutMsgSeqNum_.assign(data.OutMsgSeqNum_.get_orig());
   // LastOutMsgTime
   udDat2.LastOutMsgTime_.assign(data.LastOutMsgTime_);
   // StatusCode
   if (cpDat1->FuncCode_.get_orig() == tfc_StatusQuery)
      udDat2.StatusCode1_.assignT(esc_Normal);
   else
      udDat2.StatusCode2_.assignT(esc_Normal);
   // Text
   udDat2.Text_.clear();
}
//---------------------------------------------------------------------------

K_mf(void) TCtrlSesAppia::OnSmpReject (TSmpMsgReject* smpMsg)
{
   const SSmpMsgReject& data      = smpMsg->GetData();
   ESmpRejectReason     rejReason = data.RejReason_.get_orig();
   std::string          strApaSesID (data.RejParam1_);
   TTwseSesIDApa        apaSesID    (strApaSesID);
   std::string          strTandSesID(apaSesID.as_string_Tand());
   
   if (rejReason == srr_SesInfo_SesIDNotFound) {
      //===========================================
      // 訊息紀錄
      char logMsg[256];
      int  logLen = sprintf(logMsg, "<425> SesInfo.SesIDNotFound[%s], ReqID[%s]",
                                       strApaSesID.c_str(),
                                       ReqID_.as_string().c_str());
      inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);

      //===========================================
      // 開始處理
      TFixSesID      fixSesID(strTandSesID, true);
      TRsReqResuHnd  rshRr(RsReqResuGoh_.GetPtr(), ReqID_, fixSesID);

      if (rshRr.FindDat2() == NULL) {
         inherited::LogFileHnd_.WriteStr(elt_Info, "<436> dat2 == NULL, fixSesID[%s]", fixSesID.as_string().c_str());
         return;
      }

      SRsReqResu2& udDat2 = rshRr.BeginUpdate2();

      // StatusCode
      udDat2.StatusCode1_.assignT(esc_FixSesIDError);

      inherited::LogFileHnd_.WriteStr(elt_Info, "<445> assignT(esc_FixSesIDError)");
   }
}
//---------------------------------------------------------------------------

K_mf(void) TCtrlSesAppia::OnSmpMwEv_SessionEvents (TSmpMsgMiddlewareEvent* smpMsg)
{
   TExcMgrBase* excMgr = GetExcMgr();
   if (!excMgr)
      return;

   const SSmpMsgMiddlewareEvent& data        = smpMsg->GetData();
   const TSmpString&             strApaSesID = data.SessionID_;

   if (!CheckMarket(strApaSesID))
      return;

   // 要將 ApaSesID 轉成 TandSesID
   TTwseSesIDApa apaSesID(strApaSesID);
   std::string   strTandSesID = apaSesID.as_string_Tand();

   switch (data.EventType_) {
   case set_SessionConnected:      excMgr->Alert(TTwseExcCode::GetXCtrlCode(tec_FixSessionConnected),     strTandSesID);  break;
   case set_SessionDisconnected:   excMgr->Alert(TTwseExcCode::GetXCtrlCode(tec_FixSessionDisconnected),  strTandSesID);  break;
   case set_SessionEndOfDay:       excMgr->Alert(TTwseExcCode::GetXCtrlCode(tec_FixSessionEndOfDay),      strTandSesID);  break;
   case set_SessionHold:           excMgr->Alert(TTwseExcCode::GetXCtrlCode(tec_FixSessionHold),          strTandSesID);  break;
   case set_SessionSuspend:        excMgr->Alert(TTwseExcCode::GetXCtrlCode(tec_FixSessionSuspend),       strTandSesID);  break;
   case set_SessionResume:         excMgr->Alert(TTwseExcCode::GetXCtrlCode(tec_FixSessionResume),        strTandSesID);  break;
   case set_SessionServiceUp:      excMgr->Alert(TTwseExcCode::GetXCtrlCode(tec_FixSessionServiceUp),     strTandSesID);  break;
   case set_SessionServiceDown:    excMgr->Alert(TTwseExcCode::GetXCtrlCode(tec_FixSessionServiceDown),   strTandSesID);  break;
   case set_SessionStartEndOfDay:  excMgr->Alert(TTwseExcCode::GetXCtrlCode(tec_FixSessionStartEndOfDay), strTandSesID);  break;
   }
}
//---------------------------------------------------------------------------

K_mf(bool) TCtrlSesAppia::CheckMarket (const TSmpString& fixSesID)
{
   if (!MarketChar_ || fixSesID.empty())
      return false;

   return (fixSesID[0] == MarketChar_);
}
//---------------------------------------------------------------------------
K_mf(bool) TCtrlSesAppia::IsMyFIXSes(const std::string sesid)
{
   for(int i = 0; i < FIXSesIDs_.size(); i++)
      if(sesid == FIXSesIDs_[i])
         return true;
   return false;
}
//---------------------------------------------------------------------------
