/////////////////////////////////////////////////////////////////////////////
#if !defined(_FixStkRptCenter_h_)
#define _FixStkRptCenter_h_
//---------------------------------------------------------------------------
#include <deque>
#include "wise_ptr.h"
#include "pod_vector.h"
#include "ev_ary.h"
#include "Nsa_Time.h"
#include "tplns/Nsa_EvCtn.h"
#include "FixStkOrder.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   class TVACreater;
   using namespace Kstl;
   using namespace Nsa;

namespace FIX {
namespace Stk {
   class TTradeInfo;

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// struct SStkRpt
/////////////////////////////////////////////////////////////////////////////
struct SStkRpt
{
   TTimeStamp    UpdTime_;
   TFixSysOrdID  SysOrdID_;
   TFixMsgType2  MsgType_;
   std::string   Message_;
};

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif

/////////////////////////////////////////////////////////////////////////////
// ©µ¦ù«Å§i
/////////////////////////////////////////////////////////////////////////////
typedef wise_ptr<SStkRpt>      SStkRptWsPtr;
typedef SStkRptWsPtr::ref_obj  SStkRptWsRef;

/////////////////////////////////////////////////////////////////////////////
// class TRptCenter
/////////////////////////////////////////////////////////////////////////////
K_class TRptCenter
{
public:
   //==============================================
   // Recs Type
   typedef ev_ary<TFixSysRptID, SStkRptWsPtr,
                  index_maker<TFixSysRptID>,
                  std:: deque<SStkRptWsPtr> >  TRsRpt;

   typedef TRsRpt::iterator        TRsRptI;
   typedef TRsRpt::const_iterator  TRsRptCi;
   typedef TRsRpt::updater         TRsRptU;
   typedef TRsRptU                 TRptHnd;

protected:
   class TRsLoader;
   class TRsRptVac;

   TRsRpt       RsRpt_;
   TRsRptVac*   RsRptVac_;
   TFile        RsRptText_;
   const bool&  IsAutoFlush_;

public:
   K_ctor  TRptCenter (TTradeInfo*);
   K_dtor ~TRptCenter ();

   K_mf(TFixSysRptID) NewRpt (const TFixSysOrdID&, const TFixMsgType2&, const std::string& message);
   K_mf(TFixSysRptID) NewRpt (SStkRptWsPtr&);

   K_mf(TVACreater&)  GetSysRptsVAC ();
   K_mf(TFixSysRptID) GetLastRptID  () const;
   
   K_mf(std::string) FindOrderID (const TFixSysRptID&) const;

   inline K_mf(TRsRpt*) GetRsRpt ()  { return &RsRpt_; }

protected:
   K_mf(void) WriteToFile (const TFixSysRptID&, const SStkRptWsRef*);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Stk
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_FixStkRptCenter_h_)
