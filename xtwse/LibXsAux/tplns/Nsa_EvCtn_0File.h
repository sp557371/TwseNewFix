/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvCtn_0File_h_)
#define _Nsa_EvMap_0FileV2_h_
//---------------------------------------------------------------------------
#include "SimpleRecorder.h"
#include "kpersistent.h"
#include "Nsa_EvCtn.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// struct TEvCtnStoreDefa
/////////////////////////////////////////////////////////////////////////////
struct TEvCtnStoreDefa
{
   template <class TPersistent, class TStorage, class TKey, class TRec>
   K_mf(bool) exec (TPersistent& per, TStorage& storage, TKey& key, TRec& rec)
   {
      //**********************************************
      #if defined(__NSC__)
      return storage.store(per, SimpleRecorder_GetBasePtr(key), THasVtab<TKey>::SizeRec) &&
             storage.store(per, SimpleRecorder_GetBasePtr(rec), THasVtab<TRec>::SizeRec);

      #else //----------------------------------------

      return storage.store(per, &key, sizeof(TKey)) &&
             storage.store(per, &rec, sizeof(TRec));
      #endif
      //**********************************************
   }
};
/////////////////////////////////////////////////////////////////////////////
// class TEvCtnLoadPer
/////////////////////////////////////////////////////////////////////////////
class TEvCtnLoadPer : public Kstl::persistent_auto
{
   typedef Kstl::persistent_auto  inherited;

public:
   inline K_ctor TEvCtnLoadPer (unsigned bufSize)
      : inherited(bufSize)
   {}

   inline K_mf(void*) GetBuf ()  { return inherited::getbuf(); }
};


/////////////////////////////////////////////////////////////////////////////
// class TEvCtnReco
/////////////////////////////////////////////////////////////////////////////
/*
   不使用另外一個 map 來儲存 RoomKeys, 直接把 RoomKey 定義在 RecT 裡面
   
   RecT 的定義如下
   struct SRec
   {
      TEvCtnInn::rr_key mutable  RoomKey_;
      ...
      
      如果是固定大小的話, 可以使用 Nsa_EvMap_Default_Operator_Equal(SRec)
      否則就要自己寫 operator =
   };
*/
template <class CtnT,
          class StoreT,
          class RecT,
          class IterCastT>
class TEvCtnReco : public  recorder        <CtnT, TEvCtnInn>,
                   private inn_rooms_peeker<CtnT, TEvCtnInn>
{
protected:
   typedef recorder<CtnT, TEvCtnInn>  inher_recorder;
   typedef CtnT                       TCtn;
   typedef RecT                       TRec;
   typedef StoreT                     TStore;
   typedef IterCastT                  TIterCast;

   typedef typename inher_recorder::TInn     TInn;
   typedef typename inher_recorder::rr_key   rr_key;
   typedef typename inher_recorder::rr_room  rr_room;
   typedef typename TInn::rr_size            rr_size;

   typedef typename TCtn::key_type        TCtnKey;
   typedef typename TCtn::iterator        TCtnI;
   typedef typename TCtn::const_iterator  TCtnCi;
      
   TCtn*                  Container_;
   TInn                   Inn_;
   TStore                 Store_;
   TEvCtnLoadPer          PerLoad_;
   Kstl::persistent_auto  PerSave_;

public:
   K_ctor  TEvCtnReco (TCtn*, const std::string& fileName);
   K_dtor ~TEvCtnReco ();

   K_mf(void) BindInn ();

protected:
   //==============================================
   // TEvCtnReco function
   inline K_mf(TCtnKey)     GetKey     (const TCtnI&  iter)   { return TIterCast::GetKey   (iter);  }
   inline K_mf(TCtnKey)     GetKey     (const TCtnCi& citer)  { return TIterCast::GetKey   (citer); }
   inline K_mf(const TRec&) GetMapped  (const TCtnI&  iter)   { return TIterCast::GetMapped(iter);  }
   inline K_mf(const TRec&) GetMapped  (const TCtnCi& citer)  { return TIterCast::GetMapped(citer); }

   inline K_mf(rr_key&) fetch_room_key (       const TCtnI&  iter)   { return TIterCast::GetRec(iter) .RoomKey_; }
   inline K_mf(rr_key&) fetch_room_key (       const TCtnCi& citer)  { return TIterCast::GetRec(citer).RoomKey_; }
   inline K_mf(rr_key)  get_room_key   (TInn&, const TCtnI&  iter)   { return TIterCast::GetRec(iter) .RoomKey_; }

private:
   //==============================================
   // overwrite Kstl::recorder virtual function
   K_mf(bool) into_new_room (TInn&, const TCtnCi&);
   K_mf(bool) into_room     (TInn&, const TCtnI&);
   
   //==============================================
   // overwrite Kstl::inn_rooms_peeker virtual function
   K_mf(bool) peek_room (TCtn&, rr_room);
   
   //==============================================
   // TRecorder function
   K_mf(bool) into_room (rr_room, rr_key&, const TCtnKey&, const TRec&);
};
/////////////////////////////////////////////////////////////////////////////
// class TEvAryReco
/////////////////////////////////////////////////////////////////////////////
template <class CtnT,
          class StoreT    = TEvCtnStoreDefa,
          class RecT      = typename CtnT::mapped_type,
          class IterCastT = TIterCastEvAry<CtnT> >
class TEvAryReco : public TEvCtnReco<CtnT, StoreT, RecT, IterCastT>
{
   typedef          TEvCtnReco<CtnT, StoreT, RecT, IterCastT>  inherited;
   typedef typename inherited::TCtn  TCtn;
   
public:
   inline K_ctor TEvAryReco (TCtn* evc, const std::string& fileName)
      : inherited(evc, fileName)
   {
      inherited::BindInn();
   }
};
/////////////////////////////////////////////////////////////////////////////
// class TEvMapReco
/////////////////////////////////////////////////////////////////////////////
template <class CtnT, class StoreT = TEvCtnStoreDefa, class RecT = typename CtnT::mapped_type>
class TEvMapReco : public TEvCtnReco<CtnT, StoreT, RecT, TIterCastEvMap<CtnT> >
{
   typedef          TEvCtnReco<CtnT, StoreT, RecT, TIterCastEvMap<CtnT> >  inherited;
   typedef typename inherited::TCtn  TCtn;

public:
   inline K_ctor TEvMapReco (TCtn* evc, const std::string& fileName)
      : inherited(evc, fileName)
   {
      inherited::BindInn();
   }
};







/*
/////////////////////////////////////////////////////////////////////////////
// class TMapMaker_2k2d
/////////////////////////////////////////////////////////////////////////////
template<class TContainer1, class TContainer2>
struct TMapMaker_2k2d
{
   typedef          TContainer1                    master_map_type;
   typedef typename master_map_type::iterator      master_iterator;
   typedef typename master_map_type::key_type      master_key_type;
   typedef typename master_iterator::value_type    master_value_type;
   typedef          TContainer2                    detail_map_type;
   typedef typename detail_map_type::iterator      detail_iterator;
   typedef typename detail_map_type::key_type      detail_key_type;
   typedef typename detail_map_type::value_type    detail_value_type;
   typedef typename detail_map_type::mapped_type   detail_mapped_type;

   struct key_type {
      master_key_type   master_key_;
      detail_key_type   detail_key_;

      K_ctor  key_type  () {}
      K_ctor  key_type  (const master_key_type& mkey, const detail_key_type& dkey)
                           : master_key_(mkey), detail_key_(dkey)
                           { }
   };

   struct value_type {
      key_type           key_;
      detail_value_type  detail_value_;

      inline K_ctor value_type ()
      {}

      inline K_ctor value_type (const key_type& k, const detail_mapped_type& m)
         : key_         (k)
         , detail_value_(k.detail_key_, m)
      {}
   };


   inline K_mf(detail_map_type*) get_detail_map (master_value_type& v) const
      {
         detail_map_type* dmap;
         return v.second.GetContainer(dmap);
      }

   static inline K_mf(const master_key_type)    get_master_key   (const value_type& v)
                                                { return v.key_.master_key_; }

   static inline K_mf(detail_map_type*) fetch_detail_map (const master_iterator& i)
      {
         detail_map_type* dmap;
         return i->second.GetContainer(dmap);
      }

   static inline K_mf(void) detail_set_master_key (detail_map_type& dmap, const master_key_type& key)
      {
         dmap.set_master_key(key);
      }

   static inline K_mf(const detail_value_type&) get_detail_value (const value_type& v)
                                                { return v.detail_value_;    }

   static K_mf(bool) find_master_iterator (master_map_type&       mmap,
                                           const detail_map_type& dmap,
                                           master_iterator&       imaster)
      {
         master_iterator ifind = mmap.find(dmap.get_master_key());
         if (ifind == mmap.end())
            return false;
         imaster = ifind;
         return true;
      }
};


/////////////////////////////////////////////////////////////////////////////
// class TEvCtnReco2d
/////////////////////////////////////////////////////////////////////////////
template <class EvCtn2dMakerT, class EvDetailsT, class StoreT, class RecT>
class TEvCtnReco2d : public TEvCtnReco<EvDetailsT, StoreT, RecT>
{
protected:
   //==============================================
   // base type
   typedef TEvCtnReco<EvDetailsT, StoreT, RecT>  inherited;
   typedef EvCtn2dMakerT                         TEvCtn2dMaker;
   typedef EvDetailsT                            TEvDetails;
   typedef RecT                                  TRec;
   typedef StoreT                                TStore;

   //==============================================
   // ext type
   typedef typename TEvCtn2dMaker::master_map_type  TMasterRecs;
   typedef typename TEvCtn2dMaker::detail_map_type  TDetailRecs;
   typedef typename inherited::TCtn                 TCtn;
   typedef typename inherited::TCtnKey              TCtnKey;
   typedef typename inherited::TInn                 TInn;

   //==============================================
   // TInn type
   typedef typename TInn::rr_room                    rr_room;
   typedef typename TInn::rr_key                     rr_key;

public:
   inline K_ctor TEvCtnReco2d (TMasterRecs* rs, const std::string& fileName)
      : inherited(rs, fileName)
   {}
};
/////////////////////////////////////////////////////////////////////////////
// class TEvMapReco2d
/////////////////////////////////////////////////////////////////////////////
template <class Ctn1T, class Ctn2T, class StoreT = TEvCtnStoreDefa, class RecT = typename Ctn2T::mapped_type>
class TEvMapReco2d : public TEvCtnReco2d<TMapMaker_2k2d<Ctn1T, Ctn2T>, Kstl::ev_map2d<TMapMaker_2k2d<Ctn1T, Ctn2T> >,
                                         StoreT, RecT>
{
   typedef TEvCtnReco2d<TMapMaker_2k2d<Ctn1T, Ctn2T>, Kstl::ev_map2d<TMapMaker_2k2d<Ctn1T, Ctn2T> >,
                        StoreT, RecT>  inherited;
   
   typedef typename inherited::TMasterRecs  TMasterRecs;
   typedef typename inherited::TCtnKey      TCtnKey;
   typedef typename inherited::TRec         TRec;
   typedef typename inherited::TCtnI        TCtnI;
   typedef typename inherited::TCtnCi       TCtnCi;

public:
   inline K_ctor TEvMapReco2d (TMasterRecs* evc, const std::string& fileName)
      : inherited(evc, fileName)
   {}

protected:
   //==============================================
   // overwrite TEvCtnReco virtual function
   inline K_mf(const TCtnKey&) GetKey (const TCtnI& iter, TCtnKey& key) const
      {
         key = iter.get_key();
         return key;
      }
      
   inline K_mf(const TCtnKey&) GetKey (const TCtnCi& iter, TCtnKey& key) const
      {
         key = iter.get_key();
         return key;
      }
   
   inline K_mf(const TRec&) GetRec (const TCtnI&  iter) const  { return iter.get_mapped(); }
   inline K_mf(const TRec&) GetRec (const TCtnCi& iter) const  { return iter.get_mapped(); }
};
*/


/*
/////////////////////////////////////////////////////////////////////////////
// class TEvMapReco2d_v2
/////////////////////////////////////////////////////////////////////////////
// 抄襲 TSimpleRecorder_Detail 而來
//    class DetaMap2dMaker = TMapMaker_2k2d<TContainer1, TContainer2>
template <class DetaMap2dMaker,
          class StoreT     = TStoreDefa,
          class EvDetailsT = Kstl::ev_map2d<DetaMap2dMaker> >
class TEvMapReco2d_v2 : public TSimpleRecorder<EvDetailsT, TCommInn, 0>
{
protected:
   //==============================================
   // base type
   typedef TSimpleRecorder<EvDetailsT, TCommInn, 0>  InheritedRecorder;
   typedef StoreT                                    TStore;

   //==============================================
   // ext type
   typedef typename DetaMap2dMaker::master_map_type  TMasterRecs;
   typedef typename DetaMap2dMaker::detail_map_type  TDetailRecs;
   typedef typename InheritedRecorder::TRecs         TRecs;
   typedef typename InheritedRecorder::TRecKey       TRecKey;
   typedef typename InheritedRecorder::TRec          TRec;
   typedef typename InheritedRecorder::TInn          TInn;

   //==============================================
   // TInn type
   typedef typename TInn::rr_room                    rr_room;
   typedef typename TInn::rr_key                     rr_key;

protected:
   TInn    Inn_;
   TStore  Store_;

public:
   K_ctor TEvMapReco2d_v2 (TMasterRecs* c, const std::string& fileName);

protected:
   //==============================================
   // overwrite TSimpleRecorder
   K_mf(bool) peek_room     (TRecs&, rr_room);
   K_mf(bool) rec_into_room (rr_room, rr_key&, const TRecKey&, const TRec&);

   //----------------------------------------------
   K_mf(rr_key)  get_room_key   (TInn&, const typename InheritedRecorder::iterator&       i)  { return i.get_mapped().RoomKey_; }
   K_mf(rr_key&) fetch_room_key (       const typename InheritedRecorder::iterator&       i)  { return i.get_mapped().RoomKey_; }
   K_mf(rr_key&) fetch_room_key (       const typename InheritedRecorder::const_iterator& i)  { return i.get_mapped().RoomKey_; }
};
*/

/////////////////////////////////////////////////////////////////////////////
#include "Nsa_EvCtn_0File.cc"

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvCtn_0File_h_)
