//
// template ary2d_base<>
//
// 版本: $Id: ary2d_recs.h,v 1.3 2006/04/03 14:10:41 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef ary2d_recsH
#define ary2d_recsH
//---------------------------------------------------------------------------
#include "ary_recs.h"
#include "pod_vector.h"
//---------------------------------------------------------------------------
#include <iterator>
#include <utility>
/////////////////////////////////////////////////////////////////////////////
namespace Kstl {
//
// Kstl 命名規則說明:請參考 "ev_container.h"
//
//---------------------------------------------------------------------------
/*
   先定義什麼是2維container:
      typedef conatiner<trec> c1;//1維container
      typedef container<c1>   c2;//2維continer
      實際的例子,如下:
      typedef ary_base<tkey,trec>     cRight;
      typedef ary_base<tkey,cRight>   cLeft;
   ary2d<> 的目的是讓一個(使用相同key type的?) 2維container,可以更方便的進行操作:
      typedef ary2d_base<cLeft,cRight> cAry2d;
      cAry2d  recs;
      recs[key] //等同於 cLeft[key][key]
*/
/////////////////////////////////////////////////////////////////////////////
template <class Recs2d, class LeftIterator, class RightIterator>
class ary2d_iterator
{
  // left_iterator::reference 其實是另一個 container (ary_base<>)
  typedef ary2d_iterator<Recs2d,LeftIterator,RightIterator>        this_type;
public:
  typedef std::random_access_iterator_tag       iterator_category;
  typedef Recs2d                                recs_type;
  typedef LeftIterator                          left_iterator;
  typedef RightIterator                         right_iterator;
  typedef typename right_iterator::reference    reference;
  typedef typename right_iterator::pointer      pointer;
  typedef typename recs_type::difference_type   difference_type;
  typedef typename right_iterator::value_type   value_type;

  inline K_ctor  ary2d_iterator  (recs_type* recs, const left_iterator& lefti)
                                    : recs_(recs), lefti_(lefti)  { check_r(); }
  inline K_ctor  ary2d_iterator  (recs_type* recs, const left_iterator& lefti, const right_iterator& righti, bool isend = false)
                                    : recs_(recs), lefti_(lefti), righti_(righti), is_end_(isend) { }
  inline K_ctor  ary2d_iterator  () { }

  inline K_mf(reference)   operator*   () const  { return *righti_;       }
  inline K_mf(pointer)     operator->  () const  { return &(operator*()); }

  inline K_mf(this_type&)  operator++  ()        { if(++righti_ == lefti_->end()) { ++lefti_; check_r(); }
                                                    return *this;  }
  inline K_mf(this_type&)  operator++  (int)     { this_type tmp = *this;  ++(*this);  return tmp; }
         K_mf(this_type&)  operator--  ()        { left_iterator  leftitmp;
                                                   if(is_end_) {
                                                      if(recs_->empty())
                                                        return *this;
                                                      lefti_  = left_end(recs_);
                                                      is_end_ = false;
                                                   } else {
                                                      if(righti_ != lefti_->begin()) {
                                                         --righti_;
                                                         return *this;
                                                      }
                                                   }
                                                   check_l();
                                                   return *this;  }
  inline K_mf(this_type)  operator--   (int)     { this_type tmp = *this;  --(*this);  return tmp; }

  inline K_mf(bool)       operator==   (const this_type& a) const  { return(is_end_  &&  a.is_end_)
                                                                         || (   lefti_  == a.lefti_
                                                                             && righti_ == a.righti_); }
  inline K_mf(bool)       operator!=   (const this_type& a) const  { return !(operator==(a)); }

  K_mf(difference_type)   operator-  (const this_type& a) const
            {
               left_iterator  lhlefti;
               right_iterator lhrighti;
               left_iterator  rhlefti;
               right_iterator rhrighti;
               if(is_end_) {
                  if(a.is_end_)
                     return 0;
                  lhlefti  = left_end(recs_);
                  rhlefti  = a.lefti_;
                  rhrighti = a.righti_;
               } else {
                  lhlefti  = lefti_;
                  lhrighti = righti_;
                  if(a.is_end_)
                     rhlefti = left_end(recs_);
                  else {
                     rhlefti  = a.lefti_;
                     rhrighti = a.righti_;
                  }
               }
               typename left_iterator::difference_type  dif1 = lhlefti - rhlefti;
               if(dif1==0)
                  return(is_end_ ? 0 : (lhrighti - rhrighti));
               typename left_iterator::difference_type  dif2;
               if(dif1 > 0) {
                  //this在a之後(this有可能為end, a不可能為end)
                  dif2 = is_end_ ? 0 : (lhrighti - lhlefti->begin());
                  while(--dif1)
                     dif2 += (--lhlefti)->size();
                  return(dif2 + (rhlefti->end() - rhrighti));
               }
               //dif1 < 0 : this在a之前(this不可能為end, a有可能為end)
               dif2 = lhlefti->end() - lhrighti;
               while(++dif1)
                  dif2 += (++lhlefti)->size();
               return a.is_end_ ? dif2 : (dif2 + (rhrighti - rhlefti->begin() + 1));
            } // operator-()
   K_mf(this_type&)  operator+=  (difference_type n)
            {
            if(n > 0) {
               //往end()的方向移動
               if(is_end_)
                  return *this;
               left_iterator  leftiend = left_end(recs_);
               typename right_iterator::difference_type dif2 = lefti_->end() - righti_;
               do {
                  if(dif2 > n) {//在同一個 lefti 中移動
                     righti_ += n;
                     return *this;
                  }
                  if(++lefti_ == leftiend) {
                     is_end_ = true;
                     return *this;
                  }
                  if(dif2==n) {//移到下一個 lefti 的 begin()
                     //check_r();
                     do {
                        if(!lefti_->empty()) {
                           righti_ = lefti_->begin();
                           is_end_ = false;
                           return *this;
                        }
                     } while(++lefti_ != leftiend);
                     is_end_ = true;
                     return *this;
                  }
                  n      -= dif2;
                  dif2    = lefti_->size();
                  righti_ = lefti_->begin();
               } while(1);
            }
            if(n < 0) {
               //往begin()的方向移動
               if(is_end_) {
                  if(recs_->empty())
                     return *this;
                  is_end_ = false;
                  lefti_  = left_end(recs_);
                  check_l();
                  if(++n == 0)
                     return *this;
               }
               n = -n;
               typename right_iterator::difference_type  dif2 = righti_ - lefti_->begin() + 1;
               do {
                  if(dif2 > n) {//在同一個 lefti 中移動
                     righti_ += -n;
                     return *this;
                  }
                  if(dif2==n) {//移到前一個 lefti 的 end()-1
                     check_l();
                     return *this;
                  }
                  n -= dif2;
                  --lefti_;
                  if((dif2 = lefti_->size()) > 0) {
                     righti_ = lefti_->end();
                     --righti_;
                  }
               } while(1);
            }
            return *this;
            } // operator+=()

   inline K_mf(recs_type*)      get_recs   () const  { return recs_;   }
   inline K_mf(left_iterator)   get_lefti  () const  { return lefti_;  }
   inline K_mf(right_iterator)  get_righti () const  { return righti_; }
   inline K_mf(bool)            is_end     () const  { return is_end_; }

   static inline K_mf(left_iterator)  left_end    (recs_type* recs)  { return left_iterator(recs, recs->get_index_end());   }
   static inline K_mf(left_iterator)  left_begin  (recs_type* recs)  { return left_iterator(recs, recs->get_index_begin()); }

protected:
   K_mf(void)  check_l  ()
                  {
                     while((--lefti_)->empty());
                     righti_ = lefti_->end();
                     --righti_;
                  }
   K_mf(void)  check_r  ()
                  {
                    if(recs_->empty()) {
                    } else {
                       left_iterator  leftiend = left_end(recs_);
                       while(lefti_ != leftiend) {
                          if(lefti_->empty())
                            ++lefti_;
                          else {
                            righti_ = lefti_->begin();
                            is_end_ = false;
                            return;
                          }
                       }
                    }
                    is_end_ = true;
                  }

   recs_type*     recs_;
   left_iterator  lefti_;
   right_iterator righti_;
   bool           is_end_;
}; // ary2d_iterator
/////////////////////////////////////////////////////////////////////////////
template <class Key, class LeftAry, class RightAry>
class ary2d_base : private LeftAry
{
   typedef ary2d_base<Key,LeftAry,RightAry>  this_type;
   typedef LeftAry                           inherited;

public:
#ifndef __BORLANDC__
   using inherited::get_recs;
#endif

//Types.
   typedef          Key                         key_type;
   typedef          LeftAry                     left_type;
   typedef          RightAry                    right_type;
   typedef typename left_type::iterator         left_iterator;
   typedef typename right_type::iterator        right_iterator;

   typedef typename right_type::mapped_type     mapped_type;
   typedef typename right_type::value_type      value_type;
   typedef          size_t                      size_type;
   typedef          ptrdiff_t                   difference_type;
   typedef typename right_type::reference       reference;
   typedef typename right_type::pointer         pointer;
   typedef typename right_type::const_reference const_reference;
   typedef typename right_type::const_pointer   const_pointer;

//Iterator Types.
   typedef ary2d_iterator<typename left_type::recs_type, left_iterator, right_iterator>  iterator;
   friend class ary2d_iterator<typename left_type::recs_type, left_iterator, right_iterator>;//iterator;

   typedef ary2d_iterator<const typename left_type::recs_type, typename left_type::const_iterator, typename right_type::const_iterator>  const_iterator_base;
   friend class ary2d_iterator<const typename left_type::recs_type, typename left_type::const_iterator, typename right_type::const_iterator>;//const_iterator_base;
   class const_iterator : public const_iterator_base {
      typedef const_iterator_base   inherited;
   public:
      typedef const typename inherited::recs_type  recs_type;
      typedef typename inherited::left_iterator    left_iterator;
      typedef typename inherited::right_iterator   right_iterator;
      inline K_ctor  const_iterator  ()  { }         
      inline K_ctor  const_iterator  (recs_type* recs, const left_iterator& lefti)
                                                                : inherited(recs,lefti) { }
      inline K_ctor  const_iterator  (recs_type* recs, const left_iterator& lefti, const right_iterator& righti)
                                                                : inherited(recs,lefti,righti)
                                                                { }
      inline K_ctor  const_iterator  (const const_iterator& a)  : inherited(a.get_recs(), a.get_lefti(), a.get_righti(), a.is_end())  { }
      //↓主要是為了這個,才用繼承的方式將 const_iterator 繼承自 const_iterator_base
      inline K_ctor  const_iterator  (const iterator& a)        : inherited(a.get_recs(), a.get_lefti(), a.get_righti(), a.is_end())  { }
   };

   typedef std::reverse_iterator<iterator>         reverse_iterator;
   typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;

//construct/copy/destroy
   inline K_ctor  ary2d_base  ()  : size_catch_(0) { }
   inline K_dtor ~ary2d_base  ()  { }

//Iterators.
   inline K_mf(iterator)        begin ()       { return iterator      (get_recs(), inherited::begin()); }
   inline K_mf(const_iterator)  begin () const { return const_iterator(get_recs(), inherited::begin()); }
   inline K_mf(iterator)        end   ()       { return iterator      (get_recs(), inherited::end());   }
   inline K_mf(const_iterator)  end   () const { return const_iterator(get_recs(), inherited::end());   }

   inline K_mf(reverse_iterator)        rbegin ()       { return reverse_iterator      (end());   }
   inline K_mf(const_reverse_iterator)  rbegin () const { return const_reverse_iterator(end());   }
   inline K_mf(reverse_iterator)        rend   ()       { return reverse_iterator      (begin()); }
   inline K_mf(const_reverse_iterator)  rend   () const { return const_reverse_iterator(begin()); }

//Capacity.
   inline K_mf(size_type)  size     () const  { return size_catch_;   }
   inline K_mf(bool)       empty    () const  { return inherited::empty(); }
#ifdef max
#undef max
#endif
   inline K_mf(size_type)  max_size () const  { return std::numeric_limits<size_type>::max(); }

//ary operations
   inline K_mf(iterator)        find        (const key_type& key)       { return find_aux(key);        }
   inline K_mf(const_iterator)  find        (const key_type& key) const { return const_cast<this_type*>(this)
                                                                                 ->find_aux(key);  }
   inline K_mf(iterator)        lower_bound (const key_type& key)       { return lower_bound_aux(key); }
   inline K_mf(const_iterator)  lower_bound (const key_type& key) const { return const_cast<this_type*>(this)
                                                                                 ->lower_bound_aux(key); }
 //inline K_mf(iterator)        upper_bound (const key_type& key)       ;
 //inline K_mf(const_iterator)  upper_bound (const key_type& key) const ;

//modifiers
   inline K_mf(void)  swap  (this_type& a)
   {
      inherited::swap(a);
      std::swap(size_catch_, a.size_catch_);
   }

//元素異動操作: assign,clear,insert,erase...
   inline K_mf(void)        clear      ()                    { inherited::clear();
                                                               size_catch_ = 0; }
   inline K_mf(this_type&)  operator=  (const this_type& a)  { inherited::operator=(a);
                                                               size_catch_ = a.size_catch_;
                                                               return *this;  }

//為了讓 ary2d_base<> 能成為 RightAry, 必須要完成與 ary_base<> 相容的介面:
   typedef typename right_type::recs_type    recs_type;
   typedef typename recs_type::index_type    index_type;
// insert()
   struct ary_insert_aux : private right_type::ary_insert_aux {
      friend class ary2d_base;//this_type
      typedef typename right_type::ary_insert_aux  right_insert_aux;
      left_iterator  left_iter_;
      this_type*     this_ary2d_;
      //這兒主要是要掌握:最右端的ary是否有insert()
      //只有在最右端的ary有insert()時,才需要調整size
      //最右端的陣列才是實際存放[元素]資料的地方
      virtual K_mf(void)  operator()  (recs_type*         recs,
                                       index_type         from,
                                       size_type          count,
                                       const mapped_type* value) const
      {
         right_insert_aux::operator()(recs, from, count, value);
         this_ary2d_->size_catch_ += count;
      }
      K_mf(bool) make_bf_iterator (recs_type* recs, index_type from, iterator& i) const
      {
         right_iterator ri;
         if(right_insert_aux::make_bf_iterator(recs,from,ri)) {
            i = iterator(this_ary2d_->get_recs(), left_iter_, ri);
            return true;
         }
         return !(i = iterator(this_ary2d_->get_recs(), left_iter_)).is_end();
      }
      K_mf(iterator) make_af_iterator (recs_type* recs, index_type from, size_type count) const
      {
         return iterator(this_ary2d_->get_recs(), left_iter_, right_insert_aux::make_af_iterator(recs,from,count));
      }
   }; // struct ary_insert_aux
   friend struct ary_insert_aux;
   std::pair<iterator,bool>  insert  (const key_type& key, const mapped_type* value,
                                      ary_insert_aux& insert_aux)
   {
      left_iterator  li = inherited::insert(key,0).first;
      insert_aux.this_ary2d_ = this;
      insert_aux.left_iter_  = li;
      //li->insert()有可能呼叫 ary2d_base::insert() 或 ary_base::insert()
      std::pair<right_iterator,bool>  iright = li->insert(key,value,insert_aux);
      return std::make_pair(iterator(get_recs(), li, iright.first), iright.second);
   }
   std::pair<iterator,bool>  insert  (const key_type& key, const mapped_type* value)
   {
      return insert(key, value, insert_aux(this));
   }

// erase()
   struct ary_erase_aux1 : public right_type::ary_erase_aux1 {
      friend class ary2d_base;//this_type;
      typedef typename right_type::ary_erase_aux1        right_erase_aux;
      typedef typename right_erase_aux::right_iterator   right_iterator;
      typedef typename this_type::iterator               this_iterator;
      typedef typename this_type::left_type::recs_type   left_recs;
      this_type*     this_ary2d_;
      this_iterator  ifrom_;
      //這兒主要是要掌握:最右端的ary是否有erase()
      //只有在最右端的ary有erase()時,才需要調整size
      //最右端的陣列才是實際存放[元素]資料的地方
      K_mf(void)  af_erase     () const   { --(this_ary2d_->size_catch_);
                                            left_iterator li = ifrom_.get_lefti();
                                            if(li->empty()) {
                                               left_recs*    lrecs = this_ary2d_->get_recs();
                                               left_iterator liend = this_iterator::left_end(lrecs);
                                               if(li == this_iterator::left_begin(lrecs)) {
                                                  typename left_recs::size_type  count = 1;
                                                  while(++li != liend)
                                                    if(li->empty())  ++count;
                                                    else             break;
                                                  lrecs->erase_begin(count);
                                               } else
                                               if(li == --liend) {//erase back
                                                  typename left_recs::iterator   lribeg = lrecs->begin();
                                                  typename left_recs::iterator   lri    = lrecs->end();
                                                  typename left_recs::iterator   lriend = lri;
                                                  for(--lri;; --lri) {
                                                     if(!lri->empty()) {
                                                        ++lri;
                                                        break;
                                                     }
                                                     if(lri == lribeg)
                                                        break;
                                                  }
                                                  lrecs->erase_not_begin(lri,lriend);
                                               }
                                            }
                                          }
      K_mf(void)  erase_begin  (recs_type* recs) const  { right_erase_aux::erase_begin(recs);
                                                          af_erase();
                                                        }
      K_mf(void)  erase_back   (recs_type* recs, const typename recs_type::iterator& oiback) const
                                                        { right_erase_aux::erase_back(recs,oiback);
                                                          af_erase();
                                                        }
      K_mf(void)  clear_one    (const right_iterator& i) const
                                                        { right_erase_aux::clear_one(i); }
   };
   friend struct ary_erase_aux1;
   template <class ary_erase_aux_type>
    K_mf(bool)  erase1  (const iterator& a, ary_erase_aux_type& erase_aux)
                        { static_cast<ary_erase_aux1&>(erase_aux).this_ary2d_ = this;
                          static_cast<ary_erase_aux1&>(erase_aux).ifrom_      = a;
                          return(a.get_lefti()->erase1(a.get_righti(), erase_aux));
                        }
protected:
   K_mf(iterator)  find_aux  (const key_type& key)
   {
      left_iterator  lefti = inherited::find(key);
      if(lefti==inherited::end())
         return iterator(get_recs(),lefti);
      right_iterator  righti = lefti->find(key);
      return iterator(get_recs(), lefti, righti, (righti==lefti->end()));
   } //find_aux()

   K_mf(iterator)  lower_bound_aux  (const key_type& key)
   {
      left_iterator  lefti = inherited::find(key);
      if(lefti==inherited::end())
         return iterator(get_recs(), inherited::lower_bound(key));
      right_iterator  righti = lefti->lower_bound(key);
      if(righti == lefti->end())
         return iterator(get_recs(),++lefti);
      return iterator(get_recs(),lefti,righti);
   } //lower_bound_aux()

   size_type   size_catch_;
}; // ary2d_base
//---------------------------------------------------------------------------
template <class Key, class LeftAry, class RightAry>
inline void swap (ary2d_base<Key,LeftAry,RightAry>& a,
                  ary2d_base<Key,LeftAry,RightAry>& b)  { a.swap(b); }
/////////////////////////////////////////////////////////////////////////////
// typedef ary_base<Key,RightAry,LeftIndexMaker,pod_vector<RightAry> >    LeftAry;
template <class Key, class LeftIndexMaker, class RightAry>
class ary2dd_base : public ary2d_base< Key,
                                       ary_base<Key,RightAry,LeftIndexMaker,pod_vector<RightAry> >,
                                       RightAry
                                       >
{
};
//---------------------------------------------------------------------------
template <class Key, class LeftIndexMaker, class RightAry>
inline void swap (ary2dd_base<Key,LeftIndexMaker,RightAry>& a,
                  ary2dd_base<Key,LeftIndexMaker,RightAry>& b)  { a.swap(b); }
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif

