/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvMap_3k_Hnd_h_)
#define _Nsa_EvMap_3k_Hnd_h_
//---------------------------------------------------------------------------
#include "Nsa_EvMap_0Hnd.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TEvMap3kHnd
/////////////////////////////////////////////////////////////////////////////
template <class RsTypeT>
class TEvMap3kHnd
{
   typedef RsTypeT               TRsType;
   typedef TEvMap3kHnd<RsTypeT>  TThisType;

public:
   //==============================================
   // 基本型態
   typedef typename TRsType::TMap1     TMap1;
   typedef typename TRsType::TKey1     TKey1;
   typedef typename TRsType::TDat1     TDat1;
   typedef typename TRsType::TDatExt1  TDatExt1;
   //----------------------------------------------
   typedef typename TRsType::TMap2     TMap2;
   typedef typename TRsType::TKey2     TKey2;
   typedef typename TRsType::TDat2     TDat2;
   typedef typename TRsType::TDatExt2  TDatExt2;
   //----------------------------------------------
   typedef typename TRsType::TMap3     TMap3;
   typedef typename TRsType::TKey3     TKey3;
   typedef typename TRsType::TDat3     TDat3;

   //==============================================
   // const data ptr
   typedef const TDat1*     TDatCp1;
   typedef const TDatExt1*  TDatExtCp1;
   typedef const TKey2*     TKeyCp2;
   typedef const TDat2*     TDatCp2;

   //==============================================
   // iterator
   typedef typename TMap1::iterator  TMapI1;
   typedef typename TMap2::iterator  TMapI2;

   //==============================================
   // TEvMapHnd
   typedef TEvMapHnd<TMap1>  TMapHnd1;
   typedef TEvMapHnd<TMap2>  TMapHnd2;
   typedef TEvMapHnd<TMap3>  TMapHnd3;

   typedef typename TMapHnd3::TMapIns  TMapIns3;

protected:
   TMapHnd1  MapHnd1_;
   TMapHnd2  MapHnd2_;
   TMapHnd3  MapHnd3_;

public:
   //==============================================
   // constructor/destructor
   K_ctor TEvMap3kHnd (                                                );
   K_ctor TEvMap3kHnd (TMap1*                                          );
   K_ctor TEvMap3kHnd (TMap1*, const TKey1&                            );
   K_ctor TEvMap3kHnd (TMap1*, const TKey1&, const TKey2&              );
   K_ctor TEvMap3kHnd (TMap1*, const TKey1&, const TKey2&, const TKey3&);
   K_ctor TEvMap3kHnd (const TThisType&                                );

   //==============================================
   // FindMap
   K_mf(TMap2*) FindMap2 ();
   K_mf(TMap3*) FindMap3 ();

   //==============================================
   // GetMap
   K_mf(TMap2*) GetMap2 ();
   K_mf(TMap3*) GetMap3 ();
   
   //==============================================
   // Insert
   K_mf(bool) Insert3 (const TKey3&, const TDat3&);

   //==============================================
   // GetDat
   K_mf(TDat3&) GetDat3 ();

   /*

   //==============================================
   // FindDat
   K_mf(TDatCp2) FindDat2 ();
   
   //==============================================
   // BeginUpdate
   K_mf(TDat2&) BeginUpdate2 ();
   //----------------------------------------------
   inline K_mf(TDat1&) BeginUpdate1_Inc (bool useLast = true)  { return MapHnd1_.BeginUpdate_Inc(useLast); }
          K_mf(TDat2&) BeginUpdate2_Inc (bool useLast = true);


   //==============================================
   // inline
   // 使用時必須確定 Map2 是存在的
   inline K_mf(void) SetIter2 (TMapI2& iter)  { MapHnd2_.SetIter(iter); }

   inline K_mf(bool) MoveFirst2 ()  { return MapHnd2_.MoveFirst(); }
   inline K_mf(bool) MoveNext2  ()  { return MapHnd2_.MoveNext (); }
   
   // 我非常肯定這個資料存在, 所以就直接取資料, 不用任何檢查囉
   inline K_mf(const TDat2&) GetCurrDat2 ()  { return MapHnd2_.GetCurrDat(); }
   */

/*

   //==============================================
   // GetMap
   inline K_mf(TMap1*)  GetMap1  ()  { return MapHnd1_ .GetMap(); }
   inline K_mf(TMap12*) GetMap12 ()  { return MapHnd12_.GetMap(); }
          

   //==============================================
   // FindDat
   inline K_mf(TDatCp1) FindDat1 ()  { return MapHnd1_.FindDat(); }
          K_mf(TDatCp2) FindDat2 ();

   //==============================================
   // FindPrev
   inline K_mf(TKeyCp2) FindPrev_Key2 ()  { return MapHnd2_.FindPrev_Key(); }
   inline K_mf(TDatCp2) FindPrev_Dat2 ()  { return MapHnd2_.FindPrev_Dat(); }

   //=======================================
   // FetchDat
   inline K_mf(TDat1&) FetchDat1 ()  { return MapHnd1_.FetchDat(); }
          K_mf(TDat2&) FetchDat2 ();

   //=======================================
   // BeginUpdate
   K_mf(TDat1&) BeginUpdate1 (              bool SaveOld = false);
   K_mf(TDat1&) BeginUpdate1 (const TKey1&, bool SaveOld = false);

   
   K_mf(TDat2&) BeginUpdate2 (              const TKey2&, bool SaveOld = false);
   K_mf(TDat2&) BeginUpdate2 (const TKey1&, const TKey2&, bool SaveOld = false);



   //=======================================
   // EraseDat
   K_mf(bool) EraseDat2 (            );
   K_mf(bool) EraseDat2 (const TKey2&);

   //=======================================
   // EndUpdate
   inline K_mf(void) EndUpdate1 ()  { MapHnd1_.EndUpdate(); }

   //==============================================
   // IsNewKey1
   inline K_mf(bool) IsNewKey1 ()  { return MapHnd1_.IsNewKey(); }

   //=======================================
   // GetKey
   inline K_mf(const TKey1&) GetKey1 () const  { return MapHnd1_.GetKey(); }
   inline K_mf(const TKey2&) GetKey2 () const  { return MapHnd2_.GetKey(); }

   //=======================================
   // inline other
   inline K_mf(void) SetSyncList (TSyncList SyncList)  { HndNotify_.SetSyncList(SyncList); }
*/
};
/////////////////////////////////////////////////////////////////////////////
// class TEvMap3kHnd impl
/////////////////////////////////////////////////////////////////////////////
#define Nsa_TEvMap3kHnd_Impl(return_type)                                  \
   template <class RsTypeT>                                                \
   K_mf(return_type) TEvMap3kHnd<RsTypeT>::                                \
//---------------------------------------------------------------------------

#if defined(__BORLANDC__)
   #define Nsa_TEvMap3kHnd_Impl_S(return_type)                             \
      template <class RsTypeT>                                             \
      TEvMap3kHnd<RsTypeT>::return_type __fastcall                         \
      TEvMap3kHnd<RsTypeT>::                                               \
   //------------------------------------------------------------------------
#else
   #define Nsa_TEvMap3kHnd_Impl_S(return_type)                             \
      template <class RsTypeT>                                             \
      typename TEvMap3kHnd<RsTypeT>::return_type                           \
      TEvMap3kHnd<RsTypeT>::                                               \
   //------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

template <class RsTypeT>
K_ctor TEvMap3kHnd<RsTypeT>::TEvMap3kHnd ()
{
}
//---------------------------------------------------------------------------

template <class RsTypeT>
K_ctor TEvMap3kHnd<RsTypeT>::TEvMap3kHnd (TMap1* map1)
   : MapHnd1_(map1)
{
}
//---------------------------------------------------------------------------

template <class RsTypeT>
K_ctor TEvMap3kHnd<RsTypeT>::TEvMap3kHnd (TMap1* map1, const TKey1& key1)
   : MapHnd1_(map1, key1)
{
}
//---------------------------------------------------------------------------

template <class RsTypeT>
K_ctor TEvMap3kHnd<RsTypeT>::TEvMap3kHnd (TMap1* map1, const TKey1& key1, const TKey2& key2)
   : MapHnd1_(map1, key1)
   , MapHnd2_(NULL, key2)
{
}
//---------------------------------------------------------------------------

template <class RsTypeT>
K_ctor TEvMap3kHnd<RsTypeT>::TEvMap3kHnd (TMap1* map1, const TKey1& key1, const TKey2& key2, const TKey3& key3)
   : MapHnd1_(map1, key1)
   , MapHnd2_(NULL, key2)
   , MapHnd3_(NULL, key3)
{
}
//---------------------------------------------------------------------------

template <class RsTypeT>
K_ctor TEvMap3kHnd<RsTypeT>::TEvMap3kHnd (const TThisType& a)
   : MapHnd1_(a.MapHnd1_)
   , MapHnd2_(a.MapHnd2_)
   , MapHnd3_(a.MapHnd3_)
{
}
//---------------------------------------------------------------------------

Nsa_TEvMap3kHnd_Impl_S(TMap2*) FindMap2 ()
{
   TMap2*    map2    = NULL;
   TDatExt1* datExt1 = const_cast<TDatExt1*>(MapHnd1_.FindDat());

   if (datExt1) {
      map2 = datExt1->GetContainer(map2);
   }

   MapHnd2_.SetMap(map2);
   
   return map2;
}
//---------------------------------------------------------------------------

Nsa_TEvMap3kHnd_Impl_S(TMap3*) FindMap3 ()
{
   if (!FindMap2())
      return NULL;

   TMap3*    map3    = NULL;
   TDatExt2* datExt2 = const_cast<TDatExt2*>(MapHnd2_.FindDat());
   
   if (datExt2) {
      map3 = datExt2->GetContainer(map3);
   }

   MapHnd3_.SetMap(map3);
   
   return map3;
}
//---------------------------------------------------------------------------

Nsa_TEvMap3kHnd_Impl_S(TMap2*) GetMap2 ()
{
   TDatExt1& datExt1 = MapHnd1_.GetDat();
   TMap2*    map2    = datExt1.GetContainer(map2);

   MapHnd2_.SetMap(map2);

   return map2;
}
//---------------------------------------------------------------------------

Nsa_TEvMap3kHnd_Impl_S(TMap3*) GetMap3 ()
{
   GetMap2();

   TDatExt2& datExt2 = MapHnd2_.GetDat();
   TMap3*    map3    = datExt2.GetContainer(map3);

   MapHnd3_.SetMap(map3);

   return map3;
}
//---------------------------------------------------------------------------

Nsa_TEvMap3kHnd_Impl(bool) Insert3 (const TKey3& key3, const TDat3& dat3)
{
   GetMap3();
   return MapHnd3_.Insert(key3, dat3);
}
//---------------------------------------------------------------------------

Nsa_TEvMap3kHnd_Impl_S(TDat3&) GetDat3 ()
{
   GetMap3();
   
   return MapHnd3_.GetDat();
}
//---------------------------------------------------------------------------


/*
Nsa_TEvMap2kHnd_Impl_S(TDatCp2) FindDat2 ()
{
   TMap2* map2 = FindMap2();
   
   if (map2 == NULL)
      return NULL;
      
   return MapHnd2_.FindDat(map2);
}
//---------------------------------------------------------------------------

Nsa_TEvMap2kHnd_Impl_S(TDat2&) BeginUpdate2 ()
{
   TMap2* map2 = GetMap2();

   return MapHnd2_.BeginUpdate(map2);
}
//---------------------------------------------------------------------------

Nsa_TEvMap2kHnd_Impl_S(TDat2&) BeginUpdate2_Inc (bool useLast)
{
   TMap2* map2 = GetMap2();

   return MapHnd2_.BeginUpdate_Inc(map2, useLast);
}
//---------------------------------------------------------------------------
*/




/*











TEvMap2k2dHnd_Impl_S(TDat1&) BeginUpdate1 (bool SaveOld)
{
   TDat1& dat1 = MapHnd1_.BeginUpdate(SaveOld);

   // 在取 map1 時, 必須也要確認 map2 的 key 存在
   if (MapHnd1_.IsNewKey()) {
      MapHnd12_.FetchDat();
   }

   return dat1;
}
//---------------------------------------------------------------------------

TEvMap2k2dHnd_Impl_S(TDat1&) BeginUpdate1 (const TKey1& Key1, bool SaveOld)
{
   MapHnd1_ .SetKey(Key1);
   MapHnd12_.SetKey(Key1);

   return BeginUpdate1(SaveOld);
}
//---------------------------------------------------------------------------

TEvMap2k2dHnd_Impl_S(TDat2&) FetchDat2 ()
{
   TDat12& Dat12 = MapHnd12_.FetchDat();
   TMap2*  Map2  = &Dat12;

   if (MapHnd12_.IsNewKey()) {
      MapHnd1_.FetchDat();
   }

   return MapHnd2_.FetchDat(Map2);
}
//---------------------------------------------------------------------------

TEvMap2k2dHnd_Impl_S(TDat2&) BeginUpdate2 (const TKey2& Key2, bool SaveOld)
{
   MapHnd2_.SetKey(Key2);

   return BeginUpdate2(SaveOld);
}
//---------------------------------------------------------------------------

TEvMap2k2dHnd_Impl_S(TDat2&) BeginUpdate2 (const TKey1& Key1, const TKey2& Key2, bool SaveOld)
{
   MapHnd1_ .Assign(Key1);
   MapHnd12_.Assign(Key1);
   MapHnd2 _.Assign(Key2);

   return BeginUpdate2(SaveOld);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// EraseDat
TEvMap2k2dHnd_Impl(bool) EraseDat2 ()
{
   // 這邊利用 FindDat2 來將 MapHnd2_ 所有資料湊齊
   TDatCp2 dat2 = FindDat2();
   if (dat2 == NULL) {
      return false;
   }
   return MapHnd2_.EraseDat();
}
//---------------------------------------------------------------------------

TEvMap2k2dHnd_Impl(bool) EraseDat2 (const TKey2& key2)
{
   MapHnd2_.SetKey(key2);
   return EraseDat2();
}
//---------------------------------------------------------------------------


*/
/*





   // FindRecs
   K_mf(TMap2*) FindRecs2 ();
   K_mf(TMap2*) FindRecs2 (const TKey1& Key1);

   K_mf(TMap3*) FindRecs3 (const TKey1& Key1, const TKey2& Key2);
   K_mf(TMap3*) FindRecs3 (                   const TKey2& Key2);
   K_mf(TMap3*) FindRecs3 (                                    );

   // GetRecs
   K_mf(TMap2*) GetRecs2 (const TKey1& Key1);
   K_mf(TMap2*) GetRecs2 (                 );

   //=======================================
   // GetMap
   K_mf(TMap3*) GetMap3 (const TKey1& Key1, const TKey2& Key2);
   K_mf(TMap3*) GetMap3 (                                    );


   //=======================================
   // FindDat
   K_mf(TDatCp1) FindDat1 (const TKey1& Key1);
   K_mf(TDatCp1) FindDat1 ();

   K_mf(TDatCp2) FindDat2 (const TKey1&, const TKey2&);

   K_mf(TDatCp3) FindDat3 (const TKey1&, const TKey2&, const TKey3&);
   K_mf(TDatCp3) FindDat3 (                            const TKey3&);
   K_mf(TDatCp3) FindDat3 ();


   //=======================================
   // GetDat
   K_mf(TDatExt1&) GetDatExt1 (const TKey1& Key1);
   K_mf(TDatExt1&) GetDatExt1 ();
   
   K_mf(TDat2&) GetDat2 (const TKey1& Key1, const TKey2& Key2);
   K_mf(TDat2&) GetDat2 (                   const TKey2& Key2);
   K_mf(TDat2&) GetDat2 (                                    );

   K_mf(TDat2&) GetDat2_Inc (const TKey1& Key1);
   K_mf(TDat2&) GetDat2_Inc (                 );
   
   K_mf(TDat3&) GetDat3 (const TKey1& Key1, const TKey2& Key2, const TKey3& Key3);
   K_mf(TDat3&) GetDat3 (                   const TKey2& Key2, const TKey3& Key3);
   K_mf(TDat3&) GetDat3 (                                      const TKey3& Key3);
   K_mf(TDat3&) GetDat3 (                                                       );


   //=======================================
   // IsNewKey1
   inline K_mf(bool) IsNewKey1 ()  { return MapHnd1_.IsNewKey(); }
          K_mf(bool) IsNewKey2 ();
          K_mf(bool) IsNewKey3 ();

   inline K_mf(TMap1*) GetRecs1 ()  { return MapHnd1_.GetMap(); }

   inline K_mf(const TKey1&) GetKey1 () const  { return MapHnd1_.GetKey(); }
   inline K_mf(const TKey2&) GetKey2 () const  { return MapHnd2_.GetKey(); }
   inline K_mf(const TKey3&) GetKey3 () const  { return MapHnd3_.GetKey(); }





TEvMap3k2dHnd_Impl(bool) IsNewKey2 ()
{
   if (MapHnd1_.IsNewKey()) {
      return true;
   }

   TDatExt1* DatExt1 = (TDatExt1*)MapHnd1_.FindDat();
   TMap2*    Map2    = DatExt1->GetContainer(Map2);

   if (MapHnd2_.IsNewKey(Map2)) {
      return true;
   }
   return false;
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl(bool) IsNewKey3 ()
{
   if (MapHnd1_.IsNewKey() || MapHnd23_.IsNewKey() || MapHnd3_.IsNewKey()) {
      return true;
   }
  
   return false;
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TMap2*) FindRecs2 ()
{
   TDatExt1* DatExt1 = (TDatExt1*)MapHnd1_.FindDat();
   if (DatExt1 == NULL) {
      return NULL;
   }

   TMap2* Map2 = DatExt1->GetContainer(Map2);
   return Map2;
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TMap2*) FindRecs2 (const TKey1& Key1)
{
   MapHnd1_.Assign(Key1);
   return FindRecs2();
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TMap3*) FindRecs3 ()
{
   TDatExt1* DatExt1 = (TDatExt1*)MapHnd1_.FindDat();
   if (DatExt1 == NULL) {
      return NULL;
   }

   TMap23* Map23 = DatExt1->GetContainer(Map23);
   TDat23* Dat23 = (TDat23*)MapHnd23_.FindDat(Map23);
   if (Dat23 == NULL) {
      return NULL;
   }

   return Dat23;
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TMap3*) FindRecs3 (const TKey2& Key2)
{
   MapHnd23_.Assign(Key2);
   return FindRecs3();
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TMap3*) FindRecs3 (const TKey1& Key1, const TKey2& Key2)
{
   MapHnd1_ .Assign(Key1);
   MapHnd23_.Assign(Key2);
   return FindRecs3();
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TMap2*) GetRecs2 ()
{
   TDatExt1& DatExt1 = MapHnd1_.GetDat();

   TMap2* Map2 = DatExt1.GetContainer(Map2);
   return Map2;
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TMap2*) GetRecs2 (const TKey1& Key1)
{
   MapHnd1_ .Assign(Key1);
   return GetRecs2();
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TMap3*) GetMap3 ()
{
   TDatExt1& DatExt1 = MapHnd1_.FetchDat();
   TMap23*   Map23   = DatExt1.GetContainer(Map23);
   TDat23&   Dat23   = MapHnd23_.FetchDat(Map23);
   TMap3*    Map3    = &Dat23;

   if (MapHnd23_.IsNewKey()) {
      TMap2* Map2 = DatExt1.GetContainer(Map2);

      MapHnd2_.FetchDat(Map2);
   }

   return Map3;
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TMap3*) GetMap3 (const TKey1& Key1, const TKey2& Key2)
{
   MapHnd1_ .Assign(Key1);
   MapHnd2_ .Assign(Key2);
   MapHnd23_.Assign(Key2);

   return GetMap3();
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TDatCp1) FindDat1 ()
{
   TDatExtCp1 DatExt1 = MapHnd1_.FindDat();
   TDatCp1    Dat1    = DatExt1->GetDatPtr();

   return Dat1;
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TDatCp1) FindDat1 (const TKey1& Key1)
{
   MapHnd1_.Assign(Key1);
   return FindDat1();
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TDatCp2) FindDat2 (const TKey1& Key1, const TKey2& Key2)
{
   MapHnd1_.Assign(Key1);
   MapHnd2_.Assign(Key2);

   return FindDat2();
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TDatCp3) FindDat3 ()
{
   TDatExt1* DatExt1 = (TDatExt1*)MapHnd1_.FindDat();
   if (DatExt1 == NULL) {
      return NULL;
   }

   TMap23* Map23 = DatExt1->GetContainer(Map23);
   TDat23* Dat23 = (TDat23*)MapHnd23_.FindDat(Map23);
   if (Dat23 == NULL) {
      return NULL;
   }

   TMap3*  Map3 = Dat23;
   TDatCp3 Dat3 = MapHnd3_.FindDat(Map3);

   return Dat3;
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TDatCp3) FindDat3 (const TKey3& Key3)
{
   MapHnd3_.Assign(Key3);

   return FindDat3();
}
//---------------------------------------------------------------------------
   
TEvMap3k2dHnd_Impl_S(TDatCp3) FindDat3 (const TKey1& Key1, const TKey2& Key2, const TKey3& Key3)
{
   MapHnd1_ .Assign(Key1);
   MapHnd23_.Assign(Key2);
   MapHnd3_ .Assign(Key3);

   return FindDat3();
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TDatExt1&) GetDatExt1 ()
{
   return MapHnd1_.GetDat();
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TDatExt1&) GetDatExt1 (const TKey1& Key1)
{
   MapHnd1_.Assign(Key1);
   return MapHnd1_.GetDat();
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TDat2&) GetDat2 (const TKey1& Key1, const TKey2& Key2)
{
   MapHnd1_.Assign(Key1);

   return GetDat2(Key2);
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TDat2&) GetDat2 (const TKey2& Key2)
{
   MapHnd2_ .Assign(Key2);
   MapHnd23_.Assign(Key2);

   return GetDat2();
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TDat2&) GetDat2 ()
{
   TDatExt1& DatExt1 = MapHnd1_.GetDat();
   TMap2*    Map2    = DatExt1.GetContainer(Map2);
   TDat2&    Dat2    = MapHnd2_.GetDat(Map2);

   if (MapHnd2_.IsNewKey()) {
      TMap23* Map23 = DatExt1.GetContainer(Map23);

      MapHnd23_.GetDat(Map23);
   }

   return Dat2;
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TDat2&) GetDat2_Inc (const TKey1& Key1)
{
   MapHnd1_.Assign(Key1);

   return GetDat2_Inc();
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TDat2&) GetDat2_Inc ()
{
   TDatExt1& DatExt1 = MapHnd1_.GetDat();
   TMap2*    Map2    = DatExt1.GetContainer(Map2);
   TKey2     Key2(Map2->size() + 1);

   return GetDat2(Key2);
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TDat3&) GetDat3 (const TKey3& Key3)
{
   MapHnd3_.Assign(Key3);

   return GetDat3();
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TDat3&) GetDat3 (const TKey2& Key2, const TKey3& Key3)
{
   MapHnd2_ .Assign(Key2);
   MapHnd23_.Assign(Key2);

   return GetDat3(Key3);
}
//---------------------------------------------------------------------------

TEvMap3k2dHnd_Impl_S(TDat3&) GetDat3 (const TKey1& Key1, const TKey2& Key2, const TKey3& Key3)
{
   MapHnd1_.Assign(Key1);

   return GetDat3(Key2, Key3);
}
//---------------------------------------------------------------------------
*/

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvMap_3k_Hnd_h_)
