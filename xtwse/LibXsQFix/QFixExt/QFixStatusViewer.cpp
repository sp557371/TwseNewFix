/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "QFixStatusViewer.h"
#include "tplns/Nsa_EvCtn_1k_VA.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;
using namespace Kway::FIX;

/////////////////////////////////////////////////////////////////////////////
// class TFixStatusViewer::TUpdImpl impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TFixStatusViewer::TUpdImpl::TUpdImpl (TRecs* recs, TRecsI& iter)
   : Recs_(recs)
   , Iter_(iter)
{
}
//---------------------------------------------------------------------------

K_mf(void) TFixStatusViewer::TUpdImpl::SenderMsgSeqNumChange (int, int newVal)
{
   if (Upd_.is_invalid())
      Upd_ = Recs_->begin_update(Iter_);

   Upd_->MsnUpdTime_ = TTimeStamp::Now();

   Upd_->NextSenderMsgSeqNum_.assign(newVal);

   Upd_.flush_update();
}
//---------------------------------------------------------------------------

K_mf(void) TFixStatusViewer::TUpdImpl::TargetMsgSeqNumChange (int, int newVal)
{
   if (Upd_.is_invalid())
      Upd_ = Recs_->begin_update(Iter_);

   Upd_->MsnUpdTime_ = TTimeStamp::Now();

   Upd_->NextTargetMsgSeqNum_.assign(newVal);

   Upd_.flush_update();
}
//---------------------------------------------------------------------------

K_mf(void) TFixStatusViewer::TUpdImpl::SetDesc (const char* desc)
{
   char buf[1024];

   if (Upd_.is_invalid())
      Upd_ = Recs_->begin_update(Iter_);

   sprintf(buf, "%s %s", TTimeStamp::Now().as_string().c_str(), desc);
   Upd_->FixSesDesc_.assign(buf);

   Upd_.flush_update();
}


/////////////////////////////////////////////////////////////////////////////
// class TFixStatusViewer impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TFixStatusViewer::TFixStatusViewer ()
   : RecsVAC_(NULL)
{
}
//---------------------------------------------------------------------------

K_mf(bool) TFixStatusViewer::RegSesInfo (TFixSesInfo* fsi)
{
   SFsvKey key(fsi->GetFixSessionID(), fsi->GetTradeDate());
   TRecsI  iter = Recs_.find(key);

   if (iter != Recs_.end())
      return false;

            iter  = Recs_.fetch(key);
   TRecsU   upd   = Recs_.begin_update(iter);
   SFsvDat& udFsv = *upd;

   udFsv.FixSesInfo_ = fsi;

   //==============================================
   // 建立 TFixStatusViewer::TUpdImpl
   TUpdImpl* impl = new TUpdImpl(&Recs_, iter);
   
   fsi->SetFixSsuPtr(TFixSsuBasePtr(impl));

   return true;
}
//---------------------------------------------------------------------------

K_mf(void) TFixStatusViewer::DelSesInfo (TFixSesInfo* fsi)
{
   SFsvKey key(fsi->GetFixSessionID(), fsi->GetTradeDate());
   TRecsI  iter = Recs_.find(key);

   if (iter != Recs_.end()) {
      fsi->SetFixSsuPtr(TFixSsuBasePtr());
      Recs_.erase(iter);
   }
}


/////////////////////////////////////////////////////////////////////////////
// class TFixStatusViewerFm
/////////////////////////////////////////////////////////////////////////////
class TFixStatusViewerFm
{
   typedef TFixStatusViewer::TRecs  container;
   typedef container::iterator      iterator;
   typedef container::updater       updater;
   enum {
      IField_SessionID,
         IField_TradeDate,
         IField_MsnUpdTime,
         IField_SenderMsgSeqNum,
         IField_TargetMsgSeqNum,
         IField_FixSesDesc,
      IField_Total
   };

public:
   // GetFieldDesp
   K_mf(const Kway::TFieldDesp&) GetFieldDesp (size_t n) const
   {
      static Kway::TFieldDesp FdDsp[] = {
         { "SessionID",       30, Kway::ta_xLeft_yCenter,   Kway::ta_xLeft_yCenter,   Kway::tfs_Fixed,   },
         { "TradeDate",       10, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "UpdTime",         10, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "SenderMsgSeqNum", 16, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "TargetMsgSeqNum", 16, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "FixSesDesc",      20, Kway::ta_xLeft_yCenter,   Kway::ta_xLeft_yCenter,   Kway::tfs_CanEdit, },
      };
      return FdDsp[n];
   };

   // Get
   K_mf(std::string) Get (const iterator& i, size_t fieldNo) const
   {
      switch (fieldNo) {
         case IField_SessionID:   return i->first.SessionID_.toFormat();
         case IField_TradeDate:   return i->first.TradeDate_.as_string(Kway::TKDate::sf_ELong);
         case IField_MsnUpdTime:  return i->second.MsnUpdTime_.as_string();
         case IField_FixSesDesc:  return i->second.FixSesDesc_;
         
         case IField_SenderMsgSeqNum:
         case IField_TargetMsgSeqNum:
            {
               TFixStatusViewer::SFsvDat::TNsInt* nsInt = &(i->second.NextSenderMsgSeqNum_);
               
               if (fieldNo == IField_TargetMsgSeqNum)
                  nsInt = &(i->second.NextTargetMsgSeqNum_);
            
               if (nsInt->is_0())
                  return std::string("n/a");
                     
               return nsInt->as_string();
            }
      }
      return std::string();
   }

   // Put
   K_mf(void) Put (container& c, iterator& i, size_t fieldNo, const std::string& str)
   {
      if (fieldNo == IField_SenderMsgSeqNum || fieldNo == IField_TargetMsgSeqNum) {
         TFixSesInfo* fsi = i->second.FixSesInfo_;
         if (fsi == NULL)
            return;
            
         TFixSnsBase* snsBase = fsi->GetFixSnsPtr().get();
         if (snsBase == NULL)
            return;

         // 這邊不需要用 updater, 因為 snsBase 會透過 Session Status Updater 來異動 VA
         int val = atoi(str.c_str());
         switch (fieldNo) {
            case IField_SenderMsgSeqNum:  snsBase->SetNextSenderMsgSeqNum(val);  break;
            case IField_TargetMsgSeqNum:  snsBase->SetNextTargetMsgSeqNum(val);  break;
         }
      }
   }

   inline K_mf(size_t) GetFieldCount () const  { return IField_Total; }
};
/////////////////////////////////////////////////////////////////////////////
// class TFixStatusViewer::TRecsVAC
/////////////////////////////////////////////////////////////////////////////
class TFixStatusViewer::TRecsVAC : public Nsa::TEvCtn1kVA<TRecs, TFixStatusViewerFm>
{
   typedef Nsa::TEvCtn1kVA<TRecs, TFixStatusViewerFm>  inherited;

public:
   inline K_ctor TRecsVAC (TRecs* c) : inherited(c) {}
};


/////////////////////////////////////////////////////////////////////////////
// class TFixStatusViewer impl
/////////////////////////////////////////////////////////////////////////////
K_dtor TFixStatusViewer::~TFixStatusViewer ()
{
   if (RecsVAC_)  delete RecsVAC_;
}
//---------------------------------------------------------------------------

K_mf(Kway::TVACreater&) TFixStatusViewer::GetVAC ()
{
   if (RecsVAC_ == NULL)
      RecsVAC_ = new TRecsVAC(&Recs_);
   return *RecsVAC_;
}
//---------------------------------------------------------------------------
