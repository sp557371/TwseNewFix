/////////////////////////////////////////////////////////////////////////////
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TserSesApaSmp.h"
// libxsAux
#include "Nsa_UtilComm.h"
#include "Nsa_SeqNoGene.h"
#include "tplns/Nsa_EvMap_0File.h"
// libnsQFix
#include "src/Code/fix42/Logout.h"
// libxsExt
#include "ExtDef.h"
#include "MsgCenter.h"
// XTester
#include "TwseType.h"
#include "TserK02.h"
#include "TserIniFile.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kstl;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TTserSesApaSmp::TCliMsgID
/////////////////////////////////////////////////////////////////////////////
class TTserSesApaSmp::TCliMsgID
{
   //==============================================
   // TRecs Type
   typedef std::string  TKey;

   struct SRec
   {
      TCommInn::rr_key mutable  RoomKey_;
      
      TSmpString          ApaSesID_;
      ETwseFixRejectCode  RetCode_;
      
      K_mf(void) operator = (const SRec& a)
      {
         ApaSesID_ = a.ApaSesID_;
         RetCode_  = a.RetCode_;
      }
   };

   typedef ev_map<TKey, SRec>  TRecs;

   //==============================================
   // TRecsRecorder Type
   struct TStore
   {
      template <class TPersistent, class TStorage, class TKey, class TRec>
      K_mf(bool) exec (TPersistent& per, TStorage& storage, TKey& key, TRec& rec)
      {
         return storage.store(per, key)           &&
                storage.store(per, rec.ApaSesID_) &&
                storage.store(per, &rec.RetCode_, sizeof(rec.RetCode_));
      }
   };
   
   typedef TEvMapReco_v2<TRecs, SRec, TStore>  TRecsRecorder;

   TRecs          Recs_;
   TRecsRecorder  RecsRecorder_;

public:
   inline K_ctor TCliMsgID (const char* fileName)
      : RecsRecorder_(&Recs_, std::string(fileName))
   {}

   K_mf(std::string) GetNewID    (const TSmpString&, ETwseFixRejectCode);
   K_mf(bool)        IsValidCode (const std::string& key, const TSmpString& sesID);
};
/////////////////////////////////////////////////////////////////////////////
// class TTserSesApaSmp::TCliMsgID impl
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TTserSesApaSmp::TCliMsgID::GetNewID (const TSmpString& strApaSesID, ETwseFixRejectCode retCode)
{
   char buf[20];
   int  num = 1;
   TKey key;
   SRec rec;

   if (!Recs_.empty())
      num = atoi(Recs_.rbegin()->first.c_str()) + 1;

   sprintf(buf, "%06d", num);
   key.assign(buf);
   
   rec.ApaSesID_ = strApaSesID;
   rec.RetCode_  = retCode;
   
   Recs_.insert(TRecs::value_type(key, rec));
   return key;
}
//---------------------------------------------------------------------------

K_mf(bool) TTserSesApaSmp::TCliMsgID::IsValidCode (const std::string& key, const TSmpString& strApaSesID)
{
   TRecs::iterator iter = Recs_.find(key);
   if (iter != Recs_.end()) {
      if (iter->second.ApaSesID_ == strApaSesID && iter->second.RetCode_ == tfrc_TradeSuspended)
         return true;
   }
   return false;
}


/////////////////////////////////////////////////////////////////////////////
// class TTserSesApaSmp impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTserSesApaSmp::TTserSesApaSmp ()
   : inherited()
   , CliMsgID_(NULL)
{
   inherited::EnableRecover_ = false;

   //==============================================
   // 設定參數
   inherited::ExcCode_LinkReady_  = TTwseExcCode::GetXTestCode(tec_AppiaMwLinkReady);
   inherited::ExcCode_LinkBroken_ = TTwseExcCode::GetXTestCode(tec_AppiaMwLinkBroken);
   inherited::ExcCode_LinkFail_   = TTwseExcCode::GetXTestCode(tec_AppiaMwLinkFail);
   inherited::ExcCode_LinkFails_  = TTwseExcCode::GetXTestCode(tec_AppiaMwLinkFails);
}
//---------------------------------------------------------------------------

K_dtor TTserSesApaSmp::~TTserSesApaSmp ()
{
   if (CliMsgID_)  delete CliMsgID_;
}
//---------------------------------------------------------------------------

K_mf(bool) TTserSesApaSmp::VirBfSettingApply (std::string& str)
{
   //==============================================
   // 如果[處理程序參數]是空白的,就用程式名稱
   if (str.empty())
      str = GetProcName();

   inherited::MaxLinkFailCount_ = 2;
   inherited::LinkFailCount_    = 0;

   return true;
}
//---------------------------------------------------------------------------

K_mf(void) TTserSesApaSmp::OnMesLinkReady (TMesBase* mes)
{
   inherited::OnMesLinkReady(mes);

   char filePath[256];
   sprintf(filePath, "%sCliMsgID.bin", GetProcLogPath().c_str());
   CliMsgID_ = new TCliMsgID(filePath);
}
//---------------------------------------------------------------------------

K_mf(void) TTserSesApaSmp::OnMesLinkBroken (TMesBase* mes, const std::string& reason)
{
   inherited::OnMesLinkBroken(mes, reason);

   if (CliMsgID_) {
      delete CliMsgID_;
      CliMsgID_ = NULL;
   }
}
//---------------------------------------------------------------------------

K_mf(void) TTserSesApaSmp::OnMesLinkFail (TMesBase* mes, const std::string& reason)
{
   inherited::OnMesLinkFail(mes, reason);
}
//---------------------------------------------------------------------------

K_mf(std::string) TTserSesApaSmp::GetLogPath ()
{
   return GetProcLogPath();
}
//---------------------------------------------------------------------------

K_mf(std::string) TTserSesApaSmp::GetLogName ()
{
   // 預設是使用 MiddlewareID 當作檔名, 目前的 MiddlewareID 是使用 ProcName + HostName
   // 避免切換主機後開啟不ㄧ樣的 log
   // 所以使用 ProcName 當作檔名好了
   char logName[256];
   
   sprintf(logName, "SesApaSmp_%s", GetProcName().c_str());
   return std::string(logName);
}
//---------------------------------------------------------------------------

K_mf(void) TTserSesApaSmp::OnSmpLogon ()
{
   //==============================================
   // 詢問 appia 目前有那些 SessionID
   inherited::SmpGetMiddlewareInfoRequest();
}
//---------------------------------------------------------------------------

K_mf(void) TTserSesApaSmp::OnSmpGetMiddlewareInfoResponse (TSmpMsgGetMiddlewareInfoResponse* smpMsg)
{
   inherited::OnSmpGetMiddlewareInfoResponse(smpMsg);

   const SSmpMsgGetMiddlewareInfoResponse& data      = smpMsg->GetData();
   const SSmpMiddlewareConfig&             mwConf    = data.MiddlewareConfig_.GetData();
   const TSmpStringList&                   fixSesIDs = mwConf.SessionIDs_;

   //==============================================
   // 先查詢一下每個 SessionID 的狀態
   for (TSmpStringList::const_iterator iter=fixSesIDs.begin(); iter!=fixSesIDs.end(); ++iter) {
      inherited::SmpGetSessionInfoRequest(*iter);
   }

   //==============================================
   // 開始接收 Fix 資料
   inherited::SmpStartMiddlwareEvents();
}
//---------------------------------------------------------------------------

K_mf(void) TTserSesApaSmp::OnSmpGetSessionInfoResponse (TSmpMsgGetSessionInfoResponse* smpMsg)
{
   const SSmpMsgGetSessionInfoResponse& data = smpMsg->GetData();
         char baseMsg[128], logMsg[256];
         int  logLen;

   sprintf(baseMsg, "<236> SessionID[%s]", data.SessionID_.c_str());

   //==============================================
   // ConnectState 如果是 ReceivedLogonConnection
   // Client 端的連線會因為這樣而卡住,所以叫它重連過吧
   if (data.ConnectState_ == scs_ReceivedLogonConnection) {
      logLen = sprintf(logMsg, "%s ConnState[Received Logon], 發動斷線指令!", baseMsg);
      inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);

      TSmpOperCmd operCmd(esc_Disconnect, data.SessionID_);
      inherited::SendCmd(operCmd);
   }

   //==============================================
   // 如果發現狀態是 hold 時,要把它解除
   // 因為 appia 啟動時,有可能是全部為 hold
   if (data.HoldFlag_.is_true()) {
      logLen = sprintf(logMsg, "%s HoldFlag[Y], 發動指令解除!", baseMsg);
      inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);

      TSmpOperCmd operCmd(esc_Release, data.SessionID_);
      inherited::SendCmd(operCmd);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TTserSesApaSmp::OnSmpMwEv_OutboundAcknowledgmentEvents (TSmpMsgMiddlewareEvent* smpMsg)
{
   const SSmpMsgMiddlewareEvent& data = smpMsg->GetData();

   if (data.EventType_ == set_MessageSent) {
      if (CliMsgID_ && !data.ClientMsgID_.empty() && !data.SessionID_.empty()) {
         if (CliMsgID_->IsValidCode(data.ClientMsgID_, data.SessionID_)) {
            // 訊息已送出, 可以發出 service_down
            // 20130314 eric 必須判斷是否所有IP 都已測試完成才送 service down command
            TTserIni      iniTser;
            std::string SesID = TTwseSesIDApa(data.SessionID_).as_string_Tand();
            TStrings strs;
            int cnt = iniTser.ReadStrings("Summary","SesID",strs);
            for(int L = 0; L < cnt; L++)
               if(SesID == strs[L])
                  if(iniTser.IsTestOK(SesID)) //找到,且已測試完成, service_down
                  	break;
                  else                        //找到,但未測試完成, 離開
                     return;
            // 沒找到(設定已被刪除)時, 也要service down
            char logMsg[256];
            int  logLen;
            logLen = sprintf(logMsg, "MessageSent CliMsgID[%s] SessionID[%s], exec service_down",
                                 data.ClientMsgID_.c_str(), data.SessionID_.c_str());
            TUtilComm::ConsMsg(logMsg);
            inherited::LogFileHnd_.WriteMem(elt_Info, logMsg, logLen);
            TSmpOperCmd operCmd(esc_ServiceDown, data.SessionID_);
            inherited::SendCmd(operCmd);
         }
      }
   }
}
//---------------------------------------------------------------------------

K_mf(void) TTserSesApaSmp::OnFixLogon (const TSmpString& strApaSesID, TMessageRef msgRef, bool, TSmpMsgRef smpMsgRef)
{
   //==============================================
   // 先到 XTester.ini 裡面撈資料
   //==============================================
   // 到 TserIni 裡面取 Password 時要使用 Tandem 的 SessionID
   TTserIni      iniTser;
   TTwseSesIDApa apaSesID(strApaSesID);
   std::string   strTandSesID = apaSesID.as_string_Tand();
   std::string   strPwd       = iniTser.GetFixPassword(strTandSesID);
   
   if (strPwd.empty()) {
      char logMsg[256];
      int  logLen;
      
      logLen = sprintf(logMsg, "<302> 在 XTester.ini 中找不到 SessionID[%s] 的 Password", strTandSesID.c_str());
      inherited::LogFileHnd_.WriteMem(elt_Error, logMsg, logLen);
   
      TUtilComm::ConsMsg(logMsg);
      return;
   }
         
   int pwd = atoi(strPwd.c_str());

   //==============================================
   // 開始檢核
   const Message&     fixMsg = *msgRef;
   ETwseFixRejectCode ret    = CheckLogon(fixMsg, pwd);
   
   SendLogoutEcho(strApaSesID, fixMsg, ret);

   //==============================================
   // 寫入 K02
   if (/*ret == tfrc_KeyValueError || */ret == tfrc_TradeSuspended) {
      TSmpMsgMiddlewareEvent*       smpMsg = static_cast<TSmpMsgMiddlewareEvent*>(smpMsgRef.get());
      const SSmpMsgMiddlewareEvent& data   = smpMsg->GetData();
      int       fixVer = fixMsg.getVersion();
      if(MakeFullIPAddr(data.RemoteIP_) == MakeFullIPAddr(iniTser.GetSourceIP1(strTandSesID)))
         iniTser.SetIP1OK(strTandSesID);
      if(MakeFullIPAddr(data.RemoteIP_) == MakeFullIPAddr(iniTser.GetSourceIP2(strTandSesID)))
         iniTser.SetIP2OK(strTandSesID);
      if (iniTser.IsTestOK(strTandSesID)){
         TTserK02  k02;
         k02.AddRec(iniTser, strTandSesID, ret, pwd, data, fixVer);
      }
   }

   //==============================================
   // 紀錄執行訊息
   if (iniTser.IsTestOK(strTandSesID)) {
      // 線路測試成功
      // 1.移除 XTester.ini 資訊
      std::string appgrp = iniTser.GetAppGrp(strTandSesID);
      iniTser.RemoveSessionInfo(strTandSesID);
      
      // 2.移除 Appia Ini Session 資訊
      TAppiaIni iniAppia(iniTser.GetAppiaIniPath()+"."+appgrp);
      
      iniAppia.RemoveSessionInfo(strApaSesID, iniTser.GetAppiaMWName(appgrp));
      
      // 3.發出 service_down 指定
      // ==> 必須要等待訊息送出後才可以 service_down
      
      // 發送事件
      TExcMgrBase* excMgr = GetExcMgr();
      if (excMgr) {
         excMgr->Alert(TTwseExcCode::GetXTestCode(tec_FixSesTestOK), strTandSesID, std::string());
      }
   }
   else {
      // 發送事件
      TExcMgrBase* excMgr = GetExcMgr();
      if (excMgr) {
         if (ret == tfrc_TradeSuspended)
            excMgr->Alert(TTwseExcCode::GetXTestCode(tec_FixSesTestPOK), strTandSesID, std::string());
         else
            excMgr->Alert(TTwseExcCode::GetXTestCode(tec_FixSesTestFail), strTandSesID, std::string());
      }
   }
}
//---------------------------------------------------------------------------

K_mf(ETwseFixRejectCode) TTserSesApaSmp::CheckLogon (const Message& fixMsg, int pwd)
{
   //==============================================
   // RawData
   RawData qfRawData;

   if (!fixMsg.findField(qfRawData))
      return tfrc_RawDataLenNotEqual5;
      
   std::string rawData = qfRawData.getValue();
   if (rawData.length() != 5)
      return tfrc_RawDataLenNotEqual5;

   //==============================================
   // RawDataLength
   RawDataLength qfRawDataLength;

   if (!fixMsg.findField(qfRawDataLength))
      return tfrc_RawDataLenNotEqual5;

   if (qfRawDataLength.getValue() != 5)
      return tfrc_RawDataLenNotEqual5;

   //==============================================
   // 開始檢核
   char_ary<char, 3, ' '> appendNo;
   int                    nAppendNo;

   appendNo.assign(rawData);
   nAppendNo = appendNo.as_int();
   if (nAppendNo == 0)
      return tfrc_AppendNoEqual0;

   char        buf[20];
   std::string keyVal(rawData.substr(3));

   sprintf(buf, "%02d", ((nAppendNo * pwd / 100) % 100));
   if (strcmp(buf, keyVal.c_str()) != 0)
      return tfrc_KeyValueError;

   return tfrc_TradeSuspended;
}
//---------------------------------------------------------------------------
   
K_mf(void) TTserSesApaSmp::SendLogoutEcho (const TSmpString& strApaSesID, const Message& srcMsg, ETwseFixRejectCode rejCode)
{
   TMarket market;
   
   if (strApaSesID[0] == 'T')  market.assignT(m_Tse);
   else                        market.assignT(m_Otc);

   std::string   code   = TTwseRes::GetStsCodeByRejCode(rejCode, market);
   std::string   errMsg = GetErrMsg(code.c_str());
//   FIX42::Logout logout;
   FIX::Message  logout(FIX42::Logout::MsgType());
   Text          qfText(errMsg);
   TSmpString    msgData;

   logout.setField(qfText);
   logout.toString(msgData);

   TSmpString cliMsgID = CliMsgID_->GetNewID(strApaSesID, rejCode);

   inherited::SmpOutboundMessageDelivery(cliMsgID, strApaSesID, FIX42::Logout::MsgType().getValue(), msgData);
}
//---------------------------------------------------------------------------
K_mf(std::string) TTserSesApaSmp::MakeFullIPAddr (const std::string ipstr)
{
	Kway::byte ip[4];
   char*    Pos;
   char*    StartPos;
   int      i = 0;
   StartPos = (char*)ipstr.c_str();
   while((Pos = strchr(StartPos, '.'))){
     *Pos = 0;
     ip[i++] = atoi(StartPos);
     *Pos = '.';
     StartPos = Pos+1;
   }
   ip[i] = atoi(StartPos);
	char buf[16];
	sprintf(buf,"%03d.%03d.%03d.%03d",ip[0],ip[1],ip[2],ip[3]);
	return std::string(buf);
}
//---------------------------------------------------------------------------
