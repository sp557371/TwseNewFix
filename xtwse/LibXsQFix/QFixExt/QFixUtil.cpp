/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
//---------------------------------------------------------------------------
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "QFixUtil.h"
#include "KTime.h"
#include "src/Code/Field.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::FIX;

/////////////////////////////////////////////////////////////////////////////
// class TQFixUtil impl
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TQFixUtil::UtcTimeStamp_To_KDateTimeMS (const UtcTimeStampField& tsUtc, TKDateTimeMS& dtms)
{
   // tsUtc = 20100716-05:36:36.796
   // 直接 dtms assign 會變成 20100716 --:--:--.796
   // 所以要把 '-' 變成空白
   const std::string& str = tsUtc.getString();
   if (str.empty()) {
      dtms = TKDateTimeMS();
      return false;
   }

   char  buf[64];
   char* ptr;

   strcpy(buf, str.c_str());
   ptr = strchr(buf, (int)'-');
   if (ptr)
      *ptr = ' ';
   
   dtms.assign(buf);
   // UTC ==> 台灣時間
   dtms.IncHour(8);
   
   return true;
}
//---------------------------------------------------------------------------
