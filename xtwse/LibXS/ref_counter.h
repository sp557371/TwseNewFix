//
// 物件參考計數器 template ref_counter<>
//
// 版本: $Id: ref_counter.h,v 1.3 2004/05/12 11:47:31 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef ref_counterH
#define ref_counterH
//---------------------------------------------------------------------------
#include "KwayBase.h"
//---------------------------------------------------------------------------
namespace Kstl {
/////////////////////////////////////////////////////////////////////////////
template <class TRefCount = unsigned>
class ref_counter
{
public:
   //types.
   typedef TRefCount    ref_count_type;
   //constructor.
   inline  K_ctor  ref_counter  (TRefCount iniCount=0)  : RefCount_(iniCount)  { }
   //destructor.
   virtual K_dtor ~ref_counter  ()  { }

   //member functions
   inline K_mf(TRefCount)  get_ref_count  () const  { return RefCount_; }

   inline K_mf(void)  add_ref  ()  { ++RefCount_; }
   inline K_mf(bool)  release  ()  { if(--RefCount_ > 0)
                                        return false;
                                     delete this;
                                     return true; }
private:
   TRefCount   RefCount_;
};
/////////////////////////////////////////////////////////////////////////////
template <class TRefCount = unsigned>
class ref_counter_ost
{  //與 ref_counter<> 完全相同, 除了 K_dtor
public:
   //types.
   typedef TRefCount    ref_count_type;
   //constructor.
   inline  K_ctor  ref_counter_ost  (TRefCount iniCount=0)  : RefCount_(iniCount)  { }
   //destructor.
   virtual /*K_dtor*/ ~ref_counter_ost  ()  { }   // BCB必須配合 ost, 所以不可加上 K_dtor

   //member functions
   inline K_mf(TRefCount)  get_ref_count  () const  { return RefCount_; }

   inline K_mf(void)  add_ref  ()  { ++RefCount_; }
   inline K_mf(bool)  release  ()  { if(--RefCount_ > 0)
                                        return false;
                                     delete this;
                                     return true; }
private:
   TRefCount   RefCount_;
};
/////////////////////////////////////////////////////////////////////////////
template <class T>
class auto_ref
{
   T*    ref_counter_;
public:
   inline K_ctor  auto_ref  (T* r)  : ref_counter_(r)  { if(r)  r->add_ref(); }
   inline K_dtor ~auto_ref  ()      { if(ref_counter_) ref_counter_->release(); }
};
/////////////////////////////////////////////////////////////////////////////
LIBNS_EXTERN_TEMPLATE_CLASS   ref_counter<Kway::uint8>;
LIBNS_EXTERN_TEMPLATE_CLASS   ref_counter<Kway::uint16>;
LIBNS_EXTERN_TEMPLATE_CLASS   ref_counter<Kway::uint32>;
LIBNS_EXTERN_TEMPLATE_CLASS   ref_counter<Kway::uint64>;
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif

