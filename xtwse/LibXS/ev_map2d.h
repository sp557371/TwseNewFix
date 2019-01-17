//
// template ev_map2d<>
//
// 版本: $Id: ev_map2d.h,v 1.4 2005/12/28 07:58:28 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef ev_map2dH
#define ev_map2dH
//---------------------------------------------------------------------------
#include "ev_container.h"
#include "kutility.h"
//---------------------------------------------------------------------------
namespace Kstl {
//
// ev_map2d<>主要是將一個 [Master-Detail的容器] 包裝起來,
// 讓外界看起來像是操作一個 [單一的容器]
//
// 較常用到的地方是 recorder<> 要將資料[存放到/取出自] Inn
// [不]可給 ViewArea 使用(ViewArea要顯示Master-Detail容器,請參考 ViewAreaMgr.h 的做法)
//
/////////////////////////////////////////////////////////////////////////////
/*
template <class Iterator>
struct map2d_iterator_value_type
{
   typedef Iterator::key_type       first_type;
   typedef Iterator::mapped_type    second_type;

   K_ctor  map2d_iterator_value_type  (const Iterator* i)  : iterator_(i) {}
   const Iterator*   iterator_;
};
*/
//---------------------------------------------------------------------------
/*
template <class this_map2d, class TMapped, class nc_this_type>
class map2d_iterator_base : public ev_iterator
{
   typedef map2d_iterator_base<this_map2d,TMapped,nc_this_type> this_type;
public:
   typedef typename this_map2d::detail_map_type    detail_map_type;
   typedef typename this_map2d::master_iterator    master_iterator;
   typedef typename this_map2d::detail_iterator    detail_iterator;
   typedef typename this_map2d::key_type           key_type;
   typedef const typename this_map2d::mapped_type  mapped_type;

   K_ctor  map2d_iterator_base  (this_map2d* m)
               : map2d_(m), is_end_(true)
               { }
   K_ctor  map2d_iterator_base  (this_map2d* m, const master_iterator& i)
               : map2d_(m), iMaster_(i)
               { check_r(); }
   K_ctor  map2d_iterator_base  (this_map2d* m, const master_iterator& iMaster, const detail_iterator& iDetail)
               : map2d_(m), iMaster_(iMaster), iDetail_(iDetail), is_end_(false)
               { }
   K_ctor  map2d_iterator_base  (const nc_this_type& i)
               : map2d_(i.get_map2d()),
                 iMaster_(i.get_iMaster()),
                 iDetail_(i.get_iDetail()),
                 is_end_(i.is_end())
               { }

   K_mf(bool)  operator!=  (const this_type& i) const  { return !(operator==(i)); }
   K_mf(bool)  operator==  (const this_type& i) const  { return  (is_end_  &&  i.is_end_)
                                                              || (  map2d_   == i.map2d_
                                                                 && iMaster_ == i.iMaster_
                                                                 && iDetail_ == i.iDetail_ ); }
   K_mf(mapped_type&)  get_mapped () const   { return iDetail_->second; }
   K_mf(key_type)      get_key    () const   { return key_type(map2d_->get_master()->get_key(iMaster_),
                                                               map2d_->get_detail_map(*iMaster_)->get_key(iDetail_)); }

   K_mf(this_map2d*)            get_map2d    () const  { return map2d_;   }
   K_mf(const master_iterator)  get_iMaster  () const  { return iMaster_; }
   K_mf(const detail_iterator)  get_iDetail  () const  { return iDetail_; }
   K_mf(bool)                   is_end       () const  { return is_end_;  }

   K_mf(this_type&)  operator++  ()  { if(is_end_)
                                          return *this;
                                       detail_map_type*  dmap = map2d_->get_detail_map(*iMaster_);
                                       if(dmap==0  ||  dmap->empty())
                                          check_r();
                                       else
                                       if(++iDetail_ == dmap->end()) {
                                          ++iMaster_;
                                          check_r();
                                       }
                                       return *this;
                                     }

   struct value_type
   {
      typedef key_type       first_type;
      typedef mapped_type    second_type;

      K_ctor  value_type  (const this_type* i)  : iterator_(i) {}
      const this_type*   iterator_;
   };
   //typedef map2d_iterator_value_type<this_type>    value_type;
   K_mf(value_type)  operator*  () const  { return value_type(this); }
private:
   this_map2d*       map2d_;
   master_iterator   iMaster_;
   detail_iterator   iDetail_;
   bool              is_end_;
   K_mf(void)  check_r  ()
         {
            master_iterator   iend = map2d_->get_master()->end();
            while(iMaster_ != iend) {
               detail_map_type*  dmap = map2d_->get_detail_map(*iMaster_);
               if(dmap==0  ||  dmap->empty())
                  ++iMaster_;
               else {
                  iDetail_ = dmap->begin();
                  is_end_ = false;
                  return;
               }
            }
            is_end_ = true;
         }
   K_mf(bool)  operator==  (const ev_iterator& a) const  { return operator==(static_cast<const this_type&>(a)); }
};
*/
/////////////////////////////////////////////////////////////////////////////
template <class MasterMap, class DetailMap = typename MasterMap::mapped_type>
struct map2d_maker_base
{
   typedef MasterMap                               master_map_type;
   typedef typename master_map_type::iterator      master_iterator;
   typedef typename master_map_type::key_type      master_key_type;
   typedef typename master_iterator::value_type    master_value_type;
   typedef DetailMap                               detail_map_type;
   typedef typename detail_map_type::iterator      detail_iterator;
   typedef typename detail_map_type::key_type      detail_key_type;
   typedef typename detail_map_type::value_type    detail_value_type;

   typedef typename detail_map_type::mapped_type   mapped_type;
   struct key_type {
      master_key_type   master_key_;
      detail_key_type   detail_key_;

      K_ctor  key_type  () {}
      K_ctor  key_type  (const master_key_type& mkey, const detail_key_type& dkey)
                           : master_key_(mkey), detail_key_(dkey)
                           { }
   };

   //只要比照此處的 prototype, 您可以覆寫底下這些 members
   struct value_type {
      key_type             key_;
      detail_value_type    detail_value_;
      K_ctor  value_type  (const key_type& k, const mapped_type& m)
                              : key_(k), detail_value_(k.detail_key_, m)
                              { }
      K_ctor  value_type  ()  { }
   };
   static inline K_mf(const master_key_type)    get_master_key   (const value_type& v)              { return v.key_.master_key_; }
          inline K_mf(const detail_map_type*)   get_detail_map   (const master_value_type& v) const { return &(v.second);        }
          inline K_mf(detail_map_type*)         get_detail_map   (      master_value_type& v) const { return &(v.second);        }
   static inline K_mf(detail_map_type*)         fetch_detail_map (const master_iterator&   i)       { return &(i->second);       }
   static inline K_mf(const detail_value_type&) get_detail_value (const value_type& v)              { return v.detail_value_;    }
};// map2d_maker
//---------------------------------------------------------------------------
//另一個您要完成的重點是: 當有一個 detail_map 時, 如何找到 master_iterator:
template <class MapMakerBase>
class map2d_maker_seq_find : public MapMakerBase
{
   typedef MapMakerBase    inherited;
public:
   typedef typename inherited::master_map_type  master_map_type;
   typedef typename inherited::detail_map_type  detail_map_type;
   typedef typename inherited::master_iterator  master_iterator;

   K_mf(bool)  find_master_iterator  (master_map_type& mmap, const detail_map_type& dmap, master_iterator& imaster)
      { if(0){}//如果您選用 [循序搜尋] 的方式尋找 master_iterator,
               //您將會在這兒得到一個警告,因為當master的資料量很大時,效率會大幅降低!!
        master_iterator  iend = mmap.end();
        for(master_iterator i = mmap.begin();  i != iend;  ++i)
            if(&dmap == get_detail_map(*i)) {
               imaster = i;
               return true;
            }
        return false;
      }
   static inline K_mf(void)  detail_set_master_key  (detail_map_type&, const typename inherited::master_key_type&)   {}
};
//---------------------------------------------------------------------------
template <class MapMakerBase>
struct map2d_maker_key_find : public MapMakerBase
{
   typedef MapMakerBase    inherited;
public:
   typedef typename inherited::master_map_type  master_map_type;
   typedef typename inherited::detail_map_type  detail_map_type;
   typedef typename inherited::master_iterator  master_iterator;

   //或者,您可以在 detail_map 之中存放一個 master_key:
   static K_mf(bool)  find_master_iterator  (master_map_type& mmap, const detail_map_type& dmap, master_iterator& imaster)
      { master_iterator ifind = mmap.find(dmap.get_master_key());
        if(ifind==mmap.end())
           return false;
        imaster = ifind;
        return true;
      }
   static inline K_mf(void)  detail_set_master_key  (detail_map_type& dmap, const typename inherited::master_key_type& key)
      { dmap.set_master_key(key); }
};
/////////////////////////////////////////////////////////////////////////////
template <class MapMaker>
class ev_map2d : public  ev_container,
                 private container_ev_handler,
                 public  MapMaker
{
   typedef ev_map2d<MapMaker>  this_type;
   typedef this_type           this_map2d;
   struct  nil {};
public:
   //Types.
   typedef typename MapMaker::master_map_type      master_map_type;
   typedef typename master_map_type::iterator      master_iterator;
   typedef typename master_iterator::value_type    master_value_type;
   typedef typename MapMaker::detail_map_type      detail_map_type;
   typedef typename detail_map_type::iterator      detail_iterator;
   typedef typename detail_map_type::mapped_type   mapped_type;
   typedef typename MapMaker::key_type             key_type;
   typedef typename MapMaker::value_type           value_type;
   typedef size_t                                  size_type;

   //Iterators
   /*
   typedef map2d_iterator_base<this_type,       mapped_type,       nil>          iterator;
   typedef map2d_iterator_base<const this_type, const mapped_type, iterator_t>   const_iterator;
   */
   class iterator : public ev_iterator
   {
      typedef iterator   this_type;
   public:
      K_ctor  iterator  (this_map2d* m)
                  : map2d_(m), is_end_(true)
                  { }
      K_ctor  iterator  (this_map2d* m, const master_iterator& i)
                  : map2d_(m), iMaster_(i)
                  { check_r(); }
      K_ctor  iterator  (this_map2d* m, const master_iterator& iMaster, const detail_iterator& iDetail)
                  : map2d_(m), iMaster_(iMaster), iDetail_(iDetail), is_end_(false)
                  { }

      K_mf(bool)  operator!=  (const this_type& i) const  { return !(operator==(i)); }
      K_mf(bool)  operator==  (const this_type& i) const  { return  (is_end_  &&  i.is_end_)
                                                                 || (  map2d_   == i.map2d_
                                                                    && iMaster_ == i.iMaster_
                                                                    && iDetail_ == i.iDetail_ ); }
      K_mf(mapped_type&)  get_mapped () const   { return const_cast<mapped_type&>(iDetail_->second); }
      K_mf(key_type)      get_key    () const   { return key_type(map2d_->get_master()->get_key(iMaster_),
                                                                  map2d_->get_detail_map(*iMaster_)->get_key(iDetail_)); }

      K_mf(this_map2d*)            get_map2d    () const  { return map2d_;   }
      K_mf(const master_iterator)  get_iMaster  () const  { return iMaster_; }
      K_mf(const detail_iterator)  get_iDetail  () const  { return iDetail_; }
      K_mf(bool)                   is_end       () const  { return is_end_;  }

      K_mf(this_type&)  operator++  ()  { if(is_end_)
                                             return *this;
                                          detail_map_type*  dmap = map2d_->get_detail_map(*iMaster_);
                                          if(dmap==0  ||  dmap->empty())
                                          check_r();
                                          else
                                          if(++iDetail_ == dmap->end()) {
                                             ++iMaster_;
                                             check_r();
                                          }
                                          return *this;
                                        }

      struct value_type
      {
         typedef key_type       first_type;
         typedef mapped_type    second_type;

         K_ctor  value_type  (const this_type* i)  : iterator_(i) {}
         const this_type*   iterator_;
      };
      K_mf(value_type)  operator*  () const  { return value_type(this); }
   private:
      this_map2d*       map2d_;
      master_iterator   iMaster_;
      detail_iterator   iDetail_;
      bool              is_end_;
      K_mf(void)  check_r  ()
         {
            master_iterator   iend = map2d_->get_master()->end();
            while(iMaster_ != iend) {
               detail_map_type*  dmap = map2d_->get_detail_map(*iMaster_);
               if(dmap==0  ||  dmap->empty())
                  ++iMaster_;
               else {
                  iDetail_ = dmap->begin();
                  is_end_ = false;
                  return;
               }
            }
            is_end_ = true;
         }
      K_mf(bool)  operator==  (const ev_iterator& a) const  { return operator==(static_cast<const this_type&>(a)); }
   };
   class const_iterator : public iterator {
      typedef iterator  inherited;
   public:
      inline K_ctor  const_iterator  (const this_map2d* m)
                        : inherited(const_cast<this_map2d*>(m))
                        { }
      inline K_ctor  const_iterator  (const this_map2d* m, const master_iterator& i)
                        : inherited(const_cast<this_map2d*>(m), i)
                        { }
      inline K_ctor  const_iterator  (const this_map2d* m, const master_iterator& iMaster, const detail_iterator& iDetail)
                        : inherited(const_cast<this_map2d*>(m),iMaster,iDetail)
                        { }
      inline K_ctor  const_iterator  (const iterator& i)
                        : iterator(i)
                        { }
   };

   //constructor/destructor/
   K_ctor   ev_map2d  (master_map_type* m)
                           : detail_handler_(this), master_(m)
                           { if(m) {
                               m->add_ev_handler(this);
                               reg_details_ev();
                             }  
                           }
   K_dtor  ~ev_map2d  ()   { if(master_) {
                                bf_destroy();
                                master_->del_ev_handler(this);
                                unreg_details_ev();
                             }
                           }
   //必須提供的methods
   std::pair<iterator,bool>  insert  (const value_type& v)//給 recorder<> 取出資料時使用
      {
      master_iterator   im   = master_->fetch(this->get_master_key(v));
      detail_map_type*  dmap = this->fetch_detail_map(im);
      std::pair<detail_iterator,bool>  dv = dmap->insert(this->get_detail_value(v));
      return std::make_pair(iterator(this, im, dv.first), dv.second);
      }

   K_mf(iterator)        begin  ()        { return iterator(this, master_->begin()); }
   K_mf(iterator)        end    ()        { return iterator(this); }
   K_mf(const_iterator)  begin  () const  { return const_iterator(this, master_->begin()); }
   K_mf(const_iterator)  end    () const  { return const_iterator(this); }

   K_mf(master_map_type*)  get_master  () const  { return master_; }

   static inline K_mf(const key_type)      get_key     (const iterator& i)        { return i.get_key();    }
   static inline K_mf(const mapped_type&)  get_mapped  (const iterator& i)        { return i.get_mapped(); }
   static inline K_mf(const key_type)      get_key     (const const_iterator& i)  { return i.get_key();    }
   static inline K_mf(const mapped_type&)  get_mapped  (const const_iterator& i)  { return i.get_mapped(); }
private:
   struct updater_impl : public ev_updater_impl {
      K_ctor   updater_impl   (ev_container& c, const ev_iterator& i)
                  : ev_updater_impl(c),
                    Iter_(static_cast<const iterator&>(i))
                  { }
      K_mf(const iterator&)  get_iterator  () const   { return Iter_; }
   protected:
      iterator    Iter_;
   };
   //不支援 ViewArea, 所以不考慮 distance_begin()
   K_mf(size_type)         distance_begin   (const ev_iterator&) const  { return 0; }
   K_mf(ev_updater_impl*)  updater_factory  (const ev_iterator& i)      { return new updater_impl(*this,i); }

   struct TFunArgs {
      master_iterator*  masteri_;
      detail_map_type*  dmap_;
   };
   template <class Function>
   K_mf(Function)  for_each_detail  (master_iterator first, size_type n, Function f)
      { if(n > 0) {
           TFunArgs  args = { &first };
           do {
             if((args.dmap_ = this->get_detail_map(*first)) != 0)
                f(args);
             if(--n <= 0)
               break;
             ++first;
           } while(1);
        }
        return f;
      }
   struct detail_counter {
      size_type   sz_;
      K_ctor      detail_counter  ()  : sz_(0)  {}
      K_mf(void)  operator() (TFunArgs& args)  { sz_ += args.dmap_->size(); }
   };
   K_mf(size_type)  count_details  (const master_iterator& i, size_type n)
                                    { return for_each_detail(i, n, detail_counter()).sz_; }

        void   reg_detail        (TFunArgs& args)
                                     { this->detail_set_master_key(*args.dmap_, get_master()->get_key(*args.masteri_));
                                       args.dmap_->add_ev_handler(&detail_handler_); }
   K_mf(void)  reg_details_ev    ()  { for_each_detail(master_->begin(), master_->size(),
                                                       make_closure(this, std::mem_fun(&this_type::reg_detail)));
                                     }
        void   unreg_detail      (TFunArgs& args)
                                     { args.dmap_->del_ev_handler(&detail_handler_); }
   K_mf(void)  unreg_details_ev  ()  { for_each_detail(master_->begin(), master_->size(),
                                                       make_closure(this, std::mem_fun(&this_type::unreg_detail)));
                                     }
   template <class call_ev>
     K_mf(void)  do_event  (call_ev doEv, const ev_iterator& i, size_type n)
                                       { master_iterator im = static_cast<const master_iterator&>(i);
                                         size_type detail_n = count_details(im,n);
                                         if(detail_n > 0)
                                            (this->*doEv)(iterator(this, im), detail_n);
                                       }
   template <class call_ev, class call_reg>
     K_mf(void)  do_event  (call_ev doEv, call_reg doReg, const ev_iterator& i, size_type n)
                                       { do_event(doEv, i, n);
                                         for_each_detail( static_cast<const master_iterator&>(i), n,
                                                          make_closure(this,std::mem_fun(doReg)) );
                                       }
   // Master ev handler
     K_mf(void)  OnContainerBfDestroy (const ev_container&) { bf_destroy();  master_ = 0;        }
     K_mf(void)  OnContainerBfClear   (const ev_container&) { bf_clear();    unreg_details_ev(); }
     K_mf(void)  OnContainerAfClear   (const ev_container&) { af_clear();                        }
     K_mf(void)  OnContainerBfAssign  (const ev_container&) { bf_assign();   unreg_details_ev(); }
     K_mf(void)  OnContainerAfAssign  (const ev_container&) { reg_details_ev();     af_assign(); }
   //K_mf(void)  OnContainerBfInsert  (const ev_container&, const ev_iterator& i, size_type n);
     K_mf(void)  OnContainerAfInsert  (const ev_container&, const ev_iterator& i, size_type n)
                                       { do_event(&this_type::af_insert, &this_type::reg_detail, i, n); }
     K_mf(void)  OnContainerBfErase  (const ev_container&, const ev_iterator& i, size_type n)
                                       { do_event(&this_type::bf_erase, &this_type::unreg_detail, i, n); }
     K_mf(void)  OnContainerAfErase  (const ev_container&, const ev_iterator& i, size_type n)
                                       { af_erase(iterator(this,static_cast<const master_iterator&>(i)), n); }
   //↓資料異動
   //K_mf(void)  OnContainerBeginUpdate  (const ev_updater_impl&) { ev_mask_.reset(cem_OnContainerBeginUpdate); }
     K_mf(void)  OnContainerAfEndUpdate  (const ev_updater_impl& u)
         { master_iterator    im = static_cast<const master_iterator&>(u.get_iterator());
           TFunArgs           args = { &im, this->get_detail_map(*im) };
           if(args.dmap_)
              reg_detail(args);
         }
   //-----------------------------------------------------
   class detail_ev_handler : public container_ev_handler {
      this_map2d*  map2d_;
   public:
      K_ctor  detail_ev_handler  (this_map2d* m)  : map2d_(m)  {}
   private:
        K_mf(void)  BfDetailClear  (const detail_map_type& dmap)
            { if(dmap.empty())
                 return;
              master_map_type*  master = map2d_->get_master();
              if(master==0)//也許master正在解構!
                 return;
              master_iterator   imaster;
              if(map2d_->find_master_iterator(*master, dmap, imaster))
                 map2d_->bf_erase(iterator(map2d_,imaster), dmap.size());
            }
      // Detail ev handler
        K_mf(void)  OnContainerBfDestroy  (const ev_container& c)  { BfDetailClear(static_cast<const detail_map_type&>(c)); }
        K_mf(void)  OnContainerBfClear    (const ev_container& c)  { BfDetailClear(static_cast<const detail_map_type&>(c)); }
      //K_mf(void)  OnContainerAfClear    (const ev_container& c)  { af_clear();   }
        K_mf(void)  OnContainerBfAssign   (const ev_container& c)  { BfDetailClear(static_cast<const detail_map_type&>(c)); }
        K_mf(void)  OnContainerAfAssign   (const ev_container& c)
            { detail_map_type&  dmap = const_cast<detail_map_type&>(static_cast<const detail_map_type&>(c));
              if(dmap.empty())
                 return;
              master_iterator imaster;
              if(map2d_->find_master_iterator(*(map2d_->get_master()), dmap, imaster))
                 map2d_->do_event(&this_map2d::af_insert, imaster, 1);//fonwin:20050920: 此處應該使用imaster的數量, 原本使用 dmap.size() 是錯的.
            }
      //↓(ev_iterator& ifrom, size_type count)
        K_mf(void)  do_event_chk_dend  ( void (K_FASTCALL this_map2d::*doEv) (const ev_iterator&, size_type)
                                       , const ev_container& c, const ev_iterator& i, size_type n)
            { master_iterator          imaster;
              const detail_map_type&   dmap = static_cast<const detail_map_type&>(c);
              if(map2d_->find_master_iterator(*(map2d_->get_master()), dmap, imaster)) {
                 detail_iterator di = static_cast<const detail_iterator&>(i);
                 (map2d_->*doEv)( dmap.end()==di ? iterator(map2d_,++imaster)
                                                 : iterator(map2d_,imaster,di),
                                  n );
              }
            }
        K_mf(void)  do_event  ( void (K_FASTCALL this_map2d::*doEv) (const ev_iterator&, size_type)
                              , const ev_container& c, const ev_iterator& i, size_type n)
            { master_iterator          imaster;
              const detail_map_type&   dmap = static_cast<const detail_map_type&>(c);
              if(map2d_->find_master_iterator(*(map2d_->get_master()), dmap, imaster))
                 (map2d_->*doEv)(iterator(map2d_,imaster,static_cast<const detail_iterator&>(i)), n);
            }

        K_mf(void)  OnContainerBfInsert  (const ev_container& c, const ev_iterator& i, size_type n)   { do_event_chk_dend(&this_map2d::bf_insert, c, i, n); }
        K_mf(void)  OnContainerAfInsert  (const ev_container& c, const ev_iterator& i, size_type n)   { do_event(&this_map2d::af_insert, c, i, n); }
        K_mf(void)  OnContainerBfErase   (const ev_container& c, const ev_iterator& i, size_type n)   { do_event(&this_map2d::bf_erase, c, i, n); }
        K_mf(void)  OnContainerAfErase   (const ev_container& c, const ev_iterator& i, size_type n)   { do_event_chk_dend(&this_map2d::af_erase, c, i, n); }
      //↓資料異動
        K_mf(void)  do_event  (void (K_FASTCALL this_map2d::*doEv) (ev_updater_impl&), const ev_updater_impl& u)
            { master_iterator   im;
              if(map2d_->find_master_iterator(*(map2d_->get_master()), static_cast<const detail_map_type&>(*u.get_container()),im)) {
                 updater_impl    upr(*map2d_, iterator(map2d_, im, static_cast<const detail_iterator&>(u.get_iterator())));
                 (map2d_->*doEv)(upr);
              }
            }
        K_mf(void)  OnContainerBeginUpdate  (const ev_updater_impl& u)
            { do_event(&this_map2d::call_begin_update_evs, u); }
        K_mf(void)  OnContainerAfEndUpdate  (const ev_updater_impl& u)
            { do_event(&this_map2d::call_end_update_evs, u); }
   };
   friend class detail_ev_handler;
   detail_ev_handler    detail_handler_;
   master_map_type*     master_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif
