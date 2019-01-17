//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FTRModule.h"
#include "bxSesMgr.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw::Bx::FT;
//---------------------------------------------------------------------------
K_ctor TRModule::TRModule(void)
{
   Timer_ = new TRTimer(this);
   Timer_->Start(2*1000, &this_type::OnTimer);  // 每2秒檢查一次
}
//---------------------------------------------------------------------------
K_dtor TRModule::~TRModule()
{
   Timer_->Stop();
   delete Timer_;
   if (RSesMgr_){
      TbxFTSesMgr* sb = GetBxMgr();
      if(sb)
         sb->SetFTrm((TFTRBase*)0);
      delete RSesMgr_;
   }
}
//---------------------------------------------------------------------------
K_mf(void) TRModule::OnTimer(TTimerBase*)
{
   TbxFTSesMgr* sb = GetBxMgr();
   if(sb){
      TFile  Log_;
      char Buf[128];
      sprintf(Buf,"./Logs/%s/version.log",TKDate::Now().as_string(TKDate::sf_ELong,0).c_str());
      Log_.Open("./Logs/%4y%2m%2d/version.log", (TFileMode)(fmRandomAccess|fmOpenAlways));
      Log_.Seek(Log_.GetFileSize(), fsBegin);
      sprintf(Buf,"%s libFTrm loaded, version: 2.0.0.1, last modified date 2009-09-01.",
         TKTime::Now().as_string().c_str());
      Log_.WriteLN(Buf);
      Log_.Close();
      Timer_->Stop();
      RSesMgr_  = new TRSesMgr();
      sb->SetFTrm((TFTRBase*)RSesMgr_);
   }
}
//---------------------------------------------------------------------------
TRModule* orm;
//---------------------------------------------------------------------------
K_exportC(void*)  nsModuleStart  ()
{
   orm = new TRModule();
}
//---------------------------------------------------------------------------
K_exportC(const char*)  nsModuleDesp  ()
{
   return "Request Routing Module.  V1.0.1.0";
}
//---------------------------------------------------------------------------
K_exportC(bool)  nsModuleFinish  ()
{
   delete orm;
   return true;
}
/////////////////////////////////////////////////////////////////////////////
