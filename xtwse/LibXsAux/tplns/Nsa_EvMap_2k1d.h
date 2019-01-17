/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvMap_2k1d_h_)
#define _Nsa_EvMap_2k1d_h_
//---------------------------------------------------------------------------
#include "Nsa_EvMap.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/*
   struct SDat2
   {
      TCommInn::rr_key mutable  RoomKey_;
      ...
      
      Nsa_EvMap_Default_Operator_Equal(SDat2)
   };
*/

/////////////////////////////////////////////////////////////////////////////
// class TEvMap2k1d
/////////////////////////////////////////////////////////////////////////////
template <class KeyT1, class KeyT2, class DatT2 = SEvMapNullDatInn>
class TEvMap2k1d
{
public:
	//=======================================
	// Base type
   typedef KeyT1  TKey1;
   typedef KeyT2  TKey2;
   typedef DatT2  TDat2;

	//=======================================
   // class TMap2
   class TMap2 : public Kstl::ev_map<TKey2, TDat2>
   {
      typedef Kstl::ev_map<TKey2, TDat2>  inherited;

      TKey1  Key1_;

   public:
      inline K_mf(void) set_master_key (const TKey1& Key1)  { Key1_ = Key1; }

      inline K_mf(TKey1)  get_master_key ()       const  { return Key1_; }
      inline K_mf(TMap2*) GetContainer   (TMap2*)        { return this;  }
   };

   //=======================================
   // class TMap1
   typedef Kstl::ev_map<TKey1, TMap2>  TMap1;
   typedef TMap2                       TDat1;

   //=======================================
   // Map type
   typedef TMap1::iterator        TMapI1;
   typedef TMap2::iterator        TMapI2;
   typedef TMap1::const_iterator  TMapCi1;
   typedef TMap2::const_iterator  TMapCi2;
   typedef TMap1::updater         TMapU1;
   typedef TMap2::updater         TMapU2;

   //=======================================
	// ���F�M�䥦�� Hnd �F�������@�P
	typedef TEvMapKeyNull  TKey3;
	typedef TEvMapDatNull  TDat3;
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvMap_2k1d_h_)
