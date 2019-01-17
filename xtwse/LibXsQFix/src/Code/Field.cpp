/////////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER)
#include "stdafx.h"
#else
#include "KConfig.h"
#endif
//---------------------------------------------------------------------------
#include "Field.h"
#include "CallStack.h"
#include <sstream>
#include <numeric>
#include "UtilityQ.h"
#include <stdio.h>

namespace Kway
{
namespace FIX
{
void FieldBase::calculate() const
{
  if( m_calculated ) return;

  char buf[64];

  if( 13 + m_string.length() < sizeof(buf) )
  {
    int tagLength = STRING_SPRINTF( buf, "%d=", m_field );
    m_length = tagLength + m_string.length() + 1;
    memcpy( buf + tagLength, m_string.data(), m_string.length() );
    buf[m_length - 1] = '\001';
    m_data.assign( buf, m_length );
  }
  else
  {
    m_data = IntConvertor::convert(m_field) + "=" + m_string + "\001";
    m_length = m_data.length();
  }

  const unsigned char* iter =
    reinterpret_cast<const unsigned char*>( m_data.c_str() );
  m_total = std::accumulate( iter, iter + m_length, 0 );

  m_calculated = true;
}
} // namespace FIX
} // namespace Kway
