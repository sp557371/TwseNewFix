/////////////////////////////////////////////////////////////////////////////
#if !defined(_CtrlSesMgr_h_)
#define _CtrlSesMgr_h_
//---------------------------------------------------------------------------
#include "SesMgrR.h"
#include "ViewAreaMgr.h"
#include "TimerThread.h"
#include "SignonSvrDB.h"
#include "TwStkOrdFlow.h"
#include "CtrlRsReqResu.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   class TCtrlSesAppia;

/////////////////////////////////////////////////////////////////////////////
// class TCtrlSesMgr
/////////////////////////////////////////////////////////////////////////////
class TCtrlSesMgr : public Kway::TSesMgrR
{
   typedef Kway::TSesMgrR              inherited;
   typedef TCtrlSesMgr                 this_type;
   typedef Kway::TViewAreaMgr          TViewAreaMgr;
   typedef Kway::Signon::TSignonSvrDB  TSignonSvrDB;
   typedef Kway::TTimer<this_type>     TOTimer;
   typedef std::map<std::string, TCtrlSesAppia*> TAppGrpMap;
   typedef TAppGrpMap::iterator        TAppGrpMapi;
   typedef std::map<std::string,TProcResu> TAppGrpProc;
   typedef TAppGrpProc::iterator       TAppGrpProci;
   typedef TCtrlRsReqResu::TRecsHnd    TRsReqResuHnd;
   typedef std::map<std::string, std::string> TSesMap;
   typedef TSesMap::iterator           TSesMapi;

   //TSignonSvrDB*      SignonDB_;
   TViewAreaMgr       OrdVA_;
   TOTimer*           Timer_;
//   TCtrlSesAppia*     SesAppia_;
   TCtrlRsReqResuGoh  RsReqResuGoh_;
   TReqID             ReqID_;
   TAppGrpMap         AppGrpMap_;
   TAppGrpProc        AppGrpProc_;
   TSesMap            SesMap_;
   TStrings           AppiaGrps_;

public:
   K_ctor  TCtrlSesMgr ();
   K_dtor ~TCtrlSesMgr ();

   K_mf(void) SetAppiaSes (std::string appgrp,TCtrlSesAppia* p); 

   //inline K_mf(TSignonSvrDB*)  GetSignonDB  () const  { return SignonDB_; }
   inline K_mf(TViewAreaMgr*)  GetVAMgr     ()        { return &OrdVA_;   }
   K_mf(TCtrlSesAppia*) GetAppiaSes  (std::string);
   K_mf(TReqID) FindNewReq(std::string);
   K_mf(void)   SetAppGrpState(std::string,TProcResu);
   K_mf(TProcResu) GetAppGrpState(std::string);
   K_mf(void)   ResetReqID(){ReqID_.clear();}
private:
   K_mf(void) OnTimer (Kway::TTimerBase*);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_CtrlSesMgr_h_)
