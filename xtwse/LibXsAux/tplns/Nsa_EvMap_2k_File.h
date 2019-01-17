/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvMap_2k_File_h_)
#define _Nsa_EvMap_2k_File_h_
//---------------------------------------------------------------------------
#include "Nsa_EvMap_0File.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TEvMap2kFile
/////////////////////////////////////////////////////////////////////////////
template <class Map1T, class FileDat2T = typename Map1T::mapped_type::TMap::mapped_type,
                       class FileDat1T = typename Map1T::mapped_type::TDat>
                       
class TEvMap2kFile
{
public:
   //==============================================
   // Container
   typedef          Map1T               TMap1;
   typedef          FileDat1T           TFileDat1;
   typedef          FileDat2T           TFileDat2;
   typedef typename TMap1::mapped_type  TMappedType1;
   typedef typename TMappedType1::TMap  TMap2;

   //==============================================
   // TRecorder1
   typedef TEvMapReco<TMap1, TFileDat1>  TRecorder1;

   //==============================================
   // TRecorder2
   typedef TMapMaker_2k2d<TMap1, TMap2>  TMapMakerImpl;
   typedef TSimpRecoDeta<TMapMakerImpl>  TRecorder2;
};
   
/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvMap_2k_File_h_)
