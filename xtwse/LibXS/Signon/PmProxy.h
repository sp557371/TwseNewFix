#include "Signon/PmBase.h"

namespace Kway { namespace Signon { namespace Admin2 {
/////////////////////////////////////////////////////////////////////////////
class TPmProxy : public TPmBaseVA
{
   TPmBaseVA*  PmDest_;
public:
   K_ctor TPmProxy(TPmBaseVA& pm)
      : PmDest_(&pm)
   {
   }
   K_dtor ~TPmProxy() {
      delete PmDest_;
   }

   virtual K_mf(TPolicyBaseRef) GetPolicy (const TPolicyKey& key, bool forLocalUse) const {
      return PmDest_->GetPolicy(key, forLocalUse);
   }
   virtual K_mf(bool) AddOnUpdate (const TPolicyKey& key, TOnPmUpdate& upr) {
      return PmDest_->AddOnUpdate(key, upr);
   }
   virtual K_mf(bool) DelOnUpdate (const TPolicyKey& key, TOnPmUpdate& upr) {
      return PmDest_->DelOnUpdate(key, upr);
   }
   virtual K_mf(const char*) GetPolicyName () const {
      return PmDest_->GetPolicyName();
   }

   virtual K_mf(void) OnBfVAMgrDestroy (const TViewAreaMgr& vamgr) {
      //PmDest_->OnBfVAMgrDestroy(vamgr);//PmDest_ 不會進入 VAMgr, 所以不用轉呼叫!
      delete this;
   }
   virtual K_mf(TaViewArea) CreateNewVA() {
      return PmDest_->CreateNewVA();
   }
   virtual K_mf(size_t) GetDetails (const char**& a) {
      return PmDest_->GetDetails(a);
   }
   virtual K_mf(TVACreater*) GetDetailVACreater (size_t detailID) {
      return PmDest_->GetDetailVACreater(detailID);
   }
   virtual K_mf(TVACreater*) OnMasterViewRowMoved (TViewArea& masterVA, TViewArea*& detailVA, size_t detailID) {
      return PmDest_->OnMasterViewRowMoved(masterVA, detailVA, detailID);
   }
};
/////////////////////////////////////////////////////////////////////////////
} } }
