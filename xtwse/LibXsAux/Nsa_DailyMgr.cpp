/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_DailyMgr.h"
#include "Nsa_DailyMgrAux.h"
#include "Nsa_SysInfo.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;
//---------------------------------------------------------------------------
const TPoliName  TDailyMgr::DefPolicy("0000");

/////////////////////////////////////////////////////////////////////////////
// class TDailyEvHnd impl
/////////////////////////////////////////////////////////////////////////////
/*
K_ctor TDailyEvHnd::TDailyEvHnd ()
{
   const TAeID aeIDs[] = {
      aei_DailyOpen,
      aei_DailyClose,
      aei_DailyBfClear,
      aei_DailyClear,
      aei_DailyAfClear,
   };

   inherited::RegEv(aeIDs, numofele(aeIDs));
}
//---------------------------------------------------------------------------

K_mf(TAeResu) TDailyEvHnd::OnAppEv (SAppEvPktHdr* evInfo)
{
   switch (evInfo->EvID_) {
      case aei_DailyOpen:     OnDailyOpen   ();  break;
      case aei_DailyClose:    OnDailyClose  ();  break;
      case aei_DailyBfClear:  OnDailyBfClear();  break;
      case aei_DailyClear:    OnDailyClear  ();  break;
      case aei_DailyAfClear:  OnDailyAfClear();  break;
   }

   return aer_Continue;
}
*/


/////////////////////////////////////////////////////////////////////////////
// class TDailyMgr impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TDailyMgr::TDailyMgr ()
   : Setting_(NULL)
   , Status_ (NULL)
   , Timer_  (this)
{
   Setting_ = new TSetting(this);
   Status_  = new TStatus;
   Holiday_ = new THoliday(this);

   VAMgr_.Add(std::string("1.����骬�A"), Status_ ->GetVAC());
   VAMgr_.Add(std::string("2.�����F��"), Setting_->GetRsPoliNameVAC());
   VAMgr_.Add(std::string("3.�����]�w"), Setting_->GetRsConfKeyVAC());
   VAMgr_.Add(std::string("4.����]�w"),   Holiday_->GetVAC());

   SettingApply();

   if (TSysInfo::GetTradeDate   ().IsInvalid())  TSysInfo::SetTradeDate   (TKDate::Now());
   if (TSysInfo::GetTradeBegTime().IsInvalid())  TSysInfo::SetTradeBegTime(TKTime::Now());
}
//---------------------------------------------------------------------------

K_dtor TDailyMgr::~TDailyMgr ()
{
   Timer_.Stop();

   VAMgr_.Del(Status_ ->GetVAC());
   VAMgr_.Del(Setting_->GetRsPoliNameVAC());
   VAMgr_.Del(Setting_->GetRsConfKeyVAC());
   VAMgr_.Del(Holiday_->GetVAC());

   delete Holiday_;
   delete Status_;
   delete Setting_;
}
//---------------------------------------------------------------------------

K_mf(void) TDailyMgr::OnTimer (Kway::TTimerBase*)
{
   SettingApply();
}
//---------------------------------------------------------------------------

class TDailyMgr::TCheckTradeInfo
{
   //==============================================
   // Type
   typedef TSetting::TRsConfKey  TRsConfKey;

   //==============================================
   // struct TLatestDateTime
   struct TLatestDateTime : TTimeBlock::TLatestDate
   {
      typedef TTimeBlock::TLatestDate  inherited;

      TKTime  Time_;

      K_mf(void) SetDateTime (const TKDate& date, const TKTime& time)
      {
         if (inherited::SetDate(date))
            Time_ = time;
      }
   };

   //==============================================
   // member data
   TRsConfKey*      RsConfKey_;
   SDmrStatus&      DmrStatus_;
   TLatestDateTime  WaitOpenDate_;
   TLatestDateTime  ClosedDate_;

public:
   inline K_ctor TCheckTradeInfo (TRsConfKey* r, SDmrStatus& s)
      : RsConfKey_(r)
      , DmrStatus_(s)
   {}

   K_mf(void) Execute ();
};
//---------------------------------------------------------------------------

K_mf(void) TDailyMgr::TCheckTradeInfo::Execute ()
{
   TKDateTime     dateTime = TKDateTime::Now();
   ETradingStatus ts;

   for (TSetting::TRsConfKeyI iter=RsConfKey_->begin(); iter!=RsConfKey_->end(); ++iter) {
      TTimeBlock tb(iter->second);

      ts = tb.GetCurrDate(dateTime);
      switch (ts) {
      case ets_Opening:
         {
            DmrStatus_.TradeDate_    = tb.GetResuDate();
            DmrStatus_.TradeBegTime_ = iter->second.StTime_;
            DmrStatus_.TradingStatus_.assignT(ets_Opening);
            return;
         }

      case ets_WaitOpen:  WaitOpenDate_.SetDateTime(tb.GetWaitOpenDate(), iter->second.StTime_);  break;
      case ets_Closed:    ClosedDate_  .SetDateTime(tb.GetClosedDate  (), iter->second.StTime_);  break;
      }
   }

   //==============================================
   // ���䤣�쥿�b��������, ���N�M��䥦�����A
   if (WaitOpenDate_.Date_.IsInvalid() == false) {
      DmrStatus_.TradeDate_    = WaitOpenDate_.Date_;
      DmrStatus_.TradeBegTime_ = WaitOpenDate_.Time_;
      DmrStatus_.TradingStatus_.assignT(ets_WaitOpen);
   }

   if (ClosedDate_.Date_.IsInvalid() == false) {
      if (DmrStatus_.TradeDate_.IsInvalid() || DmrStatus_.TradeDate_ < ClosedDate_.Date_) {
         DmrStatus_.TradeDate_    = ClosedDate_.Date_;
         DmrStatus_.TradeBegTime_ = ClosedDate_.Time_;
         DmrStatus_.TradingStatus_.assignT(ets_Closed);
      }
   }
}
//---------------------------------------------------------------------------

K_mf(void) TDailyMgr::SettingApply ()
{
   Timer_.Stop();

   TSetting::TRsConfKey*  rsConfKey  = NULL;
   TSetting::TRsPoliName* rsPoliName = Setting_->GetRsPoliName();

   for (TSetting::TRsPoliNameI iter=rsPoliName->begin(); iter!=rsPoliName->end(); ++iter) {
      TKDate     tradeDate;
      SDmrStatus status;

      rsConfKey = iter->second.GetContainer(rsConfKey);
      if (rsConfKey->empty()) {
         //========================================
         // �S���ɶ��]�w, ���N��@�O�C�Ѫ� 00:00:00 ~ 23:59:59
         tradeDate = TKDate::Now();
         status.TradeBegTime_ .assign ("00:00:00");
         status.TradingStatus_.assignT(ets_Opening);
      }
      else {
         //========================================
         // ���ɶ��]�w, ���N�@�Ӥ@���ˬd�o
         TCheckTradeInfo cti(rsConfKey, status);

         cti.Execute();
         tradeDate = status.TradeDate_;

         // ���S���ŦX�� TradeDate, �ȮɥΤ��Ѧn�F
         if (tradeDate.IsInvalid())
            tradeDate = TKDate::Now();
      }

      //===========================================
      // �ˬd������޿�
      if (iter->second.TradingRule_ == ets_T1Date) {
         tradeDate.IncDay(1);
      }

      //===========================================
      // �ˬd�O�_������
      status.TradeDate_ = GetValidDate(iter->first, tradeDate);
      if (status.TradeDate_ != tradeDate)
         status.TradingStatus_.assignT(ets_Closed);

      //===========================================
      // ���� Status, �p�G��Ƥ��@�˪���, ���ͨƥ�
      UpdateStatus(iter->first, status);
   }

   Timer_.Start(10000, &this_type::OnTimer);
}
//---------------------------------------------------------------------------

K_mf(TKDate) TDailyMgr::GetValidDate (const TPoliName& poliName, const TKDate& refDate)
{
   byte   week;
   TKDate date(refDate);

   while (true) {
      //===========================================
      // �@�� [�P����] [�P����] �鬰����, �ҥH�o�����L�o
      week = date.GetWDay();
      if (week == 0 || week == 6) {
         if (Holiday_->IsWorkingDay(poliName, date) == true)
            break;
      }
      else {
         if (Holiday_->IsHoliday(poliName, date) == false)
            break;
      }

      date.IncDay(-1);
   }

   return date;
}
//---------------------------------------------------------------------------

K_mf(void) TDailyMgr::UpdateStatus (const TPoliName& poliName, const SDmrStatus& status)
{
   TStatus::TRecs* rsStatus  = Status_->GetRecs();
   TStatus::TRecsI iter      = rsStatus->fetch(poliName);
   SDmrStatus      oldStatus = iter->second;

   if (oldStatus == status)
      return;

   TStatus::TRecsU upd = rsStatus->begin_update(iter);

   *upd = status;

   if (oldStatus.TradeDate_.IsInvalid() == false) {
      if (oldStatus.TradeDate_ != status.TradeDate_) {
         // �o�ʴ���@�~�ƥ�
      }
      else if (oldStatus.TradeDate_ != status.TradeDate_) {
         // �o�ʥ�����A�ƥ�
      }
   }

   if (poliName == DefPolicy) {
      TSysInfo::SetTradeDate   (status.TradeDate_);
      TSysInfo::SetTradeBegTime(status.TradeBegTime_);
   }
}
//---------------------------------------------------------------------------


/*
K_mf(void) TDailyMgr::CheckTradeDate (TSetting::TRsConfKey* rsConfKey, SDmrStatus& status)
{
   Kway::TKTime BegTime;
   //Kway::TKDate TradeDate = TMdsSts::GetTradeDate();
   Kway::TKDate TradeDate;
   Kway::TKDate ResDate   = GetSettingTradeDate(BegTime);
   if (ResDate.IsInvalid()) {
      ResDate = Kway::TKDate::Now();
   }

   if (TradeDate != ResDate) {
      if (TradeDate.IsInvalid() == false) {
         //TMdsSts::SetTradeDate   (ResDate);
         //TMdsSts::SetTradeBegTime(BegTime);
         // �o�ʥ���鲧�ʪ��ƥ�
         //TMdsEvMgr::GetInst()->Fire_DailyClear();
      }
      else {
         //TMdsSts::SetTradeDate   (ResDate);
         //TMdsSts::SetTradeBegTime(BegTime);
      }
   }

   Status_->UpdateTradeDate(ResDate);
}
//---------------------------------------------------------------------------
*/

