/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TserIniFile.h"
#include "SyncEv.h"
#include "Nsa_UtilFile.h"
#include "ExtDef.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TTserIni impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTserIni::TTserIni ()
   : inherited()
{
   char filePath[256];

   // ./Settings/XTester.ini
   sprintf(filePath, "%s/XTester.ini", GetConfPath().c_str());
   inherited::FileRead(std::string(filePath));
}
//---------------------------------------------------------------------------

K_mf(void) TTserIni::RemoveSessionInfo (const std::string& sesID)
{
   // 移除 [Summary] 的 SesID
   inherited::EraseValue(std::string("Summary"), std::string("SesID"), sesID);

   // 移除以 SesID 命名的 section
   inherited::EraseSection(sesID);
}


/////////////////////////////////////////////////////////////////////////////
// class TAppiaIni impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TAppiaIni::TAppiaIni (const std::string& filePath)
   : inherited()
{
   TFileLock fileLock(filePath);

   do {
      if (fileLock.IsLock() == false)
         break;

      TSyncObj::CheckSync();
   } while (true);

   fileLock.SetLock(true, GetProcName().c_str());
   inherited::FileRead(filePath);
}
//---------------------------------------------------------------------------

K_dtor TAppiaIni::~TAppiaIni ()
{
   inherited::Flush();

   TFileLock fileLock(inherited::GetFilePath());
   fileLock.SetLock(false, GetProcName().c_str());
}
//---------------------------------------------------------------------------

K_mf(void) TAppiaIni::RemoveSessionInfo (const std::string& sesID, const std::string& mwName)
{
   char buf[50];

   // 移除 [MAIN] connection = [SesID]
   sprintf(buf, "[%s]", sesID.c_str());
   inherited::EraseValue(std::string("MAIN"), std::string("connection"), std::string(buf));
      
   // 移除 [SesID] 的 section
   inherited::EraseSection(sesID);
      
   // 移除 [m538001] middleware_session_id = sesID
   inherited::EraseValue(mwName, std::string("middleware_session_id"), sesID);
}
//---------------------------------------------------------------------------
