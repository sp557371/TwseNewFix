
   #error "請改使用 Nsa_LineBuf.h TLineBufNew"

/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_DynaBuffer_h_)
#define _Nsa_DynaBuffer_h_
//---------------------------------------------------------------------------
#include <string>
#include "KwayBase.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TDynaBuffer
/////////////////////////////////////////////////////////////////////////////
K_class TDynaBuffer
{
public:
   typedef size_t  size_type;

   enum {
      Default_Alloc_Size = 512,
   };

protected:
   char*      Buffer_;
   size_type  Size_;
   size_type  UsedSize_;
   size_type  AllocSize_;

public:
   K_ctor  TDynaBuffer (size_type size = Default_Alloc_Size);
   K_dtor ~TDynaBuffer ();

   K_mf(void) Append (const char*);
   K_mf(void) Append (char, size_t);
   K_mf(void) Append (const void*, size_t);

   K_mf(size_type) Find      (size_type, const void*, size_type);


   // 給定一個位置, 把這個位址以後的資料搬到最前面
   // 回傳最後 UsedSize_
   K_mf(size_type) MoveFirst (size_type);

   // 給定一個位置, 取得這個位址以後的 int
   K_mf(int) GetInt (size_type);

   K_mf(void) Clear  ();

   //==============================================
   // inline
   inline K_mf(size_type) Find (      char  ch )  { return Find(0, &ch, 1);           }
   inline K_mf(size_type) Find (const char* str)  { return Find(0, str, strlen(str)); }

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
   
   inline K_mf(const char*) GetCStr     () const  { return Buffer_;   }
   inline K_mf(size_type)   GetUsedSize () const  { return UsedSize_; }

   static const size_type npos = -1;

private:
   K_mf(bool)  Check (size_t);
   K_mf(void)  Close ();
   K_mf(char*) Alloc (size_t);
   K_mf(void)  Free  (void*);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_DynaBuffer_h_)

