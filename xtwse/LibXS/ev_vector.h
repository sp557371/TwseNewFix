//
// template ev_vector<>
//
// 版本: $Id: ev_vector.h,v 1.1.1.1 2003/10/24 15:51:17 cvsroot Exp $
//
//---------------------------------------------------------------------------
#ifndef ev_vectorH
#define ev_vectorH
//---------------------------------------------------------------------------
#include "ev_container.h"
#include "wise_obj.h"
//---------------------------------------------------------------------------
namespace Kstl {
//
// 一個特殊的 ev_container
//    1.只開放 push_back
//    2.insert 視同 push_back
//    3.不允許 erase
//    => 因為 insert() 或 erase() 之後, 之前取得的 iterator 可能會指向別的元素
// 920813:
//    1.改成允許 insert() 及 erase()
//    2.但 TViewArea 必須使用 TEvVectorVA<>  EvVectorVA.h
//
/////////////////////////////////////////////////////////////////////////////
template <class vector, class RecT = vector::value_type>
class ev_vector_iterator
{
   typedef ev_vector_iterator<vector>     this_type;
public:
   //Types.
   typedef std::random_access_iterator_tag   iterator_category;
   typedef vector::difference_type           difference_type;
   typedef vector::size_type                 index_type;
   typedef RecT                              value_type;
   typedef RecT                              mapped_type;
   typedef RecT&                             reference;
   typedef RecT*                             pointer;

   K_ctor  ev_vector_iterator  ()   { }
   K_ctor  ev_vector_iterator  (vector* recs, index_type i) : recs_(recs), index_(i)   { }

   inline K_mf(reference)  operator*   () const  { return (*recs_)[index_]; }
   inline K_mf(pointer)    operator->  () const  { return &(operator*());   }

   inline K_mf(bool)       operator==  (const this_type& a) const  { return(recs_==a.recs_ && index_==a.index_); }
   inline K_mf(this_type&) operator++  ()        { ++index_;  return *this; }
   inline K_mf(this_type&) operator++  (int)     { this_type tmp = *this;  ++index_;  return tmp; }
   inline K_mf(this_type&) operator--  ()        { --index_;  return *this; }
   inline K_mf(this_type&) operator--  (int)     { this_type tmp = *this;  --index_;  return tmp; }
   inline K_mf(this_type&) operator+=  (difference_type n)   { index_ += n;  return *this; }
   inline K_mf(this_type)  operator+   (difference_type n)   { this_type tmp(*this);   tmp+=n;  return tmp; }

   K_mf(index_type)  get_index  () const  { return index_; }
private:
   vector*     recs_;
   index_type  index_;
};
//---------------------------------------------------------------------------
template <class vector,
          class EvContainerBase = ev_container_u<ev_container_i<ev_vector_iterator<vector> >,
                                                 rec_updater_impl<typename ev_container_i<ev_vector_iterator<vector> >::iterator,
                                                                  typename ev_vector_iterator<vector>::mapped_type
                                                                  > //rec_updater_impl<>
                                                >//ev_container_t
          >
class ev_vector : public EvContainerBase
{
   typedef ev_vector<vector,EvContainerBase> this_type;
   typedef vector                            recs_type;

   wise_container<recs_type>   recs_;
public:
//Types.
    typedef EvContainerBase               ev_container_base_type;
    typedef recs_type::size_type          key_type;
    typedef recs_type::value_type         mapped_type;
    typedef recs_type::value_type         value_type;
    typedef recs_type::reference          reference;
    typedef recs_type::pointer            pointer;
    typedef recs_type::const_reference    const_reference;
    typedef recs_type::const_pointer      const_pointer;
    typedef recs_type::size_type          size_type;
    typedef recs_type::difference_type    difference_type;
//Iterator Types.
    typedef ev_container_base_type::ev_iterator             ev_iterator;
    typedef ev_container_base_type::iterator                iterator;
    typedef std::reverse_iterator<iterator>                 reverse_iterator;
    typedef ev_vector_iterator<recs_type, const value_type> const_iterator;
    typedef std::reverse_iterator<const_iterator>           const_reverse_iterator;

//construct/copy/destroy
   K_ctor  ev_vector  ()   { }
   template<class InputIterator>
     K_ctor  ev_vector  (InputIterator first, InputIterator last)  { insert(first, last); }

   inline K_dtor  ~ev_vector  ()  { bf_destroy(); }

   K_mf(this_type&)  operator=  (const this_type& a)  { return *this; }

//iterators
   inline K_mf(iterator)               begin  ()       { return orig_iterator(recs_.get(), 0); }
   inline K_mf(const_iterator)         begin  () const { return orig_iterator(recs_.get(), 0); }
   inline K_mf(iterator)               end    ()       { return orig_iterator(recs_.get(), recs_.size()); }
   inline K_mf(const_iterator)         end    () const { return orig_iterator(recs_.get(), recs_.size()); }
   inline K_mf(reverse_iterator)       rbegin ()       { return reverse_iterator(end());         }
   inline K_mf(const_reverse_iterator) rbegin () const { return const_reverse_iterator(end());   }
   inline K_mf(reverse_iterator)       rend   ()       { return reverse_iterator(begin());       }
   inline K_mf(const_reverse_iterator) rend   () const { return const_reverse_iterator(begin()); }

//capacity
   inline K_mf(bool)       empty    () const { return recs_.empty();    }
   inline K_mf(size_type)  size     () const { return recs_.size();     }
   inline K_mf(size_type)  max_size () const { return recs_.max_size(); }

//element access
   K_mf(mapped_type&)  operator[]  (key_type k)     { return (*recs_)[k]; }
   K_mf(iterator)      insert      (key_type k, const value_type& v)
      { if(k >= size())
           return insert(v).first;
        iterator  ifrom(orig_iterator(recs_.get(),k));
        bf_insert(ifrom, 1, v);
        recs_->insert(recs_->begin()+k, v);
        af_insert(ifrom, 1);
        return ifrom; }

   K_mf(iterator)      insert      (const iterator& i, const value_type& v)
      { return insert(i.get_index(), v); }

   std::pair<iterator,bool>  insert      (const value_type& v)
      { push_back(v);
        return std::make_pair(orig_iterator(recs_.get(), size()-1),true); }

   template<class InputIterator>
    K_mf(void)  insert  (InputIterator first, InputIterator last)
      { //__t.insert(first, last);
        while(first != last)    insert(*first++); }

   K_mf(void)  push_back  (const value_type& v, size_type count = 1)
      { if(count > 0) {
           iterator ifrom = end();
           bf_insert(ifrom,count,v);
           if(count==1)
              recs_->push_back(v);
           else
              recs_->resize(count+size(),v);
           af_insert(ifrom,count);
        }
      }

   inline K_mf(bool)       erase  (const iterator& i)  { if(i.get_index() >= size())
                                                            return false;
                                                         bf_erase(i,1);
                                                         recs_->erase(recs_->begin()+i.get_index());
                                                         af_erase(i,1);
                                                         return true; }
   inline K_mf(size_type)  erase  (const key_type& k)  { return erase(find(k))==true; }
   inline K_mf(size_type)  erase  (iterator ibegin, iterator iend)
                                                       { size_type   count = iend.get_index() - ibegin.get_index();
                                                         bf_erase(ibegin,count);
                                                         recs_->erase(recs_->begin()+ibegin.get_index(),
                                                                      recs_->begin()+iend.get_index());
                                                         af_erase(ibegin,count);
                                                         return count; }
   inline K_mf(void)       swap   (this_type& a)       { bf_assign();
                                                         a.bf_assign();
                                                         recs_.swap(a.recs_);
                                                         a.af_assign();
                                                         af_assign(); }
   inline K_mf(void)       clear  ()                   { bf_clear();
                                                         recs_->clear();
                                                         af_clear(); }

//map operations
   inline K_mf(iterator)       find        (key_type k)       { return orig_iterator(recs_.get(), std::min(k,size())); }
   inline K_mf(const_iterator) find        (key_type k) const { return orig_iterator(recs_.get(), std::min(k,size())); }
   inline K_mf(iterator)       lower_bound (key_type k)       { return find(k); }
 //inline K_mf(iterator)       upper_bound (key_type k)       { }
   inline K_mf(const_iterator) lower_bound (key_type k) const { return find(k); }
 //inline K_mf(const_iterator) upper_bound (key_type k) const { }

 //inline std::pair<iterator,iterator>             equal_range (const key_type& x)        { }
 //inline std::pair<const_iterator,const_iterator> equal_range (const key_type& x) const  { }

//ev_vector extended: update record
    K_mf(iterator)  fetch  (key_type k)   { return(k >= size() ? insert(mapped_type()).first : find(k)); }

//override ev_container virtual functions
    K_mf(size_type)  distance_begin  (const ev_iterator& from) const
                                     { const iterator& ifrom = static_cast<const iterator&>(from);
                                       return ifrom.get_index(); }
}; //class ev_vector
//---------------------------------------------------------------------------
template <class vector, class EvContainerBase>
inline void swap(ev_vector<vector,EvContainerBase>& a,
                 ev_vector<vector,EvContainerBase>& b)
  {
    a.swap(b);
  }
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif

