
/////////////////////////////////////////////////////////////////////////////
// struct persistent_save_my
// struct persistent_load_my
/////////////////////////////////////////////////////////////////////////////
struct persistent_save_my
{
   template <class DestType>
   static inline K_mf(bool) store (persistent& per, const DestType& a)
      { return psave(per, a); }

   static inline K_mf(bool) store (persistent& per, const void* ptr, unsigned sz)
      { return per.save(ptr, sz, false); }
};
//---------------------------------------------------------------------------
struct persistent_load_my
{
   template <class DestType>
   static inline K_mf(bool) store (const persistent& per, DestType& a)
      { return pload(per, a); }
   
   static inline K_mf(bool) store (const persistent& per, void* ptr, unsigned sz)
      { return per.load(ptr, sz, false); }
};

/////////////////////////////////////////////////////////////////////////////
// class TEvMapReco_v2 impl
/////////////////////////////////////////////////////////////////////////////
#define Nsa_TEvMapReco_v2_Impl(return_type)                                \
   template <class ContainerT, class RecT, class RoomImplT>                \
   K_mf(return_type) TEvMapReco_v2<ContainerT, RecT, RoomImplT>::          \
//---------------------------------------------------------------------------

#if defined(__BORLANDC__)
   #define Nsa_TEvMapReco_v2_Impl_S(return_type)                           \
      template <class ContainerT, class RecT, class RoomImplT>             \
      TEvMapReco_v2<ContainerT, RecT, RoomImplT>::return_type __fastcall   \
      TEvMapReco_v2<ContainerT, RecT, RoomImplT>::                         \
   //------------------------------------------------------------------------

#else

   #define Nsa_TEvMapReco_v2_Impl_S(return_type)                           \
      template <class ContainerT, class RecT, class RoomImplT>             \
      typename TEvMapReco_v2<ContainerT, RecT, RoomImplT>::return_type     \
      TEvMapReco_v2<ContainerT, RecT, RoomImplT>::                         \
   //------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

template <class ContainerT, class RecT, class RoomImplT>
K_ctor TEvMapReco_v2<ContainerT, RecT, RoomImplT>::TEvMapReco_v2 (TContainer* evc, const std::string& fileName)
   : inher_recorder(&Inn_)
   , Container_    (evc)
   , Inn_          (fileName, 0, true)
{
   this->peek_rooms(*evc, Inn_);
   inher_recorder::reg(*evc);
}
//---------------------------------------------------------------------------

template <class ContainerT, class RecT, class RoomImplT>
K_dtor TEvMapReco_v2<ContainerT, RecT, RoomImplT>::~TEvMapReco_v2 ()
{
   // 當結束時, 應 unreg event, 不然在觸發 event 時會 error
   inher_recorder::unreg(*Container_);
}
//---------------------------------------------------------------------------

Nsa_TEvMapReco_v2_Impl(bool) into_new_room (TInn& inn, const TCtnCi& citer)
{
         rr_key& key = (fetch_room_key(citer) = inn.check_in(0));
   const TRec&   dat = static_cast<const TRec&>(citer->second);
   
   return into_room(inn.get_room(key), key, citer->first, dat);
}
//---------------------------------------------------------------------------

Nsa_TEvMapReco_v2_Impl(bool) into_room (TInn& inn, const TCtnI& iter)
{
   // 取出此房客的房間鑰匙
         rr_key& key = fetch_room_key(iter);
   const TRec&   dat = static_cast<const TRec&>(iter->second);

   return into_room(inn.get_room(key), key, iter->first, dat);
}
//---------------------------------------------------------------------------

Nsa_TEvMapReco_v2_Impl(bool) peek_room (TContainer& evc, rr_room room)
{
   TCtnKey        key;
   TCtnMappedType rec;
   TRec&          dat = static_cast<TRec&>(rec);
   char           line[1024*100];

   typename TInn::rr_live_peek  peeker[] = {
      { K_GetLo(&peeker[1].obj_size_, 2), 2 },
      { line,                             0 },
   };

   if (room.peek(peeker, peeker+numofele(peeker)) == true) {
      // 載入資料
      unsigned   useSize = peeker[1].obj_size_;
      persistent per(line, useSize);

      per.rewind();

      persistent_load_my storage;
      Store_.exec(per, storage, key, dat);

      // 新增到資料表
      TCtnI iter = evc.insert(TCtnValType(key, rec)).first;

      //把[房間鑰匙]保留,待會兒存檔時,才找得到此人住的房間
      fetch_room_key(iter) = room.get_key();

      return true;
   }
   return false;
}
//---------------------------------------------------------------------------

Nsa_TEvMapReco_v2_Impl(bool) into_room (rr_room room, rr_key& rrkey, const TCtnKey& key, const TRec& rec)
{
   char       line[1024*100];
   persistent per(line, sizeof(line));

   persistent_save_my storage;
   Store_.exec(per, storage, key, rec);

   typename TInn::rr_live_into  peeker[] = {
      { K_GetLo(&peeker[1].obj_size_, 2), 2                                          },
      { line,                             static_cast<rr_size>(per.get_saved_size()) },
   };

   return room.into(rrkey, peeker, peeker+numofele(peeker));
}


/////////////////////////////////////////////////////////////////////////////
// class TEvMapReco2d_v2 impl
/////////////////////////////////////////////////////////////////////////////
#define Nsa_TEvMapReco2d_v2_Impl(return_type)                                    \
   template <class DetaMap2dMaker, class RoomImplT, class EvDetailsT>            \
   K_mf(return_type) TEvMapReco2d_v2<DetaMap2dMaker, RoomImplT, EvDetailsT>::    \
//---------------------------------------------------------------------------------

template <class DetaMap2dMaker, class RoomImplT, class EvDetailsT>
K_ctor TEvMapReco2d_v2<DetaMap2dMaker, RoomImplT, EvDetailsT>::TEvMapReco2d_v2 (TMasterRecs* c, const std::string& fileName)
   : InheritedRecorder(&Inn_, c)
   , Inn_             (fileName, 0, true)
{
   InheritedRecorder::BindInn(Inn_);
}
//---------------------------------------------------------------------------

Nsa_TEvMapReco2d_v2_Impl(bool) peek_room (TRecs& recs, rr_room room)
{
   TRecKey key;
   TRec    rec;
   char    line[1024*100];
   
   typename TInn::rr_live_peek  peeker[] = {
      { K_GetLo(&peeker[1].obj_size_, 2), 2 },
      { line,                             0 },
   };

   if (room.peek(peeker, peeker+numofele(peeker)) == true) {
      // 載入資料
      unsigned   useSize = peeker[1].obj_size_;
      persistent per(line, useSize);

      per.rewind();

      persistent_load_my storage;
      Store_.exec(per, storage, key, rec);

      // 新增到資料表
      typedef typename TRecs::value_type  value_type;
      typedef typename TRecs::iterator    iterator;
      
      iterator iter = recs.insert(value_type(key, rec)).first;

      //把[房間鑰匙]保留,待會兒存檔時,才找得到此人住的房間
      fetch_room_key(iter) = room.get_key();

      return true;
   }

   return false;
}
//---------------------------------------------------------------------------

Nsa_TEvMapReco2d_v2_Impl(bool) rec_into_room (rr_room room, rr_key& rrkey, const TRecKey& key, const TRec& rec)
{
   typedef typename TInn::rr_size  rr_size;

   char       line[1024*100];
   persistent per(line, sizeof(line));

   persistent_save_my storage;
   Store_.exec(per, storage, key, rec);

   typename TInn::rr_live_into  peeker[] = {
      { K_GetLo(&peeker[1].obj_size_, 2), 2                                          },
      { line,                             static_cast<rr_size>(per.get_saved_size()) },
   };

   return room.into(rrkey, peeker, peeker+numofele(peeker));
}
//---------------------------------------------------------------------------
