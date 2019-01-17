//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "PMachSes.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw::Stk;
using namespace Kway::Tw::Bx;
using namespace Kway::Tw::Bx::Rpt;
//---------------------------------------------------------------------------
K_ctor TPMachSes::TPMachSes   (TSesMgr* sm)
   : inherited(sm)
{
   SesMgr_       = sm;
   Recovering_   = false;
   LastSeqNo_    = 0;
   RecoverTimer_ = new TRecoverTimer(this);
   IsLn_         = false;
}
//---------------------------------------------------------------------------
K_dtor TPMachSes::~TPMachSes   ()
{
   ((TbxFTSesMgr*)SesMgr_)->DelPMachSes((TSesBase*)this, IsLn_);
   delete RecoverTimer_;
   Pmach_.Close();
}
//---------------------------------------------------------------------------
K_mf(void) TPMachSes::ResetRTimer(int interval)
{
   RecoverTimer_->Stop();
   RecoverTimer_->Start(interval*1000, &this_type::RecoverTimeOut);
}
//---------------------------------------------------------------------------
K_ctor TPMachSesLN::TPMachSesLN   (TSesMgr* sm)
   : inherited(sm)
{
   SesMgr_       = sm;
   Recovering_   = false;
   LastSeqNo_    = 0;
   RecoverTimer_ = new TRecoverTimer(this);
   IsLn_         = true;
}
//---------------------------------------------------------------------------
K_dtor TPMachSesLN::~TPMachSesLN   ()
{
   ((TbxFTSesMgr*)SesMgr_)->DelPMachSes((TSesBase*)this, IsLn_);
   delete RecoverTimer_;
   Pmach_.Close();
}
//---------------------------------------------------------------------------
K_mf(void) TPMachSesLN::ResetRTimer(int interval)
{
   RecoverTimer_->Stop();
   RecoverTimer_->Start(interval*1000, &this_type::RecoverTimeOut);
}
//---------------------------------------------------------------------------
