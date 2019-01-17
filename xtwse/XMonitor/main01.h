//========================================================================
//
// main01.h.cpp
//
//========================================================================
#ifndef main01H
#define main01H
//========================================================================
#include "ExcMgr.h"
#include "ExtDef.h"
#include "ProcMon.h"
#include "SignonSvrDB.h"
//========================================================================
using namespace Kway;
using namespace Kway::Tw::Stk;
//========================================================================
class Main01
{
   typedef Main01 this_type;
private:
   TExcMgr*                 ExcMgr_;
   Kway::TViewAreaMgr       ProcMonVA_;
   TProcMon*                ProcMon_;

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
