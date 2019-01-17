/////////////////////////////////////////////////////////////////////////////
#if !defined(_FixStkOrdCenter_h_)
#define _FixStkOrdCenter_h_
//---------------------------------------------------------------------------
#include "FixStkSysOrd.h"
#include "inn_file.h"
#include <stdlib.h>
#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   class TVACreater;
namespace FIX  {
namespace Stk  {

/////////////////////////////////////////////////////////////////////////////
// class TFsOrdCenter
/////////////////////////////////////////////////////////////////////////////
K_class TFsOrdCenter
{
public:
   typedef FIX::Stk::TFsSysOrd   TFsSysOrd;
   typedef FIX::Stk::TFsSysOrds  TFsSysOrds;
   typedef TFsSysOrds::updater   TOrdHnd;
      
   typedef Kstl::rr_inn_file<Kway::uint64, Kway::word>  inn_type;

   //儲存到inn的大小.
   static const int   SysOrdRecSz = sizeof(TFsStkOrderRec);

protected:
   class TSysOrdsRecorder;

   TFsSysOrds         SysOrds_;
   inn_type*          SysOrdsInn_;
   TSysOrdsRecorder*  SysOrdsRecorder_;
   TVACreater*        SysOrdsVAC_;

public:
   K_ctor  TFsOrdCenter (const std::string&);
   K_dtor ~TFsOrdCenter ();

   K_mf(bool) GetOrd (const TFixSysOrdID&, TFsSysOrds::iterator&);

   K_mf(TOrdHnd) GetOrdHnd (TFixSysOrdID, TFixUpdFrom);

   inline K_mf(void) SetSysOrdsVAC (TVACreater* vac)  { SysOrdsVAC_ = vac; }
   
   inline K_mf(TFsSysOrds*) GetSysOrds    ()  { return &SysOrds_;    }
   inline K_mf(TVACreater&) GetSysOrdsVAC ()  { return *SysOrdsVAC_; }

   inline K_mf(size_t) GetOrdCount () const  { return SysOrds_.size(); }
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Stk
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_FixStkOrdCenter_h_)
