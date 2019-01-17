//---------------------------------------------------------------------------
#ifndef XRecorderH
#define XRecorderH
//---------------------------------------------------------------------------
#include "SimpleRecorder.h"
#include "inn_file.h"
//---------------------------------------------------------------------------
typedef Kstl::rr_inn_file<Kway::uint64,Kway::word>    Xinn_type;
template <class TContainer,class TInn,unsigned RecSz>
K_class TXRecorder : public Kway::TSimpleRecorder<TContainer,TInn,RecSz>
{
   typedef Kway::TSimpleRecorder<TContainer,TInn,RecSz>   InheritedRecorder;
   typedef TContainer                   TRecs;
   typedef typename TInn::rr_room       rr_room;
   typedef typename TInn::rr_key        rr_key;
   typedef typename TInn::rr_size       rr_size;
   typedef typename TRecs::key_type     TRecKey;
   typedef std::map<TRecKey,rr_key>     TRoomKeys;

   TRoomKeys   RoomKeys_;
public:
   K_ctor  TXRecorder  (TInn& Inn)
               : InheritedRecorder(&Inn)
               { }
   template<class RecsArg>
   K_ctor  TXRecorder   (TInn& Inn, RecsArg* ra)
                : InheritedRecorder(Inn,ra)
                { }

   K_mf(rr_key)  get_room_key      (TInn& inn, const typename TRecs::iterator& i)
   {
        typename TRoomKeys::iterator  ikey = RoomKeys_.find(i->first);
        if(ikey == RoomKeys_.end())//找不到此人的房間鑰匙
                return TXRecorder::rr_key();
        return ikey->second;
   }
   K_mf(rr_key&)  fetch_room_key   (const typename TRecs::iterator& i)
   {
        return RoomKeys_[i->first];
   }
   K_mf(rr_key&)  fetch_room_key   (const typename TRecs::const_iterator& i)
   {
        return RoomKeys_[i->first];
   }
   K_mf(void)  BindInn  (TInn& Inn)
   {
        InheritedRecorder::BindInn(Inn);
   }

   K_mf(TRecs*) GetContainer()
   {
        return &Recs_;
   }
};
//---------------------------------------------------------------------------
#endif
