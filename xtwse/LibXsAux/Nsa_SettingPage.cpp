
   // 請改使用 Nsa_SettPage.cpp

/////////////////////////////////////////////////////////////////////////////
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_SettingPage.h"
#include "tplns/Nsa_EvMap_0File.h"
#include "tplns/Nsa_EvMap_1k_Setting.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TSettingPage::TRecsRecorder
/////////////////////////////////////////////////////////////////////////////
struct TSettingPageRecoRoom
{
   typedef TSettingPage::TRecs   TContainer;
   typedef SSpeDat               TRec;
   typedef TCommInn              TInn;
   
   typedef TContainer::key_type  TCtnKey;

   static K_mf(bool) exec_peek_room (TCtnKey&, TRec&, TInn::rr_room&);
   static K_mf(bool) exec_into_room (TInn::rr_room&, TInn::rr_key&, const TCtnKey&, const TRec&);
};
//---------------------------------------------------------------------------
class TSettingPage::TRecsRecorder : public Nsa::TEvMapReco<TRecs, SSpeDat, TSettingPageRecoRoom>
{
   typedef Nsa::TEvMapReco<TRecs, SSpeDat, TSettingPageRecoRoom>  inherited;
      
public:
   inline K_ctor TRecsRecorder (TRecs* c, const char* filePath)
      : inherited(c, std::string(filePath))
   {}
};


/////////////////////////////////////////////////////////////////////////////
// class TSettingPage impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TSettingPage::TSettingPage (const char* filePath)
   : RecsVAC_(NULL)
{
   RecsRecorder_ = new TRecsRecorder(&Recs_, filePath);

   // SNseSetting 資料結構中, [IsUse_] [Variable_] 是 pointer 型態
   // 所以如果 app 沒有 Reg 時, [IsUse_] [Variable_] 是利用檔案裡面的資料
   // 會造成 pointer 不存在而掛掉, 所以在起始的時候先設為 NULL
   //for (TRecsI ita=Recs_.begin(); ita!=Recs_.end(); ++ita) {
   //   ita->second.IsUse_    = NULL;
   //   ita->second.Variable_ = NULL;
   //}
}
//---------------------------------------------------------------------------

K_mf(TSettingPage::TRecsI) TSettingPage::Reg (const char* item, const char* setting, ESettingPageType spType, void* variable, const char* memo)
{
   std::string strItem(item);
   TRecsI      iter = Recs_.find(strItem);

   if (iter != Recs_.end()) {
      iter->second.Type_     = spType;
      iter->second.Variable_ = variable;

      // 資料有更新在啟動 updater
      if (memo) {
         std::string strMemo(memo);
         if (iter->second.Memo_ != strMemo) {
            TRecsU upd = Recs_.begin_update(iter);
            upd->Memo_ = strMemo;
         }
      }

      return iter;
   }

            iter   = Recs_.fetch(strItem);
   TRecsU   upd    = Recs_.begin_update(iter);
   SSpeDat& UpdDat = *upd;

   UpdDat.Type_     = spType;
   UpdDat.Variable_ = variable;
   UpdDat.Setting_.assign(setting);
   if (memo)
      UpdDat.Memo_.assign(memo);

   return iter;
}
//---------------------------------------------------------------------------

K_mf(bool) TSettingPage::SettingApply (TRecs* recs)
{
   // 新增可在 SettingApply 之後在新增[項目], 所以這邊如果是 NULL 的時候, 不可以移除

   for (TRecsI iter=recs->begin(); iter!=recs->end(); ++iter) {
      if (iter->second.Variable_ == NULL)
         continue;

      Update_Variable(iter->second.Type_, iter->second.Variable_, iter->second.Setting_);
   }

   return true;
}
//---------------------------------------------------------------------------

K_mf(void) TSettingPage::Update_Variable (ESettingPageType spType, void* variable, const std::string& setting)
{
   switch (spType) {
   case spt_Bool:
      {
         bool* varPtr = reinterpret_cast<bool*>(variable);

         if (setting.empty() || toupper(setting[0]) != 'Y')  *varPtr = false;
         else                                                *varPtr = true;
      }
      break;

   case spt_Int:
      {
         typedef Kstl::fixed_num<int>  TInt;

         TInt* varPtr = reinterpret_cast<TInt*>(variable);
         varPtr->assign(setting);
      }
      break;

   case spt_StdStr:
      {
         std::string* varPtr = reinterpret_cast<std::string*>(variable);
         varPtr->assign(setting);
      }
      break;

   case spt_TKTime:
      {
         TKTime* varPtr = reinterpret_cast<TKTime*>(variable);
         varPtr->assign(setting);
      }
      break;
   }
}
//---------------------------------------------------------------------------

/*

K_mf(void) TSettingPage::RegApply (bool* IsUse, const char* cpItem, const char* Setting, ESettingPageType SpType, void* Variable, const char* cpMemo)
{
   TRecsI ita = Reg(IsUse, cpItem, Setting, SpType, Variable, cpMemo);
   
   Update_Variable(ita->second.Type_, ita->second.Variable_, ita->second.Setting_.as_string());
}
//---------------------------------------------------------------------------

K_mf(void) TSettingPage::RegDisable (const char* cpItem)
{
   TSpStr Item(cpItem);
   TRecsI ita = Recs_.find(Item);

   if (ita != Recs_.end()) {
      ita->second.IsUse_    = NULL;
      ita->second.Variable_ = NULL;
   }
}
//---------------------------------------------------------------------------

K_mf(void) TSettingPage::RegRemove (const char* cpItem)
{
   TSpStr Item(cpItem);
   TRecsI ita = Recs_.find(Item);

   if (ita != Recs_.end()) {
      Recs_.erase(ita);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TSettingPage::Update (const char* cpItem, const std::string& strNewSetting)
{
   TSpStr Item(cpItem);
   TRecsI ita = Recs_.find(Item);

   if (ita != Recs_.end() && ita->second.Setting_.as_string() != strNewSetting) {
      TRecsU upd    = Recs_.begin_update(ita);
      SDat&  UpdDat = *upd;

      // 異動設定
      UpdDat.Setting_.assign(strNewSetting);
      // 異動變數
      Update_Variable(UpdDat.Type_, UpdDat.Variable_, strNewSetting);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TSettingPage::Update_Bool (const char* cpItem, bool bNewSetting)
{
   std::string strNewSetting;

   if (bNewSetting)
      strNewSetting.assign("Y");
   else
      strNewSetting.assign("N");

   Update(cpItem, strNewSetting);
}
//---------------------------------------------------------------------------

K_mf(void) TSettingPage::Update_Int  (const char* cpItem, int nNewVal)
{
	Kstl::fixed_num<int> IntObj(nNewVal);

	Update(cpItem, IntObj.as_string());
}
*/


/////////////////////////////////////////////////////////////////////////////
// class TSettingPageFm
/////////////////////////////////////////////////////////////////////////////
class TSettingPageFm
{
   typedef TSettingPage::TRecs  container;
   typedef container::iterator  iterator;
   typedef container::updater   updater;
   enum {
      IField_Item,
         IField_Setting,
         IField_Memo,
      IField_Total
   };

public:
   // GetFieldCount
   inline K_mf(size_t) GetFieldCount () const  { return IField_Total; }
   // GetFieldDesp
   K_mf(const TFieldDesp&) GetFieldDesp (size_t n) const
   {
      static TFieldDesp FdDsp[] = {
         { "項目", 50, ta_xLeft_yCenter,   ta_xLeft_yCenter,   tfs_Fixed,   },
         { "設定", 30, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
         { "備註", 40, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      };
      return FdDsp[n];
   }
   // Get
   K_mf(std::string) Get (const iterator& i, size_t fieldNo) const
   {
      switch (fieldNo) {
         case IField_Item:     return i->first;
         case IField_Setting:  return i->second.Setting_;
         case IField_Memo:     return i->second.Memo_;
      }
      return std::string();
   }
   // Put
   K_mf(void) Put (container& c, iterator& i, size_t fieldNo, const std::string& str)
   {
      if (fieldNo == IField_Item)
         return;

      updater u = c.begin_update(i);
      switch (fieldNo) {
         case IField_Setting:  u->Setting_.assign(str);  break;
         case IField_Memo:     u->Memo_   .assign(str);  break;
      }
   }
};


/////////////////////////////////////////////////////////////////////////////
// class TSettingPage::TRecsVAC
/////////////////////////////////////////////////////////////////////////////
class TSettingPage::TRecsVAC : public TEvMap1kSetting<TSettingPage, TRecs, TSettingPageFm>
{
   typedef TEvMap1kSetting<TSettingPage, TRecs, TSettingPageFm>  inherited;

public:
   inline K_ctor TRecsVAC (TSettingPage* sp, TRecs* rs)
      : inherited(sp, rs)
   {}
};


/////////////////////////////////////////////////////////////////////////////
// class TSettingPage impl
/////////////////////////////////////////////////////////////////////////////
K_dtor TSettingPage::~TSettingPage ()
{
   if (RecsVAC_)  delete RecsVAC_;

   delete RecsRecorder_;
}
//---------------------------------------------------------------------------

K_mf(TVACreater&) TSettingPage::GetVAC ()
{
   if (RecsVAC_ == NULL)
      RecsVAC_ = new TRecsVAC(this, &Recs_);

   return *RecsVAC_;
}


/////////////////////////////////////////////////////////////////////////////
// struct TSettingPageRecoRoom impl
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TSettingPageRecoRoom::exec_peek_room (TCtnKey& key, TRec& rec, TInn::rr_room& room)
{
   char caKey[256], caSetting[256], caMemo[256];

   TInn::rr_live_peek peeker[] = {
      { K_GetLo(&peeker[1].obj_size_, 1), 1 },
      { caKey,                            0 },
      { K_GetLo(&peeker[3].obj_size_, 3), 1 },
      { caSetting,                        0 },
      { K_GetLo(&peeker[5].obj_size_, 5), 1 },
      { caMemo,                           0 },
   };

   bool resu = room.peek(peeker, peeker+numofele(peeker));
   if (resu) {
      key         .assign(caKey,     peeker[1].obj_size_);
      rec.Setting_.assign(caSetting, peeker[3].obj_size_);
      rec.Memo_   .assign(caMemo,    peeker[5].obj_size_);
   }
   return resu;
}
//---------------------------------------------------------------------------

K_mf(bool) TSettingPageRecoRoom::exec_into_room (TInn::rr_room& room, TInn::rr_key& rrkey, const TCtnKey& key, const TRec& rec)
{
   TInn::rr_live_into peeker[] = {
      { K_GetLo(&peeker[1].obj_size_, 1), 1                                                 },
      { key.c_str(),                      static_cast<TInn::rr_size>(key.length())          },
      { K_GetLo(&peeker[3].obj_size_, 1), 1                                                 },
      { rec.Setting_.c_str(),             static_cast<TInn::rr_size>(rec.Setting_.length()) },
      { K_GetLo(&peeker[5].obj_size_, 1), 1                                                 },
      { rec.Memo_.c_str(),                static_cast<TInn::rr_size>(rec.Memo_.length())    },
   };

   return room.into(rrkey, peeker, peeker+numofele(peeker));
}
//---------------------------------------------------------------------------
