/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvObjCtn_h_)
#define _Nsa_EvObjCtn_h_
//---------------------------------------------------------------------------
#include <algorithm>
#include <cc++/thread.h>
#include "SyncEv.h"
#include "pod_vector.h"
#include "SyncEv.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;

namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// enum EEvObjResult
/////////////////////////////////////////////////////////////////////////////
enum EEvObjResult
{
   eor_None,
      eor_Continue,
      eor_Abort,
      eor_Finish,
};


/////////////////////////////////////////////////////////////////////////////
// class TEvObjCtn  在更新 ev 時，有些物件必須先更新，所以有 key 存在
/////////////////////////////////////////////////////////////////////////////
template <class EvObjT>
class TEvObjCtn
{
public:
   typedef EvObjT  TEvObj;
   typedef size_t  TKey;

   enum {
      eNorKeyStart = 101,
   };

private:
   struct SEvInfo
   {
      TKey     Key_;
      TEvObj*  Obj_;

      K_mf(bool) operator < (const SEvInfo& rhs) const
      {
         if (Key_ < rhs.Key_) {
            return true;
         }
         return false;
      }
   };

   typedef          pod_vector<SEvInfo>  TRecs;
   typedef typename TRecs::iterator      TRecsI;

   ost::Mutex   Mutex_;
   TEvObj*      RunningEvObj_;
   TKey         NorKey_;
   TRecs        Recs_;
   std::string  DbgName_;
   
public:
   inline K_ctor TEvObjCtn ()
      : RunningEvObj_(NULL)
      , NorKey_      (eNorKeyStart)
   {}

   inline K_ctor TEvObjCtn (const char* cpDbgName)
      : RunningEvObj_(NULL)
      , NorKey_      (eNorKeyStart)
      , DbgName_     (cpDbgName)
   {}

   K_mf(bool) Add (          TEvObj* EvObj);
   K_mf(bool) Add (TKey Key, TEvObj* EvObj);
   K_mf(void) Del (          TEvObj* EvObj);
   
   inline K_mf(size_t) GetSize () const  { return Recs_.size();  }
   inline K_mf(bool)   Empty   () const  { return Recs_.empty(); }

   template <class TWorker>
   K_mf(EEvObjResult) ForEachEv (TWorker& Worker);
};
/////////////////////////////////////////////////////////////////////////////
// class TEvObjCtn impl
/////////////////////////////////////////////////////////////////////////////
#define Nsa_TEvObjCtn_Impl(return_type)                                    \
   template <class EvObjT>                                                 \
   K_mf(return_type) TEvObjCtn<EvObjT>::                                   \
//---------------------------------------------------------------------------

#if defined(__BORLANDC__)
   #define Nsa_TEvObjCtn_Impl_S(return_type)                               \
      template <class EvObjT>                                              \
      TEvObjCtn<EvObjT>::return_type __fastcall                            \
      TEvObjCtn<EvObjT>::                                                  \
   //------------------------------------------------------------------------

#else

   #define Nsa_TEvObjCtn_Impl_S(return_type)                               \
      template <class EvObjT>                                              \
      typename TEvObjCtn<EvObjT>::return_type                              \
      TEvObjCtn<EvObjT>::                                                  \
   //------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

Nsa_TEvObjCtn_Impl(bool) Add (TEvObj* EvObj)
{
   ost::MutexLock Locker(Mutex_);
   SEvInfo        EvInfo;

   EvInfo.Key_ = NorKey_;
   EvInfo.Obj_ = EvObj;

   ++NorKey_;

   Recs_.push_back(EvInfo);
   return true;
}
//---------------------------------------------------------------------------

Nsa_TEvObjCtn_Impl(bool) Add (TKey Key, TEvObj* EvObj)
{
   if (Key == 0) {
      return Add(EvObj);
   }

   ost::MutexLock Locker(Mutex_);
   SEvInfo        EvInfo;

   EvInfo.Key_ = Key;
   EvInfo.Obj_ = EvObj;

   TRecsI ita = std::lower_bound(Recs_.begin(), Recs_.end(), EvInfo);
   if (ita != Recs_.end()) {
      if (ita->Key_ == EvInfo.Key_) {
         return false;
      }
      Recs_.insert(ita, EvInfo);
   }
   else {
      Recs_.push_back(EvInfo);
   }

   return true;
}
//---------------------------------------------------------------------------

Nsa_TEvObjCtn_Impl(void) Del (TEvObj* EvObj)
{
   if (EvObj == RunningEvObj_) {
      while (EvObj == RunningEvObj_) {
         if (DbgName_.empty() == false) {
            //__TRACE("<TEvObjCtn::Del> [%s][%s]執行中,請稍後!", DbgName_.c_str(), EvObj->GetName().c_str());
         }
         Kway::TSyncObj::CheckSync();
      }
      //RunningEvObj_ = NULL;
   }

   ost::MutexLock Locker(Mutex_);

   for (TRecsI ita=Recs_.begin(); ita!=Recs_.end(); ++ita) {
      if (ita->Obj_ == EvObj) {
         Recs_.erase(ita);
         return;
      }
   }
}
//---------------------------------------------------------------------------

template <class EvObjT>
template <class TWorker>
K_mf(EEvObjResult) TEvObjCtn<EvObjT>::ForEachEv (TWorker& Worker)
{
   if (Recs_.empty()) {
      return eor_Finish;
   }

   ost::MutexLock Locker(Mutex_);
   size_t         Count = Recs_.size();
   EEvObjResult   Result;

   for (size_t i=0; i<Count; ++i) {
      TEvObj* EvObj = Recs_[i].Obj_;

      RunningEvObj_ = EvObj;
      Mutex_.leaveMutex();

      try {
         Result = Worker.Exec(EvObj);
      }
      catch (...) {
         if (DbgName_.empty() == false) {
            //__TRACE("<TEvObjCtn::ForEachEv> [%s] exception!", DbgName_.c_str());
         }
      }

      Mutex_.enterMutex();
      RunningEvObj_ = NULL;

      if (Result == eor_Abort || Result == eor_Finish) {
         break;
      }
      
      Count = Recs_.size();
   }
   RunningEvObj_ = NULL;

   if (Result == eor_Continue) {
      Result = eor_Finish;
   }
   return Result;
}


/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvObjCtn_h_)
