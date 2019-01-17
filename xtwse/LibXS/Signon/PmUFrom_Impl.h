//
// Policy Manager - UserFrom (工作站來源管理) - Impl
//
// 避免在 alpha 的 DEC UNIX 上, g++ compile 時記憶體不足,
// 所以將 TPmUFromGroup::TImpl 抽出來
//
// 版本: $Id: PmUFrom_Impl.h,v 1.4 2005/04/26 06:55:29 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef PmUFrom_ImplH
#define PmUFrom_ImplH
/////////////////////////////////////////////////////////////////////////////
#include "PmUFrom.h"
#include "VALogger.h"
#include "SesExtab.h"
#include "SignonSvrDB.h"   //K_gd(TExtabUpdaterMgr*) SignonDB_ExtabMgr;
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Signon;
/////////////////////////////////////////////////////////////////////////////
namespace Kpui {  // Kway PmUserFrom Impl
//---------------------------------------------------------------------------
#ifdef __alpha    // alpha上,縮短名稱用
#define TUFGroupRec              Tufgr
#define TUFGroupsRec             Tufgs
#define TUFGroupKinds_KeyFinder  Tufgkf
#endif
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
struct TUFGroupRec
{
   //目前沒有任何資料,
   //  GroupKind - [TUFGroup-TUFGroupRec]*n筆
};
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
//---------------------------------------------------------------------------
// 定義存檔方式(檔案最大4G(dword),每筆Rec最大64K(word))
typedef Kstl::rr_inn_file<dword,word>                                      TInn;
typedef TSimpleMap_AddRoomKey<TUFGroup,TUFGroupRec,TInn::rr_key>::TEvRecs  TUFGroupsBase;
//---------------------------------------------------------------------------
class TUFGroupsRec : public TUFGroupsBase
{
   std::string mutable  PolicyResult_; //用在 GetPolicy() 的傳回值
public:
   K_mf(TPolicyBaseRef)  GetPolicy  () const
      {
         PolicyResult_ = GetGroups(0);
         return TPolicyBaseRef(PolicyResult_.data(),
                               static_cast<TPolicyBaseRef::second_type>(PolicyResult_.length()));
      }
   K_mf(std::string)  GetGroups  (char spl) const
      {
         std::string      result;
         bool             isFirst = true;
         const_iterator   iend = end();
         for(const_iterator  i = begin();  i != iend;  ++i) {
            const TUFGroup&   group = i->first;
            if(spl==0)
               result.insert(result.end(), group.begin(), group.end());
            else {
               if(isFirst)
                  isFirst = false;
               else
                  result += spl;
               result += group.as_string();
            }
         }
         return result;
      }

   typedef TUFGroupKind    TMasterKey;
   TMasterKey  MasterKey_;
   inline K_mf(const TMasterKey&)  get_master_key  () const                   { return MasterKey_; }
   inline K_mf(void)               set_master_key  (const TMasterKey& mkey)   { MasterKey_ = mkey; }
   inline K_mf(std::string)        GetMasterKeyStr () const                   { return MasterKey_.as_string(); }
};
typedef Kstl::ev_map<TUFGroupKind,TUFGroupsRec>    TUFGroupKinds_t; //master
K_typedef(TUFGroupKinds_t,                         TUFGroupKinds);
//---------------------------------------------------------------------------
//typedef TSimpleRecorder_Detail<map2d_maker_key_find<map2d_maker_base<TUFGroupKinds> >,
//                               TInn, sizeof(TUFGroupRec)//要存入Inn的資料大小
//                               >      TUFGroupKinds_Recorder;
typedef map2d_maker_key_find<map2d_maker_base<TUFGroupKinds> >    TUFGroupKinds_KeyFinder_t;
K_typedef(TUFGroupKinds_KeyFinder_t, TUFGroupKinds_KeyFinder);

K_typedef_TSimpleRecorder_Detail(TUFGroupKinds_Recorder,
                                 TUFGroupKinds_KeyFinder,
                                 TInn,
                                 sizeof(TUFGroupRec));
//===========================================================================
//資料顯示方法
struct TFieldMaker : public TVAKeyFieldMaker<TUFGroupKinds>
{  //Master-僅顯示群組分類
   K_mf(const TFieldDesp&)  GetFieldDesp   (field_n_type n) const
   { static TFieldDesp fdesp = { "群組分類", 12, Kway::ta_xLeft_yCenter, Kway::ta_xLeft_yCenter, Kway::tfs_Fixed, };
     return fdesp; }
}; // class TFieldMaker
//---------------------------------------------------------------------------
//Detail-資料顯示方法
struct TvacDetail : public TVACreater {
   typedef TUFGroupsRec    TRecs;
   struct TFieldMaker : public TVAKeyFieldMaker<TRecs>
   {  //Detail-僅顯示群組代碼
      K_mf(const TFieldDesp&)  GetFieldDesp   (field_n_type n) const
      { static TFieldDesp fdesp = { "群組", 4, Kway::ta_xLeft_yCenter, Kway::ta_xLeft_yCenter, Kway::tfs_Fixed, };
        return fdesp; }
   }; // class TFieldMaker
   typedef TTVALogger2d<TFieldMaker>   TVALogger;
   TVALogger  VALog_;
   K_ctor            TvacDetail   (TTableLog2d* log)  : VALog_(log) {}
   K_mf(TaViewArea)  CreateNewVA  ()   { return VALog_.CreateNewVA(0); }
};
typedef TvacDetail::TVALogger::TViewArea  TDetailVA;
typedef TvacDetail::TFieldMaker           TDetailFieldMaker;
}  // namespace Kpui
using namespace Kpui;
//---------------------------------------------------------------------------
class  TPmUFromGroup::TImpl : public  TUFGroupKinds,
                              private TUFGroupKinds_Recorder,
                              public  TPmBaseVA
{
   typedef TUFGroupKinds_Recorder   InheritedRecorder;
   typedef TUFGroupKinds            TMasterRecs;
   typedef TMasterRecs              TRecs;
   typedef TUFGroupsRec             TDetailRecs;

public:
   typedef TMasterRecs::iterator          iterator;
   typedef TMasterRecs::const_iterator    const_iterator;

   K_ctor  TImpl  (const std::string& defaultPath);

 //K_mf(TPolicyBaseRef)  GetPolicy  (const TUFGroupKind& gkind) const
   K_mf(TPolicyBaseRef)  GetPolicy  (const TPolicyKey&   gkind, bool forLocalUse) const
      {
         const_iterator   ifind = find(gkind);
         if(ifind==end())
            switch(gkind[0]) {
            case ':':   return TPolicyBaseRef(gkind.begin()+1,sizeof(TUFGroup));
            case '*':   return TPolicyBaseRef(gkind.begin(),  sizeof(TUFGroup));
            default:    return TPolicyBaseRef(0,0);
            }
         return ifind->second.GetPolicy();
      }

   //取得全部的 GroupKinds 當作輸入時的[列舉]
   TVAEditProp_GetKeys<TMasterRecs>    VAEditProp_ForKinds_;

   //註冊資料異動事件
   inline K_mf(void)  GroupKinds_add_ev_handler  (Kstl::container_ev_handler* a)    { Recs_.add_ev_handler(a); }
   inline K_mf(void)  GroupKinds_del_ev_handler  (Kstl::container_ev_handler* a)    { Recs_.del_ev_handler(a); }

   #ifdef __TransLnDB__ //如果需要從 KPTS的LnDB 轉入登入資料庫時,才需要開放底下的 members, 但您必須自己完成他
   K_mf(std::string)  LoadLnDB  (const std::string& pathLnDB);
   #endif

private:
   K_mf(TMasterRecs*)  get_recs  ()  { return this; }
   //---------------------------------------------------------------------------
   #ifndef __TransLnDB__
   TExtabLogUpdater<TExtabLog,  TTExtabUpdater<TFieldMaker> >           MasterLog_;
   TExtabLogUpdater<TExtabLog2d,TTExtabUpdater2d<TMasterRecs,
                                                 TDetailFieldMaker> >   DetailLog_;
   #endif
   //---------------------------------------------------------------------------
   //----- data members
   Kpui::TvacDetail  DetailVACreater_;
   //---------------------------------------------------------------------------
   //給 ViewAreaMgr 管理用的 members
   typedef TTVALogger<Kpui::TFieldMaker>  TVALogger;
   typedef TVALogger::TViewArea           TMasterVA;
   TVALogger  VALog_;
   K_mf(TaViewArea)  CreateNewVA  ()                  { return VALog_.CreateNewVA(this); }
   K_mf(size_t)      GetDetails   (const char**& details)
                                                      { static const char*  detDesp[] = { "群組表" };
                                                        details = detDesp;
                                                        return numofele(detDesp); }
   K_mf(TVACreater*)  GetDetailVACreater    (size_t n)  { return &DetailVACreater_; }
   K_mf(TVACreater*)  OnMasterViewRowMoved  (TViewArea& masterVA, TViewArea*& detailVA, size_t)
      {
         TMasterVA&  va = static_cast<TMasterVA&>(masterVA);
         const TMasterVA::cursor_type* cur = va.GetCurRow();
         static_cast<Kpui::TDetailVA*>(detailVA)->SetContainer(
                                 cur==0 || cur->IsInvalid() ? 0 : &(cur->GetIterator()->second) );
         return DetailVACreaterNoChange;
      }
   //---------------------------------------------------------------------------
};
/////////////////////////////////////////////////////////////////////////////
#endif
