/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "MesRegs.h"
#include "OrdSesMgr.h"
#include "SysMgrObjs.h"
//#include "OmsSes.h"
//#include "DispSes.h"
#include "TserSesApaSmp.h"
#include "TserIniFile.h"
#include "ExtDef.h"
/////////////////////////////////////////////////////////////////////////////
// Session Creater
using namespace Kway;
/////////////////////////////////////////////////////////////////////////////
static Kway::TSesBase* TserSesApaSmpCreater (const Kway::TSesReg&, Kway::TSesMgr&)
{
   return new Kway::TTserSesApaSmp;
}
//---------------------------------------------------------------------------
/*
static Kway::TSesBase* OmsSesCreater (const Kway::TSesReg&, Kway::TSesMgr& SesMgr)
{
   return new TOmsSes(&SesMgr);
}
//---------------------------------------------------------------------------
static Kway::TSesBase* DispSesCreater (const Kway::TSesReg&, Kway::TSesMgr& SesMgr)
{
   return new TDispSes(&SesMgr);
}
*/
//---------------------------------------------------------------------------
static const Kway::TSesReg SesRegs[] = {
   { "Tester Appia Socket", TserSesApaSmpCreater },
//   { "OmsSes",            OmsSesCreater        },
//   { "DispSes",           DispSesCreater       },
};


/////////////////////////////////////////////////////////////////////////////
// class TOrdSesMgr impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TOrdSesMgr::TOrdSesMgr (TSignonSvrDB* sdb)
   : inherited(std::string(GetProcLogPath()+"/ModifyLog/OrdSesMgr.log").c_str())
   , SignonDB_ (sdb)
{
   const TSysMgrObjs&   sys = GetSysMgrObjs();
   Reg(Kway::TMesRegs::GetDefaultMesRegs());
   Reg(SesRegs, SesRegs+numofele(SesRegs));
   OrdVA_.Add(std::string("1.連線狀態"),  GetStatusVACreater());
   OrdVA_.Add(std::string("2.連線設定"),  GetSettingVACreater());

   sys.SysVAMgr_->Add(std::string("MW連線管理員"), OrdVA_);
   Timer_ = new TOTimer(this);
   // cooper
   //Timer_->Start(1000*10, &this_type::OnTimer);  // 每10秒檢查一次
   Timer_->Start(1000, &this_type::OnTimer);  // 每10秒檢查一次
}
//---------------------------------------------------------------------------
K_mf(void) TOrdSesMgr::OnTimer(TTimerBase*)
{
   Timer_->Stop();
   
   // cooper { 改用自行新增的方式
   //ReloadSetting(std::string(GetProcConfPath()+"OrdSes.ini"),true);
   std::string  sesName  ("Tester Appia Socket");
   std::string  mesName  ("EvDevTcpCli");
   char         buf[512], hostName[256];
   if (gethostname(hostName, sizeof(hostName)) != 0)
      hostName[0] = '\0';

   std::string  apaMwName;
   std::string  sesSetting;
   std::string  mesSetting;
   TKIniFile IniFile(std::string(GetConfPath()+"/XTester.ini").c_str());
   TStrings strs;
   int cnt = IniFile.ReadStrings("Summary","AppiaGroup",strs);
   for(int L = 0; L < cnt; L++){
      apaMwName =IniFile.ReadString(strs[L],"AppiaMWName","APP01");
      mesSetting=IniFile.ReadString(strs[L],"AppiaSvrAddr","127.0.0.1:9108");
   // sesSetting, 使用 AppiaGroup_ApaMwName_HostName 當作 MiddlewareID, Ex: APP01_XTester_fixtmp1t
      sprintf(buf, "%s_%s_%s", strs[L].c_str(),apaMwName.c_str(), hostName);
      sesSetting.assign(buf);
      inherited::AddSetting(sesName, sesSetting, mesName, mesSetting);
   }
   delete Timer_;
}
//---------------------------------------------------------------------------
