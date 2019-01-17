//===========================================================================
//
// main01.h.cpp
//
//===========================================================================
#pragma hdrstop
#include "main01.h"
#include "SysMgrObjs.h"
#include "KIniFile.h"
#pragma package(smart_init)
using namespace Kway;
//===========================================================================
K_ctor Main01::Main01(Kway::Signon::TSignonSvrDB* signon)
{
   fprintf(stderr, "XMonitor main startup!\n");
   char Buf[256]; 
   TFile  Log_;
   Log_.Open(std::string(GetProcLogPath()+"version.log").c_str(), (TFileMode)(fmRandomAccess|fmOpenAlways));
   Log_.Seek(Log_.GetFileSize(), fsBegin);
   sprintf(Buf,"%s XMonitor main program loaded, version: 1.0.0.1, last modified date 2009-09-11.",
           TKTime::Now().as_string().c_str());
   Log_.WriteLN(Buf);
   Log_.Close();
   TKIniFile IniFile(std::string(GetConfPath()+"/Main.ini").c_str());
   TStrings vacports;
   int cnt = IniFile.ReadStrings("Main", "XMonitorVACAddr",vacports);
   const TSysMgrObjs&  sys=GetSysMgrObjs();
   for(int L = 0; L < cnt; L++)
      if(!vacports[L].empty()){
         const char* pos = strchr(vacports[L].c_str(),':');
         if(pos)
            sys.SesMgr_.get()->AddSetting("SignonDB-Mgr",GetProgName(),"EvDevTcpCli",vacports[L]);
         else
            sys.SesMgr_.get()->AddSetting("SignonDB-Mgr",GetProgName(),"EvDevTcpSvr",vacports[L]);
      }
   ExcMgr_ = new TExcMgr(1,1);
   SetExcMgr(ExcMgr_);
   ExcMgr_->Alert("XM0000");
   ProcMon_ = new TProcMon();
   SetVA();
}
//---------------------------------------------------------------------------
K_dtor Main01::~Main01()
{
   const TSysMgrObjs& sys = GetSysMgrObjs();
   ProcMonVA_.Del(*ProcMon_);
   ProcMonVA_.Del(*ProcMon_->MonConf_);
   sys.MainVAMgr_->Del(ProcMonVA_);
   sys.MainVAMgr_->Del(*ExcMgr_->GetVAMgr());

   delete ProcMon_;
   if(ExcMgr_)
      ExcMgr_->Alert("XM9999");
   SetExcMgr(0);
   delete ExcMgr_;
   fprintf(stderr, "XMonitor closed!\n");
}
//---------------------------------------------------------------------------
K_mf(void) Main01::SetVA()
{
   const TSysMgrObjs&  sys=GetSysMgrObjs();
   sys.MainVAMgr_->Add("2.事件管理員",*ExcMgr_->GetVAMgr());
   ProcMonVA_.Add("1.XTwse 程序清單",*ProcMon_);
   ProcMonVA_.Add("2.程序監管設定",*ProcMon_->MonConf_);
   sys.MainVAMgr_->Add("3.程序監管員",ProcMonVA_);
}
//---------------------------------------------------------------------------
