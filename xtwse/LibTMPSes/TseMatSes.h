//===================================================================
#ifndef TseMatSesH
#define TseMatSesH
//---------------------------------------------------------------------------
#include "SesMgr.h"
#include "TsePkSes.h"
#include <queue>
//---------------------------------------------------------------------------
namespace Kway{
namespace Tw{
namespace bx{
using namespace Kstl;
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
   #pragma pack(push,1)
#else
   #pragma pack(1)
#endif
//---------------------------------------------------------------------------
K_class TTseMatSes : public TTsePkSes
{
   typedef TTseMatSes this_type;
   typedef TTsePkSes   inherited;
   typedef Kway::Tw::Stk::TOrdCenter::TOrdHnd   TOrdHnd;
   typedef Kway::Tw::Stk::TStkOrder             TStkOrder;
   typedef Kway::TTimer<this_type> TRptTimer;
   typedef Kway::Tw::Stk::TOrdCenter::TRpt::TDeal TDeal;

private:
   TDeal                   rpt;
   TRptTimer*              Timer_;
   bool                    Sending_;
   bool                    Ready_;
   TSesMsg                 SesMsg_;
   size_t                  RptTimeOut_;
   Kway::dword             SeqNo_;
   TFile                   PMach_;
   K_mf(void)              OnTimer          (TTimerBase*);
   K_mf(void)              StartTimer       (size_t);
   inline K_mf(void)       SetRptTimeOut(size_t ms) { RptTimeOut_ = ms;}
protected:
   K_mf(void) OnPktRecv(TbxRec&);
   K_mf(void) OnMesLinkReady(TMesBase*);
   K_mf(void) OnMesLinkBroken(TMesBase*, const std::string&);
   K_mf(void) ReStartRptSys(TSize SeqNo);
public:
   K_ctor     TTseMatSes     (TbxSesMgr*, Kway::Tw::Stk::TMarket);
   K_dtor     ~TTseMatSes    ();
   K_mf(void)                 SendPkt(TbxRec&);
   inline K_mf(bool)          IsFree()                    {return (Ready_ && (!Sending_));}
   inline K_mf(bool)          IsSending()                 {return Sending_;}
   inline K_mf(TApCode)       GetApCode(){return ApCode_;}
   K_mf(void)                 OnSubSysReady();
   K_mf(void)                 DelSubSysReg();
   K_mf(void)                 ReStartLink(){};
   K_mf(void)    OnWorkerFinished(TPacketCode PkCode,const THead&){};    //Worker工作完畢回傳
   K_mf(void)    OnLinkWorkerFinished(TPacketCode PkCode,const THead&){};    //LinkWorker工作完畢回傳
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
  #pragma pack(pop)
#else
  #pragma pack()
#endif
//---------------------------------------------------------------------------
};
};
};
//---------------------------------------------------------------------------
#endif
