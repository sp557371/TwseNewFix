//
// template fixed_dec<>  固定小數位數
// template fixed_num<>  整數
//
// 版本: $Id: fixed_num.h,v 1.13 2006/04/04 02:43:12 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef fixed_numH
#define fixed_numH
//---------------------------------------------------------------------------
#include <string>
#include <stdio.h>// for sprintf()
#include <stdlib.h>
#include <ctype.h>
//---------------------------------------------------------------------------
#include "KwayBase.h"
//---------------------------------------------------------------------------
#ifdef __GNUC__ //不支援 ltoa(), _i64toa(), _ui64toa(), _atoi64
#   define ltoa(l,s,r)      sprintf(s, "%ld",  static_cast<long>(l))
#   define ultoa(l,s,r)     sprintf(s, "%lu",  static_cast<unsigned long>(l))
#   define _i64toa(l,s,r)   sprintf(s, "%lld", static_cast<long long>(l))
#   define _ui64toa(l,s,r)  sprintf(s, "%llu", static_cast<unsigned long long>(l))

#   ifdef __hpux
#     define _atoi64(s)     strtoll(s,0,10)
#   elif defined(KWAY_INT64_IS_LONG)
#     define _atoi64        atol
#   else
#     define _atoi64        atoll
#   endif
#endif

#if _MSC_VER >= 1400
// Visual C++ 2005 compiler: _MSC_VER請參考: http://msdn2.microsoft.com/zh-tw/library/b0084kay(en-us).aspx
#   define ltoa(l,s,r)      _ltoa_s(l,s,32,r)
#   define ultoa(l,s,r)     _ultoa_s(l,s,32,r)
#   define _i64toa(l,s,r)   _i64toa_s(l,s,32,r)
#   define _ui64toa(l,s,r)  _ui64toa_s(l,s,32,r)
#endif
//---------------------------------------------------------------------------
namespace Kstl {
/////////////////////////////////////////////////////////////////////////////
namespace priv {
using namespace Kway;
//---------------------------------------------------------------------------
typedef const char*  charp;
LIBNS_fn(long)           AtoL    (const char* s, const char** e);
LIBNS_fn(unsigned long)  AtoUL   (const char* s, const char** e);
LIBNS_fn(void)           LToFmt  (Kway::int64  n, int m, char*, unsigned intn, unsigned decn, bool thousandSeparator);
LIBNS_fn(void)           ULToFmt (Kway::uint64 n, int m, char*, unsigned intn, unsigned decn, bool thousandSeparator);

inline LIBNS_fn(void) NToFmt (char  n, int m, char* s, unsigned intn, unsigned decn, bool t)   { LToFmt(n,m,s,intn,decn,t); }
inline LIBNS_fn(void) NToFmt (int   n, int m, char* s, unsigned intn, unsigned decn, bool t)   { LToFmt(n,m,s,intn,decn,t); }
inline LIBNS_fn(void) NToFmt (short n, int m, char* s, unsigned intn, unsigned decn, bool t)   { LToFmt(n,m,s,intn,decn,t); }
inline LIBNS_fn(void) NToFmt (long  n, int m, char* s, unsigned intn, unsigned decn, bool t)   { LToFmt(n,m,s,intn,decn,t); }

inline LIBNS_fn(void) NToFmt (unsigned char  n, int m, char* s, unsigned intn, unsigned decn, bool t)   { ULToFmt(n,m,s,intn,decn,t); }
inline LIBNS_fn(void) NToFmt (unsigned int   n, int m, char* s, unsigned intn, unsigned decn, bool t)   { ULToFmt(n,m,s,intn,decn,t); }
inline LIBNS_fn(void) NToFmt (unsigned short n, int m, char* s, unsigned intn, unsigned decn, bool t)   { ULToFmt(n,m,s,intn,decn,t); }
inline LIBNS_fn(void) NToFmt (unsigned long  n, int m, char* s, unsigned intn, unsigned decn, bool t)   { ULToFmt(n,m,s,intn,decn,t); }

#ifndef KWAY_INT64_IS_LONG
inline LIBNS_fn(void) NToFmt (Kway::int64  n, int m, char* s, unsigned intn, unsigned decn, bool t)  { LToFmt(n,m,s,intn,decn,t);  }
inline LIBNS_fn(void) NToFmt (Kway::uint64 n, int m, char* s, unsigned intn, unsigned decn, bool t)  { ULToFmt(n,m,s,intn,decn,t); }
#endif
//---------------------------------------------------------------------------
LIBNS_fn(char*)        AsFmt (Kway::int64  n, unsigned Decimal, char* buf, unsigned intn, unsigned decn, bool filleos, bool sgn);
inline LIBNS_fn(char*) AsFmt (char  n, unsigned Decimal, char* buf, unsigned intn, unsigned decn, bool filleos, bool sgn)   { return AsFmt(Kway::int64(n), Decimal, buf, intn, decn, filleos, sgn); }
inline LIBNS_fn(char*) AsFmt (int   n, unsigned Decimal, char* buf, unsigned intn, unsigned decn, bool filleos, bool sgn)   { return AsFmt(Kway::int64(n), Decimal, buf, intn, decn, filleos, sgn); }
inline LIBNS_fn(char*) AsFmt (short n, unsigned Decimal, char* buf, unsigned intn, unsigned decn, bool filleos, bool sgn)   { return AsFmt(Kway::int64(n), Decimal, buf, intn, decn, filleos, sgn); }
#ifndef KWAY_INT64_IS_LONG
inline LIBNS_fn(char*) AsFmt (long  n, unsigned Decimal, char* buf, unsigned intn, unsigned decn, bool filleos, bool sgn)   { return AsFmt(Kway::int64(n), Decimal, buf, intn, decn, filleos, sgn); }
#endif

LIBNS_fn(char*)        AsFmt (Kway::uint64 n, unsigned Decimal, char* buf, unsigned intn, unsigned decn, bool filleos, bool sgn);
inline LIBNS_fn(char*) AsFmt (unsigned char  n, unsigned Decimal, char* buf, unsigned intn, unsigned decn, bool filleos, bool sgn)   { return AsFmt(Kway::uint64(n), Decimal, buf, intn, decn, filleos, sgn); }
inline LIBNS_fn(char*) AsFmt (unsigned int   n, unsigned Decimal, char* buf, unsigned intn, unsigned decn, bool filleos, bool sgn)   { return AsFmt(Kway::uint64(n), Decimal, buf, intn, decn, filleos, sgn); }
inline LIBNS_fn(char*) AsFmt (unsigned short n, unsigned Decimal, char* buf, unsigned intn, unsigned decn, bool filleos, bool sgn)   { return AsFmt(Kway::uint64(n), Decimal, buf, intn, decn, filleos, sgn); }
#ifndef KWAY_INT64_IS_LONG
inline LIBNS_fn(char*) AsFmt (unsigned long  n, unsigned Decimal, char* buf, unsigned intn, unsigned decn, bool filleos, bool sgn)   { return AsFmt(Kway::uint64(n), Decimal, buf, intn, decn, filleos, sgn); }
#endif
//---------------------------------------------------------------------------
const dword    DecimalDivied[] = { 1,
                                   10,
                                   100,
                                   1000,
                                   10000,
                                   100000,
                                   1000000,
                                   10000000,
                                   100000000,
                                   1000000000, };

//取出整數部分, default: byte、word、dword (unsigned types)
template <class T> inline void K_FASTCALL n2round_str (char* s, const T d, unsigned n)  { ultoa(d/DecimalDivied[n], s, 10); }

#ifdef __BORLANDC__
#pragma warn-pch  // disable warning: Cannot create pre-compiled header
#define LIBNS_TFNX(type)    LIBNS_fn(type) //BCB必須這樣使用: template<> LIBNS_fn(rtntype) function<X>
#else
#define LIBNS_TFNX(type)    type  //gcc必須這樣使用: template<> rtntype function<X>
#endif
inline LIBNS_fn(void)  sn2round_str  (char* s, long data, unsigned idx)
                                                               {  if(data < 0) {
                                                                     if((data /= (int)DecimalDivied[idx])==0)
                                                                        *s++ = '-';
                                                                  } else
                                                                     data /= DecimalDivied[idx];
                                                                  ltoa(data, s, 10);
                                                               }
template <> inline LIBNS_TFNX(void) n2round_str<char>  (char* s, const char  data, unsigned idx)  { sn2round_str(s,data,idx); }
template <> inline LIBNS_TFNX(void) n2round_str<int>   (char* s, const int   data, unsigned idx)  { sn2round_str(s,data,idx); }
template <> inline LIBNS_TFNX(void) n2round_str<short> (char* s, const short data, unsigned idx)  { sn2round_str(s,data,idx); }
template <> inline LIBNS_TFNX(void) n2round_str<long>  (char* s, const long  data, unsigned idx)  { sn2round_str(s,data,idx); }

#ifndef KWAY_INT64_IS_LONG
template <> inline LIBNS_TFNX(void) n2round_str<Kway::int64>  (char* s, const Kway::int64 data, unsigned idx)
                                                               {  Kway::int64  v = data;
                                                                  if(v < 0) {
                                                                     if((v /= DecimalDivied[idx])==0)
                                                                        *s++ = '-';
                                                                  } else
                                                                     v /= DecimalDivied[idx];
                                                                  _i64toa(v, s, 10);
                                                               }
#endif
template <> inline LIBNS_TFNX(void) n2round_str<Kway::uint64> (char* s, const Kway::uint64 data, unsigned idx)
                                                               { _ui64toa(data/DecimalDivied[idx], s, 10); }

//取出小數部分, default: byte、word、dword、uint64 (unsigned types)
#if _MSC_VER >= 1400
template <class T> inline void K_FASTCALL n2mod_str  (char* s, const T d, unsigned n)
                                                                         { sprintf_s(s, 32, ".%0*lu", n, static_cast<unsigned long>(d % DecimalDivied[n])); }
inline LIBNS_fn(void)  sn2mod_str  (char* s, const long d, unsigned n)   { sprintf_s(s, 32, ".%0*lu", n, labs(d) % DecimalDivied[n]); }
#else
template <class T> inline void K_FASTCALL  n2mod_str  (char* s, const T d, unsigned n)
                                                                         { sprintf(s, ".%0*lu", n, static_cast<unsigned long>(d % DecimalDivied[n])); }
inline LIBNS_fn(void)  sn2mod_str  (char* s, const long d, unsigned n)   { sprintf(s, ".%0*lu", n, labs(d) % DecimalDivied[n]); }
#endif

template <> inline LIBNS_TFNX(void) n2mod_str<char>  (char* s, const char  d, unsigned n) { sn2mod_str(s, d, n); }
template <> inline LIBNS_TFNX(void) n2mod_str<int>   (char* s, const int   d, unsigned n) { sn2mod_str(s, d, n); }
template <> inline LIBNS_TFNX(void) n2mod_str<short> (char* s, const short d, unsigned n) { sn2mod_str(s, d, n); }
template <> inline LIBNS_TFNX(void) n2mod_str<long>  (char* s, const long  d, unsigned n) { sn2mod_str(s, d, n); }
#ifndef KWAY_INT64_IS_LONG
template <> inline LIBNS_TFNX(void) n2mod_str<Kway::int64> (char* s, const Kway::int64  d,unsigned n) { sn2mod_str(s, static_cast<long>(d%DecimalDivied[n]), n); }
#endif
template <> inline LIBNS_TFNX(void) n2mod_str<Kway::uint64>(char* s, const Kway::uint64 d,unsigned n) { sn2mod_str(s, static_cast<long>(d%DecimalDivied[n]), n); }

// str2n<> return: is minus?
//default str2n(): byte、word、dword (unsigned types)
template <class T> inline bool K_FASTCALL str2n (charp s, T& d, charp* e)  { K_Ptr2Ref(&d) = static_cast<T>(AtoUL(s,e));  return false; }

template <> inline LIBNS_TFNX(bool) str2n<signed char>  (charp s, signed char&  d, charp* e)  { return((K_Ptr2Ref(&d) = static_cast<signed char>(AtoL(s,e)))  < 0); }
template <> inline LIBNS_TFNX(bool) str2n<int>   (charp s, int&   d, charp* e)  { return((K_Ptr2Ref(&d) = AtoL(s,e))                     < 0); }
template <> inline LIBNS_TFNX(bool) str2n<short> (charp s, short& d, charp* e)  { return((K_Ptr2Ref(&d) = static_cast<short>(AtoL(s,e))) < 0); }
template <> inline LIBNS_TFNX(bool) str2n<long>  (charp s, long&  d, charp* e)  { return((K_Ptr2Ref(&d) = AtoL(s,e))                     < 0); }

LIBNS_fn(bool) _str2n (charp s, Kway::uint64& d, charp* e);
template <> inline LIBNS_TFNX(bool) str2n<Kway::uint64>  (charp s, Kway::uint64& d, charp* e) { return _str2n(s, d, e); }

#ifndef KWAY_INT64_IS_LONG
LIBNS_fn(bool) _str2n (charp s, Kway::int64&  d, charp* e);
template <> inline LIBNS_TFNX(bool) str2n<Kway::int64>   (charp s, Kway::int64&  d, charp* e) { return _str2n(s, d, e); }
#endif
} // namespace priv
//---------------------------------------------------------------------------
LIBNS_class i128 {
   Kway::uint64 lo_;
   Kway::int64  hi_;
   K_mf(void) ComplementInverse();
   K_mf(void) Complement();
   inline i128(Kway::uint64 hi, Kway::uint64 lo) : lo_(lo), hi_(hi) {}
public:
   explicit inline i128()               : lo_(0), hi_(0)        {}
   explicit inline i128(Kway::uint64 n) : lo_(n), hi_(0)        {}
   explicit inline i128(Kway::dword n)  : lo_(n), hi_(0)        {}
   explicit inline i128(Kway::int64 n)  : lo_(n), hi_(n >> 63)  {}

   inline K_mf(i128&) operator+= (const i128& rhs) {
      Kway::uint64   old_lo = lo_;
      lo_ += rhs.lo_;
      hi_ += rhs.hi_ + (lo_ < old_lo);
      return *this;
   }
   inline K_mf(i128&) operator-= (const i128& rhs) {
      Kway::uint64   old_lo = lo_;
      lo_ -= rhs.lo_;
      hi_ -= rhs.hi_ + (lo_ > old_lo);
      return *this;
   }

#pragma region mul
   K_mf(i128&) operator*= (const i128& rhs);
   inline K_mf(i128&) operator*= (Kway::dword rhs) {
      return *this *= i128(rhs);
   }
   inline friend K_mf(i128) operator* (i128 lhs, const i128& rhs) {
      return lhs *= rhs;
   }
#pragma endregion

#pragma region div
   K_mf(i128&) operator/= (const i128& rhs);
   inline friend K_mf(i128) operator/ (i128 lhs, const i128& rhs) {
      return lhs /= rhs;
   }
   inline K_mf(i128&) operator/= (Kway::int64 rhs) {
      return *this /= i128(rhs);
   }
   inline K_mf(i128&) operator/= (Kway::uint64 rhs) {
      return *this /= i128(rhs);
   }
   inline K_mf(i128&) operator/= (Kway::dword rhs) {
      return *this /= i128(rhs);
   }
#pragma endregion

   inline operator Kway::int64() const {
      return lo_;
   }
   inline K_mf(bool) is_0() const {
      return lo_==0 && hi_==0;
   }
   inline K_mf(bool) less0() const {
      return test_ui64_msb(hi_);
   }
   static inline K_mf(bool) test_ui64_msb (Kway::uint64 n) {
      #if defined(_MSC_VER) || defined(_MSC_EXTENSIONS) || defined(__WATCOMC__)
      return ((n & (1Ui64 << 63)) != 0);
      #elif defined(__BORLANDC__)
      return ((n & (((Kway::uint64)1) << 63)) != 0);
      #else
      return ((n & (1ULL << 63)) != 0);
      #endif
   }
   inline K_mf(void) shl1 () {
      (hi_ <<= 1);
      hi_ |= (Kway::int64)test_ui64_msb(lo_);
      lo_ <<= 1;
   }
};
//---------------------------------------------------------------------------
// BCB4 不允許 sizeof() > sizeof(), 可能是有 '>' 的關係! 所以改成 !(sizeof() <= sizeof())
//template <typename T1, typename T2, bool isT1gtT2 = (sizeof(T1) > sizeof(T2)) >
template <typename T1, typename T2, bool isT1gtT2 = !(sizeof(T1) <= sizeof(T2)) >
struct int_sel_big { };

template<typename T1, typename T2>
struct int_sel_big<T1, T2, true> {
   typedef T1  itype;
};
template <typename T1, typename T2>
struct int_sel_big<T1, T2, false> {
   typedef T2  itype;
};
//---------------------------------------------------------------------------
template <typename T>
struct int_traits { };

template<> struct int_traits<Kway::int8> {
   typedef Kway::int16  i2type;
};
template<> struct int_traits<Kway::int16> {
   typedef Kway::int32  i2type;
};
template<> struct int_traits<Kway::int32> {
   typedef Kway::int64  i2type;
};
template<> struct int_traits<Kway::int64> {
   typedef i128         i2type;
};
template<> struct int_traits<i128> {
   typedef i128         i2type;
};

template<> struct int_traits<Kway::uint8> {
   typedef Kway::uint16 i2type;
};
template<> struct int_traits<Kway::uint16> {
   typedef Kway::uint32 i2type;
};
template<> struct int_traits<Kway::uint32> {
   typedef Kway::uint64 i2type;
};
template<> struct int_traits<Kway::uint64> {
   typedef i128         i2type;
};
//---------------------------------------------------------------------------
template <typename OrgT>
inline OrgT fixed_dec_d2n (double n, unsigned Decimal) {
   OrgT r;
   n *= priv::DecimalDivied[Decimal];
   n -= (r = static_cast<OrgT>(n));
   if(n >= 0.5)
      ++r;
   else if(n <= -0.5)
      --r;
   return r;
}

template <typename i2type>
struct fixed_dec_aux {
   template <typename lhsOrgT, typename rhsOrgT>
   inline static K_mf(lhsOrgT) mul (lhsOrgT ldata, unsigned lhsDecimal, rhsOrgT rdata, unsigned rhsDecimal) {
      i2type   nresult(ldata);
      nresult *= (i2type)(rdata);
      nresult /= priv::DecimalDivied[rhsDecimal];
      return (lhsOrgT)(nresult);
   }
   template <typename lhsOrgT, typename rhsOrgT>
   inline static K_mf(lhsOrgT) div (lhsOrgT ldata, unsigned lhsDecimal, rhsOrgT rdata, unsigned rhsDecimal) {
      typename int_traits<typename int_sel_big<i2type, Kway::dword>::itype>::i2type nresult(ldata);
      nresult *= priv::DecimalDivied[rhsDecimal];
      nresult /= rdata;
      return (lhsOrgT)(nresult);
   }
};
/// 在還沒有原生支援int128之前, int128的 mul, div 還是比不上 CPU 直接支援的 double.
template <>
struct fixed_dec_aux<i128> {
   template <typename lhsOrgT, typename rhsOrgT>
   inline static K_mf(lhsOrgT) mul (lhsOrgT ldata, unsigned lhsDecimal, rhsOrgT rdata, unsigned rhsDecimal) {
      double   ld = ((double)ldata) / priv::DecimalDivied[lhsDecimal];
      double   rd = ((double)rdata) / priv::DecimalDivied[rhsDecimal];
      return fixed_dec_d2n<lhsOrgT>(ld * rd, lhsDecimal);
   }
   template <typename lhsOrgT, typename rhsOrgT>
   inline static K_mf(lhsOrgT) div (lhsOrgT ldata, unsigned lhsDecimal, rhsOrgT rdata, unsigned rhsDecimal) {
      double   ld = ((double)ldata) / priv::DecimalDivied[lhsDecimal];
      double   rd = ((double)rdata) / priv::DecimalDivied[rhsDecimal];
      return fixed_dec_d2n<lhsOrgT>(ld / rd, lhsDecimal);
   }
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
template <class OrgT, unsigned Decimal>
class fixed_dec
{
   typedef fixed_dec<OrgT,Decimal>  this_type;

   OrgT  data_;

   K_ctor  fixed_dec  (OrgT n) : data_(n)  {}

public:
   typedef OrgT   orig_type;
   enum {  DecN = Decimal };

   inline K_ctor  fixed_dec  ()                      : data_()  {}
          K_ctor  fixed_dec  (OrgT n, unsigned decn) : data_( decn > Decimal
                                                               ? OrgT(n / priv::DecimalDivied[decn-Decimal])
                                                               : OrgT(n * priv::DecimalDivied[Decimal-decn]) )
                                                       { }
   inline explicit K_ctor  fixed_dec  (const char* str)        { assign(str);       }
   inline explicit K_ctor  fixed_dec  (const std::string& s)   { assign(s.c_str()); }
   inline explicit K_ctor  fixed_dec  (double n)               { assign(n);         }


   K_mf(double)       as_double  () const  { return static_cast<double>(data_) / priv::DecimalDivied[Decimal]; }

   /** as_format(): 輸出不含小數點的字串,若整數部分長度不足,則會將超過的部分切掉!
       例如: 123.456, intn=2, decn=2, 則輸出: "2345"
       若:   less_0() 則會占用intn一位,若intn==0,則會占用decn一位,填入'-'
   */
   K_mf(std::string)  as_format  (unsigned intn, unsigned decn) const
                                           { char buf[128]; return std::string(as_format(buf,intn,decn)); }
   K_mf(char*)        as_format  (char* buf, unsigned intn, unsigned decn, bool filleos = true) const
                                           { return priv::AsFmt(data_, Decimal, buf, intn, decn, filleos, false); }
   /** as_Sformat(): 第一位為正負號:
         if(above_0()) 填 '+'
         if(is_0())    填 '0'
         if(less_0())  填 '-'
         其餘與 as_format() 相同.
         輸出長度: 1 + intn + decn
   */
   K_mf(std::string)  as_Sformat (unsigned intn, unsigned decn) const
                                           { char buf[128]; return std::string(as_Sformat(buf,intn,decn)); }
   K_mf(char*)        as_Sformat (char* buf, unsigned intn, unsigned decn, bool filleos = true) const
                                           { return priv::AsFmt(data_, Decimal, buf, intn, decn, filleos, true); }

   K_mf(char*)        as_string  (char* buf, bool thousandSeparator) const {
                                             priv::NToFmt( OrgT(data_ / (int)priv::DecimalDivied[Decimal])
                                                         , int(data_ % (int)priv::DecimalDivied[Decimal])
                                                         , buf, 0, Decimal, thousandSeparator);
                                             char* endp = strchr(buf, 0);
                                             do {
                                                if(endp <= buf)
                                                   return buf;
                                             } while(*--endp=='0');
                                             *(endp + (*endp != '.')) = 0;
                                             return buf;
                                           }
   K_mf(std::string)  as_string  (unsigned intn, unsigned decn, bool thousandSeparator) const
                                           { char buf[128]; return std::string(as_string(buf,intn,decn,thousandSeparator)); }
   K_mf(char*)        as_string  (char* buf, unsigned intn, unsigned decn, bool thousandSeparator) const
                                           { OrgT  n = OrgT(data_ / (int)priv::DecimalDivied[Decimal]);
                                             int   m;
                                             if(decn) {
                                                m = int(data_ % (int)priv::DecimalDivied[Decimal]);
                                                if(decn > Decimal)
                                                   m *= priv::DecimalDivied[decn - Decimal];
                                                else
                                                if(decn < Decimal) {
                                                   m /= priv::DecimalDivied[Decimal - decn];
                                                }
                                             } else
                                                m = 0;
                                             priv::NToFmt(n, m, buf, intn, decn, thousandSeparator);
                                             return buf;
                                           }
   K_mf(std::string)  as_string  () const  { if(data_==0)
                                                return std::string();
                                             char  buf[128];
                                             priv::n2round_str<OrgT>(buf, data_, Decimal);
                                             priv::n2mod_str<OrgT>(strchr(buf,0), data_, Decimal);
                                             return std::string(buf); }
   inline K_mf(void)  assign0 ()          { data_ = 0; }
   inline K_mf(void)  assign  (double n)  { data_ = fixed_dec_d2n<OrgT>(n,Decimal); }
   K_mf(const char*)  assign  (const std::string& str) { return assign(str.c_str()); }
   K_mf(const char*)  assign  (const char* sptr)  {
                                             if(sptr==NULL) {
                                                assign0();
                                                return NULL;
                                             }
                                             const char*  strc = sptr;
                                             bool  minus = priv::str2n(strc, data_, &sptr);
                                             data_ *= static_cast<OrgT>(priv::DecimalDivied[Decimal]);
                                             if(sptr  &&  *sptr=='.') {
                                                if(!minus)
                                                    minus = (data_==0  &&  memchr(strc, '-', sptr-strc));
                                                OrgT     ndec = 0;
                                                unsigned decL = Decimal;
                                                while(decL-- > 0  &&  isdigit(*++sptr))
                                                   ndec = static_cast<OrgT>(ndec*10 + (*sptr - '0'));
                                                if(decL < numofele(priv::DecimalDivied))
                                                   ndec *= static_cast<OrgT>(priv::DecimalDivied[decL+1]);
                                                if(minus)
                                                   data_ -= ndec;
                                                else
                                                   data_ += ndec;
                                             }
                                             return sptr;
                                           }
   inline K_mf(void) assign (OrgT n, unsigned decn) {
      *this = fixed_dec(n, decn);
   }
   inline K_mf(OrgT)  get_dec () const { return static_cast<OrgT>(data_ % priv::DecimalDivied[Decimal]); }
   inline K_mf(OrgT)  get_int () const { return static_cast<OrgT>(data_ / priv::DecimalDivied[Decimal]); }

   inline K_mf(this_type)   operator+   (const this_type& a) const  { return this_type(data_ + a.data_); }
   inline K_mf(this_type&)  operator+=  (const this_type& a)        { data_ += a.data_;   return *this;  }
   inline K_mf(this_type)   operator-   (const this_type& a) const  { return this_type(data_ - a.data_); }
   inline K_mf(this_type&)  operator-=  (const this_type& a)        { data_ -= a.data_;   return *this;  }

   inline       K_mf(OrgT)  get_orig    () const     { return data_;   }

   static const unsigned   DecimalN = Decimal;

   template <class rhsOrgT, unsigned rhsDecimal>
   inline K_mf(this_type&)  operator*=  (const fixed_dec<rhsOrgT,rhsDecimal>& rhs) {
      typedef typename int_sel_big<OrgT, rhsOrgT>::itype          TSelBig;
      typedef fixed_dec_aux<typename int_traits<TSelBig>::i2type> TAux;
      data_ = TAux::mul(data_, Decimal, rhs.get_orig(), rhsDecimal);
      return *this;
   }
   template <class rhsOrgT, unsigned rhsDecimal>
   inline friend K_mf(this_type) operator* (this_type lhs, const fixed_dec<rhsOrgT,rhsDecimal>& rhs) {
      return lhs *= rhs;
   }
   inline friend K_mf(this_type) operator* (this_type lhs, OrgT r) {
      lhs.data_ *= r;
      return lhs;
   }

   template <class rhsOrgT, unsigned rhsDecimal>
   inline K_mf(this_type&)  operator/=  (const fixed_dec<rhsOrgT,rhsDecimal>& rhs) {
      data_ = fixed_dec_aux<typename int_traits<typename int_sel_big<OrgT, rhsOrgT>::itype>::i2type>
               ::div(data_,Decimal, rhs.get_orig(),rhsDecimal);
      return *this;
   }
   template <class rhsOrgT, unsigned rhsDecimal>
   inline friend K_mf(this_type) operator/ (this_type lhs, const fixed_dec<rhsOrgT,rhsDecimal>& rhs) {
      return lhs /= rhs;
   }
   inline friend K_mf(this_type) operator/ (this_type lhs, OrgT r) {
      lhs.data_ /= r;
      return lhs;
   }

   inline K_mf(bool)  operator<  (const this_type& a) const  { return(data_ <  a.data_); }
   inline K_mf(bool)  operator== (const this_type& a) const  { return(data_ == a.data_); }
   inline K_mf(bool)  operator!= (const this_type& a) const  { return(data_ != a.data_); }
   inline K_mf(bool)  is_0       ()                   const  { return(data_ == 0);       }
   inline K_mf(bool)  less_0     ()                   const  { return(data_ <  0);       }
   inline K_mf(bool)  above_0    ()                   const  { return(data_ >  0);       }

   inline K_mf(int) compare (const this_type& a) const {
      return data_ == a.data_ ? 0
           : data_ <  a.data_ ? -1
           : 1;
   }
};
/////////////////////////////////////////////////////////////////////////////
template <class OrgT>
class fixed_num
{
   typedef fixed_num<OrgT>    this_type;

   OrgT  data_;

public:
   typedef OrgT   orig_type;

   inline K_ctor  fixed_num  ()        : data_()   {}
   inline K_ctor  fixed_num  (OrgT n)  : data_(n)  {}

// 對於經常使用的 fixed_num(0) 而言, 會造成[模擬兩可]的情況 :(
// 所以只好放棄囉.
// inline explicit K_ctor  fixed_num  (const char* str)        { assign(str);       }
   inline explicit K_ctor  fixed_num  (const std::string& s)   { assign(s.c_str()); }

   K_mf(std::string)  as_format  (unsigned intn, unsigned decn) const
                                           { char buf[128]; return std::string(as_format(buf,intn,decn)); }
   K_mf(char*)        as_format  (char* buf, unsigned intn, unsigned decn, bool filleos = true) const
                                           { return fixed_dec<OrgT,0>(data_,0).as_format(buf,intn,decn,filleos); }
   K_mf(std::string)  as_string  (unsigned intn, unsigned decn, bool thousandSeparator) const
                                           { char buf[128]; return std::string(as_string(buf,intn,decn,thousandSeparator)); }
   K_mf(char*)        as_string  (char* buf, unsigned intn, unsigned decn, bool thousandSeparator) const
                                           { priv::NToFmt(data_, 0, buf, intn, decn, thousandSeparator);
                                             return buf;
                                           }
   K_mf(std::string)  as_string  () const  { return data_==0 ? std::string() : as_string0(); }
   K_mf(std::string)  as_string0 () const  { std::string result;
                                             append_string0(result);
                                             return result; }
   K_mf(void) append_string0 (std::string& str) const  {
      char  buf[128];
      priv::n2round_str<OrgT>(buf, data_, 0);
      str.append(buf);
   }
   inline K_mf(void)  assign0 ()                      { data_ = 0; }
   K_mf(const char*)  assign  (const char* sptr)      { if(sptr) priv::str2n(sptr, data_, &sptr);
                                                        else     assign0();
                                                        return sptr; }
   inline K_mf(void)  assign  (const std::string& s)  { assign(s.c_str());  }
   inline K_mf(void)  assign  (OrgT n)                { data_ = n; }

   inline K_mf(bool)  operator<  (const this_type& a) const  { return(data_ <  a.data_); }
   inline K_mf(bool)  operator== (const this_type& a) const  { return(data_ == a.data_); }
   inline K_mf(bool)  operator!= (const this_type& a) const  { return(data_ != a.data_); }
   inline K_mf(bool)  operator<  (const OrgT       a) const  { return(data_ <  a);       }
   inline K_mf(bool)  operator== (const OrgT       a) const  { return(data_ == a);       }
   inline K_mf(bool)  operator!= (const OrgT       a) const  { return(data_ != a);       }
   inline K_mf(bool)  is_0       ()                   const  { return(data_ == 0);       }

   inline K_mf(this_type&)  operator++  ()                         { ++data_; return *this; }
   inline K_mf(this_type&)  operator--  ()                         { --data_; return *this; }
   inline K_mf(this_type)   operator++  (int)                      { this_type t=*this; ++data_; return t; }
   inline K_mf(this_type)   operator--  (int)                      { this_type t=*this; --data_; return t; }
//因為已經有 operator OrgT() 的型別轉換, 所以加上 operator+() 反而會造成 gcc 的困擾
// inline K_mf(this_type)   operator+   (const this_type& a) const  { return this_type(data_ + a.data_); }
// inline K_mf(this_type)   operator+   (const OrgT       a) const  { return this_type(data_ + a);       }
   inline K_mf(this_type&)  operator+=  (const this_type& a)        { data_ += a.data_;   return *this;  }
// inline K_mf(this_type)   operator-   (const this_type& a) const  { return this_type(data_ - a.data_); }
// inline K_mf(this_type)   operator-   (const OrgT       a) const  { return this_type(data_ - a);       }
   inline K_mf(this_type&)  operator-=  (const this_type& a)        { data_ -= a.data_;   return *this;  }
   inline                   operator OrgT()                  const  { return data_;                      }
   inline K_mf(OrgT)        get_orig    ()                   const  { return data_;                      }

   static const unsigned   DecimalN = 0;
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
//---------------------------------------------------------------------------
/** 在 gcc 3.2.2 有發現一些問題, 如果沒有加上 struct FOR_GCC_BUG {};
    會造成底下的範例的現象:
//---------------------------------------------
#include "fixed_num.h"
//---------------------------------------------
//class FOR_GCC_BUG { Kstl::fixed_num<Kway::int32>  b; };  //size=4,
class FOR_GCC_BUG { char a;  };
//---------------------------------------------
#pragma pack(1)
//此處會造成: gcc認為所有的 Kstl::fixed_num<Kway::int32> 都要 pack !!?
//如果先宣告 FOR_GCC_BUG{}; 就不會了!!
struct DebugRec { Kstl::fixed_num<Kway::int32> a; };
#pragma pack()

#pragma pack(1)
class T3 { char data[3]; };
#pragma pack()
//---------------------------------------------
class BugRec1 { T3 a;  Kstl::fixed_num<Kway::int32>  b; };  //size=7 應該是8吧? 加上 struct FOR_GCC_BUG {}; 之後就是 8 了!
class BugRec2 { T3 a;  Kstl::fixed_num<Kway::uint32> b; };  //size=8
#pragma pack(1)
class BugRec3 { T3 a;  Kstl::fixed_num<Kway::int32>  b; };  //size=7
class BugRec4 { T3 a;  Kstl::fixed_num<Kway::uint32> b; };  //size=7
#pragma pack()
//---------------------------------------------
#include <stdio.h>
int main()
{
  printf("sizeof(BugRec1)=%d\n", sizeof(BugRec1)); // size = 7
  printf("sizeof(BugRec2)=%d\n", sizeof(BugRec2)); // size = 8
  printf("sizeof(BugRec3)=%d\n", sizeof(BugRec3)); // size = 7
  printf("sizeof(BugRec4)=%d\n", sizeof(BugRec4)); // size = 7
  DebugRec debugRec;//若加上 struct FOR_GCC_BUG { Kstl::fixed_num<Kway::int32> b; }; 則會在此造成 Bus error.(Solaris, gcc:3.2.2)
  printf("sizeof(DebugRec)=%d\n", sizeof(DebugRec)); // size = 7
}
//---------------------------------------------
*/
/* 可是加上這段,反而造成該pack的地方有問題:(
   比如說: TwStkOrder.h 裡面的 struct TOrdFields_ForNew;
           在建構a時: TOrdFields_ForNew a; 會 Bus error.(Solaris)gcc:3.2.2
   所以還是把這段拿掉,就讓gcc去想辦法吧!
   還有就是有用到這樣使用的地方自己小心啦.

//加上pack()不知是否能解決,還沒試過!! 20050325
//#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
//#pragma pack(push,1)
//#else
//#pragma pack(1)
//#endif
struct FOR_GCC_BUG {
   fixed_num<Kway::byte>      a;
   fixed_num<Kway::int8>      ai8;
   fixed_num<Kway::uint8>     aui8;
   fixed_num<Kway::word>      w;
   fixed_num<Kway::int16>     wi8;
   fixed_num<Kway::uint16>    wui8;
   fixed_num<Kway::dword>     dw;
   fixed_num<Kway::int32>     dwi8;
   fixed_num<Kway::uint32>    dwui8;
   fixed_num<Kway::int64>     e;
   fixed_num<Kway::uint64>    f;
   fixed_num<int>             i;
   fixed_num<unsigned int>    ui;
   fixed_num<signed int>      si;
   fixed_num<long>            l;
   fixed_num<unsigned long>   ul;
   fixed_num<signed long>     sl;
};
//#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
//#pragma pack(pop)
//#else
//#pragma pack()
//#endif
*/
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif
