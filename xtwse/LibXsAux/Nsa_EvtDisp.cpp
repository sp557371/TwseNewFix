/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_EvtDisp.h"
#include <map>
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TEvtDisp::TProcHnd impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TEvtDisp::TProcHnd::TProcHnd (const char* dbgName)
{
   if (dbgName)  DbgName_.assign(dbgName);
}
//---------------------------------------------------------------------------

K_dtor TEvtDisp::TProcHnd::~TProcHnd ()
{
   DelEv();
}
//---------------------------------------------------------------------------

K_mf(void) TEvtDisp::TProcHnd::RegEv (const TEvtDispID* idList, size_t idListSize)
{
   TEvtDisp& evtDisp = TEvtDisp::GetInst();
   
   evtDisp.RegEvHnd(idList, idListSize, this);
}
//---------------------------------------------------------------------------

K_mf(void) TEvtDisp::TProcHnd::DelEv ()
{
   TEvtDisp& evtDisp = TEvtDisp::GetInst();

   evtDisp.DelEvHnd(this);
}
//---------------------------------------------------------------------------

K_mf(TEvtDispResu) TEvtDisp::TProcHnd::SendEvent (TEvtDispID id, TEvtDispResu resu)
{
   SEvtDispPktHdr pktHdr(id, resu, sizeof(SEvtDispPktHdr));
   
   return SendEvent(pktHdr);
}
//---------------------------------------------------------------------------

K_mf(TEvtDispResu) TEvtDisp::TProcHnd::SendEvent (SEvtDispPktHdr& evInfo)
{
   TEvtDisp& evtDisp = TEvtDisp::GetInst();
   
   return evtDisp.SendEvent(evInfo);
}
//---------------------------------------------------------------------------

K_mf(void) TEvtDisp::TProcHnd::PostEvent (TEvtDispID id, TEvtDispResu resu)
{
   SEvtDispPktHdr pktHdr(id, resu, sizeof(SEvtDispPktHdr));
   
   PostEvent(pktHdr);
}
//---------------------------------------------------------------------------

K_mf(void) TEvtDisp::TProcHnd::PostEvent (SEvtDispPktHdr& evInfo)
{
   TEvtDisp& evtDisp = TEvtDisp::GetInst();

   evtDisp.PostEvent(evInfo);
}


/////////////////////////////////////////////////////////////////////////////
// class TEvtDisp impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TEvtDisp::TEvtDisp ()
   : TimerQueue_(0)
   , Timer_     (this)
{
   Timer_.Start(10, &this_type::OnTimer);
}
//---------------------------------------------------------------------------

K_dtor TEvtDisp::~TEvtDisp ()
{
   Timer_.Stop();
}
//---------------------------------------------------------------------------

K_mf(void) TEvtDisp::RegEvHnd (const TEvtDispID* idList, size_t aeIDListSize, TProcHnd* hnd)
{
   for (size_t i=0; i<aeIDListSize; ++i) {
      TProcIDHndCtnI           ita        = ProcIDHndCtn_.fetch(idList[i]);
      TProcHndCtnPtr::ref_obj* procHndCtn = ita->second.testnew();

      procHndCtn->Add(hnd);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TEvtDisp::DelEvHnd (TProcHnd* hnd)
{
   for (TProcIDHndCtnI ita=ProcIDHndCtn_.begin(); ita!=ProcIDHndCtn_.end(); ++ita) {
      TProcHndCtnPtr::ref_obj* procHndCtn = ita->second.get();

      if (procHndCtn) {
         procHndCtn->Del(hnd);
      }
   }
}
//---------------------------------------------------------------------------

K_mf(TEvtDispResu) TEvtDisp::SendEvent (SEvtDispPktHdr& evInfo)
{
   return ReadyToFire(&evInfo);
}
//---------------------------------------------------------------------------

K_mf(void) TEvtDisp::PostEvent (SEvtDispPktHdr& evInfo)
{
   TimerQueue_.PutMemBlock(&evInfo, evInfo.Size_);
}
//---------------------------------------------------------------------------

class TEvtDisp::TEvtDispWorker
{
   SEvtDispPktHdr*  EvInfo_;
   TEvtDispResu     Resu_;

public:
   inline K_ctor TEvtDispWorker (SEvtDispPktHdr* evInfo)
      : EvInfo_(evInfo)
      , Resu_  (edr_None)
   {}

   K_mf(EEvObjResult) Exec (TEvtDisp::TProcHnd* procHnd)
   {
      TEvtDispResu resu = procHnd->OnEvtProc(EvInfo_);
      if (resu != eor_Continue) {
         Resu_ = resu;
         return eor_Finish;
      }
      return eor_Continue;
   }

   inline K_mf(TEvtDispResu) GetResu () const  { return Resu_; }
};
//---------------------------------------------------------------------------
K_mf(TEvtDispResu) TEvtDisp::ReadyToFire (SEvtDispPktHdr* evInfo)
{
   TProcIDHndCtnI iter = ProcIDHndCtn_.find(evInfo->ID_);
   if (iter == ProcIDHndCtn_.end()) {
      return evInfo->DefResu_;
   }

   TProcHndCtnPtr::ref_obj* procHndCtn = iter->second.get();
   if (procHndCtn == NULL) {
      return evInfo->DefResu_;
   }

   TEvtDispWorker worker(evInfo);
   TEvtDispResu   resu;
   
   procHndCtn->ForEachEv(worker);
   resu = worker.GetResu();
   if (resu == edr_None) {
      return evInfo->DefResu_;
   }
   return resu;
}
//---------------------------------------------------------------------------

K_mf(void) TEvtDisp::OnTimer (Kway::TTimerBase*)
{
   const void*  mem;
         size_t memLen = TimerQueue_.GetMemBlock(mem);

   if (memLen > 0) {
      SEvtDispPktHdr* evInfo = (SEvtDispPktHdr*)mem;
      // 這邊得到執行結果了, 要如何通知呢?
      //TAeResu    AeResu = ReadyToFire(EvInfo);
      ReadyToFire(evInfo);

      TimerQueue_.SetMemUsed(memLen);
   }
}
//---------------------------------------------------------------------------

K_mf(TEvtDisp&) TEvtDisp::GetInst ()
{
   static TEvtDisp evtDisp;
   return evtDisp;
}
//---------------------------------------------------------------------------

