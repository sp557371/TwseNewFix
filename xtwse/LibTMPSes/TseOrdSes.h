//===================================================================
#ifndef TseOrdSesH
#define TseOrdSesH
//---------------------------------------------------------------------------
#include "SesMgr.h"
#include "TsePkSes.h"
//---------------------------------------------------------------------------
namespace Kway{
namespace Tw{
namespace bx{
using namespace Kstl;
//---------------------------------------------------------------------------
K_class TTseOrdSes : public TTsePkSes
{
   typedef TTseOrdSes this_type;
   typedef TTsePkSes   inherited;
   typedef Kway::Tw::Stk::TOrdCenter::TOrdHnd   TOrdHnd;
   typedef Kway::Tw::Stk::TStkOrder             TStkOrder;
   typedef Kway::TTimer<this_type> TOrdTimer;

private:
   TOrdTimer*              Timer_;
   TOrdTimer*              OrdTimer_;
   bool                    Sending_;
   bool                    Ready_;
   TSesMsg                 SesMsg_;
   TReqPtr                 WaitCfmReqPtr_;  // 20100629 待確認的委託要求(可能是逾時, 斷線....)

   K_mf(void)              OnTimer          (TTimerBase*);
   K_mf(void)              ResetTimer       (size_t);
   K_mf(void)              OnOrdTimer          (TTimerBase*);
   K_mf(void)              StartOrdTimer    ();
protected:
   K_mf(void) OnPktRecv(TbxRec&);
   K_mf(void) OnMesLinkReady(TMesBase*);
   K_mf(void) OnMesLinkBroken(TMesBase*, const std::string&);
public:
   K_ctor     TTseOrdSes     (TbxSesMgr*, Kway::Tw::Stk::TMarket);
   K_dtor     ~TTseOrdSes    ();
   K_mf(void)                 SendPkt(TbxRec&);
   inline K_mf(bool)          IsFree()                    {return (Ready_ && (!Sending_));}
   inline K_mf(bool)          IsSending()                 {return Sending_;}
   inline K_mf(TApCode)       GetApCode(){return ApCode_;}
   K_mf(void)                 OnSesFree();           //當委託處理完畢
   K_mf(void)                 OnRequest(TReqPtr& ReqPtr);
   K_mf(void)                 OrdUpdate(TPacketCode PkCode,const THead&);
   K_mf(void)                 OnSubSysReady();
   K_mf(void)                 DelSubSysReg();
   K_mf(void)                 ReStartLink();
   K_mf(void)    OnWorkerFinished(TPacketCode PkCode,const THead&){};    //Worker工作完畢回傳
   K_mf(void)    OnLinkWorkerFinished(TPacketCode PkCode,const THead&){};    //LinkWorker工作完畢回傳
};
//---------------------------------------------------------------------------
};
};
};
//---------------------------------------------------------------------------
#endif
