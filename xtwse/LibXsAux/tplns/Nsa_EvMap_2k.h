/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvMap_2k_h_)
#define _Nsa_EvMap_2k_h_
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
      TCommInn::rr_key mutable  RoomKey_;
      ...
      
      Nsa_EvMap_Default_Operator_Equal(SDat2)
   };
*/

/////////////////////////////////////////////////////////////////////////////
// class TEvMap2k
/////////////////////////////////////////////////////////////////////////////
template <class KeyT1, class KeyT2, class DatT2 = SEvMapDatDefInn, class DatT1 = SEvMapDatNull>
class TEvMap2k
{
public:
	//=======================================
	// Base type
   typedef KeyT1  TKey1;
   typedef KeyT2  TKey2;
   typedef DatT1  TDat1;
   typedef DatT2  TDat2;

	//=======================================
   // class TMap2
   class TMap2 : public ev_map<TKey2, TDat2>
   {
      TKey1  Key1_;

   public:
      typedef          ev_map<TKey2, TDat2>    inherited;
      typedef typename inherited::mapped_type  mapped_type;

   public:
      inline K_mf(void)         set_master_key (const TKey1& Key1)  { Key1_ = Key1; }
      inline K_mf(const TKey1&) get_master_key () const             { return Key1_; }

      inline K_mf(TMap2*) GetContainer (TMap2*)  { return this; }
      inline K_mf(TMap2*) GetContainer ()        { return this; }
   };

   //==============================================
   // struct TDatExt1
   struct TDatExt1 : TDat1
   {
      typedef TDat1  inherited;
      typedef TDat1  TDat;
      typedef TMap2  TMap;

      TMap2  Map2_;

      inline K_ctor TDatExt1 ()
         : inherited()
      {}

      inline K_ctor TDatExt1 (const TDat1& dat1)
         : inherited(dat1)
      {}

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
   typedef typename TMap2::iterator        TMapI2;
   typedef typename TMap2::const_iterator  TMapCi2;
   typedef typename TMap2::updater         TMapU2;

   //=======================================
	// 為了和其它的 Hnd 達成介面一致
	//typedef TEvMapKeyNull  TKey3;
	//typedef TEvMapDatNull  TDat3;
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvMap_2k_h_)
