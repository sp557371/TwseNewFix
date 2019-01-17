/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseFstUnit.h"
#include "TwseFstFact.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// class TTwseFstUnit impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseFstUnit::TTwseFstUnit (const std::string& sesID, TSysOrds* sos, TOrdCenter* oc, TOrdFlow* of)
   : inherited (sesID)
   , SysOrds_  (sos)
   , OrdCenter_(oc)
   , OrdFlow_  (of)
{
   inherited::Init();
      
   // 記錄啟動時 FixVer 資訊
   char msg[128];
   int  msgLen = sprintf(msg, "Param: FixVer[%d]", inherited::TradeInfo_->GetFixVer());
      
   inherited::TradeInfo_->GetFixTradeLog()->LogMem(elt_Info, msg, msgLen);
}
//---------------------------------------------------------------------------

K_mf(FIX::Stk::TTradeFact*) TTwseFstUnit::CreateTradeFact ()
{
   return new TTwseFstFact(SysOrds_, OrdCenter_, OrdFlow_);
}
//---------------------------------------------------------------------------
