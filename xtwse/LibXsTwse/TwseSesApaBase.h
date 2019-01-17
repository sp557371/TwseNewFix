/////////////////////////////////////////////////////////////////////////////
#if !defined(_TwseSesApaBase_h_)
#define _TwseSesApaBase_h_
//---------------------------------------------------------------------------
#include "ExcMgrBase.h"
#include "TwseIniFile.h"
#include "TwseExcCode.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {

/////////////////////////////////////////////////////////////////////////////
// class TTwseSesApaBase
/////////////////////////////////////////////////////////////////////////////
template <class ApaSesT>
class TTwseSesApaBase : public ApaSesT
{
protected:
   typedef ApaSesT  inherited;
   typedef typename inherited::SApaMwInfo  SApaMwInfo;

protected:
   size_t    LinkFailCount_;
   size_t    MaxLinkFailCount_;
   TErrCode  ExcCode_LinkReady_;
   TErrCode  ExcCode_LinkBroken_;
   TErrCode  ExcCode_LinkFail_;
   TErrCode  ExcCode_LinkFails_;
   
   // 連線因不明原因中斷,重連的秒數
   int  AfBrokenItvl_;

protected:
   K_ctor TTwseSesApaBase ();

   //==============================================
   // overwrite TSesBase virtual function
   K_mf(void) OnMesLinkReady  (TMesBase*);
   K_mf(void) OnMesLinkBroken (TMesBase*, const std::string&);
   K_mf(void) OnMesLinkFail   (TMesBase*, const std::string&);

   //==============================================
   // overwrite TApaSesSmpBase virtual function
   K_mf(void) VirAfSettingApply (const std::string&);
};
/////////////////////////////////////////////////////////////////////////////
// class TTwseSesApaBase impl
/////////////////////////////////////////////////////////////////////////////
#define TTwseSesApaBase_Impl(return_type)          \
   template <class ApaSesT>                        \
   K_mf(return_type) TTwseSesApaBase<ApaSesT>::    \
//---------------------------------------------------------------------------

template <class ApaSesT>
K_ctor TTwseSesApaBase<ApaSesT>::TTwseSesApaBase ()
   : inherited        ()
   , LinkFailCount_   (0)
   , MaxLinkFailCount_(2)
{
}
//---------------------------------------------------------------------------

TTwseSesApaBase_Impl(void) OnMesLinkReady (TMesBase* mes)
{
   inherited::OnMesLinkReady(mes);

   std::string uid = mes->GetUID();
 
   LinkFailCount_ = 0;
   
   TUtilComm::ConsMsg("AppiaMW Session Link Ready! (%s)(%s)", 
                        inherited::Conf_.MiddlewareID_.c_str(), 
                        uid                           .c_str());

   TExcMgrBase* excMgr = GetExcMgr();
   if (excMgr)
      excMgr->Alert(ExcCode_LinkReady_, TTwseExcCode::LinkState, uid);
}
//---------------------------------------------------------------------------

TTwseSesApaBase_Impl(void) OnMesLinkBroken (TMesBase* mes, const std::string& reason)
{
   inherited::OnMesLinkBroken(mes, reason);

   TUtilComm::ConsMsg("AppiaMW Session Link Broken! (%s)", reason.c_str());

   TExcMgrBase* excMgr = GetExcMgr();
   if (excMgr)
      excMgr->Alert(ExcCode_LinkBroken_, TTwseExcCode::LinkState, reason);

   // 如果是連線中斷的情形,馬上在進行連線
   if (inherited::ReopenTimer_.get() == NULL) {
      inherited::Reopen(std::string(), AfBrokenItvl_);
   }
}
//---------------------------------------------------------------------------

TTwseSesApaBase_Impl(void) OnMesLinkFail (TMesBase* mes, const std::string& reason)
{
   inherited::OnMesLinkFail(mes, reason);

   std::string setting = mes->GetSetting();
   
   ++LinkFailCount_;

   TUtilComm::ConsMsg("AppiaMW Session Link Fail! (%s)(%s)",
                           setting.c_str(),
                           reason .c_str());

   TExcMgrBase* excMgr = GetExcMgr();
   if (excMgr) {
      if (LinkFailCount_ < MaxLinkFailCount_)
         excMgr->Alert(ExcCode_LinkFail_, TTwseExcCode::LinkState, setting);
      else if (LinkFailCount_ == MaxLinkFailCount_)
         excMgr->Alert(ExcCode_LinkFails_, TTwseExcCode::LinkState, setting);
   }
}
//---------------------------------------------------------------------------

TTwseSesApaBase_Impl(void) VirAfSettingApply (const std::string& str)
{
   inherited::VirAfSettingApply(str);

   const TTwseMainIni::SCont& iniMainCont = TTwseMainIni::GetCont();
         SApaMwInfo&          apaMwInfo   = inherited::ApaMwInfo_;

   // 設定 LogonTimeout
   apaMwInfo.LogonInterval_ = iniMainCont.Main_.AppiaMwSesLogonItvl_ * 1000;

   // 設定 Heartbeat
   apaMwInfo.SetHbItvl(iniMainCont.Main_.AppiaMwSesBfHbItvl_, iniMainCont.Main_.AppiaMwSesAfHbItvl_);
   
   // 因某種原因自動斷線後,重連的秒數
   apaMwInfo.ReopenItvl_ = iniMainCont.Main_.AppiaMwSesReopenItvl_;

   // 設定[多次連不上時要發送訊息]的次數
   MaxLinkFailCount_ = iniMainCont.Main_.TmpFailCount_;
   // 連線因不明原因中斷,重連的秒數
   AfBrokenItvl_     = iniMainCont.Main_.AppiaMwSesAfBrokenItvl_;

   //==============================================
   // 紀錄訊息
   TFileWriterHnd& logFile = inherited::LogFileHnd_;
   char            logMsg[512];
   int             logLen;

   logLen = sprintf(logMsg, "LogonItvl:    %d", apaMwInfo.LogonInterval_);  logFile.WriteMem(elt_Info, logMsg, logLen);
   logLen = sprintf(logMsg, "ItvlBfSendHb: %d", apaMwInfo.ItvlBfSendHb_);   logFile.WriteMem(elt_Info, logMsg, logLen);
   logLen = sprintf(logMsg, "ItvlAfSendHb: %d", apaMwInfo.ItvlAfSendHb_);   logFile.WriteMem(elt_Info, logMsg, logLen);
   logLen = sprintf(logMsg, "ReopenItvl:   %d", apaMwInfo.ReopenItvl_);     logFile.WriteMem(elt_Info, logMsg, logLen);

   logLen = sprintf(logMsg, "MaxLinkFailCount: %d", MaxLinkFailCount_);  logFile.WriteMem(elt_Info, logMsg, logLen);
   logLen = sprintf(logMsg, "AfBrokenItvl:     %d", AfBrokenItvl_);      logFile.WriteMem(elt_Info, logMsg, logLen);
}

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseSesApaBase_h_)
