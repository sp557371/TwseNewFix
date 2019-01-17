/** @file nsinit.h
@brief libns的啟動及結束機制
*/
//---------------------------------------------------------------------------
#ifndef nsinitH
#define nsinitH
//---------------------------------------------------------------------------
#include "KwayList.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TNSIniter;
LIBNS_class TNSCleanup;
//---------------------------------------------------------------------------
/** @brief 將libns初始化.

在您使用任何一個 libns 的物件之前, 您必須先建立一個 TNSIniter,
如果您不確定是否已經有人建立了 TNSIniter 也沒關係, 您可以重複建立,
因為只有當最後一個 TNSIniter 解構時, 才會將執行 libns 的清理程序.
*/
LIBNS_class TNSIniter
{
   friend class TNSCleanup;
   class THolder;
   static K_mf(THolder&)  GetHolder  ();
   K_mf(void)  Init  ();
public:
           K_ctor  TNSIniter  (int argc, char* argv[]);
           K_ctor  TNSIniter  ();
   virtual K_dtor ~TNSIniter  ();

   enum TNSState {
      ns_Running,
      ns_Cleaning,
      ns_Cleaned,
   };
   static K_mf(TNSState)  GetNSState  ();
};
//---------------------------------------------------------------------------
/** @brief libns的清理程序.

當最後一個 TNSIniter 死掉時, 會呼叫所有的 TNSCleanup 的 OnNSCleanup(),
所以如果您要在 libns 結束前執行一些清理的工作時, 只要繼承 TNSCleanup,
然後覆寫 OnNSCleanup() 就可以達到目的了.
越晚建構的 TNSCleanup, 會越早執行 OnNSCleanup().
*/
LIBNS_class TNSCleanup : public TNodeBase
{
   friend class TNSIniter;
   friend class TNSIniter::THolder;
   virtual K_mf(void) OnNSCleanup   () = 0;
   virtual K_mf(void) OnNSRestartup () {};

   inline K_mf(TNSCleanup*)  GetNext  ()  { return static_cast<TNSCleanup*>(TNodeBase::GetNext()); }
protected:
           K_ctor  TNSCleanup  ();
   virtual K_dtor ~TNSCleanup  ();
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
