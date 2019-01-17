//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "DspMgr.h"
#include <stdlib.h>
#include <errno.h>
#include "ExtDef.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx::FT;
//static std::string ErrLogName = GetProcLogPath()+"/error.log";
//static std::string SysLogName = GetProcLogPath()+"/exec.log";
static TDispatchMgr*        DisMgr_;
//---------------------------------------------------------------------------
static K_mf(void) WriteLog(const std::string& fname, const std::string& str)
{
   Kway::TFile file;
   file.Open(std::string(GetProcLogPath()+"/error.log").c_str(), Kway::TFileMode(Kway::fmAppend | Kway::fmOpenAlways | Kway::fmCreatePath));
   std::string s = Kway::TKTime::Now().as_string() + " " + str;
   file.WriteLN(s.c_str());
}
//---------------------------------------------------------------------------
K_mf(void) Kway::Tw::Bx::FT::WriteSystemFail(const std::string& cmd)
{
   std::string reason;
   switch(errno)
   {
   case ENOENT: reason = "Path or file function not found!  "; break;
   case ENOEXEC:reason = "Exec format error!  ";               break;
   case ENOMEM: reason = "Not enough memory!  ";               break;
   default:     reason = "unknown!  ";                         break;
   }
   WriteLog(std::string(GetProcLogPath()+"/error.log").c_str(), reason + cmd);
}
//---------------------------------------------------------------------------
K_mf(void) Kway::Tw::Bx::FT::WriteSettingError(const std::string& error)
{
   WriteLog(std::string(GetProcLogPath()+"/error.log").c_str(), error);
}
//---------------------------------------------------------------------------
K_mf(void) Kway::Tw::Bx::FT::WriteSystemCmd(const std::string& cmd)
{
   WriteLog(std::string(GetProcLogPath()+"/exec.log").c_str(), "Success!  "+cmd);
}
//---------------------------------------------------------------------------
K_fn(const TDispatchMgr&) Kway::Tw::Bx::FT::GetDispatchMgr()
{
   if(DisMgr_ == 0)
      DisMgr_ = new TDispatchMgr();
  return *DisMgr_;
}
//---------------------------------------------------------------------------
K_fn(const TDispatchMgr&) Kway::Tw::Bx::FT::GetDispatchMgr(TFTSenderBase* fts)
{
   if(DisMgr_ == 0)
      DisMgr_ = new TDispatchMgr();
  DisMgr_->SetSender(fts);
  return *DisMgr_;
}
//---------------------------------------------------------------------------
K_mf(bool) TDispatchMgr::DoReg(const TFileDispatch* fdis)
{
  if(DispatchSet_.find(fdis) == DispatchSet_.end())
  {
    DispatchSet_.insert(fdis);
    return true;
  }
  return false;
}
//---------------------------------------------------------------------------
K_mf(TFTSenderBase*) TDispatchMgr::DoReg(const TFileDispatch* fdis,bool t)
{
  if(DispatchSet_.find(fdis) == DispatchSet_.end())
    DispatchSet_.insert(fdis);
  return FTSender_;
}
//---------------------------------------------------------------------------
K_mf(bool) TDispatchMgr::UnReg(const TFileDispatch* fdis)
{
  TDispatchSetI iter = DispatchSet_.find(fdis);
  if(iter != DispatchSet_.end())
  {
    DispatchSet_.erase(iter);
    return true;
  }
  return false;
}
//---------------------------------------------------------------------------
K_mf(void) TDispatchMgr::DoDispatch(const TFTJob& job, const std::string& fname) const
{
  for(TDispatchSetCI iter=DispatchSet_.begin(); iter!=DispatchSet_.end(); iter++)
    if(*iter)
      (*iter)->DoDispatch(job, fname);
}
//---------------------------------------------------------------------------
K_mf(std::string) TDispatchMgr::GetDLCmd(const TFTJob& job, const std::string& fname) const
{
   for(TDispatchSetCI iter=DispatchSet_.begin(); iter!=DispatchSet_.end(); iter++)
      if(*iter){
         return ((*iter)->GetDLCmd(job, fname));
      }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(bool) TDispatchMgr::DoReg(const TFileGet* fget)
{
  if(FileGet_ == NULL || FileGet_ != fget)
  {
    FileGet_ = fget;
    return true;
  }
  return false;
}
//---------------------------------------------------------------------------
K_mf(bool) TDispatchMgr::UnReg(const TFileGet* fget)
{
  if(FileGet_ && FileGet_ == fget)
  {
    FileGet_ = NULL;
    return true;
  }
  return false;
}
//---------------------------------------------------------------------------
K_mf(bool) TDispatchMgr::GetSource(const TFTJob& job, const std::string& src,
    const std::string& dst) const
{
  return FileGet_->GetSource(job, src, dst);
}
//---------------------------------------------------------------------------
K_mf(std::string) TDispatchMgr::GetULCmd(const TFTJob& job, const std::string& src,
    const std::string& dst) const
{
  if(FileGet_)
     return FileGet_->GetULCmd(job, src, dst);
  return std::string();
}
//---------------------------------------------------------------------------
K_mf(std::string) TDispatchMgr::Get3TLocalPath() const
{
  if(FileGet_)
     return FileGet_->Get3TLocalPath();
  return std::string();
}
//---------------------------------------------------------------------------

