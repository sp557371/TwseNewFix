/////////////////////////////////////////////////////////////////////////////
#if !defined(_OrdSesMgr_h_)
#define _OrdSesMgr_h_
//---------------------------------------------------------------------------
#include "SesMgrR.h"
#include "ViewAreaMgr.h"
#include "TimerThread.h"
#include "SignonSvrDB.h"
#include "TimerThread.h"
/////////////////////////////////////////////////////////////////////////////
// class TOrdSesMgr
/////////////////////////////////////////////////////////////////////////////
class TOrdSesMgr : public Kway::TSesMgrR
{
   typedef Kway::TSesMgrR              inherited;
   typedef TOrdSesMgr                  this_type;
   typedef Kway::TViewAreaMgr          TViewAreaMgr;
   typedef Kway::Signon::TSignonSvrDB  TSignonSvrDB;
   typedef Kway::TTimer<this_type>     TOTimer;

   TSignonSvrDB*  SignonDB_;
   TViewAreaMgr   OrdVA_;
   TOTimer*       Timer_;

public:
   K_ctor TOrdSesMgr (TSignonSvrDB*);

   inline K_mf(TSignonSvrDB*) GetSignonDB  () const  { return SignonDB_;  }
   inline K_mf(TViewAreaMgr*) GetVAMgr     ()        { return &OrdVA_;    }

private:
   K_mf(void) OnTimer (Kway::TTimerBase*);
};

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_OrdSesMgr_h_)

