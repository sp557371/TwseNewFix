//---------------------------------------------------------------------------
#ifndef SystexNotifyH
#define SystexNotifyH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include "PacketSes.h"
#include "char_ary.h"
#include "KwayReq.h"
#include "FTJobDef.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
//---------------------------------------------------------------------------
struct SSystexNotify
{
   Kstl::char_ary<char, 4,' '>    SeqNo_;;
   char                           Market_;
   char                           MsgType_;
   Kstl::char_ary<char, 3,' '>    FileCode_;
   Kstl::char_ary<char, 6,' '>    Time_;
   char                           Status_;
   char                           Data_[1024];
};
//---------------------------------------------------------------------------
K_class TSenderMgr;
K_class TSystexNSes : public    TLineSes,
                      protected TFTNotifyHandler
{
    typedef TLineSes            inherited;
    typedef TFTNotifyHandler    inherited_Handler;
    typedef TSystexNSes         this_type;
private:
    TSenderMgr*     SenderMgr_;
    SSystexNotify   SystexNotify_;
protected:
   K_mf(void)      OnPktRecv        (void* , int){};
   K_mf(void)      OnMesLinkReady   (TMesBase* m);
   K_mf(void)      OnMesLinkBroken  (TMesBase*, const std::string&);
   K_mf(void)      OnMesLinkFail    (TMesBase*, const std::string&){};
public:
        K_ctor     TSystexNSes (TSenderMgr* aOwner);
        K_dtor    ~TSystexNSes();
virtual K_mf(bool) DoNotify(const TFTJob&);
        K_mf(void) SendFileCmd   (bool, int, std::string&){};
};
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif
