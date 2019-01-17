/////////////////////////////////////////////////////////////////////////////
#if !defined(_TwseFsRptFlow_h_)
#define _TwseFsRptFlow_h_
//---------------------------------------------------------------------------
#include "FixStkRptFlow.h"
#include "TwStkIvacNo.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {

/////////////////////////////////////////////////////////////////////////////
// class TTwseFsRptFlow
/////////////////////////////////////////////////////////////////////////////
class TTwseFsRptFlow : public FIX::Stk::TRptFlow
{
   // FIX
   typedef FIX::TFixMsgType2          TFixMsgType2;
   typedef FIX::TFixChar              TFixChar;
   typedef FIX::Message               FixMessage;
   // FIX.Stk
   typedef FIX::Stk::TRptFlow         inherited;
   typedef FIX::Stk::TRptFlow         TFixRptFlow;
   typedef FIX::Stk::TRfReqErBase     TFixRfReqErBase;
   typedef FIX::Stk::TRfReqErOrdBase  TFixRfReqErOrdBase;
   typedef FIX::Stk::TRfReqOcrBase    TFixRfReqOcrBase;
   typedef FIX::Stk::ERfSpecRule      EFixRfSpecRule;
   typedef FIX::Stk::SRfBaseField     SFixRfBaseField;
   typedef FIX::Stk::TFsStkOrder      TFixStkOrder;
   typedef FIX::Stk::SOrdTrack        SFixOrdTrack;
   typedef FIX::Stk::TOrdTrack        TFixOrdTrack;
   // Tw.Stk
   typedef Tw::Stk::TIvacNo  TIvacNo;

public:
   K_ctor TTwseFsRptFlow (FIX::Stk::TTradeInfo*);

protected:
   //==============================================
   // overwrite FIX::Stk::TRptFlow virtual function
   K_mf(void) VirBfOnUpdate_Er  (TFixRfReqErBase&);
   K_mf(void) VirBfOnUpdate_Ocr (TFixRfReqOcrBase&);

   K_mf(bool) VirErOrd_FindOrdTrack (TFixRfReqErOrdBase&, TFixOrdTrack::TRecsHnd&, const TFixMsgType2&);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseFsRptFlow_h_)
