/////////////////////////////////////////////////////////////////////////////
#if !defined(_NsaUtilTime_h_)
#define _NsaUtilTime_h_
//---------------------------------------------------------------------------
#include <time.h>
#include <bitset>
#include "fixed_num.h"
#include "kutility.h"
#include "KTime.h"
#include "Nsa_UtilComm.h"

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
// TWeekDaysBase
/////////////////////////////////////////////////////////////////////////////
enum EWeekDay
{
   ewd_Start = 0,
      ewd_Sunday = ewd_Start,
      ewd_Monday,
      ewd_Tuesday,
      ewd_Wednesday,
      ewd_Thursday,
      ewd_Friday,
      ewd_Saturday,
   ewd_Total
};
//---------------------------------------------------------------------------
typedef std::bitset<ewd_Total>  TWeekDaysBase;

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif


/////////////////////////////////////////////////////////////////////////////
// class TWeek2
/////////////////////////////////////////////////////////////////////////////
// 0-Sunday,1-6(星期一～星期六)
LIBNSAUX_class TWeek2 : public fixed_num<Kway::byte>
{
   typedef fixed_num<Kway::byte>  inherited;

public:
   typedef inherited::orig_type  orig_type;

public:
   inline K_ctor TWeek2 (orig_type val = ewd_Start) : inherited(val) {}

   K_mf(orig_type)   as_wday   () const;
   K_mf(std::string) as_string () const;
};


/////////////////////////////////////////////////////////////////////////////
// class TWeek
// 如果將 TWeek 當 key 時, 因為在 assign_int 時會 %7, 這樣會有問題
// 所以請改用 TWeek2
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TWeek : public fixed_num<Kway::byte>
{
   typedef fixed_num<Kway::byte>  inherited;
   typedef TWeek                  this_type;

public:
   typedef inherited::orig_type  orig_type;

public:
   inline K_ctor TWeek (orig_type val = ewd_Start)  { assign_int(val); }

   K_mf(void) assign_int (int);

   K_mf(std::string) as_string () const;
};

/////////////////////////////////////////////////////////////////////////////
// class TWeekDays
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TWeekDays : public TWeekDaysBase
{
   typedef TWeekDaysBase  inherited;

public:
   K_mf(void) assign (const char*);

   K_mf(std::string) as_string () const;

   inline K_mf(void) assign (const std::string& str)  { assign(str.c_str()); }
};


/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// class TTradingStatus
/////////////////////////////////////////////////////////////////////////////
enum ETradingStatus
{
   ets_Unknown,
      ets_WaitOpen,
      ets_Opening,
      ets_Closed,
};
//---------------------------------------------------------------------------
LIBNSAUX_class TTradingStatus : public EnumT<ETradingStatus>
{
   typedef EnumT<ETradingStatus>  inherited;

public:
   inline K_ctor TTradingStatus (ETradingStatus e=ets_Unknown) : inherited(e)  {}

   K_mf(std::string) as_string () const;
};


/////////////////////////////////////////////////////////////////////////////
// class TTradingRule
/////////////////////////////////////////////////////////////////////////////
enum ETradingRule
{
   ets_TDate,     // T   Date
   ets_T1Date,    // T+1 Date
};
//---------------------------------------------------------------------------
LIBNSAUX_class TTradingRule : public EnumT<ETradingRule>
{
   typedef EnumT<ETradingRule>  inherited;

public:
   inline K_ctor TTradingRule (ETradingRule e=ets_TDate) : inherited(e)  {}

   K_mf(void) assign (const char*);
   
   K_mf(std::string) as_string () const;

   inline K_mf(void) assign (const std::string& str)  { assign(str.c_str()); }
};


/////////////////////////////////////////////////////////////////////////////
// struct STimeBlock
/////////////////////////////////////////////////////////////////////////////
struct STimeBlock
{
   typedef Kstl::fixed_num<int>  TDayID;

   TBoolChar     Use_;           // 啟用
   TWeekDays     WeekDays_;      // 星期幾
   TKTime        StTime_;        // 開始時間
   TDayID        EdDay_;         // 結束日期
   TKTime        EdTime_;        // 結束時間
   TKTime        ExecTime_;      // 執行時間
};

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif

/////////////////////////////////////////////////////////////////////////////
// class TTimeBlock
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TTimeBlock
{
public:
   struct TLatestDate
   {
      TKDate  Date_;

      K_mf(bool) SetDate (const TKDate& date)
      {
         if (Date_.IsInvalid() || Date_ < date) {
            Date_ = date;
            return true;
         }
         return false;
      }
   };

private:
   const STimeBlock&  TbData_;
   TKDate             CurrDate_;
   TKTime             CurrTime_;
   int                CurrWeekDay_;
   TKDate             ResuDate_;
   TLatestDate        WaitOpenDate_;
   TLatestDate        ClosedDate_;

public:
   K_ctor TTimeBlock (const STimeBlock&);

   K_mf(ETradingStatus) GetCurrDate (const TKDateTime&);
   
   inline K_mf(const TKDate&) GetResuDate     () const  { return ResuDate_;           }
   inline K_mf(const TKDate&) GetWaitOpenDate () const  { return WaitOpenDate_.Date_; }
   inline K_mf(const TKDate&) GetClosedDate   () const  { return ClosedDate_  .Date_; }

protected:
   K_mf(ETradingStatus) CheckRange (int nDaySt, int nDayEd);
};

/////////////////////////////////////////////////////////////////////////////
// class TTimerCheck
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TTimerCheck
{
	bool	  IsEnable_;
   uint64  NextClock_;
   int     Interval_;
   int     TimeoutCount_;

public:
   K_ctor TTimerCheck (bool isEnable = true);
	K_ctor TTimerCheck (int ms);

	K_mf(void) SetTimeout (int  ms,       bool isClrCnt = false);
   K_mf(void) SetEnable  (bool isEnable, bool isClrCnt = true);

   K_mf(bool) IsTimeoutC () const;
   K_mf(bool) IsTimeout  ();

   inline K_mf(void) Reset (bool isClrCnt = false)
      {
         if (IsEnable_)
            SetTimeout(Interval_, isClrCnt);
      }

   inline K_mf(bool)	IsEnable        () const  { return IsEnable_;     }
	inline K_mf(bool)	IsDisable       () const  { return !IsEnable_;    }
   inline K_mf(int)  GetTimeoutCount () const  { return TimeoutCount_; }
   inline K_mf(int)  GetInterval     () const  { return Interval_;     }
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_NsaUtilTime_h_)
