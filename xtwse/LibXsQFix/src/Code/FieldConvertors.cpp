/////////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER)
#include "stdafx.h"
#else
#include "KConfig.h"
#endif
//---------------------------------------------------------------------------
#include "FieldConvertors.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <limits>
#include "UtilityQ.h"

namespace Kway
{
namespace FIX
{
/////////////////////////////////////////////////////////////////////////////
// integer_to_string
/////////////////////////////////////////////////////////////////////////////
template<class T>
inline char* integer_to_string( char* buf, const size_t len, T t )
{
  const bool isNegative = t < 0;
  char* p = buf + len;

  *--p = '\0';

  if( isNegative )
  {
    if( t == (std::numeric_limits<T>::min)() )
    {
      *--p = '0' + (char)((10-t%10)%10);
      t/=10;
    }
    t = -t;
    do
    {
      *--p = '0' + (char)(t % 10);
      t /= 10;
    } while (t > 0);
    *--p = '-';
  }
  else
  {
    do
    {
      *--p = '0' + (char)(t % 10);
      t /= 10;
    } while( t > 0 );
  }
  return p;
}
/////////////////////////////////////////////////////////////////////////////
// integer_to_string_padded
/////////////////////////////////////////////////////////////////////////////
template<class T>
inline char* integer_to_string_padded
( char* buf, const size_t len, T t,
  const size_t width = 0,
  const char paddingChar = '0')
{
  if( !width ) return integer_to_string( buf, len, t );

  const bool isNegative = t < 0;
  char* p = buf + len;

  *--p = '\0';

  if( isNegative )
  {
    if( t == (std::numeric_limits<T>::min)() )
    {
      *--p = '0' + (char)(( 10 -t % 10 ) % 10);
      t/=10;
    }
    t=-t;
    do
    {
      *--p = '0' + (char)(t % 10);
      t /= 10;
    } while (t > 0);
    if( p > buf )
      *--p = '-';
  }
  else
  {
    do
    {
      *--p = '0' + (char)(t % 10);
      t /= 10;
    } while( t > 0 );
  }
  const char* stop_p = buf + len - width - 1;
  if( stop_p < buf ) stop_p = buf;
  while( p > stop_p )
    *--p = paddingChar;
  return p;
}
/////////////////////////////////////////////////////////////////////////////
// IntConvertor
/////////////////////////////////////////////////////////////////////////////
std::string IntConvertor::convert( long value )
{
  // buffer is big enough for significant digits and extra digit,
  // minus and null
  #if defined(__BORLANDC__)
  char buffer[50];
  #else
  char buffer[std::numeric_limits<long>::digits10 + 3];
  #endif
  const char* const start
    = integer_to_string( buffer, sizeof (buffer), value );
  return std::string( start, buffer + sizeof (buffer) - start - 1 );
}
//---------------------------------------------------------------------------
bool IntConvertor::convert( const std::string& value, long& result )
{
  const char* str = value.c_str();
  bool isNegative = false;
  long x = 0;

  if( *str == '-' )
  {
    isNegative = true;
    ++str;
  }

  do
  {
    const int c = *str - '0';
    if( c < 0 || 9 < c ) return false;
    x = 10 * x + c;
  } while (*++str);

  if( isNegative )
    x = -x;

  result = x;
  return true;
}
/////////////////////////////////////////////////////////////////////////////
// CheckSumConvertor
/////////////////////////////////////////////////////////////////////////////
std::string CheckSumConvertor::convert( long value )
throw( FieldConvertError )
{
  if ( value > 255 || value < 0 ) throw FieldConvertError();
  char result[4];
  if( integer_to_string_padded(result, sizeof(result), value, 3, '0') != result )
  {
    throw FieldConvertError();
  }
  return std::string( result, 3 );
}
/////////////////////////////////////////////////////////////////////////////
// DoubleConvertor
/////////////////////////////////////////////////////////////////////////////
std::string DoubleConvertor::convert( double value, int padding )
{
   char result[32];
	char *end = 0;

	int size;
	if( value == 0 || value > 0.0001 || value <= -0.0001 )
	{
		size = STRING_SPRINTF( result, "%.15g", value );

		if( padding > 0 )
		{
			char* point = result;
			end = result + size - 1;
			while( *point != '.' && *point != 0 )
				point++;

			if( *point == 0 )
			{
				end = point;
				*point = '.';
				size++;
			}
			int needed = padding - (int)(end - point);

			while( needed-- > 0 )
			{
				*(++end) = '0';
				size++;
			}
			*(end+1) = 0;
		}
	}
	else
	{
		size = STRING_SPRINTF( result, "%.15f", value );
		// strip trailing 0's
		end = result + size - 1;

		if( padding > 0 )
		{
			int discard = 15 - padding;

			while( (*end == '0') && (discard-- > 0) )
			{
				*(end--) = 0;
				size--;
			}
		}
		else
		{
			while( *end == '0' )
			{
				*(end--) = 0;
				size--;
			}
		}
	}

	return std::string( result, size );
}
/////////////////////////////////////////////////////////////////////////////
// class UtcTimeStampConvertor
/////////////////////////////////////////////////////////////////////////////
std::string UtcTimeStampConvertor::convert( const UtcTimeStamp& value, bool showMilliseconds )
throw( FieldConvertError )
{
  char result[ 18+4 ];
  int year, month, day, hour, minute, second, millis;

  value.getYMD( year, month, day );
  value.getHMS( hour, minute, second, millis );

  integer_to_string_padded( result, 5, year, 4, '0' );
  integer_to_string_padded( result + 4, 3, month, 2, '0' );
  integer_to_string_padded( result + 6, 3, day, 2, '0' );
  result[8]  = '-';
  integer_to_string_padded( result + 9, 3, hour, 2, '0' );
  result[11] = ':';
  integer_to_string_padded( result + 12, 3, minute, 2, '0' );
  result[14] = ':';
  integer_to_string_padded( result + 15, 3, second, 2, '0' );

  if( showMilliseconds )
  {
    result[17] = '.';
    if( integer_to_string_padded ( result + 18, 4, millis, 3, '0' )
        != result + 18 )
    {
      throw FieldConvertError();
    }
  }

  //cooper
  //return result;
  return std::string(result);
}
/////////////////////////////////////////////////////////////////////////////
// UtcTimeOnlyConvertor
/////////////////////////////////////////////////////////////////////////////
std::string UtcTimeOnlyConvertor::convert( const UtcTimeOnly& value, bool showMilliseconds )
throw( FieldConvertError )
{
  char result[ 9+4 ];
  int hour, minute, second, millis;

  value.getHMS( hour, minute, second, millis );

  integer_to_string_padded ( result, 3, hour, 2, '0' );
  result[2] = ':';
  integer_to_string_padded ( result + 3, 3, minute,  2, '0' );
  result[5] = ':';
  integer_to_string_padded ( result + 6, 3, second,  2, '0' );

  if( showMilliseconds )
  {
    result[8] = '.';
    if( integer_to_string_padded ( result + 9, 4, millis, 3, '0' )
        != result + 9 )
        throw FieldConvertError();
  }

  //cooper
  //return result;
  return std::string(result);
}
/////////////////////////////////////////////////////////////////////////////
// UtcDateConvertor
/////////////////////////////////////////////////////////////////////////////
std::string UtcDateConvertor::convert( const UtcDate& value )
throw( FieldConvertError )
{
  char result[ 9 ];
  int year, month, day;

  value.getYMD( year, month, day );

  integer_to_string_padded( result, 5, year, 4, '0' );
  integer_to_string_padded( result + 4, 3, month, 2, '0' );
  integer_to_string_padded( result + 6, 3, day, 2, '0' );
  //cooper
  //return result;
  return std::string(result);
}

} // namespace FIX
} // namespace Kway
