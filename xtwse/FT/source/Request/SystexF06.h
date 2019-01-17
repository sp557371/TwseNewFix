//---------------------------------------------------------------------------
#ifndef SystexF06ReqH
#define SystexF06ReqH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include "char_ary.h"
#include "ReqMgr.h"
#include "PacketSes.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
//---------------------------------------------------------------------------
K_class TSystexF06Ses : public TLineSes
{
    typedef TLineSes    inherited;
private:
    TFTFlowHandler&     Interface_;
    TFTCenterHandler&   Center_;
    TBrokerID           BrkID_;
    TMarket             Mkt_;
    TAPIMgr*            APIMgr_;
    Kway::word          LastSeq_;

protected:
   K_mf(void)      OnPktRecv        (void* , int);
   K_mf(void)      OnMesLinkReady   (TMesBase*);
   K_mf(void)      OnMesLinkBroken  (TMesBase*, const std::string&){};
   K_mf(void)      OnMesLinkFail    (TMesBase*, const std::string&){};
public:
   K_ctor         TSystexF06Ses(TAPIMgr* aOwner, TFTFlowHandler*, TFTCenterHandler*);
   K_dtor         ~TSystexF06Ses();
   K_mf(bool)     SettingApply  (const std::string&);
};
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif

