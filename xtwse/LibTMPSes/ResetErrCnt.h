//---------------------------------------------------------------------------
#ifndef ResetCntSesH
#define ResetCntSesH
//---------------------------------------------------------------------------
#include "PacketSes.h"
#include "bxSesMgr.h"
//---------------------------------------------------------------------------
namespace Kway{
namespace Tw{
namespace bx{
using namespace Kway::Tw::Stk;
//---------------------------------------------------------------------------
struct TResetCnt{
   char_ary<char, 4,' '>  BrokerID_;
   char_ary<char, 2,' '>  SocketID_;
   char_ary<char, 1,' '>  ApCode_;
   char_ary<char, 8,' '>  Time_;
   char_ary<char,35,' '>  Reserve_;
   char_ary<char, 1,' '>  NewLine_;      
};
//---------------------------------------------------------------------------
K_class TResetCntSes : public TLineSes
{
   typedef TLineSes inherited;
private:
   int             CurSeqNo_;
   int             LastSeqNo_;
   int             LineNo_;
   TbxSesMgr*      SesMgr_;
protected:
   K_mf(void)      OnPktRecv        (void* , int);
   K_mf(void)      OnMesLinkReady   (TMesBase*){};
   K_mf(void)      OnMesLinkBroken  (TMesBase*, const std::string&){};
   K_mf(void)      OnMesLinkFail    (TMesBase*, const std::string&){};
public:
   K_ctor         TResetCntSes(Kway::TSesMgr* sm);
   K_dtor         ~TResetCntSes(){};
   K_mf(bool)     SettingApply  (const std::string&);
};
//---------------------------------------------------------------------------
}
}
}
//---------------------------------------------------------------------------
#endif

