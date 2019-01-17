/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#include <vcl.h>
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_WinProc.h"
#include <stdio.h>
#include "BCB/EnumProc.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TWinProc impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TWinProc::TWinProc ()
{
   memset(&ProcInfo_, 0, sizeof(ProcInfo_));
}
//---------------------------------------------------------------------------

K_dtor TWinProc::~TWinProc ()
{
   CloseHandle();
}
//---------------------------------------------------------------------------

K_mf(bool) TWinProc::Start ()
{
   CloseHandle();

   STARTUPINFO startupInfo;
   DWORD       creationFlags;
   char        commandLine[512];
   LPCTSTR     currentDirectory = (ProcConf_.ExecPath_.empty() ? NULL : ProcConf_.ExecPath_.c_str());

   memset(&startupInfo, 0, sizeof(startupInfo));
   startupInfo.cb          = sizeof(STARTUPINFO);
   startupInfo.dwFlags     = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
   startupInfo.wShowWindow = SW_HIDE;
   //startupInfo.wShowWindow = SW_SHOWMINIMIZED;

   creationFlags = (CREATE_NEW_PROCESS_GROUP | CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS);

   sprintf(commandLine, "%s/%s", ProcConf_.ExecPath_.c_str(), ProcConf_.ProcName_.c_str());
   if (ProcConf_.CmdLine_.empty() == false) {
      strcat(commandLine, " ");
      strcat(commandLine, ProcConf_.CmdLine_.c_str());
   }

   if (!CreateProcess(NULL,               // pointer to name of executable module
                      commandLine,        // pointer to command line string
                      NULL,               // pointer to process security attributes
                      NULL,               // pointer to thread security attributes
                      true,               // handle inheritance flag
                      creationFlags,      // creation flags
                      NULL,               // pointer to new environment block
                      currentDirectory,   // pointer to current directory name
                      &startupInfo,
                      &ProcInfo_))
   {

      DWORD errCode = GetLastError();

      return false;
   }

   return true;
}
//---------------------------------------------------------------------------

K_mf(void) TWinProc::CloseHandle ()
{
   if (ProcInfo_.dwProcessId) {
      HANDLE hp = ProcInfo_.hProcess;
      HANDLE ht = ProcInfo_.hThread;

      memset(&ProcInfo_, 0, sizeof(ProcInfo_));
      ::CloseHandle(hp);
      ::CloseHandle(ht);
   }
}
//---------------------------------------------------------------------------

K_mf(bool) TWinProc::CloseQuery ()
{
   if (ProcInfo_.dwProcessId == 0)
      return false;

   //安全結束Process參考: http://www.vckbase.com/document/viewdoc/?id=415
   CMainWindowIterator itw(ProcInfo_.dwProcessId);

   for (HWND hWnd=itw.First(); hWnd; hWnd=itw.Next()) {
      DWORD bOKToKill = FALSE;

      SendMessageTimeout(hWnd, WM_QUERYENDSESSION, 0, 0,
                         SMTO_ABORTIFHUNG|SMTO_NOTIMEOUTIFNOTHUNG, 100, &bOKToKill);

      printf("hWnd[%p], bOKToKill[%d]\n", hWnd, bOKToKill);

      if (!bOKToKill)
         return false;

      PostMessage(hWnd, WM_CLOSE, 0, 0);
   }

   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TWinProc::CloseWait (bool forceTerminate)
{
   if (ProcInfo_.dwProcessId == 0)
      return false;

   DWORD resu = WaitForSingleObject(ProcInfo_.hProcess, 15*1000);

   if (resu != WAIT_OBJECT_0) {
      if (forceTerminate) {
         TerminateProcess(ProcInfo_.hProcess, 0);
      }
      else {
         //Kway::ErrLog( w==WAIT_FAILED ? "Close(%s) Result:0x%x Error:%d" : "Close(%s) Result:0x%x"
         //            , ProcessSet_.CommandLine_.c_str(), w, GetLastError() );
         //State_ = ps_CloseErr;
         return false;
      }
   }

   /*
   //wait Stdout empty
   if(Thread_==NULL  ||  Thread_->IsSyncing())
      ReadStdout(true);
   else
   if(HANDLE h = Stdout_.GetRdHandle()) {
      DWORD rdsz;
      char  buffer[1];
      while(PeekNamedPipe(h, buffer, 1, &rdsz, NULL, NULL)) {
         if(rdsz <= 0)
            break;
         SleepEx(1,true);
      }
   }
   */

   CloseHandle();

   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TWinProc::Close (bool forceTerminate)
{
   if (!CloseQuery())
      return false;
   return CloseWait(forceTerminate);
}
//---------------------------------------------------------------------------

