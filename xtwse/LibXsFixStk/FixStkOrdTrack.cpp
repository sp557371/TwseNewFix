/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FixStkOrdTrack.h"
#include "Nsa_UtilStr.h"
#include "Nsa_UtilComm.h"
#include "tplns/Nsa_EvMap_0File.h"
//#include "tplns/Nsa_EvCtn_0File.h"
#include "FixStkPer.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;
using namespace Kway::FIX;
using namespace Kway::FIX::Stk;

/////////////////////////////////////////////////////////////////////////////
// class TOrdTrack::TRecsRecorder
/////////////////////////////////////////////////////////////////////////////
/*
class TOrdTrack::TRecsRecorder
{
   struct TStore
   {
      template <class TPersistent, class TStorage, class TKey, class TRec>
      K_mf(bool) exec (TPersistent& per, TStorage& storage, TKey& key, TRec& rec)
      {
         //return storage.store(per, key)           &&
         //       storage.store(per, rec.SysOrdID_) &&
         //       storage.store(per, rec.OrigClOrdID_);
         return true;
      }
   };

   typedef TEvMapReco2d<TRsSysOrdID, TRsSeqNo, TStore>  TRecorder;
   
   TRecorder  Recorder_;


public:
   inline K_ctor TRecsRecorder (TRsSysOrdID* map1, const char* filePath)
      : Recorder_(map1, std::string(filePath))
   {}
};
*/
typedef TMapMaker_2k2d<TOrdTrack::TRsSysOrdID, TOrdTrack::TRsSeqNo>  TOrdTrackMapMaker;
//---------------------------------------------------------------------------
struct TOrdTrackRecoRoom
{
   typedef TOrdTrackMapMaker::key_type  TKey;
   typedef SOrdTrack                    TRec;
   typedef TCommInn                     TInn;

   //**********************************************
   #if !defined(_FixTag_Twse_Ver2_)
   Kstl::persistent_auto  PerSave_;

   inline K_ctor TOrdTrackRecoRoom ()
      : PerSave_(1024*10)
   {}
   #endif
   //**********************************************

   K_mf(bool) exec_peek_room (TKey&, TRec&, TInn::rr_room&);
   K_mf(bool) exec_into_room (TInn::rr_room&, TInn::rr_key&, const TKey&, const TRec&);
};
//---------------------------------------------------------------------------
class TOrdTrack::TRecsRecorder : public Nsa::TSimpRecoDeta<TOrdTrackMapMaker, TOrdTrackRecoRoom>
{
   typedef Nsa::TSimpRecoDeta<TOrdTrackMapMaker, TOrdTrackRecoRoom>  inherited;

public:
   inline K_ctor TRecsRecorder (TRsSysOrdID* map1, const char* filePath)
      : inherited(map1, std::string(filePath))
   {}
};


/////////////////////////////////////////////////////////////////////////////
// class TOrdTrack::TRecsHnd impl
/////////////////////////////////////////////////////////////////////////////
K_mf(TOrdTrack::TRecsHnd::ENewStatus) TOrdTrack::TRecsHnd::FindNewStatus ()
{
   if (inherited::FindMap2() == NULL || inherited::MoveFirst2() == false)
      return ens_NotFound;
   
   bool found = false;
   
   do {
      const SOrdTrack&   cdOt = inherited::GetDat2(false);
            EFixOrdCmdID oid  = cdOt.OrdCmd_.get_orig();

      if (oid == fcid_New || fcid_RptNew) {
         found = true;
         
         if (cdOt.FlowID_.IsFinish())
            return ens_Sent;
      }
         
   } while (inherited::MoveNext2());

   if (found)
      return ens_Found;
   
   return ens_NotFound;
}
//---------------------------------------------------------------------------

K_mf(bool) TOrdTrack::TRecsHnd::FindFinished (const TFixMsgType2& msgType)
{
   if (inherited::FindMap2() == NULL || inherited::MoveFirst2() == false)
      return false;
   
   do {
      const SOrdTrack& cdOt = inherited::GetDat2(false);

      if (cdOt.MsgType_ == msgType && cdOt.FlowID_.IsFinish())
         return true;
         
   } while (inherited::MoveNext2());

   return false;
}
//---------------------------------------------------------------------------

K_mf(bool) TOrdTrack::TRecsHnd::FindUnfinished (const TFixMsgType2& msgType)
{
   if (inherited::FindMap2() == NULL || inherited::MoveFirst2() == false)
      return false;
   
   do {
      const SOrdTrack& cdOt = inherited::GetDat2(false);

      if (cdOt.MsgType_ == msgType && cdOt.FlowID_.IsFinish() == false)
         return true;
         
   } while (inherited::MoveNext2());

   return false;
}


/////////////////////////////////////////////////////////////////////////////
// class TOrdTrack impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TOrdTrack::TOrdTrack (const std::string& logPath)
   : RecsRecorder_(NULL)
{
   char filePath[256];

   sprintf(filePath, "%sOrdTrack3.bin", logPath.c_str());
   RecsRecorder_ = new TRecsRecorder(&RsSysOrdID_, filePath);
}
//---------------------------------------------------------------------------

K_dtor TOrdTrack::~TOrdTrack ()
{
   if (RecsRecorder_)  delete RecsRecorder_;
}
//---------------------------------------------------------------------------

K_mf(TOrdTrack::TRsSeqNoU) TOrdTrack::AddTrack (const TFixSysOrdID& sysOrdID, const SOrdTrack& dat)
{
   TFixSysSeqNo key2(1);
   TRsSysOrdIDI iter1   = RsSysOrdID_.fetch(sysOrdID);
   TRsSeqNo*    rsSeqNo = iter1->second.GetContainer();

   if (rsSeqNo->empty() == false)
      key2.assign(rsSeqNo->rbegin()->first.get_orig() + 1);

   TRsSeqNoIns ins = rsSeqNo->insert(TRsSeqNo::value_type(key2, dat));
   TRsSeqNoU   upd = rsSeqNo->begin_update(ins.first);

   return upd;
}
//---------------------------------------------------------------------------

K_mf(TOrdTrack::TRsSeqNoU) TOrdTrack::GetNewTrack (const TFixSysOrdID& sysOrdID, const SOrdTrack& dat)
{
   TFixSysSeqNo key2(1);
   TRsSysOrdIDI iter1   = RsSysOrdID_.fetch(sysOrdID);
   TRsSeqNo*    rsSeqNo = iter1->second.GetContainer();

   if (rsSeqNo->empty() == false)
      key2.assign(rsSeqNo->rbegin()->first.get_orig() + 1);

   TRsSeqNoIns ins = rsSeqNo->insert(TRsSeqNo::value_type(key2, dat));
   TRsSeqNoU   upd = rsSeqNo->begin_update(ins.first);

   upd->RecvTime_ = TTimeStamp::Now();
   
   return upd;
}
//---------------------------------------------------------------------------

K_mf(TOrdTrack::TRsSeqNo*) TOrdTrack::FindRsSeqNo (const TFixSysOrdID& sysOrdID)
{
   TRsSysOrdIDI iter = RsSysOrdID_.find(sysOrdID);
   if (iter != RsSysOrdID_.end())
      return iter->second.GetContainer();
   return NULL;
}
//---------------------------------------------------------------------------

K_mf(TOrdTrack::TRsSeqNo*) TOrdTrack::GetRsSeqNo (const TFixSysOrdID& sysOrdID)
{
   TRsSysOrdIDI iter = RsSysOrdID_.fetch(sysOrdID);
   return iter->second.GetContainer();
}


/////////////////////////////////////////////////////////////////////////////
// struct TOrdTrackRecoRoom impl
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TOrdTrackRecoRoom::exec_peek_room (TKey& key, TRec& rec, TInn::rr_room& room)
{
   //**********************************************
   #if defined(_FixTag_Twse_Ver2_)
   TInn::rr_live_peek peeker[] = {
      { &key,             sizeof(TKey)      },
      { rec.GetDataBeg(), rec.GetDataSize() },
   };

   return room.peek(peeker, peeker+numofele(peeker));

   #else //----------------------------------------
   
   char line[1024*10];

   TInn::rr_live_peek peeker[] = {
      { &key,                             sizeof(TKey) },
      { K_GetLo(&peeker[2].obj_size_, 2), 2            },
      { line,                             0            },
   };

   if (room.peek(peeker, peeker+numofele(peeker)) == true) {
      unsigned         useSize = peeker[2].obj_size_;
      Kstl::persistent per(line, useSize);

      per.rewind();

      Kstl::pload(per, rec);

      return true;
   }
   return false;
   #endif
   //**********************************************
}
//---------------------------------------------------------------------------

K_mf(bool) TOrdTrackRecoRoom::exec_into_room (TInn::rr_room& room, TInn::rr_key& rrkey, const TKey& key, const TRec& rec)
{
   //**********************************************
   #if defined(_FixTag_Twse_Ver2_)
   TInn::rr_live_into peeker[] = {
      { &key,             sizeof(TKey)      },
      { rec.GetDataBeg(), rec.GetDataSize() },
   };
   return room.into(rrkey, peeker, peeker+numofele(peeker));

   #else //----------------------------------------

   PerSave_.rewind();
   Kstl::psave(PerSave_, rec);

   TInn::rr_live_into peeker[] = {
      { &key,                             sizeof(TKey)                                          },
      { K_GetLo(&peeker[2].obj_size_, 2), 2                                                     },
      { PerSave_.getbuf(),                static_cast<TInn::rr_size>(PerSave_.get_saved_size()) },
   };
   
   return room.into(rrkey, peeker, peeker+numofele(peeker));
   #endif
   //**********************************************
}
//---------------------------------------------------------------------------
