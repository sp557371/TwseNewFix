//
// 一些額外的 functions
//
// 版本: $Id: ExtFuncs.h,v 1.4 2004/10/04 01:13:28 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef ExtFuncsH
#define ExtFuncsH
//---------------------------------------------------------------------------
#include <string>
#include <errno.h>
#include "KTime.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_gd(int)      _argc;
LIBNS_gd(char**)   _argv;
//---------------------------------------------------------------------------
LIBNS_fn(void)  InitExtFuncs  (int argc, char** argv);
//---------------------------------------------------------------------------
extern LIBNSDLL int   ErrLog            (const char* fmt, ...);
LIBNS_fn(void)        SetErrLogFileName (const std::string& fileName);
LIBNS_fn(std::string) GetErrLogFileName ();
//---------------------------------------------------------------------------
// src = 將要被替換的字串,若dst找不到src,則直接傳回dst
// dst = 被替換後
extern LIBNSDLL char* ReplaceStr (char* dst, const char* src, const char* fmt, ...);
//---------------------------------------------------------------------------
LIBNS_fn(std::string)  ChangeYMD  (const char* src, TKDate date = TKDate::Now());
/////////////////////////////////////////////////////////////////////////////
LIBNS_fn(int)  GetArgs  (char argcode, const char** param, int argFrom = 1);
/////////////////////////////////////////////////////////////////////////////
LIBNS_fn(const char*)  SkipSpaces     (const char* data);
inline LIBNS_fn(char*) SkipSpaces     (char* data)  { return const_cast<char*>(SkipSpaces(const_cast<const char*>(data))); }
LIBNS_fn(const char*)  SkipEndSpaces  (const char* beg, const char* end);///< return: 去除尾端後的 end;
inline LIBNS_fn(char*) SkipEndSpaces  (char* beg, char* end)  { return const_cast<char*>(SkipEndSpaces((const char*)beg, (const char*)end)); }
LIBNS_fn(bool)         SetPathOption  (const std::string& lndata, std::string& path);
LIBNS_fn(std::string)  SetPath        (const std::string& path);//如果需要,在尾端加上 '/'
LIBNS_fn(std::string)  MergeFileName  (const std::string& path, const std::string& fname);
LIBNS_fn(std::string)  ExtractPath     (const std::string& fileName);
LIBNS_fn(std::string)  ExtractFileName (const std::string& fileName);
LIBNS_fn(char*)        FetchLine       (char*& msgstr);

/** 判斷 string 是否符合 wild字串的定義.
    wild的內容可以有 '*' 或 '?'
    wlen = wild 的字串長度, 必須 >= 0.
    若outptr!=NULL, 則 *outptr = string的結束位置.
*/
LIBNS_fn(bool) IsWildStrMatch (const char* wild, const char* string, int wlen, const char** outptr = NULL);
LIBNS_fn(bool) IsWildStrMatch (const char* wild, const char* string, const char** outptr = NULL);
/////////////////////////////////////////////////////////////////////////////
#ifdef K_WINDOWS
LIBNS_fn(std::string) SysErrorMessage (DWORD eno);
#define strerror(eno)   Kway::SysErrorMessage(eno).c_str()
#ifdef errno
#undef errno
#endif
#define errno           ::GetLastError() 
#else
inline const char* SysErrorMessage (int eno)	{ return strerror(eno); }
#endif
/////////////////////////////////////////////////////////////////////////////
/**
協助判斷 static/global 物件是否已經死亡!
您可以在您的.cpp之中,您定義的物件之前先建立一個 TStaticState 物件,
然後在使用您的變數之前,判斷此物件的狀態即可.
*/
struct TStaticState
{
   enum EState {
      ss_Initing,
      ss_Destroying,
   };
   inline K_ctor       TStaticState ()  : State_(ss_Initing) {}
   inline K_dtor      ~TStaticState ()  { State_ = ss_Destroying; }
   inline K_mf(EState) GetState     () const  { return State_; }
   inline K_mf(bool)   IsDestroying () const  { return State_==ss_Destroying; }
   inline K_mf(bool)   IsRunning    () const  { return State_==ss_Initing;    }
private:
   EState   State_;
};
/////////////////////////////////////////////////////////////////////////////
typedef K_mf(const std::string)     (*TGetConfig)  ();
K_gd(TGetConfig)                   GetLogPath;
K_gd(TGetConfig)                   GetProcLogPath;
K_gd(TGetConfig)                   GetConfPath;
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
#endif

