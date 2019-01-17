//========================================================================
//
// main01.h.cpp
//
//========================================================================
#ifndef main01H
#define main01H
//========================================================================
#include "OrdSesMgr.h"
#include "ExcMgr.h"
#include "MsgCenter.h"
#include "bxSesMgr.h"
#include "TwseIniFile.h"
//========================================================================
using namespace Kway;
using namespace Kway::Tw::Stk;
//========================================================================
class Main01
{
   typedef Main01 this_type;
private:
   TOrdSesMgr               *OrdSesMgr_;
   Kway::Tw::bx::TbxSesMgr* bxSesMgr_;
   TExcMgr*                 ExcMgr_;
   TMsgTable*               MsgC_;
   Kway::TViewAreaMgr       SysSettingVA_;
   TSegDefs*                SegDefs_;
   TTwseMainIni*            MainIni_;

   K_mf(void) SetVA();

protected:
   Kway::byte            HostID_;
   bool                  UseHostID_;
   bool                  UseClOrdIDMgr_;
   bool                  UseExcMgr_;
   bool                  UseEvLog_;
public:
   K_ctor Main01(Kway::Signon::TSignonSvrDB*);
   K_dtor ~Main01();
};
//---------------------------------------------------------------------------
#endif
