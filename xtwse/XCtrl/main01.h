//========================================================================
//
// main01.h.cpp
//
//========================================================================
#ifndef main01H
#define main01H
//========================================================================
//#include "AppiaSesMgr.h"
#include "SignonSvrDB.h"
#include "ExcMgr.h"
#include "MsgCenter.h"
#include "ExtDef.h"
#include "THConf.h"
#include "TwseIniFile.h"
//========================================================================
using namespace Kway;
using namespace Kway::Tw::Stk;
//========================================================================
class Main01
{
   typedef Main01 this_type;
private:
   TExcMgr*                   ExcMgr_;
   TMsgTable*                 MsgC_;
   Kway::TViewAreaMgr         SysSettingVA_;
   Kway::TViewAreaMgr         ProcMonVA_;
   Kway::Twse::TBrkConfPage*  BrkConfPage_;
   TTwseMainIni*              MainIni_;

   K_mf(void) SetVA();

protected:
   Kway::byte            HostID_;
   bool                  UseHostID_;
   bool                  UseExcMgr_;
   bool                  UseEvLog_;
public:

   K_ctor Main01(Kway::Signon::TSignonSvrDB*);
   K_dtor ~Main01();

};
//---------------------------------------------------------------------------
#endif
