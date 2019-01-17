/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_SimpThread_h_)
#define _Nsa_SimpThread_h_
//---------------------------------------------------------------------------
#include <cc++/thread.h>
#include "libnsAux.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TSimpThread
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TSimpThread : protected ost::Thread
{
   typedef ost::Thread  inherited;

protected:
   bool  StopThread_;
   bool  Started_;

public:
   K_ctor  TSimpThread ();
          ~TSimpThread ();

   //==============================================
   // TSimpThread function
   K_mf(bool) Start ();
   K_mf(void) Stop  ();

   inline K_mf(bool) IsStarted ()  { return Started_; }

protected:
   //==============================================
   // overwrite ost::Thread virtual function
   void run (void);

   //==============================================
   // TSimpThread virtual function
   virtual K_mf(int)  Execute () = 0;
   virtual K_mf(bool) BfRun   ()  { return true; }
   virtual K_mf(void) AfRun   ()  {}
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_SimpThread_h_)
