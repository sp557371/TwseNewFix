/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FixStkCoids.h"
#include "tplns/Nsa_EvCtn_1k_VA.h"
#include "FixStkIntf.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kstl;
using namespace Kway::Nsa;
using namespace Kway::FIX;
using namespace Kway::FIX::Stk;

/////////////////////////////////////////////////////////////////////////////
// class TCoids::TRecsRecorder
/////////////////////////////////////////////////////////////////////////////
class TCoids::TRecsRecorder : public container_ev_handler
{
   struct SFileRec
   {
      TRecs::key_type     Key_;
      TRecs::mapped_type  Rec_;

      inline K_ctor SFileRec ()  {}
      inline K_ctor SFileRec (const TRecs::key_type& key, TRecs::mapped_type& rec)
         : Key_(key)
         , Rec_(rec)
      {}
   };

   TRecs*  Recs_;
   TFile   File_;

public:
   K_ctor  TRecsRecorder (TRecs* rs, const char* filePath);
   K_dtor ~TRecsRecorder ();

private:
   //==============================================
   // overwrite container_ev_handler virtual function
   K_mf(void) OnContainerAfEndUpdate (const ev_updater_impl&);

   //==============================================
   // TRecsRecorder function
   K_mf(void) Load (const char* filePath);
};
/////////////////////////////////////////////////////////////////////////////
// class TCoids::TRecsRecorder impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TCoids::TRecsRecorder::TRecsRecorder (TRecs* rs, const char* filePath)
   : Recs_(rs)
{
   Load(filePath);

   File_.Open(filePath, (TFileMode)(fmAppend|fmCreatePath));
   Recs_->add_ev_handler(this);
}
//---------------------------------------------------------------------------

K_dtor TCoids::TRecsRecorder::~TRecsRecorder ()
{
   Recs_->del_ev_handler(this);
}
//---------------------------------------------------------------------------

K_mf(void) TCoids::TRecsRecorder::Load (const char* filePath)
{
   TFile    file(filePath, fmRead);
   SFileRec fileRec;

   while (!file.IsEOF()) {
      if (file.Read(&fileRec, sizeof(fileRec)) != sizeof(fileRec))
         break;

      Recs_->insert(TRecs::value_type(fileRec.Key_, fileRec.Rec_));
   }
}
//---------------------------------------------------------------------------

K_mf(void) TCoids::TRecsRecorder::OnContainerAfEndUpdate (const ev_updater_impl& u)
{
   TRecsI   iter = static_cast<const TRecsI&>(u.get_iterator());
   SFileRec fileRec(iter->first, iter->second);

   File_.Write(&fileRec, sizeof(SFileRec));
}


/////////////////////////////////////////////////////////////////////////////
// class TCoids impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TCoids::TCoids (TTradeInfo* ti)
   : RecsVAC_     (NULL)
   , RecsRecorder_(NULL)
   , IsAutoFlush_ (ti->GetFlushCoids())
{
   char fullPath[256];

   sprintf(fullPath, "%sClOrdIDs4.bin", ti->GetLogPath().c_str());
   RecsRecorder_ = new TRecsRecorder(&Recs_, fullPath);
}
//---------------------------------------------------------------------------

K_mf(bool) TCoids::AddCoid (const TFixClOrdID& clOrdID, TRecsU& upd, TFixSysOrdID sid)
{
   typedef TRecs::value_type  value_type;

   SCoid dat;
   dat.SysOrdID_ = sid;

   std::pair<TRecsI, bool> ins = Recs_.insert(value_type(clOrdID, dat));
      
   if (ins.second)
      upd = Recs_.begin_update(ins.first);
      
   return ins.second;
}
//---------------------------------------------------------------------------

K_mf(const SCoid*) TCoids::FindDat (const TFixClOrdID& key)
{
   TRecsI iter = Recs_.find(key);
   if (iter == Recs_.end())
      return NULL;
   return &(iter->second);
}

/////////////////////////////////////////////////////////////////////////////
// class TCoidsFm
/////////////////////////////////////////////////////////////////////////////
class TCoidsFm
{
   typedef TCoids::TRecs        container;
   typedef container::iterator  iterator;
   enum {
      IField_ClOrdID,
         IField_SysOrdID,
         IField_OrigClOrdID,
      IField_Total
   };

public:
   // GetFieldDesp
   K_mf(const TFieldDesp&) GetFieldDesp (size_t n) const
   {
      static TFieldDesp FdDsp[] = {
         { "ClOrdID",     20, ta_xLeft_yCenter,  ta_xLeft_yCenter,  tfs_Fixed,   },
         { "SysOrdID",    10, ta_xRight_yCenter, ta_xRight_yCenter, tfs_CanEdit, },
         { "OrigClOrdID", 20, ta_xRight_yCenter, ta_xRight_yCenter, tfs_CanEdit, },
      };
      return FdDsp[n];
   };

   // Get
   K_mf(std::string) Get (const iterator& i, size_t fieldNo) const
   {
      switch (fieldNo) {
         case IField_ClOrdID:      return i->first              .as_string();
         case IField_SysOrdID:     return i->second.SysOrdID_   .as_string();
         case IField_OrigClOrdID:  return i->second.OrigClOrdID_.as_string();
      }
      return std::string();
   }

   inline K_mf(size_t) GetFieldCount () const  { return IField_Total; }
   inline K_mf(void)   Put           (container& c, iterator& i, size_t fieldNo, const std::string& str) {}
};
/////////////////////////////////////////////////////////////////////////////
// class TCoids::TRecsVAC
/////////////////////////////////////////////////////////////////////////////
class TCoids::TRecsVAC : public TEvCtn1kVA<TRecs, TCoidsFm>
{
   typedef TEvCtn1kVA<TRecs, TCoidsFm>  inherited;

public:
   inline K_ctor TRecsVAC (TRecs* c) : inherited(c) {}
};

/////////////////////////////////////////////////////////////////////////////
// class TCoids impl
/////////////////////////////////////////////////////////////////////////////
K_dtor TCoids::~TCoids ()
{
   if (RecsVAC_)       delete RecsVAC_;
   if (RecsRecorder_)  delete RecsRecorder_;
}
//---------------------------------------------------------------------------

K_mf(TVACreater&) TCoids::GetVAC ()
{
   if (RecsVAC_ == NULL)
      RecsVAC_ = new TRecsVAC(&Recs_);
   return *RecsVAC_;
}
//---------------------------------------------------------------------------
