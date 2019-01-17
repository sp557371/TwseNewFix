/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FixStkOrder.h"
#include "Nsa_LineBuf.h"
#include "Nsa_UtilStr.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;
using namespace Kway::FIX;
using namespace Kway::FIX::Stk;

/*
/////////////////////////////////////////////////////////////////////////////
// class TStkState impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TStkState::assign (const char* str)
{
   inherited::reset();

   if (!str || strlen(str) == 0)
      return;
      
   TStrSplit sp(str, ',');
   size_t    size = sp.GetSize();
   size_t    val;

   for (size_t i=0; i<size; ++i) {
      val = (size_t)sp.GetInt(i);
      if (val < ess_Total)
         inherited::set(val);
   }
}
//---------------------------------------------------------------------------

K_mf(std::string) TStkState::as_string () const
{
   const char* itemList[] = {
      "PartialNew",
   };

   TLineBuf line(128);
   
   for (size_t i=0; i<ess_Total; ++i) {
      if (inherited::test(i)) {
         line.Append(itemList[i]);
         line.Append(',');
      }
   }

   if (line.Empty())
      return std::string();

   line.EraseBack(1);
   return std::string(line.GetCStr());
}
//---------------------------------------------------------------------------
*/
