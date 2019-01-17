
/////////////////////////////////////////////////////////////////////////////
// struct per_load_evctn_file
// struct per_save_evctn_file
/////////////////////////////////////////////////////////////////////////////
struct per_load_evctn_file
{
   template <class DestType>
   static inline K_mf(bool) store (const Kstl::persistent& per, DestType& a)
      { return Kstl::pload(per, a); }
   
   static inline K_mf(bool) store (const Kstl::persistent& per, void* ptr, unsigned sz)
      { return per.load(ptr, sz, false); }

   template <class DestType>
   static inline K_mf(bool) check_wsptr (DestType& a)
   {
      typedef typename DestType::ref_obj  DestTypeRef;

      DestTypeRef* recRef = a.get();
      if (!recRef) {
         recRef = new DestTypeRef;
         a.reset(recRef);
      }
      return (recRef != NULL);
   }
};
//---------------------------------------------------------------------------
struct per_save_evctn_file
{
   template <class DestType>
   static inline K_mf(bool) store (Kstl::persistent& per, const DestType& a)
      { return Kstl::psave(per, a); }

   static inline K_mf(bool) store (Kstl::persistent& per, const void* ptr, unsigned sz)
      { return per.save(ptr, sz, false); }

   template <class DestType>
   static inline K_mf(bool) check_wsptr (DestType& a)
      { return (a.get() != NULL); }
};


/////////////////////////////////////////////////////////////////////////////
// class TEvCtnReco impl
/////////////////////////////////////////////////////////////////////////////
#define Nsa_TEvCtnReco_Impl(return_type)                                   \
   template <class CtnT, class StoreT, class RecT, class IterCastT>        \
   K_mf(return_type) TEvCtnReco<CtnT, StoreT, RecT, IterCastT>::           \
//---------------------------------------------------------------------------

#if defined(__BORLANDC__)
   #define Nsa_TEvCtnReco_Impl_S(return_type)                              \
      template <class CtnT, class StoreT, class RecT, class IterCastT>     \
      TEvCtnReco<CtnT, StoreT, RecT, IterCastT>::return_type __fastcall    \
      TEvCtnReco<CtnT, StoreT, RecT, IterCastT>::                          \
   //------------------------------------------------------------------------

#else

   #define Nsa_TEvCtnReco_Impl_S(return_type)                              \
      template <class CtnT, class StoreT, class RecT, class IterCastT>     \
      typename TEvCtnReco<CtnT, StoreT, RecT, IterCastT>::return_type      \
      TEvCtnReco<CtnT, StoreT, RecT, IterCastT>::                          \
   //------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

template <class CtnT, class StoreT, class RecT, class IterCastT>
K_ctor TEvCtnReco<CtnT, StoreT, RecT, IterCastT>::TEvCtnReco (TCtn* evc, const std::string& fileName)
   : inher_recorder(&Inn_)
   , Container_    (evc)
   , Inn_          (fileName, 0, true)
   , PerLoad_      (1024*100)
   , PerSave_      (1024*100)
{
}
//---------------------------------------------------------------------------

template <class CtnT, class StoreT, class RecT, class IterCastT>
K_dtor TEvCtnReco<CtnT, StoreT, RecT, IterCastT>::~TEvCtnReco ()
{
   // 當結束時, 應 unreg event, 不然在觸發 event 時會 error
   inher_recorder::unreg(*Container_);
}
//---------------------------------------------------------------------------

Nsa_TEvCtnReco_Impl(void) BindInn ()
{
   peek_rooms(*Container_, Inn_);
   inher_recorder::reg(*Container_);
}
//---------------------------------------------------------------------------

Nsa_TEvCtnReco_Impl(bool) into_new_room (TInn& inn, const TCtnCi& citer)
{
         rr_key& rrkey = (fetch_room_key(citer) = inn.check_in(0));
         TCtnKey key   = GetKey   (citer);
   const TRec&   rec   = GetMapped(citer);

   return into_room(inn.get_room(rrkey), rrkey, key, rec);
}
//---------------------------------------------------------------------------

Nsa_TEvCtnReco_Impl(bool) into_room (TInn& inn, const TCtnI& iter)
{
   // 取出此房客的房間鑰匙
         rr_key& rrkey = fetch_room_key(iter);
         TCtnKey key   = GetKey   (iter);
   const TRec&   rec   = GetMapped(iter);

   return into_room(inn.get_room(rrkey), rrkey, key, rec);
}
//---------------------------------------------------------------------------

Nsa_TEvCtnReco_Impl(bool) peek_room (TCtn& evc, rr_room room)
{
   // 在 ev_ary 中, TRec 都是 const
   // 所以想辦法搞掉 const
   typedef typename Kstl::DeConst<TRec>::R&  TRecRefe;
   typedef typename TCtn::value_type         value_type;
   typedef typename TCtn::updater            updater;

   value_type val;
   TCtnKey&   key = const_cast<TCtnKey&>(val.first);
   TRecRefe   rec = const_cast<TRecRefe>(val.second);
   //char       line[1024*100];

   typename TInn::rr_live_peek  peeker[] = {
      { K_GetLo(&peeker[1].obj_size_, 2), 2 },
      //{ line,                             0 },
      { PerLoad_.GetBuf(),                0 },
   };

   if (room.peek(peeker, peeker+numofele(peeker)) == true) {
      // 載入資料
      unsigned   useSize = peeker[1].obj_size_;
      //persistent per(line, useSize);
      //per.rewind();
      PerLoad_.ResetBufferSize(useSize);

      per_load_evctn_file storage;
      Store_.exec((const Kstl::persistent&)PerLoad_, storage, key, rec);

      // 新增到資料表
      std::pair<TCtnI, bool> ins = evc.insert(val);
      if (!ins.second) {
         updater upd = evc.begin_update(ins.first);
         *upd = val.second;
      }

      //把[房間鑰匙]保留,待會兒存檔時,才找得到此人住的房間
      fetch_room_key(ins.first) = room.get_key();

      return true;
   }
   return false;
}
//---------------------------------------------------------------------------

Nsa_TEvCtnReco_Impl(bool) into_room (rr_room room, rr_key& rrkey, const TCtnKey& key, const TRec& rec)
{
   //char       line[1024*100];
   //persistent per(line, sizeof(line));
   PerSave_.rewind();

   per_save_evctn_file storage;
   Store_.exec((Kstl::persistent&)PerSave_, storage, key, rec);

   typename TInn::rr_live_into  peeker[] = {
      { K_GetLo(&peeker[1].obj_size_, 2), 2                                          },
      //{ line,                             static_cast<rr_size>(per.get_saved_size()) },
      { PerSave_.getbuf(),                static_cast<rr_size>(PerSave_.get_saved_size()) },
   };

   return room.into(rrkey, peeker, peeker+numofele(peeker));
}


/*
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
*/
