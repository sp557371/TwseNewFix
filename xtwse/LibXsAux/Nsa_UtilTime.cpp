/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_UtilTime.h"
#include "TimerThread.h"
#include "Nsa_UtilStr.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TWeek2 impl
/////////////////////////////////////////////////////////////////////////////
K_mf(TWeek2::orig_type) TWeek2::as_wday () const
{
   orig_type val = inherited::get_orig();

   if (val >= ewd_Total)
      val %= 7;

   return val;
}
//---------------------------------------------------------------------------

K_mf(std::string) TWeek2::as_string () const
{
   orig_type val = as_wday();

   switch (val) {
   case 0:  return std::string("星期日");
   case 1:  return std::string("星期一");
   case 2:  return std::string("星期二");
   case 3:  return std::string("星期三");
   case 4:  return std::string("星期四");
   case 5:  return std::string("星期五");
   case 6:  return std::string("星期六");
   }
   return std::string();
}


/////////////////////////////////////////////////////////////////////////////
// class TWeek impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TWeek::assign_int (int val)
{
   if (val >= ewd_Total)
      val %= 7;
   
   inherited::assign(val);
}
//---------------------------------------------------------------------------

K_mf(std::string) TWeek::as_string () const
{
   orig_type n = inherited::get_orig();

   switch (n) {
   case 0:  return std::string("星期日");
   case 1:  return std::string("星期一");
   case 2:  return std::string("星期二");
   case 3:  return std::string("星期三");
   case 4:  return std::string("星期四");
   case 5:  return std::string("星期五");
   case 6:  return std::string("星期六");
   }
   return std::string();
}


/////////////////////////////////////////////////////////////////////////////
// class TWeekDays impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TWeekDays::assign (const char* str)
{
   inherited::reset();
   if (str == NULL) {
      return;
   }

   TStrSplit sp(str, ',');
   size_t    size = sp.GetSize();
   EWeekDay  day;

   // 0,1,4,6
   for (size_t i=0; i<size; ++i) {
      day = (EWeekDay)sp.GetInt(i);
      if (day >= ewd_Sunday && day < ewd_Total) {
         inherited::set(day);
      }
   }
}
//---------------------------------------------------------------------------

K_mf(std::string) TWeekDays::as_string () const
{
   char   buf[128], tmp[20];
   size_t nBeg = (size_t)ewd_Sunday;
   size_t nEnd = (size_t)ewd_Total;

   buf[0] = '\0';
   for (size_t day=nBeg; day<nEnd; ++day) {
      if (inherited::test(day)) {
         sprintf(tmp, "%d,", (int)day);
         strcat(buf, tmp);
      }
   }

   size_t len = strlen(buf);
   if (len > 0) {
      // 去掉最後一個 ','
      buf[len-1] = '\0';
   }

   return std::string(buf);
}


/////////////////////////////////////////////////////////////////////////////
// class TTradingStatus impl
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TTradingStatus::as_string () const
{
   switch (Data_) {
      case ets_WaitOpen:  return std::string("WaitOpen");
      case ets_Opening:   return std::string("Opening");
      case ets_Closed:    return std::string("Closed");
   }
   return std::string();
}


/////////////////////////////////////////////////////////////////////////////
// class TTradingRule impl
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TTradingRule::as_string () const
{
   switch (Data_) {
      case ets_TDate:   return std::string("1.T_Date");
      case ets_T1Date:  return std::string("2.T+1_Date");
   }
   return std::string();
}
//---------------------------------------------------------------------------

K_mf(void) TTradingRule::assign (const char* str)
{
   Data_ = ets_TDate;
   if (str != NULL) {
      int val = atoi(str);
      if (val != 1 && val != 2)
         val = 1;
      
      Data_ = (ETradingRule)(val - 1);
   }
}


/////////////////////////////////////////////////////////////////////////////
// class TTimeBlock impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTimeBlock::TTimeBlock (const STimeBlock& tb)
   : TbData_(tb)
{
}
//---------------------------------------------------------------------------

K_mf(ETradingStatus) TTimeBlock::GetCurrDate (const TKDateTime& dateTime)
{
   ResuDate_          .Clear();
   WaitOpenDate_.Date_.Clear();
   ClosedDate_  .Date_.Clear();

   if (TbData_.Use_.is_false() || TbData_.WeekDays_.any() == false) {
      return ets_Unknown;
   }

   CurrDate_    = dateTime.GetDate();
   CurrTime_    = dateTime.GetTime();
   CurrWeekDay_ = (int)CurrDate_.GetWDay();

   int            daySt, dayEd;
   ETradingStatus ts     = ets_Unknown;
   int            incDay = TbData_.EdDay_.get_orig();
   size_t         beg    = (size_t)ewd_Sunday;
   size_t         end    = (size_t)ewd_Total;

   for (size_t day=beg; day<end; ++day) {
      if (TbData_.WeekDays_.test(day) == false) {
         continue;
      }

      daySt = (int)day;
      dayEd = daySt + incDay;
      // 0,1,2,3,4,5,6
      if (dayEd > 6)
         dayEd -= 7;

      ResuDate_ = CurrDate_;
      ts        = CheckRange(daySt, dayEd);

      switch (ts) {
         case ets_Opening:   return ts;
         case ets_WaitOpen:  WaitOpenDate_.SetDate(ResuDate_);  break;
         case ets_Closed:    ClosedDate_  .SetDate(ResuDate_);  break;
      }
   }

   return ts;
}
//---------------------------------------------------------------------------

K_mf(ETradingStatus) TTimeBlock::CheckRange (int daySt, int dayEd)
{
   //==============================================
   // [開始日期]與今天相同
   if (daySt == CurrWeekDay_) {
      //------------------------------
      // [開始時間]尚未到達
      if (CurrTime_ < TbData_.StTime_) {
         // 超過[執行時間]
         if (TbData_.ExecTime_.IsInvalid() == false && CurrTime_ > TbData_.ExecTime_) {
            return ets_WaitOpen;
         }
         // 看起來還在[上一交易日]的[Closed]
         ResuDate_.IncDay(-1);
         return ets_Closed;
      }

      //------------------------------
      // 結束日期不是今天代表尚未結束，還在時間區塊中
      if (dayEd != CurrWeekDay_) {
         return ets_Opening;
      }

      //------------------------------
      // 結束日期是今天，看看時間到了沒
      if (CurrTime_ < TbData_.EdTime_) {
         return ets_Opening;
      }
      
      return ets_Closed;
   }

   //==============================================
   // 開始日期不是今天，那就看看結束日是不是今天
   if (dayEd == CurrWeekDay_) {
      //------------------------------
      // 先取得開始日期是何時
      // 因為交易日是利用 wDaySt 來看，所以要推回 wDatSt 的日期
      if (dayEd < daySt) {
         // 表示繞過一圈的，把它加回去
         dayEd += 7;
      }

      ResuDate_.IncDay(daySt - dayEd);

      //------------------------------
      // [結束時間]尚未到達
      if (CurrTime_ < TbData_.EdTime_)
         return ets_Opening;

      //------------------------------
      // [結束時間]已經到達
      if (TbData_.ExecTime_.IsInvalid() == true || CurrTime_ < TbData_.ExecTime_) {
         // [執行時間]尚未到達
         return ets_Closed;
      }

      //------------------------------
      // [結束時間]已經到達, [執行時間]已經到達
      ResuDate_.IncDay(1);
      return ets_WaitOpen;
   }

   //==============================================
   //開始日期與結束日期都沒有吻合的，那就看看今天有沒有在範圍內
   if (daySt <= dayEd) {
      if (daySt < CurrWeekDay_ && CurrWeekDay_ < dayEd) {
         //---------------------------
         // 這個設定可能是好幾天, 剛好今天落在這個期間
         ResuDate_.IncDay(CurrWeekDay_ - daySt);
         return ets_Opening;
      }
      else {
         return ets_Unknown;
      }
   }
   else {
      // wDaySt = 5
      // wDayEd = 2
      // wDay_  = 6
      if (daySt < CurrWeekDay_ && dayEd < CurrWeekDay_) {
         ResuDate_.IncDay(daySt - CurrWeekDay_);
         return ets_Opening;
      }
      // wDaySt = 5
      // wDayEd = 2
      // wDay_  = 1
      else if (CurrWeekDay_ < daySt && CurrWeekDay_ < dayEd) {
         ResuDate_.IncDay(daySt - (CurrWeekDay_ + 7));
         return ets_Opening;
      }
   }

   return ets_Unknown;
}


/////////////////////////////////////////////////////////////////////////////
// class TTimerCheck impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTimerCheck::TTimerCheck (bool isEnable)
	: IsEnable_    (isEnable)
   , NextClock_   (0)
   , Interval_    (0)
   , TimeoutCount_(0)
{
}
//---------------------------------------------------------------------------

K_ctor TTimerCheck::TTimerCheck (int ms)
{
	SetTimeout(ms, true);
}
//---------------------------------------------------------------------------

K_mf(void) TTimerCheck::SetTimeout (int ms, bool isClrCnt)
{
   IsEnable_  = true;
   NextClock_ = TTimerBase::GetClockMS() + static_cast<uint64>(ms);
   Interval_  = ms;

   if (isClrCnt)
      TimeoutCount_ = 0;
}
//---------------------------------------------------------------------------

K_mf(bool) TTimerCheck::IsTimeoutC () const
{
	if (IsEnable_ == false)
		return false;

   uint64 nowClock = TTimerBase::GetClockMS();
   if (NextClock_ <= nowClock)
      return true;
      
   return false;
}
//---------------------------------------------------------------------------

K_mf(bool) TTimerCheck::IsTimeout ()
{
   bool ret = IsTimeoutC();

   if (ret)
      TimeoutCount_++;

   return ret;
}
//---------------------------------------------------------------------------

K_mf(void) TTimerCheck::SetEnable (bool isEnable, bool isClrCnt)
{
	IsEnable_ = isEnable;
	if (isClrCnt) {
	   TimeoutCount_ = 0;
   }
}
//---------------------------------------------------------------------------
