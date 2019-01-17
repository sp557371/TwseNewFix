/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseFsOrd.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// class TTwseFsCnvt impl
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TTwseFsCnvt::CheckFixPri (const TFixStdString& pri)
{
   // Max 4 digits + 2 decimals
   if (pri.empty())
      return false;

   size_t      len    = pri.length();
   const char* ptrBeg = pri.c_str();
   const char* ptrEnd = (ptrBeg + len);
   const char* ptrDot = NULL;

   for (const char* ptr=ptrBeg; ptr!=ptrEnd; ++ptr) {
      if (isdigit(*ptr))
         continue;

      if (*ptr != '.' || ptrDot)
         return false;
         
      ptrDot = ptr;
   }

   if (ptrDot) {
      std::string dig(ptrBeg,   ptrDot);
      std::string dec(ptrDot+1, ptrEnd);

      if (dig.length() > 4 || dec.length() > 2)
         return false;
   }
   else {
      if (len > 4)
         return false;
   }

   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TTwseFsCnvt::CheckFixQty (const TFixStdString& qty)
{
   // Max 3 digits
   if (qty.empty())
      return false;

   size_t      len    = qty.length();
   const char* ptrBeg = qty.c_str();
   const char* ptrEnd = (ptrBeg + len);

   //==============================================
   // 俱计程 3 ,ぃす砛计翴Τ
   // す砛   '0' ~ '9', '.'
   // ぃす砛 '-'
   const char* ptrDot = NULL;
   
   for (const char* ptr=ptrBeg; ptr!=ptrEnd; ++ptr) {
      if (isdigit(*ptr))
         continue;

      if (*ptr != '.' || ptrDot)
         return false;
         
      ptrDot = ptr;
   }
   
   if (!ptrDot) {
      if (len > 3)
         return false;
   }
   else {
      // Τ计翴,浪琩计翴琌ぃ 0
      int val = atoi(++ptrDot);
      if (val != 0)
         return false;
   }

   return true;
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFsCnvt::MakeDealExexID (const TStkDeals::const_iterator& iterDeal, TFixExecID& execID)
{
   char buf[20];

   sprintf(buf, "%08d%07d", iterDeal->first.get_orig(), iterDeal->second.BSeqNo_.get_orig());
   execID.assign(buf);
}
//---------------------------------------------------------------------------
