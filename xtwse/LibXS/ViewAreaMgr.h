//
// 透過 TVACreater 產生 TViewArea
// 並交給 TViewAreaMgr 管理
// 同時 TViewAreaMgr 提供 TVACreater 的註冊
//
// 版本: $Id: ViewAreaMgr.h,v 1.7 2005/08/09 05:25:22 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef ViewAreaMgrH
#define ViewAreaMgrH
//---------------------------------------------------------------------------
#include "ViewArea.h"
#include "ev_map.h"
//---------------------------------------------------------------------------
namespace Kway {
/*/////////////////////////////////////////////////////////////////////////*/
LIBNS_class  TViewAreaMgr;
/*/////////////////////////////////////////////////////////////////////////*/
// 負責建立 ViewArea, 及對應的 details ViewArea
LIBNS_class  TVACreater
{
public:
   virtual K_dtor  ~TVACreater  ()  {}

   //當 TViewAreaMgr 解構前, 會使用 OnBfVAMgrDestroy() 通知大家
   //即使您使用不同名稱跟 TViewAreaMgr::Reg() 註冊多次,也只會收到一次通知
   virtual K_mf(void)  OnBfVAMgrDestroy  (const TViewAreaMgr&)  {}

   //呼叫CreateNewVA()的人,必須管理由這兒產生出來的VA
   virtual K_mf(TaViewArea)  CreateNewVA  () = 0;
   inline  K_mf(TaViewArea)  CreateNewVA  (const std::string& vaIdName, TViewArea* parent)
                                             { TaViewArea   va(CreateNewVA().release());
                                               if(va.get()) {
                                                  if(!vaIdName.empty() && va->GetIdName().empty())
                                                     va->SetIdName(vaIdName);
                                                  va->SetParentVA(parent);
                                               }
                                               return va; }

//●底下的 virtual functions, 是為了支援 detail ViewArea 用的, 預設為沒有 detail
   //↓GetDetails()取得此 VACreater 共有多少 detail 資料表
   virtual K_mf(size_t)      GetDetails   (const char**&)   { return 0; }
                         //範例: { static const char* desp[] = { "庫存明細", "委託明細", "成交明細" };
                         //        a = desp;
                         //        return numofele(desp); }
   //↓GetDetailVACreater()取出第 n(detailID) 個 detail 的 VACreater,
   //  您傳回的 VACreater 必須由您自己管理, 且不可任意拋棄!
   virtual K_mf(TVACreater*) GetDetailVACreater (size_t detailID)  { return 0; }

   //↓當 MasterVA(由this CreateNewVA()產生的VA) 發出 OnViewRowMoved() 事件時, 要如何處理 detail:
   //  (1) 改變 detail 的 VACreater : 若傳回值不是 DetailVACreaterNoChange,
   //                                 則表示要改變 detail 的 VACreater
   //                                 [參數:detailVA]的值變成 don't care
   //  (2) 產生新的 VA              : 修改 [參數:detailVA]
   //  (3) 改變 VA 對應的 Container : 設定 detailVA->SetContainer() 即可
   //      (2)與(3) 皆傳回 DetailVACreaterNoChange
   //  ↓您不可以刪除 detailVA
   static TVACreater* const   DetailVACreaterNoChange;
   virtual K_mf(TVACreater*)  OnMasterViewRowMoved  (TViewArea& masterVA, TViewArea*& detailVA, size_t detailID)
                                                      { return DetailVACreaterNoChange; }
};
/*/////////////////////////////////////////////////////////////////////////*/
template <class TRecs>
struct TVAKeyFieldMaker
{
   typedef TRecs                             TEvContainer;
   typedef typename TEvContainer::iterator   iterator;
   typedef size_t                            field_n_type;

// K_mf(const TFieldDesp&)  GetFieldDesp   (field_n_type n) const;  還是必須由您自己來完成
   K_mf(field_n_type)       GetFieldCount  () const                 { return 1; }

   K_mf(std::string) Get  (const iterator& i, field_n_type) const   { return TEvContainer::get_key(i).as_string(); }
   K_mf(void)        Put  (TEvContainer&, iterator&, field_n_type, const std::string&) {}
}; // class TVAKeyFieldMaker
/*/////////////////////////////////////////////////////////////////////////*/
// 負責管理一群 TVACreater
// TViewAreaMgr 本身也是一個 TVACreater
LIBNS_class  TViewAreaMgr : public TVACreater
{
   typedef Kstl::ev_map<std::string,TVACreater*>   TVACreaters_t;
   K_typedef(TVACreaters_t,                        TVACreaters);
   typedef TVACreaters::iterator                   TVACreateri;
   TVACreaters    VACreaters_;
   struct TFieldMaker : public TVAKeyFieldMaker<TVACreaters>
   {
      TViewAreaMgr*   VAMgr_;
      K_ctor TFieldMaker(TViewAreaMgr& mgr)  : VAMgr_(&mgr) {
      }
      K_mf(const TFieldDesp&)  GetFieldDesp  (field_n_type) const;
      K_mf(std::string)        Get           (const iterator& i, field_n_type) const;
   }; // struct TFieldMaker
   friend struct TFieldMaker;
   std::string DetailName_;
   const char* DetailNames_[1];
   std::string FieldTitle_;
   TFieldDesp  FieldDesp_;
public:
   K_ctor  TViewAreaMgr (const std::string& fieldTitle = std::string(), const std::string& detailName = std::string());
   K_dtor ~TViewAreaMgr ();  //通知所有註冊的VACreater

   K_mf(void) SetFieldTitle (const std::string& fieldTitle);

   //註冊/刪除 TVACreater
   K_mf(bool)  Add  (const std::string& evcName, TVACreater&);
   K_mf(bool)  Del  (const std::string& evcName, TVACreater&);
   K_mf(bool)  Del  (TVACreater&);
   K_mf(bool)  Find (TVACreater&);
   //取出註冊資訊
   //  取得全部的註冊名稱
   K_mf(bool)  GetRegNames   (Kway::TStrings&) const;
   //  取得某註冊名稱,全部資料的Keys
   K_mf(bool)  GetDetailKeys (const std::string& evcName, Kway::TStrings&) const;
   //  取得某註冊名稱,全部資料的Keys
   K_mf(TVACreater*)  GetReg (const std::string& evcName) const;

   //繼承自 TVACreater 的 methods
   K_mf(TaViewArea)   CreateNewVA           ();
   K_mf(size_t)       GetDetails            (const char**&);
   K_mf(TVACreater*)  GetDetailVACreater    (size_t);
   K_mf(TVACreater*)  OnMasterViewRowMoved  (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);

   typedef TTViewArea<TVACreaters,TFieldMaker>    TMgrVA;
   K_mf(TVACreater*)  GetDetailVACreater    (TMgrVA&);

   //public types
   //--------------------------------------------------
   class TVAGetEditProp_GetNames : public TVAGetEditProp
   {  //取得 VAMgr 全部的[註冊名稱], 當作輸入時的[列舉]
      TViewAreaMgr&  VAMgr_;
   public:
      inline K_ctor  TVAGetEditProp_GetNames  (TViewAreaMgr& vaMgr)  : VAMgr_(vaMgr)  { }
      K_mf(TEditStyle)  GetEnums  (const TViewArea&, const TViewCursor*, size_t, TStrings* result, bool&)
                                    { if(result)   VAMgr_.GetRegNames(*result);
                                      return es_AllRecsEqual; }
   };
   /*--------------------------------------------------*/
   class TVAGetEditProp_GetKeys : public TVAGetEditProp
   {  //取得 VAMgr 某[註冊名稱] [全部的Keys], 當作輸入時的[列舉]
      TViewAreaMgr&  VAMgr_;
   public:
      inline K_ctor  TVAGetEditProp_GetKeys  (TViewAreaMgr& vaMgr)  : VAMgr_(vaMgr)  { }
      K_mf(TEditStyle)  GetEnums  (const TViewArea&, const TViewCursor* cur, size_t, TStrings* result, bool&)
                                    { if(result  &&  cur)
                                         VAMgr_.GetDetailKeys(cur->GetStr(0), *result);
                                      return es_EnumsByKey; }
   };

   static const char       DefaultDetailName[];  // = "資料表";
   static const TFieldDesp DefaultDetailField;
};
/*/////////////////////////////////////////////////////////////////////////*/
} // namespace Kway
//---------------------------------------------------------------------------
#endif

