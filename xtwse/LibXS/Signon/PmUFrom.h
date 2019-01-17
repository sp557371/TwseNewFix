//
// Policy Manager - UserFrom (工作站來源管理)
//
// 版本: $Id: PmUFrom.h,v 1.8 2006/04/04 04:04:19 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef PmUFromH
#define PmUFromH
//---------------------------------------------------------------------------
//
// 工作站來源(PIv4,IPv6,Modem,RS232...)
// 政策管理資料表,基礎定義
//
//   例如: 從 (IPv4) 172.16.100.170 來的工作站,允許登入 :8770 的群組 (單一群組 8770)
//         從 (IPv4) 127.0.0.1      來的工作站,允許登入 *     的群組 (任意群組)
//         從 (IPv4) 172.16.100.169 來的工作站,允許登入 877   的群組 (GroupKind=877,查詢群組列表)
//         從 (TEL)  0225071823     來的工作站,允許登入 :kway 的群組 (單一群組 kway)
//
//---------------------------------------------------------------------------
#include "Signon/Policy/UFGroups.h"
#include "PmBase.h"
#include "inn_file.h"
#include "SimpleRecorder.h"
#include "ViewArea.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
using Kway::Signon::Policy::TUFGroup;
typedef TPolicyKey          TUFGroupKind;
class TPmUFromGroup;
//---------------------------------------------------------------------------
struct TUFGroupPolicy         //使用者來源(ipv4、ipv6、modem...),允許簽入的使用者群組
{
   TUFGroupKind   GroupKind_; // *     = 不限群組(可登入任意群組),
                              // :xxxx = 僅可登入xxxx群組
                              // gkID  = 用 gkID 查詢 GroupKind 對照表所列的群組

   TPmGroupID     PmGroupID_; // 其他管理政策的設定,可用此ID要求PmGroup查詢一組[PolicyName-PolicyKey]


   //因為此結構會用在許多地方(IPv4,IPv6...),所以將一些可能會共用的method,
   //擺在這兒,提供大家方便使用:
   //  取出欄位描述,預設提供3個欄位:
   //    (0)政策群代碼、(1)群組分類、(2)允許簽入的群組(從 TPmUFromGroup* 取得)
   static        K_mf(const TFieldDesp&)  GetFieldDesp   (TPmUFromGroup*, size_t fieldNo);
   static inline K_mf(size_t)             GetFieldCount  ()               { return 3; }
                 K_mf(std::string)        Get            (TPmUFromGroup*, size_t fieldNo) const;
                 K_mf(void)               Put            (size_t fieldNo, const std::string&);
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
class TPmUFromGroup : public TPmGroup
{  // TPmUFromGroup其實就是一個 TPmGroup
   //    1.多提供了一個不需要註冊的 Policy: GetPmGroupKinds()
   //    2.提供GroupKind的事件註冊機制
   //    3.GetPolicy()的參數將原本的 TPmGroupID 改成 TUFGroupPolicy
   //      當使用 TUFGroupPolicy.PmGroupID_ 找不到對應的 Policy 時,
   //      且 PolicyName == Signon::TUFGroups::PolicyName, 則改查詢 GroupKind
   typedef TPmGroup  inherited;
   class TImpl;
   TImpl*   Impl_;
public:
   K_ctor  TPmUFromGroup  (const std::string& defaultPath, TPmMgr*,
                           const std::string& mGroupName, const std::string& mTableName,
                           const std::string& dGroupName, const std::string& dTableName);
   K_dtor ~TPmUFromGroup  ();

   //當 policyName==TUFGroups::PolicyName 時, 則先使用 gp.GroupKind_ 查詢 GroupKinds
   //若找不到,才使用 gp.PmGroupID_ 查詢 [政策群]的設定
   K_mf(TPolicyBaseRef)  GetPolicy  (const TPmMgr&, const TUFGroupPolicy*, const std::string& policyName, bool forLocalUse) const;
   K_mf(std::string)     GetGroups  (const TUFGroupKind&) const;  //每個可登入群組用「,」分開

   //取出預設的 GroupKind - Policy
   K_mf(TPmBaseVA*) GetPmGroupKinds  ();
   //註冊事件(當GroupKinds資料有異動時)
   K_mf(void)  GroupKinds_add_ev_handler  (Kstl::container_ev_handler*);
   K_mf(void)  GroupKinds_del_ev_handler  (Kstl::container_ev_handler*);

   //取出全部的 GroupKind 當作輸入時的 [列舉]
   K_mf(TVAGetEditProp&)  GetVAEditProp_ForKinds  ();

   #ifdef __TransLnDB__ //如果需要從 KPTS的LnDB 轉入登入資料庫時,才需要開放底下的 members, 但您必須自己完成他
   K_mf(std::string)  LoadLnDB  (const std::string& pathLnDB);
   #endif
};
/////////////////////////////////////////////////////////////////////////////
template <class TUFKey, class TUFRec = TUFGroupPolicy, size_t RecSz = sizeof(TUFRec)>
              //TUFKey = TUFIPv4Range (IPv4 的一組 ip 範圍)
              //         TUFIPv6Range (IPv6 的一組 ip 範圍)
              //         TUFTel       (電話號碼)
              //         ...
class TPmUFromBase : public TVACreater
{
   typedef TVACreater                           inherited;
   typedef TPmUFromBase<TUFKey,TUFRec,RecSz>    this_type;
   typedef this_type                            TUFromBase;
   typedef word                                 rr_size; //如果在底下的 rr_size 使用 TUFInn::rr_size, bcb4 compile會有問題? 唉...
#ifdef __TransLnDB__ //如果需要從 KPTS的LnDB 轉入登入資料庫時,才需要開放底下的 members
protected:
#endif
   typedef Kstl::rr_inn_file<dword,rr_size>     TUFInn;
   //---------------------------------------------------------------------------
   class TUFRecorder : public TSimpleRecorder_AddRoomKey<TUFKey,TUFRec,TUFInn,RecSz>
   {
      friend class TPmUFromBase; //TPmUFromBase<>
      typedef TSimpleRecorder_AddRoomKey<TUFKey,TUFRec,TUFInn,sizeof(TUFRec)> inherited;
      #ifndef __BORLANDC__
         using inherited::BindInn;
         using inherited::rec_size;
         using inherited::Recs_;
      #endif
   public:
      K_ctor  TUFRecorder  (const std::string& fileName, typename TUFromBase::rr_size roomSize)
               : inherited(fileName, roomSize)
               {
               if(sizeof(typename TRecs::key_type) + rec_size > roomSize)
                  throw "TPmUFromBase::TUFRecorder 客房住不下囉!!";
               BindInn();
               }
      typedef typename inherited::TRecs  TRecs_t;
      K_typedef(TRecs_t,                 TRecs);
      K_mf(TRecs*)  get_recs ()  { return static_cast<TRecs*>(&Recs_); }
      #ifdef __TransLnDB__ //如果需要從 KPTS的LnDB 轉入登入資料庫時,才需要開放底下的 members
      typedef TRecs::iterator    iterator;
      typedef inherited::TInn    TInn;
      inline K_mf(void)  peek_rooms  (TRecs& recs, TInn& inn)  { inherited::peek_rooms(recs,inn); }
      inline K_mf(void)  insert      (iterator s, iterator e)  { Recs_.insert(s,e); }
      #endif
   };
   //---------------------------------------------------------------------------
   TUFRecorder    Recorder_;
   TPmUFromGroup* UFromGroup_;

protected:
   typedef typename TUFRecorder::TRecs    TRecs;
   inline K_mf(TRecs&)                    recs  ()    { return *Recorder_.get_recs(); }
   inline K_mf(typename TRecs::iterator)  begin ()    { return recs().begin();        }
   inline K_mf(typename TRecs::iterator)  end   ()    { return recs().end();          }
   K_mf(const TUFRec*)  find  (const TUFKey& key, typename TRecs::iterator* iresult)
      { typename TRecs::iterator i = recs().find(key);
        if(iresult)  *iresult = i;
        return(i==recs().end() ? 0 : &(i->second)); }

public:
   K_ctor  TPmUFromBase  (const std::string& fileName, rr_size roomSize)
      : Recorder_(fileName,roomSize),
        UFromGroup_(0)
      {
      }

// K_mf(const TUFRec*)  GetPolicy  (const TUFKey&, bool forLocalUse) const;

   inline K_mf(void)            SetUFromGroup  (TPmUFromGroup& a)   { UFromGroup_ = &a;   }
   inline K_mf(TPmUFromGroup*)  GetUFromGroup  () const             { return UFromGroup_; }

protected:
   //給 ViewAreaMgr 管理用的 members
   template <class TFieldMaker, class TVALogger>
   K_mf(TaViewArea)  CreateNewVA_T  (const TFieldMaker& fieldMaker, TVALogger* log)
   {
      typedef TVAMkKey   TKeyMaker;
      class TVA : public TTViewArea<TRecs,TFieldMaker,TKeyMaker,TVALogger> {
         //若 TPmUFromGroup->GroupKinds 有異動,則整個ViewArea更新畫面!
         //當然,您也可以根據異動的資料尋找要更新的紀錄,不過太麻煩了!
         //況且畫面上的資料也不會太多,所以整個更新,不至造成太大的Loading!
         typedef TTViewArea<TRecs,TFieldMaker,TKeyMaker,TVALogger>  inherited;
         class THdr//TGroupKindsHandler
                  : public Kstl::container_ev_handler {
            TVA*           va_;
            TPmUFromGroup* UFromGroup_;
            K_mf(void)  OnContainerBfDestroy    (const ev_container&)     { UFromGroup_ = 0; }
            K_mf(void)  OnContainerAfClear      (const ev_container&)     { va_->Repaint();  }
            K_mf(void)  OnContainerAfAssign     (const ev_container&)     { va_->Repaint();  }
            K_mf(void)  OnContainerAfInsert     (const ev_container&, const ev_iterator&, size_type)  { va_->Repaint(); }
            K_mf(void)  OnContainerAfErase      (const ev_container&, const ev_iterator&, size_type)  { va_->Repaint(); }
            K_mf(void)  OnContainerAfEndUpdate  (const ev_updater_impl&)  { va_->Repaint(); }
         public:
            K_ctor  THdr//TGroupKindsHandler
                    (TVA* va, TPmUFromGroup* UFromGroup)
                        : va_(va), UFromGroup_(UFromGroup)
                        { if(UFromGroup)       UFromGroup->GroupKinds_add_ev_handler(this);  }
            virtual K_dtor ~THdr()//TGroupKindsHandler  ()
                        { if(UFromGroup_)      UFromGroup_->GroupKinds_del_ev_handler(this); }
         };
         friend class THdr;//TGroupKindsHandler;
           THdr                GroupPolicyHandler_;
         //TGroupKindsHandler  GroupPolicyHandler_;
      public:
         K_ctor  TVA  (TRecs* recs, const TFieldMaker& fieldMaker, TPmUFromGroup* PmUFromGroup, TVALogger* log)
                  : inherited(recs,fieldMaker,inherited::DefaultViewControls(),log),
                    GroupPolicyHandler_(this, PmUFromGroup)
                  { }
      };
      return TaViewArea(new TVA(Recorder_.get_recs(), fieldMaker, UFromGroup_, log));
   }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
#endif

