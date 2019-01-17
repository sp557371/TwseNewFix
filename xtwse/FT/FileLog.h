//---------------------------------------------------------------------------
#ifndef FileLogH
#define FileLogH
//---------------------------------------------------------------------------
#include "KFile.h"             
#include "KTime.h"
using namespace Kway;
//char* logFN=".\\RptLog\\RptRecvLog.log";
//---------------------------------------------------------------------------
K_class FileLog// : public TFile
{
protected:
   TFile        Log_;
public:
   K_ctor FileLog();//(char* fileName);
   K_dtor ~FileLog(){Log_.Close();};

   K_mf(bool) log(char* buf, int len);
   K_mf(bool) log(char* buf) { return log(buf, strlen(buf)); }
};
//---------------------------------------------------------------------------
K_class MesLog : public FileLog
{
public:
   K_ctor MesLog(const char* filename);
};
//---------------------------------------------------------------------------
extern FileLog	appLog;
#endif

