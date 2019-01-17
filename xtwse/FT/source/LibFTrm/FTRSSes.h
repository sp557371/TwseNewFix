//---------------------------------------------------------------------------
#ifndef FTRSSesH
#define FTRSSesH
//---------------------------------------------------------------------------
#include "FTRSesMgr.h"
#include "TimerThread.h"
#include "FTRMDef.h"
//---------------------------------------------------------------------------
namespace Kway{
namespace Tw{
namespace Bx{
namespace FT{
//---------------------------------------------------------------------------
K_class TRSSes : public TFTrm_Base
{
   typedef TRSSes                          this_type;
   typedef TFTrm_Base                      inherited;
private:
   TRSesMgr*               SesMgr_;
   Kway::TTimer<this_type> Timer_;
   THostID                 HostID_;
protected:
   K_mf(void)      OnPktRecv        (SFTrm&);
   K_mf(int)       GetPacketSize    (SFTrm*,bool);
   K_mf(void)      OnMesLinkBroken  (TMesBase*, const std::string&);
   K_mf(void)      OnMesLinkFail    (TMesBase*, const std::string&){};
   K_mf(void)      OnMesLinkReady   (TMesBase*);
   K_mf(void)      OnTimer          (TTimerBase*);
   K_mf(void)      ResetTimer       (size_t secs);
   K_mf(void)      ShowMsg          (char*);
public:
   K_ctor          TRSSes           (Kway::TSesMgr* sm);
   K_dtor          ~TRSSes          ();
   K_mf(void)      SendState        (TState, Kway::byte);
};
//---------------------------------------------------------------------------
};
};
};
};
//---------------------------------------------------------------------------
#endif
