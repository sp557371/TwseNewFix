//
// 格式化字串拆解工具
//
// 版本: $Id: TokenStr.h,v 1.1 2004/09/14 03:23:40 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TokenStrH
#define TokenStrH
//---------------------------------------------------------------------------
#include <vector>
#include <string>
//---------------------------------------------------------------------------
#ifdef KWAY_BUILD_API
   #define LIBNS_class  class
   #define K_ctor
   #define K_dtor
   #define K_mf(rtn)    rtn
   #define TTokenStr    TKwayTokenStr
#else
   #include "KwayBase.h"
   namespace Kway {
#endif
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TTokenStr : private std::vector<char*>
{
   typedef std::vector<char*>    TResult;
   std::string    Destin_;
   size_type      SizeCatch_;

public:
   K_ctor  TTokenStr  ();
   K_ctor  TTokenStr  (const char* format, const std::string& source);
   K_ctor  TTokenStr  (const char* format, const char* source);

   K_mf(size_type)  Do  (const char* format, const char* source);

   inline K_mf(bool)       empty   ()            const  { return SizeCatch_==0; }
   inline K_mf(size_type)  size    ()            const  { return SizeCatch_;    }
   inline K_mf(char*)  operator[]  (size_type n) const  { return n >= size() ? 0  : TResult::operator[](n); }
   inline K_mf(const char*) getstr (size_type n) const  { return n >= size() ? "" : TResult::operator[](n); }
};
/////////////////////////////////////////////////////////////////////////////
#ifndef KWAY_BUILD_API
   } // namespace Kway
#endif
//---------------------------------------------------------------------------
#endif

