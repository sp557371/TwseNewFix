//===========================================================================
//
// main01.h.cpp
//
//===========================================================================
#pragma hdrstop
#include "main01.h"
#include "SysMgrObjs.h"
//#include "TwseFilesHandler.h"
#pragma package(smart_init)
using namespace Kway;
using namespace Kway::Tw::Fix::Stk;
//===========================================================================
K_ctor Main01::Main01(Kway::Signon::TSignonSvrDB* signon)
{
   fprintf(stderr, "Main01 startup!\n");
   /*
   TFile  Log_;
   char Buf[128]; 
   sprintf(Buf,"%s/version.log",Path.c_str());
   Log_.Open("./Logs/%4y%2m%2d/version.log", (TFileMode)(fmRandomAccess|fmOpenAlways));
   Log_.Seek(Log_.GetFileSize(), fsBegin);
   sprintf(Buf,"%s XTwse main program loaded, version: 1.0.1.1, last modified date 2008-12-10.",
           TKTime::Now().as_string().c_str());
   Log_.WriteLN(Buf);
   Log_.Close();
   */
   //OrdSesMgr_  =new TOrdSesMgr();//(SysInfo_);
   //OrdSesMgr_->Init(signon);//(OrdFlow_,OrdCenter_, signon);
   FixSesMgr_ = &Kway::Tw::Fix::Stk::GetFixSesMgr();
   FixSesMgr_->Init(signon);
   //FixSesMgr_->StartSesMgr();
   SetVA();
   
   //Kway::Tw::Bx::FT::TwseFilesHandler  fileH = new Kway::Tw::Bx::FT::TwseFilesHandler();
}
//---------------------------------------------------------------------------
K_dtor Main01::~Main01()
{
   //delete OrdSesMgr_;
   fprintf(stderr, "假開盤FIX委託工具 closed!\n");
}
//========================================================================
K_mf(std::string) Main01::DateIntoPath(char* DirName,bool head)
{
   char DatePath[128];
   sprintf(DatePath,"./Logs/%s/%s/",TKDate::Now().as_string(TKDate::sf_ELong,0).c_str(),DirName);
   return std::string(DatePath);
}
//========================================================================
K_mf(void) Main01::SetVA()
{
   const TSysMgrObjs&  sys=GetSysMgrObjs();
   sys.SysVAMgr_->Add("FIX委託連線管理員", *FixSesMgr_->GetVAMgr());
}
//========================================================================
//========================================================================
