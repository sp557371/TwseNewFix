//---------------------------------------------------------------------------
#ifndef TwStkPersistentH
#define TwStkPersistentH
//---------------------------------------------------------------------------
#include "TwStkOrder.h"
#include "kpersistent.h"
//---------------------------------------------------------------------------
namespace Kstl {

using namespace Kway::Tw::Stk;
/////////////////////////////////////////////////////////////////////////////
inline bool pload (const Kstl::persistent& per, TBrokerID& a)  { return per.loadHiLoTrans(a); }
inline bool psave (Kstl::persistent& per, const TBrokerID& a)  { return per.saveHiLoTrans(a); }
//---------------------------------------------------------------------------
inline bool pload (const Kstl::persistent& per, TIvacNo& a)    { return per.loadHiLoTrans(a); }
inline bool psave (Kstl::persistent& per, const TIvacNo& a)    { return per.saveHiLoTrans(a); }
//---------------------------------------------------------------------------
inline bool pload (const Kstl::persistent& per, TIvacKey& a)
{  TBrokerID   b;
   TIvacNo     i;
   if(!pload(per,b)  ||  !pload(per,i))
      return false;
   a.Set(b,i);
   return true;
}

inline bool psave (Kstl::persistent& per, const TIvacKey& a)
{  TBrokerID   b = a.GetBrokerID();
   TIvacNo     i = a.GetIvacNo();
   return psave(per, b)
       && psave(per, i);
}
//---------------------------------------------------------------------------
inline bool pload (const Kstl::persistent& per, TScForces& a)  { return per.loadHiLoTrans(a); }
inline bool psave (Kstl::persistent& per, const TScForces& a)  { return per.saveHiLoTrans(a); }
//---------------------------------------------------------------------------
inline bool pload (const Kstl::persistent& per, TScCode& a)    { return per.loadBytes(a); }
inline bool psave (Kstl::persistent& per, const TScCode& a)    { return per.saveBytes(a); }
//---------------------------------------------------------------------------
inline bool pload (const Kstl::persistent& per, TOrigSysOrdID& a) { return pload(per, a.HostID_) && pload(per, a.SysOrdID_); }
inline bool psave (Kstl::persistent& per, const TOrigSysOrdID& a) { return psave(per, a.HostID_) && psave(per, a.SysOrdID_); }
//---------------------------------------------------------------------------
inline bool pload (const Kstl::persistent& per, TTseOrdID& a)
{
   TTseOrdID::TTerm  term;
   TTseOrdID::TSeq   seqn;
   if(!pload(per,term)
   || !pload(per,seqn))
      return false;
   a = TTseOrdID(term[0],seqn);
   return true;
}

inline bool psave (Kstl::persistent& per, const TTseOrdID& a)
{
   TTseOrdID::TTerm  term = a.GetTermID();
   TTseOrdID::TSeq   seqn = a.GetSeqNo();
   return psave(per,term)
       && psave(per,seqn);
}
/////////////////////////////////////////////////////////////////////////////
TWSTK_fn(bool) pload (const Kstl::persistent&, TOrdSource&);
TWSTK_fn(bool) psave (Kstl::persistent&, const TOrdSource&);
TWSTK_fn(bool) pload (const Kstl::persistent&, TOrdFields_ForNew&);
TWSTK_fn(bool) psave (Kstl::persistent&, const TOrdFields_ForNew&);
TWSTK_fn(bool) pload (const Kstl::persistent&, TStkOrder_Rec&);
TWSTK_fn(bool) psave (Kstl::persistent&, const TStkOrder_Rec&);
TWSTK_fn(bool) pload (const Kstl::persistent&, TStkDeal&);
TWSTK_fn(bool) psave (Kstl::persistent&, const TStkDeal&);
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif
