/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
   #pragma hdrstop
#endif
//---------------------------------------------------------------------------
#include <iostream>
#include "SignonSvrDB.h"
#include "SyncEv.h"
#include "InpHandler.h"
#include "main01.h"
#include "signal.h"
//---------------------------------------------------------------------------
#if defined(K_UNIXLIKE)
   #include <dlfcn.h>
   #define LoadLibrary(fname)		    dlopen(fname,RTLD_NOW)
   #define FreeLibrary(hnd)		    dlclose(hnd)
   #define GetProcAddress(hnd,name)  dlsym(hnd,name)
   #define HINSTANCE			          void*
#else
   #define dlerror()  ""
#endif
//---------------------------------------------------------------------------
using namespace Kway::Signon;
using namespace Kway::Signon::Policy;
//---------------------------------------------------------------------------
typedef TSignonSvr*  (*TfnSignonSvrStart)();
typedef void         (*TfnSignonSvrFinish)(TSignonSvr*);
HINSTANCE            SvrLib_;
TSignonSvrDB*        SignonSvr;
Main01*              RunMain;
//---------------------------------------------------------------------------
void  FreeSvrLib  ()
{
   if(SvrLib_ == NULL)
      return;

   #if defined(K_UNIXLIKE)
   TfnSignonSvrFinish   fnFinish = (TfnSignonSvrFinish)(GetProcAddress(SvrLib_, "SignonSvrFinish"));
   #else
   TfnSignonSvrFinish   fnFinish = (TfnSignonSvrFinish)(GetProcAddress(SvrLib_, "_SignonSvrFinish"));
   #endif
   if(fnFinish) {
      fprintf(stderr,"結束中...");
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
   #if defined(K_UNIXLIKE)
   TfnSignonSvrStart fnStart = (TfnSignonSvrStart)(GetProcAddress(SvrLib_, "SignonSvrStart"));
   #else
   TfnSignonSvrStart fnStart = (TfnSignonSvrStart)(GetProcAddress(SvrLib_, "_SignonSvrStart"));
   #endif
   if(fnStart == 0) {
      fprintf(stderr,"Sym ERROR:%s\n", dlerror());
      return;
   }
   fprintf(stderr,"啟動中...\n");
   SignonSvr = static_cast<TSignonSvrDB*>(fnStart());
   fprintf(stderr,"OK!\n");
}
//---------------------------------------------------------------------------
int InitMain ()
{
  signal(SIGINT, SIG_IGN);
  signal(SIGSEGV, SIG_IGN);
  #if defined(__BORLANDC__)
  LoadSvrLib("libxsSignonSvrDB.dll");
  #else
  LoadSvrLib("libxsSignonSvrDB.so");
  #endif
  RunMain=new Main01(SignonSvr);
  return 0;
}
//---------------------------------------------------------------------------
// cooper {
#if defined(__hpux)
extern "C" void allow_unaligned_data_access ();
#endif
// } cooper
//---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
   // cooper {
   #if defined(__hpux)
   // http://docs.hp.com/en/7730/newhelp0610/pragmas.htm#pragma-pack-ex3
   // Link in the library -lunalign
   allow_unaligned_data_access();
   #endif
   // } cooper

   InitProc();
   std::string str = std::string(argv[0]);
   std::string::size_type   Pos1,Pos2;
   Pos1 = 0;
   Pos2 = 0;
   while((Pos1 = str.find('/',Pos2)) != std::string::npos)
      str = std::string(str.c_str()+Pos1+1);
   SetProgName(str);
   SetProcName(str);
   SetProcBrkID(TBrokerID("----"));
   if(argc > 1) {
      for(int L = 1;  L < argc;  ++L){
         if(argv[L][0]=='-' && argv[L][1]=='p'){
            SetProcName(std::string(argv[L]).substr(2,std::string(argv[L]).length()-2));
            break;
         }
         if(argv[L][0]=='-' && argv[L][1]=='m'){
            SetProcMkt(argv[L][2]=='T'?Kway::Tw::Stk::m_Tse:Kway::Tw::Stk::m_Otc);
            continue;
         }
      }
   }
   Kway::TNSIniter   nsIniter(argc,argv); 
   Kway::TInpHandler  inp;
   Kway::TSyncObj::AppRun(InitMain, argc, argv, &inp);
   delete RunMain;
   FreeSvrLib();
  
   return 0;
}
//===========================================================================
