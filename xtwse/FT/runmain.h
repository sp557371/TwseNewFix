#ifndef RUNMAINH
#define RUNMAINH

#include "KwayBase.h"
#include "bxSesMgr.h"
#include "ReqMgr.h"
#include "NotifyMgr.h"
#include "FTCenter.h"
#include "FTSchedule.h"
#include "SesMgr.h"
#include "SysMgrObjs.h"
#include "FTSysSetting.h"
#include "DspMgr.h"
#include "ExcMgr.h"
#include "MsgCenter.h"
//---------------------------------------------------------------------------
K_class TRunMain
{
   typedef TRunMain   this_type;
private:
   TExcMgr*        ExcMgr_;
   TMsgTable*      MsgC_;
protected:
        K_mf(void) Init();
        K_mf(void) Free();
public:
   	K_ctor  TRunMain();
   	K_dtor ~TRunMain();
};
//---------------------------------------------------------------------------
#endif
