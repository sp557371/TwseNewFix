//========================================================================
//
// main01.h.cpp
//
//========================================================================
#ifndef main01H
#define main01H
//========================================================================
//#include "OrdSesMgr.h"
//#include "OrdScheduler.h"
//#include "SysInfo.h"
#include "FixSesMgr.h"
//========================================================================
using namespace Kway;
//using namespace Kway::Tw::Stk;
//========================================================================
class Main01
{
   typedef Main01 this_type;
   typedef Kway::TTimer<this_type>  TOTimer;
private:
   //TOrdSesMgr*                      OrdSesMgr_;
   Kway::Tw::Fix::Stk::TFixSesMgr*  FixSesMgr_;
   
   //TOTimer*  Timer_;
   //K_mf(void)  OnTimer(TTimerBase*);

   K_mf(void) SetVA();

protected:
public:

   K_ctor Main01(Kway::Signon::TSignonSvrDB*);
   K_dtor ~Main01();
   K_mf(std::string) DateIntoPath(char* DirName,bool head);
};
//---------------------------------------------------------------------------
#endif
