/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvMap_2k1d_File_h_)
#define _Nsa_EvMap_2k1d_File_h_
//---------------------------------------------------------------------------
#include "Nsa_EvMap_File.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TEvMap2k1dFile
/////////////////////////////////////////////////////////////////////////////
template <class MapT1, class FileDat2 = MapT1::mapped_type::mapped_type>
class TEvMap2k1dFile
{
public:
   //==============================================
   // Container
   typedef          MapT1               TMap1;
   typedef typename TMap1::mapped_type  TMap2;
   typedef          FileDat2            TFileDat2;

   //==============================================
   // Recorder
   typedef TRecorder2d<TMap1, TMap2, TFileDat2>  TRecorder;
};
   
/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvMap_2k1d_File_h_)
