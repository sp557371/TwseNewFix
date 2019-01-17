//
// libnsSignonPolicy.h
//
// ª©¥»: $Id$
//
//---------------------------------------------------------------------------
#ifndef libnsSignonPolicyH
#define libnsSignonPolicyH
//---------------------------------------------------------------------------
#include "KwayBase.h"
//---------------------------------------------------------------------------
#ifdef LIBNSSIGNONPOLICY_EXPORTS
  #define SIGPODLL               K_EXPORT
#else
  #define SIGPODLL               K_IMPORT
#endif

#define SIGPO_class              class  SIGPODLL
#define SIGPO_struct             struct SIGPODLL
// functions
#define SIGPO_fn(return_type)    extern SIGPODLL return_type K_FASTCALL
// global datas
#define SIGPO_gd(data_type)      extern SIGPODLL data_type
// extern "C" export
#define SIGPO_exportC(data_type) extern "C" SIGPODLL data_type
//---------------------------------------------------------------------------
#endif
