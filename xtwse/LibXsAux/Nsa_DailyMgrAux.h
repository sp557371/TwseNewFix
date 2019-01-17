/////////////////////////////////////////////////////////////////////////////
#if !defined(_NsaDailyMgrAux_h_)
#define _NsaDailyMgrAux_h_
//---------------------------------------------------------------------------
#include "Nsa_DailyMgr.h"
#include "tplns/Nsa_EvMap_2k.h"
#include "libnsAux.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::TSetting
/////////////////////////////////////////////////////////////////////////////
class TDailyMgr::TSetting
{
public:
   //==============================================
   // TRecsType
   typedef TEvMap2k<TPoliName, TConfKey, SDmrSetting, SDmrPolicy>  TRecsType;
   typedef TRecsType::TMap1       TRsPoliName;
   typedef TRsPoliName::iterator  TRsPoliNameI;
   typedef TRsPoliName::updater   TRsPoliNameU;
   typedef TRecsType::TMap2       TRsConfKey;
   typedef TRsConfKey::iterator   TRsConfKeyI;

private:
   class TRecsRecorder;
   class TRsPoliNameFm;
   class TRsPoliNameVAC;
   class TRsConfKeyFm1;
   class TRsConfKeyFm2;
   class TRsConfKeyVAC;

   TDailyMgr*       DailyMgr_;
   TRsPoliName      RsPoliName_;
   TRecsRecorder*   RecsRecorder_;
   TRsPoliNameVAC*  RsPoliNameVAC_;
   TRsConfKeyVAC*   RsConfKeyVAC_;

public:
   K_ctor  TSetting (TDailyMgr*);
   K_dtor ~TSetting ();

   K_mf(bool) SettingApply (TRsPoliName*);
   K_mf(bool) SettingApply (TRsConfKey*);

   K_mf(void) AfSettingApply (TRsPoliName*);
   K_mf(void) AfSettingApply (TRsConfKey*);

   K_mf(TVACreater&) GetRsPoliNameVAC ();
   K_mf(TVACreater&) GetRsConfKeyVAC  ();

   inline K_mf(TRsPoliName*) GetRsPoliName ()  { return &RsPoliName_; }
};


/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::TSetting
/////////////////////////////////////////////////////////////////////////////
class TDailyMgr::TStatus
{
public:
   //==============================================
   // TRecsType
   typedef Kstl::ev_map<TPoliName, SDmrStatus>  TRecs;
   typedef TRecs::iterator                      TRecsI;
   typedef TRecs::updater                       TRecsU;

public:
   K_ctor  TStatus ();
   K_dtor ~TStatus ();

   K_mf(void) Update (const TPoliName&, const SDmrStatus&);

   K_mf(Kway::TVACreater&) GetVAC ();

   inline K_mf(TRecs*) GetRecs ()  { return &Recs_; }

private:
   class TFieldMaker;
   class TRecsVAC;

   TRecs      Recs_;
   TRecsVAC*  RecsVAC_;
};


/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr::THoliday
/////////////////////////////////////////////////////////////////////////////
class TDailyMgr::THoliday
{
public:
   //==============================================
   // TRecsType
   typedef TEvMap2k<TPoliName, TKDate, SDmrHoliday>  TRecsType;
   typedef TRecsType::TMap1       TRsPoliName;
   typedef TRsPoliName::iterator  TRsPoliNameI;
   typedef TRecsType::TMap2       TRsDate;
   typedef TRsDate::iterator      TRsDateI;

private:
   class TRecsRecorder;
   class TFieldMaker1;
   class TFieldMaker2;
   class TRecsVAC;

   TDailyMgr*      DailyMgr_;
   TRsPoliName     RsPoliName_;
   TRecsRecorder*  RecsRecorder_;
   TRecsVAC*       RecsVAC_;

public:
   K_ctor  THoliday (TDailyMgr*);
   K_ctor ~THoliday ();

   K_mf(bool) SettingApply   (TRsDate*);
   K_mf(void) AfSettingApply (TRsDate*);

   // 如果資料表裡面沒有設定資料 ==> 為交易日
   // 如果 Holiday_ == false     ==> 為交易日
   //                            ==> 為假日
   K_mf(bool) IsHoliday (const TPoliName&, const TKDate&);

   // 尋找資料表裡面有設定資料
   // 而且 Holiday_ == false
   K_mf(bool) IsWorkingDay (const TPoliName&, const TKDate&);

   K_mf(Kway::TVACreater&) GetVAC ();

   inline K_mf(TRsPoliName*) GetRecs ()  { return &RsPoliName_; }
};


/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_NsaDailyMgrAux_h_)

