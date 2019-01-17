//---------------------------------------------------------------------------
#ifndef bxTestSesH
#define bxTestSesH
//---------------------------------------------------------------------------
#include "bxDef.h"
#include "bxSes.h"
#include "bxLink.h"
#include "bxLinkWorker.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace bx {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
   #pragma pack(push,1)
#else
   #pragma pack(1)
#endif
//---------------------------------------------------------------------------
K_class TbxTestSes : public TbxSes
{
   typedef TbxSes       inherited;
   typedef TbxTestSes   this_type;

   TWkr_L1010           Wrk_L1010_;
   TWkr_L1020           Wrk_L1020_;
   TWkr_L1030           Wrk_L1030_;
   size_t               LinkTimeOut_;

   K_mf(void)    AddLinkWorkers();
public:
   K_ctor  TbxTestSes  (TbxSesMgr* SesMgr,Kway::Tw::Stk::TMarket mkt):inherited(SesMgr)
   {
        SetLinkTimeOut(60000);         //TCP/IP連線子系統之TimeOut 時間為六秒
        AddLinkWorkers();
        SetMarket(mkt);
   }
   K_dtor ~TbxTestSes  ();
   K_mf(void)    ReStartLink();
   K_mf(void)    OnSubSysReady(){};
   K_mf(void)    DelSubSysReg()  {};
   K_mf(void)    OnWorkerFinished(TPacketCode PkCode,const THead&){ }
   K_mf(void)    OnLinkWorkerFinished(TPacketCode PkCode,const THead& pk);
   inline K_mf(void) SetLinkTimeOut(size_t ms) { LinkTimeOut_ = ms;}
   K_mf(void)    OnMesLinkReady(TMesBase*);
   K_mf(void)    OnTimer(TTimerBase*);
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
   #pragma pack(pop)
#else
   #pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
} // namespace bx
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif
