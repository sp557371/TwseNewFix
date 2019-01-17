/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_LogFile.h"
#include <stdio.h>
#include <time.h>
#include <stdarg.h>  // for va_start, va_end, va_list
#include "KTime.h"
#include "Nsa_SysInfo.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TLogMsg impl
/////////////////////////////////////////////////////////////////////////////
void TLogMsg::PutStr (ELogType logType, const char* fmt, ...)
{
   va_list argptr;
   char    buf[5120];
   int     len;

   va_start(argptr, fmt);
   len = vsprintf(buf, fmt, argptr);
   va_end(argptr);

   if (len == EOF)
      return;
   else if (len >= 5119)
      len = 5119;

   buf[len] = '\0';

   Put(logType, buf, len, NULL, true);
}
//---------------------------------------------------------------------------

K_mf(void) TLogMsg::Put (ELogType logType, const void* data, size_t size, const TKDateTimeMS* dtms, bool incNewLine)
{
   char        buf[128];
   int         len;
   const char* ccpData = (const char*)data;
   
   //==============================================
   // 看看是否要先換行
   if (data && size > 0) {
      if (ccpData[0] == 0x0a) {
         inherited::AppendNewLine();
         ++ccpData;
         size -= 1;
      }
   }

   //==============================================
   // 時間  2007/03/29 13:37:21.044 
   TKDateTimeMS currDtms((dtms ? *dtms : TKDateTimeMS::Now()));

   len = sprintf(buf, "%s ", currDtms.as_string(TKDate::sf_ELong).c_str());
   inherited::Append(buf, len);

   //==============================================
   // LogType
   static const char* LogTypeDesc[] = {
      "[Info]     ",
      "[Warning]  ",
      "[Error]    ",
      "[Send]     ",
      "[Recv]     ",
      "[MesIn]    ",
      "[Debug]    ",
   };
   static const size_t LogTypeDescLen = strlen(LogTypeDesc[0]);

   inherited::Append(LogTypeDesc[logType], LogTypeDescLen);

   //==============================================
   // 資料部份
   inherited::Append(ccpData, size);
   
   //==============================================
   // 結束符號
   if (incNewLine)
      inherited::AppendNewLine();
}


/////////////////////////////////////////////////////////////////////////////
// class TLogFile impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TLogFile::TLogFile (const char* filePath)
{
   Open(filePath);
}
//---------------------------------------------------------------------------

K_mf(bool) TLogFile::Open (const char* filePath)
{
   Kway::TFileMode opmode = Kway::TFileMode(Kway::fmCreatePath | Kway::fmAppend);

   return File_.Open(filePath, opmode);
}
//---------------------------------------------------------------------------

K_mf(bool) TLogFile::OpenDaily (const char* fileName, const char* subDir)
{
   char filePath[256];
   
   if (subDir)  sprintf(filePath, "./Logs/%s/%s/%s", TSysInfo::GetTradeDateStr().c_str(), subDir, fileName);
   else         sprintf(filePath, "./Logs/%s/%s",    TSysInfo::GetTradeDateStr().c_str(),         fileName);

   return Open(filePath);
}
//---------------------------------------------------------------------------

void TLogFile::LogStr (ELogType mtype, const char* fmt, ...)
{
   va_list argptr;
   int     cnt;
   char    buffer[5120];

   va_start(argptr, fmt);
   cnt = vsprintf(buffer, fmt, argptr);
   va_end(argptr);

   if (cnt == EOF)
      return;
   else if (cnt >= 5119)
      cnt = 5119;

   buffer[cnt] = '\0';

   WriteLog(mtype, buffer, cnt, NULL, NULL);
}
//---------------------------------------------------------------------------

void TLogFile::LogStr (ELogType logType, const TKDateTimeMS& dateTimeMS, const char* fmt, ...)
{
   va_list argptr;
   int     cnt;
   char    buffer[5120];

   va_start(argptr, fmt);
   cnt = vsprintf(buffer, fmt, argptr);
   va_end(argptr);

   if (cnt == EOF)
      return;
   else if (cnt >= 5119)
      cnt = 5119;

   buffer[cnt] = '\0';

   WriteLog(logType, buffer, cnt, NULL, &dateTimeMS);
}
//---------------------------------------------------------------------------

K_mf(void) TLogFile::LogSize (ELogType mtype, const void* data, size_t size)
{
   WriteLog(mtype, data, size, NULL, NULL);
}
//---------------------------------------------------------------------------

K_mf(void) TLogFile::WriteLog (ELogType mtype, const void* data, size_t size, const char* strExt, const TKDateTimeMS* dateTimeMS)
{
   if (File_.IsOpened() == false) {
      return;
   }
   
   char        tmp[128];
   int         len;
   TLineBufNew line(1024);
   const char* ccpData = (const char*)data;
   
   //==============================================
   // 看看是否要先換行
   if (data && size > 0) {
      if (ccpData[0] == 0x0a) {
         line.AppendNewLine();
         ++ccpData;
         size -= 1;
      }
   }

   //==============================================
   // 時間  2007/03/29 13:37:21.044 
   if (dateTimeMS)  len = sprintf(tmp, "%s ", dateTimeMS->        as_string(TKDate::sf_ELong).c_str());
   else             len = sprintf(tmp, "%s ", TKDateTimeMS::Now().as_string(TKDate::sf_ELong).c_str());

   line.Append(tmp, len);

   //==============================================
   // LogType
   switch (mtype) {
   case elt_Info:
   default:           len = sprintf(tmp, "%-13s", "[Info]");     break;
   case elt_Warning:  len = sprintf(tmp, "%-13s", "[Warning]");  break;
   case elt_Error:    len = sprintf(tmp, "%-13s", "[Error]");    break;
   case elt_Send:     len = sprintf(tmp, "%-13s", "[Send]");     break;
   case elt_Recv:     len = sprintf(tmp, "%-13s", "[Recv]");     break;
   case elt_MesIn:    len = sprintf(tmp, "%-13s", "[MesIn]");    break;
   case elt_Debug:    len = sprintf(tmp, "%-13s", "[Debug]");    break;

   //case elt_ExtMsg:
   //   {
   //      if (strExt)  sprintf(tmp, "[%-11s]", strExt);
   //      else         sprintf(tmp, "%-13s",   "[ExtMsg]");
   //   }
   //   break;
   //
   //default:  tmp[0] = '\0';  break;
   }

   line.Append(tmp, len);

   //==============================================
   // 資料部份
   line.Append(ccpData, size);
   
   //==============================================
   // 結束符號
   line.AppendNewLine();

   File_.Write(line.GetCStr(), (Kway::dword)line.GetUsedSize());
}
//---------------------------------------------------------------------------

K_mf(TLogFile&) TLogFile::GetInst ()
{
   static TLogFile logFile(std::string(std::string("./Logs/")      +
                                       TSysInfo::GetTradeDateStr() +
                                       std::string("/KwayNsaErr.log")).c_str());
   return logFile;
}
//---------------------------------------------------------------------------

