/* -*- C++ -*- */

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

#ifndef FIX_FIELDCONVERTORS_H
#define FIX_FIELDCONVERTORS_H

#include "FieldTypes.h"
#include "Exceptions.h"
#include <stdlib.h>
namespace Kway
{
namespace FIX
{
/// Empty convertor is a no-op.
struct EmptyConvertor
{
  static const std::string& convert( const std::string& value )
  { return value; }
};

typedef EmptyConvertor StringConvertor;

/// Converts integer to/from a string
K_class IntConvertor
{
public:
  static std::string convert( long value );

  static bool convert( const std::string& value, long& result );

  static long convert( const std::string& value )
  throw( FieldConvertError )
  {
    long result = 0;
    if( !convert( value, result ) )
      throw FieldConvertError();
    else
      return result;
  }
};

/// Converts checksum to/from a string
struct CheckSumConvertor
{
  static std::string convert( long value )
  throw( FieldConvertError );

  static bool convert( const std::string& value, long& result )
  {
    return IntConvertor::convert( value, result );
  }

  static long convert( const std::string& value )
  throw( FieldConvertError )
  {
    return IntConvertor::convert( value );
  }
};

/// Converts double to/from a string
K_class DoubleConvertor
{
public:
	static std::string convert( double value, int padding = 0 );

	static bool convert( const std::string& value, double& result )
	{
		const char * i = value.c_str();

		// Catch null strings
		if( !*i ) return false;
		// Eat leading '-' and recheck for null string
		if( *i == '-' && !*++i ) return false;

		bool haveDigit = false;

		if( isdigit(*i) )
		{
			haveDigit = true;
			while( isdigit (*++i) );
		}

		if( *i == '.' && isdigit(*++i) )
		{
			haveDigit = true;
			while( isdigit (*++i) );
		}

		if( *i || !haveDigit ) return false;
		result = strtod( value.c_str(), 0 );
		return true;
	}

	static double convert( const std::string& value )
		throw( FieldConvertError )
	{
		double result = 0.0;
		if( !convert( value, result ) )
			throw FieldConvertError();
		else
			return result;
	}
};

/// Converts character to/from a string
struct CharConvertor
{
  static std::string convert( char value )
  {
    if( value == '\0' ) return "";
    return std::string( 1, value );
  }

  static bool convert( const std::string& value, char& result )
  {
    if( value.size() != 1 ) return false;
    result = value[0];
    return true;
  }

  static char convert( const std::string& value )
  throw( FieldConvertError )
  {
    char result = '\0';
    if( !convert( value, result ) )
      throw FieldConvertError();
    else
      return result;
  }
};

/// Converts boolean to/from a string
struct BoolConvertor
{
  static std::string convert( bool value )
  {
    const char ch = value ? 'Y' : 'N';
    return std::string( 1, ch );
  }

  static bool convert( const std::string& value, bool& result )
  {
    if( value.size() != 1 ) return false;
    switch( value[0] )
    {
      case 'Y': result = true; break;
      case 'N': result = false; break;
      default: return false;
    }

    return true;
  }

  static bool convert( const std::string& value )
  throw( FieldConvertError )
  {
    bool result = false;
    if( !convert( value, result ) )
      throw FieldConvertError();
    else
      return result;
  }
};

/// Converts a UtcTimeStamp to/from a string
K_class UtcTimeStampConvertor
{
public:
  static std::string convert( const UtcTimeStamp& value,
                              bool showMilliseconds = false )
  throw( FieldConvertError );

  static UtcTimeStamp convert( const std::string& value,
                               bool calculateDays = false )
  throw( FieldConvertError )
  {
    bool haveMilliseconds = false;

    switch( value.size() )
    {
      case 21: haveMilliseconds = true;
      case 17: break;
      default: throw FieldConvertError();
    }

    int i = 0;
    int c = 0;
    for( c = 0; c < 8; ++c )
      if( !isdigit(value[i++]) ) throw FieldConvertError();
    if (value[i++] != '-') throw FieldConvertError();
    for( c = 0; c < 2; ++c )
      if( !isdigit(value[i++]) ) throw FieldConvertError();
    if( value[i++] != ':' ) throw FieldConvertError();
    for( c = 0; c < 2; ++c )
      if( !isdigit(value[i++]) ) throw FieldConvertError();
    if( value[i++] != ':' ) throw FieldConvertError();
    for( c = 0; c < 2; ++c )
      if( !isdigit(value[i++]) ) throw FieldConvertError();

    if( haveMilliseconds )
    {
      if( value[i++] != '.' ) throw FieldConvertError();
      for( c = 0; c < 3; ++c )
	      if( !isdigit(value[i++]) ) throw FieldConvertError();
    }

    int year, mon, mday, hour, min, sec, millis;

    i = 0;

    year = value[i++] - '0';
    year = 10 * year + value[i++] - '0';
    year = 10 * year + value[i++] - '0';
    year = 10 * year + value[i++] - '0';

    mon = value[i++] - '0';
    mon = 10 * mon + value[i++] - '0';
    if( mon < 1 || 12 < mon ) throw FieldConvertError();

    mday = value[i++] - '0';
    mday = 10 * mday + value[i++] - '0';
    if( mday < 1 || 31 < mday ) throw FieldConvertError();

    ++i; // skip '-'

    hour = value[i++] - '0';
    hour = 10 * hour + value[i++] - '0';
    // No check for >= 0 as no '-' are converted here
    if( 23 < hour ) throw FieldConvertError();

    ++i; // skip ':'

    min = value[i++] - '0';
    min = 10 * min + value[i++] - '0';
    // No check for >= 0 as no '-' are converted here
    if( 59 < min ) throw FieldConvertError();

    ++i; // skip ':'

    sec = value[i++] - '0';
    sec = 10 * sec + value[i++] - '0';

    // No check for >= 0 as no '-' are converted here
    if( 60 < sec ) throw FieldConvertError();

    if( haveMilliseconds )
    {
      millis = (100 * (value[i+1] - '0')
                + 10 * (value[i+2] - '0')
                + (value[i+3] - '0'));
    }
    else
      millis = 0;

    return UtcTimeStamp (hour, min, sec, millis,
                         mday, mon, year);
  }
};

/// Converts a UtcTimeOnly to/from a string
struct UtcTimeOnlyConvertor
{
  static std::string convert( const UtcTimeOnly& value,
                              bool showMilliseconds = false)
  throw( FieldConvertError );

  static UtcTimeOnly convert( const std::string& value )
  throw( FieldConvertError )
  {
    bool haveMilliseconds = false;

    switch( value.size() )
    {
      case 12: haveMilliseconds = true;
      case 8: break;
      default: throw FieldConvertError();
    }

    int i = 0;
    int c = 0;
    for( c = 0; c < 2; ++c )
      if( !isdigit(value[i++]) ) throw FieldConvertError();
    if( value[i++] != ':' ) throw FieldConvertError();
    for( c = 0; c < 2; ++c )
      if( !isdigit(value[i++]) ) throw FieldConvertError();
    if( value[i++] != ':' ) throw FieldConvertError();
    for( c = 0; c < 2; ++c )
      if( !isdigit(value[i++]) ) throw FieldConvertError();

    if( haveMilliseconds )
    {
      // ++i instead of i++ skips the '.' separator
      for( c = 0; c < 3; ++c )
	      if( !isdigit(value[++i]) ) throw FieldConvertError();
    }

    int hour, min, sec, millis;
 
    i = 0;

    hour = value[i++] - '0';
    hour = 10 * hour + value[i++] - '0';
    // No check for >= 0 as no '-' are converted here
    if( 23 < hour ) throw FieldConvertError();
    ++i; // skip ':'

    min = value[i++] - '0';
    min = 10 * min + value[i++] - '0';
    // No check for >= 0 as no '-' are converted here
    if( 59 < min ) throw FieldConvertError();
    ++i; // skip ':'

    sec = value[i++] - '0';
    sec = 10 * sec + value[i++] - '0';
    // No check for >= 0 as no '-' are converted here
    if( 60 < sec ) throw FieldConvertError();

    if( haveMilliseconds )
    {
      millis = (100 * (value[i+1] - '0')
                + 10 * (value[i+2] - '0')
                + (value[i+3] - '0'));
    }
    else
      millis = 0;

    return UtcTimeOnly( hour, min, sec, millis );
  }
};

/// Converts a UtcDate to/from a string
struct UtcDateConvertor
{
  static std::string convert( const UtcDate& value )
  throw( FieldConvertError );

  static UtcDate convert( const std::string& value )
  throw( FieldConvertError )
  {
    if( value.size() != 8 ) throw FieldConvertError();

    int i = 0;
    for( int c=0; c<8; ++c )
      if( !isdigit(value[i++]) ) throw FieldConvertError();

    int year, mon, mday;

    i = 0;

    year = value[i++] - '0';
    year = 10 * year + value[i++] - '0';
    year = 10 * year + value[i++] - '0';
    year = 10 * year + value[i++] - '0';

    mon = value[i++] - '0';
    mon = 10 * mon + value[i++] - '0';
    if( mon < 1 || 12 < mon )
      throw FieldConvertError();

    mday = value[i++] - '0';
    mday = 10 * mday + value[i++] - '0';
    if( mday < 1 || 31 < mday )
      throw FieldConvertError();

    return UtcDateOnly( mday, mon, year );
  }
};

typedef UtcDateConvertor UtcDateOnlyConvertor;

typedef StringConvertor STRING_CONVERTOR;
typedef CharConvertor CHAR_CONVERTOR;
typedef DoubleConvertor PRICE_CONVERTOR;
typedef IntConvertor INT_CONVERTOR;
typedef DoubleConvertor AMT_CONVERTOR;
typedef DoubleConvertor QTY_CONVERTOR;
typedef StringConvertor CURRENCY_CONVERTOR;
typedef StringConvertor MULTIPLEVALUESTRING_CONVERTOR;
typedef StringConvertor EXCHANGE_CONVERTOR;
typedef UtcTimeStampConvertor UTCTIMESTAMP_CONVERTOR;
typedef BoolConvertor BOOLEAN_CONVERTOR;
typedef StringConvertor LOCALMKTDATE_CONVERTOR;
typedef StringConvertor DATA_CONVERTOR;
typedef DoubleConvertor FLOAT_CONVERTOR;
typedef DoubleConvertor PRICEOFFSET_CONVERTOR;
typedef StringConvertor MONTHYEAR_CONVERTOR;
typedef StringConvertor DAYOFMONTH_CONVERTOR;
typedef UtcDateConvertor UTCDATE_CONVERTOR;
typedef UtcTimeOnlyConvertor UTCTIMEONLY_CONVERTOR;
typedef IntConvertor NUMINGROUP_CONVERTOR;
typedef DoubleConvertor PERCENTAGE_CONVERTOR;
typedef IntConvertor SEQNUM_CONVERTOR;
typedef IntConvertor LENGTH_CONVERTOR;
typedef StringConvertor COUNTRY_CONVERTOR;
typedef CheckSumConvertor CHECKSUM_CONVERTOR;
} // namespace FIX
} // namespace Kway

#endif //FIX_FIELDCONVERTORS_H
