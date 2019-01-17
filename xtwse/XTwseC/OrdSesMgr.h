/////////////////////////////////////////////////////////////////////////////
#if !defined(_OrdSesMgr_h_)
#define _OrdSesMgr_h_
//---------------------------------------------------------------------------
#include "SesMgrR.h"
#include "ViewAreaMgr.h"
#include "TimerThread.h"
#include "SignonSvrDB.h"
#include "TwStkOrdFlow.h"
#include "SysInfo.h"

/////////////////////////////////////////////////////////////////////////////
// class TOrdSesMgr
/////////////////////////////////////////////////////////////////////////////
class TOrdSesMgr : public Kway::TSesMgrR
{
   typedef Kway::TSesMgrR              inherited;
   typedef TOrdSesMgr                  this_type;
   typedef Kway::TViewAreaMgr          TViewAreaMgr;
   typedef Kway::Tw::Stk::TOrdFlow     TOrdFlow;
   typedef Kway::Tw::Stk::TOrdCenter   TOrdCenter;
   typedef Kway::Signon::TSignonSvrDB  TSignonSvrDB;
   typedef Kway::TTimer<this_type>     TOTimer;

   TOrdFlow*      OrdFlow_;
   TOrdCenter*    OrdCenter_;
   TSignonSvrDB*  SignonDB_;
   TViewAreaMgr   OrdVA_;
//   TSysInfoPage*  SysInfo_;
   TOTimer*       Timer_;
   bool           IsDR_;

public:
   K_ctor TOrdSesMgr (/*TSysInfoPage*, */TOrdFlow*, TSignonSvrDB*);

   inline K_mf(TOrdFlow*)     GetOrdFlow   ()        { return OrdFlow_;   }
   inline K_mf(TOrdCenter*)   GetOrdCenter ()        { return OrdCenter_; }
   inline K_mf(TSignonSvrDB*) GetSignonDB  () const  { return SignonDB_;  }
//   inline K_mf(TSysInfoPage*) GetSysInfo   () const  { return SysInfo_;   }
   inline K_mf(TViewAreaMgr*) GetVAMgr     ()        { return &OrdVA_;    }

private:
   K_mf(void) OnTimer (TTimerBase*);
   // 在啟動 Appia 連線的檢查
   K_mf(bool) PreChkNormal ();
   K_mf(bool) PreChkDR     ();
};

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_OrdSesMgr_h_)

