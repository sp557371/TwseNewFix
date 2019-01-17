/////////////////////////////////////////////////////////////////////////////
#if !defined(_NsaDailyMgr_h_)
#define _NsaDailyMgr_h_
//---------------------------------------------------------------------------
#include "char_ary.h"
#include "fixed_num.h"
#include "KTime.h"
#include "TimerThread.h"
#include "ViewAreaMgr.h"
#include "Nsa_BaseType.h"
#include "Nsa_GlobObjs.h"
#include "Nsa_UtilComm.h"
#include "Nsa_UtilTime.h"
#include "tplns/Nsa_EvMap.h"
#include "libnsAux.h"

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
// struct SDmrPolicy
/////////////////////////////////////////////////////////////////////////////
struct SDmrPolicy
{
   typedef Kstl::char_ary<char, 20, ' '>  TPolicyDesc;

   TTradingRule  TradingRule_;   // 交易日基準
   TPolicyDesc   PolicyDesc_;
};
/////////////////////////////////////////////////////////////////////////////
// struct SDmrSetting
/////////////////////////////////////////////////////////////////////////////
struct SDmrSetting : STimeBlock
{
   TCommInn::rr_key mutable  RoomKey_;
};
/////////////////////////////////////////////////////////////////////////////
// struct SDmrStatus
/////////////////////////////////////////////////////////////////////////////
struct SDmrStatus
{
   TKDate          TradeDate_;
   TKTime          TradeBegTime_;
   TTradingStatus  TradingStatus_;

   inline K_mf(bool) operator == (const SDmrStatus& a) const
      {
         return (TradeDate_     == a.TradeDate_    &&
                 TradeBegTime_  == a.TradeBegTime_ &&
                 TradingStatus_ == a.TradingStatus_);
      }
};
/////////////////////////////////////////////////////////////////////////////
// struct SDmrHoliday
/////////////////////////////////////////////////////////////////////////////
struct SDmrHoliday
{
   typedef Kstl::char_ary<char, 30, ' '>  TRemark;

   TCommInn::rr_key mutable  RoomKey_;

   TBoolCharYes  Holiday_;
   TRemark       Remark_;
};

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif


/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TDailyMgr : public TGlobObj
{
   typedef TDailyMgr          this_type;
   typedef TTimer<this_type>  TTimerType;

public:
   class TSetting;
   class TStatus;
   class THoliday;
   friend class TSetting;
   friend class TStatus;
   friend class THoliday;

private:
   class TCheckTradeInfo;

   TSetting*          Setting_;
   TStatus*           Status_;
   THoliday*          Holiday_;
   TViewAreaMgr       VAMgr_;
   TTimer<this_type>  Timer_;

public:
   K_ctor  TDailyMgr ();
   K_dtor ~TDailyMgr ();

   inline K_mf(TVACreater&) GetVAC ()  { return VAMgr_; }

   //==============================================
   // static
   static const TPoliName  DefPolicy;

protected:
   K_mf(void) SettingApply ();
   K_mf(void) OnTimer      (TTimerBase*);

   // 檢查今天是否為假日
   // 如果是的話, 就要往前推, 尋找上一交易日
   K_mf(TKDate) GetValidDate (const TPoliName&, const TKDate&);

   // 異動狀態, 資料不一樣時就發動事件
   K_mf(void) UpdateStatus (const TPoliName&, const SDmrStatus&);

   inline K_mf(void)      StopCheck  ()  { Timer_.Stop();   }
   inline K_mf(TStatus*)  GetStatus  ()  { return Status_;  }
   inline K_mf(THoliday*) GetHoliday ()  { return Holiday_; }
};

/////////////////////////////////////////////////////////////////////////////
// class TDailyMgrGoh
/////////////////////////////////////////////////////////////////////////////
class TDailyMgrGoh : public TGlobObjHnd<TDailyMgr>
{
   typedef TGlobObjHnd<TDailyMgr>  inherited;

public:
   inline K_ctor TDailyMgrGoh (bool autoQuery = false)
      : inherited("Nsa.TDailyMgr", autoQuery)
   {}
};

/////////////////////////////////////////////////////////////////////////////
// class TDailyEvHnd
/////////////////////////////////////////////////////////////////////////////
/*
K_class TDailyEvHnd : public TAppEvMgr::TEvHnd
{
   typedef TAppEvMgr::TEvHnd  inherited;

public:
   K_ctor  TDailyEvHnd ();

   static inline K_mf(void) FireDailyOpen    ()  { inherited::SendEvent(aei_DailyOpen);    }
   static inline K_mf(void) FireDailyClose   ()  { inherited::SendEvent(aei_DailyClose);   }
   static inline K_mf(void) FireDailyBfClear ()  { inherited::SendEvent(aei_DailyBfClear); }
   static inline K_mf(void) FireDailyClear   ()  { inherited::SendEvent(aei_DailyClear);   }
   static inline K_mf(void) FireDailyAfClear ()  { inherited::SendEvent(aei_DailyAfClear); }

protected:
   //==============================================
   // overwrite TAppEvMgr::TEvHnd virtual function
   K_mf(TAeResu) OnAppEv (SAppEvPktHdr*);

   //==============================================
   // TDailyEvHnd virtual function
   virtual K_mf(void) OnDailyOpen    () = 0;
   virtual K_mf(void) OnDailyClose   () = 0;
   virtual K_mf(void) OnDailyBfClear () = 0;
   virtual K_mf(void) OnDailyClear   () = 0;
   virtual K_mf(void) OnDailyAfClear () = 0;
};
*/

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_NsaDailyMgr_h_)
