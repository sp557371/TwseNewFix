/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FixStkOrdCenter.h"
#include "FixStkOrdCenter_Aux.h"
#include "tplns/Nsa_EvCtn_1k_VA.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::FIX;
using namespace Kway::FIX::Stk;

/////////////////////////////////////////////////////////////////////////////
// class TFsOrdCenter impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TFsOrdCenter::TFsOrdCenter (const std::string& logPath)
   : SysOrdsInn_     (NULL)
   , SysOrdsRecorder_(NULL)
   , SysOrdsVAC_     (NULL)
{
   char fullPath[256];

   sprintf(fullPath, "%sSysOrds.bin", logPath.c_str());
   //**********************************************
   #if !defined(_FixStkVer_Fastest_)
   SysOrdsInn_      = new inn_type        (std::string(fullPath), SysOrdRecSz, false);
   SysOrdsRecorder_ = new TSysOrdsRecorder(&SysOrds_, SysOrdsInn_);
   #endif
   //**********************************************
}
//---------------------------------------------------------------------------

K_dtor TFsOrdCenter::~TFsOrdCenter ()
{
   if (SysOrdsVAC_)       delete SysOrdsVAC_;
   if (SysOrdsRecorder_)  delete SysOrdsRecorder_;
   if (SysOrdsInn_)       delete SysOrdsInn_;
}
//---------------------------------------------------------------------------

K_mf(bool) TFsOrdCenter::GetOrd (const TFixSysOrdID& sysOrdID, TFsSysOrds::iterator& iter)
{
   iter = SysOrds_.find(sysOrdID);
   if (iter == SysOrds_.end())
      return false;
   return true;
}
//---------------------------------------------------------------------------

K_mf(TFsOrdCenter::TOrdHnd) TFsOrdCenter::GetOrdHnd (TFixSysOrdID sysOrdID, TFixUpdFrom uf)
{
   TFsSysOrds::iterator iter;

   if (GetOrd(sysOrdID, iter))
      return SysOrds_.begin_update(iter, uf);
   return TOrdHnd();
}
//---------------------------------------------------------------------------
