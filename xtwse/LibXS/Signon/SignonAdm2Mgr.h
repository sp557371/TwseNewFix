#include "SignonAdm2Policy.h"
#include "Signon/PmBase.h"
#include "recorder2.h"
#include "wise_ptr.h"
#include "SignonSvr_DC.h"
#include "ExtFuncs.h"

namespace Kway { namespace Signon { namespace Admin2 {
/////////////////////////////////////////////////////////////////////////////
class TSignonSvrL2DC : public TSignonSvr_DataCenter, public TViewAreaMgr, public ref_counter<>
{
   typedef TSignonSvr_DataCenter inherited;
public:
   K_ctor TSignonSvrL2DC (TSignonSvr_DataCenter& parentDC, const std::string& dbpath, const std::string& logfn);

   K_mf(void) SetPrevUserID (const std::string& prevUserID);

   K_mf(TaViewArea) CreateNewVA();

   K_mf(int)  SignonUser  (const std::string& user, const std::string& pass, const std::string& ufrom, const std::string& clientAP, TSignonResult& sgnResult);
   K_mf(bool) SignoutUser (const std::string& user, const std::string& ufrom, const std::string& clientAP, TOnlineID& onlineID);
};
typedef Kstl::wise_ref<TSignonSvrL2DC>   TL2DCPtr;
//---------------------------------------------------------------------------
class TAdmin2UserMgr : public TAdmin2Policy
{
   typedef TAdmin2Policy   inherited;
   TL2DCPtr    L2DC_;
   TPolicyKey  CurPrevUserID_;

public:
   inline K_ctor TAdmin2UserMgr (const TPolicyRights::TMasterKey& mkey = TPolicyRights::TMasterKey())
      : inherited(mkey)
      , L2DC_(NULL)
      { }
   inline K_mf(TAdmin2Policy&) operator= (const TAdmin2Rights& r) {
      return inherited::operator=(r);
   }
   inline K_mf(void) operator= (const TAdmin2UserMgr& r) {
      inherited::operator=(r);
      L2DC_ = r.L2DC_;
   }
   K_mf(TL2DCPtr&) GetL2DC(TSignonSvr_DataCenter& parentDC) {
      if(!L2DC_.get()) {
         std::string key    = PolicyRights_.GetMasterKey().as_string();
         std::string dbpath = MergeFileName(parentDC.GetPathDB(), "adm2/" + key) + "/";
         std::string logfn  = "./Logs/%4y%2m%2d/Signon_" + key + ".log";
         L2DC_.reset(new TL2DCPtr::ref_obj(parentDC, dbpath, logfn));
      }
      if(CurPrevUserID_ != PrevUserID_) {
         CurPrevUserID_ = PrevUserID_;
         L2DC_->SetPrevUserID(PrevUserID_.as_string());
      }
      return L2DC_;
   }
};
/////////////////////////////////////////////////////////////////////////////
class TAdmin2UserMgrMap : public Kstl::ev_map<TPolicyKey, TAdmin2UserMgr>
{
   typedef Kstl::ev_map<TPolicyKey,TAdmin2UserMgr>   inherited;
public:
   struct value_type : public inherited::value_type {
      inline K_ctor  value_type  (const key_type& key, const mapped_type& rec)
         : inherited::value_type(key,rec)
         { }
      inline K_ctor  value_type  (const key_type& key, const TAdmin2UserMgr::TBase& rec)
         : inherited::value_type(key, TAdmin2UserMgr(key))
         { static_cast<TAdmin2UserMgr::TBase&>(second) = rec; }
   };

   K_mf(iterator)  fetch  (const key_type& k) {
      iterator position = lower_bound(k);
      if(position == end()
      || key_comp()(position->first, k)
      || key_comp()(k, position->first))
         return insert(position, value_type(k,mapped_type(k)));
      return position;
   }

   K_mf(mapped_type&)  operator[]  (const key_type& k) {
      value_type tmp(k,mapped_type(k));
      return (*((insert(tmp)).first)).second;
   }
};
//---------------------------------------------------------------------------
typedef Kstl::TSimpleRecorder2< TAdmin2UserMgrMap     //TConatiner
                              , TAdmin2Inn            //Inn
                              , TAdmin2UserMgr::TBase>//要儲存的type.
        TAdmin2UserMgr_Recorder;
//===========================================================================
class  TAdmin2PolicyMgr : protected TAdmin2UserMgr_Recorder
                        , public    Kway::Signon::TPmBaseVA
{
   typedef TAdmin2UserMgr_Recorder  InheritedRecorder;

public:
   using InheritedRecorder::TRecs;
   using InheritedRecorder::get_recs;

   K_ctor  TAdmin2PolicyMgr (const std::string& defaultPath, TPmMgr& userPmMgr);
   K_dtor ~TAdmin2PolicyMgr ();

   K_mf(Kway::Signon::Policy::TPolicyBaseRef)  GetPolicy  (const Kway::Signon::TPolicyKey& key, bool forLocalUse) const;

   //資料顯示方法
   struct  TFieldMaker
   {
      typedef TRecs                    TEvContainer;
      typedef TEvContainer::iterator   iterator;
      typedef size_t                   field_n_type;

      K_mf(field_n_type)             GetFieldCount  () const;
      K_mf(const Kway::TFieldDesp&)  GetFieldDesp   (field_n_type n) const;
      K_mf(std::string) Get  (const iterator& i, field_n_type) const;
      K_mf(void)        Put  (TEvContainer&, iterator&, field_n_type, const std::string&);
   }; // class TFieldMaker

private:
   //給 ViewAreaMgr 管理用的 members
   K_mf(Kway::TaViewArea)  CreateNewVA  ();
   K_mf(size_t)            GetDetails            (const char**&);
   K_mf(Kway::TVACreater*) GetDetailVACreater    (size_t);
   K_mf(Kway::TVACreater*) OnMasterViewRowMoved  (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);

   class TImpl;
   TImpl*   Impl_;
};
/////////////////////////////////////////////////////////////////////////////
} } }
