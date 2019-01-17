//---------------------------------------------------------------------------
#ifndef OrdSesMgrBaseH
#define OrdSesMgrBaseH
//---------------------------------------------------------------------------
#include "SesMgrR.h"
#include "ViewAreaMgr.h"
#include "TwStkOrdFlow.h"
#include "TwStkOrdCenter.h"
#include "TwStkClOrdIDMgr.h"
#include "SysInfo.h"
#include "SignonSvrDB.h"
//---------------------------------------------------------------------------
K_class TOrdSesMgrBase : public Kway::TSesMgrR
{
typedef Kway::TSesMgrR             inherited;
typedef Kway::TViewAreaMgr         TViewAreaMgr;
typedef TOrdSesMgrBase             this_type;
typedef Kway::Tw::Stk::TOrdFlow    TOrdFlow;
typedef Kway::Tw::Stk::TOrdCenter  TOrdCenter;
typedef Kway::Signon::TSignonSvrDB TSignonSvrDB;
private:
protected:
   TViewAreaMgr            OrdVA_;
   TSysInfoPage*           SysInfo_;
   TAutoClOrdGrpMgr*       ClOrdGrpMgr_;
   TOrdFlow*               OrdFlow_;
   TOrdCenter*             OrdCenter_;
   TSignonSvrDB*           SignonDB_;
public:
   K_ctor                  TOrdSesMgrBase(TSysInfoPage*, TAutoClOrdGrpMgr*);
   K_dtor                  ~TOrdSesMgrBase();
   K_mf(TViewAreaMgr*)     GetVAMgr()            { return &OrdVA_;}
   K_mf(TSysInfoPage*)     GetSysInfo()    const { return SysInfo_; }
   K_mf(TAutoClOrdGrpMgr*) GetClOrdIDMgr() const { return ClOrdGrpMgr_;}
   K_mf(TOrdFlow*)         GetOrdFlow()    const { return OrdFlow_;}
   K_mf(TOrdCenter*)       GetOrdCenter()  const { return OrdCenter_;}
   K_mf(TSignonSvrDB*)     GetSignonDB()   const { return SignonDB_;}
};
//---------------------------------------------------------------------------
K_fn(TOrdSesMgrBase*) GetOrdSesMgr();
K_fn(void)            SetOrdSesMgr(TOrdSesMgrBase*);
//---------------------------------------------------------------------------
#endif
