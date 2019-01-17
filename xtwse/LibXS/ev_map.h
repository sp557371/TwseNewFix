//
// template ev_map<>
//
// 版本: $Id: ev_map.h,v 1.5 2006/04/03 14:28:44 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef ev_mapH
#define ev_mapH
//---------------------------------------------------------------------------
#include "ev_container.h"
#include <map>
//---------------------------------------------------------------------------
namespace Kstl {
//
// Kstl 命名規則說明:請參考 "ev_container.h"
//
//為了避免無法掌握 Rec 的改變, 建議將 RecT 設為 const
//當有異動的需求時, 則使用 EvContainerBase::begin_update()
//詳細的說明請參考 "ev_container.h"
//
/////////////////////////////////////////////////////////////////////////////
template <class Key, class RecT,
#ifdef __alpha//為了縮短在alpha機器上的 symbol name
          class EvContainerBase = ev_container_t2<std::map<Key,RecT> >
#else
          class Compare         = std::less<Key>,
          class Allocator       = std::allocator<std::pair<const Key, RecT> >,
          class EvContainerBase = ev_container_t2<std::map<Key,RecT,Compare,Allocator> >
#endif
          >
class ev_map : public EvContainerBase
{
#ifdef __alpha
   typedef ev_map<Key, RecT, EvContainerBase>                     this_type;
   typedef std::less<Key>                                         Compare;
   typedef std::allocator<std::pair<const Key, RecT> >            Allocator;
#else
   typedef ev_map<Key, RecT, Compare, Allocator, EvContainerBase> this_type;
#endif
   typedef std::map<Key,RecT,Compare,Allocator>                   like_type;
   typedef EvContainerBase                                        inherited;
public:
//Types.
    typedef Key                           key_type;
    typedef RecT                          mapped_type;
    typedef EvContainerBase               ev_container_base_type;
    typedef std::pair<const Key, RecT>    value_type;
    typedef Compare                       key_compare;
    typedef Allocator                     allocator_type;
private:
    /*typedef __RWSTD::__rb_tree<key_type, value_type,
                               __RWSTD::__select1st<value_type, key_type>,
                               key_compare, allocator_type> __rep_type;*/
    typedef std::map<key_type, mapped_type, key_compare, allocator_type>   __rep_type;
    __rep_type       __t;

#ifndef __BORLANDC__
protected:
    using inherited::bf_destroy;
    using inherited::bf_assign;
    using inherited::af_assign;
    using inherited::bf_clear;
    using inherited::af_clear;
#endif

public:
//Types.
    typedef typename __rep_type::reference                reference;
    typedef typename __rep_type::pointer                  pointer;
    typedef typename __rep_type::const_reference          const_reference;
    typedef typename __rep_type::const_pointer            const_pointer;
    typedef typename __rep_type::size_type                size_type;
    typedef typename __rep_type::difference_type          difference_type;
//Iterator Types.
  //typedef typename __rep_type::iterator                 iterator;
    typedef typename __rep_type::const_iterator           const_iterator;
  //typedef typename __rep_type::reverse_iterator         reverse_iterator;
    typedef typename __rep_type::const_reverse_iterator   const_reverse_iterator;

    typedef typename ev_container_base_type::ev_iterator  ev_iterator;
    typedef typename ev_container_base_type::iterator     iterator;
    typedef typename std::reverse_iterator<iterator>      reverse_iterator;

//construct/copy/destroy
    explicit
      K_ctor  ev_map (const Compare& comp = Compare(), const Allocator& alloc = Allocator())
                     //: __t(comp, false, alloc) {}
                       : __t(comp, alloc) {}

    template<class InputIterator>
     K_ctor  ev_map (InputIterator first, InputIterator last, const Compare& comp = Compare(),
                                                            const Allocator& alloc = Allocator())
                     //: __t(first, last, comp, false, alloc) {}
                       : __t(first, last, comp, alloc) {}

   inline K_ctor   ev_map (const this_type& x) : __t(x.__t) {}//: __t(x.__t, false) {}
   inline K_dtor  ~ev_map ()                   { bf_destroy(); }

    K_mf(this_type&)  operator=  (const this_type& x) { bf_assign();
                                                        __t = x.__t;
                                                        af_assign();
                                                        return *this; }

   inline K_mf(allocator_type)  get_allocator  () const  { return __t.get_allocator(); }

   inline K_mf(bool) operator== (const this_type& x) const { return __t == x.__t; }
   inline K_mf(bool) operator!= (const this_type& x) const { return __t != x.__t; }

//iterators
   inline K_mf(iterator)               begin  ()       { return __t.begin();  }
   inline K_mf(const_iterator)         begin  () const { return __t.begin();  }
   inline K_mf(iterator)               end    ()       { return __t.end();    }
   inline K_mf(const_iterator)         end    () const { return __t.end();    }

   /** 如果使用BCB4(BCB5,BCB6已修正),則需要修改 iterator.h
         原本:
            template <class U>
            reverse_iterator (const reverse_iterator<U>& x) : current(x) {}
         修改後:
            template <class U>
            reverse_iterator (const reverse_iterator<U>& x) : current(x.base()) {}
   */
   inline K_mf(reverse_iterator)       rbegin ()       { return __t.rbegin(); }
   inline K_mf(const_reverse_iterator) rbegin () const { return __t.rbegin(); }
   inline K_mf(reverse_iterator)       rend   ()       { return __t.rend();   }
   inline K_mf(const_reverse_iterator) rend   () const { return __t.rend();   }

//capacity
   inline K_mf(bool)       empty    () const { return __t.empty();    }
   inline K_mf(size_type)  size     () const { return __t.size();     }
   inline K_mf(size_type)  max_size () const { return __t.max_size(); }

//element access
   K_mf(mapped_type&)  operator[]  (const key_type& k)
   {
      value_type tmp(k,mapped_type());
      return (*((insert(tmp)).first)).second;
   }
   std::pair<iterator,bool>  insert  (const value_type& v)
   {
      iterator position = lower_bound(v.first);
      if(position != end()) {
         key_compare keycomp = __t.key_comp();
         if(!keycomp(position->first, v.first)   //key相同
         && !keycomp(v.first, position->first))  //不需要 insert()
            return std::pair<iterator,bool>(position,false);
      }
      this->bf_insert(position,1,v);
      std::pair<iterator,bool>   r(__t.insert(position,v),true);
      this->af_insert(r.first,1);
      return r;
   }
   K_mf(iterator)  insert  (iterator position, const value_type& v)
   {
      key_compare    keycomp = __t.key_comp();
      iterator       iend    = end();
      if(position == iend) {                          //position==end
         if(empty())                                  //  is empty
            return insert(v).first;                   //  => normal insert
         // v 應該要在 rightmost 之後
         --iend;
         if(keycomp(v.first, iend->first)) // v 在 rightmost 之前 => position is incorrect
            return insert(v).first;        //   => normal insert
                                           // v 不在 rightmost 之前(=之後or相等)
         if(keycomp(iend->first, v.first)) {// => 且 rightmost 在 v 之前
                                            //       => v 在 rightmost 之後 => position is correct
         } else                             // => 且 rightmost 不在 v 之前
            return iend;                    //       => v 的位置就在 rightmost
      } else {
         if(keycomp(v.first, position->first)) { // v 在 position 之前 => position maybe correct
            if(position == begin()) {            //   position==begin  => position is correct
            } else {                             // 判斷:若 v 在 before(=position-1)之後
               iterator before = position;       //      則 position is correct
               --before;
               if(keycomp(v.first, before->first))// v 在 before 之前 => position is incorrect
                  return insert(v).first;         //   => normal insert
               // v 在 before 之後or相等
               if(keycomp(before->first, v.first)) {// before 在 v 之前 => v 在 before 之後
                                                    //    position is correct
               } else                               // => v 的位置就在 before
                  return before;                    //    return before;
            }
         } else// v 在 position 之後or相等
         if(keycomp(position->first, v.first))  // position 在 v 之前 => v 在 position 之後
            return insert(v).first;             //    => position is incorrect => normal insert
         else                                   // => v 的位置就在 position
            return position;                    //      return position
      }
      //position 的位置是正確的:
      this->bf_insert(position,1,v);
      position = __t.insert(position, v);
      this->af_insert(position,1);
      return position;
   }

   template<class InputIterator>
    K_mf(void)  insert  (InputIterator first, InputIterator last)
    {
      //__t.insert(first, last);
      while(first != last)    insert(*first++);
    }

   K_mf(bool)  erase  (iterator position)
                        {  this->bf_erase(position,1);
                           iterator afpos = position;
                           bool     isend = (++afpos == end());//刪除最後一個
                           __t.erase(position);
                           this->af_erase((isend ? end() : afpos), 1);
                           return true;
                        }
   K_mf(size_type)  erase  (const key_type& x)
                        {
                           iterator i = find(x);
                           if(i == end())
                              return 0;
                           return erase(i);
                        }
   K_mf(size_type)  erase  (iterator first, iterator last)
                        {
                          iterator iend = end();
                          if(first==begin()  &&  last==iend) {
                             size_type sz = size();
                             clear();
                             return sz;
                          }
                          size_type  count = std::distance(first,last);
                          if(count > 0) {
                              this->bf_erase(first,count);
                              bool  isend = (last==iend);
                              __t.erase(first,last);
                              this->af_erase((isend ? end() : last), count);
                          } // if(count > 0
                          return count;
                        }

   K_mf(void)  swap  (this_type& v)  { bf_assign();
                                       v.bf_assign();
                                       __t.swap(v.__t);
                                       v.af_assign();
                                       af_assign(); }
   K_mf(void)  clear  ()   { bf_clear();
                             __t.erase(begin(), end());
                             af_clear(); }

//observers
   inline K_mf(key_compare)   key_comp   () const { return __t.key_comp(); }

//map operations
   inline K_mf(iterator)       find (const key_type& x)        { return __t.find(x);  }
   inline K_mf(const_iterator) find (const key_type& x)  const { return __t.find(x);  }
   inline K_mf(size_type)      count (const key_type& x) const { return __t.count(x); }
   inline K_mf(iterator)       lower_bound (const key_type& x) { return __t.lower_bound(x); }
   inline K_mf(iterator)       upper_bound (const key_type& x) { return __t.upper_bound(x); }
   inline K_mf(const_iterator) lower_bound (const key_type& x) const { return __t.lower_bound(x); }
   inline K_mf(const_iterator) upper_bound (const key_type& x) const { return __t.upper_bound(x); }

   inline std::pair<iterator,iterator>             equal_range (const key_type& x)        { return __t.equal_range(x); }
   inline std::pair<const_iterator,const_iterator> equal_range (const key_type& x) const  { return __t.equal_range(x); }

//ev_map extended: update record
   K_mf(iterator)  fetch  (const key_type& k)
   {
      iterator position = lower_bound(k);
      if(position == end()
      || __t.key_comp()(position->first, k)
      || __t.key_comp()(k, position->first))
         return insert(position, value_type(k,mapped_type()));
      return position;
   }
//擴充給 ViewArea 使用的 members
   static inline K_mf(const key_type&)     get_key     (const iterator& i)       { return i->first;  }
   static inline K_mf(const mapped_type&)  get_mapped  (const iterator& i)       { return i->second; }
   static inline K_mf(const key_type&)     get_key     (const const_iterator& i) { return i->first;  }
   static inline K_mf(const mapped_type&)  get_mapped  (const const_iterator& i) { return i->second; }

//override ev_container virtual functions
    K_mf(size_type)  distance_begin  (const ev_iterator& from) const
                                     { const_iterator ibegin = begin();
                                       const_iterator ifrom  = static_cast<const iterator&>(from);
                                       return std::distance(ibegin,ifrom); }
}; //class ev_map
//---------------------------------------------------------------------------
#ifdef __alpha
template <class Key, class RecT, class EvContainerBase>
inline void swap(ev_map<Key,RecT,EvContainerBase>& a,
                 ev_map<Key,RecT,EvContainerBase>& b)
  {
    a.swap(b);
  }
#else
template <class Key, class RecT, class Compare, class Allocator, class EvContainerBase>
inline void swap(ev_map<Key,RecT,Compare,Allocator,EvContainerBase>& a,
                 ev_map<Key,RecT,Compare,Allocator,EvContainerBase>& b)
  {
    a.swap(b);
  }
/////////////////////////////////////////////////////////////////////////////
//如果您的 EvContainerBase 與預設的不同, 並且嫌使用 ev_map<> 的參數麻煩,
//則您可以先定義好 typedef std::map<...>   stdmap;
//再使用 ev_map2<> 來定義 ev_map<>
template <class stdmap,
          class EvContainerBase>
class ev_map2 : public ev_map<typename stdmap::key_type,
                              const typename stdmap::mapped_type,//為何要在此處加上 const:
                                                        //因為即使您在定義 stdmap 時有加上 const,
                                                        //在此處若沒有加上 const,
                                                        //BCB會將 mapped_type 視為非 const
                                                        //這是 BCB 的問題, 還是標準的定義就是如此呢?
                              typename stdmap::key_compare,
                              typename stdmap::allocator_type,
                                       EvContainerBase>
{
};
//---------------------------------------------------------------------------
template <class stdmap, class EvContainerBase>
inline void swap(ev_map2<stdmap,EvContainerBase>& a,
                 ev_map2<stdmap,EvContainerBase>& b)
  {
    a.swap(b);
  }
#endif   // __alpha..else
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif
