/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_Time.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>//for isdigit()
#ifdef _MSC_VER
#  include <Windows.h>
#  pragma warning (disable : 4244)
#elif defined(K_WINDOWS32)
#  include <dos.h>// for gettime()
#  define HAVE_GETTIME
#else
#  include <sys/time.h>//for gettimeofday(),settimeofday(),
#endif
#include "char_ary.h"
#include "fixed_num.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kstl;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TTimeStamp impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TTimeStamp::assign (const char* str)
{
   Time_  .Clear();
   TimeMS_.SetMS(0);

   const char* ptr = Time_.assign(str);
   if (ptr && *ptr != '\0' && Time_.IsInvalid() == false) {
      if (*ptr == '.')
         ++ptr;
         
      TimeMS_.assign(ptr);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TTimeStamp::assign_str_hhmmssms_9 (const char* str)
{
   using namespace Kstl;

   Time_  .Clear();
   TimeMS_.SetMS(0);

   if (!str || strlen(str) != 9)
      return;

   // 084500080 ==> 08:45:00.080
   struct STimeStr
   {
      char_ary<char, 2, '0'>  Hour_;
      char_ary<char, 2, '0'>  Min_;
      char_ary<char, 2, '0'>  Sec_;
      char_ary<char, 3, '0'>  Ms_;
   } timeStr;

   memcpy(&timeStr, str, sizeof(timeStr));

   Time_  .Encode((byte)timeStr.Hour_.as_int(), (byte)timeStr.Min_.as_int(), (byte)timeStr.Sec_.as_int());
   TimeMS_.SetMS ((word)timeStr.Ms_.as_int());
}
//---------------------------------------------------------------------------

K_mf(void) TTimeStamp::assign_hhmmssms_8 (int val)
{
   int hh = ( val / 1000000);
   int mm = ((val / 10000) % 100);
   int ss = ((val / 100)   % 100);
   int ms = ((val % 100)   * 10);

   Time_  .Encode((byte)hh, (byte)mm, (byte)ss);
   TimeMS_.SetMS ((word)ms);
}
//---------------------------------------------------------------------------

K_mf(int) TTimeStamp::as_int_hhmmssms_8 () const
{
   if (Time_.IsInvalid())
      return 0;

   // 09:44:16.910 ==> 09441691
   int hh = Time_  .GetHour();
   int mm = Time_  .GetMin ();
   int ss = Time_  .GetSec ();
   int ms = TimeMS_.GetMS  ();

   int val = (hh * 1000000) +
             (mm * 10000)   +
             (ss * 100)     +
             (ms / 10);

   return val;
}
//---------------------------------------------------------------------------

K_mf(void) TTimeStamp::assign_hhmmssms_9 (int val)
{
   int hh = ( val / 10000000);
   int mm = ((val / 100000) % 100);
   int ss = ((val / 1000)   % 100);
   int ms = ( val % 1000);

   Time_  .Encode((byte)hh, (byte)mm, (byte)ss);
   TimeMS_.SetMS ((word)ms);
}
//---------------------------------------------------------------------------

K_mf(void) TTimeStamp::assign_hhmmss (int val)
{
   int hh = ( val / 10000);
   int mm = ((val / 100) % 100);
   int ss =  (val % 100);

   Time_  .Encode((byte)hh, (byte)mm, (byte)ss);
   TimeMS_.SetMS (0);
}
//---------------------------------------------------------------------------

K_mf(std::string) TTimeStamp::as_string (bool timeLongStr, int msLen) const
{
   std::string result = Time_.as_string(timeLongStr);
      
   if (Time_.IsInvalid()) {
      return result;
   }
      
   char buf[128];
   sprintf(buf, "%s.%s", result.c_str(), TimeMS_.as_string(msLen).c_str());
   return std::string(buf);
}
//------------------------------------------------------------------------

K_mf(TTimeStamp) TTimeStamp::Now ()
{
   #if defined(K_WINDOWS)
      SYSTEMTIME    tm;
      GetLocalTime(&tm);
      return TTimeStamp(tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds);
   #else
      time_t          t;  time(&t);
      struct tm*      s = localtime(&t);
      struct timeval  tp;
      struct timezone tzp;
      gettimeofday(&tp, &tzp);
      return TTimeStamp(static_cast<byte>(s->tm_hour),
                        static_cast<byte>(s->tm_min),
                        static_cast<byte>(s->tm_sec),
                        static_cast<word>(tp.tv_usec / 1000));
   #endif
}
//------------------------------------------------------------------------

K_mf(long) TTimeStamp::operator - (const TTimeStamp& a) const
{
   long lhs = (long)(  Time_.GetSecOfDay() * 1000 +   TimeMS_.GetMS());
   long rhs = (long)(a.Time_.GetSecOfDay() * 1000 + a.TimeMS_.GetMS());

   return (lhs - rhs);
}


/////////////////////////////////////////////////////////////////////////////
// class TTimeUs impl
/////////////////////////////////////////////////////////////////////////////
static const int  USecVal = 1000000;
//---------------------------------------------------------------------------
K_mf(void) TTimeUs::assign (const char* str)
{
   Time_ .Clear();
   USec_ = 0;

   const char* ptr = Time_.assign(str);
   if (ptr && *ptr != '\0' && Time_.IsInvalid() == false) {
      if (*ptr == '.')
         ++ptr;
         
      USec_ = atoi(ptr);
   }
}
//---------------------------------------------------------------------------

K_mf(std::string) TTimeUs::as_string (bool timeLongStr, int usLen) const
{
   std::string result = Time_.as_string(timeLongStr);
      
   if (Time_.IsInvalid()) {
      return result;
   }
      
   int  usIdx;
   char fmt[20], buf[128];
   int  usec = USec_;
   
   if (usLen <= 0 || usLen >= 6) {
      usLen = 6;
      usIdx = 0;
   }
   else {
      usIdx = 6 - usLen;
   }

   sprintf(fmt, "%%s.%%0%dd", usLen);
   //#if defined(__NSC__)
   usec /= priv::DecimalDivied[usIdx];
   //#else
   //usec /= fixed_num_private::DecimalDivied[usIdx];
   //#endif
   sprintf(buf, fmt, result.c_str(), usec);

   return std::string(buf);
}
//------------------------------------------------------------------------

K_mf(TTimeUs) TTimeUs::Now ()
{
   #if defined(K_WINDOWS)
      SYSTEMTIME    tm;
      GetLocalTime(&tm);
      return TTimeUs(tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds * 1000);
   #else
      time_t          t;  time(&t);
      struct tm*      s = localtime(&t);
      struct timeval  tp;
      struct timezone tzp;
      gettimeofday(&tp, &tzp);
      return TTimeUs(static_cast<byte>(s->tm_hour),
                     static_cast<byte>(s->tm_min),
                     static_cast<byte>(s->tm_sec),
                     static_cast<int> (tp.tv_usec));
   #endif
}
//------------------------------------------------------------------------

K_mf(long) TTimeUs::operator - (const this_type& a) const
{
   long lhs = (long)(  Time_.GetSecOfDay() * USecVal +   USec_);
   long rhs = (long)(a.Time_.GetSecOfDay() * USecVal + a.USec_);

   return (lhs - rhs);
}
//------------------------------------------------------------------------

K_mf(void) TTimeUs::IncUs (int usec)
{
   div_t div_val;

   USec_   += usec;
   div_val  = div(USec_, USecVal);

   Time_.IncSec(div_val.quot);
   USec_ = div_val.rem;
}


/////////////////////////////////////////////////////////////////////////////
// class TPerfTime impl
/////////////////////////////////////////////////////////////////////////////
/*
   LARGE_INTEGER liFreq, liBf, liAf;

   QueryPerformanceFrequency(&liFreq);
   QueryPerformanceCounter  (&liBf);

   Sleep(100);

   QueryPerformanceCounter  (&liAf);

   long decodeDulation = ((liAf.QuadPart - liBf.QuadPart) * 1000) / liFreq.QuadPart;
*/
/*
K_mf(void) TPerfTime::Start ()
{
   //**********************************************
   #if defined(K_WINDOWS)
   LARGE_INTEGER li;
   
   if (QueryPerformanceCounter(&li))
      TimeCur_ = li.QuadPart;
   else
      TimeCur_ = 0;

   #else //----------------------------------------

   TimeCur_ = gethrtime();
   if (TimeCur_ == -1) {
      // error
      TimeCur_ = 0;
   }
   #endif
   //**********************************************
}
//------------------------------------------------------------------------

K_mf(void) TPerfTime::Stop  ()
{
   if (TimeCur_ == 0)
      return;

   Kway::int64 timeNow = 0;

   //**********************************************
   #if defined(K_WINDOWS)
   LARGE_INTEGER li;

   if (QueryPerformanceCounter(&li))
      timeNow = li.QuadPart;

   #else //----------------------------------------

   timeNow = gethrtime();
   if (timeNow == -1) {
      // error
      timeNow = 0;
   }
   #endif
   //**********************************************

   if (timeNow == 0)
      return;

   //==============================================
   // ¶}©l­pºâ
   TimeGap_ = timeNow - TimeCur_;

   if (TimeGap_ > TimeMax_)  TimeMax_ = TimeGap_;
   if (TimeGap_ < TimeMin_)  TimeMin_ = TimeGap_;

   TimeTol_ += TimeGap_;
   ++CntTol_;

   TimeCur_ = 0;
}
//------------------------------------------------------------------------

K_mf(std::string) TPerfTime::ShowState ()
{
   char buf[128];
   int  len = sprintf(buf, "Gap[%s], Max[%s], Min[%s], Avg[%d]",
                              TInt64(TimeGap_).as_string().c_str(),
                              TInt64(TimeMax_).as_string().c_str(),
                              TInt64(TimeMin_).as_string().c_str(),
                              (CntTol_ != 0 ? (int)(TimeTol_/CntTol_) : 0));

   return std::string(buf, len);
}
*/
//------------------------------------------------------------------------

