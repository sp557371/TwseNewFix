//
// [ev_容器] 的 [載入] 及 [儲存]
//    template inn_rooms_peeker<>
//    template recorder<>
//
// 版本: $Id: recorder.h,v 1.1.1.1 2003/10/24 15:51:17 cvsroot Exp $
//
//---------------------------------------------------------------------------
#ifndef recorderH
#define recorderH
//---------------------------------------------------------------------------
#include "inn.h"
#include "ev_container.h"
//---------------------------------------------------------------------------
namespace Kstl {
//
// Kstl 命名規則說明:請參考 "ev_container.h"
//
/////////////////////////////////////////////////////////////////////////////
template <class TContainer, class TInn>
class inn_rooms_peeker
{
public:
   virtual K_dtor ~inn_rooms_peeker  ()  {};

   K_mf(void)  peek_rooms  (TContainer& recs, TInn& inn)
         {
            typename TInn::iterator  iend = inn.end();
            for(typename TInn::iterator i = inn.begin();  i != iend;  ++i)
                if(!peek_room(recs, *i))
                   break;
         }
private:
   virtual K_mf(bool)  peek_room  (TContainer&, typename TInn::rr_room) = 0;
};
//---------------------------------------------------------------------------
template <class TContainer, class inn_type>
class recorder : private container_ev_handler
{
   typedef recorder<TContainer,inn_type>   this_type;
protected:
   typedef inn_type                    TInn;
   typedef typename TInn::rr_key       rr_key;
   typedef typename TInn::rr_room      rr_room;

private:
   virtual K_mf(bool)    into_new_room  (TInn&, const typename TContainer::const_iterator&) = 0;
   virtual K_mf(bool)    into_room      (TInn&, const typename TContainer::iterator&) = 0;
   virtual K_mf(rr_key)  get_room_key   (TInn&, const typename TContainer::iterator&) = 0;

   //如果您的 recorder 與 inn 為 [ 1對1 ] 的關係,則不必修改 OnContainerClear()
   virtual K_mf(void)  OnContainerClear  (const ev_container&)  { if(inn_) inn_->clear(); }

   TInn* inn_;

public:
           K_ctor  recorder  (TInn* inn)  : inn_(inn) { }
   virtual K_dtor ~recorder  ()           {}

   inline K_mf(void)  reg    (TContainer& evc)  { evc.add_ev_handler(this); }
   inline K_mf(void)  unreg  (TContainer& evc)  { evc.del_ev_handler(this); }

protected:
   inline K_mf(TInn*)  SetInn  (TInn* inn)   { TInn* bf=inn_;  inn_ = inn;  return bf; }
   inline K_mf(TInn*)  GetInn  ()            { return inn_; }

private:
// K_mf(void)  OnContainerBfDestroy  (const ev_container&)     { }
// K_mf(void)  OnContainerBfClear    (const ev_container&)     { }
   K_mf(void)  OnContainerAfClear    (const ev_container& evc) { OnContainerClear(evc); }

// K_mf(void)  OnContainerBfAssign   (const ev_container&) { }
   K_mf(void)  OnContainerAfAssign   (const ev_container& evc)
      {
         OnContainerClear(evc);
         if(inn_==0)
            return;
         const TContainer&                     recs = static_cast<const TContainer&>(evc);
         typename TContainer::const_iterator   iend = recs.end();
         for(typename TContainer::const_iterator  i = recs.begin();  i != iend;  ++i)
             into_new_room(*inn_, i);
      }

// K_mf(void)  OnContainerBfInsert   (const ev_container&, const ev_iterator&, size_type) { }
   K_mf(void)  OnContainerAfInsert   (const ev_container&, const ev_iterator& iter, size_type count)
                  {
                     if(count > 0  &&  inn_) {
                        typename TContainer::iterator   i = static_cast<const typename TContainer::iterator&>(iter);
                        do {
                           into_new_room(*inn_, i);
                           if(--count <= 0)
                              break;
                           ++i;
                        } while(1);
                     }
                  }
   K_mf(void)  OnContainerBfErase  (const ev_container&, const ev_iterator& itr, size_type count)
                  {
                     if(count > 0  &&  inn_) {
                        typename TContainer::iterator i = static_cast<const typename TContainer::iterator&>(itr);
                        do {
                           rr_key   key = get_room_key(*inn_,i);
                           if(!(key.is_invalid()))  //曾經 check in 的客倌
                              inn_->check_out(key);  //要被 erase 之前,需要先 check out
                           if(--count <= 0)
                              break;
                           ++i;
                        } while(1);
                     }
                  }
// K_mf(void)  OnContainerAfErase    (const ev_container&, const ev_iterator&, size_type)

// K_mf(void)  OnContainerBeginUpdate  (const ev_updater_impl&) { ev_mask.reset(cem_OnContainerBeginUpdate); }
   K_mf(void)  OnContainerAfEndUpdate  (const ev_updater_impl& u)
                  {  if(inn_)    into_room(*inn_,static_cast<const typename TContainer::iterator&>(u.get_iterator()));
                  }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif

