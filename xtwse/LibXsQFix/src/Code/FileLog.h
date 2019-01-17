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

#ifndef FIX_FILELOG_H
#define FIX_FILELOG_H

#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 4290 )
#endif

#include "Log.h"
#include "SessionSettings.h"
#include <fstream>

namespace Kway
{
namespace FIX
{
/**
 * Creates a file based implementation of Log
 *
 * This stores all log events into flat files
 */
K_class FileLogFactory : public LogFactory
{
public:
  FileLogFactory( const SessionSettings& settings )
: m_settings( settings ), m_globalLog(0), m_globalLogCount(0) {};
  FileLogFactory( const std::string& path )
: m_path( path ), m_globalLog(0), m_globalLogCount(0) {};

public:
  Log* create();
  Log* create( const SessionID& );
  void destroy( Log* log );

private:
  std::string m_path;
  SessionSettings m_settings;
  Log* m_globalLog;
  int m_globalLogCount;
};

/**
 * File based implementation of Log
 *
 * Three files are created by this implementation.  One for outgoing
 * messages, one for incoming message, and one for events.
 *
 * The formats of the files are:<br>
 * &nbsp;&nbsp;
 *   [path]+[BeginString]-[SenderCompID]-[TargetCompID].messages<br>
 * &nbsp;&nbsp;
 *   [path]+[BeginString]-[SenderCompID]-[TargetCompID].event<br>
 *
 */
class FileLog : public Log
{
public:
  FileLog( const std::string& path );
  FileLog( const std::string& path, const SessionID& sessionID );
  virtual ~FileLog();

  void clear();

  // cooper {
  //void onIncoming( const std::string& value )
  //{ m_messages << value << std::endl << std::flush; }
  //void onOutgoing( const std::string& value )
  //{ m_messages << value << std::endl << std::flush; }
  //void onEvent( const std::string& value )
  //{
  //  UtcTimeStamp now;
  //  m_event << UtcTimeStampConvertor::convert( now )
  //  << " : " << value << std::endl << std::flush;
  //}
  void onIncoming( const std::string& value );
  void onOutgoing( const std::string& value );
  void onEvent   ( const std::string& value );
  // } cooper

private:
  void init( std::string path, const std::string& prefix );

  std::ofstream m_messages;
  std::ofstream m_event;
  std::string m_messagesFileName;
  std::string m_eventFileName;
};
} // namespace FIX
} // namespace Kway

#endif //FIX_LOG_H