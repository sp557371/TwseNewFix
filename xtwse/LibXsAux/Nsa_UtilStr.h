/////////////////////////////////////////////////////////////////////////////
#if !defined(_NsaUtilStr_h_)
#define _NsaUtilStr_h_
//---------------------------------------------------------------------------
#include <string>
#include <vector>
#include "libnsAux.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TStrSplit
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TStrSplit
{
   enum {
      Default_Extra_Num = 3,
   };
   //------------------------------------------------------------------------
   struct SData {
      char*   dat;
      size_t  len;
   };
   typedef std::vector<SData>  TDataA;
   typedef TDataA::iterator    TDataI;
   //------------------------------------------------------------------------

public:
   K_ctor  TStrSplit ();
   K_ctor  TStrSplit (const char* str, const char ch);
   K_ctor  TStrSplit (const char* str, const char* SplitStr);
   K_dtor ~TStrSplit ();

   K_mf(size_t)      SplitByChar (const char* str, const char ch);
   K_mf(size_t)      SplitByStr  (const char* str, const char* SplitStr);

   K_mf(const char*) GetData (size_t idx);
   K_mf(std::string) GetStr  (size_t idx);
   K_mf(int)         GetInt  (size_t idx);
   K_mf(int)         GetInt  (size_t idx, const char* key, char keysp);
   
   K_mf(bool)        FindStr_ByKey (const char* key, char keysp, std::string& str);
   K_mf(bool)        FindInt_ByKey (const char* key, char keysp, int& ResVal);
   K_mf(std::string) GetStr_ByKey  (const char* key, char keysp);
   K_mf(int)         GetInt_ByKey  (const char* key, char keysp);
   K_mf(char)        GetChar_ByKey (const char* key, char keysp);

   inline K_mf(size_t) GetSize ()  { return DataA_.size(); }

private:
   K_mf(char*) GetKeyVal (const char* key, char keysp, TDataI& ita);
   K_mf(void)  Clear ();

private:
   TDataA  DataA_;
};

/////////////////////////////////////////////////////////////////////////////
// class TUtilStr
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TUtilStr
{
public:
   static K_mf(std::string) MakeLowercase (const std::string&);
   // Âà¦¨¤j¼g
   static K_mf(const char*) MakeUppercase (const char* src, char* dst);

   static K_mf(const char*) HasBig5Code (const char*);

   #if defined(__BORLANDC__)
   static K_mf(int) AnsiToUTF8 (const char*, char*, int);
   #endif

   static K_mf(std::string) LTrim (const std::string&);
   static K_mf(std::string) RTrim (const std::string&);
   static K_mf(std::string) Trim  (const std::string&);
};


/////////////////////////////////////////////////////////////////////////////
// class TSettStrBase
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TSettStrBase
{
public:
   typedef std::string  TValue;

   static const char  DefSpl[];
   static const int   DefSplLen;

public:
   //==============================================
   // TSettStrBase function
   K_mf(size_t) Unpack (const char* src, int srcLen, const char* spl = NULL);

protected:
   //==============================================
   // TSettStrBase virtual function
   virtual K_mf(void) OnBfUnpack  ()              = 0;
   virtual K_mf(void) OnUnpackOne (const TValue&) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// class TSettStrSp
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TSettStrSp : public TSettStrBase
{
   typedef std::vector<TValue>  TValues;
   typedef TValues::iterator    TValuesI;

protected:
   TValues  Values_;

public:
   K_mf(size_t) Pack (char* buf, size_t bufSize, const char* spl = NULL);

   K_mf(void) SetVal (size_t idx, const std::string& val);
   K_mf(void) SetVal (size_t idx, int                val);

   K_mf(bool) GetStr (size_t idx, const char*&) const;
   K_mf(bool) GetInt (size_t idx, int&)         const;
 
   inline K_mf(void) Resize (size_t size)  { Values_.resize(size); }

protected:
   //==============================================
   // overwrite TSettStrBase virtual function
   inline K_mf(void) OnBfUnpack  ()                   { Values_.clear();        }
   inline K_mf(void) OnUnpackOne (const TValue& val)  { Values_.push_back(val); }
};


/////////////////////////////////////////////////////////////////////////////
// class TSettStrSpKey
/////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_NsaUtilStr_h_)

