/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_UtilStr.h"
#include <stdio.h>
#include <stdlib.h>
#include "ExtFuncs.h"
#include "Nsa_LineBuf.h"
//---------------------------------------------------------------------------
#if defined(__BORLANDC__)
   #include <sysutils.hpp>
#endif
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TStrSplit impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TStrSplit::TStrSplit ()
{
}
//------------------------------------------------------------------------

K_ctor TStrSplit::TStrSplit (const char* str, const char ch)
{
   SplitByChar(str, ch);
}
//------------------------------------------------------------------------

K_ctor TStrSplit::TStrSplit (const char* str, const char* SplitStr)
{
   SplitByStr(str, SplitStr);
}
//------------------------------------------------------------------------

K_dtor TStrSplit::~TStrSplit ()
{
   Clear();
}
//------------------------------------------------------------------------

K_mf(size_t) TStrSplit::SplitByChar (const char* str, const char ch)
{
   Clear();

   const char*  head = str;
   const char*  tail;
   size_t       strLen = strlen(str);
   SData        one;

   tail = strchr(head, (int)ch);
   while (tail) {
      one.len = (tail - head);
      // 在配置空間時多配給個, 在取資料時會在尾端加結束符號
      one.dat = new char[one.len + Default_Extra_Num];
      if (one.dat != NULL) {
			memcpy(one.dat, head, one.len);
         memset(one.dat+one.len, 0, Default_Extra_Num);
      }

      DataA_.push_back(one);

      head = (tail + 1);
      tail = strchr(head, ch);
      strLen -= one.len + 1;
   }

   if (strLen > 0) {
      one.len = strLen;
      // 在配置空間時多配給個, 在取資料時會在尾端加結束符號
		one.dat = new char[one.len + Default_Extra_Num];
		if (one.dat != NULL)
		{
			memcpy(one.dat, head, one.len);
         memset(one.dat+one.len, 0, Default_Extra_Num);
		}
		DataA_.push_back(one);
   }

   return DataA_.size();
}
//------------------------------------------------------------------------

K_mf(size_t) TStrSplit::SplitByStr (const char* str, const char* SplitStr)
{
   Clear();

   const char* head = str;
   const char* tail;
   size_t      strLen      = strlen(str);
   size_t      SplitStrLen = strlen(SplitStr);
   SData       one;

   tail = strstr(head, SplitStr);
   while (tail) {
      one.len = (tail - head);
      // 在配置空間時多配給個, 在取資料時會在尾端加結束符號
      one.dat = new char[one.len + Default_Extra_Num];
      if (one.dat != NULL) {
			memcpy(one.dat, head, one.len);
         memset(one.dat+one.len, 0, Default_Extra_Num);
      }

      DataA_.push_back(one);

      head    = (tail + SplitStrLen);
      tail    = strstr(head, SplitStr);
      strLen -= (one.len + SplitStrLen);
   }

   if (strLen > 0) {
      one.len = strLen;
      // 在配置空間時多配給個, 在取資料時會在尾端加結束符號
		one.dat = new char[one.len + Default_Extra_Num];
		if (one.dat != NULL)
		{
			memcpy(one.dat, head, one.len);
         memset(one.dat+one.len, 0, Default_Extra_Num);
		}
		DataA_.push_back(one);
   }

   return DataA_.size();
}
//------------------------------------------------------------------------

K_mf(const char*) TStrSplit::GetData (size_t idx)
{
   if (idx >= DataA_.size())
      return NULL;

   TDataI ita = (DataA_.begin() + idx);
   if (ita->len == 0 || ita->dat == NULL)
      return NULL;

   return (const char*)ita->dat;
}
//------------------------------------------------------------------------

K_mf(std::string) TStrSplit::GetStr (size_t idx)
{
   const char* cpData = GetData(idx);

   if (cpData == NULL) {
      return std::string();
   }

   return std::string(cpData);
}
//------------------------------------------------------------------------

K_mf(int) TStrSplit::GetInt (size_t idx)
{
   const char* cpData = GetData(idx);

   if (cpData == NULL) {
      return 0;
   }

   return atoi(cpData);
}
//------------------------------------------------------------------------

K_mf(int) TStrSplit::GetInt (size_t idx, const char* key, char keysp)
{
   char* cpData = (char*)GetData(idx);

   if (cpData == NULL) {
      return 0;
   }

   size_t keyLen = strlen(key);
   
   if (strncmp(key, cpData, keyLen) == 0) {
      char* ptr = strchr(cpData+keyLen, (int)keysp);
      if (ptr) {
         return atoi(ptr+1);
      }
   }
   
   return 0;
}
//------------------------------------------------------------------------

K_mf(void) TStrSplit::Clear ()
{
   for (TDataI ita=DataA_.begin(); ita!=DataA_.end(); ++ita) {
      if (ita->dat) {
         delete [] ita->dat;
         ita->dat = NULL;
         ita->len = 0;
      }
   }

   DataA_.clear();
}
//------------------------------------------------------------------------

K_mf(char*) TStrSplit::GetKeyVal (const char* key, char keysp, TDataI& ita)
{
   char   caKey[50];
   size_t nKeyLen;

   if (ita->dat == NULL) {
      return NULL;
   }

   char* ptr = strchr(ita->dat, (int)keysp);
   if (ptr == NULL) {
      return NULL;
   }

   nKeyLen = (ptr - ita->dat);
   memcpy(caKey, ita->dat, nKeyLen);
   caKey[nKeyLen] = '\0';

   if (strcmp(key, caKey) != 0) {
      return NULL;
   }

   return (ptr + 1);
}
//------------------------------------------------------------------------

K_mf(bool) TStrSplit::FindStr_ByKey (const char* key, char keysp, std::string& str)
{
   char* ptr;

   str.clear();
   for (TDataI ita=DataA_.begin(); ita!=DataA_.end(); ++ita) {
      ptr = GetKeyVal(key, keysp, ita);
      if (ptr) {
         str = ptr;
         return true;
      }
   }

   return false;
}
//------------------------------------------------------------------------

K_mf(bool) TStrSplit::FindInt_ByKey (const char* key, char keysp, int& ResVal)
{
   std::string str;

   if (FindStr_ByKey(key, keysp, str) == true) {
      ResVal = atoi(str.c_str());
      return true;
   }
   return false;
}
//------------------------------------------------------------------------

K_mf(std::string) TStrSplit::GetStr_ByKey (const char* key, char keysp)
{
   std::string str;

   if (FindStr_ByKey(key, keysp, str) == true) {
      return str;
   }
   return std::string();
}
//------------------------------------------------------------------------

K_mf(int) TStrSplit::GetInt_ByKey (const char* key, char keysp)
{
   std::string str;

   if (FindStr_ByKey(key, keysp, str) == true) {
      return atoi(str.c_str());
   }
   return 0;
}
//------------------------------------------------------------------------

K_mf(char) TStrSplit::GetChar_ByKey (const char* key, char keysp)
{
   std::string str = GetStr_ByKey(key, keysp);

   if (str.empty()) {
      return 0;
   }
   
   return str[0];
}


/////////////////////////////////////////////////////////////////////////////
// class TUtilStr impl
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TUtilStr::MakeLowercase (const std::string& strSrc)
{
   std::string            strDst(strSrc);
   std::string::size_type len = strDst.length();
      
   for (std::string::size_type i=0; i<len; ++i)
      strDst = tolower(strSrc[i]);

   return strDst;
}
//---------------------------------------------------------------------------

K_mf(const char*) TUtilStr::MakeUppercase (const char* src, char* dst)
{
   size_t      len    = strlen(src);
   const char* srcEnd = (src + len);
   char*       dstCur = dst;

   while (src < srcEnd) {
      *dstCur = toupper(*src);

      ++src;
      ++dstCur;
   }
   *dstCur = '\0';

   return dst;
}
//---------------------------------------------------------------------------
#if defined(__BORLANDC__)
K_mf(int) TUtilStr::AnsiToUTF8 (const char* src, char* dst, int dstSize)
{
   WideString wsSrc(src);
   return WideCharToMultiByte(CP_UTF8, 0, wsSrc.c_bstr(), -1, dst, dstSize, NULL, NULL);
}
#endif
//---------------------------------------------------------------------------

K_mf(const char*) TUtilStr::HasBig5Code (const char* str)
{
   if (str == NULL)
      return NULL;

   size_t len = strlen(str);
   if (len <= 1)
      return NULL;

   const char* end = (str + len - 1);
   word  val;

   for (const char* beg=str; beg!=end; ++beg) {
      val = (*beg)*256 + *(beg+1);
      if ((val >= 41280 && val <= 41915) ||
          (val >= 41917 && val <= 41919) ||
          (val >= 42048 && val <= 50814) ||
          (val >= 50849 && val <= 51411) ||
          (val >= 51520 && val <= 63998))
      {
         return beg;
      }
   }

   return NULL;
}
//---------------------------------------------------------------------------

K_mf(std::string) TUtilStr::LTrim (const std::string& str)
{
   if (str.empty())
      return std::string();

   const char* beg = str.c_str();
   const char* cur = SkipSpaces(beg);

   if (!cur)
      return std::string();
   else if (cur == beg)
      return str;
     
   return std::string(cur);
}
//---------------------------------------------------------------------------

K_mf(std::string) TUtilStr::RTrim (const std::string& str)
{
   if (str.empty())
      return std::string();

   const char* beg = str.c_str();
   const char* end = strchr(beg, 0);
   const char* cur = SkipEndSpaces(beg, end);

   if (cur == beg)
      return str;
   
   return std::string(beg, end);
}
//---------------------------------------------------------------------------

K_mf(std::string) TUtilStr::Trim (const std::string& str)
{
   if (str.empty())
      return std::string();

   const char* beg    = str.c_str();
   const char* newBeg = SkipSpaces(beg);

   if (!newBeg)
      return std::string();

   const char* end    = strchr(newBeg, 0);
   const char* newEnd = SkipEndSpaces(newBeg, end);

   if (newBeg == beg && newEnd == end)
      return str;

   return std::string(newBeg, newEnd);
}


/////////////////////////////////////////////////////////////////////////////
// class TSettStrBase impl
/////////////////////////////////////////////////////////////////////////////
const char  TSettStrBase::DefSpl[]  = { 0x01, 0x00 };
const int   TSettStrBase::DefSplLen = 1;
//---------------------------------------------------------------------------
K_mf(size_t) TSettStrBase::Unpack (const char* src, int srcLen, const char* spl)
{
   OnBfUnpack();

   if (!src || srcLen<=0)
      return 0;

   char splStr[20];
   int  splLen;

   if (spl)  splLen = sprintf(splStr, "%s", spl);
   else      splLen = sprintf(splStr, "%s", DefSpl);

   const char* mem;
   const char* srcCur    = src;
   const char* srcSkip   = src;
   const char* srcEnd    = (src + srcLen);
   int         splStrBeg = (int)splStr[0];
   size_t      size      = 0;

   while (srcSkip < srcEnd) {
      mem = (const char*)(memchr(srcSkip, splStrBeg, srcEnd-srcSkip));
      if (!mem)
         break;

      // 檢查剩下的長度是否大於 splLen
      if (srcEnd - mem < splLen)
         break;

      int i = 1;
      for (; i<splLen; ++i) {
         if (mem[i] != splStr[i])
            break;
      }

      if (i == splLen) {
         // 找到完全吻合的
         TValue val(srcCur, mem-srcCur);

         OnUnpackOne(val);
         srcCur  = (mem + splLen);
         srcSkip = srcCur;

         ++size;
      }
      else {
         // 資料不吻合, 往下移動一次囉
         srcSkip = (mem + 1);
      }
   }

   //==============================================
   // 將最後一筆資料填入
   TValue val(srcCur, srcEnd-srcCur);

   OnUnpackOne(val);
   ++size;

   return size;
}


/////////////////////////////////////////////////////////////////////////////
// class TSettStrSp impl
/////////////////////////////////////////////////////////////////////////////
K_mf(size_t) TSettStrSp::Pack (char* buf, size_t bufSize, const char* spl)
{
   if (Values_.empty())
      return 0;

   TLineBuf line(buf, bufSize);
   char     splStr[50];
   int      splLen;

   if (spl)  splLen = sprintf(splStr, "%s", spl);
   else      splLen = sprintf(splStr, "%s", TSettStrBase::DefSpl);

   for (TValuesI iter=Values_.begin(); iter!=Values_.end(); ++iter) {
      line.Append(*iter);
      line.Append(splStr, splLen);
   }

   //==============================================
   // 去掉最後一個 splStr
   line.EraseBack(splLen);

   return line.GetUsedSize();
}
//---------------------------------------------------------------------------

K_mf(void) TSettStrSp::SetVal (size_t idx, const std::string& str)
{
   if (idx >= Values_.size())
      Values_.resize(idx+1);

   Values_[idx] = str;
}
//---------------------------------------------------------------------------

K_mf(void) TSettStrSp::SetVal (size_t idx, int val)
{
   char buf[20];
   
   sprintf(buf, "%d", val);
   SetVal(idx, std::string(buf));
}
//---------------------------------------------------------------------------

K_mf(bool) TSettStrSp::GetStr (size_t idx, const char*& ptr) const
{
   if (idx >= Values_.size())
      return false;

   ptr = Values_[idx].c_str();
   
   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TSettStrSp::GetInt (size_t idx, int& val) const
{
   const char* ptr;

   if (!GetStr(idx, ptr))
      return false;

   val = atoi(ptr);
   return true;
}
//---------------------------------------------------------------------------
