#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxSes.h"
#include "MesTCPClientTSE.h"
#include "FTSysSetting.h" 
//#include "FileLog.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx;
//---------------------------------------------------------------------------
const char Kway::Tw::Bx::SMsg::ACode_LineOpened [3] = "01";
const char Kway::Tw::Bx::SMsg::ACode_LineClosed [3] = "02";
const char Kway::Tw::Bx::SMsg::ACode_APPReady   [3] = "03";
const char Kway::Tw::Bx::SMsg::ACode_ApTimeout  [3] = "04";
const char Kway::Tw::Bx::SMsg::ACode_HBTimeout  [3] = "05";
const char Kway::Tw::Bx::SMsg::ACode_LinkTimeout[3] = "06";
//---------------------------------------------------------------------------
const char SStatusStr::Processing = 'N';
const char SStatusStr::Linked     = 'L';
const char SStatusStr::Retrying   = 'P';
//---------------------------------------------------------------------------
const std::string TWorker::WorkerErrMsg = std::string("Exception! Can't find the worker!");
//---------------------------------------------------------------------------
K_ctor TbxSesBase::TbxSesBase(TbxFTSesMgr& aMgr)
    : inherited(static_cast<Kway::TSesMgr*>(&aMgr)),
    SesMgr_(&aMgr)
{
}
//---------------------------------------------------------------------------
K_mf(int) TbxSesBase::GetPacketSize(TbxData& aPkt)
{
  TWorker* worker = Workers_.GetWorker(aPkt.GetWorkKey());
  return (worker ? worker->GetPacketSize(*this, aPkt):0);
}
//---------------------------------------------------------------------------
K_mf(bool) TbxSesBase::APacket(TbxData& aPkt)
{
  TWorker* worker = Workers_.GetWorker(aPkt.GetWorkKey());
  return (worker ? worker->APacket(*this, aPkt):false);
}
//---------------------------------------------------------------------------
K_mf(int) TbxSesBase::BeforeAPacket(TbxData& aPkt)
{
  TWorker* worker = Workers_.GetWorker(aPkt.GetWorkKey());
  return (worker ? worker->BeforeAPacket(*this, aPkt):0);
}
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////






