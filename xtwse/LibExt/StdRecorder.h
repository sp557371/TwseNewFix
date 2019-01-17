//---------------------------------------------------------------------------
#ifndef StdRecorderH
#define StdRecorderH
//---------------------------------------------------------------------------
#include "char_ary.h"
#include "inn_file.h"
#include "recorder.h"
#include "ev_map.h"
#include "ev_map2d.h"
#include "SimpleRecorder.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kstl;
//---------------------------------------------------------------------------
typedef Kstl::rr_inn_file<Kway::uint64,Kway::word>     TStdInn;
//---------------------------------------------------------------------------
template<class TContainer, class TRecord>
class TStdRecorder : protected Kstl::recorder<TContainer,TStdInn>,
                     protected Kstl::inn_rooms_peeker<TContainer,TStdInn>
{
   typedef Kstl::recorder<TContainer,TStdInn>  inherited;
public:
   typedef          TStdInn             rr_inns;
   typedef typename rr_inns::rr_room    rr_room;
   typedef typename rr_inns::rr_key     rr_key;
   typedef typename rr_inns::rr_size    rr_size;
   typedef          TContainer          TRecs;
   typedef typename TRecs::key_type     TRecKey;
   typedef          TRecord             TReca;
   typedef std::map<TRecKey,rr_key>     TRoomKeys;
   K_ctor  TStdRecorder  (TRecs& recs, const char* fileName)
      : inherited(&inn_),
        inn_(std::string(fileName),0,true)
   {
      this->peek_rooms(recs, inn_);
      this->reg(recs);
   }
private:
   K_mf(bool)  into_room  (rr_inns::rr_room room,   rr_key&     key,
                                         const TRecKey&  recKey, const TReca& rec)
   {
      rr_inns::rr_live_into    peeker[] = {
         { &recKey,              sizeof(recKey)                              },
         { &rec,                 sizeof(rec)                                 },
         };
      return room.into(key, peeker, peeker+numofele(peeker));
   }
   K_mf(bool)    into_new_room  (rr_inns& inn, const typename TRecs::const_iterator& i)
   {
      rr_key&  key = (RoomKeys_[i->first] = inn.check_in(0));
      return into_room(inn.get_room(key), key, i->first, i->second);
   }
   K_mf(bool)    into_room      (rr_inns& inn, const typename TRecs::iterator& i)
   {
      rr_key&  key = RoomKeys_[i->first];
      if(key.is_invalid())
         return false;
      return into_room(inn.get_room(key), key, i->first, i->second);
   }
   K_mf(rr_key)  get_room_key   (rr_inns& inn, const typename TRecs::iterator& i)
   {
      typename TRoomKeys::iterator ikey = RoomKeys_.find(i->first);
      if(ikey == RoomKeys_.end())
         return rr_key();
//         return TStdRecorder::rr_key();
      return ikey->second;
   }
   K_mf(bool)  peek_room  (TRecs& recs, rr_inns::rr_room room)
   {
      TRecKey  recKey;
      TReca    rec;
      rr_inns::rr_live_peek    peeker[] = {{ &recKey,              sizeof(recKey)},
                                           { &rec,                 sizeof(rec)   },
                                       };
      if(room.peek(peeker, peeker+numofele(peeker))) {
         recs.insert(std::make_pair(recKey,rec));
         RoomKeys_[recKey] = room.get_key();
         return true;
      }
      return false;
   }
   rr_inns     inn_;
   TRoomKeys   RoomKeys_;
};
//---------------------------------------------------------------------------
/*
2維的儲存較為複雜, 必須先定義detail 的資料
typedef TDMapValue<TValue>     TDValue;
再定義detail 的容器結構
typedef TDMap<TMasterKey, TDetailKey, TDValue>   TDContainer;
然後定義master 的容器結構
typedef Kstl::ev_map<TMasterKey, TDContainer>    TMContainer;
最後定義這個容器的recorder
typedef TStdRecorder2d<TMContainer, TDContainer> T2dRecorder;
T2dRecorder* Recorder_ = new T2dRecorder(TMContainer*,filename);
*/
//---------------------------------------------------------------------------
template<class TValue>
struct TDMapValue : public TValue
{
   TStdInn::rr_key mutable RoomKey_;
};
//---------------------------------------------------------------------------
template<class TMKey, class TDKey, class TDValue>
class TDMap : public Kstl::ev_map<TDKey, TDValue>
{
   TMKey Key_;
public:
   K_mf(TMKey) get_master_key() const {return Key_;}
   K_mf(void) set_master_key(const TMKey& masterkey){ Key_ = masterkey; }
   inline K_mf(std::string)        GetMasterKeyStr () const { return Key_.as_string();}
};
//---------------------------------------------------------------------------
template<class TContainer1, class TContainer2>
struct TStdMapMaker : public Kstl::map2d_maker_base<TContainer1,TContainer2> 
{
   typedef Kstl::map2d_maker_base<TContainer1,TContainer2> inherited;
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
//---------------------------------------------------------------------------
template<class TContainer1, class TContainer2>
class TStdRecorder2d 
{
   typedef TStdMapMaker<TContainer1, TContainer2>                          TNMapMaker;
   typedef typename TContainer1::key_type                                  TPKey;
   typedef typename TContainer2::value_type                                TValue;
   typedef typename TContainer2::key_type                                  TDKey;
   typedef Kway::TSimpleRecorder_Detail<TNMapMaker,TStdInn,sizeof(TValue)> TSRecorder2d;
   TSRecorder2d*   Recorder_;        
public:
   K_ctor  TStdRecorder2d(TContainer1* c, char* filename)
   {
      Recorder_ = new TSRecorder2d(c, filename,0);
      Recorder_->BindInn();  
   }
   K_dtor  ~TStdRecorder2d()
   {
      delete Recorder_;  
   }
};
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
#endif

