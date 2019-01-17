/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_UtilFile.h"
#include "KFile.h"
#include "KTime.h"
#include <stdio.h>
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TFileLock impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TFileLock::TFileLock (const std::string& filePath)
{
   char fileName[256];

   sprintf(fileName, "%s.lock", filePath.c_str());
   FileName_.assign(fileName);
}
//---------------------------------------------------------------------------

K_mf(bool) TFileLock::IsLock ()
{
   TFile file(FileName_.c_str(), fmRead);
   if (!file.IsOpened())
      return false;

   char             buf[256];
   TFile::size_type rz = file.Read(buf, sizeof(buf));

   if (rz > 0) {
      if (strncmp(buf, "lock", 4) == 0)
         return true;
   }
   
   return false;
}
//---------------------------------------------------------------------------

K_mf(void) TFileLock::SetLock (bool isLock, const char* desc)
{
   if (!isLock) {
      int ret = unlink(FileName_.c_str());
      if (ret == 0)
         return;
   }

   char  state[20], buf[256];
   int   len;
   TFile file(FileName_.c_str(), (TFileMode)(fmWrite|fmTrunc|fmCreatePath));

   if (isLock)  strcpy(state, "lock");
   else         strcpy(state, "unlock");

   len = sprintf(buf, "%s by %s %s", state, desc, TKDateTimeMS::Now().as_string(TKDate::sf_ELong).c_str());
   file.Write(buf, len);
}
//---------------------------------------------------------------------------
