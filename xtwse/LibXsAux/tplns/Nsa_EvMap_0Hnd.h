/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvMap_0Hnd_h_)
#define _Nsa_EvMap_0Hnd_h_
//---------------------------------------------------------------------------
#include <bitset>

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TEvMapHnd
/////////////////////////////////////////////////////////////////////////////
template <class MapT>
class TEvMapHnd
{ 
   enum EStatusCode {
      esc_Find,
      esc_NewKey,
      esc_IterValid,
      esc_Total
   };
   typedef std::bitset<esc_Total>  TStsCode;

public:
   //==============================================
   // 基本宣告
   typedef TEvMapHnd<MapT>  TThisType;
   typedef MapT             TMap;

   //==============================================
   // 延伸宣告
   typedef typename TMap::key_type          TKey;
   typedef typename TMap::mapped_type       TDat;
   typedef typename TMap::value_type        TValueType;
   typedef typename TMap::iterator          TMapI;
   typedef typename TMap::updater           TMapU;
   typedef          const TKey*             TKeyCp;
   typedef          const TDat*             TDatCp;
   typedef typename std::pair<TMapI, bool>  TMapIns;

protected:
   TMap*     Map_;
   TKey      Key_;
   TMapI     MapIter_;
   TMapU     MapUpd_;
   TStsCode  StsCode_;
  
public:
   //==============================================
   // constructor/destructor
           K_ctor  TEvMapHnd ();
           K_ctor  TEvMapHnd (TMap*);
           K_ctor  TEvMapHnd (TMap*, const TKey&);
           K_ctor  TEvMapHnd (const TThisType&);
   virtual K_dtor ~TEvMapHnd ();

   //=======================================
   // Set
   K_mf(void) Reset   (bool incMap = true);
   K_mf(void) SetMap  (TMap*);
   K_mf(void) SetKey  (const TKey&);
   K_mf(void) SetIter (TMapI);

   //==============================================
   // FindDat
   K_mf(TDatCp) FindDat ();
   K_mf(TDatCp) FindDat (TMap*             );
   K_mf(TDatCp) FindDat (       const TKey&);
   K_mf(TDatCp) FindDat (TMap*, const TKey&);

   //==============================================
   // GetDat
   K_mf(TDat&) GetDat ();
   K_mf(TDat&) GetDat (TMap*);
   K_mf(TDat&) GetDat (TMap*, const TKey&);

   //==============================================
   // GetIter
   inline K_mf(TMapI) GetIter ()  { return Fetch(); }

   //==============================================
   // GetCurrDat
   // 我非常肯定這個資料存在, 所以就直接取資料, 不用任何檢查囉
   inline K_mf(const TDat&) GetCurrDat ()  { return MapIter_->second; }

   //=======================================
   // Erase
   K_mf(bool) Erase ();
   K_mf(bool) Erase (const TKey&);

   //==============================================
   // Insert
   K_mf(bool) Insert (const TKey&, const TDat&);

   //=======================================
   // GetKey
   inline K_mf(const TKey&) GetKey     () const  { return Key_; }
          K_mf(bool)        GetKey_Inc (TKey&, bool useLast = true);

   //==============================================
   // BeginUpdate
   K_mf(TDat&) BeginUpdate (                  );
   K_mf(TDat&) BeginUpdate (TMap*             );
   K_mf(TDat&) BeginUpdate (       const TKey&);
   K_mf(TDat&) BeginUpdate (TMap*, const TKey&);
   //--------------------------------------
   K_mf(TDat&) BeginUpdate_Inc (       bool useLast = true);
   K_mf(TDat&) BeginUpdate_Inc (TMap*, bool useLast = true);

   //==============================================
   // EndUpdate
   K_mf(void) EndUpdate (bool ResetSts = false);

   //=======================================
   // FindLast
   // 取得最後一筆的 key, iterator 不移動喔
   K_mf(TKeyCp) FindLast_Key ();

   //=======================================
   // FindPrev
   K_mf(bool)   FindPrev     (TMapI&);
   K_mf(TKeyCp) FindPrev_Key ();
   K_mf(TDatCp) FindPrev_Dat ();

   //=======================================
   // Move
   K_mf(bool) MoveFirst ();
   K_mf(bool) MoveNext  ();

   //=======================================
   // IsNewKey
   K_mf(bool) IsNewKey (                  );
   K_mf(bool) IsNewKey (TMap*             );
   K_mf(bool) IsNewKey (       const TKey&);
   K_mf(bool) IsNewKey (TMap*, const TKey&);
   //--------------------------------------
   K_mf(bool) IsFirstKey ();

   //=======================================
   // inline
   inline K_mf(TMap*) GetMap      ()        { return Map_; }
   inline K_mf(bool)  IsIterVlaid () const  { return StsCode_.test(esc_IterValid); }

   //=======================================
   // operator
   K_mf(TThisType&) operator = (const TThisType&);

protected:
   //=======================================
   // 基本操作
   K_mf(bool)  Find  ();
   K_mf(TMapI) Fetch ();
};
/////////////////////////////////////////////////////////////////////////////
// class TEvMapHnd impl
/////////////////////////////////////////////////////////////////////////////
#define Nsa_TEvMapHnd_Impl(return_type)                                    \
   template <class MapT>                                                   \
   K_mf(return_type) TEvMapHnd<MapT>::                                     \
//---------------------------------------------------------------------------

#if defined(__BORLANDC__)
   #define Nsa_TEvMapHnd_Impl_S(return_type)                               \
      template <class MapT>                                                \
      TEvMapHnd<MapT>::return_type __fastcall                              \
      TEvMapHnd<MapT>::                                                    \
   //------------------------------------------------------------------------
#else
   #define Nsa_TEvMapHnd_Impl_S(return_type)                               \
      template <class MapT>                                                \
      typename TEvMapHnd<MapT>::return_type                                \
      TEvMapHnd<MapT>::                                                    \
   //------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

template <class MapT>
K_ctor TEvMapHnd<MapT>::TEvMapHnd ()
   : Map_(NULL)
{
}
//---------------------------------------------------------------------------

template <class MapT>
K_ctor TEvMapHnd<MapT>::TEvMapHnd (TMap* map)
   : Map_(map)
{
}
//---------------------------------------------------------------------------

template <class MapT>
K_ctor TEvMapHnd<MapT>::TEvMapHnd (TMap* map, const TKey& key)
   : Map_(map)
   , Key_(key)
{
}
//---------------------------------------------------------------------------

template <class MapT>
K_ctor TEvMapHnd<MapT>::TEvMapHnd (const TThisType& rhs)
   : Map_    (rhs.Map_)
   , Key_    (rhs.Key_)
   , MapIter_(rhs.MapIter_)
   , MapUpd_ (rhs.MapUpd_)
   , StsCode_(rhs.StsCode_)
{
}
//---------------------------------------------------------------------------

template <class MapT>
K_ctor TEvMapHnd<MapT>::~TEvMapHnd ()
{
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl(void) Reset (bool incMap)
{
   if (MapUpd_.is_invalid() == false) {
      MapUpd_.end_update();
   }

   if (incMap)
      Map_ = NULL;

   Key_ = TKey();
   StsCode_.reset();
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl(void) SetMap (TMap* map)
{
   if (Map_ == map) {
      return;
   }
   
   if (MapUpd_.is_invalid() == false) {
      MapUpd_.end_update();
   }
   
   Map_ = map;
   StsCode_.reset();
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl(void) SetKey (const TKey& key)
{
   if (Key_ == key) {
      return;
   }
  
   if (MapUpd_.is_invalid() == false) {
      MapUpd_.end_update();
   }

   Key_ = key;
   StsCode_.reset();
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl(void) SetIter (TMapI iter)
{
   SetKey(iter->first);
   
   MapIter_ = iter;
   
   StsCode_.set(esc_Find);
   StsCode_.set(esc_IterValid);
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl(bool) Find ()
{
   if (StsCode_.test(esc_IterValid)) {
      return true;
   }

   if (StsCode_.test(esc_Find) == false) {
      if (Map_ == NULL) {
         return false;
      }

      MapIter_ = Map_->find(Key_);

      if (MapIter_ == Map_->end()) {
         StsCode_.set(esc_NewKey);
      }
      else {
         StsCode_.reset(esc_NewKey);
         StsCode_.set  (esc_IterValid);
      }

      StsCode_.set(esc_Find);
   }

   return (StsCode_.test(esc_NewKey) ? false : true);
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl_S(TMapI) Fetch ()
{
   if (StsCode_.test(esc_IterValid))
      return MapIter_;

   TMapIns ins = Map_->insert(TValueType(Key_, TDat()));

   StsCode_.set(esc_Find);
   if (ins.second == false) {
      // 表示資料已存在
      StsCode_.reset(esc_NewKey);
   }
   else {
      // 表示資料不存在
      StsCode_.set(esc_NewKey);
   }

   MapIter_ = ins.first;
   StsCode_.set(esc_IterValid);

   return MapIter_;
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl_S(TDatCp) FindDat ()
{
   if (Find() == false) {
      return NULL;
   }
   return (TDatCp)(&(MapIter_->second));
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl_S(TDatCp) FindDat (TMap* map)
{
   SetMap(map);
   return FindDat();
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl_S(TDatCp) FindDat (const TKey& key)
{
   SetKey(key);
   return FindDat();
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl_S(TDatCp) FindDat (TMap* map, const TKey& key)
{
   SetKey(key);
   SetMap(map);
   return FindDat();
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl_S(TDat&) GetDat ()
{
   Fetch();
   return MapIter_->second;
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl_S(TDat&) GetDat (TMap* map)
{
   SetMap(map);
   return GetDat();
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl_S(TDat&) GetDat (TMap* map, const TKey& key)
{
   SetKey(key);
   SetMap(map);
   return GetDat();
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl(bool) Erase ()
{
   if (Find() == false) {
      return false;
   }

   // 接下來要移除 iterator, 所以狀態要先清掉
   if (MapUpd_.is_invalid() == false) {
      MapUpd_.end_update();
   }
   StsCode_.reset();

   Map_->erase(MapIter_);

   return true;
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl(bool) Erase (const TKey& key)
{
   SetKey(key);
   return Erase();
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl(bool) GetKey_Inc (TKey& key, bool useLast)
{
   if (Map_ == NULL) {
      return false;
   }
   else if (Map_->empty()) {
      key.assign(1);
   }
   else {
      if (useLast)  key = Map_->rbegin()->first;
      else          key = Map_->begin ()->first;
      ++key;
   }

   return true;
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl_S(TDat&) BeginUpdate ()
{
   if (MapUpd_.is_invalid() == false)
      return *MapUpd_;
   
   Fetch();

   MapUpd_ = Map_->begin_update(MapIter_);
   return *MapUpd_;
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl_S(TDat&) BeginUpdate (TMap* map)
{
   SetMap(map);
   return BeginUpdate();
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl_S(TDat&) BeginUpdate (const TKey& key)
{
   SetKey(key);
   return BeginUpdate();
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl_S(TDat&) BeginUpdate (TMap* map, const TKey& key)
{
   SetKey(key);
   SetMap(map);
   return BeginUpdate();
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl_S(TDat&) BeginUpdate_Inc (bool useLast)
{
   TKey key;

   GetKey_Inc(key, useLast);
   return BeginUpdate(key);
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl_S(TDat&) BeginUpdate_Inc (TMap* map, bool useLast)
{
   SetMap(map);

   return BeginUpdate_Inc(useLast);
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl(void) EndUpdate (bool resetSts)
{
   if (MapUpd_.is_invalid() == false) {
      MapUpd_.end_update();
   }

   if (resetSts) {
      StsCode_.reset();
   }
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl(bool) Insert (const TKey& key, const TDat& dat)
{
   Reset(false);

   if (Map_ == NULL)
      return false;

   TMapIns ins = Map_->insert(TValueType(key, dat));

   if (ins.second) {
      Key_     = key;
      MapIter_ = ins.first;
      
      StsCode_.set(esc_Find);
      StsCode_.set(esc_NewKey);
      StsCode_.set(esc_IterValid);
   }

   return ins.second;
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl(bool) IsNewKey ()
{
   // 沒有 Find 的歷史紀錄
   if (StsCode_.test(esc_Find) == false) {
      Find();
   }

   // 已經 Find 過了, 就直接看結果囉
   return (StsCode_.test(esc_NewKey));
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl(bool) IsNewKey (TMap* map)
{
   SetMap(map);
   return IsNewKey();
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl(bool) IsNewKey (TMap* map, const TKey& key)
{
   SetKey(key);
   SetMap(map);
   return IsNewKey();
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl(bool) IsNewKey (const TKey& key)
{
   SetKey(key);
   return IsNewKey();
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl(bool) IsFirstKey ()
{
   // 沒有 Find 的歷史紀錄
   if (StsCode_.test(esc_Find) == false) {
      Find();
   }
   // 看看 ita 是否合法
   if (StsCode_.test(esc_IterValid) == false) {
      return false;
   }

   return (MapIter_ == Map_->begin() ? true : false);
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl(bool) MoveFirst ()
{
   if (Map_ == NULL || Map_->empty()) {
      return false;
   }

   SetIter(Map_->begin());
   return true;
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl(bool) MoveNext ()
{
   if (Map_ == NULL || Map_->empty() || StsCode_.test(esc_IterValid) == false) {
      return false;
   }

   TMapI iter = MapIter_;

   ++iter;
   if (iter == Map_->end()) {
      return false;
   }

   SetIter(iter);
   return true;
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl_S(TKeyCp) FindLast_Key ()
{
   TKeyCp key = NULL;
   
   if (Map_ != NULL && Map_->empty() == false) {
      key = &(Map_->rbegin()->first);
   }

   return key;
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl(bool) FindPrev (TMapI& iter)
{
   // 先確認這個 Key 值是存在的
   if (Find() == false) {
      return false;
   }

   if (MapIter_ == Map_->begin()) {
      return false;
   }
   
   iter = MapIter_;
   --iter;
   return true;
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl_S(TKeyCp) FindPrev_Key ()
{
   TMapI iter;

   if (FindPrev(iter) == false) {
      return NULL;
   }
   return (TKeyCp)(&(iter->first));
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl_S(TDatCp) FindPrev_Dat ()
{
   TMapI iter;

   if (FindPrev(iter) == false) {
      return NULL;
   }
   return (TDatCp)(&(iter->second));
}
//---------------------------------------------------------------------------

Nsa_TEvMapHnd_Impl_S(TThisType&) operator = (const TThisType& a)
{
   Map_     = a.Map_;
   Key_     = a.Key_;
   MapIter_ = a.MapIter_;
   MapUpd_  = a.MapUpd_;
   StsCode_ = a.StsCode_;

   return *this;
}

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvMap_0Hnd_h_)

