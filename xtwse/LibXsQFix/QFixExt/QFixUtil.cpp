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
   // ���� dtms assign �|�ܦ� 20100716 --:--:--.796
   // �ҥH�n�� '-' �ܦ��ť�
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
   // UTC ==> �x�W�ɶ�
   dtms.IncHour(8);
   
   return true;
}
//---------------------------------------------------------------------------
