/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FixStkIntf.h"
#include <stdio.h>
#include "KTime.h"
// FixStk
#include "FixStkTradeMgr.h"
#include "FixStkOrdCenterVA.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::FIX;
using namespace Kway::FIX::Stk;

/////////////////////////////////////////////////////////////////////////////
// class TTradeLog impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTradeLog::TTradeLog (const char* logPath)
   : LogFile_(NULL)
   , LogPath_(logPath)
{
}
//---------------------------------------------------------------------------

K_mf(void) TTradeLog::LogMem (ELogType mtype, const void* data, size_t lenData)
{
   if (!LogFile_) {
      char logMsg[128];
      int  logLen;

      LogFile_ = new TLogFile(LogPath_.c_str());

      //logLen = sprintf(logMsg, "%cStartup!", 0x0a);
      //LogFile_->LogSize(elt_Info, logMsg, logLen);
   }

   LogFile_->LogSize(mtype, data, lenData);
}


/////////////////////////////////////////////////////////////////////////////
// class TTradeInfo impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTradeInfo::TTradeInfo (const std::string& sesID)
   : SesID_         (sesID)
   , VaMgr_         (new TViewAreaMgr)
   , FixVer_        (42)   // ¹w³]¬°4.2
   , FixTradeLog_   (NULL)
   , FixCoids_      (NULL)
   , FixOrdTrack_   (NULL)
   , FixOrdFlow_    (NULL)
   , FixOrdCenter_  (NULL)
   //**********************************************
   #if defined(_FixStkVer_RptTrack_)
   , FixRptTrack_   (NULL)
   #endif
   //**********************************************
   , FixRptFlow_    (NULL)
   , FixRptCenter_  (NULL)
   , FixTradeUnit_  (NULL)
   , FixTradeAdpt_  (NULL)
{
   char buf[256];

   sprintf(buf, "./Logs/%s/FixStk/%s/",
                     Kway::TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0).c_str(),
                     sesID.c_str());
   LogPath_.assign(buf);
}
//---------------------------------------------------------------------------

K_dtor TTradeInfo::~TTradeInfo ()
{
   delete VaMgr_;
   
   if (FixTradeLog_)  delete FixTradeLog_;
}
//---------------------------------------------------------------------------

K_mf(void) TTradeInfo::UseLogFile ()
{
   if (FixTradeLog_)
      delete FixTradeLog_;

   char filePath[256];
   
   sprintf(filePath, "%sOrdMsg.log", LogPath_.c_str());
   FixTradeLog_ = new TTradeLog(filePath);
}


/////////////////////////////////////////////////////////////////////////////
// class TTradeFact impl
/////////////////////////////////////////////////////////////////////////////
K_mf(TTradeInfo*) TTradeFact::CreateTradeInfo (const std::string& sesID)
{
   return new TTradeInfo(sesID);
}
//---------------------------------------------------------------------------

K_mf(TVACreater*) TTradeFact::CreateOrdCenetrVAC (TTradeInfo* fixTradeInfo)
{
   return new TFsOrdCenterVA(fixTradeInfo);
}
//---------------------------------------------------------------------------
