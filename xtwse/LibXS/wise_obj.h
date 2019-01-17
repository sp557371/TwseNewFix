//
// 智慧物件 template wise_obj<>
//
// 版本: $Id: wise_obj.h,v 1.3 2006/04/03 14:27:32 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef wise_objH
#define wise_objH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include <algorithm>//for std::swap()
//---------------------------------------------------------------------------
namespace Kstl {
//
// Kstl 命名規則說明:請參考 "ev_container.h"
//
// wise_obj<>使用[延遲複製]技巧,例如:
//    wise_obj<TMyRec>  a;
//    wise_obj<TMyRec>  b;
//    TMyRec*           p1 = &a;
//    b = a;            //此時 b 與 a, 指向同一份資料
//    // ※若此時 a 先死了, 則可不必進行實際的記憶體複製動作
//    a->MyField_ = x;  //此時 a 會將原本的資料複製到新的記憶體,然後將 x 填入
//    b->MyField_ = y;  //b的修改,不會影響a
// 但使用此技巧有一點需要注意:
//    TMyRec*           p2 = &a;
//    此時 p2 != p1
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
template <class RecT>
class wise_obj {
   typedef wise_obj<RecT>  this_type;

   struct ref_type {
      Kway::word  ref_count_;
      RecT        rec_;

      K_mf(bool)  release  ()    { if(--ref_count_ > 0)
                                      return false;
                                   delete this;
                                   return true;  }
      K_mf(void)  add_ref  ()    { ++ref_count_; }
   };
   mutable ref_type*  ptr_;
protected:
   inline ref_type*  get_ptr () const  { return ptr_; }

public:
   inline K_ctor  wise_obj  ()                   : ptr_(0)      { }
   inline K_ctor  wise_obj  (const this_type& a) : ptr_(a.ptr_) { if(ptr_)  ptr_->add_ref(); }
   inline K_dtor ~wise_obj  ()                   { if(ptr_)  ptr_->release(); }

   inline K_mf(const RecT*)  get  () const  { return &(operator*()); }
   inline K_mf(RecT*)        get  ()        { return &(operator*()); }

   inline K_mf(void)  swap  (this_type& a)  { std::swap(ptr_, a.ptr_); }
   inline K_mf(void)  clear ()              { if(ptr_) {
                                                 ptr_->release();
                                                 ptr_ = 0;
                                              }   
                                            }

   K_mf(this_type&)  operator=  (const this_type& a)
                                    {  ref_type* oldptr = ptr_;
                                       if((ptr_ = a.ptr_) != 0)
                                          ptr_->add_ref();
                                       if(oldptr)
                                          oldptr->release();
                                       return *this;
                                    }
   K_mf(void)  bf_modify  ()  const {
                                ref_type* oldptr = ptr_;
                                if(oldptr  &&  oldptr->ref_count_==1)
                                   return;
                                ptr_ = new ref_type;
                                ptr_->ref_count_ = 1;
                                if(oldptr) {
                                   ptr_->rec_ = oldptr->rec_;
                                   oldptr->release();
                                }
                              }

   K_mf(const RecT&)  operator*   () const  { if(ptr_==0)  bf_modify();  return ptr_->rec_; }
   K_mf(const RecT*)  operator->  () const  { return &(operator*());           }
   K_mf(const RecT*)  operator&   () const  { return &(operator*());           }
   K_mf(RecT&)        operator*   ()        { bf_modify();  return ptr_->rec_; }
   K_mf(RecT*)        operator->  ()        { return &(operator*());           }
   K_mf(RecT*)        operator&   ()        { return &(operator*());           }

   //其他的擴充
   inline K_mf(bool)  empty  () const  { return(ptr_==0); }
};
//---------------------------------------------------------------------------
template <class RecT>
class wise_container : public wise_obj<RecT>
{
   typedef wise_obj<RecT>  inherited;
public:
   #ifndef __BORLANDC__
   using inherited::get_ptr;
   #endif

   inline K_mf(bool)                      empty  () const  { return(inherited::empty()   ||  get_ptr()->rec_.empty()); }
   inline K_mf(typename RecT::size_type)  size   () const  { return(inherited::empty() ? 0 : get_ptr()->rec_.size());  }
   inline K_mf(bool)                    is_null  () const  { return(get_ptr()==0); }
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif

