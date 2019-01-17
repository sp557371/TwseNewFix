//---------------------------------------------------------------------------
#pragma hdrstop

#include "SysMgrObjs.h"
#include "MesRegs.h"
#include "OrdSesMgrBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//========================================================================
using namespace Kway;
/////////////////////////////////////////////////////////////////////////////
static TOrdSesMgrBase* OrdSesMgrBase;
//---------------------------------------------------------------------------
K_fn(TOrdSesMgrBase*) GetOrdSesMgr()
{
   return OrdSesMgrBase;
}
//---------------------------------------------------------------------------
K_fn(void) SetOrdSesMgr(TOrdSesMgrBase* OrdSesMgr)
{
      OrdSesMgrBase = OrdSesMgr;
}
//---------------------------------------------------------------------------
K_ctor TOrdSesMgrBase::TOrdSesMgrBase(TSysInfoPage* sysinfo,TAutoClOrdGrpMgr*  clidmgr)
   :TSesMgrR("./Logs/%4y%2m%2d/ModifyLog/OrdSesMgr.log")
{
   SysInfo_     = sysinfo;
   ClOrdGrpMgr_ = clidmgr;
   const TSysMgrObjs&   sys = GetSysMgrObjs();
   OrdVA_.Add("委託連線設定", GetSettingVACreater());
   OrdVA_.Add("委託連線狀態", GetStatusVACreater());
   sys.SysVAMgr_->Add("委託連線管理員", OrdVA_);
}
//---------------------------------------------------------------------------
K_dtor TOrdSesMgrBase::~TOrdSesMgrBase()
{
   const TSysMgrObjs&   sys = GetSysMgrObjs();
   sys.SysVAMgr_->Del(OrdVA_);
   OrdVA_.Del(GetSettingVACreater());
   OrdVA_.Del(GetStatusVACreater());
   SetOrdSesMgr(0);
}
//---------------------------------------------------------------------------
