//
// template char_ary<>
//    字元陣列的template,讓你可以方便的使用字元陣列
//    用來取代  char  data[n];
//
// 版本: $Id: char_ary.h,v 1.7 2006/04/04 00:30:43 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef char_aryH
#define char_aryH
//---------------------------------------------------------------------------
#include "fixed_num.h"
//---------------------------------------------------------------------------
#include <string>
#include <algorithm>
#include <functional>
#include <ctype.h>
#include <string.h>
//---------------------------------------------------------------------------
namespace Kstl {
#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS  //fonwin: 不警告 std::fill_n() 
#endif
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
template <class CharT, unsigned N>
class char_ary_base
{
   typedef char_ary_base<CharT,N>   this_type;
protected:
   CharT    data_[N];
public:
   static const unsigned Size = N;
   //Iterators.
   typedef CharT*          iterator;
   typedef const CharT*    const_iterator;
   inline K_mf(iterator)        begin  ()        { return data_;   }
   inline K_mf(iterator)        end    ()        { return data_+N; }
   inline K_mf(const_iterator)  begin  () const  { return data_;   }
   inline K_mf(const_iterator)  end    () const  { return data_+N; }

   inline K_mf(int)   as_int     () const  { struct {
                                                this_type   dat_;
                                                char        eos_;
                                             }  tmp;
                                             tmp.dat_ = *this;
                                             tmp.eos_ = 0;
                                             return atoi(static_cast<char*>(tmp.dat_.data_)); }
   inline K_mf(Kway::int64) as_int64 () const
                                           { struct {
                                                this_type   dat_;
                                                char        eos_;
                                             }  tmp;
                                             tmp.dat_ = *this;
                                             tmp.eos_ = 0;
                                             Kway::int64 d = 0;
                                             const char* e;
                                             Kstl::priv::str2n<Kway::int64>(tmp.dat_.data_, d, &e);
                                             return d; }

   inline K_mf(const CharT)  operator[]  (unsigned n) const      { return data_[n]; }
   inline K_mf(void)         set         (unsigned n, CharT ch)  { data_[n] = ch;   }
   inline K_mf(void)         fill        (CharT ch)              { std::fill_n(begin(), N, ch); }
   inline K_mf(void)         swap        (this_type& a)          { this_type b(a); a = *this; *this = b; }

   inline K_mf(int) find (CharT ch) const
      {  const_iterator  ifind = std::find(begin(), end(), ch);
         return ifind==end() ? -1 : (int)(ifind-begin());
      }
};
/////////////////////////////////////////////////////////////////////////////
template <class CharT, unsigned N, CharT NullChar>
class char_ary : public char_ary_base<CharT,N>
{
   typedef char_ary_base<CharT,N>      inherited;
   typedef char_ary<CharT,N,NullChar>  this_type;

#ifndef __BORLANDC__
protected:
   using inherited::data_; //for 避免 gcc error.
public:
   using inherited::begin;
   using inherited::end;
#endif

public:
   static const CharT   Null = NullChar;
   //Types.

   //constructor/destructor
   inline          K_ctor  char_ary  ()                         { std::fill_n(data_, N, NullChar); }
   inline explicit K_ctor  char_ary  (const char* str)          { assign(str);                     }
   inline explicit K_ctor  char_ary  (const std::string& str)   { assign(str);                     }
   inline          K_ctor  char_ary  (const char* sbegin, const char* send) { assign(sbegin,send); }
   inline          K_ctor  char_ary  (const char* sbegin, unsigned len)     { assign(sbegin,len);  }

   template <class InputIterator>
     inline K_ctor  char_ary  (InputIterator begin, unsigned len)   { assign(begin,len); }

   //取出
   K_mf(std::string)  as_string  () const  { std::string             str(data_,N);
                                             CharT                   tmpFind = NullChar;
                                             std::string::size_type  lpos = str.find_last_not_of(&tmpFind,N,1);
                                             if(lpos == std::string::npos)//全部都是NullChar
                                                return std::string();
                                             if(++lpos < N)
                                                str.erase(lpos);
                                             return str;
                                           }
   K_mf(bool)         empty      () const  { const CharT* p = data_;
                                             for(int L = N;  L > 0;  --L)
                                                if(*p++ != NullChar)
                                                   return false;
                                                return true;
                                           }
   //填入
   inline K_mf(void)  assign  (const std::string& str)  { assign(str.begin(), (unsigned)str.length()); }
   inline K_mf(void)  assign  (const char* str)         { if(str) assign(str, (unsigned)strlen(str));
                                                          else    clear();  }
   inline K_mf(void)  assign  (const char* sbegin, const char* send) { assign(sbegin, (unsigned)(send-sbegin)); }
   template <class InputIterator>
    inline K_mf(void) assign  (InputIterator begin, unsigned len)
                                                { if(N > len)
                                                     std::fill_n(std::copy(begin, begin+len, data_),
                                                                 N-len, NullChar);
                                                  else
                                                     std::copy(begin, begin+N, data_);
                                                }
          K_mf(void)  assigni64 (Kway::int64 n, char prefiller = NullChar) {
                                                  char   buf[N+16];
                                                  int    len = sprintf(buf, "%0*lld", N, n);
                                                  if(prefiller != '0') {
                                                     if(prefiller==0  ||  isdigit(prefiller))   prefiller = ' ';
                                                     char*  ptr = buf;
                                                     while(*ptr=='0')   *ptr++ = prefiller;
                                                  }
                                                  if(len > N)
                                                    assign(buf + (len - N), N);
                                                  else
                                                    assign(buf, len);
          }
          inline K_mf(void)  assign  (size_t n, char prefiller = NullChar) {
             assigni64(n, prefiller);
          }
   inline K_mf(void)  clear  (CharT ch = NullChar)  { this->fill(ch); }
   //比較 compare
   inline K_mf(bool)  operator<  (const this_type& dst) const
                                 { return std::lexicographical_compare(begin(),end(),
                                                                       dst.begin(),dst.end()); }
   inline K_mf(bool)  operator== (const this_type& dst) const
                                 { return memcmp(begin(), dst.begin(), N)==0; }
                                 /* MSC的debug mode會誤判有問題,所以乾脆用memcmp()
                                 { return !std::lexicographical_compare(begin(),end(),
                                                                        dst.begin(),dst.end(),
                                                                        std::not_equal_to<CharT>()); }*/
   inline K_mf(bool)  operator!= (const this_type& dst) const  { return !operator==(dst); }
   inline K_mf(int)   compare    (const this_type& dst) const {
      return memcmp(begin(), dst.begin(), N);
   }
};
/////////////////////////////////////////////////////////////////////////////
template <class CharT, CharT NullChar>
class char_ary1 : public char_ary_base<CharT,1>
{
   typedef char_ary_base<CharT,1>     inherited;
   typedef char_ary1<CharT,NullChar>  this_type;

#ifndef __BORLANDC__
protected:
   using inherited::data_; //for 避免 gcc error.
#endif

public:
   static const CharT   Null = NullChar;
   //Types.

   //constructor/destructor
   inline          K_ctor  char_ary1  ()                         { data_[0] = NullChar;     }
   inline explicit K_ctor  char_ary1  (const char* str)          { data_[0] = *str;         }
   inline explicit K_ctor  char_ary1  (const std::string& str)   { data_[0] = *str.c_str(); }
   inline          K_ctor  char_ary1  (CharT ch)                 { data_[0] = ch;           }

   //取出
   inline K_mf(std::string)  as_string  () const  { return empty() ? std::string() : std::string(data_,1); }
   inline K_mf(bool)         empty      () const  { return(data_[0]==0  ||  data_[0]==NullChar);           }
   //填入
   inline K_mf(void)  assign  (const std::string& str)  { data_[0] = *str.c_str(); }
   inline K_mf(void)  assign  (const char* str)         { data_[0] = *str;         }
   inline K_mf(void)  assign  (CharT ch)                { data_[0] = ch;           }
   inline K_mf(void)  clear   ()                        { data_[0] = NullChar;     }
   //比較 compare
   inline K_mf(bool)  operator<  (const this_type& dst) const   { return data_[0] <  dst.data_[0]; }
   inline K_mf(bool)  operator== (const this_type& dst) const   { return data_[0] == dst.data_[0]; }
   inline K_mf(bool)  operator!= (const this_type& dst) const   { return data_[0] != dst.data_[0]; }
};
//---------------------------------------------------------------------------
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char, 1,' '>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char, 2,' '>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char, 3,' '>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char, 4,' '>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char, 5,' '>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char, 6,' '>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char, 7,' '>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char, 8,' '>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char, 9,' '>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char,10,' '>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char,11,' '>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char,12,' '>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char,13,' '>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char,14,' '>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char,15,' '>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char,16,' '>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char,40,' '>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char,80,' '>;
//---------------------------------------------------------------------------
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char, 1, 0>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char, 2, 0>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char, 3, 0>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char, 4, 0>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char, 5, 0>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char, 6, 0>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char, 7, 0>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char, 8, 0>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char, 9, 0>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char,10, 0>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char,11, 0>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char,12, 0>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char,13, 0>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char,14, 0>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char,15, 0>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char,16, 0>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char,40, 0>;
LIBNS_EXTERN_TEMPLATE_CLASS     Kstl::char_ary<char,80, 0>;
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

