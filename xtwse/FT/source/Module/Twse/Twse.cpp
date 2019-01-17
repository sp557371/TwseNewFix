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
#include "ExtDef.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw::Bx;
using namespace Kway::Tw::Bx::FT;
//---------------------------------------------------------------------------
static TTwseFileGet  Get_;
static TTwseDispatch Disp_;
static TTwseSetting*  Setting_;
static TwseFilesHandler Twse_;
//---------------------------------------------------------------------------
TTwseLmModTimer* lm;
//---------------------------------------------------------------------------
K_exportC(Kway::TnsmStartObj*)  nsModuleStart  ()
{
   TDispatchMgr& mgr = const_cast<TDispatchMgr&>(GetDispatchMgr());
   Twse_.SetFTSender(mgr.DoReg(&Disp_,true));
   mgr.DoReg(&Get_);
   Setting_ = new TTwseSetting();
   TFile  Log_;
   char Buf[256];
   Log_.Open(std::string(GetProcLogPath()+"/version.log").c_str(),
             (TFileMode)(fmRandomAccess|fmOpenAlways));
   Log_.Seek(Log_.GetFileSize(), fsBegin);
   sprintf(Buf,"%s Twse FT Module loaded, version: 2.0.0.1, last modified date 2009-12-08.",
      TKTime::Now().as_string().c_str());
   Log_.WriteLN(Buf);
   Log_.Close();
   lm = new TTwseLmModTimer();
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
   if(lm)
      delete lm;
   //Setting_.VAMgr_.Del(*(Twse_.GetVAC()));
   delete Setting_;
   return true;
}
//---------------------------------------------------------------------------
K_mf(void) TTwseDispatch::DoDispatch(const TFTJob& job, const std::string& fname) const
{
   std::string fcode = job.FileCode_.as_string();
   std::string fpathname = fname;
// --> 雅萍要求，非F01, F03, F04, F05, F06, F07, F08, F11, F14, F15, F16, F17 不處理, 請以 XF1005 非預定處理之檔案回覆
// 所以在這裡一律不做檔名的檢查
// 20130502 add by k288
//   if(fcode[0]=='F'){// || fcode[0]=='K'){
      Twse_.StartHandleAFile((Kway::Tw::Stk::TMarket&)job.Market_, fcode, fpathname);
//   }
}
//---------------------------------------------------------------------------
K_mf(bool) TTwseFileGet::GetSource (const TFTJob& job, const std::string& src,
    const std::string& dst) const
{
   std::string fname, cmd, err;
   fname = src;
   if(job.FileCode_.as_string() == "F02"){
      std::string rname;
      rname = job.FileCode_.as_string();
      TFTJob* fj = (TFTJob*)&job;
      fj->SrcData_.assign(rname);
      fname += rname;
   }
   if(Setting_->GetULCmd(fname, dst, cmd, err) == false) {
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
K_ctor TTwseLmModTimer::TTwseLmModTimer()
   : Timer_(this)
{
   Timer_.Start(5*1000, &this_type::OnTimer);  // .2.....
fprintf(stderr,"Timer start!\n");
}
//---------------------------------------------------------------------------
K_dtor TTwseLmModTimer::~TTwseLmModTimer()
{
   Timer_.Stop();
   
   if(Setting_->GetVAMgr()) {
      //Setting_->GetVAMgr()->Del(*(Twse_.GetVAC()));
   }     
}
//---------------------------------------------------------------------------
K_mf(void) TTwseLmModTimer::OnTimer(TTimerBase*)
{
   TDispatchMgr& mgr = const_cast<TDispatchMgr&>(GetDispatchMgr());
   TFTSenderBase* ft = mgr.DoReg(&Disp_,true);
   if(ft != 0){
      Twse_.SetFTSender(ft);
      Timer_.Stop();
   }
} 
//---------------------------------------------------------------------------


