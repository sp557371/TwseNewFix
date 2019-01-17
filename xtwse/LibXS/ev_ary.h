//
// template ev_ary<>
//
// 版本: $Id: ev_ary.h,v 1.3 2006/04/03 14:42:54 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef ev_aryH
#define ev_aryH
/////////////////////////////////////////////////////////////////////////////
#include "ev_container.h"
#include "ary_recs.h"
//---------------------------------------------------------------------------
#include <vector>
#include <limits>
/////////////////////////////////////////////////////////////////////////////
namespace Kstl {
//
// Kstl 命名規則說明:請參考 "ev_container.h"
//
/////////////////////////////////////////////////////////////////////////////
template <class Key, class RecT,
          class IndexMaker      = index_maker<Key>,
          class Container       = std::vector<RecT>,
          class EvContainerBase = //null_ev_container<const_ary_base<Key,RecT,IndexMaker,Container>::iterator>
                                  ev_container_t1< const_ary_base<Key,RecT,IndexMaker,Container> >
          >
class ev_ary :  public EvContainerBase,  protected const_ary_base<Key,RecT,IndexMaker,Container>
{
   typedef ev_ary<Key,RecT,IndexMaker,Container,EvContainerBase>  this_type;
   typedef const_ary_base<Key,RecT,IndexMaker,Container>          ary_base_type;
   typedef EvContainerBase                                        ev_inherited;
   typedef typename ary_base_type::index_type                     index_type;
   typedef typename ary_base_type::recs_type                      recs_type;

#ifndef __BORLANDC__
protected:
   using ev_inherited::bf_destroy;
   using ev_inherited::bf_assign;
   using ev_inherited::af_assign;
   using ev_inherited::bf_clear;
   using ev_inherited::af_clear;
public:
   using ary_base_type::get_recs;
#endif

public:
//Types.
   typedef typename ev_inherited::updater                   updater;//不加上這行,gcc會找不到 updater
   typedef typename ary_base_type::key_type                 key_type;
   typedef typename ary_base_type::mapped_type              mapped_type;
   typedef typename ary_base_type::value_type               value_type;
   typedef typename ary_base_type::index_maker_type         index_maker_type;
   typedef typename ary_base_type::allocator_type           allocator_type;
   typedef typename ary_base_type::size_type                size_type;
   typedef typename ary_base_type::difference_type          difference_type;
   typedef typename ary_base_type::pointer                  pointer;
   typedef typename ary_base_type::const_pointer            const_pointer;
   typedef typename ary_base_type::reference                reference;
   typedef typename ary_base_type::const_reference          const_reference;

   typedef EvContainerBase                                  ev_container_base_type;
   typedef typename ev_container_base_type::ev_iterator     ev_iterator;
   typedef typename ev_container_base_type::iterator        iterator;
   typedef typename std::reverse_iterator<iterator>         reverse_iterator;
   typedef typename ary_base_type::const_iterator           const_iterator;
   typedef typename ary_base_type::const_reverse_iterator   const_reverse_iterator;

//construct/copy/destroy
   explicit
    inline K_ctor ev_ary  (const index_maker_type& a = index_maker_type())
                              : ary_base_type(a) { }
   inline K_dtor ~ev_ary  ()  { bf_destroy(); }

   K_mf(this_type&)  operator=  (const this_type& a)  { bf_assign();
                                                        ary_base_type::operator=(a);
                                                        af_assign();
                                                        return *this;  }
//Iterators.
   inline K_mf(iterator)                begin  ()       { return ary_base_type::begin();  }
   inline K_mf(const_iterator)          begin  () const { return ary_base_type::begin();  }
   inline K_mf(iterator)                end    ()       { return ary_base_type::end();    }
   inline K_mf(const_iterator)          end    () const { return ary_base_type::end();    }
   inline K_mf(iterator)                back   ()       { return ary_base_type::back();   }
   inline K_mf(const_iterator)          back   () const { return ary_base_type::back();   }
   inline K_mf(reverse_iterator)        rbegin ()       { return ary_base_type::rbegin(); }
   inline K_mf(const_reverse_iterator)  rbegin () const { return ary_base_type::rbegin(); }
   inline K_mf(reverse_iterator)        rend   ()       { return ary_base_type::rend();   }
   inline K_mf(const_reverse_iterator)  rend   () const { return ary_base_type::rend();   }

//Capacity.
   inline K_mf(size_type)  size     () const  { return ary_base_type::size();     }
   inline K_mf(bool)       empty    () const  { return ary_base_type::empty();    }
   inline K_mf(size_type)  max_size () const  { return ary_base_type::max_size(); }

//Element access.
   inline K_mf(const_reference)  operator[]  (const key_type& key)
   {
      return *fetch(key);
   } //operator[]

   inline K_mf(iterator)  insert  (const iterator&, //positionHint,
                                   const value_type& v)
   {
      return insert_aux(v.first, &v.second).first;
   }
   template<class InputIterator>
    K_mf(void)  insert  (InputIterator first, InputIterator last)
    {
      //__t.insert(first, last);
      while(first != last)    insert(*first++);
    }
   inline std::pair<iterator,bool>  insert  (const value_type& v)
   {
      return insert_aux(v.first, &v.second);
   }

   //erase() 只允許刪除 [第一個] or [最後個] 元素
   struct ary_erase_aux1 {
      this_type*        ary_;
      const iterator&   position_;
      K_ctor  ary_erase_aux1  (this_type* ary, const iterator& position)  : ary_(ary), position_(position) { }
      inline K_mf(void)  erase_begin  (recs_type* recs) const
                                                { ary_->bf_erase(position_,1);
                                                  recs->erase_begin();
                                                  ary_->af_erase(ary_->begin(),1); }
      inline K_mf(void)  erase_back   (recs_type* recs, const typename ary_base_type::orig_iterator& oi) const
                                                { ary_->bf_erase(position_,1);
                                                  recs->erase_not_begin(oi);
                                                  ary_->af_erase(ary_->end(),1); }
      inline K_mf(void)  clear_one  (const typename ary_base_type::iterator&) const
                                                { updater  u = ary_->begin_update(position_);
                                                  *u = mapped_type(); }
   };
   friend struct ary_erase_aux1;
   inline K_mf(bool)  erase  (iterator position)  { return ary_base_type::erase1(position,
                                                                                 ary_erase_aux1(this,position)); }
     K_mf(size_type)  erase  (const key_type& a)  { iterator i = find(a);
                                                    return(i == end() ? 0 : erase(i)); }

   struct ary_erase_aux2 {
      this_type*  ary_;
      iterator&   from_;
      inline K_ctor  ary_erase_aux2  (this_type* ary, iterator& from)
                                          : ary_(ary), from_(from)
                                          { }
      inline K_mf(void)  clear        (recs_type*) const { ary_->clear(); }
             K_mf(void)  erase_begin  (recs_type* recs, size_type count) const
                                          { ary_->bf_erase(from_, count);
                                            recs->erase_begin(count);
                                            ary_->af_erase(ary_->begin(), count);
                                          }
             K_mf(void)  erase_not_begin (recs_type* recs, const typename ary_base_type::orig_iterator& first,
                                                           const typename ary_base_type::orig_iterator& last,
                                                           size_type count) const
                                          { ary_->bf_erase(from_, count);
                                            recs->erase_not_begin(first, last);
                                            ary_->af_erase(ary_->end(),count);
                                          }
             K_mf(void)  clear_one   (const typename ary_base_type::iterator&, const mapped_type& value) const
                                                { updater  u = ary_->begin_update(from_);
                                                  *u = value;
                                                  ++from_; }
   };
   friend struct ary_erase_aux2;
   K_mf(size_type)  erase  (iterator first, iterator last)
                     { return ary_base_type::erase2(first, last, ary_erase_aux2(this,first)); }

   K_mf(void)  swap  (this_type& a)    { bf_assign();
                                         a.bf_assign();
                                         ary_base_type::swap(a);
                                         a.af_assign();
                                         af_assign(); }
   K_mf(void)  clear  ()   { bf_clear();
                             ary_base_type::clear();
                             af_clear(); }

//modifiers
   inline K_mf(void)  reserve  (const key_type& keyfront, const key_type& keyback)
   {  //只做[空間預留],不變動內容
      ary_base_type::reserve(keyfront, keyback);
   }

//ary operations
   inline K_mf(iterator)        find        (const key_type& key)       { return ary_base_type::find(key);        }
   inline K_mf(const_iterator)  find        (const key_type& key) const { return ary_base_type::find(key);        }
   inline K_mf(iterator)        lower_bound (const key_type& key)       { return ary_base_type::lower_bound(key); }
   inline K_mf(const_iterator)  lower_bound (const key_type& key) const { return ary_base_type::lower_bound(key); }
// inline K_mf(iterator)        upper_bound (const key_type& key)       { return ary_base_type::upper_bound(key); }
// inline K_mf(const_iterator)  upper_bound (const key_type& key) const { return ary_base_type::upper_bound(key); }

//ev_ary extended: update record
   inline K_mf(iterator) fetch     (const key_type& key)   { return insert_aux(key, 0).first; }
   inline K_mf(key_type) push_back (const mapped_type& v)  {
      key_type  key = empty() ? key_type() : (end()-1).get_idx();
      insert(value_type(++key, v));
      return key;
   }

//擴充給 ViewArea 使用的 members
  static inline K_mf(const mapped_type&)  get_mapped  (const iterator& i)     { return *i; }
//static inline K_mf(const key_type&)     get_key     (const iterator& i);

//override ev_container virtual functions
   K_mf(size_type)  distance_begin  (const ev_iterator& from) const
                                     { const iterator  ifrom  = static_cast<const iterator&>(from);
                                       return ifrom.get_idx() - get_recs()->get_index_begin(); }

public:
   //↓開放出來給 ary_base<>::ev_ary_insert_aux() 使用
   K_mf(void)  insert_auxev  (recs_type* recs, index_type from, size_type count, const mapped_type& value)
   {
      typedef typename ary_base_type::iterator  ary_base_iterator;
      iterator  itrFrom(ary_base_iterator(recs, static_cast<index_type>(from + recs->get_index_begin())));
      this->bf_insert(itrFrom, count, value);
      recs->insert_from(from, count, value);
      if(from==static_cast<index_type>(0))
         itrFrom = begin();
      this->af_insert(itrFrom, count);
   }
protected:
   struct ary_insert_aux {
      this_type*   ary_;
      K_ctor      ary_insert_aux  (this_type* a)  : ary_(a) { }
      K_mf(void)  operator()  (recs_type* recs, index_type from, size_type count, const mapped_type* value) const
         {
            evary_insert_aux(ary_, recs, from, count, value);
            /*
            if(count<=1  ||  value==0)
               ary_->insert_auxev(recs, from, count, (value ? *value : mapped_type()));
            else
            if(from==0) {
               ary_->insert_auxev(recs, 0, count-1, mapped_type());  //填入[非預期的]資料
               ary_->insert_auxev(recs, 0, 1,       *value);         //填入[要安插的]資料
            } else {
               ary_->insert_auxev(recs, from,   count-1, mapped_type());//填入[非預期的]資料
               ary_->insert_auxev(recs, static_cast<index_type>(from+count-1), 1, *value);//填入[要安插的]資料
            }
            */
         }
   };
   friend struct ary_insert_aux;
   std::pair<iterator,bool>  insert_aux  (const key_type& key, const mapped_type* value)
   {
      //return ary_base_type::insert(key, value, ary_insert_aux(this));
      std::pair<typename ary_base_type::iterator,bool>   result = ary_base_type::insert(key, value, ary_insert_aux(this));
      return std::pair<iterator,bool>(result.first, result.second);
   }
//data members
};// class ev_ary
//---------------------------------------------------------------------------
template <class Key, class RecT, class IndexMaker, class Container, class EvContainerBase>
inline void swap (ev_ary<Key,RecT,IndexMaker,Container,EvContainerBase>& a,
                  ev_ary<Key,RecT,IndexMaker,Container,EvContainerBase>& b)  { a.swap(b); }
/////////////////////////////////////////////////////////////////////////////
template <class arybase, // 您必須先定義好 typedef Kstl::ary_base<...>   arybase;
          class EvContainerBase>
class ev_ary2 : public ev_ary<typename arybase::key_type,
                              typename arybase::mapped_type,
                              typename arybase::index_maker_type,
                              typename arybase::recs_org_type,
                                       EvContainerBase>
{
};
//---------------------------------------------------------------------------
template <class arybase, class EvContainerBase>
inline void swap(ev_ary2<arybase,EvContainerBase>& a,
                 ev_ary2<arybase,EvContainerBase>& b)
  {
    a.swap(b);
  }
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
/////////////////////////////////////////////////////////////////////////////
#endif

