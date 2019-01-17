#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxRpt.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw;
using namespace Kway::Tw::bx;
/////////////////////////////////////////////////////////////////////////////
K_ctor TR1::TR1(TSubSysName subsys)
{
   SubSysName_ = subsys;//.assign("50");
   FunctionCode_.assign("00");
   MessageType_.assign("00");
   StatusCode_.assign("00");
}
/////////////////////////////////////////////////////////////////////////////
K_ctor TR5::TR5(TSubSysName subsys)
{
   SubSysName_ = subsys;//.assign("50");
   FunctionCode_.assign("00");
   MessageType_.assign("05");
   StatusCode_.assign("00");
}
