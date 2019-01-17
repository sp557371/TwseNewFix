/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvMap_0FileV1_h_)
#define _Nsa_EvMap_0FileV1_h_
//---------------------------------------------------------------------------
#include "char_ary.h"
#include "recorder.h"
#include "ev_map2d.h"
#include "SimpleRecorder.h"
#include "Nsa_EvMap.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// struct TDefaRoom
/////////////////////////////////////////////////////////////////////////////
//template <class ContainerT, class RecT>
//struct TDefaRoom
//{
//   typedef          ContainerT            TContainer;
//   typedef          RecT                  TRec;
//   typedef          TCommInn              TInn;
//   
//   typedef typename TContainer::key_type  TCtnKey;
//
//   static K_mf(bool) exec_peek_room (TCtnKey&, TRec&, TInn::rr_room&);
//   static K_mf(bool) exec_into_room (TInn::rr_room&, TInn::rr_key&, const TCtnKey&, const TRec&);
//};
template <class KeyT, class RecT>
struct TDefaRoom
{
   typedef KeyT      TKey;
   typedef RecT      TRec;
   typedef TCommInn  TInn;

   static K_mf(bool) exec_peek_room (TKey&, TRec&, TInn::rr_room&);
   static K_mf(bool) exec_into_room (TInn::rr_room&, TInn::rr_key&, const TKey&, const TRec&);
};


/////////////////////////////////////////////////////////////////////////////
// class TRecorder
/////////////////////////////////////////////////////////////////////////////
//template <class ContainerT, class RecT, class RoomImplT = TDefaRoom<ContainerT, RecT> >
template <class ContainerT, class RecT, class RoomImplT = TDefaRoom<typename ContainerT::key_type, RecT> >
class TEvMapReco : public  Kstl::recorder<ContainerT, TCommInn>,
                   private Kstl::inn_rooms_peeker<ContainerT, TCommInn>
{
   typedef Kstl::recorder<ContainerT, TCommInn>  inher_recorder;

   typedef          ContainerT                  TContainer;
   typedef          RecT                        TRec;
   typedef          RoomImplT                   TRoomImpl;

   typedef typename inher_recorder::TInn        TInn;
   typedef typename inher_recorder::rr_key      rr_key;
   typedef typename inher_recorder::rr_room     rr_room;
   typedef typename TInn::rr_size               rr_size;

   typedef typename TContainer::key_type        TCtnKey;
   typedef typename TContainer::mapped_type     TCtnMappedType;
   typedef typename TContainer::value_type      TCtnValType;
   typedef typename TContainer::iterator        TCtnI;
   typedef typename TContainer::const_iterator  TCtnCi;

   typedef std::map<TCtnKey, rr_key>            TRoomKeys;

   TContainer*  Container_;
   TInn         Inn_;
   TRoomKeys    RoomKeys_;

public:
   K_ctor  TEvMapReco (TContainer*, const std::string& fileName);
   K_dtor ~TEvMapReco ();
   
private:
   //==============================================
   // overwrite Kstl::recorder virtual function
   K_mf(bool)   into_new_room (TInn&, const TCtnCi&);
   K_mf(bool)   into_room     (TInn&, const TCtnI&);
   K_mf(rr_key) get_room_key  (TInn&, const TCtnI&);

   //==============================================
   // overwrite Kstl::inn_rooms_peeker virtual function
   K_mf(bool) peek_room (TContainer&, rr_room);
   
   //==============================================
   // TRecorder function
   K_mf(bool) into_room (rr_room, rr_key&, const TCtnKey&, const TRec&);
};


/////////////////////////////////////////////////////////////////////////////
// class TMapMaker
/////////////////////////////////////////////////////////////////////////////
template<class TContainer1, class TContainer2>
struct TMapMaker : public Kstl::map2d_maker_base<TContainer1,TContainer2>
{
   typedef Kstl::map2d_maker_base<TContainer1,TContainer2> inherited;
   typedef TContainer1                             master_map_type;
   typedef typename master_map_type::iterator      master_iterator;
   typedef typename master_map_type::key_type      master_key_type;
   typedef typename master_iterator::value_type    master_value_type;
   typedef TContainer2                             detail_map_type;
   typedef typename detail_map_type::iterator      detail_iterator;
   typedef typename detail_map_type::key_type      detail_key_type;
   typedef typename detail_map_type::value_type    detail_value_type;
   typedef typename detail_map_type::inherited     Rdetail_map;

   typedef typename detail_map_type::mapped_type   mapped_type;
   struct key_type {
      master_key_type   master_key_;
      detail_key_type   detail_key_;

      K_ctor  key_type  () {}
      K_ctor  key_type  (const master_key_type& mkey, const detail_key_type& dkey)
                           : master_key_(mkey), detail_key_(dkey)
                           { }
   };

   struct value_type {
      key_type             key_;
      detail_value_type    detail_value_;
      K_ctor  value_type  (const key_type& k, const mapped_type& m)
                              : key_(k), detail_value_(k.detail_key_, m)
                              { }
      K_ctor  value_type  ()  { }
   };
   static inline K_mf(const master_key_type)    get_master_key   (const value_type& v)
                                                { return v.key_.master_key_; }
          inline K_mf(detail_map_type*)         get_detail_map   (      master_value_type& v)
                                          const { return &(v.second);        }
   static inline K_mf(detail_map_type*)         fetch_detail_map (const master_iterator&   i)
                                                { return &(i->second);       }
   static inline K_mf(const detail_value_type&) get_detail_value (const value_type& v)
                                                { return v.detail_value_;    }
   static inline K_mf(void)  detail_set_master_key  (typename inherited::detail_map_type& dmap,
                                                     const typename inherited::master_key_type& key)
   { dmap.set_master_key(key); }

   static K_mf(bool) find_master_iterator(typename inherited::master_map_type& mmap
                                          ,const typename inherited::detail_map_type& dmap
                                          ,typename inherited::master_iterator& imaster)
   {
      typename inherited::master_iterator ifind = mmap.find(dmap.get_master_key());
      if(ifind==mmap.end())
         return false;
      imaster = ifind;
      return true;
   }
};


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
         detail_map_type* dmap = NULL;
         return v.second.GetContainer(dmap);
      }

   static inline K_mf(const master_key_type)    get_master_key   (const value_type& v)
                                                { return v.key_.master_key_; }

   static inline K_mf(detail_map_type*) fetch_detail_map (const master_iterator& i)
      {
         detail_map_type* dmap = NULL;
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
// struct nse_detail_map2d_maker
/////////////////////////////////////////////////////////////////////////////
template<class TContainer1, class TContainer2>
struct nse_detail_map2d_maker : public Kstl::map2d_maker_base<TContainer1, TContainer2>
{
   typedef TContainer2                             detail_map_type;
   typedef TContainer1                             master_map_type;
   typedef typename master_map_type::iterator      master_iterator;
   typedef typename master_iterator::value_type    master_value_type;

   //       inline K_mf(const detail_map_type*)  get_detail_map   (const master_value_type& v) const { return &(v.second//.GetDetail());  }
   //       inline K_mf(detail_map_type*)        get_detail_map   (      master_value_type& v) const { return &(v.second//.GetDetail());  }
   //static inline K_mf(detail_map_type*)        fetch_detail_map (const master_iterator&   i)       { return &(i->second//.GetDetail()); }
          inline K_mf(const detail_map_type*)  get_detail_map   (const master_value_type& v) const { return &(v.second);  }
          inline K_mf(detail_map_type*)        get_detail_map   (      master_value_type& v) const { return &(v.second);  }
   static inline K_mf(detail_map_type*)        fetch_detail_map (const master_iterator&   i)       { return &(i->second); }
};


/////////////////////////////////////////////////////////////////////////////
// class TRecorder2d
/////////////////////////////////////////////////////////////////////////////
template <class TContainer1, class TContainer2, class TDat2>
class TRecorder2d
{
   typedef TMapMaker<TContainer1, TContainer2>                          TNMapMaker;
   typedef typename TContainer2::value_type                             TValue;
   typedef TSimpleRecorder_Detail<TNMapMaker, TCommInn, sizeof(TDat2)>  TSRecorder2d;

   TSRecorder2d*  Recorder_;

public:
   // constructor
   K_ctor TRecorder2d (TContainer1* c, const std::string& fileName)
   {
      //Recorder_ = new TSRecorder2d(c, fileName, 0//sizeof(TContainer1::key_type)+sizeof(TContainer2::value_type));
      Recorder_ = new TSRecorder2d(c, fileName, 0);
      Recorder_->BindInn();
   }

   // destructor
   K_dtor ~TRecorder2d ()
   {
      delete Recorder_;
   }
};


/////////////////////////////////////////////////////////////////////////////
// class TSimpRecoDeta
/////////////////////////////////////////////////////////////////////////////
// ß€≈ß TSimpleRecorder_Detail ¶”®”
//    class DetaMap2dMaker = TMapMaker<TContainer1, TContainer2>
template <class DetaMap2dMaker,
          //class RoomImplT  = TDefaRoom<DetaMap2dMaker::key_type, DetaMap2dMaker::detail_value_type>,
          class RoomImplT  = TDefaRoom<typename DetaMap2dMaker::key_type, typename DetaMap2dMaker::detail_mapped_type>,
          class EvDetailsT = Kstl::ev_map2d<DetaMap2dMaker> >
class TSimpRecoDeta : public TSimpleRecorder<EvDetailsT, TCommInn, 0>
{
protected:
   typedef TSimpleRecorder<EvDetailsT, TCommInn, 0>  InheritedRecorder;
   typedef RoomImplT                                 TRoomImpl;
   //----------------------------------------------
   typedef typename DetaMap2dMaker::master_map_type  TMasterRecs;
   typedef typename DetaMap2dMaker::detail_map_type  TDetailRecs;
   typedef typename InheritedRecorder::TRecs         TRecs;
   typedef typename InheritedRecorder::TRecKey       TRecKey;
   typedef typename InheritedRecorder::TRec          TRec;
   typedef typename InheritedRecorder::TInn          TInn;
   //----------------------------------------------
   typedef typename TInn::rr_room                    rr_room;
   typedef typename TInn::rr_key                     rr_key;

protected:
   TInn       Inn_;
   TRoomImpl  RoomImpl_;

public:
   K_ctor TSimpRecoDeta (TMasterRecs* c, const std::string& fileName);

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

/////////////////////////////////////////////////////////////////////////////
#include "Nsa_EvMap_0FileV1.cc"

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvMap_0FileV1_h_)
