/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "ApaSesConf.h"
//#include "tplns/Nsa_EvMap_0File.h"
//---------------------------------------------------------------------------
//using namespace Kway;

/*
/////////////////////////////////////////////////////////////////////////////
// class TAppiaConf::TRecsRecorder impl
/////////////////////////////////////////////////////////////////////////////
class TAppiaConf::TRecsRecorder : public Nsa::TRecorder<TRecs, SAppiaConf>
{
   typedef Nsa::TRecorder<TRecs, SAppiaConf>  inherited;

public:
   inline K_ctor TRecsRecorder (TRecs* recs, const std::string& fileName)
      : inherited(recs, fileName)
   {}
};


/////////////////////////////////////////////////////////////////////////////
// class TAppiaConf impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TAppiaConf::TAppiaConf ()
   : RecsVAC_(NULL)
{
   RecsRecorder_ = new TRecsRecorder(&Recs_, std::string("./Settings/AppiaConf.bin"));
}
//---------------------------------------------------------------------------

K_mf(const SAppiaConf*) TAppiaConf::FindRec (const TAppiaConfKey& key)
{
   TRecsI iter = Recs_.find(key);
   
   if (iter == Recs_.end()) {
      return NULL;
   }
   return (const SAppiaConf*)(&(iter->second));
}


/////////////////////////////////////////////////////////////////////////////
// class TAppiaConfFm
/////////////////////////////////////////////////////////////////////////////
class TAppiaConfFm
{
   typedef TAppiaConf::TRecs    container;
   typedef container::iterator  iterator;
   typedef container::updater   updater;
   enum {
      IField_Key,
         IField_MiddlewareID,
         IField_ReliableMode,
      IField_Total
   };

public:
   // GetFieldCount
   inline K_mf(size_t) GetFieldCount () const  { return IField_Total; }

   // GetFieldDesp
   K_mf(const Kway::TFieldDesp&) GetFieldDesp (size_t n) const
   {
      static Kway::TFieldDesp FdDsp[] = {
         { "Key",          10, Kway::ta_xLeft_yCenter,  Kway::ta_xLeft_yCenter,  Kway::tfs_Fixed,   },
         { "MiddlewareID", 16, Kway::ta_xRight_yCenter, Kway::ta_xRight_yCenter, Kway::tfs_CanEdit, },
         { "ReliableMode", 15, Kway::ta_xRight_yCenter, Kway::ta_xRight_yCenter, Kway::tfs_CanEdit, },
      };
      return FdDsp[n];
   };

   // Get
   K_mf(std::string) Get (const iterator& i, size_t fieldNo) const
   {
      switch (fieldNo) {
         case IField_Key:           return i->first.as_string();
         case IField_MiddlewareID:  return i->second.MiddlewareID_.as_string();
         case IField_ReliableMode:  return i->second.ReliableMode_.as_string();
      }
      return std::string();
   }

   // Put
   K_mf(void) Put (container& c, iterator& i, size_t fieldNo, const std::string& str)
   {
      if (fieldNo == IField_Key) {
         return;
      }

      updater u = c.begin_update(i);
      switch (fieldNo) {
         case IField_MiddlewareID:  u->MiddlewareID_.assign(str);  break;
         case IField_ReliableMode:  u->ReliableMode_.assign(str);  break;
      }
   }
};
/////////////////////////////////////////////////////////////////////////////
// class TAppiaConf::TRecsVAC
/////////////////////////////////////////////////////////////////////////////
class TAppiaConf::TRecsVAC : public Kway::TVACreater
{
   typedef Kway::TTViewArea<TRecs, TAppiaConfFm>  TViewer;
      
   TRecs*  Recs_;

public:
   inline K_ctor TRecsVAC (TRecs* Recs)
      : Recs_(Recs)
   {}

   K_mf(Kway::TaViewArea) CreateNewVA ()
   {
      TAppiaConfFm        maker;
      Kway::TViewControls vcs;
      
      vcs.set(Kway::vco_Insert);
      vcs.set(Kway::vco_Erase);
      vcs.set(Kway::vco_CopyCurrent);

      return Kway::TaViewArea(new TViewer(Recs_, maker, vcs));
   }
};


/////////////////////////////////////////////////////////////////////////////
// class TAppiaConf impl
/////////////////////////////////////////////////////////////////////////////
K_dtor TAppiaConf::~TAppiaConf ()
{
   if (RecsVAC_) {
      delete RecsVAC_;
      RecsVAC_ = NULL;
   }
   if (RecsRecorder_) {
      delete RecsRecorder_;
      RecsRecorder_ = NULL;
   }
}
//---------------------------------------------------------------------------

K_mf(Kway::TVACreater&) TAppiaConf::GetVAC ()
{
   if (RecsVAC_ == NULL) {
      RecsVAC_ = new TRecsVAC(&Recs_);
   }
   return *RecsVAC_;
}
//---------------------------------------------------------------------------

K_mf(TAppiaConf&) TAppiaConf::GetInst ()
{
   static TAppiaConf appiaConf;
   return appiaConf;
}
//---------------------------------------------------------------------------
*/
