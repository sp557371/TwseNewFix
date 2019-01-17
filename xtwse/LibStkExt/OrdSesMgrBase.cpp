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
   OrdVA_.Add("�e�U�s�u�]�w", GetSettingVACreater());
   OrdVA_.Add("�e�U�s�u���A", GetStatusVACreater());
   sys.SysVAMgr_->Add("�e�U�s�u�޲z��", OrdVA_);
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
