/////////////////////////////////////////////////////////////////////////////
#if !defined(_QFixField_h_)
#define _QFixField_h_
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include <string>
#include "src/Code/FieldNumbers.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway  {
namespace FIX   {
namespace FIELD {

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
   
/////////////////////////////////////////////////////////////////////////////
// class TFieldNum
/////////////////////////////////////////////////////////////////////////////
K_class TFieldNum
{
   typedef TFieldNum  this_type;

   Field  Data_;

public:
   inline K_ctor TFieldNum (Field field = (Field)0) : Data_(field)  {}

   K_mf(void)        assign    (const char*);
   K_mf(std::string) as_string () const;

   inline K_mf(void) assignT (Field field)             { Data_ = field;       }
   inline K_mf(void) assign  (int nVal)                { Data_ = (Field)nVal; }
   inline K_mf(void) assign  (const std::string& str)  { assign(str.c_str()); }
   inline K_mf(void) clear   ()                        { Data_ = (Field)0;    }

   inline K_mf(Field) get_orig () const  { return Data_;      }
   inline K_mf(int)   as_int   () const  { return (int)Data_; }
   
   inline K_mf(bool) operator <  (const this_type& rhs) const  { return (Data_ <  rhs.Data_); }
   inline K_mf(bool) operator == (const this_type& rhs) const  { return (Data_ == rhs.Data_); }
};

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif

/////////////////////////////////////////////////////////////////////////////
}; // namespace FIELD
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_QFixField_h_)

