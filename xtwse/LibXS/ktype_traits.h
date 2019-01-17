//
// 因為 BCB4 尚未支援一些 traits 的處理,
// 所以就從別的地方抄一些過來用囉
//
// 版本: $Id: ktype_traits.h,v 1.1.1.1 2003/10/24 15:51:17 cvsroot Exp $
//
//---------------------------------------------------------------------------
#ifndef ktype_traitsH
#define ktype_traitsH
//---------------------------------------------------------------------------
namespace Kstl {
//
// Kstl 命名規則說明:請參考 "ev_container.h"
//
/////////////////////////////////////////////////////////////////////////////
struct true_type  {};
struct false_type {};
//---------------------------------------------------------------------------
template <class T>
struct type_traits
{
   typedef true_type     is_type_traits_base;
   typedef false_type    has_trivial_default_constructor;
   typedef false_type    has_trivial_copy_constructor;
   typedef false_type    has_trivial_assignment_operator;
   typedef false_type    has_trivial_destructor;
   typedef false_type    is_pod_type;
};
//---------------------------------------------------------------------------
struct type_traits_pod
{
   typedef true_type    has_trivial_default_constructor;
   typedef true_type    has_trivial_copy_constructor;
   typedef true_type    has_trivial_assignment_operator;
   typedef true_type    has_trivial_destructor;
   typedef true_type    is_pod_type;
};
//---------------------------------------------------------------------------
  template<> struct type_traits<char>           : public type_traits_pod {};
  template<> struct type_traits<signed char>    : public type_traits_pod {};
  template<> struct type_traits<unsigned char>  : public type_traits_pod {};
  template<> struct type_traits<int>            : public type_traits_pod {};
//template<> struct type_traits<signed int>     : public type_traits_pod {};
  template<> struct type_traits<unsigned int>   : public type_traits_pod {};
  template<> struct type_traits<long>           : public type_traits_pod {};
//template<> struct type_traits<signed long>    : public type_traits_pod {};
  template<> struct type_traits<unsigned long>  : public type_traits_pod {};
  template<> struct type_traits<short>          : public type_traits_pod {};
//template<> struct type_traits<signed short>   : public type_traits_pod {};
  template<> struct type_traits<unsigned short> : public type_traits_pod {};
  template<> struct type_traits<float>          : public type_traits_pod {};
  template<> struct type_traits<double>         : public type_traits_pod {};
  template<> struct type_traits<long double>    : public type_traits_pod {};

  template<class T> struct type_traits<T*>      : public type_traits_pod {};
/////////////////////////////////////////////////////////////////////////////
/*
template <class Iterator>
inline typename std::iterator_traits<Iterator>::value_type*  value_type  (const Iterator&) {
  return static_cast<typename std::iterator_traits<Iterator>::value_type*>(0);
}
*/
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif
