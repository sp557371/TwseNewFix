/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvMap_2k2d_h_)
#define _Nsa_EvMap_2k2d_h_
//---------------------------------------------------------------------------
#include "Nsa_EvMap.h"

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
      Nse2::TNseInn::rr_key mutable  RoomKey_;
      ...

      Nse2_EvMap_Default_Operator_Equal(SDat2)
   };
*/

/////////////////////////////////////////////////////////////////////////////
// class TEvMap2k2d
/////////////////////////////////////////////////////////////////////////////
template <class KeyT1, class KeyT2, class DatT1, class DatT2>
class TEvMap2k2d
{
public:
   typedef KeyT1  TKey1;
   typedef KeyT2  TKey2;
   typedef DatT1  TDat1;
   typedef DatT2  TDat2;

   //==============================================
   // class TMap2
   class TMap2 : public Kstl::ev_map<TKey2, TDat2>
   {
      TKey1  Key1_;

   public:
      typedef Kstl::ev_map<TKey2, TDat2>  inherited;

      inline K_mf(void) set_master_key (const TKey1& Key1)  { Key1_ = Key1; }
      
      inline K_mf(TKey1)  get_master_key ()       const  { return Key1_; }
      inline K_mf(TMap2*) GetContainer   (TMap2*)        { return this;  }
      inline K_mf(TMap2*) GetContainer   ()              { return this;  }
   };

   //==============================================
   // struct TDatExt1
   struct TDatExt1
   {
      TDat1  Dat1_;
      TMap2  Map2_;
   };

   //==============================================
   // TMap1
   typedef ev_map<TKey1, TDatExt1>  TMap1;

   //==============================================
   // iterator, updater
   typedef TMap1::iterator            TMapI1;
   typedef TMap1::const_iterator      TMapCi1;
   typedef TMap1::updater             TMapU1;
   typedef TMap2::iterator            TMapI2;
   typedef TMap2::const_iterator      TMapCi2;
   typedef TMap2::updater             TMapU2;

   //==============================================
	// 為了和其它的 Hnd 達成介面一致
	typedef TEvMapKeyNull  TKey3;
	typedef TEvMapDatNull  TDat3;
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvMap_2k2d_h_)
