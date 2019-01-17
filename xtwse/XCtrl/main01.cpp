//===========================================================================
//
// main01.h.cpp
//
//===========================================================================
#pragma hdrstop
#include "main01.h"
#include "SysMgrObjs.h"
#include "KIniFile.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
using namespace Kway;
//===========================================================================
K_ctor Main01::Main01(Kway::Signon::TSignonSvrDB* signon)
{
   fprintf(stderr, "XCtrl main startup!\n");
   ExcMgr_ = new TExcMgr(1,1,0);
   SetExcMgr(ExcMgr_);
   ExcMgr_->Alert("XC0000");
   MsgC_ = new TMsgTable(true); // 錯誤訊息對照表
   SetMsgCenter(MsgC_);
   char Buf[256]; 
   TFile  Log_;
   Log_.Open(std::string(GetProcLogPath()+"version.log").c_str(), (TFileMode)(fmRandomAccess|fmOpenAlways));
   Log_.Seek(Log_.GetFileSize(), fsBegin);
   sprintf(Buf,"%s XCtrl main program loaded, version: 2.0.0.1, last modified date 2013-02-01.",
           TKTime::Now().as_string().c_str());
   Log_.WriteLN(Buf);
   Log_.Close();

   // cooper { 減少讀取 Main.ini 的次數
   MainIni_ = new TTwseMainIni;
   MainIni_->InitCont();
   // } cooper

   BrkConfPage_ = new Kway::Twse::TBrkConfPage();
   SetVA();

   // cooper
   if (MainIni_) {
      delete MainIni_;
      MainIni_ = NULL;
   }
}
//---------------------------------------------------------------------------
K_dtor Main01::~Main01()
{
   const TSysMgrObjs& sys = GetSysMgrObjs();
   sys.MainVAMgr_->Del(SysSettingVA_);
   SysSettingVA_.Del(*MsgC_->GetVA());
   SysSettingVA_.Del(*BrkConfPage_->GetVA());
   SysSettingVA_.Del(*BrkConfPage_->GetErrCntVA());
   sys.MainVAMgr_->Del(*ExcMgr_->GetVAMgr());

   SetMsgCenter((TMsgTable*)0);
   delete MsgC_;
   if(ExcMgr_)
      ExcMgr_->Alert("XC9999");
   SetExcMgr(0);
   delete ExcMgr_;
   delete BrkConfPage_;
   fprintf(stderr, "XCtrl closed!\n");
}
//---------------------------------------------------------------------------
K_mf(void) Main01::SetVA()
{
   const TSysMgrObjs&  sys=GetSysMgrObjs();
   sys.MainVAMgr_->Add("2.事件管理員",*ExcMgr_->GetVAMgr());
   SysSettingVA_.Add("錯誤碼對照表",*MsgC_->GetVA());
   SysSettingVA_.Add("TMP Handler 設定",*BrkConfPage_->GetVA());
   SysSettingVA_.Add("TMP 錯誤次數管理",*BrkConfPage_->GetErrCntVA());
   sys.MainVAMgr_->Add("3.系統設定",SysSettingVA_);
   
   TStrings vacports;
   // cooper {
   //TKIniFile IniFile(std::string(GetConfPath()+"/Main.ini").c_str());
   //int cnt = IniFile.ReadStrings("Main", "XCtrlVACAddr",vacports);
   int cnt = MainIni_->GetXCtrlVACAddr(vacports);
   // } cooper
   
   for(int L = 0; L < cnt; L++)
      if(!vacports[L].empty()){
         const char* pos = strchr(vacports[L].c_str(),':');
         if(pos)
            sys.SesMgr_.get()->AddSetting("SignonDB-Mgr",GetProgName(),"EvDevTcpCli",vacports[L]);
         else
            sys.SesMgr_.get()->AddSetting("SignonDB-Mgr",GetProgName(),"EvDevTcpSvr",vacports[L]);
      }

}
//---------------------------------------------------------------------------
