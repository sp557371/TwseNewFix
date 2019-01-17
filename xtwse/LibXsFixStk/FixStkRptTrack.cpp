/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FixStkRptTrack.h"
#include "Nsa_UtilStr.h"
#include "tplns/Nsa_EvMap_0File.h"
#include "FixStkIntf.h"
#include "FixStkPer.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;
using namespace Kway::FIX;
using namespace Kway::FIX::Stk;

//*************************************************
#if defined(_FixStkVer_RptTrack_)

/////////////////////////////////////////////////////////////////////////////
// class TRptTrack::TRecsRecorder
/////////////////////////////////////////////////////////////////////////////
typedef TMapMaker_2k2d<TRptTrack::TRsSysOrdID, TRptTrack::TRsSeqNo>  TRptTrackMapMaker;
//---------------------------------------------------------------------------
struct TRptTrackRecoRoom
{
   typedef TRptTrackMapMaker::key_type  TKey;
   typedef SRptTrack                    TRec;
   typedef TCommInn                     TInn;

   static K_mf(bool) exec_peek_room (TKey&, TRec&, TInn::rr_room&);
   static K_mf(bool) exec_into_room (TInn::rr_room&, TInn::rr_key&, const TKey&, const TRec&);
};
//---------------------------------------------------------------------------
class TRptTrack::TRecsRecorder : public TSimpRecoDeta<TRptTrackMapMaker, TRptTrackRecoRoom>
{
   typedef TSimpRecoDeta<TRptTrackMapMaker, TRptTrackRecoRoom>  inherited;

public:
   inline K_ctor TRecsRecorder (TRsSysOrdID* map1, const char* filePath)
      : inherited(map1, std::string(filePath))
   {}
};


/////////////////////////////////////////////////////////////////////////////
// class TRptTrack impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TRptTrack::TRptTrack (TTradeInfo* ti)
   : RecsRecorder_(NULL)   
{
   if (!ti->GetFixTiProp()->test(tip_Disable_RptTrack_Reco)) {
      char filePath[256];
      sprintf(filePath, "%sRptTrack.bin", ti->GetLogPath().c_str());
      RecsRecorder_ = new TRecsRecorder(&RsSysOrdID_, filePath);
   }
}
//---------------------------------------------------------------------------

K_dtor TRptTrack::~TRptTrack ()
{
   if (RecsRecorder_)  delete RecsRecorder_;
}
//---------------------------------------------------------------------------

K_mf(TRptTrack::TRsSeqNo*) TRptTrack::FindRsSeqNo (const TFixSysOrdID& sysOrdID)
{
   TRsSysOrdIDI iter = RsSysOrdID_.find(sysOrdID);
   if (iter != RsSysOrdID_.end())
      return iter->second.GetContainer();
   return NULL;
}
//---------------------------------------------------------------------------

K_mf(TRptTrack::TRsSeqNo*) TRptTrack::GetRsSeqNo (const TFixSysOrdID& sysOrdID)
{
   TRsSysOrdIDI iter = RsSysOrdID_.fetch(sysOrdID);
   return iter->second.GetContainer();
}


/////////////////////////////////////////////////////////////////////////////
// struct TRptTrackRecoRoom impl
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TRptTrackRecoRoom::exec_peek_room (TKey& key, TRec& rec, TInn::rr_room& room)
{
   char line[1024*10];

   TInn::rr_live_peek peeker[] = {
      { &key,                             sizeof(TKey) },
      { K_GetLo(&peeker[2].obj_size_, 2), 2            },
      { line,                             0            },
   };

   if (room.peek(peeker, peeker+numofele(peeker)) == true) {
      unsigned         useSize = peeker[2].obj_size_;
      Kstl::persistent per(line, useSize);

      per.save  (line, useSize, false);
      per.rewind();

      Kstl::pload(per, rec);

      return true;
   }
   
   return false;
}
//---------------------------------------------------------------------------

K_mf(bool) TRptTrackRecoRoom::exec_into_room (TInn::rr_room& room, TInn::rr_key& rrkey, const TKey& key, const TRec& rec)
{
   Kstl::persistent_auto per(1024*10);
   Kstl::psave(per, rec);

   TInn::rr_live_into peeker[] = {
      { &key,                             sizeof(TKey)                                     },
      { K_GetLo(&peeker[2].obj_size_, 2), 2                                                },
      { per.getbuf(),                     static_cast<TInn::rr_size>(per.get_saved_size()) },
   };
   
   return room.into(rrkey, peeker, peeker+numofele(peeker));
}
//---------------------------------------------------------------------------

#endif // defined(_FixStkVer_RptTrack_)
//*************************************************
