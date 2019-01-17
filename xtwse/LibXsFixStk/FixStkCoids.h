/////////////////////////////////////////////////////////////////////////////
#if !defined(_FixStkCoids_h_)
#define _FixStkCoids_h_
//---------------------------------------------------------------------------
#include "ev_map.h"
#include "tplns/Nsa_EvCtn.h"
#include "QFixExt/QFixTagType.h"
#include "FixStkOrder.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   class TVACreater;
   using namespace Kstl;

namespace FIX  {
namespace Stk  {
   class TTradeInfo;

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// struct SCoid
/////////////////////////////////////////////////////////////////////////////
struct SCoid
{
   TFixSysOrdID     SysOrdID_;
   TFixOrigClOrdID  OrigClOrdID_;
};

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif

/////////////////////////////////////////////////////////////////////////////
// class TCoids
/////////////////////////////////////////////////////////////////////////////
K_class TCoids
{
public:
   typedef ev_map<TFixClOrdID, SCoid>  TRecs;
   typedef TRecs::iterator             TRecsI;
   typedef TRecs::updater              TRecsU;

public:
   K_ctor  TCoids (TTradeInfo*);
   K_dtor ~TCoids ();
   
   K_mf(bool)         AddCoid (const TFixClOrdID&, TRecsU&, TFixSysOrdID sid = TFixSysOrdID());
   K_mf(const SCoid*) FindDat (const TFixClOrdID&);
   
   K_mf(TVACreater&) GetVAC ();

private:
   class TRecsRecorder;
   class TRecsVAC;

   TRecs           Recs_;
   TRecsRecorder*  RecsRecorder_;
   TRecsVAC*       RecsVAC_;
   const bool&     IsAutoFlush_;
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Stk
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_FixStkCoids_h_)
