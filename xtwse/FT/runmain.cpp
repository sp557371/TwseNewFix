#include "runmain.h"
//#include "FileLog.h"
using namespace Kway;
#include "SyncEv.h"
#include "ExtFuncs.h"
#include "ExcMgrBase.h"
#include "KIniFile.h"
#include "FTExtDef.h"
using namespace Kway::Tw::Bx;
using namespace Kway::Tw::Bx::FT;
//---------------------------------------------------------------------------
K_ctor TRunMain::TRunMain()
{
   TSyncObj::CheckSync();
   InitExtFuncs(::_argc, ::_argv);
   TFile  Log_;
   char Buf[512];
   std::string fname = GetProcLogPath()+"/version.log";
   Log_.Open(fname.c_str(), (TFileMode)(fmRandomAccess|fmOpenAlways));
   Log_.Seek(Log_.GetFileSize(), fsBegin);
   sprintf(Buf,"%s XFT main program loaded, version: 1.0.4.2, last modified date 2009-11-22.",
           TKTime::Now().as_string().c_str());
   Log_.WriteLN(Buf);
   Log_.Close();
   ExcMgr_ = new TExcMgr(1,1);
   SetExcMgr(ExcMgr_);
   if(GetExcMgr())
      GetExcMgr()->Alert("XF0000","Startup");
   GetSysMgrObjs();
   const TSysMgrObjs&  sys=GetSysMgrObjs();
   sys.SysVAMgr_->Add("事件管理員",*ExcMgr_->GetVAMgr());
   MsgC_ = new TMsgTable(false); // 錯誤訊息對照表
   SetMsgCenter(MsgC_);
//   sys.SysVAMgr_->Add("錯誤碼對照表",*MsgC_->GetVA());
   GetCenter();
   GetSchedule();
   GetSysSetting();
   TbxFTSesMgr* bxmgr = GetBxMgr();
   GetAPI();
   GetSender();
   GetDispatchMgr((TFTSenderBase*)bxmgr);
//   SetFTSender((TFTSenderBase*)GetBxMgr());
   TKIniFile IniFile(std::string(GetConfPath()+"/Main.ini").c_str());
   TStrings vacports;
   int cnt = IniFile.ReadStrings("Main", "XFTVACAddr",vacports);
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
K_dtor TRunMain::~TRunMain()
{
   SetMsgCenter((TMsgTable*)0);
   const TSysMgrObjs&  sys=GetSysMgrObjs();
   sys.SysVAMgr_->Del(*ExcMgr_->GetVAMgr());
//   sys.SysVAMgr_->Del(*MsgC_->GetVA());
   if(GetExcMgr())
      GetExcMgr()->Alert("XF9999","Shutdown");
   SetExcMgr(0);
   delete ExcMgr_;
   delete MsgC_;
}
//---------------------------------------------------------------------------

