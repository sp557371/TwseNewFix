//---------------------------------------------------------------------------
#ifndef MesIPC_UnixH
#define MesIPC_UnixH
//---------------------------------------------------------------------------
#include "MesIPC.h"
#include "KFile.h"
#include "EvDev.h"
#include "TimerThread.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
class TMesIPC::TImpl : public TMesIPC_ImplBase
                     , public TAutoEvFD // pseudo-terminal fd
                     , TTimerBase
{
   typedef TMesIPC_ImplBase   inherited;
   pid_t    ProcessID_;

   K_mf(void) CloseHandles (EProcessState ps, const std::string& msg, int* st);
   K_mf(bool) OpenProcess  ();

   K_mf(void)      OnTimer   ();
   K_mf(EEvResult) OnEvRead  (TFD, TEventsDev*);
   K_mf(EEvResult) OnEvWrite (TFD, TEventsDev*);
public:
   inline K_ctor TImpl(TMesIPC* owner)
      : inherited(owner)
      , TTimerBase(ots_MainThread)
      , ProcessID_(0)
      { }
   inline K_dtor ~TImpl() {
      CloseProcess();
   }
   K_mf(bool) OpenProcess  (const TProcessSet&);
   K_mf(bool) CloseProcess ();
   K_mf(int)  SendStdin    (const void* buffer, size_t size);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
