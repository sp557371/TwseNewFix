//---------------------------------------------------------------------------
#ifndef MesIPC_WinH
#define MesIPC_WinH
//---------------------------------------------------------------------------
#include "MesIPC.h"
#include "BCB/WinPipe.h"
#include "KFile.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
class TMesIPC::TImpl : public TMesIPC_ImplBase
{
   typedef TMesIPC_ImplBase   inherited;

   PROCESS_INFORMATION  ProcessInfo_;
   TPipe                PipeStdin_;
   TPipe                PipeStdout_;

   class TProcessThread;
   friend class TProcessThread;
   TProcessThread*   Thread_;
   K_mf(void) Thr_Execute (HANDLE endEvent);

   K_mf(bool) OpenProcess ();
   K_mf(int)  ReadStdout  ();
   K_mf(bool) CloseProcessQuery ();
   K_mf(bool) CloseProcessWait  ();
   K_mf(bool) CloseHandles      (EProcessState, const std::string& msg, bool showExitCode);
public:
   inline K_ctor TImpl(TMesIPC* owner)
      : inherited(owner)
      , Thread_(NULL)
      { ZeroMemory(&ProcessInfo_, sizeof(ProcessInfo_));
      }

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
