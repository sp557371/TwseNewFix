
   #error "請改使用 Nsa_LineBuf.h TLineBufNew"

/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_DynaBuffer.h"
#include "MemPool.h"
#include "Nsa_SysInfo.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TDynaBuffer impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TDynaBuffer::TDynaBuffer (size_type size)
   : Buffer_   (NULL)
   , Size_     (0)
   , UsedSize_ (0)
   , AllocSize_(size)
{
   Buffer_ = Alloc(size);
   if (Buffer_) {
      Size_ = size;
   }
}
//---------------------------------------------------------------------------

K_dtor TDynaBuffer::~TDynaBuffer ()
{
   Close();
}
//---------------------------------------------------------------------------

K_mf(void) TDynaBuffer::Clear ()
{
   if (Buffer_) {
      UsedSize_ = 0;
      memset(Buffer_, 0, Size_);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TDynaBuffer::Append (const char* str)
{
   if (str == NULL)
      return;

   Append(str, strlen(str));
}
//---------------------------------------------------------------------------

K_mf(void) TDynaBuffer::Append (char ch, size_t size)
{
   char buf[1024];

   memset(buf, (int)ch, size);
   Append(buf, size);
}
//---------------------------------------------------------------------------

K_mf(void) TDynaBuffer::Append (const void* mem, size_type len)
{
   if (len == 0)
      return;

   if (Check(len+1) == false)   // 不管是什麼型態都保留結束字元
      return;

   memcpy(Buffer_+UsedSize_, mem, len);
   
   UsedSize_ += len;
   Buffer_[UsedSize_] = '\0';
}
//---------------------------------------------------------------------------

K_mf(TDynaBuffer::size_type) TDynaBuffer::Find (size_type pos, const void* buf, size_type size)
{
   if (pos >= UsedSize_)
      return npos;

   // 如果要比對的字串比現在內容還長的話就不用比了
   if (size > UsedSize_)
      return npos;

   const char* beg = (Buffer_ + pos);
   const char* end = (Buffer_ + UsedSize_ - size + 1);

   for (; beg!=end; ++beg) {
      if (memcmp(beg, buf, size) == 0) {
         size_type pos = (beg - Buffer_);
         return pos;
      }
   }

   return npos;
}
//---------------------------------------------------------------------------

K_mf(TDynaBuffer::size_type) TDynaBuffer::MoveFirst (size_type pos)
{
   if (pos == 0 || pos >= UsedSize_)
      return UsedSize_;

   UsedSize_ = UsedSize_ - pos;
   memcpy(Buffer_, Buffer_+pos, UsedSize_);
   Buffer_[UsedSize_] = '\0';

   return UsedSize_;
}
//---------------------------------------------------------------------------

K_mf(void) TDynaBuffer::Close ()
{
   if (Buffer_) {
      Free(Buffer_);
      Buffer_   = NULL;
      Size_     = 0;
      UsedSize_ = 0;
   }
}
//---------------------------------------------------------------------------

K_mf(bool) TDynaBuffer::Check (size_type size)
{
   size_type newSize = (UsedSize_ + size);

   if (Size_ <= newSize) {
      //===========================================
      // alloc AllocSize_ 的倍數
      size_type num     = (newSize / AllocSize_) + 1;
                newSize = num * AllocSize_;

      char* ptr = Alloc(newSize);
      if (ptr == NULL)
         return false;

      if (Buffer_) {
         memcpy(ptr, Buffer_, UsedSize_);
         Free(Buffer_);
      }

      Buffer_ = ptr;
      Size_   = newSize;
   }

   return true;
}
//---------------------------------------------------------------------------

K_mf(char*) TDynaBuffer::Alloc (size_type size)
{
   return (char*)(TSysInfo::GetMemPool().Alloc(size));
}
//---------------------------------------------------------------------------

K_mf(void) TDynaBuffer::Free (void* mem)
{
   TSysInfo::GetMemPool().Free(mem);
}
//---------------------------------------------------------------------------

K_mf(int) TDynaBuffer::GetInt (size_type pos)
{
   if (pos >= UsedSize_)
      return 0;

   return atoi(Buffer_+pos);
}
//---------------------------------------------------------------------------

