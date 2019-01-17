//---------------------------------------------------------------------------
#ifndef Req3TH
#define Req3TH
//---------------------------------------------------------------------------
#include "ReqDef.h"
#include "ReqMgr.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
//---------------------------------------------------------------------------
K_class TReq3TSes : public TAPISesBase
{
    typedef TAPISesBase inherited;
private:
    TFTFlowHandler&     Interface_;
    TFTCenterHandler&   Center_;
    TAPIMgr*            APIMgr_;
        K_mf(bool) Validate      (StXFile::SFileRequest&);
        K_mf(bool) Validate      (StXFile::SMsgRequest&);
protected:
    // inherited by SesPacket
virtual K_mf(int)  GetPacketSize (StXFile::SPkt&);
virtual K_mf(int)  BeforeAPacket (StXFile::SPkt&);
virtual K_mf(bool) APacket       (StXFile::SPkt&);
virtual K_mf(void) AfterAPacket  (StXFile::SPkt&);
        K_mf(bool) SendRep       (eRetCode);
public:
        K_ctor     TReq3TSes (TAPIMgr* aOwner, TFTFlowHandler*, TFTCenterHandler*);
        K_dtor    ~TReq3TSes();
};
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif

