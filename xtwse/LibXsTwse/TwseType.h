/////////////////////////////////////////////////////////////////////////////
#if !defined(_TwseType_h_)
#define _TwseType_h_
//---------------------------------------------------------------------------
#include "char_ary.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;

/////////////////////////////////////////////////////////////////////////////
// class TTwseBaseFunc
/////////////////////////////////////////////////////////////////////////////
K_class TTwseBaseFunc
{
public:
   static K_mf(std::string) GetVersion    ();
   static K_mf(std::string) GetModifyDate ();
};

/////////////////////////////////////////////////////////////////////////////
// class TTwseSesIDTand
/////////////////////////////////////////////////////////////////////////////
// T5380X1
K_class TTwseSesIDTand
{
   typedef char_ary<char, 1, ' '>  TMarket;
   typedef char_ary<char, 4, ' '>  TSesID;
   typedef char_ary<char, 2, ' '>  TSocketID;

   TMarket    Market_;
   TSesID     SesID_;
   TSocketID  SocketID_;

public:
   inline          K_ctor TTwseSesIDTand ()                        {}
   inline explicit K_ctor TTwseSesIDTand (const std::string& str)  { assign(str); }

   // assign
   // T5380X1 => T5380X1
          K_mf(void) assign (const char*);
   inline K_mf(void) assign (const std::string& str)  { assign(str.c_str()); }

   // assign_Apa
   // T5U3U8UALXL1U => T538ax1
          K_mf(void) assign_Apa (const char*);
   inline K_mf(void) assign_Apa (const std::string& str)  { assign_Apa(str.c_str()); }

   inline K_mf(void) clear ()
      {
         Market_  .clear();
         SesID_   .clear();
         SocketID_.clear();
      }

   // as_string
   // T5380X1 => T5380X1
   K_mf(std::string) as_string () const;
   
   // as_string_Apa
   // T538ax1 => T5U3U8UALXL1U
   K_mf(std::string) as_string_Apa () const;
      
   inline K_mf(const TMarket&)   GetMarket   () const  { return Market_;   }
   inline K_mf(const TSesID&)    GetSesID    () const  { return SesID_;    }
   inline K_mf(const TSocketID&) GetSocketID () const  { return SocketID_; }
};

/////////////////////////////////////////////////////////////////////////////
// class TTwseSesIDApa
/////////////////////////////////////////////////////////////////////////////
// T5U3U8UAUXU1U
// T5U3U8UALXL1U
K_class TTwseSesIDApa
{
   typedef char_ary<char, 1, ' '>  TMarket;
   typedef char_ary<char, 8, ' '>  TSesID;
   typedef char_ary<char, 4, ' '>  TSocketID;

   TMarket    Market_;
   TSesID     SesID_;
   TSocketID  SocketID_;

public:
   inline          K_ctor TTwseSesIDApa ()                        {}
   inline explicit K_ctor TTwseSesIDApa (const std::string& str)  { assign(str); }

   // assign
   // T5U3U8UAUXU1U => T5U3U8UAUXU1U
          K_mf(void) assign (const char*);
   inline K_mf(void) assign (const std::string& str)  { assign(str.c_str()); }
      
   // assign_Tand
   // T538ax1 => T5U3U8UALXL1U
          K_mf(void) assign_Tand (const char*);
   inline K_mf(void) assign_Tand (const std::string& str)  { assign_Tand(str.c_str()); }

   inline K_mf(void) clear ()
      {
         Market_  .clear();
         SesID_   .clear();
         SocketID_.clear();
      }

   // as_string
   // T5U3U8UAUXU1U => T5U3U8UAUXU1U
   K_mf(std::string) as_string() const;

   // as_string_Tand
   // T5U3U8UALXL1U => T538ax1
   K_mf(std::string) as_string_Tand () const;
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseType_h_)
