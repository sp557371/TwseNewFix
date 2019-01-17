//===========================================================================
//
// main01.h.cpp
//
//===========================================================================
#pragma hdrstop
#include "main01.h"
#include "SysMgrObjs.h"
#include "KIniFile.h"
#include "TserIniFile.h"
#include "TserK02.h"
#include "TwseType.h"
#pragma package(smart_init)
using namespace Kway;
//===========================================================================
K_ctor Main01::Main01(Kway::Signon::TSignonSvrDB* signon)
{
   fprintf(stderr, "Main01 startup!\n");
   ExcMgr_ = new TExcMgr(1,1);
   SetExcMgr(ExcMgr_);
   MsgC_ = new TMsgTable(false); // 錯誤訊息對照表
   SetMsgCenter(MsgC_);
   ExcMgr_->Alert("XT0000");
   char Buf[256]; 
   TFile  Log_;
   Log_.Open(std::string(GetProcLogPath()+"version.log").c_str(), (TFileMode)(fmRandomAccess|fmOpenAlways));
   Log_.Seek(Log_.GetFileSize(), fsBegin);
   sprintf(Buf,"%s XTester main program loaded, version: 2.0.0.1, last modified date 2013-02-01.",
           TKTime::Now().as_string().c_str());
   Log_.WriteLN(Buf);
   Log_.Close();
   Log_.Open(std::string(GetProcLogPath()+"F02").c_str(), (TFileMode)(fmRandomAccess|fmOpenAlways));
   if(Log_.GetFileSize() == 0){
      sprintf(Buf,"%-*s",sizeof(STserK02Data2),TKDate::Now().as_string(TKDate::sf_ELong,0).c_str());
      Log_.Write(Buf);
   }
   Log_.Close();

   // cooper { 減少讀取 Main.ini 的次數
   MainIni_ = new TTwseMainIni;
   MainIni_->InitCont();
   // } cooper

// 在載入線路資訊之前, 要先刪除己過期的線路設定
   TKIniFile IniFile(std::string(GetConfPath()+"/XTester.ini").c_str());
   TStrings sess;
   int cnt = IniFile.ReadStrings("Summary", "SesID",sess);
   int duration = IniFile.ReadInt("Summary", "Expired",60);
   TKDate CurDate = TKDate::Now();
   TKDate ApplyDate;
   for(int L = 0; L < cnt; L++) {
      if(!sess[L].empty()){
         ApplyDate.assign(IniFile.ReadString(sess[L], "ApplyDate","20100101"));
         if((CurDate - ApplyDate) > duration){
            std::string tmp = IniFile.ReadString(sess[L],"Type","");
            if(tmp[0]=='N'){ // FIX 線路
               // 移除 Appia Ini Session 資訊
               tmp = IniFile.ReadString(sess[L],"AppiaGroup","");
               IniFile.EraseValue(std::string("Summary"), std::string("SesID"), sess[L]);
               IniFile.EraseSection(sess[L]);

               std::string appiaIniPath = IniFile.ReadString(std::string("Summary"), std::string("AppiaIniPath"), std::string());
               std::string appiaMwName  = IniFile.ReadString(tmp, std::string("AppiaMWName"),  std::string());
               TAppiaIni   iniAppia(std::string(appiaIniPath+"."+tmp));
               TTwseSesIDTand apases(sess[L]);
               iniAppia.RemoveSessionInfo(apases.as_string_Apa(), appiaMwName);
               ExcMgr_->Alert("XT1101",sess[L],std::string("ApplyDate:"+ApplyDate.as_string(TKDate::sf_ELong)));
            }
         }
      }
   }
// 20130131 已不需測試TMP線路了
//   bxSesMgr_   = new Kway::Tw::bx::TbxSesMgr(0, 0, true);
   OrdSesMgr_  = new TOrdSesMgr(signon);
//   SysSettingVA_.Add(std::string("0.系統狀態"),*SysInfo_);
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
   sys.SysVAMgr_->Del(*ExcMgr_->GetVAMgr());
//   Scheduler_->SetSender((TSenderBase*)0);
//   delete bxSesMgr_;
   delete OrdSesMgr_;
   SetMsgCenter((TMsgTable*)0);
   delete MsgC_;
   if(ExcMgr_)
      ExcMgr_->Alert("XT9999");
   SetExcMgr(0);
   delete ExcMgr_;
   fprintf(stderr, "XTester closed!\n");
}
//---------------------------------------------------------------------------
K_mf(void) Main01::SetVA()
{
   const TSysMgrObjs&  sys=GetSysMgrObjs();
   sys.SysVAMgr_->Add("事件管理員",*ExcMgr_->GetVAMgr());
   
   TStrings vacips;
   // cooper {
   //TKIniFile KIniFile(std::string(GetConfPath()+"/Main.ini").c_str());
   //int cnt = KIniFile.ReadStrings("Main", "XTesterVACAddr",vacips);
   int cnt = MainIni_->GetXTesterVACAddr(vacips);
   // } cooper

   for(int L = 0; L < cnt; L++) {
      if(!vacips[L].empty()){
         const char* pos = strchr(vacips[L].c_str(),':');
         if(pos)
            sys.SesMgr_.get()->AddSetting("SignonDB-Mgr",GetProgName(),"EvDevTcpCli",vacips[L]);
         else
            sys.SesMgr_.get()->AddSetting("SignonDB-Mgr",GetProgName(),"EvDevTcpSvr",vacips[L]);
      }
   }
}
//---------------------------------------------------------------------------
