/////////////////////////////////////////////////////////////////////////////
#if !defined(_TwseFsOrd_h_)
#define _TwseFsOrd_h_
//---------------------------------------------------------------------------
#include "TwseFixField.h"
#include "FixStk.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {

/////////////////////////////////////////////////////////////////////////////
// struct STwseFsOrdReseChar
/////////////////////////////////////////////////////////////////////////////
struct STwseFsOrdReseChar
{
   TFixTwseIvacnoFlag  TwseIvacnoFlag_;
   TFixTwseOrdType     TwseOrdType_;
   TFixTwseExCode      TwseExCode_;
   TFixTwseWarrantLP   TwseWarrantLP_;
};

/////////////////////////////////////////////////////////////////////////////
// struct STwseOrdTrackReseChar
/////////////////////////////////////////////////////////////////////////////
struct STwseOrdTrackReseChar
{
   TFixTwseWarrantLP  TwseWarrantLP_;
};

/////////////////////////////////////////////////////////////////////////////
// class TTwseFsCnvt
/////////////////////////////////////////////////////////////////////////////
class TTwseFsCnvt
{
   typedef Tw::Stk::TStkQty            TStkQty;
   typedef Tw::Stk::TStkPri            TStkPri;
   typedef Tw::Stk::TOrdSource         TOrdSource;
   typedef Tw::Stk::TOrdFields_ForNew  TOrdFields_ForNew;
   typedef Tw::Stk::TStkDeals          TStkDeals;
   typedef Tw::Stk::TStkOrder          TStkOrder;
   //--------------------------------------------------------
   typedef FIX::TFixStdString1         TFixStdString;
   typedef FIX::TFixExecID             TFixExecID;
   typedef FIX::TFixQty2               TFixQty;
   typedef FIX::TFixPri2               TFixPri;
   typedef FIX::Stk::TFixSysOrdID      TFixSysOrdID;

public:
   static K_mf(bool) CheckFixPri    (const TFixStdString&);
   static K_mf(bool) CheckFixQty    (const TFixStdString&);
   static K_mf(void) MakeDealExexID (const TStkDeals::const_iterator&, TFixExecID&);

   static inline K_mf(TStkPri) FromFixPri (const TFixPri& fixPri)
      {
         TStkPri twPri;
         if (fixPri.is_null() == false)
            twPri.assign(fixPri.as_double());
         return twPri;
      }
   
   static inline K_mf(TStkQty) FromFixQty (const TFixQty& fixQty)
      {
         TStkQty twQty;
         if (fixQty.is_null() == false)
            twQty.assign(fixQty.get_orig());
         return twQty;
      }

   static inline K_mf(TFixPri) ToFixPri (const TStkPri& twPri)
      {
         TFixPri fixPri;
         fixPri.assign(twPri.as_double());
         return fixPri;
      }

   static inline K_mf(TFixQty) ToFixQty (const TStkQty& twQty)
      {
         TFixQty fixQty;
         fixQty.assign(twQty.get_orig()); 
         return fixQty;
      }

   static inline K_mf(void) FromFixSysOrdID (const TFixSysOrdID& fsoid, TOrdFields_ForNew& ord)
      { ord.OrigSource_.ClOrdID_.assign(fsoid.as_string()); }
         
   static inline K_mf(TFixSysOrdID) ToFixSysOrdID (const TOrdSource& ordSource)
      {
         TFixSysOrdID fixSysOrdID(ordSource.ClOrdID_.as_string());
         return fixSysOrdID;
      }

   static inline K_mf(TFixSysOrdID) ToFixSysOrdID (const TStkOrder* stkOrd)
      {  return ToFixSysOrdID(stkOrd->OrigSource_);  }
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseFsOrd_h_)
