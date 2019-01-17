//
// template pod_vector<>
//
// 版本: $Id: pod_vector.h,v 1.6 2006/04/04 04:02:48 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef pod_vectorH
#define pod_vectorH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <new>
#include <iterator>
#include <algorithm>
#include <string.h>
#include <stdexcept>//out_of_range()
#include <limits.h>//UINT_MAX

#ifdef __BORLANDC__
#include <mem.h>
#elif _MSC_VER
#pragma warning( disable : 4200 )  // Disable warning: value_type data_[0];
#endif
//---------------------------------------------------------------------------
#include "kutility.h"
//---------------------------------------------------------------------------
namespace Kstl {
//
// Kstl 命名規則說明:請參考 "ev_container.h"
//
/*
   pod_vector<T> 主要是省掉 [當記憶體重新分配] 時, 大量 operator=() 的操作, 所以:
      1. T必須是可以直接將記憶體複製的型態, 例 wise_ptr<>、wise_obj<>、wise_container<>皆可。
      2. 當 pod_vector<> 解構或erase()時,仍會呼叫T的解構元。
      3. 當 pod_vector<> 增加一個元素時,會先將元素存放的記憶體空間清0,然後執行 T::operator=()
*/
/////////////////////////////////////////////////////////////////////////////
LIBNS_class pod_allocator_base
{
public:
   static K_mf(void*)  allocate    (size_t n)                  { return  ::malloc(n);            }
   static K_mf(void)   deallocate  (void* ptr)                 { if(ptr) ::free(ptr);            }
   static K_mf(void*)  reallocate  (void* ptr, size_t newsize) { return  ::realloc(ptr,newsize); }
};
//---------------------------------------------------------------------------
template <class T>
class  pod_allocator : public pod_allocator_base
{
public:
   static const size_t  reserve_elements = 8;   //每當記憶體重新分配,需要預留的空間
};
/////////////////////////////////////////////////////////////////////////////
template <class T, class Allocator = pod_allocator<T> >
class pod_vector //plain old data, 需由您自己確定 T 為 POD type
{
   typedef pod_vector<T>   this_type;
public:
//Types.
   typedef Allocator       allocator_type;
   typedef T               value_type;
   typedef size_t          size_type;
   typedef ptrdiff_t       difference_type;
   typedef T*              iterator;
   typedef const T*        const_iterator;
   typedef T&              reference;
   typedef const T&        const_reference;
   typedef T*              pointer;
   typedef const T*        const_pointer;
   typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;
   typedef std::reverse_iterator<iterator>         reverse_iterator;

// Construct/Copy/Destroy
inline          K_ctor  pod_vector  ()                    : recs_(0) { }
inline explicit K_ctor  pod_vector  (size_type n, const value_type& a = value_type())
                                                          : recs_(0) { assign(n, a); }
inline          K_ctor  pod_vector  (const this_type& a)  : recs_(0) { *this = a;    }
template <class InputIterator>
  inline        K_ctor  pod_vector  (InputIterator from, InputIterator endto)
                                                          : recs_(0) { assign(from,endto); }
inline          K_dtor ~pod_vector  ()  { clear(); }

   K_mf(this_type&)  operator=  (const this_type& a)
      {
         size_type   asz = a.size();
         AryPtr*  newptr = ( capacity() < asz
                              ? realloc_spaces(0, a.capacity())
                              : recs_ );
         if(newptr==0)
            return *this;
         if((newptr->size_ = asz) > 0)
            memcpy(newptr->data_, a.recs_->data_, asz*sizeof(value_type));
         if(recs_ != newptr) {
            AryPtr* oldptr = recs_;
            recs_ = newptr;
            allocator_type::deallocate(oldptr);
         }
         return *this;
      }

template <class InputIterator>
   K_mf(void)  assign  (InputIterator from, InputIterator endto)
         {
         clear();
         insert(begin(),from,endto);
         }
   K_mf(void)  assign  (size_type n, const value_type& a)
         {
         clear();
         insert(begin(),n,a);
         }

//Iterators.
inline K_mf(iterator)          begin  ()  { return recs_ ? recs_->begin() : 0; }
inline K_mf(iterator)          end    ()  { return recs_ ? recs_->end()   : 0; }
inline K_mf(reverse_iterator)  rbegin ()  { return reverse_iterator(end());   }
inline K_mf(reverse_iterator)  rend   ()  { return reverse_iterator(begin()); }

inline K_mf(const_iterator)          begin  () const  { return recs_ ? recs_->begin() : 0;      }
inline K_mf(const_iterator)          end    () const  { return recs_ ? recs_->end()   : 0;      }
inline K_mf(const_reverse_iterator)  rbegin () const  { return const_reverse_iterator(end());   }
inline K_mf(const_reverse_iterator)  rend   () const  { return const_reverse_iterator(begin()); }

//Capacity.
inline K_mf(size_type)  size     () const  { return(recs_ ? recs_->size_ : 0); }
inline K_mf(size_type)  max_size () const  { return UINT_MAX/sizeof(value_type) < 1 ? size_type(1)
                                           : size_type(UINT_MAX/sizeof(value_type)); }
inline K_mf(size_type)  capacity () const { return(recs_ ? recs_->capacity_ : 0); }
inline K_mf(bool)       empty    () const { return(recs_==0  ||  recs_->size_==0); }

K_mf(void) resize (size_type newsz) { size_type oldsz = size();
                                      if(newsz < oldsz) {
                                         erase(begin()+newsz, end());
                                      } else
                                      if(newsz > oldsz)
                                         insert_spaces(end(), newsz-oldsz);
                                    }
K_mf(void) resize (size_type newsz, const value_type& v)
                                    { size_type oldsz = size();
                                      if(newsz < oldsz) {
                                         erase(begin()+newsz, end());
                                      } else
                                      if(newsz > oldsz)
                                         insert_aux(end(), newsz-oldsz, v);
                                    }
K_mf(void) reserve (size_type sz)   { check_spaces(sz);
                                    }

// Element Access
inline K_mf(reference)        operator[] (size_type n)         { return *(begin() + n); }
inline K_mf(const_reference)  operator[] (size_type n) const   { return *(begin() + n); }
       K_mf(reference)        at         (size_type n)         { if(n >= size())  throw std::out_of_range();
                                                                 return operator[](n); }
       K_mf(const_reference)  at         (size_type n) const   { if(n >= size())  throw std::out_of_range();
                                                                 return operator[](n); }
inline K_mf(reference)        front      ()         { return *begin();   }
inline K_mf(const_reference)  front      () const   { return *begin();   }
inline K_mf(reference)        back       ()         { return *(end()-1); }
inline K_mf(const_reference)  back       () const   { return *(end()-1); }

// Modifiers
inline K_mf(void)      push_back (const value_type& a)                   { insert_aux(end(),1,a); }
inline K_mf(void)      pop_back  ()                                      { --(recs_->size_);      }
inline K_mf(iterator)  insert    (iterator start, const value_type& a)   { return insert_aux(start, 1, a); }
inline K_mf(void)      insert    (iterator start, size_type n, const value_type& a)
                                                                         { insert_aux(start, n, a); }
template <class InputIterator>
   K_mf(void)   insert    (iterator start, InputIterator from, InputIterator endto)
      {
         difference_type isz = endto - from;
         if(isz <= 0)
            return;
         start = insert_spaces(start, isz);
         for(;  from != endto;  ++from)
            *start++ = *from;
      }

inline K_mf(iterator)  erase     (iterator a)   { return erase(a,a+1); }
       K_mf(iterator)  erase     (iterator from, iterator endto)
                                  { if(recs_==NULL) return begin();
                                    destroy(from,endto);
                                    size_type n = recs_->end() - endto;
                                    if(n > 0)
                                       memcpy(from, endto, n*sizeof(value_type));
                                    recs_->size_ -= (endto - from);
                                    return from;
                                  }
inline K_mf(void)  swap   (this_type& a) { std::swap(recs_, a.recs_);   }
inline K_mf(void)  clear  ()  { destroy(begin(), end());
                                allocator_type::deallocate(recs_); //因為是pod type,所以不用 delete values
                                recs_ = 0; }

template <class U>
K_mf(const_iterator)  find  (const U& a) const  { return std::find(begin(),end(),a); }

template <class U>
K_mf(iterator)  find  (const U& a)  { return std::find(begin(),end(),a); }

template <class U>
K_mf(bool)  find (const U& a, const_iterator& i) const
                                        { const_iterator iend = end();
                                          i = std::find(begin(),iend,a);
                                          return(i!=iend); }
template <class U>
K_mf(bool)  find (const U& a, iterator& i)
                                        { iterator iend = end();
                                          i = std::find(begin(),iend,a);
                                          return(i!=iend); }
template <class U>
K_mf(bool)  add  (const U& a)  { iterator i;
                                 if(find(a,i))
                                    return false;
                                 push_back(a);
                                 return true; }
template <class U>
K_mf(bool)  del  (const U& a)  { iterator i;
                                 if(!find(a,i))
                                    return false;
                                 erase(i);
                                 return true; }

K_mf(void)  add  (value_type  begin, value_type  end)   { while(begin != end)   add(begin++);  }
K_mf(void)  del  (value_type  begin, value_type  end)   { while(begin != end)   del(begin++);  }
K_mf(void)  add  (value_type* begin, value_type* end)   { while(begin != end)   add(*begin++); }
K_mf(void)  del  (value_type* begin, value_type* end)   { while(begin != end)   del(*begin++); }

protected:
   struct AryPtr {
      size_type   size_;
      size_type   capacity_;
      value_type  data_[];

      inline K_mf(iterator)  begin  ()  { return data_;         }
      inline K_mf(iterator)  end    ()  { return data_ + size_; }
   };
   K_mf(AryPtr*)  realloc_spaces  (AryPtr* oldrecs, size_type expect_nsz)
   {
      AryPtr*  newptr = static_cast<AryPtr*>( allocator_type::reallocate(oldrecs,
                                                   sizeof(AryPtr) + expect_nsz * sizeof(value_type)) );
      if(newptr==0) 
         throw std::bad_alloc();//BCB:compiler可能會有錯,但應是BCB的bug,請結束BCB,然後再make即可通過!!
      newptr->capacity_ = expect_nsz;
      return newptr;
   }
   K_mf(void)  check_spaces  (size_type expect_nsz)
      {
         if(capacity() >= expect_nsz)
            return;
         size_type   oldsz  = size();
         AryPtr*     newptr = realloc_spaces(recs_, expect_nsz + allocator_type::reserve_elements);//多n個元素的預留空間
         newptr->size_ = oldsz;
         recs_ = newptr;
      }
   K_mf(iterator)  insert_spaces  (iterator start, size_type n)
      {
         size_type   pos = start - begin();
         check_spaces(size()+n);
         start = recs_->data_ + pos;
         pos   = recs_->size_ - pos;
         if(pos > 0)
            memmove(start+n, start, pos*sizeof(value_type));
         recs_->size_ += n;
         memset(start, 0, sizeof(value_type)*n);//將新增加出來的空間清空(填入0)
         return(start);
      }
   K_mf(iterator)  insert_aux  (iterator start, size_type n, const value_type& a)
      {
         start = insert_spaces(start, n);
         for(size_type L = 0;  L < n;  ++L)
            *start++ = a;
         return(start);
      }

   AryPtr*  recs_;
};
//---------------------------------------------------------------------------
template <class T, class Allocator>
inline void swap (pod_vector<T,Allocator>& a,
                  pod_vector<T,Allocator>& b)  { a.swap(b); }
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif
