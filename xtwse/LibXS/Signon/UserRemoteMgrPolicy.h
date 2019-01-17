//
// UserRemoteMgrPolicy.使用者遠端管理政策
//
// 版本: $Id: UserRemoteMgrPolicy.h,v 1.2 2005/04/26 05:57:37 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef UserRemoteMgrPolicyH
#define UserRemoteMgrPolicyH
//---------------------------------------------------------------------------
#include "./Signon/Policy/UPRemoteMgr.h"
#include "PmBase.h"
#include "inn_file.h"
#include "recorder2.h"
#include "VALogger.h"
#include "SesExtab.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
/////////////////////////////////////////////////////////////////////////////
using Kway::Signon::Policy::TUserRemoteMgrPolicyBase;
using Kway::Signon::Policy::TUserRemoteMgrPolicy;
typedef Kstl::rr_inn_file<dword,word>  TUserRemoteMgrInn;
/////////////////////////////////////////////////////////////////////////////
struct TUserRemoteMgrPolicy_Key : public TPolicyKey {};
/*
// 定義資料表
K_typedef_TSimpleRecorder_AddRoomKey(TUserRemoteMgrPolicy_Recorder,
                                     TUserRemoteMgrPolicy_Key,
                                     TUserRemoteMgrPolicy,
                                     TUserRemoteMgrInn,
                                     sizeof(TUserRemoteMgrPolicyBase));
*/
class TUserRemoteMgrPolicyMap : public Kstl::ev_map<TUserRemoteMgrPolicy_Key,TUserRemoteMgrPolicy>
{
   typedef Kstl::ev_map<TUserRemoteMgrPolicy_Key,TUserRemoteMgrPolicy>   inherited;
public:
   struct value_type : public inherited::value_type {
      inline K_ctor  value_type  (const key_type& key, const mapped_type& rec)
         : inherited::value_type(key,rec)
         { }
      inline K_ctor  value_type  (const key_type& key, const TUserRemoteMgrPolicyBase& rec)
         : inherited::value_type(key, TUserRemoteMgrPolicy(key))
         { static_cast<TUserRemoteMgrPolicyBase&>(second) = rec; }
   };

   K_mf(iterator)  fetch  (const key_type& k)
   {
      iterator position = lower_bound(k);
      if(position == end()
      || key_comp()(position->first, k)
      || key_comp()(k, position->first))
         return insert(position, value_type(k,mapped_type(k)));
      return position;
   }

   K_mf(mapped_type&)  operator[]  (const key_type& k)
   {
      value_type tmp(k,mapped_type(k));
      return (*((insert(tmp)).first)).second;
   }
};
//---------------------------------------------------------------------------
typedef Kstl::TSimpleRecorder2< TUserRemoteMgrPolicyMap   //TConatiner
                              , Policy::TURM_Inn          //Inn
                              , TUserRemoteMgrPolicyBase> //要儲存的type.
        TUserRemoteMgrPolicy_Recorder;
//===========================================================================
SIGDB_class  TUserRemoteMgrPolicy_Mgr : protected TUserRemoteMgrPolicy_Recorder
                                      , public    TPmBaseVA
{
   typedef TUserRemoteMgrPolicy_Recorder  InheritedRecorder;

public:
   using InheritedRecorder::TRecs;
   using InheritedRecorder::get_recs;

   // fields[0]=資料表名稱, fields[1]=權限
   K_ctor  TUserRemoteMgrPolicy_Mgr (const std::string& defaultPath, TFieldDesp* vaFields = 0);
   K_dtor ~TUserRemoteMgrPolicy_Mgr ();

   static K_mf(const TFieldDesp*) GetDefaultVAFields();
   K_mf(TPmBaseVA*) CreatePmBaseVA (Kway::Signon::TSignonSvr_DataCenter& owner, TSignonSvr_DataCenter* dcParentLimited);

   K_mf(TPolicyBaseRef)  GetPolicy  (const TPolicyKey& key, bool forLocalUse) const
                { return forLocalUse ? GetPolicy_TRec(Recs_, static_cast<const TRecs::key_type&>(key), (TUserRemoteMgrPolicy*)0)
                                     : TPolicyBaseRef(); }

   //資料顯示方法
   struct  TFieldMaker
   {
      typedef TRecs                    TEvContainer;
      typedef TEvContainer::iterator   iterator;
      typedef size_t                   field_n_type;

      K_mf(field_n_type)       GetFieldCount  () const;
      K_mf(const TFieldDesp&)  GetFieldDesp   (field_n_type n) const;
      K_mf(std::string) Get  (const iterator& i, field_n_type) const;
      K_mf(void)        Put  (TEvContainer&, iterator&, field_n_type, const std::string&);
   }; // class TFieldMaker

private:
   //給 ViewAreaMgr 管理用的 members
   K_mf(TaViewArea)  CreateNewVA  ();
   K_mf(size_t)      GetDetails            (const char**&);
   K_mf(TVACreater*) GetDetailVACreater    (size_t);
   K_mf(TVACreater*) OnMasterViewRowMoved  (TViewArea& masterVA, TViewArea*& detailVA, size_t detailID);

   class TImpl;
   TImpl*   Impl_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
#endif


