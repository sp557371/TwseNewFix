//
// UNIX 實際同步機制的實作的地方
//    class TSyncObj::TSyncHandle
//
// 版本: $Id: SyncEv_UNIX.h,v 1.3 2005/08/09 07:14:01 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SyncEv_UNIXH
#define SyncEv_UNIXH
//---------------------------------------------------------------------------
#include "SyncEv.h"
#include <pthread.h>
/////////////////////////////////////////////////////////////////////////////
namespace Kway {
//---------------------------------------------------------------------------
class TSyncObj::TSyncHandle
{
   virtual K_mf(bool)  SyncExec  () = 0;
   union {
      int      SyncPipeFDS_[2];
      struct {
         int   ReadFD_;
         int   WriteFD_;
      };
   };
// ost::Thread*   MainThread_;
   pthread_t      MainThreadId_;
   long           RdCount_, WrCount_;

   friend class TSyncObj;
public:
           K_ctor  TSyncHandle ();
   virtual K_dtor ~TSyncHandle ();

   K_mf(void)  SyncStart     (bool wait);
   K_mf(bool)  InMainThread  ();

   K_mf(void)  ProcessSync   ();//請參考 TSyncObj::ProcessSync() 的說明
};
//---------------------------------------------------------------------------
} // namespace Kway
/////////////////////////////////////////////////////////////////////////////
#endif
