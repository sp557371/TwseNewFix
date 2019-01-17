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

#ifndef FIX_SESSIONID_H
#define FIX_SESSIONID_H

#include "Fields.h"
#include <stdio.h> // sprintf

namespace Kway
{
namespace FIX
{
/// Unique session id consists of BeginString, SenderCompID and TargetCompID.
class SessionID
{
public:
  SessionID() {}

  SessionID( const BeginString& beginString,
             const SenderCompID& senderCompID,
             const TargetCompID& targetCompID,
             const std::string& sessionQualifier = "" )
  : m_beginString( beginString ),
    m_senderCompID( senderCompID ),
    m_targetCompID( targetCompID ),
    m_sessionQualifier( sessionQualifier ) {}

  SessionID( const std::string& beginString,
             const std::string& senderCompID,
             const std::string& targetCompID,
             const std::string& sessionQualifier = "" )
  : m_beginString( BeginString(beginString) ),
    m_senderCompID( SenderCompID(senderCompID) ),
    m_targetCompID( TargetCompID(targetCompID) ),
    m_sessionQualifier( sessionQualifier ) {}

  const BeginString& getBeginString() const
    { return m_beginString; }
  const SenderCompID& getSenderCompID() const
    { return m_senderCompID; }
  const TargetCompID& getTargetCompID() const
    { return m_targetCompID; }
  const std::string& getSessionQualifier() const
    { return m_sessionQualifier; }

  /// Get a string representation of the SessionID
  std::string toString() const
  {
      std::string str;
      return toString( str );
  }

  /// Build from string representation of SessionID
  void fromString( const std::string& str )
  {
    std::string::size_type first =
      str.find_first_of(':');
    std::string::size_type second =
      str.find("->");
    std::string::size_type third =
      str.find_last_of(':');
    if( first == std::string::npos )
      return;
    if( second == std::string::npos )
      return;
    m_beginString = str.substr(0, first);
    m_senderCompID = str.substr(first+1, second - first - 1);
    if( first == third )
    {
      m_targetCompID = str.substr(second+2);
      m_sessionQualifier = "";
    }
    else
    {
      m_targetCompID = str.substr(second+2, third - second - 2);
      m_sessionQualifier = str.substr(third+1);
    }
  }

  /// Get a string representation without making a copy
  std::string& toString( std::string& str ) const
  {
    str = getBeginString().getValue() + ":" +
          getSenderCompID().getValue() + "->" +
          getTargetCompID().getValue();
    if( m_sessionQualifier.size() )
      str += ":" + m_sessionQualifier;
    return str;
  }

  // cooper { FIX.4.0-JSUN-VTRD-UAT
  std::string toFormat () const
  {
    char buf[256];
    
    sprintf(buf, "%s-%s-%s", m_beginString .getString().c_str(),
                             m_senderCompID.getString().c_str(),
                             m_targetCompID.getString().c_str());
    
    if (m_sessionQualifier.empty() == false) {
      char tmp[100];
      
      sprintf(tmp, "-%s", m_sessionQualifier.c_str());
      strcat(buf, tmp);
    }
    
    return std::string(buf);
  }

  K_mf(void) assign (const char* str)
  {
    std::string strEmpty;
    
    m_beginString     .setValue(strEmpty);
    m_senderCompID    .setValue(strEmpty);
    m_targetCompID    .setValue(strEmpty);
    m_sessionQualifier.assign  (strEmpty);

    if (str == NULL || strlen(str) == 0)
      return;

    char  buf[128];
    char* beg;
    char* end;
    
    strcpy(buf, str);
    beg = buf;
    end = strchr(buf, (int)'-');
    if (end == NULL)  return;
    *end = '\0';
    m_beginString.setValue(std::string(beg));
    
    beg = ++end;
    end = strchr(beg, (int)'-');
    if (end == NULL)  return;
    *end = '\0';
    m_senderCompID.setValue(std::string(beg));
  
    beg = ++end;
    end = strchr(beg, (int)'-');
    if (end == NULL)  return;
    *end = '\0';
    m_targetCompID.setValue(std::string(beg));
  
    beg = ++end;
    m_sessionQualifier.assign(beg);
  }

  inline void assign (const std::string& str)  { assign(str.c_str()); }
  
  inline BeginString&  getBeginString      ()  { return m_beginString;      }
  inline SenderCompID& getSenderCompID     ()  { return m_senderCompID;     }
  inline TargetCompID& getTargetCompID     ()  { return m_targetCompID;     }
  inline std::string&  getSessionQualifier ()  { return m_sessionQualifier; }
  // } cooper

  friend bool operator<( const SessionID&, const SessionID& );
  friend bool operator==( const SessionID&, const SessionID& );
  friend bool operator!=( const SessionID&, const SessionID& );
  friend std::ostream& operator<<( std::ostream&, const SessionID& );
  friend std::ostream& operator>>( std::ostream&, const SessionID& );

  SessionID operator~()
  {
    return SessionID( m_beginString, SenderCompID( m_targetCompID ),
                      TargetCompID( m_senderCompID ), m_sessionQualifier );
  }

private:
  BeginString m_beginString;
  SenderCompID m_senderCompID;
  TargetCompID m_targetCompID;
  std::string m_sessionQualifier;
};
/*! @} */

inline bool operator<( const SessionID& lhs, const SessionID& rhs )
{
  if ( lhs.m_beginString < rhs.m_beginString )
    return true;
  else if ( rhs.m_beginString < lhs.m_beginString )
    return false;
  else if ( lhs.m_senderCompID < rhs.m_senderCompID )
    return true;
  else if ( rhs.m_senderCompID < lhs.m_senderCompID )
    return false;
  else if ( lhs.m_targetCompID < rhs.m_targetCompID )
    return true;
  else if ( rhs.m_targetCompID < lhs.m_targetCompID )
    return false;
  else if ( lhs.m_sessionQualifier < rhs.m_sessionQualifier )
    return true;
  else if ( rhs.m_sessionQualifier < lhs.m_sessionQualifier )
    return false;
  else
    return false;
}
inline bool operator==( const SessionID& lhs, const SessionID& rhs )
{
  return ( ( lhs.m_beginString == rhs.m_beginString ) &&
           ( lhs.m_senderCompID == rhs.m_senderCompID ) &&
           ( lhs.m_targetCompID == rhs.m_targetCompID ) &&
           ( lhs.m_sessionQualifier == rhs.m_sessionQualifier ));
}
inline bool operator!=( const SessionID& lhs, const SessionID& rhs )
{
  return !( lhs == rhs );
}

// cooper { 應該沒用到吧
//inline std::ostream& operator<<
//( std::ostream& stream, const SessionID& sessionID )
//{
//  std::string str;
//  stream << sessionID.toString( str );
//  return stream;
//}
//
//inline std::istream& operator>>
//( std::istream& stream, SessionID& sessionID )
//{
//  std::string str;
//  stream >> str;
//  sessionID.fromString( str );
//  return stream;
//}
// } cooper

} // namespace FIX
} // namespace Kway

#endif //FIX_SESSIONID_H
