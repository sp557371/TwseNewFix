/////////////////////////////////////////////////////////////////////////////
#if !defined(_FixStkRptTrack_h_)
#define _FixStkRptTrack_h_
//---------------------------------------------------------------------------
#include "tplns/Nsa_EvMap_2k.h"
#include "tplns/Nsa_EvMap_2k_Hnd.h"
#include "QFixExt/QFixTagType.h"
#include "FixStk.h"

//*************************************************
#if defined(_FixStkVer_RptTrack_)

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Nsa;

namespace FIX  {
namespace Stk  {
//---------------------------------------------------------------------------
class TTradeInfo;

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// struct SRptTrack
/////////////////////////////////////////////////////////////////////////////
struct SRptTrack
{
   TCommInn::rr_key mutable  RoomKey_;

   TFixMsgType2   MsgType_;
   // 基本資訊
   TFixStdString  ClOrdID_;      // 011
   TFixStdString  OrigClOrdID_;  // 041
   TFixStdString  Text_;         // 058
   // 狀態
   TFixOrdStatus  OrdStatus_;    // 039
   TFixExecType   ExecType_;     // 150
   // qty
   TFixQty2       CumQty_;       // 014
   TFixQty2       LastShares_;   // 032
   TFixQty2       OrderQty_;     // 038
   TFixQty2       LeavesQty_;    // 151
   // pri
   TFixPri2       LastPx_;       // 031
   TFixPri2       Price_;        // 044

   K_mf(void) operator = (const SRptTrack& a)
   {
      MsgType_     = a.MsgType_;
      ClOrdID_     = a.ClOrdID_;
      OrigClOrdID_ = a.OrigClOrdID_;
      Text_        = a.Text_;
      OrdStatus_   = a.OrdStatus_;
      ExecType_    = a.ExecType_;
      CumQty_      = a.CumQty_;
      LastShares_  = a.LastShares_;
      OrderQty_    = a.OrderQty_;
      LeavesQty_   = a.LeavesQty_;
      LastPx_      = a.LastPx_;
      Price_       = a.Price_;
   }
};

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif

/////////////////////////////////////////////////////////////////////////////
// class TRptTrack
/////////////////////////////////////////////////////////////////////////////
K_class TRptTrack
{
public:
   //==============================================
   // TRecsType
   typedef Kway::Nsa::TEvMap2k<TFixSysOrdID, TFixSysSeqNo, SRptTrack>  TRecsType;
   typedef TRecsType::TMap1       TRsSysOrdID;
   typedef TRsSysOrdID::iterator  TRsSysOrdIDI;
   typedef TRecsType::TMap2       TRsSeqNo;

   //==============================================
   // TRecsHnd
   class TRecsHnd : public TEvMap2kHnd<TRsSysOrdID>
   {
      typedef TEvMap2kHnd<TRsSysOrdID>  inherited;
      
   public:
      inline K_ctor TRecsHnd (TRptTrack* p, const TFixSysOrdID& key1)
         : inherited(&(p->RsSysOrdID_), key1)
      {}
   };
   friend class TRecsHnd;

public:
   K_ctor  TRptTrack (TTradeInfo*);
   K_dtor ~TRptTrack ();

   K_mf(TRsSeqNo*) FindRsSeqNo (const TFixSysOrdID&);
   K_mf(TRsSeqNo*) GetRsSeqNo  (const TFixSysOrdID&);
     
   inline K_mf(TRsSysOrdID*) GetRecs ()  { return &RsSysOrdID_; }

private:
   class TRecsRecorder;

   TRsSysOrdID     RsSysOrdID_;
   TRecsRecorder*  RecsRecorder_;
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Stk
}; // namespace FIX
}; // namespace Kway

#endif // defined(_FixStkVer_RptTrack_)
//*************************************************

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_FixStkRptTrack_h_)
