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
   case 0:  return std::string("�P����");
   case 1:  return std::string("�P���@");
   case 2:  return std::string("�P���G");
   case 3:  return std::string("�P���T");
   case 4:  return std::string("�P���|");
   case 5:  return std::string("�P����");
   case 6:  return std::string("�P����");
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
   case 0:  return std::string("�P����");
   case 1:  return std::string("�P���@");
   case 2:  return std::string("�P���G");
   case 3:  return std::string("�P���T");
   case 4:  return std::string("�P���|");
   case 5:  return std::string("�P����");
   case 6:  return std::string("�P����");
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
      // �h���̫�@�� ','
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
   // [�}�l���]�P���ѬۦP
   if (daySt == CurrWeekDay_) {
      //------------------------------
      // [�}�l�ɶ�]�|����F
      if (CurrTime_ < TbData_.StTime_) {
         // �W�L[����ɶ�]
         if (TbData_.ExecTime_.IsInvalid() == false && CurrTime_ > TbData_.ExecTime_) {
            return ets_WaitOpen;
         }
         // �ݰ_���٦b[�W�@�����]��[Closed]
         ResuDate_.IncDay(-1);
         return ets_Closed;
      }

      //------------------------------
      // ����������O���ѥN��|�������A�٦b�ɶ��϶���
      if (dayEd != CurrWeekDay_) {
         return ets_Opening;
      }

      //------------------------------
      // ��������O���ѡA�ݬݮɶ���F�S
      if (CurrTime_ < TbData_.EdTime_) {
         return ets_Opening;
      }
      
      return ets_Closed;
   }

   //==============================================
   // �}�l������O���ѡA���N�ݬݵ�����O���O����
   if (dayEd == CurrWeekDay_) {
      //------------------------------
      // �����o�}�l����O���
      // �]�������O�Q�� wDaySt �ӬݡA�ҥH�n���^ wDatSt �����
      if (dayEd < daySt) {
         // ���¶�L�@�骺�A�⥦�[�^�h
         dayEd += 7;
      }

      ResuDate_.IncDay(daySt - dayEd);

      //------------------------------
      // [�����ɶ�]�|����F
      if (CurrTime_ < TbData_.EdTime_)
         return ets_Opening;

      //------------------------------
      // [�����ɶ�]�w�g��F
      if (TbData_.ExecTime_.IsInvalid() == true || CurrTime_ < TbData_.ExecTime_) {
         // [����ɶ�]�|����F
         return ets_Closed;
      }

      //------------------------------
      // [�����ɶ�]�w�g��F, [����ɶ�]�w�g��F
      ResuDate_.IncDay(1);
      return ets_WaitOpen;
   }

   //==============================================
   //�}�l����P����������S���k�X���A���N�ݬݤ��Ѧ��S���b�d��
   if (daySt <= dayEd) {
      if (daySt < CurrWeekDay_ && CurrWeekDay_ < dayEd) {
         //---------------------------
         // �o�ӳ]�w�i��O�n�X��, ��n���Ѹ��b�o�Ӵ���
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
