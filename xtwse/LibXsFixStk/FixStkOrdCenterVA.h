/////////////////////////////////////////////////////////////////////////////
#if !defined(_FixStkOrdCenterVA_h_)
#define _FixStkOrdCenterVA_h_
//---------------------------------------------------------------------------
#include "ViewAreaMgr.h"
#include "FixStkOrdCenter.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace FIX  {
namespace Stk  {
//---------------------------------------------------------------------------
class TTradeInfo;

/////////////////////////////////////////////////////////////////////////////
// class TFsOrdCenterVA
/////////////////////////////////////////////////////////////////////////////
class TFsOrdCenterVA : public TVACreater
{
   typedef TFsOrdCenter::TFsSysOrds  TFsSysOrds;

private:
   class TSysOrdsFm;
   
   TFsSysOrds*  SysOrds_;

public:
   K_ctor TFsOrdCenterVA (TTradeInfo*);

protected:
   //==============================================
   // overwrite TVACreater virtual function
   K_mf(TaViewArea) CreateNewVA ();
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Stk
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_FixStkOrdCenterVA_h_)
