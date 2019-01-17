/////////////////////////////////////////////////////////////////////////////
#if !defined(_libnsAux_h_)
#define _libnsAux_h_
//---------------------------------------------------------------------------
#include "KwayBase.h"

/////////////////////////////////////////////////////////////////////////////
// 基本定義
/////////////////////////////////////////////////////////////////////////////
#if defined(LIBNSAUX_NODLL)
  #define LIBNSAUXDLL
#elif defined(LIBNSAUX_EXPORTS)
  #define LIBNSAUXDLL               K_EXPORT
#else
  #define LIBNSAUXDLL               K_IMPORT
#endif

#define LIBNSAUX_class              class  LIBNSAUXDLL
#define LIBNSAUX_struct             struct LIBNSAUXDLL
// functions
#define LIBNSAUX_fn(return_type)    extern LIBNSAUXDLL return_type K_FASTCALL
// global datas
#define LIBNSAUX_gd(data_type)      extern LIBNSAUXDLL data_type
// extern "C" export
#define LIBNSAUX_exportC(data_type) extern "C" LIBNSAUXDLL data_type

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_libnsAux_h_)

