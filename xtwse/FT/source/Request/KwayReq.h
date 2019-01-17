//---------------------------------------------------------------------------
#ifndef KwayReqH
#define KwayReqH
//---------------------------------------------------------------------------
#include "ReqDef.h"
#include "ReqMgr.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
//---------------------------------------------------------------------------
K_class TAPISes : public TAPISesBase
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
        K_ctor     TAPISes (TAPIMgr* aOwner, TFTFlowHandler*, TFTCenterHandler*);
        K_dtor    ~TAPISes();
};
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif

