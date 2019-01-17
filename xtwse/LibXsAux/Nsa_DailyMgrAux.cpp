/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_DailyMgrAux.h"
#include "ev_map.h"
#include "Nsa_UtilStr.h"
#include "tplns/Nsa_EvMap_0Util.h"
#include "tplns/Nsa_EvMap_1k_Setting.h"
#include "tplns/Nsa_EvCtn_1k_VA.h"
#include "tplns/Nsa_EvMap_2k_File.h"
#include "tplns/Nsa_EvMap_2k1d_Setting.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::TSetting::TRsConfKeyReco
/////////////////////////////////////////////////////////////////////////////
typedef TEvMap2kFile<TDailyMgr::TSetting::TRsPoliName>  TDailyMgrSettingRecoType;
//---------------------------------------------------------------------------
class TDailyMgr::TSetting::TRecsRecorder
{
   TDailyMgrSettingRecoType::TRecorder1  Reco1_;
   TDailyMgrSettingRecoType::TRecorder2  Reco2_;

public:
   inline K_ctor TRecsRecorder (TRsPoliName* c, const char* fp1, const char* fp2)
      : Reco1_(c, std::string(fp1))
      , Reco2_(c, std::string(fp2))
   {}
};
/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::TSetting impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TDailyMgr::TSetting::TSetting (TDailyMgr* dm)
   : DailyMgr_     (dm)
   , RsPoliNameVAC_(NULL)
   , RsConfKeyVAC_ (NULL)
{
   RecsRecorder_ = new TRecsRecorder(&RsPoliName_, "./Settings/NsaDmConf.bin", "./Settings/NsaDmConfDet.bin");

   // 確認是否有 [系統預設]
   TRsPoliNameI iter = RsPoliName_.find(DefPolicy);
   if (iter == RsPoliName_.end()) {
      TRsPoliNameU upd = RsPoliName_.begin_update(RsPoliName_.fetch(DefPolicy));

      upd->PolicyDesc_.assign("系統預設使用");
   }
}
//---------------------------------------------------------------------------

K_mf(bool) TDailyMgr::TSetting::SettingApply (TRsPoliName*)
{
   DailyMgr_->StopCheck();
   return true;
}
//---------------------------------------------------------------------------

K_mf(bool) TDailyMgr::TSetting::SettingApply (TRsConfKey*)
{
   DailyMgr_->StopCheck();
   return true;
}
//---------------------------------------------------------------------------

K_mf(void) TDailyMgr::TSetting::AfSettingApply (TRsPoliName*)
{
   // 因為這邊有可能新增 or 移除 TPoliName, 所以要同步一下其餘資料表
   TEvMapUtil::Sync_Key(&RsPoliName_, DailyMgr_->GetStatus ()->GetRecs());
   TEvMapUtil::Sync_Key(&RsPoliName_, DailyMgr_->GetHoliday()->GetRecs());

   DailyMgr_->SettingApply();
}
//---------------------------------------------------------------------------

K_mf(void) TDailyMgr::TSetting::AfSettingApply (TRsConfKey*)
{
   DailyMgr_->SettingApply();
}


/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::TSetting::TRsPoliNameFm
/////////////////////////////////////////////////////////////////////////////
class TDailyMgr::TSetting::TRsPoliNameFm
{
   typedef TRsPoliName          container;
   typedef container::iterator  iterator;
   typedef container::updater   updater;
   enum {
      IField_PolicyName,
         IField_TradingRule,
         IField_PolicyDesc,
      IField_Total
   };

public:
   K_mf(const Kway::TFieldDesp&) GetFieldDesp (size_t n) const;
   K_mf(std::string)             Get          (const iterator& i, size_t fieldNo) const;
   K_mf(void)                    Put          (container& c, iterator& i, size_t fieldNo, const std::string& str);

   inline K_mf(size_t) GetFieldCount () const  { return IField_Total; }
};
/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::TSetting::TRsPoliNameFm impl
/////////////////////////////////////////////////////////////////////////////
static const char* TradingRuleSelects[] = {
   "1.T_Date",
   "2.T+1_Date",
};
//---------------------------------------------------------------------------
struct TTradingRuleEditProp : public Kway::TVAGetEditProp
{
   K_mf(TEditStyle) GetEnums (const Kway::TViewArea&, const Kway::TViewCursor*, size_t, Kway::TStrings* result, bool& selectOnly)
   {
      if (result)
         std::copy(TradingRuleSelects, TradingRuleSelects+numofele(TradingRuleSelects), std::back_inserter(*result));
      selectOnly = true;
      return es_AllRecsEqual;
   }
};
//---------------------------------------------------------------------------
static TTradingRuleEditProp  TradingRuleEditProp;
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&) TDailyMgr::TSetting::TRsPoliNameFm::GetFieldDesp (size_t n) const
{
   static Kway::TFieldDesp FdDsp[] = {
      { "資源代號",   12, Kway::ta_xLeft_yCenter,   Kway::ta_xLeft_yCenter,   Kway::tfs_Fixed,                           },
      { "交易日基準", 15, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, 0, &TradingRuleEditProp },
      { "資源敘述",   20, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit,                         },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------

K_mf(std::string) TDailyMgr::TSetting::TRsPoliNameFm::Get (const iterator& i, size_t fieldNo) const
{
   switch (fieldNo) {
      case IField_PolicyName:
         {
            if (i->first == TDailyMgr::DefPolicy)
               return std::string("系統預設");
            return i->first.as_string();
         }

      case IField_TradingRule:  return i->second.TradingRule_.as_string();
      case IField_PolicyDesc:   return i->second.PolicyDesc_ .as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------

K_mf(void) TDailyMgr::TSetting::TRsPoliNameFm::Put (container& c, iterator& i, size_t fieldNo, const std::string& str)
{
   if (fieldNo == IField_PolicyName)
      return;

   updater u = c.begin_update(i);
   switch (fieldNo) {
      case IField_TradingRule:  u->TradingRule_.assign(str);  break;
      case IField_PolicyDesc:   u->PolicyDesc_ .assign(str);  break;
   }
}
/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::TSetting::TRsConfKeyFm1
/////////////////////////////////////////////////////////////////////////////
class TDailyMgr::TSetting::TRsConfKeyFm1
{
   typedef TRsPoliName          container;
   typedef container::iterator  iterator;
   enum {
      IField_PolicyName,
      IField_Total
   };

public:
   K_mf(const Kway::TFieldDesp&) GetFieldDesp (size_t n) const;
   K_mf(std::string)             Get          (const iterator& i, size_t fieldNo) const;

   inline K_mf(size_t) GetFieldCount () const  { return IField_Total; }
   inline K_mf(void)   Put           (container&, iterator&, size_t, const std::string&) {}
};
/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::TSetting::TRsConfKeyFm1 impl
/////////////////////////////////////////////////////////////////////////////
K_mf(const Kway::TFieldDesp&) TDailyMgr::TSetting::TRsConfKeyFm1::GetFieldDesp (size_t n) const
{
   static Kway::TFieldDesp FdDsp[] = {
      { "資源代號", 12, Kway::ta_xLeft_yCenter, Kway::ta_xLeft_yCenter, Kway::tfs_Fixed, },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------

K_mf(std::string) TDailyMgr::TSetting::TRsConfKeyFm1::Get (const iterator& i, size_t fieldNo) const
{
   switch (fieldNo) {
      case IField_PolicyName:
         {
            if (i->first == TDailyMgr::DefPolicy)
               return std::string("系統預設");
            return i->first.as_string();
         }
   }
   return std::string();
}
/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::TSetting::TRsConfKeyFm2
/////////////////////////////////////////////////////////////////////////////
class TDailyMgr::TSetting::TRsConfKeyFm2
{
   typedef TRsConfKey           container;
   typedef container::iterator  iterator;
   typedef container::updater   updater;
   enum {
      IField_ConfKey,
         IField_Use,
         IField_WeekDays,
         IField_StTime,
         IField_EdDay,
         IField_EdTime,
         IField_ExecTime,
      IField_Total
   };

public:
   K_mf(const Kway::TFieldDesp&) GetFieldDesp (size_t n) const;
   K_mf(std::string)             Get          (const iterator& i, size_t fieldNo) const;
   K_mf(void)                    Put          (container& c, iterator& i, size_t fieldNo, const std::string& str);

   inline K_mf(size_t) GetFieldCount () const  { return IField_Total; }
};
/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::TSetting::TRsConfKeyFm2 impl
/////////////////////////////////////////////////////////////////////////////
static const char* DayIDSelects[] = {
   "1.當日",
   "2.隔一日",
   "3.隔二日",
   "4.隔三日",
};
//---------------------------------------------------------------------------
struct TDayIDEditProp : public Kway::TVAGetEditProp
{
   K_mf(TEditStyle) GetEnums (const Kway::TViewArea&, const Kway::TViewCursor*, size_t, Kway::TStrings* result, bool& selectOnly)
   {
      if (result)
         std::copy(DayIDSelects, DayIDSelects+numofele(DayIDSelects), std::back_inserter(*result));
      selectOnly = true;
      return es_AllRecsEqual;
   }
};
//---------------------------------------------------------------------------
static TDayIDEditProp  DayIDEditProp;
//---------------------------------------------------------------------------

K_mf(const Kway::TFieldDesp&) TDailyMgr::TSetting::TRsConfKeyFm2::GetFieldDesp (size_t n) const
{
   static Kway::TFieldDesp FdDsp[] = {
      { "設定名稱",   10, Kway::ta_xLeft_yCenter,   Kway::ta_xLeft_yCenter,   Kway::tfs_Fixed,                           },
      { "啟用",        6, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit,                         },
      { "星期設定",   12, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit,                         },
      { "開始時間",   10, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit,                         },
      { "結束日期",   10, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, 0, &DayIDEditProp       },
      { "結束時間",   10, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit,                         },
      { "清盤時間",   10, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit,                         },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------

K_mf(std::string) TDailyMgr::TSetting::TRsConfKeyFm2::Get (const iterator& i, size_t fieldNo) const
{
   switch (fieldNo) {
      case IField_ConfKey:      return i->first.as_string();
      case IField_Use:          return i->second.Use_        .as_string();
      case IField_WeekDays:     return i->second.WeekDays_   .as_string();
      case IField_StTime:       return i->second.StTime_     .as_string();
      case IField_EdDay:        return std::string(DayIDSelects[i->second.EdDay_.get_orig()]);
      case IField_EdTime:       return i->second.EdTime_     .as_string();
      case IField_ExecTime:     return i->second.ExecTime_   .as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------

K_mf(void) TDailyMgr::TSetting::TRsConfKeyFm2::Put (container& c, iterator& i, size_t fieldNo, const std::string& str)
{
   if (fieldNo == IField_ConfKey)
      return;

   updater u = c.begin_update(i);
   switch (fieldNo) {
      case IField_Use:       u->Use_     .assign(str);  break;
      case IField_WeekDays:  u->WeekDays_.assign(str);  break;
      case IField_StTime:    u->StTime_  .assign(str);  break;
      case IField_EdDay:
         {
            u->EdDay_.assign(str);
            u->EdDay_--;
            if (u->EdDay_.get_orig() > numofele(DayIDSelects))
               u->EdDay_.assign0();
         }
         break;
      case IField_EdTime:       u->EdTime_     .assign(str);  break;
      case IField_ExecTime:     u->ExecTime_   .assign(str);  break;
   }
}
/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::TSetting::TRsPoliNameVAC
/////////////////////////////////////////////////////////////////////////////
class TDailyMgr::TSetting::TRsPoliNameVAC : public TEvMap1kSetting<TSetting, TRsPoliName, TRsPoliNameFm>
{
   typedef TEvMap1kSetting<TSetting, TRsPoliName, TRsPoliNameFm>  inherited;

public:
   inline K_ctor TRsPoliNameVAC (TSetting* o, TRsPoliName* c)
      : inherited(o, c)
   {}
};
/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::TSetting::TRsConfKeyVAC
/////////////////////////////////////////////////////////////////////////////
class TDailyMgr::TSetting::TRsConfKeyVAC : public TEvMap2k1dSetting<TSetting, TRsPoliName, TRsConfKeyFm1, TRsConfKeyFm2>
{
   typedef TEvMap2k1dSetting<TSetting, TRsPoliName, TRsConfKeyFm1, TRsConfKeyFm2>  inherited;

public:
   inline K_ctor TRsConfKeyVAC (TSetting* o, TRsPoliName* c)
      : inherited(o, c)
   {}
};
/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::TSetting impl
/////////////////////////////////////////////////////////////////////////////
K_dtor TDailyMgr::TSetting::~TSetting ()
{
   if (RsConfKeyVAC_)   delete RsConfKeyVAC_;
   if (RsPoliNameVAC_)  delete RsPoliNameVAC_;

   delete RecsRecorder_;
}
//---------------------------------------------------------------------------

K_mf(TVACreater&) TDailyMgr::TSetting::GetRsPoliNameVAC ()
{
   if (RsPoliNameVAC_ == NULL)
      RsPoliNameVAC_ = new TRsPoliNameVAC(this, &RsPoliName_);
   return *RsPoliNameVAC_;
}
//---------------------------------------------------------------------------

K_mf(TVACreater&) TDailyMgr::TSetting::GetRsConfKeyVAC ()
{
   if (RsConfKeyVAC_ == NULL)
      RsConfKeyVAC_ = new TRsConfKeyVAC(this, &RsPoliName_);
   return *RsConfKeyVAC_;
}


/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::TStatus impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TDailyMgr::TStatus::TStatus ()
   : RecsVAC_(NULL)
{
}
//---------------------------------------------------------------------------

K_mf(void) TDailyMgr::TStatus::Update (const TPoliName& poliName, const SDmrStatus& status)
{
   TRecsU upd = Recs_.begin_update(Recs_.fetch(poliName));

   *upd = status;
}
/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::TStatus::TFieldMaker
/////////////////////////////////////////////////////////////////////////////
class TDailyMgr::TStatus::TFieldMaker
{
   typedef TRecs                container;
   typedef container::iterator  iterator;
   typedef container::updater   updater;
   enum {
      IField_PolicyName,
         IField_TradeDate,
         IField_TradingStatus,
      IField_Total
   };

public:
   K_mf(const Kway::TFieldDesp&) GetFieldDesp (size_t n) const;
   K_mf(std::string)             Get          (const iterator& i, size_t fieldNo) const;

   inline K_mf(size_t) GetFieldCount () const  { return IField_Total; }
   inline K_mf(void)   Put           (container& c, iterator& i, size_t fieldNo, const std::string& str) {}
};
/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::TStatus::TFieldMaker impl
/////////////////////////////////////////////////////////////////////////////
K_mf(const Kway::TFieldDesp&) TDailyMgr::TStatus::TFieldMaker::GetFieldDesp (size_t n) const
{
   static Kway::TFieldDesp FdDsp[] = {
      { "資源代號", 12, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_Fixed,   },
      { "交易日期", 12, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
      { "交易狀態", 12, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------

K_mf(std::string) TDailyMgr::TStatus::TFieldMaker::Get (const iterator& i, size_t fieldNo) const
{
   switch (fieldNo) {
      case IField_PolicyName:
         {
            if (i->first == TDailyMgr::DefPolicy)
               return std::string("系統預設");
            return i->first.as_string();
         }

      case IField_TradeDate:      return i->second.TradeDate_    .as_string(TKDate::sf_ELong);
      case IField_TradingStatus:  return i->second.TradingStatus_.as_string();
   }
   return std::string();
}
/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::TStatus::TRecsVAC
/////////////////////////////////////////////////////////////////////////////
class TDailyMgr::TStatus::TRecsVAC : public TEvCtn1kVA<TRecs, TFieldMaker>
{
   typedef TEvCtn1kVA<TRecs, TFieldMaker>  inherited;

public:
   inline K_ctor TRecsVAC (TRecs* c) : inherited(c)  {}
};
/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::TStatus impl
/////////////////////////////////////////////////////////////////////////////
K_dtor TDailyMgr::TStatus::~TStatus ()
{
   if (RecsVAC_)
      delete RecsVAC_;
}
//---------------------------------------------------------------------------

K_mf(Kway::TVACreater&) TDailyMgr::TStatus::GetVAC ()
{
   if (RecsVAC_ == NULL)
      RecsVAC_ = new TRecsVAC(&Recs_);
   return *RecsVAC_;
}


/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::THoliday::TRecsRecorder
/////////////////////////////////////////////////////////////////////////////
typedef TEvMap2kFile<TDailyMgr::THoliday::TRsPoliName>  TDailyMgrHolidayRecoType;
//---------------------------------------------------------------------------
class TDailyMgr::THoliday::TRecsRecorder
{
   TDailyMgrHolidayRecoType::TRecorder2  Reco2_;

public:
   inline K_ctor TRecsRecorder (TRsPoliName* c, const char* fp2)
      : Reco2_(c, std::string(fp2))
   {}
};
/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::THoliday impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TDailyMgr::THoliday::THoliday (TDailyMgr* dm)
   : DailyMgr_(dm)
   , RecsVAC_ (NULL)
{
   RecsRecorder_ = new TRecsRecorder(&RsPoliName_, "./Settings/NsaDmHoliday.bin");

   // 確認是否有 [系統預設]
   TRsPoliNameI iter = RsPoliName_.find(DefPolicy);
   if (iter == RsPoliName_.end()) {
      RsPoliName_.fetch(DefPolicy);
   }
}
//---------------------------------------------------------------------------

K_mf(bool) TDailyMgr::THoliday::SettingApply (TRsDate*)
{
   DailyMgr_->StopCheck();
   return true;
}
//---------------------------------------------------------------------------

K_mf(void) TDailyMgr::THoliday::AfSettingApply (TRsDate*)
{
   DailyMgr_->SettingApply();
}
//---------------------------------------------------------------------------

K_mf(bool) TDailyMgr::THoliday::IsHoliday (const TPoliName& poliName, const TKDate& date)
{
   TRsPoliNameI iterPoli = RsPoliName_.find(poliName);
   if (iterPoli == RsPoliName_.end())
      return false;

   TRsDate* rsDate   = iterPoli->second.GetContainer();
   TRsDateI iterDate = rsDate->find(date);
   if (iterDate == rsDate->end())
      return false;

   return iterDate->second.Holiday_.is_true();
}
//---------------------------------------------------------------------------

K_mf(bool) TDailyMgr::THoliday::IsWorkingDay (const TPoliName& poliName, const TKDate& date)
{
   TRsPoliNameI iterPoli = RsPoliName_.find(poliName);
   if (iterPoli == RsPoliName_.end())
      return false;

   TRsDate* rsDate   = iterPoli->second.GetContainer();
   TRsDateI iterDate = rsDate->find(date);
   if (iterDate == rsDate->end())
      return false;

   return iterDate->second.Holiday_.is_false();
}


/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::THoliday::TFieldMaker1
/////////////////////////////////////////////////////////////////////////////
class TDailyMgr::THoliday::TFieldMaker1
{
   typedef TRsPoliName          container;
   typedef container::iterator  iterator;
   enum {
      IField_PolicyName,
      IField_Total
   };

public:
   K_mf(const Kway::TFieldDesp&) GetFieldDesp (size_t n) const;
   K_mf(std::string)             Get          (const iterator& i, size_t fieldNo) const;

   inline K_mf(size_t) GetFieldCount () const  { return IField_Total; }
   inline K_mf(void)   Put           (container&, iterator&, size_t, const std::string&) {}
};
/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::THoliday::TFieldMaker1 impl
/////////////////////////////////////////////////////////////////////////////
K_mf(const Kway::TFieldDesp&) TDailyMgr::THoliday::TFieldMaker1::GetFieldDesp (size_t n) const
{
   static Kway::TFieldDesp FdDsp[] = {
      { "資源代號", 12, Kway::ta_xLeft_yCenter, Kway::ta_xLeft_yCenter, Kway::tfs_Fixed, },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------

K_mf(std::string) TDailyMgr::THoliday::TFieldMaker1::Get (const iterator& i, size_t fieldNo) const
{
   switch (fieldNo) {
      case IField_PolicyName:
         {
            if (i->first == TDailyMgr::DefPolicy)
               return std::string("系統預設");
            return i->first.as_string();
         }
   }
   return std::string();
}
/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::THoliday::TFieldMaker2
/////////////////////////////////////////////////////////////////////////////
class TDailyMgr::THoliday::TFieldMaker2
{
   typedef TRsDate              container;
   typedef container::iterator  iterator;
   typedef container::updater   updater;
   enum {
      IField_Date,
         IField_Holiday,
         IField_Remark,
      IField_Total
   };

public:
   K_mf(const Kway::TFieldDesp&) GetFieldDesp (size_t n) const;
   K_mf(std::string)             Get          (const iterator& i, size_t fieldNo) const;
   K_mf(void)                    Put          (container& c, iterator& i, size_t fieldNo, const std::string& str);

   inline K_mf(size_t) GetFieldCount () const  { return IField_Total; }
};
/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::THoliday::TFieldMaker2 impl
/////////////////////////////////////////////////////////////////////////////
K_mf(const Kway::TFieldDesp&) TDailyMgr::THoliday::TFieldMaker2::GetFieldDesp (size_t n) const
{
   static Kway::TFieldDesp FdDsp[] = {
      { "日期",     12, Kway::ta_xLeft_yCenter,  Kway::ta_xLeft_yCenter,  Kway::tfs_Fixed,   },
      { "非交易日", 10, Kway::ta_xRight_yCenter, Kway::ta_xRight_yCenter, Kway::tfs_CanEdit, },
      { "備註",     15, Kway::ta_xRight_yCenter, Kway::ta_xRight_yCenter, Kway::tfs_CanEdit, },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------

K_mf(std::string) TDailyMgr::THoliday::TFieldMaker2::Get (const iterator& i, size_t fieldNo) const
{
   switch (fieldNo) {
      case IField_Date:     return i->first.as_string(Kway::TKDate::sf_ELong);
      case IField_Holiday:  return i->second.Holiday_.as_string();
      case IField_Remark:   return i->second.Remark_ .as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------

K_mf(void) TDailyMgr::THoliday::TFieldMaker2::Put (container& c, iterator& i, size_t fieldNo, const std::string& str)
{
   if (fieldNo == IField_Date) {
      return;
   }

   updater u = c.begin_update(i);
   switch (fieldNo) {
      case IField_Holiday:  u->Holiday_.assign(str);  break;
      case IField_Remark:   u->Remark_ .assign(str);  break;
   }
}
/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::THoliday::TRecsVAC
/////////////////////////////////////////////////////////////////////////////
class TDailyMgr::THoliday::TRecsVAC : public TEvMap2k1dSetting<THoliday, TRsPoliName, TFieldMaker1, TFieldMaker2>
{
   typedef TEvMap2k1dSetting<THoliday, TRsPoliName, TFieldMaker1, TFieldMaker2>  inherited;

public:
   inline K_ctor TRecsVAC (THoliday* o, TRsPoliName* c)
      : inherited(o, c)
   {}
};
/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::THoliday impl
/////////////////////////////////////////////////////////////////////////////
K_dtor TDailyMgr::THoliday::~THoliday ()
{
   if (RecsVAC_)
      delete RecsVAC_;
   delete RecsRecorder_;
}
//---------------------------------------------------------------------------

K_mf(Kway::TVACreater&) TDailyMgr::THoliday::GetVAC ()
{
   if (RecsVAC_ == NULL)
      RecsVAC_ = new TRecsVAC(this, &RsPoliName_);
   return *RecsVAC_;
}
//---------------------------------------------------------------------------

