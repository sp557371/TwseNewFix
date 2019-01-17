/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvMap_0FileV2_h_)
#define _Nsa_EvMap_0FileV2_h_
//---------------------------------------------------------------------------
#include "SimpleRecorder.h"
#include "kpersistent.h"
#include "Nsa_EvMap.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;

namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// struct TStoreDefa
/////////////////////////////////////////////////////////////////////////////
struct TStoreDefa
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

/*
   不使用另外一個 map 來儲存 RoomKeys, 直接把 RoomKey 定義在 RecT 裡面
   
   RecT 的定義如下
   struct SRec
   {
      TCommInn::rr_key mutable  RoomKey_;
      ...
      
      如果是固定大小的話, 可以使用 Nsa_EvMap_Default_Operator_Equal(SRec)
      否則就要自己寫 operator =
   };
*/
/////////////////////////////////////////////////////////////////////////////
// class TEvMapReco_v2
/////////////////////////////////////////////////////////////////////////////
template <class ContainerT, class RecT, class StoreT = TStoreDefa>
class TEvMapReco_v2 : public  recorder        <ContainerT, TCommInn>,
                      private inn_rooms_peeker<ContainerT, TCommInn>
{
   typedef recorder<ContainerT, TCommInn>  inher_recorder;

   typedef          ContainerT                  TContainer;
   typedef          RecT                        TRec;
   typedef          StoreT                      TStore;

   typedef typename inher_recorder::TInn        TInn;
   typedef typename inher_recorder::rr_key      rr_key;
   typedef typename inher_recorder::rr_room     rr_room;
   typedef typename TInn::rr_size               rr_size;

   typedef typename TContainer::key_type        TCtnKey;
   typedef typename TContainer::mapped_type     TCtnMappedType;
   typedef typename TContainer::value_type      TCtnValType;
   typedef typename TContainer::iterator        TCtnI;
   typedef typename TContainer::const_iterator  TCtnCi;

   TContainer*  Container_;
   TInn         Inn_;
   TStore       Store_;

public:
   K_ctor  TEvMapReco_v2 (TContainer*, const std::string& fileName);
   K_dtor ~TEvMapReco_v2 ();
   
private:
   //==============================================
   // overwrite Kstl::recorder virtual function
   K_mf(bool) into_new_room (TInn&, const TCtnCi&);
   K_mf(bool) into_room     (TInn&, const TCtnI&);
   
   inline K_mf(rr_key) get_room_key (TInn&, const TCtnI& i)  { return i->second.RoomKey_; }

   //==============================================
   // overwrite Kstl::inn_rooms_peeker virtual function
   K_mf(bool) peek_room (TContainer&, rr_room);
   
   //==============================================
   // TRecorder function
   K_mf(bool) into_room (rr_room, rr_key&, const TCtnKey&, const TRec&);
   
   inline K_mf(rr_key&) fetch_room_key (const TCtnI&  i)  { return i->second.RoomKey_; }
   inline K_mf(rr_key&) fetch_room_key (const TCtnCi& i)  { return i->second.RoomKey_; }
};


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

/////////////////////////////////////////////////////////////////////////////
#include "Nsa_EvMap_0FileV2.cc"

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvMap_0FileV2_h_)
