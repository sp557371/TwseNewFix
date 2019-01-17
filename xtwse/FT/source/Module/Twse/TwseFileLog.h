//---------------------------------------------------------------------------
#ifndef TwseFileLogH
#define TwseFileLogH
//---------------------------------------------------------------------------
#include "KFile.h"
//---------------------------------------------------------------------------
namespace Kway {
//---------------------------------------------------------------------------
K_class TwseFileLog
{
protected:
   Kway::TFileMode    FileMode_;
   Kway::TFile        Log_;
   K_mf(bool) WriteLog(Kway::TFile* file, const char* buf, int len, bool rt);
   
public:
   K_ctor  TwseFileLog(const char* fname);
   K_dtor ~TwseFileLog() { Log_.Close(); }

   K_mf(bool) Log(const char* buf, int len=0, bool rt=true);
   K_mf(bool) Log(const std::string& s) { return Log(s.c_str()); }
};
//---------------------------------------------------------------------------
//K_fn(std::string)  GetLogPath();
//---------------------------------------------------------------------------
}   // Kway
//---------------------------------------------------------------------------
#endif

