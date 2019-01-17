/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_WinProc_h_)
#define _Nsa_WinProc_h_
//---------------------------------------------------------------------------
#include <string>
#include "libnsAux.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// struct SWinProcConf
/////////////////////////////////////////////////////////////////////////////
struct SWinProcConf
{
   std::string  ExecPath_;
   std::string  ProcName_;
   std::string  CmdLine_;
};


/////////////////////////////////////////////////////////////////////////////
// class TWinProc
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TWinProc
{
   PROCESS_INFORMATION  ProcInfo_;
   SWinProcConf         ProcConf_;

public:
   K_ctor  TWinProc ();
   K_dtor ~TWinProc ();

   K_mf(bool) Start      ();
   K_mf(bool) Close      (bool forceTerminate);
   K_mf(bool) CloseQuery ();
   K_mf(bool) CloseWait  (bool forceTerminate);

   inline K_mf(void) SetPathName (const std::string& path, const std::string& name)
      {
         ProcConf_.ExecPath_ = path;
         ProcConf_.ProcName_ = name;
      }

   inline K_mf(void) SetCmdLine (const std::string& cmd)  { ProcConf_.CmdLine_ = cmd; }

protected:
   K_mf(void) CloseHandle ();
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_WinProc_h_)
