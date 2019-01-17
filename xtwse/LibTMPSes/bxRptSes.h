//---------------------------------------------------------------------------#ifndef bxRptSesH
#define bxRptSesH
//---------------------------------------------------------------------------
#include "bxDef.h"
#include "bxSes.h"
#include "bxLinkSes.h"
#include "bxRpt.h"
#include "bxRptWorker.h"
#include "bxSesMgr.h"
#include "TwStkOrder.h"
#include "TwStk.h"
#include "bxRecorder.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace bx {
/////////////////////////////////////////////////////////////////////////////
K_class TbxRptSes : public TbxLinkSes
{
   typedef TbxLinkSes       inherited;
   typedef TbxRptSes        this_type;
   TWkr_T5000           Wrk_T5000_;
   TWkr_T5010           Wrk_T5010_;
   TWkr_T5020           Wrk_T5020_;
   size_t               RptTimeOut_;
   TSize                SeqNo_;
   TFile                PMach_;
#ifdef __bxFT
   typedef std::map<TSize,TSize> TPMap;
   TPMap                PMap_;
   bool                 Recovering_;
   TTimer<this_type>    RecoverTimer_;
   K_mf(void)           OnRecoverTimer  (TTimerBase*);
   K_mf(bool)           CallRecover(TSize, TSize);
#endif
   K_mf(void)    AddWorkers();
   K_mf(void)    ReStartRptSys(TSize SeqNo);
public:
   K_ctor  TbxRptSes  (TbxSesMgr*,Kway::Tw::Stk::TMarket);
   K_dtor ~TbxRptSes  ();
   K_mf(void)    OnSubSysReady();
   K_mf(void)    OnTimer           (TTimerBase*);
   K_mf(void)    OnWorkerFinished(TPacketCode PkCode,const THead&);
   inline K_mf(void) SetRptTimeOut(size_t ms) { RptTimeOut_ = ms;}
#ifdef __bxFT   K_mf(void)    RptRecover(TRptBody, bool);#endif};
/////////////////////////////////////////////////////////////////////////////
} // namespace bx
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif

