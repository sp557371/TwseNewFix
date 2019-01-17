//
// 智慧指標 template wise_ptr<>
//
// 版本: $Id: wise_ptr.h,v 1.8 2006/04/03 14:27:21 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef wise_ptrH
#define wise_ptrH
//---------------------------------------------------------------------------
#include "ref_counter.h"
#include <algorithm>//for std::swap()
//---------------------------------------------------------------------------
namespace Kstl {
//
// wise_ptr<> 使用[指標計數器]技巧,來決定指標所指物件是否需要繼續存在
//
/////////////////////////////////////////////////////////////////////////////
template <class ObjType, class TRefCount>
class wise_ref_obj : public ObjType
                   , public ref_counter<TRefCount>
{
   typedef ObjType                           inherited;
   typedef wise_ref_obj<ObjType,TRefCount>   this_type;
public:
   //constructor.
   inline  K_ctor  wise_ref_obj  ()  { }

   template <class T1>
      inline K_ctor  wise_ref_obj  (const T1& a1) : inherited(a1) { }
   template <class T1>
      inline K_ctor  wise_ref_obj  (T1& a1) : inherited(a1) { }

   template <class T1, class T2>
      inline K_ctor  wise_ref_obj  (const T1& a1, const T2& a2) : inherited(a1,a2) { }

   template <class T1, class T2, class T3>
      inline K_ctor  wise_ref_obj  (const T1& a1, const T2& a2, const T3& a3) : inherited(a1,a2,a3) { }

   template <class T1, class T2, class T3, class T4>
      inline K_ctor  wise_ref_obj  (const T1& a1, const T2& a2, const T3& a3, const T4& a4) : inherited(a1,a2,a3,a4) { }

   template <class T1, class T2, class T3, class T4, class T5>
      inline K_ctor  wise_ref_obj  (const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5) : inherited(a1,a2,a3,a4,a5) { }

   template <class T1, class T2, class T3, class T4, class T5, class T6>
      inline K_ctor  wise_ref_obj  (const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5, const T6& a6) : inherited(a1,a2,a3,a4,a5,a6) { }
};
/////////////////////////////////////////////////////////////////////////////
template <class RefObj>
class wise_ref
{
   typedef wise_ref<RefObj>  this_type;
   RefObj*  RefObj_;
public:
   //types.
   typedef RefObj                               ref_obj;
   typedef typename ref_obj::ref_count_type     ref_count_type;
   //constructor/destructor
   explicit K_ctor  wise_ref  (RefObj* a)          : RefObj_(a)         { if(a)        a->add_ref();       }
            K_ctor  wise_ref  (const this_type& a) : RefObj_(a.RefObj_) { if(RefObj_)  RefObj_->add_ref(); }
            K_ctor  wise_ref  ()                   : RefObj_(0)         { }
            K_dtor ~wise_ref  ()                   { if(RefObj_)  RefObj_->release(); }

   //operators.
   inline K_mf(this_type&)  operator=  (const this_type& a)  { reset(a.RefObj_);   return *this; }

   inline K_mf(const RefObj&)  operator*   () const   { return *RefObj_; }
   inline K_mf(const RefObj*)  operator->  () const   { return RefObj_;  }
   inline K_mf(RefObj&)        operator*   ()         { return *RefObj_; }
   inline K_mf(RefObj*)        operator->  ()         { return RefObj_;  }
   inline K_mf(bool)           operator==  (const this_type& a) const   { return RefObj_==a.RefObj_; }
   inline K_mf(bool)           operator!=  (const this_type& a) const   { return RefObj_!=a.RefObj_; }

   //member functions.
   inline K_mf(ref_count_type) get_ref_count () const   { return(RefObj_ ? RefObj_->get_ref_count() : 0); }

   inline K_mf(const RefObj*)  get      () const    { return RefObj_;              }
   inline K_mf(RefObj*)        get      ()          { return RefObj_;              }
   inline K_mf(RefObj*)        testnew  ()          { if(RefObj_) return RefObj_;
                                                       return renew();              }
   inline K_mf(RefObj*)        renew    ()          { return reset(new RefObj);    }
          K_mf(RefObj*)        reset    (RefObj* a = 0)
                                                     { RefObj* old = RefObj_;
                                                       if((RefObj_ = a) != 0)
                                                          a->add_ref();
                                                       if(old)
                                                          old->release();
                                                       return RefObj_;
                                                     }
   inline K_mf(void)  swap  (this_type& a)  { std::swap(RefObj_, a.RefObj_); }
};
/////////////////////////////////////////////////////////////////////////////
template <class ObjType, class TRefCount = unsigned>
class wise_ptr : public wise_ref<wise_ref_obj<ObjType, TRefCount> >
{
   typedef wise_ref<wise_ref_obj<ObjType,TRefCount> >    inherited;
   typedef wise_ptr<ObjType, TRefCount>                  this_type;
public:
   explicit K_ctor  wise_ptr  (typename inherited::ref_obj* a)  : inherited(a) { }
            K_ctor  wise_ptr  (const this_type& a)              : inherited(a) { }
            K_ctor  wise_ptr  ()                                : inherited()  { }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif

