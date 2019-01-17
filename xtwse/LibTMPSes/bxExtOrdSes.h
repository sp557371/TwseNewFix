//---------------------------------------------------------------------------
#ifndef bxExtOrdSesH
#define bxExtOrdSesH
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
K_class TbxAuctSes : public TbxLinkSes
{
   typedef TbxLinkSes       inherited;
   typedef TbxAuctSes       this_type;
   typedef Kway::Tw::Stk::TOrdCenter::TOrdHnd   TOrdHnd;
   typedef Kway::Tw::Stk::TStkOrder             TStkOrder;

   TWkr_Auction         Wrk_Auction_;
   size_t               OrderTimeOut_;
   TbxRecorder*         Recorder;
   TSesMsg              SesMsg_;
   K_mf(void)           AddWorkers   ();
   K_mf(void)           ShowState    (EState);
public:
   K_ctor               TbxAuctSes   (TbxSesMgr*,Kway::Tw::Stk::TMarket);
   K_dtor               ~TbxAuctSes  ();
//   inline  K_mf(bool)   IsFree       ()    {return State_ == sp_WaitForOrder ? true : false;}
   K_mf(void)           OnSubSysReady();
   K_mf(void)           DelSubSysReg ();
   K_mf(void)           OnSesFree    ();           //當委託處理完畢
   K_mf(void)           OnRequest    (TReqPtr ReqPtr);
   K_mf(void)           OnTimer      (TTimerBase*);
   K_mf(void)           OnWorkerFinished(TPacketCode PkCode,const THead&);
   inline K_mf(void)    SetOrderTimeOut(size_t ms) { OrderTimeOut_ = ms;}
   K_mf(bool)           SettingApply  (const std::string&);
};
//---------------------------------------------------------------------------
K_class TbxLendSes : public TbxLinkSes             // 標借作業
{
   typedef TbxLinkSes       inherited;
   typedef TbxLendSes       this_type;
   typedef Kway::Tw::Stk::TOrdCenter::TOrdHnd   TOrdHnd;
   typedef Kway::Tw::Stk::TStkOrder             TStkOrder;

   TWkr_Lend            Wrk_Lend_;
   size_t               OrderTimeOut_;
   TbxRecorder*         Recorder;
   TSesMsg              SesMsg_;
   K_mf(void)           AddWorkers   ();
   K_mf(void)           ShowState    (EState);
public:
   K_ctor               TbxLendSes   (TbxSesMgr*,Kway::Tw::Stk::TMarket);
   K_dtor               ~TbxLendSes  ();
//   inline  K_mf(bool)   IsFree       ()    {return State_ == sp_WaitForOrder ? true : false;}
   K_mf(void)           OnSubSysReady();
   K_mf(void)           DelSubSysReg ();
   K_mf(void)           OnSesFree    ();           //當委託處理完畢
   K_mf(void)           OnRequest    (TReqPtr ReqPtr);
   K_mf(void)           OnTimer      (TTimerBase*);
   K_mf(void)           OnWorkerFinished(TPacketCode PkCode,const THead&);
   inline K_mf(void)    SetOrderTimeOut(size_t ms) { OrderTimeOut_ = ms;}
   K_mf(bool)           SettingApply  (const std::string&);
};
//---------------------------------------------------------------------------
K_class TbxTendSes : public TbxLinkSes                  // 標購作業
{
   typedef TbxLinkSes       inherited;
   typedef TbxTendSes       this_type;
   typedef Kway::Tw::Stk::TOrdCenter::TOrdHnd   TOrdHnd;
   typedef Kway::Tw::Stk::TStkOrder             TStkOrder;

   TWkr_Tend            Wrk_Tend_;
   size_t               OrderTimeOut_;
   TbxRecorder*         Recorder;
   TSesMsg              SesMsg_;
   K_mf(void)           AddWorkers   ();
   K_mf(void)           ShowState    (EState);
public:
   K_ctor               TbxTendSes   (TbxSesMgr*,Kway::Tw::Stk::TMarket);
   K_dtor               ~TbxTendSes  ();
//   inline  K_mf(bool)   IsFree       ()    {return State_ == sp_WaitForOrder ? true : false;}
   K_mf(void)           OnSubSysReady();
   K_mf(void)           DelSubSysReg ();
   K_mf(void)           OnSesFree    ();           //當委託處理完畢
   K_mf(void)           OnRequest    (TReqPtr ReqPtr);
   K_mf(void)           OnTimer      (TTimerBase*);
   K_mf(void)           OnWorkerFinished(TPacketCode PkCode,const THead&);
   inline K_mf(void)    SetOrderTimeOut(size_t ms) { OrderTimeOut_ = ms;}
   K_mf(bool)           SettingApply  (const std::string&);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace bx
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif
