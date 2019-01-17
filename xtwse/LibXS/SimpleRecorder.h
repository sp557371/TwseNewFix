//
// 定義一種簡單的 [ev_容器] 儲存方式
//
// 版本: $Id: SimpleRecorder.h,v 1.9 2006/04/04 03:53:28 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SimpleRecorderH
#define SimpleRecorderH
//---------------------------------------------------------------------------
#include "recorder.h"
#include "ev_map.h"
#include "ev_map2d.h"
//---------------------------------------------------------------------------
namespace Kway {
/*
定義取出房間鑰匙的方法,有兩種:
(1)第一種是將 RoomKey 放在 RecsMgr 之中:
class TRecsMgr : private TSimpleRecorder<TRecs,TInn,sizeof(TRec)>
{
   //將 TRec 對應到的房間鑰匙紀錄起來, 當 TRec 需要重新進入房間時,才有辦法找到原來的房間
   typedef std::map<TRecKey,rr_key>  TRoomKeys;
   TRoomKeys   RoomKeys_;
   K_mf(TInn::rr_key&)  fetch_room_key  (const iterator& i)         { return RoomKeys_[i->first]; }
   K_mf(TInn::rr_key&)  fetch_room_key  (const  const_iterator& i)  { return RoomKeys_[i->first]; }
   K_mf(TInn::rr_key)   get_room_key    (TInn&, const iterator& i)
                                          { TRoomKeys::iterator  ikey = RoomKeys_.find(i->first);
                                             return(ikey == RoomKeys_.end()   //找不到此人的房間鑰匙
                                                      ? rr_key()
                                                      : ikey->second);
                                          }
};
//----------------------------------------------------------------------------------------
(2)第二種是將 RoomKey 放在 TRec 之中, 此種做法相較上述的做法速度較快(因為省掉了RoomKeys_)
   但因為要改變 TRecs 的定義方式, 所以請自行考慮適合您的方式:
class TRec_RoomKey : public TRec {
   friend class TRecsMgr;
   TInn::rr_key mutable  RoomKey_; //將房門鑰匙放在這兒,可簡化 TRecsMgr 的處理
};           // ^^^^^^^ 請注意此處的 mutable, 表示可以不經過 updater 來更新 TRec 的內容
typedef Kstl::ev_map<TRecKey, const TRec_RoomKey>   TRecs;
class TRecsMgr : private TSimpleRecorder<TRecs,TInn,sizeof(TRec)>
{
   //直接從 TRec_RoomKey 取出 RoomKey, 可避免再透過map<>尋找, 速度較快
   K_mf(TInn::rr_key)   get_room_key    (TInn&, const iterator& i)  { return i->second.RoomKey_; }
   K_mf(TInn::rr_key&)  fetch_room_key  (const iterator& i)         { return i->second.RoomKey_; }
   K_mf(TInn::rr_key&)  fetch_room_key  (const const_iterator& i)   { return i->second.RoomKey_; }
};
*/
/////////////////////////////////////////////////////////////////////////////
template <class Container, class Inn, unsigned RecSz>
class TSimpleRecorder : protected Kstl::recorder        <Container, Inn>,
                        protected Kstl::inn_rooms_peeker<Container, Inn>
{
   typedef Kstl::recorder<Container, Inn>   InheritedRecorder;

public:
   typedef Container                      TRecs;
   typedef Inn                            TInn;
   typedef typename TRecs::key_type       TRecKey;
   typedef typename TRecs::mapped_type    TRec;

   K_ctor  TSimpleRecorder  (TInn* inn)
               : InheritedRecorder(inn),
                 Recs_()
               { }
   template<class RecsArg>
   K_ctor  TSimpleRecorder  (TInn* inn, RecsArg* ra)
               : InheritedRecorder(inn),
                 Recs_(ra)
               { }

   static const unsigned rec_size = RecSz;

protected:
   K_mf(void)         BindInn        (TInn&);
   K_mf(TRecs*)       get_recs       ()         { return &Recs_; }
   K_mf(const TRecs*) get_recs_const () const   { return &Recs_; }//避免在建構時呼叫 get_recs() 不知要 const 還是不要?

   typedef typename TRecs::iterator          iterator;
   typedef typename TRecs::const_iterator    const_iterator;
   TRecs   Recs_;

protected:
   typedef typename TInn::rr_room    rr_room;
   typedef typename TInn::rr_key     rr_key;
   typedef typename TInn::rr_size    rr_size;

   //底下的 member function, 是給 inn_rooms_peeker<> 用的
   //  從客房取出一筆 TRec
   virtual K_mf(bool)  peek_room    (TRecs&, rr_room);
   virtual K_mf(bool)  af_peek_room (TRecKey&, TRec&) { return true; }

   //將一筆 TRec 放到客房裡面
   virtual K_mf(bool)  rec_into_room  (rr_room, rr_key&, const TRecKey&, const TRec&);

   //底下的 member functions, 為了給 recorder<> 用, 是一定要的啦!
   virtual K_mf(bool)  into_new_room  (TInn&, const const_iterator&);
   virtual K_mf(bool)  into_room      (TInn&, const iterator&);

private:
//取得 rr_key 的方法, 還是需要由後續 [衍生出來的class] 來定義
//      K_mf(rr_key)   get_room_key    (TInn&, const iterator&);
virtual K_mf(rr_key&)  fetch_room_key  (const iterator&) = 0;
virtual K_mf(rr_key&)  fetch_room_key  (const const_iterator&) = 0;
};
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
class TVtabSize {
   virtual ~TVtabSize() {}
   friend void DeWarning();//讓gcc不要出現Warn訊息.
};
//---------------------------------------------------------------------------
template <class T>
class THasVtab : public T {
	struct TV : public T {
		virtual ~TV() {}
	};
public:
   static const bool HasVirtual = ((sizeof(TV) - sizeof(T))==0);
   static const int  SizeVPtr   = (HasVirtual ? sizeof(TVtabSize) : 0);
   static const int  SizeRec    = (sizeof(T) - SizeVPtr);
};
template <class T>
class THasVtab_None {
public:
   enum {
      HasVirtual = 0,
      SizeVPtr   = 0,
      SizeRec    = sizeof(T),
   };
};
template <> class THasVtab<int8>   : public THasVtab_None<int8>   {};
template <> class THasVtab<uint8>  : public THasVtab_None<uint8>  {};
template <> class THasVtab<int16>  : public THasVtab_None<int16>  {};
template <> class THasVtab<uint16> : public THasVtab_None<uint16> {};
template <> class THasVtab<int32>  : public THasVtab_None<int32>  {};
template <> class THasVtab<uint32> : public THasVtab_None<uint32> {};
template <> class THasVtab<int64>  : public THasVtab_None<int64>  {};
template <> class THasVtab<uint64> : public THasVtab_None<uint64> {};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
//===========================================================================
// VC 把 vptr 放在 this 的開頭, 儲存/載入時,要避開 __vfptr
#ifdef _MSC_VER
   template <class T>
   inline void* SimpleRecorder_GetBasePtr(const T& r, unsigned vptrsz = THasVtab<T>::SizeVPtr) {
      return (Kway::byte*)(&r) + vptrsz;
   }
#else
   #define SimpleRecorder_GetBasePtr(r)      ((void*)&r)
#endif
/////////////////////////////////////////////////////////////////////////////
template <class TRecs, class TInn, unsigned RecSz>
K_mf(void)  TSimpleRecorder<TRecs,TInn,RecSz>::BindInn  (TInn& inn)
{
   this->SetInn(&inn);
   this->peek_rooms(Recs_, inn);
   this->reg(Recs_);
}
//---------------------------------------------------------------------------
template <class TRecs, class TInn, unsigned RecSz>
K_mf(bool)  TSimpleRecorder<TRecs,TInn,RecSz>::peek_room  (TRecs& recs, rr_room room)
{
   typename DeConst<TRecKey>::R  recKey;
   typename DeConst<TRec>::R     rec;
   //定義 簡單的[房間結構]
   typename TInn::rr_live_peek    peeker[] = { { SimpleRecorder_GetBasePtr(recKey), THasVtab<TRecKey>::SizeRec },
                                               { SimpleRecorder_GetBasePtr(rec),    RecSz                      },
                                               };
   //從房間取出資料
   bool peekResult = this->GetInn()->is_dynamic_room_size()
                        ? room.peek_objs_or_fill(peeker, peeker+numofele(peeker), 1)
                        : room.peek(peeker, peeker+numofele(peeker));
   if(peekResult) {
      if(!af_peek_room(recKey, rec))
         return true;
      //資料取出OK,將取出的資料放到 recs 之中
      typedef typename TRecs::value_type  value_type;
      iterator i = recs.insert(value_type(recKey,rec)).first;
      //把[房間鑰匙]保留,待會兒存檔時,才找得到此人住的房間
      fetch_room_key(i) = room.get_key();
      return true;
   }
   return false;
}
//---------------------------------------------------------------------------
template <class TRecs, class TInn, unsigned RecSz>
K_mf(bool)  TSimpleRecorder<TRecs,TInn,RecSz>::rec_into_room  (rr_room        room,
                                                               rr_key&        key,
                                                               const TRecKey& recKey,
                                                               const TRec&    rec)
{
   //定義[房間結構]
   typename TInn::rr_live_into    peeker[] = { { SimpleRecorder_GetBasePtr(recKey), THasVtab<TRecKey>::SizeRec },
                                               { SimpleRecorder_GetBasePtr(rec),    RecSz                      },
                                               };
   //將資料存入房間
   return room.into(key, peeker, peeker+numofele(peeker));
}
//---------------------------------------------------------------------------
template <class TRecs, class TInn, unsigned RecSz>
K_mf(bool)  TSimpleRecorder<TRecs,TInn,RecSz>::into_new_room  (TInn& inn, const const_iterator& i)
{  //有新的房客加入[容器]內,所以開一間新的客房,讓新的房客住
   // check_in()時,要給需要的房間大小,如果填入0,表示使用[客棧]的預設值
   using namespace Kstl;
   rr_key&  key = (fetch_room_key(i) = inn.check_in(0));
   //return into_room(inn.get_room(key), key, i->first, i->second);
   return rec_into_room(inn.get_room(key), key, Recs_.get_key(i), Recs_.get_mapped(i));
}
//---------------------------------------------------------------------------
template <class TRecs, class TInn, unsigned RecSz>
K_mf(bool)  TSimpleRecorder<TRecs,TInn,RecSz>::into_room  (TInn& inn, const iterator& i)
{  //已經住進此客棧的房客,要修改住進客棧的內容
   using namespace Kstl;
   rr_key&  key = fetch_room_key(i);//取出此房客的房間鑰匙
   //return into_room(inn.get_room(key), key, i->first, i->second);
   return rec_into_room(inn.get_room(key), key, Recs_.get_key(i), Recs_.get_mapped(i));
}
/////////////////////////////////////////////////////////////////////////////
/*
//---------------------------------------------------------------------------
// 因為大部分單純存檔需求的 container, 會將 RoomKey 包含在 container 的元素之中,
// 造成使用 updater 更新資料時的問題(RoomKey可能會有非預期的變動),
// 為了避免這種情形,使用 updater 時,可用底下的 template class 將 RoomKey 藏起來:
// 使用方式可參考 "SignOn\UserDB.h"
template <class TKey, class TContainerRec, class TNoRoomKeyRec, class TEvUpdaterImplBase = Kstl::ev_updater_impl>
struct ev_updater_impl_hide_room_key : public TEvUpdaterImplBase
{
   typedef std::pair<const TKey, const TContainerRec>    iterator_value;
   typedef TNoRoomKeyRec&                                reference;
   typedef TNoRoomKeyRec*                                pointer;
   typedef const TNoRoomKeyRec&                          const_reference;
   typedef const TNoRoomKeyRec*                          const_pointer;

   inline K_ctor  ev_updater_impl_hide_room_key  (Kstl::ev_container& c, const Kstl::ev_iterator& i)
                     : TEvUpdaterImplBase(c),
                       Iter_(static_cast<const Iterator&>(i))
                     { }

   // updater 要更新的是 TNoRoomKeyRec, 不可以是 TContainerRec,
   // 因為要避免 *u = i->second; 造成 RoomKey 被改變的情形!
   // 所以要用底下的方式轉型:
   virtual K_mf(reference)  get_modifyer  ()  { return const_cast<reference>(static_cast<const_reference>(  a.second  )); }
   virtual K_mf(reference)  get_modifyer  ()  { return const_cast<reference>(*Iter_); }
};
//---------------------------------------------------------------------------
*/
/////////////////////////////////////////////////////////////////////////////
template <class Key, class Rec, class RoomKey>
class TSimpleMap_AddRoomKey
{
public:
   //為了 alpha, 所以名稱要瘦身! 原本為 struct TRec_RoomKey;
   struct R_ : public Rec
   {
      RoomKey mutable  RoomKey_; //將房門鑰匙放在這兒,可簡化Mgr的處理
                                  //因為將房間的鑰匙放在 ev_map<> 的元素之中
                                  //所以要更改 updater_impl 的寫法,如下:
   };
   typedef R_  TRec_RoomKey;
   //------------------------------------------------------------------------
   typedef std::map<Key, const TRec_RoomKey> TOrigMap_t;
   K_typedef(TOrigMap_t,                     TOrigMap);
   typedef typename TOrigMap::iterator       TOrigIter;
   #ifdef __alpha //為了避免Symbol太長,所以在此解開ev_container_i<>
      class TRecsBase : public Kstl::ev_container
      {
      typedef Kstl::ev_container  inherited;
      public:
         //Iterators.
         typedef TOrigIter  orig_iterator;
         struct iterator : public ev_iterator, public orig_iterator
         {
         inline K_ctor          iterator   ()                        { }
         inline K_ctor          iterator   (const iterator& a)       : orig_iterator(a) { }
         inline K_ctor          iterator   (const orig_iterator& a)  : orig_iterator(a) { }
         inline K_mf(iterator&) operator=  (const iterator& a)       { *static_cast<orig_iterator*>(this) = a; return *this; }
         inline K_mf(iterator&) operator=  (const orig_iterator& a)  { *static_cast<orig_iterator*>(this) = a; return *this; }

         inline K_mf(bool)  operator==  (const iterator& a)    const  { return static_cast<const orig_iterator&>(a)==*this; }
         inline K_mf(bool)  operator==  (const ev_iterator& a) const  { return operator==(static_cast<const iterator&>(a)); }
         };
      };
   #else
      typedef Kstl::ev_container_i<TOrigIter>   TRecsBase;
   #endif
   //------------------------------------------------------------------------
   struct updater_impl : public Kstl::ev_updater_impl {
      typedef Rec&                           reference;
      typedef Rec*                           pointer;
      typedef typename TRecsBase::iterator   iterator;
      typedef typename iterator::value_type  iterator_value;

      inline K_ctor  updater_impl  (Kstl::ev_container& c, const Kstl::ev_iterator& i)
                     : Kstl::ev_updater_impl(c),
                       Iter_(static_cast<const iterator&>(i))
                     { }

      virtual K_mf(const iterator&)  get_iterator  () const    { return Iter_; }
      virtual K_mf(reference)        get_modifyer  ()          { return static_cast<reference>(
                                                                           const_cast<TRec_RoomKey&>(Iter_->second) ); }
   protected:
      iterator    Iter_;
   };
   //typedef ev_updater_impl_hide_room_key<Key, TRec_RoomKey, Rec, Kstl::ev_updater_impl>    updater_impl;
   //------------------------------------------------------------------------
   typedef Kstl::ev_container_u<TRecsBase, updater_impl>    TRecsBaseU_t;
   K_typedef(TRecsBaseU_t,                                  TRecsBaseU);
   typedef Kstl::ev_map<Key, const TRec_RoomKey,
                  #ifndef __alpha
                        typename TOrigMap::key_compare, typename TOrigMap::allocator_type,
                  #endif
                        TRecsBaseU
                       >   TEvRecs_t;
   K_typedef(TEvRecs_t,    TEvRecs);
};
/////////////////////////////////////////////////////////////////////////////
//為了讓存檔的定義方式更容易些,所以增加底下的 template:
template <class Key, class Rec, class Inn, unsigned RecSz = sizeof(Rec)>
class TSimpleRecorder_AddRoomKey : protected TSimpleRecorder<typename TSimpleMap_AddRoomKey<Key, Rec, typename Inn::rr_key>::TEvRecs,
                                                             Inn, RecSz>
{
   typedef TSimpleMap_AddRoomKey<Key,Rec,typename Inn::rr_key>  TMap_AddRoomKey;
   typedef typename TMap_AddRoomKey::TEvRecs          TEvRecs;
   typedef TSimpleRecorder<TEvRecs, Inn, RecSz>       inherited;
public:
   typedef typename inherited::TInn                   TInn;
   typedef TEvRecs                                    TRecs;
   typedef typename inherited::TRec                   TRec;
   typedef typename TRecs::iterator                   iterator;
   typedef typename TRecs::const_iterator             const_iterator;
   typedef typename TMap_AddRoomKey::TRec_RoomKey     TRec_RoomKey;

   K_ctor  TSimpleRecorder_AddRoomKey  (const std::string& fileName, typename TInn::rr_size roomSize, bool can_check_out = true)
               : inherited(&Inn_),
                 Inn_(fileName, roomSize, can_check_out)
               { }
protected:
   K_mf(void)  BindInn  ()   {  inherited::BindInn(Inn_); }

private:
   //資料儲存的[客棧]
   TInn   Inn_;
   //定義取出房間鑰匙的方法
   K_mf(typename TInn::rr_key)   get_room_key    (TInn&, const iterator& i)  { return i->second.RoomKey_; }
   K_mf(typename TInn::rr_key&)  fetch_room_key  (const iterator& i)         { return i->second.RoomKey_; }
   K_mf(typename TInn::rr_key&)  fetch_room_key  (const const_iterator& i)   { return i->second.RoomKey_; }
   /*
   //否則需要將 TRec 對應到的房間鑰匙紀錄起來, 當 TRec 需要重新進入房間時,才有辦法找到原來的房間
   typedef std::map<Key,Inn::rr_key>  TRoomKeys;
   TRoomKeys   RoomKeys_;
   K_mf(TInn::rr_key&)  fetch_room_key  (const iterator& i)        { return RoomKeys_[i->first]; }
   K_mf(TInn::rr_key&)  fetch_room_key  (const const_iterator& i)  { return RoomKeys_[i->first]; }
   K_mf(TInn::rr_key)   get_room_key    (TInn&, const iterator& i)
                                             { TRoomKeys::iterator  ikey = RoomKeys_.find(i->first);
                                                return(ikey == RoomKeys_.end()   //找不到此人的房間鑰匙
                                                         ? rr_key()
                                                         : ikey->second);
                                             }
   */
};
//---------------------------------
#define K_typedef_TSimpleRecorder_AddRoomKey_t(TL,TR) \
class TR : public TL {                                \
public:                                               \
   K_ctor TR (const std::string& fileName, TInn::rr_size roomSize, bool can_check_out = true)\
      : TL(fileName, roomSize, can_check_out)         \
      { }                                             \
   K_typedef(TL::TRecs, TRecs);                       \
   K_mf(TRecs*)       get_recs       ()         { return static_cast<TRecs*>(&Recs_);       }  \
   K_mf(const TRecs*) get_recs_const () const   { return static_cast<const TRecs*>(&Recs_); }  \
}
// 在alpha上, 以上的 static_cast<> 其實是不安全的轉型!
//---------------------------------
#define K_typedef_TSimpleRecorder_AddRoomKey(TR,Key,Rec,Inn,RecSz)   \
typedef TSimpleRecorder_AddRoomKey<Key, Rec, Inn, RecSz> TR##_t;     \
K_typedef_TSimpleRecorder_AddRoomKey_t(TR##_t, TR)
/////////////////////////////////////////////////////////////////////////////
//定義 Detail 資料表的儲存
//  Detail TRec 必須包含 RoomKey_
template <class DetailMap2dMaker, class TInn, unsigned RecSz, class TevDetails = Kstl::ev_map2d<DetailMap2dMaker> >
class TSimpleRecorder_Detail : protected TSimpleRecorder<TevDetails,//Kstl::ev_map2d<DetailMap2dMaker>,
                                                         TInn,
                                                         RecSz>//要存入TInn的資料大小
{
protected:
 //typedef Kstl::ev_map2d<DetailMap2dMaker>                 TevDetails;
   typedef typename DetailMap2dMaker::master_map_type       TMasterRecs;
   typedef typename DetailMap2dMaker::detail_map_type       TDetailRecs;
   typedef TInn                                             TDetailsInn;
   typedef TSimpleRecorder<TevDetails,TDetailsInn,RecSz>    InheritedRecorder;
public:
   typedef TevDetails                                       TRecs;

   K_ctor  TSimpleRecorder_Detail  (TMasterRecs* MasterRecs, const std::string& fileName,
                                    typename TInn::rr_size roomSize, bool can_check_out = true)
               : InheritedRecorder(&Inn_, MasterRecs),
                 Inn_(fileName, roomSize, can_check_out)
               { }
   K_mf(void)  BindInn  ()  { InheritedRecorder::BindInn(Inn_); }
private:
   //資料儲存的[客棧]
   TInn   Inn_;
   //定義取出房間鑰匙的方法
   K_mf(typename TInn::rr_key)   get_room_key    (TInn&, const typename InheritedRecorder::iterator& i)  { return i.get_mapped().RoomKey_; }
   K_mf(typename TInn::rr_key&)  fetch_room_key  (const typename InheritedRecorder::iterator& i)         { return i.get_mapped().RoomKey_; }
   K_mf(typename TInn::rr_key&)  fetch_room_key  (const typename InheritedRecorder::const_iterator& i)   { return i.get_mapped().RoomKey_; }
};
//---------------------------------
#define K_typedef_TSimpleRecorder_Detail_t(TL,TR)                        \
class TR : public TL {                                                   \
public:                                                                  \
   K_ctor  TR  (TMasterRecs* MasterRecs, const std::string& fileName,    \
                TDetailsInn::rr_size roomSize, bool can_check_out = true)\
      : TL(MasterRecs, fileName, roomSize, can_check_out)                \
      { }                                                                \
  K_typedef_NoDefCtor(TL::TRecs, TRecs);                                 \
  K_mf(TRecs*)       get_recs       ()         { return static_cast<TRecs*>(&Recs_);       }  \
  K_mf(const TRecs*) get_recs_const () const   { return static_cast<const TRecs*>(&Recs_); }  \
}
// 在alpha上, 以上的 static_cast<> 其實是不安全的轉型!
//---------------------------------
#define K_typedef_TSimpleRecorder_Detail(TR,DetailMap2dMaker,TInn,RecSz)    \
typedef Kstl::ev_map2d<DetailMap2dMaker>                           TR##_t2d;\
K_typedef_NoDefCtor(TR##_t2d, TR##2d);                                      \
typedef TSimpleRecorder_Detail<DetailMap2dMaker,TInn,RecSz,TR##2d> TR##_t;  \
K_typedef_TSimpleRecorder_Detail_t(TR##_t, TR)
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

