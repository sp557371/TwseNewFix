/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseRes.h"
#include "TwseFixField.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// class TTwseRes impl
/////////////////////////////////////////////////////////////////////////////
// 2013/12/17 miguel change 10 -> 12
//const char*  TTwseRes::TseConn        = "10";
const char*  TTwseRes::TseConn        = "12";
// end
const char*  TTwseRes::TseFull        = "30";
const char*  TTwseRes::TseFixed       = "32";
const char*  TTwseRes::TseOdd         = "40";
//---------------------------------------------------------------------------
// 2013/12/17 miguel change 91 -> 12
//const char*  TTwseRes::OtcConn        = "91";
const char*  TTwseRes::OtcConn        = "12";
// end
const char*  TTwseRes::OtcFull        = "93";
const char*  TTwseRes::OtcFixed       = "98";
const char*  TTwseRes::OtcOdd         = "94";
//---------------------------------------------------------------------------
const char*  TTwseRes::AppendNoError       = "01";
// 2013/12/17 miguel change 04->02 70->03 71->08
//const char*  TTwseRes::KeyValueError       = "04";
const char*  TTwseRes::KeyValueError       = "02";
//const char*  TTwseRes::AppendNoEqual0      = "70";
const char*  TTwseRes::AppendNoEqual0      = "03";
//const char*  TTwseRes::RawDataLenNotEqual5 = "71";
const char*  TTwseRes::RawDataLenNotEqual5 = "08";
// end
const char*  TTwseRes::ApCodeErr           = "72";
const char*  TTwseRes::SenderCompIDError   = "73";
const char*  TTwseRes::MsgTypeError        = "78";
const char*  TTwseRes::TradeSuspended      = "86";
//---------------------------------------------------------------------------
const char*  TTwseRes::BrokerIDErr         = "12";
//-------------------------------------------------
/*
// SeqNoError
const char*  TTwseRes::SeqNoError          = "18";
const char*  TTwseRes::SeqNoErrorOdd       = "17";
*/
//-------------------------------------------------
const char*  TTwseRes::QuantityError       = "22";
//-------------------------------------------------
const char*  TTwseRes::QuantityWasCut      = "31";
const char*  TTwseRes::QuantityWasCutOdd   = "32";
//-------------------------------------------------
const char*  TTwseRes::ClOrdIDRepeat       = "70";
const char*  TTwseRes::IvacNoLenErr        = "71";
const char*  TTwseRes::SeqNoLenErr         = "72";
const char*  TTwseRes::StkNoLenErr         = "73";
const char*  TTwseRes::PriLenErr           = "74";
const char*  TTwseRes::QtyLenErr           = "75";
const char*  TTwseRes::ErrorOverLimit      = "77";
const char*  TTwseRes::WarrantLPErr        = "78";
const char*  TTwseRes::IvacNoFlagErr       = "79";
const char*  TTwseRes::TwseOrdTypeErr      = "80";
const char*  TTwseRes::OrigClOrdIDErr      = "81";
const char*  TTwseRes::ClOrdIDErr          = "82";
const char*  TTwseRes::OrdTypeErr          = "83";
const char*  TTwseRes::OrdContNotMatch     = "84";
const char*  TTwseRes::TwseOrdIdDup        = "85";
const char*  TTwseRes::FrontEndSystemError = "98";
const char*  TTwseRes::CallCenter          = "99";
//---------------------------------------------------------------------------
//const char*  TTwseRes::ApCodeErr      = "76";
//const char*  TTwseRes::MsgTypeError   = "78";
//---------------------------------------------------------------------------
const int    TTwseRes::IntQuantityWasCut    = atoi(TTwseRes::QuantityWasCut);    // 31.戈禦秈┪ㄩ芥〆癠计秖砆搭
const int    TTwseRes::IntQuantityWasCutOdd = atoi(TTwseRes::QuantityWasCutOdd); // 32.戈禦秈〆癠计秖砆搭
//---------------------------------------------------------------------------

K_mf(std::string) TTwseRes::GetErrCodeHdr (int rejCode, const TMarket& mkt, const TTrxSegment& trxSeg)
{
   std::string code;

   if (rejCode > tfrc_Conn_Start && rejCode < tfrc_Conn_End) {
      if (mkt == m_Tse)  code.assign(TseConn);
      else               code.assign(OtcConn);
   }
   else {
      switch (trxSeg) {
      case ts_Full:
         if (mkt == m_Tse)  code.assign(TseFull);
         else               code.assign(OtcFull);
         break;

      case ts_Fix:
         if (mkt == m_Tse)  code.assign(TseFixed);
         else               code.assign(OtcFixed);
         break;
      
      case ts_Odd:
         if (mkt == m_Tse)  code.assign(TseOdd);
         else               code.assign(OtcOdd);
         break; 
      }
   }
   
   return code;
}
//---------------------------------------------------------------------------

K_mf(std::string) TTwseRes::GetStsCodeByRejCode (int rejCode, const TMarket& mkt, const TTrxSegment& trxSeg)
{
   std::string code = GetErrCodeHdr(rejCode, mkt, trxSeg);
   if (code.empty())
      return code;

   //==============================================
   // 硈絬╰参
   if (rejCode > tfrc_Conn_Start && rejCode < tfrc_Conn_End) {
      switch (rejCode) {
         case tfrc_AppendNoError:        code.append(AppendNoError);        break;
         case tfrc_KeyValueError:        code.append(KeyValueError);        break;
         case tfrc_AppendNoEqual0:       code.append(AppendNoEqual0);       break;
         case tfrc_RawDataLenNotEqual5:  code.append(RawDataLenNotEqual5);  break;
         case tfrc_ApCodeErr:            code.append(ApCodeErr);            break;
         case tfrc_SenderCompIDError:    code.append(SenderCompIDError);    break;
         case tfrc_MsgTypeError:         code.append(MsgTypeError);         break;
         case tfrc_TradeSuspended:       code.append(TradeSuspended);       break;
      }
   }
   //==============================================
   // ユ╰参
   else if (rejCode > tfrc_Trade_Start && rejCode < tfrc_Trade_End) {
      switch (rejCode) {
         case tfrc_BrokerIDErr:          code.append(BrokerIDErr);     break;
         /*
         case tfrc_SeqNoError:
            {
               if (trxSeg != ts_Odd)  code.append(SeqNoError);
               else                   code.append(SeqNoErrorOdd);
            }
            break;
         */
         case tfrc_QuantityError:        code.append(QuantityError);   break;
         case tfrc_QuantityWasCut:
            {
               if (trxSeg == ts_Odd)  code.append(QuantityWasCutOdd);
               else                   code.append(QuantityWasCut);
            }
            break;
         case tfrc_IvacNoLenErr:         code.append(IvacNoLenErr);         break;
         case tfrc_SeqNoLenErr:          code.append(SeqNoLenErr);          break;
         case tfrc_StkNoLenErr:          code.append(StkNoLenErr);          break;
         case tfrc_PriLenErr:            code.append(PriLenErr);            break;
         case tfrc_QtyLenErr:            code.append(QtyLenErr);            break;
         case tfrc_ErrorOverLimit:       code.append(ErrorOverLimit);       break;
         case tfrc_WarrantLPErr:         code.append(WarrantLPErr);         break;
         case tfrc_IvacNoFlagErr:        code.append(IvacNoFlagErr);        break;
         case tfrc_TwseOrdTypeErr:       code.append(TwseOrdTypeErr);       break;
         case tfrc_OrigClOrdIDErr:       code.append(OrigClOrdIDErr);       break;
         case tfrc_ClOrdIDErr:           code.append(ClOrdIDErr);           break;
         case tfrc_OrdTypeErr:           code.append(OrdTypeErr);           break;
         case tfrc_TwseOrdIdDup:         code.append(TwseOrdIdDup);         break;
         case tfrc_FrontEndSystemError:  code.append(FrontEndSystemError);  break;
         case tfrc_CallCenter:           code.append(CallCenter);           break;
      }
   }
   //==============================================
   // FIX.Stk 箇砞﹚竡
   else {
      switch (rejCode) {
         case FIX::Stk::wrc_ClOrdIDTheSame:   code.append(ClOrdIDRepeat);    break;
         case FIX::Stk::wrc_OrdContNotMatch:  code.append(OrdContNotMatch);  break;
      }
   }

   return code;
}
//---------------------------------------------------------------------------

K_mf(std::string) TTwseRes::GetStsCodeByRejCode (int rejCode, const TMarket& mkt, const TFixTargetSubID& fixTargetSubID)
{
   TFixTwseTargetSubID twseTargetSubID(fixTargetSubID);
   TTrxSegment         trxSeg = twseTargetSubID.as_TrxSegment();

   return GetStsCodeByRejCode(rejCode, mkt, trxSeg);
}
//---------------------------------------------------------------------------

K_mf(std::string) TTwseRes::GetStsCodeByTwseCode (int twseCode, const TMarket& mkt, const TTrxSegment& trxSeg)
{
   // 硂娩倒﹚ twseCode 琌ㄢ絏 code, 礚猭耞琌琌[硈絬╰参] or [ユ╰参]
   // ┮箇砞ㄏノ [ユ╰参]
   std::string code = GetErrCodeHdr(tfrc_None, mkt, trxSeg);
   if (code.empty())
      return code;

   char buf[20];
   sprintf(buf, "%02d", twseCode);
   code.append(buf);

   return code;
}
//---------------------------------------------------------------------------

K_mf(std::string) TTwseRes::GetStsCodeByTwseCode (int twseCode, const TMarket& mkt, const TFixTargetSubID& fixTargetSubID)
{
   TFixTwseTargetSubID twseTargetSubID(fixTargetSubID);
   TTrxSegment         trxSeg = twseTargetSubID.as_TrxSegment();

   return GetStsCodeByTwseCode(twseCode, mkt, trxSeg);
}
//---------------------------------------------------------------------------
