/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_SysInfo.h"
#include "MemPool.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TSysInfo impl
/////////////////////////////////////////////////////////////////////////////
TKDate  TSysInfo::TradeDate_;
TKTime  TSysInfo::TradeBegTime_;
//---------------------------------------------------------------------------

K_mf(const TKDate&) TSysInfo::GetTradeDate ()
{
   if (TradeDate_.IsInvalid()) {
      TradeDate_ = TKDate::Now();
   }
   return TradeDate_;
}
//---------------------------------------------------------------------------

K_mf(bool) TSysInfo::IsCrossDay ()
{
   TKDate currDate = TKDate::Now();
   if (currDate != GetTradeDate())
      return true;
   return false;
}
//---------------------------------------------------------------------------

K_mf(TMemPool&) TSysInfo::GetMemPool ()
{
   static TMemPool memPool;
   return memPool;
}
//---------------------------------------------------------------------------

