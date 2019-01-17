

/////////////////////////////////////////////////////////////////////////////
// struct TDefaRoom impl
/////////////////////////////////////////////////////////////////////////////
#define Nsa_TDefaRoom_Impl(return_type)                                    \
   template <class KeyT, class RecT>                                       \
   K_mf(return_type) TDefaRoom<KeyT, RecT>::                               \
//---------------------------------------------------------------------------

Nsa_TDefaRoom_Impl(bool) exec_peek_room (TKey& key, TRec& rec, TInn::rr_room& room)
{
   TInn::rr_live_peek peeker[] = {
      { &key, sizeof(TKey) },
      { &rec, sizeof(TRec) },
   };

   return room.peek(peeker, peeker+numofele(peeker));
}
//---------------------------------------------------------------------------

Nsa_TDefaRoom_Impl(bool) exec_into_room (TInn::rr_room& room, TInn::rr_key& rrkey, const TKey& key, const TRec& rec)
{
   TInn::rr_live_into peeker[] = {
      { &key, sizeof(TKey) },
      { &rec, sizeof(TRec) },
   };

   return room.into(rrkey, peeker, peeker+numofele(peeker));
}


/////////////////////////////////////////////////////////////////////////////
// class TEvMapReco impl
/////////////////////////////////////////////////////////////////////////////
#define Nsa_TEvMapReco_Impl(return_type)                                   \
   template <class ContainerT, class RecT, class RoomImplT>                \
   K_mf(return_type) TEvMapReco<ContainerT, RecT, RoomImplT>::             \
//---------------------------------------------------------------------------

#if defined(__BORLANDC__)
   #define Nsa_TEvMapReco_Impl_S(return_type)                              \
      template <class ContainerT, class RecT, class RoomImplT>             \
      TEvMapReco<ContainerT, RecT, RoomImplT>::return_type __fastcall      \
      TEvMapReco<ContainerT, RecT, RoomImplT>::                            \
   //------------------------------------------------------------------------

#else

   #define Nsa_TEvMapReco_Impl_S(return_type)                              \
      template <class ContainerT, class RecT, class RoomImplT>             \
      typename TEvMapReco<ContainerT, RecT, RoomImplT>::return_type        \
      TEvMapReco<ContainerT, RecT, RoomImplT>::                            \
   //------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

template <class ContainerT, class RecT, class RoomImplT>
K_ctor TEvMapReco<ContainerT, RecT, RoomImplT>::TEvMapReco (TContainer* evc, const std::string& fileName)
   : inher_recorder(&Inn_)
   , Container_    (evc)
   , Inn_          (fileName, 0, true)
{
   this->peek_rooms(*evc, Inn_);
   inher_recorder::reg(*evc);
}
//---------------------------------------------------------------------------

template <class ContainerT, class RecT, class RoomImplT>
K_dtor TEvMapReco<ContainerT, RecT, RoomImplT>::~TEvMapReco ()
{
   // 當結束時, 應 unreg event, 不然在觸發 event 時會 error
   inher_recorder::unreg(*Container_);
}
//---------------------------------------------------------------------------

Nsa_TEvMapReco_Impl(bool) into_new_room (TInn& inn, const TCtnCi& citer)
{
   rr_key&     key = (RoomKeys_[citer->first] = inn.check_in(0));
   const TRec& dat = static_cast<const TRec&>(citer->second);
   
   return into_room(inn.get_room(key), key, citer->first, dat);
}
//---------------------------------------------------------------------------

Nsa_TEvMapReco_Impl(bool) into_room (TInn& inn, const TCtnI& iter)
{
   rr_key&     key = RoomKeys_[iter->first];
   const TRec& dat = static_cast<const TRec&>(iter->second);

   return into_room(inn.get_room(key), key, iter->first, dat);
}
//---------------------------------------------------------------------------

Nsa_TEvMapReco_Impl_S(rr_key) get_room_key (TInn& inn, const TCtnI& iter)
{
   typename TRoomKeys::iterator ikey = RoomKeys_.find(iter->first);

   if (ikey == RoomKeys_.end())
      return rr_key();
   return ikey->second;
}
//---------------------------------------------------------------------------

Nsa_TEvMapReco_Impl(bool) peek_room  (TContainer& evc, rr_room room)
{
   TCtnKey        key;
   TCtnMappedType rec;
   TRec&          dat = static_cast<TRec&>(rec);
   
   if (TRoomImpl::exec_peek_room(key, dat, room)) {
      evc.insert(TCtnValType(key, rec));
      RoomKeys_[key] = room.get_key();
      return true;
   }
   return false;
}
//---------------------------------------------------------------------------

Nsa_TEvMapReco_Impl(bool) into_room (rr_room room, rr_key& rrkey, const TCtnKey& key, const TRec& rec)
{
   return TRoomImpl::exec_into_room(room, rrkey, key, rec);
}


/////////////////////////////////////////////////////////////////////////////
// class TSimpRecoDeta impl
/////////////////////////////////////////////////////////////////////////////
#define Nsa_TSimpRecoDeta_Impl(return_type)                                      \
   template <class DetaMap2dMaker, class RoomImplT, class EvDetailsT>            \
   K_mf(return_type) TSimpRecoDeta<DetaMap2dMaker, RoomImplT, EvDetailsT>::      \
//---------------------------------------------------------------------------------

template <class DetaMap2dMaker, class RoomImplT, class EvDetailsT>
K_ctor TSimpRecoDeta<DetaMap2dMaker, RoomImplT, EvDetailsT>::TSimpRecoDeta (TMasterRecs* c, const std::string& fileName)
   : InheritedRecorder(&Inn_, c)
   , Inn_             (fileName, 0, true)
{
   InheritedRecorder::BindInn(Inn_);
}
//---------------------------------------------------------------------------

Nsa_TSimpRecoDeta_Impl(bool) peek_room (TRecs& recs, rr_room room)
{
   TRecKey key;
   TRec    rec;
   
   if (RoomImpl_.exec_peek_room(key, rec, room)) {
      //資料取出OK,將取出的資料放到 recs 之中
      typedef typename TRecs::value_type  value_type;
      typedef typename TRecs::iterator    iterator;
      
      iterator i = recs.insert(value_type(key, rec)).first;
      //把[房間鑰匙]保留,待會兒存檔時,才找得到此人住的房間
      fetch_room_key(i) = room.get_key();
      return true;
   }
   return false;
}
//---------------------------------------------------------------------------

Nsa_TSimpRecoDeta_Impl(bool) rec_into_room (rr_room room, rr_key& rrkey, const TRecKey& key, const TRec& rec)
{
   return RoomImpl_.exec_into_room(room, rrkey, key, rec);
}
//---------------------------------------------------------------------------
