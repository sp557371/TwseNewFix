/////////////////////////////////////////////////////////////////////////////
#if !defined(_NsaBaseType_h_)
#define _NsaBaseType_h_
//---------------------------------------------------------------------------
#include <vector>
#include "char_ary.h"
#include "fixed_num.h"
#include "pod_vector.h"
#include "KFile.h"
#include "libnsAux.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// define
/////////////////////////////////////////////////////////////////////////////
#define NSA_KFILE_INVALID_SIZE  TFile::size_type(-1)

/////////////////////////////////////////////////////////////////////////////
// Â²³æªº«¬ºA
/////////////////////////////////////////////////////////////////////////////
typedef fixed_num<size_t>  TSeqNoSizet;
typedef fixed_num<size_t>  TNsaSizeT;
typedef fixed_num<int>     TNsaInt;
typedef fixed_num<int32>   TNsaInt32;
typedef fixed_num<int64>   TNsaInt64;
typedef fixed_num<uint32>  TNsaUInt32;
//---------------------------------------------------------------------------
typedef std::vector<std::string>  TSvStrings;
typedef pod_vector <std::string>  TKpvStrings;
//---------------------------------------------------------------------------
typedef char_ary<char, 20, 0>    TConfKey;
typedef char_ary<char, 20, 0>    TPoliName;

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_NsaBaseType_h_)
