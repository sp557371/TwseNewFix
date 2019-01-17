/////////////////////////////////////////////////////////////////////////////
#if !defined(_CtrlIniFile_h_)
#define _CtrlIniFile_h_
//---------------------------------------------------------------------------
#include "KIniFile.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {

/////////////////////////////////////////////////////////////////////////////
// class TCtrlIni
/////////////////////////////////////////////////////////////////////////////
class TCtrlIni : public TKIniFile
{
   typedef TKIniFile  inherited;

public:
   K_ctor TCtrlIni ();

   // GetTandemCommandPort
   inline K_mf(std::string) GetTandemCommandPort ()
      { return inherited::ReadString(std::string("Summary"), std::string("TandemCommandPort"), std::string()); }
         
   // GetXCtrlAppiaMwPort
   inline K_mf(std::string) GetXCtrlAppiaMwPort ()
      { return inherited::ReadString(std::string("Summary"), std::string("XCtrlAppiaMwPort"), std::string()); }
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_CtrlIniFile_h_)
