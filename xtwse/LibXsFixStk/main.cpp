//========================================================================
//
//========================================================================
#include <iostream>
#include "SignonSvrDB.h"
#include "SyncEv.h"
#include "InpHandler.h"
//#include "main01.h"
#include "signal.h"
#include <dlfcn.h>
//---------------------------------------------------------------------------
#define LoadLibrary(fname)		dlopen(fname,RTLD_NOW)
#define FreeLibrary(hnd)		dlclose(hnd)
#define GetProcAddress(hnd,name)	dlsym(hnd,name)
#define HINSTANCE			void*
//---------------------------------------------------------------------------
using namespace Kway::Signon;
using namespace Kway::Signon::Policy;
//---------------------------------------------------------------------------
typedef TSignonSvr*  (*TfnSignonSvrStart)();
typedef void         (*TfnSignonSvrFinish)(TSignonSvr*);
HINSTANCE            SvrLib_;
TSignonSvrDB*        SignonSvr;
//Main01*              RunMain;
//---------------------------------------------------------------------------
void  FreeSvrLib  ()
{
   if(SvrLib_ == NULL)
      return;

   TfnSignonSvrFinish   fnFinish = (TfnSignonSvrFinish)(GetProcAddress(SvrLib_, "SignonSvrFinish"));
   if(fnFinish) {
      fprintf(stderr,"������...");
      fnFinish(SignonSvr);
      fprintf(stderr,"OK!\n");
   }
   fprintf(stderr,"FreeLibrary...");
   FreeLibrary(SvrLib_);
   fprintf(stderr,"OK!\n");
}
//---------------------------------------------------------------------------
void  LoadSvrLib  (const char* name)
{
   FreeSvrLib();
   fprintf(stderr,"Loading... SignonSvr: %s\n", name);

   SvrLib_ = LoadLibrary(name);
   if(SvrLib_ == NULL) {
      fprintf(stderr,"Load ERROR:%s\n", dlerror());
      return;
   }
   TfnSignonSvrStart fnStart = (TfnSignonSvrStart)(GetProcAddress(SvrLib_, "SignonSvrStart"));
   if(fnStart == 0) {
      fprintf(stderr,"Sym ERROR:%s\n", dlerror());
      return;
   }
   fprintf(stderr,"�Ұʤ�...\n");
   SignonSvr = static_cast<TSignonSvrDB*>(fnStart());
   fprintf(stderr,"OK!\n");
}
//---------------------------------------------------------------------------
int InitMain ()
{
  signal(SIGINT, SIG_IGN);
  signal(SIGSEGV, SIG_IGN);
  LoadSvrLib("libxsSignonSvrDB.so");
  //RunMain=new Main01(SignonSvr);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////
// main
/////////////////////////////////////////////////////////////////////////////
/*
#include "StrSplit.h"
void Test_StrSplit ()
{
   TStrSplit sp("1=a;2=b", ';');
   
   fprintf(stderr, "1=%s\n", sp.GetStr_ByKey("1", '=').c_str());
}
*/
//---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
   Kway::TNSIniter   nsIniter(argc,argv); 
   Kway::TInpHandler inp;
  
   //Test_StrSplit();
   
   Kway::TSyncObj::AppRun(InitMain, argc, argv, &inp);
   //delete RunMain;
   FreeSvrLib();
}
//===========================================================================
