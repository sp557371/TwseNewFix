/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "CtrlIniFile.h"
#include "ExtDef.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// class TCtrlIni impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TCtrlIni::TCtrlIni ()
   : inherited()
{
   char filePath[256];

   // ./Settings/XCtrl.ini
   sprintf(filePath, "%s/XCtrl.ini", GetConfPath().c_str());
   inherited::FileRead(std::string(filePath));
}
//---------------------------------------------------------------------------
