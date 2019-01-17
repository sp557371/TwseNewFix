//
// ViewArea Client - Session
//
// 版本: $Id: SesVAC.h,v 1.2 2005/08/09 05:48:36 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SesVACH
#define SesVACH
//---------------------------------------------------------------------------
#include "SesClient.h"
#include "ViewAreaMgr.h"
#include "ShadowVA.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Pk {
//---------------------------------------------------------------------------
class TSesVAC;
/////////////////////////////////////////////////////////////////////////////
class TSesVACEv
{
   friend class TSesVAC;
   virtual K_mf(void)  OnSesCaption    (TSesVAC&, const std::string&) = 0;
   virtual K_mf(void)  OnSesVAMgrs     (TSesVAC&, const TStrings&) = 0;
   virtual K_mf(void)  OnSesState      (TSesVAC&, const std::string& reason) = 0;
   virtual K_mf(void)  OnSesBfDestroy  (TSesVAC&) = 0;
public:
   //提供給相關的處理 VA 的 class 產生此事件, 所以開放成public
   virtual K_mf(void)  OnResetVACr  (TSesVAC&, int layer, TVACreater*) = 0;
};
/////////////////////////////////////////////////////////////////////////////
class TSesVAC : public TSesClient
{
   typedef TSesClient   inherited;
   //---------------------------------------------------------------------------
   class TWorkerVA : public TWorker
   {
      K_mf(TCheckResult)  OnPkCheck  (TSesPk&, const THead&, TSize recvSize);
      K_mf(void)          OnPkRecv   (TSesPk&, const THead&);
   };
   friend class TWorkerVA;
   static TWorkerVA  VAWorker_;
   //---------------------------------------------------------------------------
   K_mf(void)  OnMesLinkBroken   (TMesBase*, const std::string&);
   K_mf(void)  OnMesLinkReady    (TMesBase*);
   K_mf(void)  OnStateChanged    (const std::string&);

   TSesVACEv*     Ev_;
   TShadowVACr    ShadowVACr_;
   word           ReqID_;
   bool           WaittingReq_;

   K_mf(void)          ClearVACr       ();
   K_mf(void)          OnPkVAAck       (const TSesVAAck&);
   K_mf(TShadowVACr*)  FindShadowVACr  (const TSesVAAck&, TSesVALayer);
protected:
   K_mf(void) OnSignon     (const std::string& result);
   K_mf(bool) SettingApply (const std::string&);

public:
   K_ctor  TSesVAC  (TSesVACEv*, TWorkersBase&, TVer, const std::string& apName, TOnlineClient&);
   K_dtor ~TSesVAC  ();

   K_mf(TVACreater&)       GetVAMgr  (const char* vaMgrName);
   inline K_mf(TSesVACEv*) GetEv     ()                  { return Ev_; }
   inline K_mf(void)       SetEv     (TSesVACEv* ev)     { Ev_ = ev;   }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Pk
} // namespace Kway
//---------------------------------------------------------------------------
#endif

