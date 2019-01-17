/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvMap_3k_h_)
#define _Nsa_EvMap_3k_h_
//---------------------------------------------------------------------------
#include "Nsa_EvMap_2k.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;
namespace Nsa  {

/*
   struct SDat1
   {
      ...
   };

   struct SDat2
   {
      ...
   };

   struct SDat3
   {
      TCommInn::rr_key mutable  RoomKey_;
      ...
      
      Nsa_EvMap_Default_Operator_Equal(SDat3)
   };
*/

/////////////////////////////////////////////////////////////////////////////
// class TEvMap3k
/////////////////////////////////////////////////////////////////////////////
template <class KeyT1, class KeyT2, class KeyT3, class DatT3 = SEvMapDatDefInn,
                                                 class DatT2 = SEvMapDatNull,
                                                 class DatT1 = SEvMapDatNull>
class TEvMap3k
{
public:
	//==============================================
	// Base type
   typedef KeyT1  TKey1;
   typedef KeyT2  TKey2;
   typedef KeyT3  TKey3;
   typedef DatT1  TDat1;
   typedef DatT2  TDat2;
   typedef DatT3  TDat3;

	//==============================================
   // TDetailMap
   typedef TEvMap2k<TKey2, TKey3, TDat3, TDat2>  TDetailMap;
   //----------------------------------------------
   typedef typename TDetailMap::TMap1     TMap2;
   typedef typename TDetailMap::TDatExt1  TDatExt2;
   //----------------------------------------------
   typedef typename TDetailMap::TMap2     TMap3;
   /*
   typedef TDetailMap::TMapI1    TMapI2;
   typedef TDetailMap::TMapCi1   TMapCi2;
   typedef TDetailMap::TMapU1    TMapU2;
   typedef TDetailMap::TMap12    TMap23;
   typedef TDetailMap::TMapI12   TMapI23;
   typedef TDetailMap::TMapCi12  TMapCi23;
   typedef TDetailMap::TMapU12   TMapU23;
   typedef TDetailMap::TMapI2    TMapI3;
   typedef TDetailMap::TMapCi2   TMapCi3;
   typedef TDetailMap::TMapU2    TMapU3;
   */

	//==============================================
   // TDatExt1
   struct TDatExt1 : public TDat1
   {
      typedef TDat1  inherited;
      typedef TDat1  TDat;
      typedef TMap2  TMap;

      TMap2  Map2_;

      inline K_ctor TDatExt1 ()                  : inherited()      {}
      inline K_ctor TDatExt1 (const TDat1& dat1) : inherited(dat1)  {}

      inline K_mf(      TDat1&) GetDat1 ()        { return *(static_cast<TDat1*>(this)); }
      inline K_mf(const TDat1&) GetDat1 () const  { return *(static_cast<TDat1*>(this)); }

      inline K_mf(TMap2*) GetContainer ()  		 { return &Map2_; }
      inline K_mf(TMap2*) GetContainer (TMap2*)  { return &Map2_; }
   };

   //==============================================
   // TMap1
   typedef ev_map<TKey1, TDatExt1>  TMap1;

   //==============================================
   // iterator, updater
   typedef typename TMap1::iterator        TMapI1;
   typedef typename TMap1::const_iterator  TMapCi1;
   typedef typename TMap1::updater         TMapU1;
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvMap_3k_h_)

