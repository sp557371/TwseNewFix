//---------------------------------------------------------------------------
#ifndef SystexReqH
#define SystexReqH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include "char_ary.h"
//#include "FTFlowDef.h"
#include "ReqMgr.h"
#include "PacketSes.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
//---------------------------------------------------------------------------
struct SSystexReq
{
   Kstl::char_ary<char, 4,' '>    SeqNo_;;
   char                           Market_;
   char                           MsgType_;
   Kstl::char_ary<char, 3,' '>    FileCode_;
   Kstl::char_ary<char, 6,' '>    Time_;
   char                           Data_[1024];
};
struct TFType : public SSystexReq{
  	Kstl::char_ary<char,16,' '>    ULName_;
  	Kstl::char_ary<char,16,' '>    DLName_;
};
struct TMsgType : public SSystexReq{
   Kstl::char_ary<char, 4,' '>    Length_;
   Kstl::char_ary<char,1024,0>    Data_;
};
//---------------------------------------------------------------------------
K_class TSystexSes : public TLineSes
{
    typedef TLineSes    inherited;
private:
    TFTFlowHandler&     Interface_;
    TFTCenterHandler&   Center_;
    TBrokerID           BrkID_;
    TAPIMgr*            APIMgr_;
    Kway::word          LastSeq_;
    SSystexReq          SystexReq_;

protected:
   K_mf(void)      OnPktRecv        (void* , int);
   K_mf(void)      OnMesLinkReady   (TMesBase*);
   K_mf(void)      OnMesLinkBroken  (TMesBase*, const std::string&){};
   K_mf(void)      OnMesLinkFail    (TMesBase*, const std::string&){};
public:
   K_ctor         TSystexSes(TAPIMgr* aOwner, TFTFlowHandler*, TFTCenterHandler*);
   K_dtor         ~TSystexSes();
   K_mf(bool)     SettingApply  (const std::string&);
};
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif

