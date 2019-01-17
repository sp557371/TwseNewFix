/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "QFixTagTypeBase.h"
#include <math.h>
#include "src/Code/Field.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::FIX;
//---------------------------------------------------------------------------
const TFixQty2::orig_type  TFixQty2::Null_Qty = 0x80000000;
const TFixPri2::orig_type  TFixPri2::Null_Pri = ((TFixPri2::orig_type)pow(2, 32)*
                                                 (TFixPri2::orig_type)pow(2, 31));

/////////////////////////////////////////////////////////////////////////////
// class TFixStdString1 impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TFixStdString1::assign (const FieldBase& fb)
{
   inherited::assign(fb.getString());
}


/////////////////////////////////////////////////////////////////////////////
// class TFixBool impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TFixBool::assign (char ch)
{
   if (ch == 'Y' || ch == 'y')
      Data_ = Bool_True;
   else
      Data_ = Bool_False;
}
//---------------------------------------------------------------------------

K_mf(void) TFixBool::assign (const char* str)
{
   size_t len;

   Data_ = Bool_False;
   if (!str || (len = strlen(str)) == 0)
      return;

   if (len == 1) {
      assign(str[0]);
   }
   else {
   }
}
//---------------------------------------------------------------------------

K_mf(void) TFixBool::assign (const FieldBase& fb)
{
   assign(fb.getString().c_str());
}


/////////////////////////////////////////////////////////////////////////////
// class TFixChar impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TFixChar::assign (const FieldBase& fb)
{
   assign(fb.getString().c_str());
}


/////////////////////////////////////////////////////////////////////////////
// class TFixInt impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TFixInt::assign (const FieldBase& fb)
{
   assign(fb.getString().c_str());
}
