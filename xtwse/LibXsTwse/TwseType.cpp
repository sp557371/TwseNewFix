/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseType.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// class TTwseBaseFunc impl
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TTwseBaseFunc::GetVersion ()
{
   return std::string("1.2.1");
}
//---------------------------------------------------------------------------

K_mf(std::string) TTwseBaseFunc::GetModifyDate ()
{
   return std::string("2011-05-18");
}


/////////////////////////////////////////////////////////////////////////////
// class TTwseSesIDTand impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TTwseSesIDTand::assign (const char* str)
{
   // T538001
   clear();

   size_t len = strlen(str);
   if (len == 0)
      return;
      
   if (len >= 1)  Market_  .assign(str);
   if (len >= 5)  SesID_   .assign(str+1);
   if (len >= 7)  SocketID_.assign(str+5);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesIDTand::assign_Apa (const char* str)
{
   // T5U3U8UAUXU1U => T538AX1
   // T5U3U8UALXL1U => T538ax1
   size_t len = strlen(str);
   if (len == 0) {
      clear();
      return;
   }

   const char* beg = str;
   const char* end = (beg + len);
         char  buf[20];
         char* ptr = buf;
         char  ch1, ch2;

   memset(buf, 0, sizeof(buf));
   *ptr = *beg;

   for (++beg; beg<end; ++beg) {
      ch1 = *beg;

      ++ptr;
      if (isdigit(ch1)) {
         *ptr = ch1;
         ++beg;
      }
      else if (isalpha(ch1)) {
         ch2 = *(++beg);
         
              if (ch2 == 'U')  *ptr = toupper(ch1);
         else if (ch2 == 'L')  *ptr = tolower(ch1);
         else                  return;
      }
      else {
         return;
      }
   }

   assign(buf);
}
//---------------------------------------------------------------------------

K_mf(std::string) TTwseSesIDTand::as_string () const
{
   std::string str(Market_.begin(), sizeof(TTwseSesIDTand));
   return str;
}
//---------------------------------------------------------------------------

K_mf(std::string) TTwseSesIDTand::as_string_Apa () const
{
   TTwseSesIDApa apaSesID;
   
   apaSesID.assign_Tand(as_string().c_str());
   return apaSesID.as_string();
}


/////////////////////////////////////////////////////////////////////////////
// class TTwseSesIDApa impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TTwseSesIDApa::assign (const char* str)
{
   // T5U3U8UAUXU1U
   // T5U3U8UALXL1U
   clear();

   size_t len = strlen(str);
   if (len == 0)
      return;
      
   if (len >= 1)   Market_  .assign(str);
   if (len >= 9)   SesID_   .assign(str+1);
   if (len >= 13)  SocketID_.assign(str+9);
}
//---------------------------------------------------------------------------

K_mf(void) TTwseSesIDApa::assign_Tand (const char* str)
{
   // T538AX1 => T5U3U8UAUXU1U
   // T538ax1 => T5U3U8UALXL1U
   size_t len = strlen(str);
   if (len == 0) {
      clear();
      return;
   }

   const char* beg = str;
   const char* end = (beg + len);
         char  buf[20];
         char* ptr = buf;
         char  ch;

   memset(buf, 0, sizeof(buf));
   *ptr = *beg;

   for (++beg; beg<end; ++beg) {
      ch = *beg;

      ++ptr;
      if (isdigit(ch)) {
         *ptr     = ch;
         *(++ptr) = 'U';
      }
      else if (isalpha(ch)) {
         if (ch >= 'A' && ch <= 'Z') {
            *ptr     = ch;
            *(++ptr) = 'U';
         }
         else {
            *ptr     = toupper(ch);
            *(++ptr) = 'L';
         }
      }
      else {
         return;
      }
   }

   assign(buf);
}
//---------------------------------------------------------------------------

K_mf(std::string) TTwseSesIDApa::as_string() const
{
   std::string str(Market_.begin(), sizeof(TTwseSesIDApa));
   return str;
}
//---------------------------------------------------------------------------

K_mf(std::string) TTwseSesIDApa::as_string_Tand () const
{
   TTwseSesIDTand tandSesID;
   
   tandSesID.assign_Apa(as_string().c_str());
   return tandSesID.as_string();
}
//---------------------------------------------------------------------------
