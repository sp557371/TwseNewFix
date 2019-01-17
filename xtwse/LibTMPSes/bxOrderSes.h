//---------------------------------------------------------------------------
#ifndef bxOrderSesH
#define bxOrderSesH
//---------------------------------------------------------------------------
#include "bxDef.h"
#include "bxSes.h"
#include "bxLinkSes.h"
#include "bxOrder.h"
#include "bxOrderWorker.h"
#include "bxSesMgr.h"
#include "TwStkOrder.h"
#include "TwStk.h"
#include "bxRecorder.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace bx {
/////////////////////////////////////////////////////////////////////////////
K_class TbxOrderSes : public TbxLinkSes
{
   typedef TbxLinkSes       inherited;
   typedef TbxOrderSes      this_type;
   typedef Kway::Tw::Stk::TOrdCenter::TOrdHnd   TOrdHnd;
   typedef Kway::Tw::Stk::TStkOrder             TStkOrder;
   typedef Kway::TTimer<this_type>              TTimer;

   TWkr_T3000           Wrk_T3000_;
   TWkr_T4000           Wrk_T4000_;
   TWkr_TOrder          Wrk_TOrder_;
   TWkr_OOrder          Wrk_OOrder_;

   size_t               OrderTimeOut_;
   TbxRecorder*         Recorder;
   TSesMsg              SesMsg_;
   TTimer               ResendTimer_;

   K_mf(void)    AddWorkers();
   K_mf(void)    ShowState(EState);
protected:
   K_mf(void)    OnResendTimer    (TTimerBase*);
   K_mf(void)    StartResendTimer (size_t);
   K_mf(void)    StopResendTimer  ();
public:
   K_ctor        TbxOrderSes  (TbxSesMgr*,Kway::Tw::Stk::TMarket);
   K_dtor        ~TbxOrderSes  ();
   K_mf(void)    OnSubSysReady();
   K_mf(void)    DelSubSysReg();
   K_mf(TApCode) GetApCode();
   K_mf(void)    OnSesFree();           //當委託處理完畢
   K_mf(void)    OnRequest(TReqPtr& ReqPtr);
   K_mf(void)    OnTimer           (TTimerBase*);
   K_mf(void)    OnWorkerFinished(TPacketCode PkCode,const THead&);
   inline K_mf(void) SetOrderTimeOut(size_t ms) { OrderTimeOut_ = ms;}
   K_mf(void)    OrdUpdate(TPacketCode PkCode,const THead&);
};
//========================================================================
} // namespace bx
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif
