/****************************************************************************
** Copyright (c) quickfixengine.org  All rights reserved.
**
** This file is part of the QuickFIX FIX Engine
**
** This file may be distributed under the terms of the quickfixengine.org
** license as defined by quickfixengine.org and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.quickfixengine.org/LICENSE for licensing information.
**
** Contact ask@quickfixengine.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#ifdef _MSC_VER
#include "stdafx.h"
#else
// cooper {
//#include "config.h"
#include "KConfig.h"
// } cooper
#endif
#include "CallStack.h"

#include "FieldTypes.h"
#include "UtilityQ.h"

#ifdef HAVE_FTIME
# include <sys/timeb.h>
#endif

namespace Kway
{
namespace FIX
{

DateTime
DateTime::now ()
{
#if defined (HAVE_FTIME)
    timeb tb;
    ftime (&tb);
    return fromTimeT (tb.time, tb.millitm);
#elif defined (_POSIX_SOURCE)
    struct timeval tv;
    gettimeofday (&tv, 0);
    return fromTimeT (tv.tv_sec, tv.tv_usec / 1000);
#else
    return fromTimeT (::time (0), 0);
#endif
}
//---------------------------------------------------------------------------
// cooper {
DateTime DateTime::nowLocal ()
{
   int year, month,  day;
   int hour, minute, second, millis;

   #if defined(K_WINDOWS)
      SYSTEMTIME tm;

      GetLocalTime(&tm);
      year   = tm.wYear;
      month  = tm.wMonth;
      day    = tm.wDay;
      hour   = tm.wHour;
      minute = tm.wMinute;
      second = tm.wSecond;
      millis = tm.wMilliseconds;

   #else

      struct timeval  tp;
      struct timezone tzp;

      gettimeofday(&tp, &tzp);

      time_t     t = tp.tv_sec;
      struct tm* s = localtime(&t);

      year   = (s->tm_year + 1900);
      month  = (s->tm_mon  + 1);
      day    =  s->tm_mday;
      hour   =  s->tm_hour;
      minute =  s->tm_min;
      second =  s->tm_sec;
      millis = (tp.tv_usec / 1000);
   #endif

   return DateTime(julianDate(year, month, day),
                   makeHMS   (hour, minute, second, millis));
}
// } cooper
//---------------------------------------------------------------------------

DateTime DateTime::fromTimeT( time_t t, int millis )
{
  struct tm tm = time_gmtime( &t );
  return fromTm( tm, millis );
}

} // namespace FIX
} // namespace Kway

