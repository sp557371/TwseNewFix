/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_GlobObjs.h"
#include <stdio.h>
#include "tplns/Nsa_EvCtn_1k_VA.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TGlobObjHndBase impl
/////////////////////////////////////////////////////////////////////////////
K_dtor TGlobObjHndBase::~TGlobObjHndBase ()
{
   if (GlobObj_) {
      Cleanup();
   }
}
//---------------------------------------------------------------------------

K_mf(bool) TGlobObjHndBase::Initialize ()
{
   if (GlobObj_) {
      return true;
   }
   
   TGlobObjs& globObjs = TGlobObjs::GetInst();

   return globObjs.Initialize(*this);
}
//---------------------------------------------------------------------------

K_mf(bool) TGlobObjHndBase::QueryValid ()
{
   if (GlobObj_) {
      return true;
   }

   TGlobObjs& globObjs = TGlobObjs::GetInst();

   return globObjs.QueryValid(*this);
}
//---------------------------------------------------------------------------

K_mf(void) TGlobObjHndBase::Cleanup ()
{
   TGlobObjs& globObjs = TGlobObjs::GetInst();
      
   globObjs.Cleanup(*this);
}
//---------------------------------------------------------------------------

K_mf(TGlobObj*) TGlobObjHndBase::Reset (TGlobObj* globObj)
{
   TGlobObj* old = GlobObj_;

   GlobObj_ = globObj;
   if (GlobObj_ != NULL)
      GlobObj_->add_ref();

   if (old)
      old->release();

   return GlobObj_;
}


/////////////////////////////////////////////////////////////////////////////
// class TGlobObjs impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TGlobObjs::TGlobObjs ()
   : RecsVAC_(NULL)
{
}
//---------------------------------------------------------------------------

K_mf(bool) TGlobObjs::Initialize (TGlobObjHndBase& hndBase)
{
   ost::MutexLock locker(Mutex_);

   TGlobObj*          globObj;
   const std::string& strObjName = hndBase.GetObjName();
   TRecsI             iter = Recs_.find(strObjName);

   if (iter == Recs_.end()) {
      globObj = hndBase.ObjCreater();
      if (globObj == NULL) {
         return false;
      }

      iter = Recs_.fetch(strObjName);
      iter->second.reset(globObj);
   }
   else {
      globObj = iter->second.get();
   }

   hndBase.Reset(globObj);
   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TGlobObjs::QueryValid (TGlobObjHndBase& hndBase)
{
   ost::MutexLock locker(Mutex_);

   TRecsI iter = Recs_.find(hndBase.GetObjName());

   if (iter == Recs_.end()) {
      return false;
   }

   hndBase.Reset(iter->second.get());
   return true;
}
//---------------------------------------------------------------------------

K_mf(void) TGlobObjs::Cleanup (TGlobObjHndBase& hndBase)
{
   ost::MutexLock locker(Mutex_);

   TRecsI iter = Recs_.find(hndBase.GetObjName());

   hndBase.Reset(NULL);
   if (iter != Recs_.end()) {
      unsigned refCount = iter->second.get_ref_count();

      if (refCount <= 1) {
         TGlobObj* obj = iter->second.get();
         if (!obj || obj->KeepLast() == false)
            Recs_.erase(iter);
      }
   }
}
//---------------------------------------------------------------------------

K_mf(void) TGlobObjs::Cleanup ()
{
   ost::MutexLock locker(Mutex_);

   Recs_.clear();
}
//---------------------------------------------------------------------------

K_mf(TGlobObjs&) TGlobObjs::GetInst ()
{
   static TGlobObjs globObjs;
   return globObjs;
}


/////////////////////////////////////////////////////////////////////////////
// class TGlobObjsFm
/////////////////////////////////////////////////////////////////////////////
class TGlobObjsFm
{
   typedef TGlobObjs::TRecs     container;
   typedef container::iterator  iterator;
   typedef container::updater   updater;
   enum {
      IField_ObjName,
         IField_ObjCnt,
         IField_ObjPtr,
      IField_Total
   };

public:
   // GetFieldCount
   inline K_mf(size_t) GetFieldCount () const  { return IField_Total; }
   inline K_mf(void)   Put           (container& c, iterator& i, size_t fieldNo, const std::string& str)  {}

   // GetFieldDesp
   K_mf(const TFieldDesp&) GetFieldDesp (size_t n) const
   {
      static TFieldDesp FdDsp[] = {
         { "物件名稱",   40, ta_xLeft_yCenter,   ta_xLeft_yCenter,   tfs_Fixed,   },
         { "物件使用數", 13, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "物件位置",   10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      };
      return FdDsp[n];
   };

   // Get
   K_mf(std::string) Get (const iterator& i, size_t fieldNo) const
   {
      switch (fieldNo) {
         case IField_ObjName:  return i->first;
         case IField_ObjCnt:
            {
               unsigned refCount = i->second.get_ref_count();
               char     buf[20];

               sprintf(buf, "%d", refCount-1);
               return std::string(buf);
            }

         case IField_ObjPtr:
            {
               TGlobObj* globObj = i->second.get();

               if (globObj) {
                  char buf[20];
                  sprintf(buf, "%p", globObj);
                  return std::string(buf);
               }
            }
            break;
      }
      return std::string();
   }
};
/////////////////////////////////////////////////////////////////////////////
// class TGlobObjs::TRecsVAC
/////////////////////////////////////////////////////////////////////////////
class TGlobObjs::TRecsVAC : public TEvCtn1kVA<TRecs, TGlobObjsFm>
{
   typedef TEvCtn1kVA<TRecs, TGlobObjsFm>  inherited;

public:
   inline K_ctor TRecsVAC (TRecs* map1) : inherited(map1)  {}
};


/////////////////////////////////////////////////////////////////////////////
// class TGlobObjs impl
/////////////////////////////////////////////////////////////////////////////
K_dtor TGlobObjs::~TGlobObjs ()
{
   Destroy();
}
//---------------------------------------------------------------------------

K_mf(void) TGlobObjs::Destroy ()
{
   if (RecsVAC_) {
      delete RecsVAC_;
      RecsVAC_ = NULL;
   }

   Cleanup();
}
//---------------------------------------------------------------------------

K_mf(TVACreater&) TGlobObjs::GetVAC ()
{
   if (RecsVAC_ == NULL) {
      RecsVAC_ = new TRecsVAC(&Recs_);
   }
   return *RecsVAC_;
}
//---------------------------------------------------------------------------

