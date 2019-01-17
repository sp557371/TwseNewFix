/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "CtrlSesIniter.h"
#include "ModuleLoader.h"
#include "SysMgrObjs.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// class TCtrlSesIniter impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TCtrlSesIniter::TCtrlSesIniter ()
   : RsReqResuGoh_(false)
{
   const TSysMgrObjs& sys = GetSysMgrObjs();
   TViewAreaMgr*      vaMgr;
   
   RsReqResuGoh_.Initialize();
   
   CtrlSesMgr_ = new TCtrlSesMgr;
   vaMgr       = CtrlSesMgr_->GetVAMgr();

   vaMgr->Add(std::string("3.Tandem資料表"), RsReqResuGoh_->GetVac());

   sys.MainVAMgr_->Add(std::string("5.主機連線管理員"), *vaMgr);
}
//---------------------------------------------------------------------------

K_dtor TCtrlSesIniter::~TCtrlSesIniter ()
{
   const TSysMgrObjs& sys   = GetSysMgrObjs();
   TViewAreaMgr*      vaMgr = CtrlSesMgr_->GetVAMgr();

   sys.MainVAMgr_->Del(*vaMgr);
   vaMgr->Del(RsReqResuGoh_->GetVac());

   delete CtrlSesMgr_;
}


/////////////////////////////////////////////////////////////////////////////
// Module Loader Function
/////////////////////////////////////////////////////////////////////////////
K_exportC(Kway::TnsmStartObj) nsModuleStart ()
{
   TCtrlSesIniter* Initer = new TCtrlSesIniter;

   return (Kway::TnsmStartObj)Initer;
}
//---------------------------------------------------------------------------

K_exportC(const char*) nsModuleDesp ()
{
   return "XCtrl Ses Module";
}
//---------------------------------------------------------------------------

K_exportC(bool) nsModuleFinish (Kway::TnsmStartObj StartObj)
{
   TCtrlSesIniter* Initer = reinterpret_cast<TCtrlSesIniter*>(StartObj);
   delete Initer;
   return true;
}
//---------------------------------------------------------------------------
