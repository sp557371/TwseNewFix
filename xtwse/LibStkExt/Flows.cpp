//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
#include "Flows.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Stk;
//---------------------------------------------------------------------------
TExtFlows* ExtFlows;
//---------------------------------------------------------------------------
K_fn(TExtFlows*)  GetExtFlows  (){
   return ExtFlows;
}
//---------------------------------------------------------------------------
K_fn(void)  SetExtFlows  (TExtFlows* ef){
   ExtFlows = ef;
}
/////////////////////////////////////////////////////////////////////////////

