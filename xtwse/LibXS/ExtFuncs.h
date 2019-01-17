//
// �@���B�~�� functions
//
// ����: $Id: ExtFuncs.h,v 1.4 2004/10/04 01:13:28 fonwin Exp $
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
// src = �N�n�Q�������r��,�Ydst�䤣��src,�h�����Ǧ^dst
// dst = �Q������
extern LIBNSDLL char* ReplaceStr (char* dst, const char* src, const char* fmt, ...);
//---------------------------------------------------------------------------
LIBNS_fn(std::string)  ChangeYMD  (const char* src, TKDate date = TKDate::Now());
/////////////////////////////////////////////////////////////////////////////
LIBNS_fn(int)  GetArgs  (char argcode, const char** param, int argFrom = 1);
/////////////////////////////////////////////////////////////////////////////
LIBNS_fn(const char*)  SkipSpaces     (const char* data);
inline LIBNS_fn(char*) SkipSpaces     (char* data)  { return const_cast<char*>(SkipSpaces(const_cast<const char*>(data))); }
LIBNS_fn(const char*)  SkipEndSpaces  (const char* beg, const char* end);///< return: �h�����ݫ᪺ end;
inline LIBNS_fn(char*) SkipEndSpaces  (char* beg, char* end)  { return const_cast<char*>(SkipEndSpaces((const char*)beg, (const char*)end)); }
LIBNS_fn(bool)         SetPathOption  (const std::string& lndata, std::string& path);
LIBNS_fn(std::string)  SetPath        (const std::string& path);//�p�G�ݭn,�b���ݥ[�W '/'
LIBNS_fn(std::string)  MergeFileName  (const std::string& path, const std::string& fname);
LIBNS_fn(std::string)  ExtractPath     (const std::string& fileName);
LIBNS_fn(std::string)  ExtractFileName (const std::string& fileName);
LIBNS_fn(char*)        FetchLine       (char*& msgstr);

/** �P�_ string �O�_�ŦX wild�r�ꪺ�w�q.
    wild�����e�i�H�� '*' �� '?'
    wlen = wild ���r�����, ���� >= 0.
    �Youtptr!=NULL, �h *outptr = string��������m.
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
��U�P�_ static/global ����O�_�w�g���`!
�z�i�H�b�z��.cpp����,�z�w�q�����󤧫e���إߤ@�� TStaticState ����,
�M��b�ϥαz���ܼƤ��e,�P�_�����󪺪��A�Y�i.
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

