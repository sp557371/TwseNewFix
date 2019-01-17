//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#include <stdio.h>
#endif
//---------------------------------------------------------------------------
#include "TwseFileLog.h"
#include "KTime.h"
//---------------------------------------------------------------------------
using namespace Kway;
//---------------------------------------------------------------------------
/*
K_mf(std::string)  Kway::Tw::Fix::GetLogPath()
{
   static std::string fn;
   char Buf[128];
   sprintf(Buf, "./Logs/%s/", TKDate::Now().as_string(TKDate::sf_ELong,0).c_str());
   fn = std::string(Buf);
   return fn;
}
*/
//---------------------------------------------------------------------------
K_ctor TwseFileLog::TwseFileLog(const char* fname)
    : FileMode_(Kway::TFileMode(fmCreatePath|fmOpenAlways|fmAppend))
{
   Log_.Open(fname, FileMode_);
}
//---------------------------------------------------------------------------
K_mf(bool) TwseFileLog::Log(const char* buf, int len, bool rt)
{
   return WriteLog(&Log_, buf, len==0 ? strlen(buf):len, rt);
}
//---------------------------------------------------------------------------
K_mf(bool) TwseFileLog::WriteLog(Kway::TFile* file, const char* buf, int len, bool rt)
{
   //static const char NewLine[] = {0x0A};
   static const char NewLine[] = {0x0A,0x00};
   //static const char NewLine[] = "\n";
   std::string time = Kway::TKTime::Now().as_string() + "." + Kway::TKTimeMS::Now().as_string() + "---";
   file->Write(time.c_str());
   TFile::size_type size = file->Write(buf, len);
   if(rt)
      file->Write(NewLine);

   return size==len;
}
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
