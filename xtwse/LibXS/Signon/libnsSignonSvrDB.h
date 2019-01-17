//
// libnsSignonSvrDB.h
//
// ª©¥»: $Id$
//
//---------------------------------------------------------------------------
#ifndef libnsSignonSvrDBH
#define libnsSignonSvrDBH
//---------------------------------------------------------------------------
#include "KwayBase.h"
//---------------------------------------------------------------------------
#ifdef LIBNSSIGNONSVRDB_EXPORTS
  #define SIGDBDLL               K_EXPORT
#else
  #define SIGDBDLL               K_IMPORT
#endif

#define SIGDB_class              class  SIGDBDLL
#define SIGDB_struct             struct SIGDBDLL
// functions
#define SIGDB_fn(return_type)    extern SIGDBDLL return_type K_FASTCALL
// global datas
#define SIGDB_gd(data_type)      extern SIGDBDLL data_type
// extern "C" export
#define SIGDB_exportC(data_type) extern "C" SIGDBDLL data_type
//---------------------------------------------------------------------------
#endif
