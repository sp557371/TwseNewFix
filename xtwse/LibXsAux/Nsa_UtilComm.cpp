/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#if defined(__BORLANDC__)
#include <vcl.h>
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_UtilComm.h"
#include <stdio.h>
#include <stdarg.h>
#include "KTime.h"
#include "Nsa_LineBuf.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TSizeCalc impl
/////////////////////////////////////////////////////////////////////////////
const Kway::uint64  TSizeCalc::Size_1KB =       1024;
const Kway::uint64  TSizeCalc::Size_1MB =    1048576;
const Kway::uint64  TSizeCalc::Size_1GB = 1073741824;
//---------------------------------------------------------------------------
K_mf(std::string) TSizeCalc::as_string () const
{
   char buf[100];
   int  len;
   
   buf[0] = '\0';
   // Byte
   if (Data_ < Size_1KB) {
      len = sprintf(buf, "%d Byte", Data_);
   }
   // KB
   else if (Data_ < Size_1MB) {
      double d = Data_ / (double)Size_1KB;
      len = sprintf(buf, "%.2f KB", d);
   }
   // MB
   else if (Data_ < Size_1GB) {
      double d = Data_ / (double)Size_1MB;
      len = sprintf(buf, "%.2f MB", d);
   }
   // G
   else {
      double d = Data_ / (double)Size_1GB;
      len = sprintf(buf, "%.3f GB", d);
   }
   
   return std::string(buf, len);
}


/////////////////////////////////////////////////////////////////////////////
// class TUtilComm impl
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TUtilComm::FileExists (const char* filePath)
{
   FILE* fp = fopen(filePath, "r");

   if (fp) {
      fclose(fp);
      return true;
   }

   return false;   
}
//---------------------------------------------------------------------------

void TUtilComm::ConsMsg (const char* fmt, ...)
{
   va_list  argptr;
   char     buf[5120];
   TLineBuf line;
   int      len;

   len = sprintf(buf, "%s ", TKDateTimeMS::Now().as_string(TKDate::sf_ELong).c_str());
   line.Append(buf, len);

   va_start(argptr, fmt);
   len = vsprintf(buf, fmt, argptr);
   va_end(argptr);

   if (len != EOF)
      line.Append(buf, len);

   line.AppendNewLine();

   fprintf(stderr, line.GetCStr());
}
//---------------------------------------------------------------------------

K_mf(void) TUtilComm::ConsMsg (int msgLen, const char* msg)
{
   char     buf[50];
   TLineBuf line;
   int      len;

   len = sprintf(buf, "%s ", TKDateTimeMS::Now().as_string(TKDate::sf_ELong).c_str());
   line.Append(buf, len);

   line.Append(msg, msgLen);
   line.AppendNewLine();

   fprintf(stderr, line.GetCStr());
}
//---------------------------------------------------------------------------
#if defined(__BORLANDC__)
K_mf(bool) TUtilComm::GetArgs (char argcode, const char** param)
{
   if (param) {
      *param = NULL;
   }

   for (int L=1; L<_argc; ++L) {
      if (_argv[L][0] == '-' && _argv[L][1] == argcode) {
         if (param) {
            if(_argv[L][2] != 0) {
               *param = _argv[L]+2;
            }
            else if (_argv[L+1] != NULL) {
               *param = _argv[L+1];
            }
         }

         return true;
      }
   }

   return false;
}
//---------------------------------------------------------------------------

K_mf(bool) TUtilComm::SetPrvInstance ()
{
   // 利用【程式名稱 + " - Kway"】來當 Title
   // Ex:
   //    C:\Kway\Mds\Mds.exe
   //    ==> Title = "Mds - Kway"
   std::string            strExePath = _argv[0];
   std::string::size_type pos        = strExePath.rfind('\\');
   std::string            strExeName = strExePath.substr(pos+1);

   pos = strExeName.find('.');
   if (pos != std::string::npos) {
      strExeName.erase(pos);
   }

   std::string strTitle(strExeName);
   strTitle.append(" - Kway");

   bool ResVal = SetPrvInstance(strExeName.c_str(), strTitle.c_str());
   if (ResVal == false) {
      Application->Title = strTitle.c_str();
   }

   return ResVal;
}
//---------------------------------------------------------------------------

K_mf(bool) TUtilComm::SetPrvInstance (const char* caExeName, const char* caTitle)
{
   char buf[128];
   
   sprintf(buf, "%sExe", caExeName);
   ::CreateMutex(NULL, FALSE, buf);
   if (GetLastError() == ERROR_ALREADY_EXISTS) {
      HWND FirsthWnd, FirstChildhWnd;

      sprintf(buf, "%sTitleTesting", caTitle);
      Application->Title = buf;

      FirsthWnd = ::FindWindow("TApplication", caTitle);
      if (::IsIconic(FirsthWnd))
         ::ShowWindow(FirsthWnd, SW_SHOWDEFAULT);
      ::SetForegroundWindow(FirsthWnd);
      FirstChildhWnd = ::GetLastActivePopup(FirsthWnd);
      if (FirsthWnd != FirstChildhWnd)
         ::BringWindowToTop(FirstChildhWnd); // A pop-up is active so bring it to the top too.

      return true;
   }
   return false;
}
//---------------------------------------------------------------------------

K_mf(std::string) TUtilComm::GetModuleVersion ()
{
   char filePath[256];

   if (!GetModuleFileName(NULL, filePath, sizeof(filePath)))
      return std::string();

   return GetModuleVersion(filePath);
}
//----------------------------------------------------------------------

K_mf(std::string) TUtilComm::GetModuleVersion (const char* ModulePath)
{
   BOOL	 m_bValid;
   LPBYTE lpbyVIB;
	DWORD  dwDummy = 0;
	
   DWORD dwSize = GetFileVersionInfoSize(const_cast<LPTSTR>(ModulePath),
                                         &dwDummy // Set to 0
	                                     );

   if (dwSize == 0) {
      return std::string();
   }
   
   lpbyVIB = new BYTE[dwSize];
   if (lpbyVIB == NULL) {
      return std::string();
   }

   std::string strResVal;

   if (GetFileVersionInfo(const_cast<LPTSTR>(ModulePath), 0, dwSize, lpbyVIB)) {
      VS_FIXEDFILEINFO vsffi;
      UINT             uLen    = 0;
	   LPVOID           lpVSFFI = NULL;

      if (VerQueryValue(lpbyVIB, _T("\\"), (LPVOID*)&lpVSFFI, &uLen)) {
         memcpy(&vsffi, lpVSFFI, sizeof(VS_FIXEDFILEINFO));
         if (vsffi.dwSignature == VS_FFI_SIGNATURE) {
            char buf[50];
            WORD Major   = HIWORD(vsffi.dwFileVersionMS);
            WORD Minor   = LOWORD(vsffi.dwFileVersionMS);
            WORD Release = HIWORD(vsffi.dwFileVersionLS);
            WORD Build   = LOWORD(vsffi.dwFileVersionLS);

            sprintf(buf, "%d.%d.%d.%d", Major, Minor, Release, Build);
            strResVal = buf;
         }
      }
   }

   delete [] lpbyVIB;
   return strResVal;
}
#endif // #if defined(__BORLANDC__)
//---------------------------------------------------------------------------
#if defined(K_WINDOWS)
K_mf(bool) TUtilComm::GetModuleName (char* fileName, bool incExt)
{
   char   path[256];
   char*  cpStart;
   char*  cpEnd;

   fileName[0] = '\0';
   if (!GetModuleFileName(NULL, path, sizeof(path)))
      return false;

   cpStart = strrchr(path, (int)'\\');
   if (!cpStart)
      return false;

   // 包含副檔名
   if (incExt) {
      strcpy(fileName, cpStart+1);
      return true;
   }

   cpEnd = strchr(cpStart, (int)'.');
   if (cpEnd) {
      *cpEnd = '\0';
   }
   strcpy(fileName, cpStart+1);

   return true;
}
//----------------------------------------------------------------------

void TUtilComm::BugOut (const char* fmt, ...)
{
	HWND hWnd = FindWindow("BugOut", NULL);
	if (hWnd == NULL)
		return;

	char       Buffer[1024*10];
	char*      cpTime = Buffer;
	char*      cpData = Buffer + 11;
	SYSTEMTIME sysTime;
	va_list    argptr;

	GetLocalTime(&sysTime);
	sprintf(cpTime, "<%02d:%02d:%02d>", sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	cpTime[10] = ' ';

	va_start(argptr, fmt);
	vsprintf(cpData, fmt, argptr);
	va_end  (argptr);

	Buffer[1023] = '\0';
	DWORD Length = lstrlen(Buffer) + 1;

	COPYDATASTRUCT cs;
	cs.dwData = 0x1245;
	cs.cbData = Length;
	cs.lpData = Buffer;
	SendMessage(hWnd, WM_COPYDATA, (WPARAM)0, (LPARAM)(PCOPYDATASTRUCT)&cs);
}
#endif // #if defined(K_WINDOWS)
//---------------------------------------------------------------------------
