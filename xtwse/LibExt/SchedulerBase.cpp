//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
#include "SchedulerBase.h"

//---------------------------------------------------------------------------
TSchedulerBase* SchedulerBase;
//---------------------------------------------------------------------------
K_fn(TSchedulerBase*&)  GetScheduler  (){
   return SchedulerBase;
}
//---------------------------------------------------------------------------
K_fn(void)  SetScheduler  (TSchedulerBase* sb){
   SchedulerBase = sb;
}
/////////////////////////////////////////////////////////////////////////////

