/////////////////////////////////////////////////////////////////////////////
#if !defined(_NsaSysInfo_h_)
#define _NsaSysInfo_h_
//---------------------------------------------------------------------------
#include "KTime.h"
#include "libnsAux.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   class TMemPool;
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TSysInfo
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TSysInfo
{
public:
   static        K_mf(const TKDate&) GetTradeDate    ();
   static        K_mf(bool)          IsCrossDay      ();
   static        K_mf(TMemPool&)     GetMemPool      ();
   static inline K_mf(const TKTime&) GetTradeBegTime ()  { return TradeBegTime_; }
   static inline K_mf(std::string)   GetTradeDateStr ()  { return GetTradeDate().as_string(TKDate::sf_ELong, 0); }

   static inline K_mf(void) SetTradeDate    (const TKDate& theDate)  { TradeDate_    = theDate; }
   static inline K_mf(void) SetTradeBegTime (const TKTime& theTime)  { TradeBegTime_ = theTime; }

private:
   static TKDate  TradeDate_;
   static TKTime  TradeBegTime_;
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_NsaSysInfo_h_)
