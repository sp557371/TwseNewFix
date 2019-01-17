//---------------------------------------------------------------------------
#ifndef Notify3TH
#define Notify3TH
//---------------------------------------------------------------------------
#include "NotifyDef.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
//---------------------------------------------------------------------------
K_class TSenderMgr;
K_class TNotify3TSes : public TNotify3TBase,
                       protected TFTNotifyHandler
{
    typedef TNotify3TBase       inherited;
    typedef TFTNotifyHandler    inherited_Handler;
    typedef TNotify3TSes         this_type;
private:
   Kway::TTimer<this_type> HBTimer_;
   K_mf(void)              ResetTimer       (size_t secs);
virtual K_mf(void) OnMesLinkReady   (Kway::TMesBase*);
virtual K_mf(void) OnMesLinkBroken  (Kway::TMesBase*, const std::string& reason);
    TSenderMgr*     SenderMgr_;
protected:
    // inherited by SesPacket
virtual K_mf(int)  GetPacketSize (SNotifyPkt::SPkt&);
virtual K_mf(int)  BeforeAPacket (SNotifyPkt::SPkt&);
virtual K_mf(bool) APacket       (SNotifyPkt::SPkt&);
        K_mf(void) OnHBTimer     (TTimerBase*);
public:
        K_ctor     TNotify3TSes  (TSenderMgr* aOwner);
        K_dtor    ~TNotify3TSes  ();
        K_mf(bool) SendFeedBack  (StXFile::SFileRequest&, char aProcFlag, char* aReplyCode, int aSeqNo=-1);
virtual K_mf(bool) DoNotify      (const TFTJob&);
virtual K_mf(void) SendFileCmd   (bool, int, std::string&);
};
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif
