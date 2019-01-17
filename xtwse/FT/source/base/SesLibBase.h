//---------------------------------------------------------------------------
#ifndef SesLibBaseH
#define SesLibBaseH
//---------------------------------------------------------------------------
#include "SesMgr.h"
#include "TimerThread.h"
#include "Mes.h"
#include "Ses.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
//---------------------------------------------------------------------------
K_class TSesLibBase : public TSesBase
{
   typedef TSesBase             inherited;
   typedef TSesLibBase          this_type;
   typedef TTimer<this_type>    Timer;

protected:
   TSesMgr* Owner_;
   int      HeadCode_;
   long     GiveupBytes_;
   int      AutoRelinkSecs_;
   Timer    Timer_;
   char     StateBuf_[128];
        K_mf(int)  GiveupBuf        (char* aBuf, int aSkipSz, int aBufSz);
   //↓計時器的處理，請注意，單位是sec，預設的DoTimeOut()不做任何事情
virtual K_mf(void) DoTimeOut        (TTimerBase*);
virtual K_mf(void) OnMesLinkReady   (TMesBase*)                       {}
virtual K_mf(void) OnMesLinkFail    (TMesBase*, const std::string& reason)  {};
virtual K_mf(void) OnMesLinkBroken  (TMesBase*, const std::string& reason)  {};
virtual K_mf(void) OnMesRecvReady   (TMesBase*, size_t /*size*/) { CheckRecv(); }
virtual K_mf(void) CheckRecv        () = 0;
virtual K_mf(char*)GetState         () = 0;
public:

        K_ctor  TSesLibBase           (TSesMgr* aMgr);
        K_mf(TSesMgr*) GetOwner       ()  { return Owner_; }
        K_mf(void) StopTimer          ();
        K_mf(void) StartTimer         (unsigned secs, unsigned ms=0);
};
//---------------------------------------------------------------------------
};  // Bx
};  // Tw
};  // Kway
#endif
