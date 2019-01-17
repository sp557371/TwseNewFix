//
// libnsTwStk.h
//
// ª©¥»: $Id$
//
//---------------------------------------------------------------------------
#ifndef libnsTwStkH
#define libnsTwStkH
//---------------------------------------------------------------------------
#include "KwayBase.h"
//---------------------------------------------------------------------------
#ifdef LIBNSTWSTK_EXPORTS
  #define TWSTKDLL               K_EXPORT
#else
  #define TWSTKDLL               K_IMPORT
#endif

#define TWSTK_class              class  TWSTKDLL
#define TWSTK_struct             struct TWSTKSDLL
// functions
#define TWSTK_fn(return_type)    extern TWSTKDLL return_type K_FASTCALL
// global datas
#define TWSTK_gd(data_type)      extern TWSTKDLL data_type
// extern "C" export
#define TWSTK_exportC(data_type) extern "C" TWSTKDLL data_type

#define TWSTK_EXTERN_TEMPLATE_CLASS     template class TWSTKDLL
//---------------------------------------------------------------------------
#endif
