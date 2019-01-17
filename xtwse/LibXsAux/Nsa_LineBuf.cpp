/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include <stdlib.h>
#include "Nsa_LineBuf.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;
using std::min;

/////////////////////////////////////////////////////////////////////////////
// class TLineBufBase
/////////////////////////////////////////////////////////////////////////////
K_ctor TLineBufBase::TLineBufBase ()
   : Buffer_   (NULL)
   , BufSize_  (0)
   , UsedSize_ (0)
   , AllocSize_(0)
{
}
//---------------------------------------------------------------------------

K_ctor TLineBufBase::TLineBufBase (char* buf, size_type bufSize)
   : Buffer_   (buf)
   , BufSize_  (bufSize)
   , UsedSize_ (0)
   , AllocSize_(0)
{
   if (!Buffer_)
      BufSize_ = 0;
}
//---------------------------------------------------------------------------

K_ctor TLineBufBase::TLineBufBase (char* buf, size_type bufSize, size_type allocSize)
   : Buffer_   (buf)
   , BufSize_  (bufSize)
   , UsedSize_ (0)
   , AllocSize_(allocSize)
{
   if (!Buffer_)
      BufSize_ = 0;
}
//---------------------------------------------------------------------------

K_mf(void) TLineBufBase::Clear ()
{
   if (Buffer_ && BufSize_) {
      UsedSize_  = 0;
      Buffer_[0] = 0x00;
   }
}
//---------------------------------------------------------------------------

K_mf(TLineBufBase::size_type) TLineBufBase::Check (size_type size)
{
   // 在 Check 時多保留一個結束字元
   size_type newSize = (UsedSize_ + size + 1);

   if (BufSize_ < newSize) {
      if (AllocSize_ != 0) {
         // 配置 AllocSize_ 的倍數
         size_type num     = (newSize / AllocSize_) + 1;
                   newSize = num * AllocSize_;
      }

      char* ptr = Alloc(newSize);
      if (ptr == NULL) {
         // 配置失敗, 那也就只能把 buf 填滿
         size_type leftSize = (BufSize_ - UsedSize_);
         if (leftSize <= 1)
            return 0;

         return min(leftSize, size-1);
      }

      if (Buffer_) {
         memcpy(ptr, Buffer_, UsedSize_);
         Free(Buffer_);
      }

      Buffer_  = ptr;
      BufSize_ = newSize;
   }

   return size;
}
//---------------------------------------------------------------------------

K_mf(void) TLineBufBase::Assign (const char* str)
{
   if (str) {
      Assign(str, strlen(str));
   }
}
//---------------------------------------------------------------------------

K_mf(void) TLineBufBase::Assign (const void* mem, size_type len)
{
   UsedSize_ = 0;
   Append(mem, len);
}
//---------------------------------------------------------------------------

K_mf(void) TLineBufBase::Append (const char* str)
{
   if (str == NULL)
      return;

   Append(str, strlen(str));
}
//---------------------------------------------------------------------------

K_mf(void) TLineBufBase::Append (char ch, size_t size)
{
   char buf[1024];

   memset(buf, (int)ch, size);
   Append(buf, size);
}
//---------------------------------------------------------------------------

K_mf(void) TLineBufBase::Append (const void* mem, size_type len)
{
   if (len == 0)
      return;

   size_type cpyLen = Check(len);
   if (cpyLen == 0)
      return;

   memcpy(Buffer_+UsedSize_, mem, cpyLen);

   UsedSize_ += cpyLen;
   Buffer_[UsedSize_] = '\0'; // 不管是什麼型態都保留結束字元
}
//---------------------------------------------------------------------------

K_mf(TLineBufBase::size_type) TLineBufBase::FindStr (size_type pos, const char* str)
{
   size_t strSize = strlen(str);
   if (!str || strSize == 0 || pos >= UsedSize_)
      return npos;

   // 如果要比對的字串比現在指定內容還長的話就不用比了
   size_t leftSize = UsedSize_ - pos;
   if (strSize > leftSize)
      return npos;

   const char* beg  = (Buffer_ + pos);
   const char* resu = strstr(beg, str);

   if (resu) {
      size_type pos = (resu - Buffer_);
      return pos;
   }

   return npos;
}
//---------------------------------------------------------------------------

K_mf(TLineBufBase::size_type) TLineBufBase::FindMem (size_type pos, const void* cmpBuf, size_type cmpSize, size_type* posScan)
{
   if (!cmpBuf || cmpSize == 0 || pos >= UsedSize_) {
      // 要比對的位置比現在的長度還大
      // posScan 不變
      return npos;
   }

   // 如果要比對的字串比現在指定內容還長的話就不用比了
   size_t leftSize = UsedSize_ - pos;
   if (cmpSize > leftSize) {
      // posScan 不變
      return npos;
   }

   const char*     beg      = (Buffer_ + pos);
   const char*     end      = (Buffer_ + UsedSize_ - cmpSize + 1);
   const char*     cmpData  = (const char*)cmpBuf;
         int       first    = (int)cmpData[0];
         size_type posFound = npos;
   const void*     mem;

   do {
      mem = memchr(beg, first, end-beg);
      if (!mem) {
         // 找不到喔,異動 posScan 的位置
         if (posScan)
            *posScan = UsedSize_;

         break;
      }
      // 找到第一個字元了, 看看之後的資料有沒有一樣
      beg = (const char*)mem;
      if (cmpSize == 1) {
         posFound = (beg - Buffer_);
         return posFound;
      }

      ++beg;
      if (memcmp(beg, cmpData+1, cmpSize-1) == 0) {
         posFound = (beg - 1 - Buffer_);
         return posFound;
      }
      else {
         // 紀錄目前的 posScan
         if (posScan)
            *posScan = (beg - Buffer_);
      }
   
   } while (beg < end);

   return posFound;

   /*
   if (pos >= UsedSize_)
      return npos;

   // 如果要比對的字串比現在指定內容還長的話就不用比了
   size_t leftSize = UsedSize_ - pos;
   if (cmpSize > leftSize)
      return npos;

   const char* beg = (Buffer_ + pos);
   const char* end = (Buffer_ + UsedSize_ - cmpSize + 1);

   for (; beg<end; ++beg) {
      if (memcmp(beg, cmpBuf, cmpSize) == 0) {
         size_type pos = (beg - Buffer_);
         return pos;
      }
   }

   return npos;
   */
}
//---------------------------------------------------------------------------

K_mf(TLineBufBase::size_type) TLineBufBase::MoveFirst (size_type pos)
{
   if (pos == 0 || pos > UsedSize_)
      return UsedSize_;

   UsedSize_ = UsedSize_ - pos;
   if (UsedSize_ > 0)
      memcpy(Buffer_, Buffer_+pos, UsedSize_);
   Buffer_[UsedSize_] = '\0';

   return UsedSize_;
}
//---------------------------------------------------------------------------

K_mf(int) TLineBufBase::GetInt (size_type pos)
{
   if (pos >= UsedSize_)
      return 0;

   return atoi(Buffer_+pos);
}
//---------------------------------------------------------------------------

K_mf(void) TLineBufBase::EraseBack (size_type len)
{
   if (len > UsedSize_)
      return;

   UsedSize_ -= len;
   Buffer_[UsedSize_] = '\0';
}
//---------------------------------------------------------------------------

K_mf(TLineBufBase::size_type) TLineBufBase::GetLeftSize () const
{
   // 要保留一個位置讓我放 '\0' 喔
   size_t bufSz = (BufSize_ - 1);

   if (bufSz <= UsedSize_)
      return 0;

   return (bufSz - UsedSize_);
}
//---------------------------------------------------------------------------

K_mf(void) TLineBufBase::IncUsedSize (size_type szInc)
{
   // 要保留一個位置讓我放 '\0' 喔
   size_t szUsed = (UsedSize_ + szInc);

   if (szUsed >= BufSize_)
      szUsed = (BufSize_ - 1);

   UsedSize_          = szUsed;
   Buffer_[UsedSize_] = '\0';
}


/////////////////////////////////////////////////////////////////////////////
// class TLineBuf impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TLineBuf::TLineBuf (size_t bufSize)
   : inherited    (new char[bufSize], bufSize)
   , IsNeedToFree_(true)
{
}
//---------------------------------------------------------------------------

K_ctor TLineBuf::TLineBuf (char* buf, size_t bufSize)
   : inherited    (buf, bufSize)
   , IsNeedToFree_(false)
{
}
//---------------------------------------------------------------------------

K_dtor TLineBuf::~TLineBuf ()
{
   if (IsNeedToFree_ && Buffer_)
      delete [] Buffer_;
}


/////////////////////////////////////////////////////////////////////////////
// class TLineBufNew impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TLineBufNew::TLineBufNew (size_t bufSize)
   : inherited(new char[bufSize], bufSize, bufSize)
{
}
//---------------------------------------------------------------------------

K_ctor TLineBufNew::TLineBufNew (const TLineBufNew& a)
   : inherited(new char[a.BufSize_], a.BufSize_, a.AllocSize_)
{
   inherited::Assign(a.Buffer_, a.UsedSize_);
}
//---------------------------------------------------------------------------

K_dtor TLineBufNew::~TLineBufNew ()
{
   if (Buffer_)
      delete [] Buffer_;
}
//---------------------------------------------------------------------------
