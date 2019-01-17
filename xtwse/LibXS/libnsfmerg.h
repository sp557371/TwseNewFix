//
// libnsfmerg.h
//
// ª©¥»: $Id$
//
//---------------------------------------------------------------------------
#ifndef libnsfmergH
#define libnsfmergH
//---------------------------------------------------------------------------
#include "KwayBase.h"
//---------------------------------------------------------------------------
#ifdef LIBNSFMERG_EXPORTS
  #define FMERGDLL               K_EXPORT
#else
  #define FMERGDLL               K_IMPORT
#endif

#define FMERG_class              class  FMERGDLL
#define FMERG_struct             struct FMERGDLL
// functions
#define FMERG_fn(return_type)    extern FMERGDLL return_type K_FASTCALL
// global datas
#define FMERG_gd(data_type)      extern FMERGDLL data_type
// extern "C" export
#define FMERG_exportC(data_type) extern "C" FMERGDLL data_type
//---------------------------------------------------------------------------
#endif
