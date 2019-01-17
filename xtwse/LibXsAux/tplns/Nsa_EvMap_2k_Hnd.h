/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvMap_2k_Hnd_h_)
#define _Nsa_EvMap_2k_Hnd_h_
//---------------------------------------------------------------------------
#include "Nsa_EvMap_0Hnd.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TEvMap2k2dHnd
/////////////////////////////////////////////////////////////////////////////
template <class MapT>
class TEvMap2kHnd
{
   typedef TEvMap2kHnd<MapT>  TThisType;

public:
   //==============================================
   // 基本型態
   typedef MapT  TMap1;
   
   typedef typename TMap1::key_type     TKey1;
   typedef typename TMap1::mapped_type  TDatExt1;
   typedef typename TDatExt1::TDat      TDat1;
   typedef typename TDatExt1::TMap      TMap2;
   typedef typename TMap2::key_type     TKey2;
   typedef typename TMap2::mapped_type  TDat2;

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

protected:
   TMapHnd1  MapHnd1_;
   TMapHnd2  MapHnd2_;

public:
   //==============================================
   // constructor/destructor
   K_ctor TEvMap2kHnd (                                  );
   K_ctor TEvMap2kHnd (TMap1*                            );
   K_ctor TEvMap2kHnd (TMap1*, const TKey1&              );
   K_ctor TEvMap2kHnd (TMap1*, const TKey1&, const TKey2&);
   K_ctor TEvMap2kHnd (const TThisType&                  );

   //==============================================
   // FindMap
   K_mf(TMap2*) FindMap2 ();

   //==============================================
   // FindDat
   inline K_mf(TDatCp1) FindDat1 ()  { return MapHnd1_.FindDat(); }

          K_mf(TDatCp2) FindDat2 (            );
          K_mf(TDatCp2) FindDat2 (const TKey2&);

   //==============================================
   // GetMap
   inline K_mf(TMap1*) GetMap1 ()  { return MapHnd1_.GetMap(); }
          K_mf(TMap2*) GetMap2 ();

   //==============================================
   // SetKey2
   inline K_mf(void) SetKey2 (const TKey2& key2)  { MapHnd2_.SetKey(key2); }

   //==============================================
   // GetKey
   inline K_mf(const TKey1&) GetKey1 () const  { return MapHnd1_.GetKey(); }
   inline K_mf(const TKey2&) GetKey2 () const  { return MapHnd2_.GetKey(); }

   //==============================================
   // GetDat
   inline K_mf(TDat1&) GetDat1 ()  { return MapHnd1_.GetDat(); }
          K_mf(TDat2&) GetDat2 (bool check = true);

   //==============================================
   // GetIter
   inline K_mf(TMapI1) GetIter1 ()  { return MapHnd1_.GetIter(); }

   //==============================================
   // BeginUpdate
   inline K_mf(TDat1&) BeginUpdate1 ()  { return MapHnd1_.BeginUpdate(); }
          K_mf(TDat2&) BeginUpdate2 ();
          K_mf(TDat2&) BeginUpdate2 (const TKey2&);
   //----------------------------------------------
   inline K_mf(TDat1&) BeginUpdate1_Inc (bool useLast = true)  { return MapHnd1_.BeginUpdate_Inc(useLast); }
          K_mf(TDat2&) BeginUpdate2_Inc (bool useLast = true);

   //==============================================
   // Insert
   K_mf(bool) Insert1 (const TKey1&, const TDat1&);

   //=======================================
   // Erase
   K_mf(bool) Erase2 (            );
   K_mf(bool) Erase2 (const TKey2&);

   //==============================================
   // MoveFirst
   K_mf(bool) MoveFirst1 ();
   K_mf(bool) MoveFirst2 ();

   //==============================================
   // MoveNext
          K_mf(bool) MoveNext1 ();
   inline K_mf(bool) MoveNext2 ()  { return MapHnd2_.MoveNext(); }

   //==============================================
   // inline
   // 使用時必須確定 Map2 是存在的
   inline K_mf(void) SetIter2 (TMapI2& iter)  { MapHnd2_.SetIter(iter); }
  


/*

   //==============================================
   // GetMap
   inline K_mf(TMap1*)  GetMap1  ()  { return MapHnd1_ .GetMap(); }
   inline K_mf(TMap12*) GetMap12 ()  { return MapHnd12_.GetMap(); }
          

   //==============================================
   // FindPrev
   inline K_mf(TKeyCp2) FindPrev_Key2 ()  { return MapHnd2_.FindPrev_Key(); }
   inline K_mf(TDatCp2) FindPrev_Dat2 ()  { return MapHnd2_.FindPrev_Dat(); }

   //=======================================
   // FetchDat
          K_mf(TDat2&) FetchDat2 ();

   //=======================================
   // BeginUpdate
   K_mf(TDat1&) BeginUpdate1 (const TKey1&, bool SaveOld = false);

   
   K_mf(TDat2&) BeginUpdate2 (const TKey1&, const TKey2&, bool SaveOld = false);




   //=======================================
   // EndUpdate
   inline K_mf(void) EndUpdate1 ()  { MapHnd1_.EndUpdate(); }

   //==============================================
   // IsNewKey1
   inline K_mf(bool) IsNewKey1 ()  { return MapHnd1_.IsNewKey(); }

   //=======================================
   // inline other
   inline K_mf(void) SetSyncList (TSyncList SyncList)  { HndNotify_.SetSyncList(SyncList); }
*/
};
/////////////////////////////////////////////////////////////////////////////
// class TEvMap2kHnd impl
/////////////////////////////////////////////////////////////////////////////
#define Nsa_TEvMap2kHnd_Impl(return_type)                                  \
   template <class MapT>                                                   \
   K_mf(return_type) TEvMap2kHnd<MapT>::                                   \
//---------------------------------------------------------------------------

#if defined(__BORLANDC__)
   #define Nsa_TEvMap2kHnd_Impl_S(return_type)                             \
      template <class MapT>                                                \
      TEvMap2kHnd<MapT>::return_type __fastcall                            \
      TEvMap2kHnd<MapT>::                                                  \
   //------------------------------------------------------------------------
#else
   #define Nsa_TEvMap2kHnd_Impl_S(return_type)                             \
      template <class MapT>                                                \
      typename TEvMap2kHnd<MapT>::return_type                              \
      TEvMap2kHnd<MapT>::                                                  \
   //------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

template <class MapT>
K_ctor TEvMap2kHnd<MapT>::TEvMap2kHnd ()
{
}
//---------------------------------------------------------------------------

template <class MapT>
K_ctor TEvMap2kHnd<MapT>::TEvMap2kHnd (TMap1* map1)
   : MapHnd1_(map1)
{
}
//---------------------------------------------------------------------------

template <class MapT>
K_ctor TEvMap2kHnd<MapT>::TEvMap2kHnd (TMap1* map1, const TKey1& key1)
   : MapHnd1_(map1, key1)
{
}
//---------------------------------------------------------------------------

template <class MapT>
K_ctor TEvMap2kHnd<MapT>::TEvMap2kHnd (TMap1* map1, const TKey1& key1, const TKey2& key2)
   : MapHnd1_(map1, key1)
   , MapHnd2_(NULL, key2)
{
}
//---------------------------------------------------------------------------

template <class MapT>
K_ctor TEvMap2kHnd<MapT>::TEvMap2kHnd (const TThisType& a)
   : MapHnd1_(a.MapHnd1_)
   , MapHnd2_(a.MapHnd2_)
{
}
//---------------------------------------------------------------------------

Nsa_TEvMap2kHnd_Impl_S(TMap2*) FindMap2 ()
{
   TMap2*    map2    = NULL;
   TDatExt1* datExt1 = const_cast<TDatExt1*>(MapHnd1_.FindDat());

   if (datExt1 != NULL) {
      map2 = datExt1->GetContainer(map2);
   }

   MapHnd2_.SetMap(map2);

   return map2;
}
//---------------------------------------------------------------------------

Nsa_TEvMap2kHnd_Impl_S(TDatCp2) FindDat2 ()
{
   TMap2* map2 = FindMap2();
   
   if (map2 == NULL)
      return NULL;
      
   return MapHnd2_.FindDat(map2);
}
//---------------------------------------------------------------------------

Nsa_TEvMap2kHnd_Impl_S(TDatCp2) FindDat2 (const TKey2& key2)
{
   MapHnd2_.SetKey(key2);
   return FindDat2();
}
//---------------------------------------------------------------------------

Nsa_TEvMap2kHnd_Impl_S(TMap2*) GetMap2 ()
{
   TDatExt1& datExt1 = MapHnd1_.GetDat();
   TMap2*    map2    = datExt1.GetContainer(map2);

   MapHnd2_.SetMap(map2);

   return map2;
}
//---------------------------------------------------------------------------

Nsa_TEvMap2kHnd_Impl_S(TDat2&) GetDat2 (bool check)
{
   if (check) {
      TMap2* map2 = GetMap2();
      return MapHnd2_.GetDat(map2);
   }
   return MapHnd2_.GetDat();
}
//---------------------------------------------------------------------------

Nsa_TEvMap2kHnd_Impl_S(TDat2&) BeginUpdate2 ()
{
   TMap2* map2 = GetMap2();
   return MapHnd2_.BeginUpdate(map2);
}
//---------------------------------------------------------------------------

Nsa_TEvMap2kHnd_Impl_S(TDat2&) BeginUpdate2 (const TKey2& key2)
{
   MapHnd2_.SetKey(key2);
   return BeginUpdate2();
}
//---------------------------------------------------------------------------

Nsa_TEvMap2kHnd_Impl_S(TDat2&) BeginUpdate2_Inc (bool useLast)
{
   TMap2* map2 = GetMap2();
   return MapHnd2_.BeginUpdate_Inc(map2, useLast);
}
//---------------------------------------------------------------------------

Nsa_TEvMap2kHnd_Impl(bool) MoveFirst1 ()
{
   if (MapHnd1_.IsFirstKey())
      return true;

   MapHnd2_.Reset();
   return MapHnd1_.MoveFirst();
}
//---------------------------------------------------------------------------

Nsa_TEvMap2kHnd_Impl(bool) MoveFirst2 ()
{
   if (FindMap2() == NULL)
      return false;

   // 在 FindMap2 時, 會將 map2 設定到 MapHnd2_
   return MapHnd2_.MoveFirst();
}
//---------------------------------------------------------------------------

Nsa_TEvMap2kHnd_Impl(bool) MoveNext1 ()
{
   MapHnd2_.Reset();

   return MapHnd1_.MoveNext();
}
//---------------------------------------------------------------------------

Nsa_TEvMap2kHnd_Impl(bool) Insert1 (const TKey1& key1, const TDat1& dat1)
{
   TDatExt1 datExt1(dat1);
   return MapHnd1_.Insert(key1, datExt1);
}
//---------------------------------------------------------------------------

Nsa_TEvMap2kHnd_Impl(bool) Erase2 ()
{
   // 這邊利用 FindDat2 來將 MapHnd2_ 所有資料湊齊
   if (!FindDat2())
      return false;

   return MapHnd2_.Erase();
}
//---------------------------------------------------------------------------

Nsa_TEvMap2kHnd_Impl(bool) Erase2 (const TKey2& key2)
{
   MapHnd2_.SetKey(key2);
   return Erase2();
}
//---------------------------------------------------------------------------

/*
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

TEvMap2k2dHnd_Impl_S(TDat2&) BeginUpdate2 (const TKey1& Key1, const TKey2& Key2, bool SaveOld)
{
   MapHnd1_ .Assign(Key1);
   MapHnd12_.Assign(Key1);
   MapHnd2 _.Assign(Key2);

   return BeginUpdate2(SaveOld);
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

TEvMap3k2dHnd_Impl_S(TDat3&) GetDat3 ()
{
   // 先檢查 Map23 裡面有沒有資料
   TDatExt1& DatExt1 = MapHnd1_.GetDat();
   TMap23*   Map23   = DatExt1.GetContainer(Map23);
   TDat23&   Dat23   = MapHnd23_.GetDat(Map23);

   if (MapHnd23_.IsNewKey()) {
      TMap2* Map2 = DatExt1.GetContainer(Map2);

      MapHnd2_.GetDat(Map2);
   }

   return MapHnd3_.GetDat(&Dat23);
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
#endif // !defined(_Nsa_EvMap_2k_Hnd_h_)
