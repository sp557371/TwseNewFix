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

#ifndef FIX_MUTEX_H
#define FIX_MUTEX_H

// cooper
//#include "UtilityQ.h"
#include <pthread.h>
namespace Kway
{
namespace FIX
{
/// Portable implementation of a mutex.
class Mutex
{
public:
  Mutex()
  {
// cooper
//#ifdef _MSC_VER
#if defined(_MSC_VER) || defined(__BORLANDC__)
    InitializeCriticalSection( &m_mutex );
#else
    m_count = 0;
    m_threadID = 0;
    //pthread_mutexattr_t attr;
    //pthread_mutexattr_init(&attr);
    //pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    //pthread_mutex_init(&m_mutex, &attr);
    pthread_mutex_init( &m_mutex, 0 );
#endif
  }

  ~Mutex()
  {
// cooper
//#ifdef _MSC_VER
#if defined(_MSC_VER) || defined(__BORLANDC__)
    DeleteCriticalSection( &m_mutex );
#else
    pthread_mutex_destroy( &m_mutex );
#endif
  }

  void lock()
  {
// cooper
//#ifdef _MSC_VER
#if defined(_MSC_VER) || defined(__BORLANDC__)
    EnterCriticalSection( &m_mutex );
#else
    if ( m_count && m_threadID == pthread_self() )
    { ++m_count; return ; }
    pthread_mutex_lock( &m_mutex );
    ++m_count;
    m_threadID = pthread_self();
#endif
  }

  void unlock()
  {
// cooper
//#ifdef _MSC_VER
#if defined(_MSC_VER) || defined(__BORLANDC__)
    LeaveCriticalSection( &m_mutex );
#else
    if ( m_count > 1 )
    { m_count--; return ; }
    --m_count;
    m_threadID = 0;
    pthread_mutex_unlock( &m_mutex );
#endif
  }

private:
// cooper
//#ifdef _MSC_VER
#if defined(_MSC_VER) || defined(__BORLANDC__)
  CRITICAL_SECTION m_mutex;
#else
  pthread_mutex_t m_mutex;
  pthread_t m_threadID;
  int m_count;
#endif
};

/// Locks/Unlocks a mutex using RAII.
class Locker
{
public:
  Locker( Mutex& mutex )
  : m_mutex( mutex )
  {
    m_mutex.lock();
  }

  ~Locker()
  {
    m_mutex.unlock();
  }
private:
  Mutex& m_mutex;
};

/// Does the opposite of the Locker to ensure mutex ends up in a locked state.
class ReverseLocker
{
public:
  ReverseLocker( Mutex& mutex )
  : m_mutex( mutex )
  {
    m_mutex.unlock();
  }

  ~ReverseLocker()
  {
    m_mutex.lock();
  }
private:
  Mutex& m_mutex;
};

} // namespace FIX
} // namespace Kway

#endif