/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "MesRegs.h"
#include "ExtDef.h"
#include "ExcMgrBase.h"
#include "OrdSesMgr.h"
#include "SysMgrObjs.h"
#include "TwseType.h"
#include "TwseSesApaSmp.h"
#include "TwseExcCode.h"
#include "TwseIniFile.h"
#include "TwseFstAdpt.h"
//---------------------------------------------------------------------------
static Kway::TSesBase* TwseSesApaSmpCreater (const Kway::TSesReg&, Kway::TSesMgr& sesMgr)
{
   TOrdSesMgr&              ordSesMgr = static_cast<TOrdSesMgr&>(sesMgr);
   Kway::Tw::Stk::TOrdFlow* of        = ordSesMgr.GetOrdFlow();

   return new TTwseSesApaSmp(of);
}
//---------------------------------------------------------------------------
static const Kway::TSesReg SesRegs[] = {
   { "TWSE Appia Socket", TwseSesApaSmpCreater },
};


/////////////////////////////////////////////////////////////////////////////
// class TOrdSesMgr impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TOrdSesMgr::TOrdSesMgr (/*TSysInfoPage* sysinfo, */TOrdFlow* of, TSignonSvrDB* sdb)
   : inherited(std::string(GetProcLogPath()+"/ModifyLog/OrdSesMgr.log").c_str())
   , OrdFlow_  (of)
   , OrdCenter_(&(of->GetOrdCenter()))
   , SignonDB_ (sdb)
{
//   SysInfo_ = sysinfo;
   const TSysMgrObjs&   sys = GetSysMgrObjs();
   Reg(Kway::TMesRegs::GetDefaultMesRegs());
   Reg(SesRegs, SesRegs+numofele(SesRegs));
   OrdVA_.Add(std::string("1.MW連線狀態"),  GetStatusVACreater());
   OrdVA_.Add(std::string("2.MW連線設定"),  GetSettingVACreater());
   sys.SysVAMgr_->Add(std::string("MW連線管理員"), OrdVA_);
   // 是否是 DR
   TTwseDrIni iniDr;
   IsDR_ = iniDr.IsDR();

   Timer_ = new TOTimer(this);
   Timer_->Start(1000, &this_type::OnTimer);  // 每10秒檢查一次
}
//---------------------------------------------------------------------------
// 在啟動 Appia 連線的檢查.一般環境
K_mf(bool) TOrdSesMgr::PreChkNormal ()
{
   TExcMgrBase* excMgr = GetExcMgr();

   //==============================================
   // 等待任何一條 TmpSes 連上線
   if (IsTMPReady() == false) {
      // 發送事件
      if (excMgr)
         excMgr->Alert(TTwseExcCode::GetXTwseCode(tec_AppiaMwWaitTmpSesReady), TTwseExcCode::LinkState);

      TUtilComm::ConsMsg("TMPSes 登入作業尚未完成, 等待中");
      return false;
   }

   //==============================================
   // 判斷現在是否有在等待 TmpSes 末筆查詢作業都完成的狀態
   FIX::Stk::TTradeMgr& tradeMgr = FIX::Stk::TTradeMgr::GetInst();

   if (tradeMgr.GetUnitSize() > 0) {
      FIX::Stk::TTradeUnitPtr unitPtr = tradeMgr.GetFirstUnit();
      FIX::Stk::TTradeUnit*   unit    = unitPtr.get();

      if (unit) {
         TTwseFstAdpt* adptTwse = static_cast<TTwseFstAdpt*>(unit->GetFixTradeAdpt());

         if (adptTwse->IsReady() == false) {
            // 發送事件
            if (excMgr)
               excMgr->Alert(TTwseExcCode::GetXTwseCode(tec_AppiaMwWaitTmpSesReadyAll), TTwseExcCode::LinkState);

            TUtilComm::ConsMsg("TMPSes 登入作業尚未全部完成, 等待中");
            return false;
         }
      }
   }

   return true;
}
//---------------------------------------------------------------------------
// 在啟動 Appia 連線的檢查.DR環境
K_mf(bool) TOrdSesMgr::PreChkDR ()
{
   if (!IsDRTMPReady()) {
      TUtilComm::ConsMsg("DR 端委託讀檔作業尚未完成,等待中!");
      return false;
   }
   return true;
}
//---------------------------------------------------------------------------
K_mf(void) TOrdSesMgr::OnTimer(TTimerBase*)
{
   if (!IsDR_) {
      if (!PreChkNormal())
         return;
   }
   else {
      if (!PreChkDR())
         return;
   }

   Timer_->Stop();

   // cooper { 改用自行新增的方式
   //ReloadSetting(std::string(GetProcConfPath()+"OrdSes.ini"),true);
   TTwseMainIni iniMain;
   std::string  sesName     ("TWSE Appia Socket");
   std::string  mesName     ("EvDevTcpCli");
   std::string  strTandSesID(GetProcName());
   std::string  sesSetting;
   char         buf[512], hostName[256];

   //==============================================
   // sesSetting, 使用 TandSesID_HostName 當作 MiddlewareID
   if (gethostname(hostName, sizeof(hostName)) != 0)
      hostName[0] = '\0';

   sprintf(buf, "%s_%s", strTandSesID.c_str(), hostName);
   sesSetting.assign(buf);

   //==============================================
   // mesSetting
   // M0 => 不合併
   // R? => RetrySeconds
   sprintf(buf, "%s:%s,M0R%d", iniMain.GetAppiaSvrIP         ().c_str(),
                               iniMain.GetAppiaSvrPort       ().c_str(),
                               iniMain.GetSocketRetryInterval());

   inherited::AddSetting(sesName, sesSetting, mesName, std::string(buf));
   // } cooper

   delete Timer_;

   //Kway::ErrLog("<TOrdSesMgr::OnTimer> leave");
}
//---------------------------------------------------------------------------
