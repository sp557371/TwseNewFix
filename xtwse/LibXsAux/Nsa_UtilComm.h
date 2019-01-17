/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_UtilComm_h_)
#define _Nsa_UtilComm_h_
//---------------------------------------------------------------------------
#include <string>
#include "libnsAux.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// class TBoolChar
// 使用 char 可以在檔案中很快的找到 'Y' or 'N'
// 如果是用 bool 的話，檔案中是存 1 or 0，不好找
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TBoolChar
{
   char  value_;

public:
   inline          K_ctor TBoolChar ()       : value_('N')            {}
   inline explicit K_ctor TBoolChar (bool b) : value_(b ? 'Y' : 'N')  {}

   inline K_mf(void) assign (const char* str)
      {
         if (strlen(str) > 0 && (str[0] == 'Y' || str[0] == 'y'))
            value_ = 'Y';
         else
            value_ = 'N';
      }
   
   inline K_mf(void) assign (bool b)                  { value_ = (b ? 'Y' : 'N'); }
   inline K_mf(void) assign (const std::string& str)  { assign(str.c_str());      }

   inline K_mf(bool) is_true  () const  { return (value_ == 'Y' ? true  : false); }
   inline K_mf(bool) is_false () const  { return (is_true()     ? false : true);  }
   
   inline K_mf(std::string) as_string (bool showN = false) const
      {
              if (is_true())  return std::string("Y", 1);
         else if (showN)      return std::string("N", 1);

         return std::string();
      }

   inline K_mf(int) as_int () const
      {
         if (is_true())  return 1;
         return 0;
      }
   
   inline K_mf(bool) operator <  (const TBoolChar& rhs) const  { return (value_ <  rhs.value_); }
   inline K_mf(bool) operator == (const TBoolChar& rhs) const  { return (value_ == rhs.value_); }
   inline K_mf(bool) operator != (const TBoolChar& rhs) const  { return (value_ != rhs.value_); }
};
//---------------------------------------------------------------------------
LIBNSAUX_class TBoolCharYes : public TBoolChar
{
   typedef TBoolChar  inherited;

public:
   inline K_ctor TBoolCharYes () : inherited(true)  {}
};


/////////////////////////////////////////////////////////////////////////////
// class TDayID
/////////////////////////////////////////////////////////////////////////////
enum EDayID
{
   edi_T0 = 0, // 當日
   edi_T1 = 1, // 隔一日
   edi_T2 = 2, // 隔二日
   edi_T3 = 3, // 隔三日
};
//---------------------------------------------------------------------------
LIBNSAUX_class TDayID
{
   typedef TDayID  this_type;
   
   EDayID  Data_;
   
public:
   K_ctor TDayID ()         : Data_(edi_T0) {}
   K_ctor TDayID (EDayID e) : Data_(e)      {}

   K_mf(std::string) as_string () const
      {
         switch (Data_) {
         case edi_T0:  return std::string("1.當日");
         case edi_T1:  return std::string("2.隔一日");
         case edi_T2:  return std::string("3.隔二日");
         case edi_T3:  return std::string("4.隔三日");
         }
         return std::string("");
      }

   K_mf(void) assign (const char* str)
      {
              if (strcmp(str, "1.當日")   == 0)  Data_ = edi_T0;
         else if (strcmp(str, "2.隔一日") == 0)  Data_ = edi_T1;
         else if (strcmp(str, "3.隔二日") == 0)  Data_ = edi_T2;
         else if (strcmp(str, "4.隔三日") == 0)  Data_ = edi_T3;
         else                                    Data_ = edi_T0;
      }

   K_mf(void) assign_int (int n)
      {
         switch (n) {
            case 1:
            default:  Data_ = edi_T0;  break; 
            case 2:   Data_ = edi_T1;  break;
            case 3:   Data_ = edi_T2;  break;
            case 4:   Data_ = edi_T3;  break;
         }
      }

   inline K_mf(void) assign  (const std::string& s)  { assign(s.c_str()); }
   inline K_mf(void) assignT (EDayID    e)           { Data_ = e;         }
   inline K_mf(void) assignT (this_type a)           { Data_ = a.Data_;   }
   inline K_mf(void) clear   ()                      { Data_ = edi_T0;    }

   inline K_mf(int) as_int () const  { return (int)Data_; }
};

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif


/////////////////////////////////////////////////////////////////////////////
// class TSizeCalc
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TSizeCalc
{
   uint64  Data_;

public:
   inline K_ctor TSizeCalc (uint64 n = 0) : Data_(n)  {}

   K_mf(std::string) as_string () const;

   inline K_mf(void) assignT   (uint64 n)  { Data_  = n;              }
   inline K_mf(void) assign_mb (size_t n)  { Data_  = (Size_1MB * n); }
   inline K_mf(void) inc       (size_t n)  { Data_ += n;              }
   inline K_mf(void) clear     ()          { Data_  = 0;              }
   
   inline K_mf(uint64) get_orig    ()                   const  { return Data_;              }
   inline K_mf(bool)   operator <  (const TSizeCalc& a) const  { return (Data_ <  a.Data_); }
   inline K_mf(bool)   operator == (const TSizeCalc& a) const  { return (Data_ == a.Data_); }
   inline K_mf(bool)   operator != (const TSizeCalc& a) const  { return (Data_ != a.Data_); }
   
   static const uint64  Size_1KB;
   static const uint64  Size_1MB;
   static const uint64  Size_1GB;
};


/////////////////////////////////////////////////////////////////////////////
// class TUtilComm
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TUtilComm
{
public:
   static K_mf(bool) FileExists (const char*);

   static void       ConsMsg (const char* fmt, ...);
   static K_mf(void) ConsMsg (int, const char*);

   //**********************************************
   #if defined(__BORLANDC__)
   static K_mf(bool) GetArgs (char argcode, const char** param);
   
   static K_mf(bool) SetPrvInstance ();
   static K_mf(bool) SetPrvInstance (const char* exeName, const char* title);

   // 取得版本號
   static K_mf(std::string) GetModuleVersion ();
   static K_mf(std::string) GetModuleVersion (const char* ModulePath);
   #endif
   //**********************************************
   
   //**********************************************
   #if defined(K_WINDOWS)
   static K_mf(bool) GetModuleName (char* fileName, bool incExt);

   static void BugOut (const char* fmt, ...);
   #endif
   //**********************************************
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_UtilComm_h_)

