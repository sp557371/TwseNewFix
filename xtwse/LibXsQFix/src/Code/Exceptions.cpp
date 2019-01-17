/////////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER)
#include "stdafx.h"
#else
#include "KConfig.h"
#endif
//---------------------------------------------------------------------------
#include "Exceptions.h"
#include "UtilityQ.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace FIX  {

/////////////////////////////////////////////////////////////////////////////
// SocketException
/////////////////////////////////////////////////////////////////////////////
std::string SocketException::errorToWhat()
{
  #if defined(_MSC_VER) || defined(__BORLANDC__)
  error = WSAGetLastError();
  char buffer[2048];
  FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, error,
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                 buffer, 2048, NULL );
  //cooper
  //return buffer;
  return std::string(buffer);
    
  #else
  error = errno;
  return strerror( error );
  #endif
}
//---------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////
}; // namespace FIX
}; // namespace Kway
