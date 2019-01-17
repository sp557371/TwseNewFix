/////////////////////////////////////////////////////////////////////////////
#if !defined(_TwseFstAdptRpt_h_)
#define _TwseFstAdptRpt_h_
//---------------------------------------------------------------------------
#include "TwseFstAdpt.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {

/////////////////////////////////////////////////////////////////////////////
// class TTwseFstAdpt::TFixRptPreSet
/////////////////////////////////////////////////////////////////////////////
class TTwseFstAdpt::TFixRptPreSet : public TFixRptFlow::TFlowStep
{
   TTwseFstAdpt*  Adpt_;

public:
   K_ctor  TFixRptPreSet (TTwseFstAdpt*);
   K_dtor ~TFixRptPreSet ();

private:
   //==============================================
   // overwrite TFixRptFlow::TFlowStep virtual function
   K_mf(bool) OnRptReq (TFixRfReqPtr);
};

/////////////////////////////////////////////////////////////////////////////
// class TTwseFstAdpt::TFixRptImpl
/////////////////////////////////////////////////////////////////////////////
class TTwseFstAdpt::TFixRptImpl : public TFixRptFlow::TFlowStep
{
   class TExecIDGene;
   
   TTwseFstAdpt*  Adpt_;
   TFixRptFlow*   FixRptFlow_;
   TExecIDGene*   ExecIDGene_;

public:
   K_ctor  TFixRptImpl (TTwseFstAdpt*);
   K_dtor ~TFixRptImpl ();

private:
   //==============================================
   // overwrite TFixRptFlow::TFlowStep virtual function
   K_mf(bool) OnRptReq (TFixRfReqPtr);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseFstAdptRpt_h_)
