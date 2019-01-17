//
// 一些簡單的 template 工具:
//    template xxx_enum_flag<>
//    template EnumT<>
//    ...
//
// 版本: $Id: kutility.h,v 1.4 2004/07/09 05:23:48 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef kutilityH
#define kutilityH
//---------------------------------------------------------------------------
#include "ktype_traits.h"
#include "KwayBase.h"
//---------------------------------------------------------------------------
//#include <function>
#include <string>
#include <ctype.h>
//---------------------------------------------------------------------------
namespace Kstl {
/////////////////////////////////////////////////////////////////////////////
template <class T> inline T& clear_enum_flag (T& a, T b)  { a = static_cast<T>(a & ~b); return a; }
template <class T> inline T& set_enum_flag   (T& a, T b)  { a = static_cast<T>(a |  b); return a; }
template <class T> inline T& set_enum_flag   (T& a, T b, bool c)
                                                                { return c ? set_enum_flag(a,b) : clear_enum_flag(a,b); }
template <class T> inline T& set_enum_flag   (T& a, T b, char c)
                                                                { if(toupper(c)=='Y')
                                                                     return set_enum_flag(a,b);
                                                                  return clear_enum_flag(a,b);
                                                                }
#ifdef K_EnumPacked_Bug
template <class T, class T2> inline T& clear_enum_flag (T& a, T2 b)  { a = static_cast<T>(a & ~b); return a; }
template <class T, class T2> inline T& set_enum_flag   (T& a, T2 b)  { a = static_cast<T>(a |  b); return a; }
template <class T, class T2> inline T& set_enum_flag   (T& a, T2 b, bool c)
                                                                { return c ? set_enum_flag(a,b) : clear_enum_flag(a,b); }
template <class T, class T2> inline T& set_enum_flag   (T& a, T2 b, char c)
                                                                { if(toupper(c)=='Y')
                                                                     return set_enum_flag(a,b);
                                                                  return clear_enum_flag(a,b);
                                                                }
#endif
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
template <class EType>
class EnumT {
   typedef EnumT<EType>    this_type;
protected:
   EType    Data_;
public:
   inline K_ctor      EnumT    (EType a = EType(0))  { Data_ = a; }
   inline             operator EType() const  { return Data_;     }
   inline K_mf(void)  assignT  (EType a)      { Data_ = a;        }
   inline K_mf(void)  assignT  (this_type a)  { Data_ = a.Data_;  }

   inline K_mf(std::string)  as_string  () const                  { return std::string((char*)&Data_,1); }
   inline K_mf(void)         assign     (const std::string& s)    { Data_ = (EType)toupper(*s.c_str());  }
};

LIBNS_EXTERN_TEMPLATE_CLASS   EnumT<Kway::int8>;
LIBNS_EXTERN_TEMPLATE_CLASS   EnumT<Kway::uint8>;
LIBNS_EXTERN_TEMPLATE_CLASS   EnumT<Kway::int16>;
LIBNS_EXTERN_TEMPLATE_CLASS   EnumT<Kway::uint16>;
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
struct delete_t {
   template <class T>
   void operator() (T* ptr)  { delete ptr; }
};
struct delete_pair_2nd {
   template <class T>
   void operator() (T& ptr)  { delete ptr.second; }
};
/////////////////////////////////////////////////////////////////////////////
#ifdef __BORLANDC__
//for pointer used. 避免 BCB 的 warning.
#pragma argsused
#endif
template <class T>
inline void  destroy  (T* pointer)  { pointer->~T(); }
//---------------------------------------------------------------------------
template <class ForwardIterator, class T>
inline void  destroy_v  (ForwardIterator first, ForwardIterator last, T*) {
   typedef typename type_traits<T>::has_trivial_destructor  has_trivial_destructor;
   destroy_aux(first, last, has_trivial_destructor());
}
//---------------------------------------------------------------------------
template <class ForwardIterator>
inline void  destroy  (ForwardIterator first, ForwardIterator last) {
   typedef typename std::iterator_traits<ForwardIterator>::value_type  value_type;
   destroy_v(first, last, static_cast<value_type*>(0));
   //destroy_v(first, last, std::__value_type(first));
}
//---------------------------------------------------------------------------
template <class ForwardIterator>
inline void  destroy_aux  (ForwardIterator first, ForwardIterator last, false_type) {
   for(;  first != last;  ++first)
      destroy(&*first);
}
//---------------------------------------------------------------------------
template <class ForwardIterator>
inline void  destroy_aux  (ForwardIterator,//first,
                           ForwardIterator,//last,
                           true_type) {
   // has_trivial_destructor, 不需解構
}
/////////////////////////////////////////////////////////////////////////////
template <class T, class pmf_type>
class closure_type
{
   //typedef pmf_type::first_argument_type   obj_type;
   typedef typename pmf_type::result_type    result_type;
   typedef T                                 obj_type;
   obj_type    obj_;
   pmf_type    pmf_;
public:
   K_ctor  closure_type  (obj_type obj, pmf_type pmf) : obj_(obj), pmf_(pmf) {}

   inline K_mf(result_type)  operator()  ()             { return pmf_(obj_); }

   template <class arg_type1>
   inline K_mf(result_type)  operator()  (arg_type1 a)  { return pmf_(obj_,a); }

   template <class at1, class at2>
   inline K_mf(result_type)  operator()  (at1 a1, at2 a2)  { return pmf_(obj_,a1,a2); }
};
//---------------------------------------------------------------------------
template <class T, class pmf_type>
inline closure_type<T,pmf_type>  make_closure  (T obj, pmf_type pmf)
{
return closure_type<T,pmf_type>(obj,pmf);
}
/////////////////////////////////////////////////////////////////////////////
template <class TPair, class TResult>
inline typename TPair::second_type  split_pair_rtn2  (const TPair& p, TResult& r)    { r = p.first;  return p.second; }
//---------------------------------------------------------------------------
template <class TPair, class TResult>
inline typename TPair::first_type   split_pair_rtn1  (const TPair& p, TResult& r)    { r = p.second;  return p.first; }
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif
