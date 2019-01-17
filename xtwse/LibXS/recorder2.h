/**
   template recorder2<> 第二版.
   第一版在 "Recorder.h"
*/
//---------------------------------------------------------------------------
#ifndef recorder2H
#define recorder2H
//---------------------------------------------------------------------------
#include "inn.h"
#include "ev_container.h"
#include "recorder.h"
//---------------------------------------------------------------------------
namespace Kstl {
//
// Kstl 命名規則說明:請參考 "ev_container.h"
//
/////////////////////////////////////////////////////////////////////////////
/** 此 recorder2<> 包含了一個TInn,
    且適用於: 記錄 [1 or 多] 個 Container.

   可參考 TwStkFinaDetail.cpp      Master-Detail
          TwStkCust.cpp            單層Table.
          UserRemoteMgrPolicy.cpp  包含單層 & Master-Detail
          
*/
template <class TContainer, class TInn>
class recorder2 : private container_ev_handler
{
   typedef recorder2<TContainer,TInn>  this_type;
public:
   typedef TContainer   container_type;
   typedef TInn         inn_type;
   /** 建構.
   */
   K_ctor  recorder2  (const std::string& fname, typename TInn::rr_size room_size, bool can_check_out = true)
      : Inn_(fname, room_size, can_check_out)
      { }

   /* 解構
   */
   virtual K_dtor ~recorder2  ()  {}

   /** [開始/結束]監看某 Container 的變化,並記錄到 Inn 之中.
   */
   inline K_mf(void)  regto  (TContainer& evc)  { evc.add_ev_handler(this); }
   inline K_mf(void)  unreg  (TContainer& evc)  { evc.del_ev_handler(this); }

   /** 取得儲存物件的 inn.
   */
   inline K_mf(TInn&)  get_inn  ()   { return Inn_; }

   /** 將某 Container 從 Inn 之中清除掉.
   */
   K_mf(void)  ClearContainsRooms  (const ev_container& evc)
      {  TContainer& c = const_cast<TContainer&>(static_cast<const TContainer&>(evc));
         typename TContainer::iterator iend = c.end();
         for(typename TContainer::iterator i = c.begin();  i != iend;  ++i) {
            rr_key   key = get_room_key(i, c);
            if(!key.is_invalid())
               Inn_.check_out(key);
         }
      }
protected:
   typedef typename TInn::rr_key    rr_key;
   typedef typename TInn::rr_room   rr_room;

   TInn  Inn_;

   /** 您必須完成底下的 3 個 mf, 才能正確無誤的幫您將資料存入 Inn.
   */
   virtual K_mf(rr_key) get_room_key  (const typename TContainer::iterator&, const TContainer&) = 0;
   virtual K_mf(bool)   into_new_room (const typename TContainer::const_iterator&, const TContainer&) = 0;
   virtual K_mf(bool)   into_room     (const typename TContainer::iterator&, const TContainer&) = 0;

   /** 如果您的 recorder 與 container 為 [ 1對1 ] 的關係,則不必修改 OnContainerClear()
       否則(一個recorder, 儲存多個container),您必須:
         K_mf(void)  OnContainerClear    (const ev_container&)       { }
         K_mf(void)  OnContainerBfClear  (const ev_container& evc)   { ClearContainsRooms(evc); }
         K_mf(void)  OnContainerBfAssign (const ev_container& evc)   { ClearContainsRooms(evc); }
   */
   virtual K_mf(void)  OnContainerClear  (const ev_container&)  { Inn_.clear(); }

   /** 底下的 mf 處理 Container 事件, 並轉換成對應的儲存動作.
   */
// K_mf(void)  OnContainerBfDestroy  (const ev_container&)     { }
// K_mf(void)  OnContainerBfClear    (const ev_container&)     { }
   K_mf(void)  OnContainerAfClear    (const ev_container& evc) { OnContainerClear(evc); }

// K_mf(void)  OnContainerBfAssign   (const ev_container&) { }
   K_mf(void)  OnContainerAfAssign   (const ev_container& evc)
      {
         OnContainerClear(evc);
         const TContainer&                     recs = static_cast<const TContainer&>(evc);
         typename TContainer::const_iterator   iend = recs.end();
         for(typename TContainer::const_iterator  i = recs.begin();  i != iend;  ++i)
             into_new_room(i, recs);
      }

// K_mf(void)  OnContainerBfInsert   (const ev_container&, const ev_iterator&, size_type) { }
   K_mf(void)  OnContainerAfInsert   (const ev_container& evc, const ev_iterator& iter, size_type count)
                  {
                     if(count > 0) {
                        typename TContainer::iterator   i = static_cast<const typename TContainer::iterator&>(iter);
                        do {
                           into_new_room(i, static_cast<const TContainer&>(evc));
                           if(--count <= 0)
                              break;
                           ++i;
                        } while(1);
                     }
                  }
   K_mf(void)  OnContainerBfErase  (const ev_container& evc, const ev_iterator& itr, size_type count)
                  {
                     if(count > 0) {
                        typename TContainer::iterator i = static_cast<const typename TContainer::iterator&>(itr);
                        do {
                           rr_key   key = get_room_key(i, static_cast<const TContainer&>(evc));
                           if(!(key.is_invalid()))  //曾經 check in 的客倌
                              Inn_.check_out(key);  //要被 erase 之前,需要先 check out
                           if(--count <= 0)
                              break;
                           ++i;
                        } while(1);
                     }
                  }
// K_mf(void)  OnContainerAfErase    (const ev_container&, const ev_iterator&, size_type)

// K_mf(void)  OnContainerBeginUpdate  (const ev_updater_impl&) { ev_mask.reset(cem_OnContainerBeginUpdate); }
   K_mf(void)  OnContainerAfEndUpdate  (const ev_updater_impl& u)
                  {  into_room( static_cast<const typename TContainer::iterator&>(u.get_iterator())
                              , static_cast<const TContainer&>(*u.get_container()));
                  }
};
/////////////////////////////////////////////////////////////////////////////
template <class TMaster, class TRecorder>
class master_container_handler : private container_ev_handler
{
public:
   typedef TMaster                              TMasterContainer;
   typedef typename TMasterContainer::iterator  TMasterIterator;
   typedef typename TRecorder::container_type   TDetailContainer;

   inline explicit K_ctor master_container_handler (TRecorder* recorder)
         : Recorder_(recorder)
         { }

   /** 開始監看某 MasterContainer 的變化,並記錄到 Recorder 之中.
   */
   K_mf(void)  RegAll  (TMasterContainer& recs)
         {  TMasterIterator  iend = recs.end();
            for(TMasterIterator i = recs.begin();  i != iend;  ++i)
               Recorder_->regto(GetDetailContainer(i));
            recs.add_ev_handler(this);
         }
   /** 取消監看某 MasterContainer 的變化,不影響已記錄到 Recorder 的資料.
   */
   K_mf(void) UnregAll (TMasterContainer& recs)
      {  TMasterIterator   iend = recs.end();
         for(TMasterIterator  i = recs.begin();  i != iend;  ++i)
            Recorder_->unreg(GetDetailContainer(i));
         recs.del_ev_handler(this);//fonwin: 2008/10/16
      }

private:
   TRecorder*  Recorder_;

   /** 您必須告知如何取得 DetailContainer,
       我才能幫您儲存 DetailContainer 裡面的元素.
   */
   virtual K_mf(TDetailContainer&)  GetDetailContainer  (TMasterIterator&) = 0;

   K_mf(void) UnregAll (const ev_container& evc)
      {  UnregAll(const_cast<TMasterContainer&>(static_cast<const TMasterContainer&>(evc)));
      }

   K_mf(void) ClearAll (const ev_container& evc)
      {  UnregAll(evc);
         Recorder_->get_inn().clear();
      }

   K_mf(void) OnContainerBfDestroy   (const ev_container& evc)  { UnregAll(evc); }
   K_mf(void) OnContainerBfClear     (const ev_container& evc)  { ClearAll(evc); }
 //K_mf(void) OnContainerAfClear     (const ev_container&)
   K_mf(void) OnContainerBfAssign    (const ev_container& evc)  { ClearAll(evc); }
   K_mf(void) OnContainerAfAssign    (const ev_container& evc)
         {  RegAll(const_cast<TMasterContainer&>(static_cast<const TMasterContainer&>(evc))); }
 //K_mf(void) OnContainerBfInsert    (const ev_container&, const ev_iterator&, size_type) { }
   K_mf(void) OnContainerAfInsert    (const ev_container&, const ev_iterator& i, size_type count)
         {  TMasterIterator itr = static_cast<const TMasterIterator&>(i);
            for(;;) {
               Recorder_->regto(GetDetailContainer(itr));
               if(--count <= 0)
                  break;
               ++itr;
            }
         }
   K_mf(void) OnContainerBfErase     (const ev_container&, const ev_iterator& i, size_type count)
         {  TMasterIterator itr = static_cast<const TMasterIterator&>(i);
            for(;;) {
               TDetailContainer& recs = GetDetailContainer(itr);
               Recorder_->unreg(recs);
               Recorder_->ClearContainsRooms(recs);
               if(--count <= 0)
                  break;
               ++itr;
            }
         }
 //K_mf(void) OnContainerAfErase     (const ev_container&, const ev_iterator& i, size_type count)
 //K_mf(void) OnContainerBeginUpdate (const ev_updater_impl&) { ev_mask.reset(cem_OnContainerBeginUpdate); }
 //K_mf(void) OnContainerAfEndUpdate (const ev_updater_impl& u)
};
/////////////////////////////////////////////////////////////////////////////
template <class TL, class TR>
inline void PeekerAssign (TL& left, const TR& right) {
   left = right;
}
/////////////////////////////////////////////////////////////////////////////
template <class TContainer, class TInn, class TRecType>
class map_recorder2 : public recorder2<TContainer,TInn>
{
   typedef recorder2<TContainer,TInn>     inherited;
public:
   typedef TContainer                     container_type;
   typedef typename TInn::rr_key          rr_key;
   typedef typename TInn::rr_room         rr_room;
   typedef typename TInn::rr_size         rr_size; 
   typedef typename TContainer::key_type  key_type;
   typedef TRecType                       rec_type;

protected:
#ifndef __BORLANDC__
   using inherited::get_inn;
#endif

   // override from InheritedRecorder.
   K_mf(bool)  into_new_room  (const typename container_type::const_iterator& i, const container_type& recs)
      {  i->second.RoomKey_ = get_inn().check_in(0);
         return IntoRoom(i->second.RoomKey_, i->first, i->second);
      }
   K_mf(bool)  into_room  (const typename container_type::iterator& i, const container_type& recs)
      {  return IntoRoom(i->second.RoomKey_, i->first, i->second);
      }
   K_mf(rr_key)  get_room_key   (const typename container_type::iterator& i, const container_type&)
      {  return i->second.RoomKey_;  }

   K_mf(bool)  IntoRoom  (rr_key& rkey, const key_type& key, const rec_type& rec)
      {  typename TInn::rr_live_into    peeker[] = {
                                       { &key, sizeof(key) },
                                       { &rec, sizeof(rec) },
                                       };
         //將資料存入房間
         return get_inn().get_room(rkey).into(rkey, peeker, peeker+numofele(peeker));
      }
public:
   inline K_ctor  map_recorder2  (const std::string& fileName, typename TInn::rr_size roomSize, bool canCheckOut)
      : inherited(fileName, roomSize, canCheckOut)
      { }

   struct TPeeker : public inn_rooms_peeker<TContainer, TInn>
   {  
      inline K_ctor TPeeker(bool isDynamicRoomSize) : IsDynamicRoomSize_(isDynamicRoomSize) {
      }
   private:
      bool IsDynamicRoomSize_;
      // override from inn_rooms_peeker<>.
      K_mf(bool)  peek_room  (container_type& recs, rr_room room)
      {  static key_type   key;
         static rec_type   rec;
         typename TInn::rr_live_peek    peeker[] = {
                                       { &key, sizeof(key) },
                                       { &rec, sizeof(rec) },
                                       };
         //將資料從房間取出
         bool peekResult = IsDynamicRoomSize_
                              ? room.peek_objs_or_fill(peeker, peeker+numofele(peeker), 1)
                              : room.peek(peeker, peeker+numofele(peeker));
         if(!peekResult)
            return false;
         bool                                insResult;
         typename container_type::value_type v(key,rec);
         typename container_type::iterator   i = split_pair_rtn1(recs.insert(v), insResult);
         if(!insResult) {//Key已存在,更新內容.
            typename container_type::updater u(recs.begin_update(i));
            PeekerAssign(*u, rec);
         }
         //把[房間鑰匙]保留,待會兒存檔時,才找得到此人住的房間
         i->second.RoomKey_ = room.get_key();
         return true;
      }
   };
};
//---------------------------------------------------------------------------
//加上這段: BindInn(), TRecs, get_recs(), 可與 TSimpleRecorder 相容.
template <class TContainer, class TInn, class TRecType>
class TSimpleRecorder2 : public map_recorder2<TContainer, TInn, TRecType>
{
   typedef map_recorder2<TContainer, TInn, TRecType>  inherited;
public:
   inline K_ctor  TSimpleRecorder2  (const std::string& fileName, typename TInn::rr_size roomSize, bool canCheckOut = true)
      : inherited(fileName, roomSize, canCheckOut)
      { }

   K_mf(void)  BindInn  ()
      {  typename inherited::TPeeker   peeker(get_inn().is_dynamic_room_size());
         peeker.peek_rooms(Recs_, get_inn());
         this->regto(Recs_);
      }

protected:
#ifndef __BORLANDC__
   using inherited::get_inn;
#endif
   typedef typename inherited::container_type  TRecs;

   inline K_mf(TRecs*) get_recs ()   { return &Recs_; }

   inline K_mf(const TRecs*) get_recs_const () const  { return &Recs_; }

   TRecs    Recs_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif

