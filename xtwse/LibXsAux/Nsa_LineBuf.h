/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_LineBuf_h_)
#define _Nsa_LineBuf_h_
//---------------------------------------------------------------------------
#include <string>
#include "libnsAux.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TLineBufBase
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TLineBufBase
{
public:
   typedef size_t  size_type;

protected:
   char*   Buffer_;
   size_t  BufSize_;
   size_t  UsedSize_;
   size_t  AllocSize_;

public:
           K_ctor  TLineBufBase ();
           K_ctor  TLineBufBase (char*, size_type);
           K_ctor  TLineBufBase (char*, size_type, size_type);
   virtual K_ctor ~TLineBufBase ()  {}

   //==============================================
   // Assign
   K_mf(void) Assign (const char*);
   K_mf(void) Assign (const void*, size_type);

   //==============================================
   // Append
   K_mf(void) Append (const char*);
   K_mf(void) Append (char, size_t size = 1);
   K_mf(void) Append (const void*, size_type);

   //==============================================
   // Find
   K_mf(size_type) FindStr (size_type pos, const char* str);
   K_mf(size_type) FindMem (size_type pos, const void* cmpBuf, size_type cmpSize, size_type* posScan);

   inline K_mf(size_type) FindChar (char ch, size_type pos = 0)
      {
         char buf[10];
         buf[0] = ch;
         buf[1] = '\0';
         return FindStr(pos, buf);
      }

   // 給定一個位置, 把這個位址以後的資料搬到最前面
   // 回傳最後 UsedSize_
   K_mf(size_type) MoveFirst (size_type);

   // 給定一個位置, 取得這個位址以後的 int
   K_mf(int) GetInt (size_type);

   // 從後面移除
   K_mf(void) EraseBack (size_type);

   K_mf(void) Clear ();

   // 累加已使用大小
   K_mf(void) IncUsedSize (size_type);

   //==============================================
   // inline
   inline K_mf(void) Append             (const std::string& str)  { Append(str.c_str(), str.length()); }
   inline K_mf(void) Append_WinNewLine  ()                        { Append("\r\n", 2);                 }
   inline K_mf(void) Append_UnixNewLine ()                        { Append("\n",   1);                 }

   inline K_mf(void) AppendNewLine ()
      {
         #if defined(K_WINDOWS)
         Append_WinNewLine();
         #else
         Append_UnixNewLine();
         #endif
      }

   inline K_mf(void*) GetBegPtr ()  { return Buffer_;           }
   inline K_mf(void*) GetCurPtr ()  { return Buffer_+UsedSize_; }

          K_mf(size_type)   GetLeftSize () const;
   inline K_mf(bool)        Empty       () const  { return (UsedSize_ == 0);     }
   inline K_mf(const char*) GetCStr     () const  { return Buffer_;              }
   inline K_mf(size_type)   GetUsedSize () const  { return UsedSize_;            }

   inline K_mf(char) operator [] (unsigned n) const  { return Buffer_[n]; }

   static const size_type npos = (size_type)-1;

protected:
   //==============================================
   // TLineBufBase virtual function
   virtual K_mf(char*) Alloc (size_type)  { return NULL; }
   virtual K_mf(void)  Free  (char*)      {              }
   
   //==============================================
   // TLineBufBase function
   K_mf(size_type) Check (size_type);
};


/////////////////////////////////////////////////////////////////////////////
// class TLineBuf
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TLineBuf : public TLineBufBase
{
   typedef TLineBufBase  inherited;

   bool  IsNeedToFree_;

public:
   K_ctor  TLineBuf (           size_t bufSize = 512);
   K_ctor  TLineBuf (char* buf, size_t bufSize);
   K_dtor ~TLineBuf ();
};


/////////////////////////////////////////////////////////////////////////////
// class TLineBufNew
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TLineBufNew : public TLineBufBase
{
   typedef TLineBufBase  inherited;

public:
   K_ctor TLineBufNew (size_t bufSize = 512);
   K_ctor TLineBufNew (const TLineBufNew&);
   
   K_dtor ~TLineBufNew ();

protected:
   //==============================================
   // overwrite TLineBufBase virtual function
   inline K_mf(char*) Alloc (size_type n)
      {
         char* ptr = new char[n];
         return ptr;
      }
   
   inline K_mf(void) Free (char* ptr)  { delete [] ptr; }

private:
   //==============================================
   // 尚未實作
   K_mf(void) operator = (const TLineBufNew&);
};


/////////////////////////////////////////////////////////////////////////////
// class TLineBufMp
/////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_LineBuf_h_)
