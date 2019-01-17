/////////////////////////////////////////////////////////////////////////////
#if !defined(_QFixUtil_h_)
#define _QFixUtil_h_
//---------------------------------------------------------------------------
#include "KwayBase.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   class TKDateTimeMS;

namespace FIX  {
   class UtcTimeStampField;
   
/////////////////////////////////////////////////////////////////////////////
// class TQFixUtil
/////////////////////////////////////////////////////////////////////////////
K_class TQFixUtil
{
public:
   static K_mf(bool) UtcTimeStamp_To_KDateTimeMS (const UtcTimeStampField&, TKDateTimeMS&);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_QFixUtil_h_)
