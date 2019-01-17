/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "CtrlFieldType.h"
#include "ExtDef.h"
#include "Nsa_Time.h"
#include "Nsa_LineBuf.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TTandFuncCode impl
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TTandFuncCode::as_desc () const
{
   ETandFuncCode funcCode = get_orig();
   if (funcCode <= tfc_None || funcCode >= tfc_Total)
      return std::string();

   char buf[10];
   sprintf(buf, "%02d.", (int)funcCode);

   std::string str(buf);

   switch (funcCode) {
      case tfc_StatusQuery:           str.append("StatusQuery");           break;
      case tfc_Suspend:               str.append("Suspend");               break;
      case tfc_SuspendWithTerminate:  str.append("SuspendWithTerminate");  break;
      case tfc_Resume:                str.append("Resume");                break;
      case tfc_ServiceDown:           str.append("ServiceDown");           break;
      case tfc_ServiceUp:             str.append("ServiceUp");             break;
      case tfc_Hold:                  str.append("Hold");                  break;
      case tfc_Release:               str.append("Release");               break;
      case tfc_SequenceReset:         str.append("SequenceReset");         break;
      case tfc_Heartbeat:             str.append("Heartbeat");             break;
   }

   return str;
}


/////////////////////////////////////////////////////////////////////////////
// class TTandemTime impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TTandemTime::Now ()
{
   TTimeStamp ts(TTimeStamp::Now());
   char       buf[20];
   
   sprintf(buf, "%s%03d", ts.GetTime().as_string(false).c_str(), ts.GetTimeMS().GetMS());
   inherited::assign(buf);
}
//---------------------------------------------------------------------------

K_mf(void) TTandemTime::assign (const TKDateTimeMS& dt)
{
   char buf[20];
   
   sprintf(buf, "%s%03d", dt.GetTime().as_string(false).c_str(), dt.GetMS().GetMS());
   inherited::assign(buf);
}
//---------------------------------------------------------------------------

K_mf(std::string) TTandemTime::as_string () const
{
   if (inherited::empty())
      return std::string();

   TLineBuf line(20);
   
   line.Append(inherited::begin(), 2);
   line.Append(':');
   line.Append(inherited::begin()+2, 2);
   line.Append(':');
   line.Append(inherited::begin()+4, 2);
   line.Append('.');
   line.Append(inherited::begin()+6, 3);
   
   return std::string(line.GetCStr());
}


/////////////////////////////////////////////////////////////////////////////
// class TFixSesID impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TFixSesID::assign (const char* str)
{
   BrokerID_.clear();
   SocketID_.clear();

   size_t len = strlen(str);
   if (!str || len == 0)
      return;

   if (len > 4) {
      BrokerID_.assign(str, 4);
      SocketID_.assign(str+4);
   }
   else {
      BrokerID_.assign(str);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TFixSesID::assign_full (const char* str)
{
   BrokerID_.clear();
   SocketID_.clear();

   size_t len = strlen(str);
   if (!str || len <= 1)
      return;

   assign(str+1);
}
//---------------------------------------------------------------------------

K_mf(std::string) TFixSesID::as_string () const
{
   if (BrokerID_.empty())
      return std::string();

   char buf[10];
   sprintf(buf, "%s%s", BrokerID_.as_string().c_str(), SocketID_.as_string().c_str());
   return std::string(buf);
}
//---------------------------------------------------------------------------

K_mf(std::string) TFixSesID::as_full () const
{
   std::string str = as_string();
   if (str.empty())
      return str;

   char buf[20];
   char ch = 'T';
   
   if (GetProcMkt() == Tw::Stk::m_Otc)
      ch = 'O';

   sprintf(buf, "%c%s", ch, str.c_str());
   return std::string(buf);
}
//---------------------------------------------------------------------------
