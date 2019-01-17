/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseFstFact.h"
#include "ExtDef.h"
#include "TwseFsOrdFlow.h"
#include "TwseFsRptFlow.h"
#include "TwseFstAdpt.h"
#include "TwseFsOrdCenterVA.h"
#include "TwseIniFile.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// class TTwseFstFact impl
/////////////////////////////////////////////////////////////////////////////
K_mf(TTwseFstFact::TFixTradeInfo*) TTwseFstFact::CreateTradeInfo (const std::string& sesID)
{
   return new TTwseFstInfo(sesID, SysOrds_, OrdCenter_);
}
//---------------------------------------------------------------------------

K_mf(TTwseFstFact::TFixOrdFlow*) TTwseFstFact::CreateOrdFlow (TFixTradeInfo* fti)
{
   return new TTwseFsOrdFlow(fti);
}
//---------------------------------------------------------------------------

K_mf(TTwseFstFact::TFixRptFlow*) TTwseFstFact::CreateRptFlow (TFixTradeInfo* fti)
{
   return new TTwseFsRptFlow(fti);
}
//---------------------------------------------------------------------------

K_mf(TTwseFstFact::TFixTradeAdpt*) TTwseFstFact::CreateTradeAdpt (TFixTradeInfo* fti)
{
   return new TTwseFstAdpt(fti, SysOrds_, OrdCenter_, OrdFlow_);
}
//---------------------------------------------------------------------------

K_mf(TVACreater*) TTwseFstFact::CreateOrdCenetrVAC (TFixTradeInfo* fixTradeInfo)
{
   return new TTwseFsOrdCenterVA(fixTradeInfo);
}


/////////////////////////////////////////////////////////////////////////////
// class TTwseFstInfo impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseFstInfo::TTwseFstInfo (const std::string& sesID, TSysOrds* sos, TOrdCenter* oc)
   : inherited (sesID)
   , SysOrds_  (sos)
   , OrdCenter_(oc)
{
   char buf[256];

   sprintf(buf, "%s/FixStk/%s_", GetProcLogPath().c_str(), sesID.c_str());
   LogPath_.assign(buf);

   //==============================================
   // sesID 就是 TandSesID
   TandSesID_ = sesID;

   //==============================================
   // 設定 Debug Level
   byte dbgLevel = GetDebugLevel();

   if (dbgLevel != 3)
      inherited::FixTiProp_.set(FIX::Stk::tip_Disable_RptTrack_Reco);

   //==============================================
   // 設定 Flush
   FIX::Stk::TTradeInfo::SFlushInfo& flhInfo     = inherited::FlushInfo_;
   const TTwseMainIni::SCont&        iniMainCont = TTwseMainIni::GetContC();

   flhInfo.Coids_      = iniMainCont.Main_.AutoFlushCoids_;
   flhInfo.OrdCenter_  = iniMainCont.Main_.AutoFlushOrdCenter_;
   flhInfo.OrdTrack_   = iniMainCont.Main_.AutoFlushOrdTrack_;
   flhInfo.RptCenter_  = iniMainCont.Main_.AutoFlushRptCenter_;
   flhInfo.ExecIDGene_ = iniMainCont.Main_.AutoFlushExecIDGene_;
   
   // 設定 Fix 版本號
   inherited::SetFixVer(iniMainCont.Main_.FixVer_);
}
//---------------------------------------------------------------------------
