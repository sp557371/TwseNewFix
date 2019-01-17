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

#ifndef FIX_FIELDMAP
#define FIX_FIELDMAP

#ifdef _MSC_VER
#pragma warning( disable: 4786 )
#endif

#include "Field.h"
#include "MessageSorters.h"
#include "Exceptions.h"
// cooper {
//#include "CallStack.h"
//#include "UtilityQ.h"
#include "ref_counter.h"
// } cooper
#include <map>
#include <vector>
// cooper {
//#include <sstream>
//#include <algorithm>
// } cooper

// cooper {
#ifdef ENABLE_DEBUG_ALLOCATOR
  #include <ext/debug_allocator.h>
  #define ALLOCATOR __gnu_cxx::debug_allocator
#elif ENABLE_NEW_ALLOCATOR
  #include <ext/new_allocator.h>
  #define ALLOCATOR __gnu_cxx::new_allocator
#elif HAVE_BOOST_FAST_POOL_ALLOCATOR
  #include <boost/pool/pool_alloc.hpp>
  #define ALLOCATOR boost::fast_pool_allocator
#elif HAVE_MT_ALLOCATOR
  #include <ext/mt_allocator.h>
  #define ALLOCATOR __gnu_cxx::__mt_alloc
#elif HAVE_BOOST_POOL_ALLOCATOR
  #include <boost/pool/pool_alloc.hpp>
  #define ALLOCATOR boost::pool_allocator
#elif HAVE_POOL_ALLOCATOR
  #include <ext/pool_allocator.h>
  #define ALLOCATOR __gnu_cxx::__pool_alloc
#elif HAVE_BITMAP_ALLOCATOR
  #include <ext/bitmap_allocator.h>
  #define ALLOCATOR __gnu_cxx::bitmap_allocator
#else
  #define ALLOCATOR std::allocator
#endif
// } cooper

namespace Kway
{
namespace FIX
{
/**
 * Stores and organizes a collection of Fields.
 *
 * This is the basis for a message, header, and trailer.  This collection
 * class uses a sorter to keep the fields in a particular order.
 */
K_class FieldMap : public Kstl::ref_counter<>
{
public:
#if defined(_MSC_VER) && _MSC_VER < 1300
  typedef std::multimap < int, FieldBase, message_order > Fields;
  typedef std::map < int, std::vector < FieldMap* >, std::less<int> > Groups;
#else
  typedef std::multimap < int, FieldBase, message_order, 
                          ALLOCATOR<std::pair<const int,FieldBase> > > Fields;
  typedef std::map < int, std::vector < FieldMap* >, std::less<int>, 
                     ALLOCATOR<std::pair<const int, std::vector< FieldMap* > > > > Groups;
#endif

  typedef Fields::const_iterator iterator;
  typedef iterator const_iterator;
  typedef Groups::const_iterator g_iterator;
  typedef Groups::const_iterator g_const_iterator;

  FieldMap( const message_order& order =
            message_order( message_order::normal ) )
  : m_fields( order ) {}

  FieldMap( const int order[] )
  : m_fields( message_order(order) ) {}

  FieldMap( const FieldMap& copy )
  { *this = copy; }

  // cooper {
  //virtual ~FieldMap();
  K_dtor ~FieldMap();
  // } cooper

  FieldMap& operator=( const FieldMap& rhs );

  /// Set a field without type checking
  void setField( const FieldBase& field, bool overwrite = true )
  throw( RepeatedTag )
  {
    Fields::iterator i = m_fields.find( field.getField() );
    if( i == m_fields.end() )
      m_fields.insert( Fields::value_type( field.getField(), field ) );
    else
    {
      if( overwrite )
        i->second = field;
      else
        m_fields.insert( Fields::value_type( field.getField(), field ) );
    }
  }
  /// Set a field without a field class
  void setField( int field, const std::string& value )
  throw( RepeatedTag, NoTagValue )
  {
    FieldBase fieldBase( field, value );
    setField( fieldBase );
  }

  /// Get a field without type checking
  FieldBase& getField( FieldBase& field )
  const throw( FieldNotFound )
  {
    Fields::const_iterator iter = m_fields.find( field.getField() );
    if ( iter == m_fields.end() )
      throw FieldNotFound( field.getField() );
    field = iter->second;
    return field;
  }

  // cooper {
  bool findField( FieldBase& field ) const
  {
    Fields::const_iterator iter = m_fields.find( field.getField() );
    if ( iter == m_fields.end() )
      return false;
    field = iter->second;
    return true;
  }
  // } cooper
  
  /// Get a field without a field class
  const std::string& getField( int field )
  const throw( FieldNotFound )
  {
    return getFieldRef( field ).getString();
  }

  // cooper {
  /// Get a field without a field class
  bool findField( int field, std::string& str ) const
  {
    Fields::const_iterator iter = m_fields.find( field );
    if ( iter == m_fields.end() )
      return false;
    str = iter->second.getString();
    return true;
  }
  // } cooper

  /// Get direct access to a field through a referencce
  const FieldBase& getFieldRef( int field )
  const throw( FieldNotFound )
  {
    Fields::const_iterator iter = m_fields.find( field );
    if ( iter == m_fields.end() )
      throw FieldNotFound( field );
    return iter->second;
  }

  /// Get direct access to a field through a pointer
  const FieldBase* const getFieldPtr( int field )
  const throw( FieldNotFound )
  {
    return &getFieldRef( field );
  }

  /// Check to see if a field is set
  bool isSetField( const FieldBase& field ) const
  { return m_fields.find( field.getField() ) != m_fields.end(); }
  /// Check to see if a field is set by referencing its number
  bool isSetField( int field ) const
  { return m_fields.find( field ) != m_fields.end(); }

  /// Remove a field. If field is not present, this is a no-op.
  void removeField( int field );

  /// Add a group.
  void addGroup( int field, const FieldMap& group, bool setCount = true );

  /// Replace a specific instanct of a group.
  void replaceGroup( int num, int field, FieldMap& group );

  /// Get a specific instance of a group.
  FieldMap& getGroup( int num, int field, FieldMap& group ) const
    throw( FieldNotFound );

  /// Remove a specific instance of a group.
  void removeGroup( int num, int field );
  /// Remove all instances of a group.
  void removeGroup( int field );

  /// Check to see any instance of a group exists
  bool hasGroup( int field ) const;
  /// Check to see if a specific instance of a group exists
  bool hasGroup( int num, int field ) const;
  /// Count the number of instance of a group
  int groupCount( int field ) const;

  /// Clear all fields from the map
  void clear();
  /// Check if map contains any fields
  bool isEmpty();

  int totalFields() const;

  std::string& calculateString( std::string&, bool clear = true ) const;
   
  // cooper
  std::string& calculateString1( std::string& ) const;

  int calculateLength( int beginStringField = FIELD::BeginString,
                       int bodyLengthField = FIELD::BodyLength,
                       int checkSumField = FIELD::CheckSum ) const;

  int calculateTotal( int checkSumField = FIELD::CheckSum ) const;

  iterator begin() const { return m_fields.begin(); }
  iterator end() const { return m_fields.end(); }
  g_iterator g_begin() const { return m_groups.begin(); }
  g_iterator g_end() const { return m_groups.end(); }

private:
  Fields m_fields;
  Groups m_groups;
};
/*! @} */
} // namespace FIX
} // namespace Kway

#define FIELD_SET( MAP, FIELD )           \
bool isSet( const FIELD& field ) const    \
{ return (MAP).isSetField(field); }       \
void set( const FIELD& field )            \
{ (MAP).setField(field); }                \
FIELD& get( FIELD& field ) const          \
{ return (FIELD&)(MAP).getField(field); }

#define FIELD_GET_PTR( MAP, FLD ) \
(const FIX::FLD*)MAP.getFieldPtr( FIX::FIELD::FLD )
#define FIELD_GET_REF( MAP, FLD ) \
(const FIX::FLD&)MAP.getFieldRef( FIX::FIELD::FLD )

#endif //FIX_FIELDMAP

