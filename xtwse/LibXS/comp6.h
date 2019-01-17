//
// template TComp6<>
//
// 版本: $Id: comp6.h,v 1.2 2006/04/04 03:23:13 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef Comp6H
#define Comp6H
//---------------------------------------------------------------------------
#include "char_ary.h"
//---------------------------------------------------------------------------
namespace Kstl {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
template <unsigned modN>
struct TComp6_Aux
{
   static inline K_mf(long) AppLastDigit  (long result, const Kway::byte*)  { return result; }
   //比較 compare
   static inline K_mf(bool)  comp_less  (const Kway::byte* l, const Kway::byte* r, unsigned len)
                                          { return std::lexicographical_compare(l, l+len, r, r+len); }
   static inline K_mf(bool)  comp_eq    (const Kway::byte* l, const Kway::byte* r, unsigned len)
                                          { return !std::lexicographical_compare(l, l+len, r, r+len,
                                                                                 std::not_equal_to<Kway::byte>()); }
};
//---------------------------------------------------------------------------
template <>
struct TComp6_Aux<1>
{
   static inline K_mf(long) AppLastDigit  (long result, const Kway::byte* data_)
                                          { return (result * 10) + (*data_ >> 4); }
   static inline K_mf(bool) comp_less  (const Kway::byte* l, const Kway::byte* r, unsigned len)
                                          { int cr = memcmp(l, r, --len);
                                            if(cr < 0)   return true;
                                            if(cr > 0)   return false;
                                            return (l[len]&0xf0) < (r[len]&0xf0); }
   static inline K_mf(bool) comp_eq  (const Kway::byte* l, const Kway::byte* r, unsigned len)
                                          { if(memcmp(l, r, --len) == 0)
                                               return (l[len]&0xf0) == (r[len]&0xf0);
                                            return false; }
};
/////////////////////////////////////////////////////////////////////////////
template <unsigned N>
class TComp6 : protected char_ary_base<Kway::byte, (N+1)/2>
{
   typedef char_ary_base<Kway::byte, (N+1)/2>   inherited;
   typedef TComp6<N>                            this_type;
   typedef TComp6_Aux<N%2>                      aux_type;

#ifndef __BORLANDC__
protected:
   using inherited::data_; //for 避免 gcc error.
#endif

public:
   inline K_mf(int)  as_int () const  { return static_cast<int>(as_long()); }
   inline K_mf(long) as_long() const  { long  result = 0;
                                        for(int L = 0;  L < (N/2);  ++L)
                                            result = result * 100
                                                   + ( ((Kway::byte)data_[L] >> 4) * 10)
                                                   + (data_[L] & 0x0f);
                                        return aux_type::AppLastDigit(result, data_ + N/2);
                                      }

   inline K_mf(void)  clear  ()              { inherited::fill(0); }
   inline K_mf(void)  swap   (this_type& a)  { inherited::swap(a); }
   inline K_mf(void)  assign (unsigned v)    { clear();
                                               unsigned L = (N - (N%2));
                                               while(L && v) {
   	                                            Kway::byte result = Kway::byte(v % 10);
                                                  if(L%2)
                                                     result <<= 4;
                                                  data_[--L / 2] |= result;
   	                                              v /= 10;
                                               }
   	                                         }
   //比較 compare
   inline K_mf(bool)  operator<  (const this_type& dst) const
                                       { return aux_type::comp_less(data_, dst.data_, N/2); }
   inline K_mf(bool)  operator== (const this_type& dst) const
                                       { return aux_type::comp_eq(data_, dst.data_, N/2); }
   inline K_mf(bool)  operator!= (const this_type& dst) const  { return !operator==(dst); }
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

