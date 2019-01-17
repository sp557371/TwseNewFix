//---------------------------------------------------------------------------
#pragma hdrstop

//---------------------------------------------------------------------------
#include <iostream>
#include "ModuleLoader.h"
#include "SyncEv.h"
#include "SignonSvrDB.h"
#include "InpHandler.h"
#include "runmain.h"
//------------------------------------------------------------- --------------
#ifdef K_UNIXLIKE
   #include <dlfcn.h>
   #define LoadLibrary(path)        dlopen(path,RTLD_NOW)
   #define FreeLibrary(handle)      dlclose(handle)
   #if HAVE_DLFUNC
   #define GetProcAddress(h,sym)    dlfunc(h,sym)
   #else
   #define GetProcAddress(h,sym)    dlsym(h,sym)
   #endif
#endif

using namespace Kway;
using namespace Kway::Signon;
using namespace Kway::Signon::Policy;
//---------------------------------------------------------------------------
typedef TSignonSvr*  (*TfnSignonSvrStart)   ();
typedef void         (*TfnSignonSvrFinish)  (TSignonSvr*);
TSignonSvrDB*         SignonSvr;
//---------------------------------------------------------------------------
HINSTANCE     SvrLib_;
TRunMain*     RunMain;
//---------------------------------------------------------------------------
//TViewAreaMgr    SysViewAreaMgr; //系統設定管理員
K_mf(void) LoadSvrLib(const char* name);
K_mf(void) FreeSvrLib();
//---------------------------------------------------------------------------
K_mf(void) LoadSvrLib(const char* name)
{
   FreeSvrLib();
   printf("Loading... SignonSvr: %s\n", name);
   fflush(stdout);

   SvrLib_ = LoadLibrary(name);
   if(SvrLib_ == NULL) {
      printf("Load ERROR:%s\n", dlerror());
      return;
   }
   TfnSignonSvrStart fnStart = (TfnSignonSvrStart)(GetProcAddress(SvrLib_, "SignonSvrStart"));
   if(fnStart == 0) {
      printf("Sym ERROR:%s\n", dlerror());
      return;
   }
   printf("啟動中...\n");
   fflush(stdout);

   SignonSvr = static_cast<TSignonSvrDB*>(fnStart());
   printf("OK!\n");
}
//---------------------------------------------------------------------------
K_mf(void) FreeSvrLib()
{
   if(SvrLib_ == NULL)
      return;
   TfnSignonSvrFinish   fnFinish = (TfnSignonSvrFinish)(GetProcAddress(SvrLib_, "SignonSvrFinish"));
   if(fnFinish) {
      printf("結束中...");
      fflush(stdout);
      fnFinish(SignonSvr);
      printf("OK!\n");
   }
   printf("FreeLibrary...");
   fflush(stdout);
   FreeLibrary(SvrLib_);
   printf("OK!\n");
}
//---------------------------------------------------------------------------
int InitMain ()
{
   LoadSvrLib("libxsSignonSvrDB.so");
   RunMain = new TRunMain();
   return 0;
}
//---------------------------------------------------------------------------
#if defined(__hpux)
extern "C" void allow_unaligned_data_access ();
#endif
int main(int argc, char* argv[])
{
   #if defined(__hpux)
   allow_unaligned_data_access();
   #endif
   InitProc();
   std::string str = std::string(argv[0]);
   std::string::size_type   Pos1,Pos2;
   Pos1 = 0;
   Pos2 = 0;
   while((Pos1 = str.find('/',Pos2)) != std::string::npos)
      str = std::string(str.c_str()+Pos1+1);
   SetProgName(str);
   SetProcName(str);
  if(argc > 1) {
     for(int L = 1;  L < argc;  ++L){
        if(argv[L][0]=='-' && argv[L][1]=='p'){
           SetProcName(std::string(argv[L]).substr(2,std::string(argv[L]).length()-2));
           continue;
        }
        if(argv[L][0]=='-' && argv[L][1]=='m'){
           SetProcMkt(argv[L][2]=='T'?Kway::Tw::Stk::m_Tse:Kway::Tw::Stk::m_Otc);
           continue;
        }
        if(argv[L][0]=='-' && argv[L][1]=='b'){
           SetProcBrkID(TBrokerID(std::string(argv[L]).substr(2,std::string(argv[L]).length()-2)));
           continue;
        }
     }
  }
  Kway::TNSIniter nsIniter(argc, argv);
  Kway::TInpHandler  inp;
  Kway::TSyncObj::AppRun(InitMain, argc, argv, &inp);
  delete RunMain;
  FreeSvrLib();
}
//---------------------------------------------------------------------------


