//---------------------------------------------------------------------------
#ifndef bxLinkSesH
#define bxLinkSesH
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
K_class TbxLinkSes : public TbxSes
{
   typedef TbxSes       inherited;
   typedef TbxLinkSes   this_type;

   TWkr_L1010           Wrk_L1010_;
   TWkr_L1020           Wrk_L1020_;
   TWkr_L1030           Wrk_L1030_;
//   TLinkErrInfo         LinkErrInfo_;
   size_t               LinkTimeOut_;

//   K_mf(void)    SetLinkErrInfo    ();
   K_mf(void)    AddLinkWorkers();
protected:
   ETradeKind           TradeKind_;
public:
   K_ctor  TbxLinkSes  (TbxSesMgr* SesMgr):inherited(SesMgr)
   {
        SetLinkTimeOut(60000);         //TCP/IP連線子系統之TimeOut 時間為六秒
        AddLinkWorkers();
//        SetLinkErrInfo();
   }
   K_dtor ~TbxLinkSes  ();
   virtual K_mf(void)    ReStartLink();
   virtual K_mf(void)    OnSubSysReady() = 0;
   K_mf(void)    DelSubSysReg()  {};
   K_mf(void)    OnWorkerFinished(TPacketCode PkCode,const THead&){ }
//   K_mf(void)    OnLinkWorkerFinished(TPacketCode PkCode,TStatusCode StCode);
   K_mf(void)    OnLinkWorkerFinished(TPacketCode PkCode,const THead& pk);
   inline K_mf(void) SetLinkTimeOut(size_t ms) { LinkTimeOut_ = ms;}
   K_mf(ETradeKind) GetSesTK(){return TradeKind_;}
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
