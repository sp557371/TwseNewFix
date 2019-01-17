//
// libsneTwStk.h
//
// ª©¥»: $Id$
//
//---------------------------------------------------------------------------
#ifndef libsneTwStkH
#define libsneTwStkH
//---------------------------------------------------------------------------
#include "KwayBase.h"
//---------------------------------------------------------------------------
#ifdef LIBSNETWSTK_EXPORTS
  #define SNETWSTKDLL               K_EXPORT
#else
  #define SNETWSTKDLL               K_IMPORT
#endif

#define SNETWSTK_class              class  SNETWSTKDLL
#define SNETWSTK_struct             struct SNETWSTKDLL
// functions
#define SNETWSTK_fn(return_type)    extern SNETWSTKDLL return_type K_FASTCALL
// global datas
#define SNETWSTK_gd(data_type)      extern SNETWSTKDLL data_type
// extern "C" export
#define SNETWSTK_exportC(data_type) extern "C" SNETWSTKDLL data_type

#define TWSTK_EXTERN_TEMPLATE_CLASS     template class TWSTKDLL
//---------------------------------------------------------------------------
#endif
