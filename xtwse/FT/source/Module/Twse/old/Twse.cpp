//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Twse.h"
#include "ModuleLoader.h"
#include "DspMgr.h"
#include "TwseSets.h"
#include "KFile.h"
#include "TwseFilesHandler.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw::Bx;
using namespace Kway::Tw::Bx::FT;
//---------------------------------------------------------------------------
static TTwseFileGet  Get_;
static TTwseDispatch Disp_;
static TTwseSetting  Setting_;
static TwseFilesHandler Twse_;
//---------------------------------------------------------------------------
K_exportC(Kway::TnsmStartObj*)  nsModuleStart  ()
{
   TDispatchMgr& mgr = const_cast<TDispatchMgr&>(GetDispatchMgr());
   mgr.DoReg(&Disp_);
   mgr.DoReg(&Get_);
   TFile  Log_;
   char Buf[256];
   strcpy(Buf,"./Logs/%4y%2m%2d/version.log");
   Log_.Open(Buf, (TFileMode)(fmRandomAccess|fmOpenAlways));
   Log_.Seek(Log_.GetFileSize(), fsBegin);
   sprintf(Buf,"%s Twse FT Module loaded, version: 2.0.0.1, last modified date 2009-09-01.",
      TKTime::Now().as_string().c_str());
   Log_.WriteLN(Buf);
   Log_.Close();
   return 0;
}
//---------------------------------------------------------------------------
K_exportC(const char*)  nsModuleDesp  ()
{
   return "Twse 檔案處理模組";
}
//---------------------------------------------------------------------------
K_exportC(bool)  nsModuleFinish  (Kway::TnsmStartObj*)
{
   TDispatchMgr& mgr = const_cast<TDispatchMgr&>(GetDispatchMgr());
   mgr.UnReg(&Disp_);
   mgr.UnReg(&Get_);
   return true;
}
//---------------------------------------------------------------------------
K_mf(void) TTwseDispatch::DoDispatch(const TFTJob& job, const std::string& fname) const
{
   std::string fcode = job.FileCode_.as_string();
   std::string fpathname = fname;
   if(fcode[0]=='K'){
      Twse_.StartHandleAFile((Kway::Tw::Stk::TMarket&)job.Market_, fcode, fpathname);
   }
}
//---------------------------------------------------------------------------
K_mf(bool) TTwseFileGet::GetSource (const TFTJob& job, const std::string& src,
    const std::string& dst) const
{
   std::string fname, cmd, err;
   fname = src;
   if(job.FileCode_.as_string() == "K02"){
      std::string rname;
      rname  = (job.Market_ == Kway::Tw::Stk::m_Tse?"T":"O");
      rname += job.FileCode_.as_string();
      TFTJob* fj = (TFTJob*)&job;
      fj->SrcData_.assign(rname);
      fname += rname;
   }
   if(Setting_.GetULCmd(fname, dst, cmd, err) == false) {
      WriteSettingError(err);
      return false;
   }else {
      WriteSystemCmd(cmd);
      FILE* ptr;
      if((ptr = popen(cmd.c_str(),"w")) == NULL) {
         WriteSystemFail(cmd);
         return false;
      }
      pclose(ptr);
   }
   return true;
}
//---------------------------------------------------------------------------


