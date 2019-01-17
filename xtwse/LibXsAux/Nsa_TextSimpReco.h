/////////////////////////////////////////////////////////////////////////////
#if !defined(_NsaTextSimpReco_h_)
#define _NsaTextSimpReco_h_
//---------------------------------------------------------------------------
#include "Nsa_BaseType.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TTextSimpReco_Reader
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TTextSimpReco_Reader
{
   class TFileReader;

   std::string  FilePath_;
   char         NewLineSplit_[5];

public:
   K_ctor TTextSimpReco_Reader (const char* cpFilePath);
   K_ctor TTextSimpReco_Reader (const char* ccpFilePath, const char* ccpNewLineSplit);

   K_mf(std::string) GetLastRec (size_t OneRecMaxSize = 256);
   K_mf(size_t)      GetAllRec  (TKpvStrings&);
};


/////////////////////////////////////////////////////////////////////////////
// class TTextSimpReco_Writer
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TTextSimpReco_Writer
{
   std::string  FilePath_;
   char         NewLineSplit_[5];

public:
   K_ctor TTextSimpReco_Writer (const char* cpFilePath);

   K_mf(void) Append    (const char* cpData);
   K_mf(void) SetAllRec (TKpvStrings&);

   inline K_mf(void) Append (const std::string& strData)  { Append(strData.c_str()); }
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_NsaTextSimpReco_h_)
