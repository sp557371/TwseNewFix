/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "QFixField.h"
#include <stdio.h>
#include <stdlib.h>
#include "src/Code/DeprecatedFieldNumbers.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::FIX;
using namespace Kway::FIX::FIELD;

/////////////////////////////////////////////////////////////////////////////
// class TFieldNum impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TFieldNum::assign (const char* str)
{
   char  buf[128];
   char* ptr;

   strcpy(buf, str);
   ptr = strchr(buf, (int)'.');
   if (ptr) {
      *ptr = '\0';
   }

   Data_ = (Field)(atoi(buf));
}
//---------------------------------------------------------------------------

K_mf(std::string) TFieldNum::as_string () const
{
   if ((int)Data_ == 0) {
      return std::string("000.None");
   }
   
   switch (Data_) {
   //case :  return std::string("");
   case Account:                return std::string("001.Account");
   case AvgPx:                  return std::string("006.AvgPx");
   case BeginString:            return std::string("008.BeginString");
   case ClOrdID:                return std::string("011.ClOrdID");
   case CumQty:                 return std::string("014.CumQty");
   case ExecID:                 return std::string("017.ExecID");
   case HandlInst:              return std::string("021.HandlInst");
   case LastPx:                 return std::string("031.LastPx");
   case LastQty:                return std::string("032.LastQty");
   case MsgType:                return std::string("035.MsgType");
   case OrderID:                return std::string("037.OrderID");
   case OrderQty:               return std::string("038.OrderQty");
   case OrdStatus:              return std::string("039.OrdStatus");
   case OrdType:                return std::string("040.OrdType");
   case OrigClOrdID:            return std::string("041.OrigClOrdID");
   case PossDupFlag:            return std::string("043.PossDupFlag");
   case Price:                  return std::string("044.Price");
   case SenderSubID:            return std::string("050.SenderSubID");
   case Side:                   return std::string("054.Side");
   case Symbol:                 return std::string("055.Symbol");
   case TargetSubID:            return std::string("057.TargetSubID");
   case Text:                   return std::string("058.Text");
   case TimeInForce:            return std::string("059.TimeInForce");
   case TransactTime:           return std::string("060.TransactTime");
   case ListID:                 return std::string("066.ListID");
   //case :  return std::string("");
   //case :  return std::string("");
   //case :  return std::string("");
   //case :  return std::string("");
   case PositionEffect:           return std::string("077.PositionEffect"); // FIX42: OpenClose
   case RawDataLength:            return std::string("095.RawDataLength");
   case RawData:                  return std::string("096.RawData");
   case OnBehalfOfCompID:         return std::string("115.OnBehalfOfCompID");
   case DeliverToCompID:          return std::string("128.DeliverToCompID");
   case ExecType:                 return std::string("150.ExecType");
   case LeavesQty:                return std::string("151.LeavesQty");
   case SecurityType:             return std::string("167.SecurityType");
   case SecondaryOrderID:         return std::string("198.SecondaryOrderID");
   case MaturityMonthYear:        return std::string("200.MaturityMonthYear");
   case SecurityExchange:         return std::string("207.SecurityExchange");
   case SubscriptionRequestType:  return std::string("263.SubscriptionRequestType");
   case TradSesReqID:             return std::string("335.TradSesReqID");
   case TradingSessionID:         return std::string("336.TradingSessionID");
   case TradSesMethod:            return std::string("338.TradSesMethod");
   case TradSesMode:              return std::string("339.TradSesMode");
   case EncodedTextLen:           return std::string("354.EncodedTextLen");
   case EncodedText:              return std::string("355.EncodedText");
   case ExecRestatementReason:    return std::string("378.ExecRestatementReason");
   case BidID:                    return std::string("390.BidID");
   case CFICode:                  return std::string("461.CFICode");
   }
   
   DeprecatedField depField = (DeprecatedField)Data_;
   switch (depField) {
   case ExecTransType:  return std::string("020.ExecTransType");
   case ClientID:       return std::string("109.ClientID");
   }
   
   char caMsg[50];
   sprintf(caMsg, "%d.¡´UserDefine¡´", (int)Data_);

   return std::string(caMsg);
}
//---------------------------------------------------------------------------
