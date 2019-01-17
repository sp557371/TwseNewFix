/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseFixField.h"
#include "Nsa_UtilComm.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// class TwseUtcTimeStamp impl
/////////////////////////////////////////////////////////////////////////////
TwseUtcTimeStamp::TwseUtcTimeStamp (int field)
   : inherited(field)
{
   TKDateTimeMS dt = TKDateTimeMS::Now();
      
   Assign(dt);
}
//---------------------------------------------------------------------------

TwseUtcTimeStamp::TwseUtcTimeStamp (int field, const TKTime& time)
   : inherited(field)
{
   TKDateTimeMS dt = TKDateTimeMS::Now();

   if (time.IsInvalid() == false)
      dt.SetTime(time);
   
   dt.SetMS(TKTimeMS());

   Assign(dt);
}
//---------------------------------------------------------------------------

TwseUtcTimeStamp::TwseUtcTimeStamp (int field, const TKTime& time, const TKTimeMS& timems)
   : inherited(field)
{
   TKDateTimeMS dt = TKDateTimeMS::Now();

   if (time.IsInvalid() == false)
      dt.SetTime(time);

   dt.SetMS(timems);

   Assign(dt);
}
//---------------------------------------------------------------------------

void TwseUtcTimeStamp::Assign (const TKDateTimeMS& dt)
{
   TKDateTimeMS newDt(dt);
   char         buf[50];

   // 轉成 UTC 時間
   newDt.IncHour(-8);

   // YYYYMMDD-HH:MM:SS.sss
   sprintf(buf, "%s-%s.%s", newDt.GetDate().as_string(TKDate::sf_ELong, 0).c_str(),
                            newDt.GetTime().as_string(true)               .c_str(),
                            newDt.GetMS  ().as_string(3)                  .c_str());

   inherited::setValue(std::string(buf));
}


/////////////////////////////////////////////////////////////////////////////
// class TFixTwseTargetSubID (56) impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TFixTwseTargetSubID::assign_TrxSegment (const TTrxSegment& trxSeg)
{
   switch (trxSeg) {
      case Tw::Stk::ts_Full:  inherited::assign(TwseTargetSubID_Regular);     break;
      case Tw::Stk::ts_Odd:   inherited::assign(TwseTargetSubID_OddLots);     break;
      case Tw::Stk::ts_Fix:   inherited::assign(TwseTargetSubID_FixedPrice);  break;
      default:                inherited::clear ();                            break;
   }
}
//---------------------------------------------------------------------------

K_mf(TFixTwseTargetSubID::TTrxSegment) TFixTwseTargetSubID::as_TrxSegment () const
{
   // 因為 TrxSegment 預設是 ts_Full, 怕為了有問題, 所以給定 ts_Early
   // 可以區分是否有效的
   TTrxSegment trxSeg(Tw::Stk::ts_Early);
   
   if (!inherited::empty()) {
      char ch = inherited::operator [] (0);

      switch (ch) {
      case TwseTargetSubID_Regular:     trxSeg.assignT(Tw::Stk::ts_Full);  break;
      case TwseTargetSubID_OddLots:     trxSeg.assignT(Tw::Stk::ts_Odd);   break;
      case TwseTargetSubID_FixedPrice:  trxSeg.assignT(Tw::Stk::ts_Fix);   break;
      }
   }
   return trxSeg;
}


/////////////////////////////////////////////////////////////////////////////
// class TFixTwseSide impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TFixTwseSide::assign_BSCode (const TBSCode& bsCode)
{
   switch (bsCode) {
      case Tw::Stk::bsc_Buy:   inherited::assignT(FIX::Side_BUY);   break;
      case Tw::Stk::bsc_Sell:  inherited::assignT(FIX::Side_SELL);  break;
      default:                 inherited::clear  ();                break;
   }
}
//---------------------------------------------------------------------------

K_mf(TFixTwseSide::TBSCode) TFixTwseSide::as_BSCode () const
{
   TBSCode bsCode;
   
   switch (inherited::Data_) {
      case FIX::Side_BUY:   bsCode.assignT(Tw::Stk::bsc_Buy);   break;
      case FIX::Side_SELL:  bsCode.assignT(Tw::Stk::bsc_Sell);  break;
   }
   return bsCode;
}


/////////////////////////////////////////////////////////////////////////////
// class TFixTwseExecType impl
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TFixTwseExecType::as_desc () const
{
   if (inherited::Data_ == ExecType_PartialNew)
      return std::string("P.PartialNew");
   
   return inherited::as_desc();
}


/////////////////////////////////////////////////////////////////////////////
// class TFixTwseIvacnoFlag impl
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TFixTwseIvacnoFlag::as_desc () const
{
   if (inherited::is_null())
      return std::string("Null");

   return inherited::as_string();
}


/////////////////////////////////////////////////////////////////////////////
// class TFixTwseOrdType impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TFixTwseOrdType::assign_OType (const TOType& oType)
{
   switch (oType) {
      case Tw::Stk::ot_Gn:    inherited::assignT(TwseOrdType_Normal);          break;
      case Tw::Stk::ot_CrA:   inherited::assignT(TwseOrdType_FinaPurcOnMarg);  break;
      case Tw::Stk::ot_DbA:   inherited::assignT(TwseOrdType_FinaShortSell);   break;
      case Tw::Stk::ot_CrS:   inherited::assignT(TwseOrdType_FirmPurcOnMarg);  break;
      case Tw::Stk::ot_DbS:   inherited::assignT(TwseOrdType_FirmShortSell);   break;
      case Tw::Stk::ot_Tse5:  inherited::assignT(TwseOrdType_SblShortSell5);   break;
      case Tw::Stk::ot_Tse6:  inherited::assignT(TwseOrdType_SblShortSell6);   break;
      default:                inherited::clear  ();                            break;
   }
}
//---------------------------------------------------------------------------

K_mf(TFixTwseOrdType::TOType) TFixTwseOrdType::as_OType () const
{
   TOType oType;

   switch (Data_) {
      case TwseOrdType_Normal:          oType.assignT(Tw::Stk::ot_Gn);    break;
      case TwseOrdType_FinaPurcOnMarg:  oType.assignT(Tw::Stk::ot_CrA);   break;
      case TwseOrdType_FinaShortSell:   oType.assignT(Tw::Stk::ot_DbA);   break;
      case TwseOrdType_FirmPurcOnMarg:  oType.assignT(Tw::Stk::ot_CrS);   break;
      case TwseOrdType_FirmShortSell:   oType.assignT(Tw::Stk::ot_DbS);   break;
      case TwseOrdType_SblShortSell5:   oType.assignT(Tw::Stk::ot_Tse5);  break;
      case TwseOrdType_SblShortSell6:   oType.assignT(Tw::Stk::ot_Tse6);  break;
   }
   return oType;
}


/////////////////////////////////////////////////////////////////////////////
// class TFixTwseExCode impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TFixTwseExCode::assign_SfGroup (const TSfGroup& sfGroup, const TTrxSegment& trxSeg)
{
   if (!sfGroup.empty()) {
      inherited::assignT(sfGroup[0]);
   }
   else {
      // 指定預設值
      if (trxSeg == Tw::Stk::ts_Odd) {
         inherited::assignT(TwseExCode_OddLots);
      }
      else {
         inherited::assignT(TwseExCode_RegularFixedPrice);
      }
   }
}
//---------------------------------------------------------------------------
