//---------------------------------------------------------------------------
#ifndef FTRCSesH
#define FTRCSesH
//---------------------------------------------------------------------------
#include "FTRSesMgr.h"
#include "FTRMDef.h"
//---------------------------------------------------------------------------
namespace Kway{
namespace Tw{
namespace Bx{
namespace FT{
//---------------------------------------------------------------------------
K_class TRCSes : public TFTrm_Base
{
   typedef TRCSes       this_type;
   typedef TFTrm_Base   inherited;
private:
   TRSesMgr*               SesMgr_;
   Kway::TTimer<this_type> Timer_;
   std::string             IP_,RIP_;
   TStates                 RemoteState_;
   Kway::byte              RemoteHID_;
   std::string             Remote3TtmpPath_;
protected:
   K_mf(void)              OnPktRecv        (SFTrm&);
   K_mf(int)               GetPacketSize    (SFTrm*,bool);
   K_mf(void)              OnMesLinkBroken  (TMesBase*, const std::string&);
   K_mf(void)              OnMesLinkFail    (TMesBase*, const std::string&);
   K_mf(void)              OnMesLinkReady   (TMesBase*);
   K_mf(void)              OnTimer          (TTimerBase*);
   K_mf(void)              ResetTimer       (size_t secs);
   K_mf(void)              ShowMsg          (char*);
   K_mf(bool)              SettingApply     (const std::string&);
public:
   K_ctor                  TRCSes           (Kway::TSesMgr* sm);
   K_dtor                  ~TRCSes          ();
   K_mf(bool)              SendReq          (const TFTJob*);
   K_mf(Kway::byte)        GetRemoteHID     (){return RemoteHID_;}
};
//---------------------------------------------------------------------------
};
};
};
};
//---------------------------------------------------------------------------
#endif
