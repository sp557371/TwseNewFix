/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseExcCode.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// class TTwseExcCode impl
/////////////////////////////////////////////////////////////////////////////
static const TErrKind  Ek_XTwse  ("XX");
static const TErrKind  Ek_XCtrl  ("XC");
static const TErrKind  Ek_XTester("XT");
//---------------------------------------------------------------------------
static const TErrNum   En_Null                     ("");
//---------------------------------------------------------------------------
static const TErrNum   En_AppiaMwLinkReady         ("2000");
static const TErrNum   En_AppiaMwLinkFail          ("2010");
static const TErrNum   En_AppiaMwLinkFails         ("2011");
static const TErrNum   En_AppiaMwLinkBroken        ("2012");
static const TErrNum   En_AppiaMwAlreadyLogon      ("2013");
static const TErrNum   En_AppiaMwSessionIDNotMatch ("2014");
static const TErrNum   En_AppiaMwFixMsgSeqNumLess  ("2015");
static const TErrNum   En_AppiaMwWaitTmpSesReady   ("2016");
static const TErrNum   En_AppiaMwWaitTmpSesReadyAll("2017");
static const TErrNum   En_AppiaMwOutboundMsgReject ("2018");
static const TErrNum   En_AppiaMwOrderCountOver    ("2019");
//------------------------------------
static const TErrNum   En_InMsgSeqNumEqual        ("2051");
static const TErrNum   En_InMsgSeqNumLess         ("2052");
static const TErrNum   En_InMsgSeqNumAbove        ("2053");
static const TErrNum   En_InMsgSeqNumRcvOK        ("2054");
static const TErrNum   En_OutMsgSeqNumEqual       ("2061");
static const TErrNum   En_OutMsgSeqNumLess        ("2062");
static const TErrNum   En_OutMsgSeqNumAbove       ("2063");
static const TErrNum   En_OutMsgSeqNumRcvOK       ("2064");
//---------------------------------------------------------------------------
static const TErrNum   En_FixSessionConnected     ("2101");
static const TErrNum   En_FixSessionDisconnected  ("2102");
static const TErrNum   En_FixSessionEndOfDay      ("2103");
static const TErrNum   En_FixSessionHold          ("2104");
static const TErrNum   En_FixSessionSuspend       ("2105");
static const TErrNum   En_FixSessionResume        ("2106");
static const TErrNum   En_FixSessionServiceUp     ("2107");
static const TErrNum   En_FixSessionServiceDown   ("2108");
static const TErrNum   En_FixSessionStartEndOfDay ("2109");
//---------------------------------------------------------------------------
static const TErrNum   En_FixSesTestOK            ("2200");
static const TErrNum   En_FixSesTestFail          ("2201");
static const TErrNum   En_FixSesTestPOK           ("2202");
//---------------------------------------------------------------------------
static const TErrNum   En_TandemLinkReady         ("3000");
static const TErrNum   En_TandemRecvCmd           ("3001");
static const TErrNum   En_TandemLinkBroken        ("3010");
//---------------------------------------------------------------------------
const std::string  TTwseExcCode::LinkState   ("1.連線狀態");
const std::string  TTwseExcCode::InMsgSeqNum ("2.輸入訊息序號");
const std::string  TTwseExcCode::OutMsgSeqNum("3.輸出訊息序號");
const std::string  TTwseExcCode::ObMsgRej    ("4.輸出訊息錯誤");
const std::string  TTwseExcCode::OrdCnt      ("5.委託筆數");
//---------------------------------------------------------------------------

K_mf(bool) TTwseExcCode::GetErrNum (ETwseExcCode code, TErrNum& errNum)
{
   static const TErrNum  ErrNumList[] = {
      En_Null,
      //===========================================
      // Middleware
      En_AppiaMwLinkReady,
      En_AppiaMwLinkFail,
      En_AppiaMwLinkFails,
      En_AppiaMwLinkBroken,
      En_AppiaMwAlreadyLogon,
      En_AppiaMwSessionIDNotMatch,
      En_AppiaMwFixMsgSeqNumLess,
      En_AppiaMwWaitTmpSesReady,
      En_AppiaMwWaitTmpSesReadyAll,
      En_AppiaMwOutboundMsgReject,
      En_AppiaMwOrderCountOver,
      //
      En_InMsgSeqNumEqual,
      En_InMsgSeqNumLess,
      En_InMsgSeqNumAbove,
      En_InMsgSeqNumRcvOK,
      En_OutMsgSeqNumEqual,
      En_OutMsgSeqNumLess,
      En_OutMsgSeqNumAbove,
      En_OutMsgSeqNumRcvOK,
      //===========================================
      // Fix Session
      En_FixSessionConnected,
      En_FixSessionDisconnected,
      En_FixSessionEndOfDay,
      En_FixSessionHold,
      En_FixSessionSuspend,
      En_FixSessionResume,
      En_FixSessionServiceUp,
      En_FixSessionServiceDown,
      En_FixSessionStartEndOfDay,
      //===========================================
      // for XTester
      En_FixSesTestOK,
      En_FixSesTestFail,
      En_FixSesTestPOK,
      //===========================================
      // for XCtrl
      En_TandemLinkReady,
      En_TandemRecvCmd,
      En_TandemLinkBroken,
   };

   if (code > tec_None && code < tec_Total) {
      errNum = ErrNumList[code];
      return true;
   }
   
   return false;
}
//---------------------------------------------------------------------------

K_mf(TErrCode) TTwseExcCode::GetXTwseCode (ETwseExcCode code)
{
   TErrCode errCode(Ek_XTwse);
   
   GetErrNum(code, errCode.ErrNum_);
   return errCode;
}
//---------------------------------------------------------------------------

K_mf(TErrCode) TTwseExcCode::GetXCtrlCode (ETwseExcCode code)
{
   TErrCode errCode(Ek_XCtrl);
   
   GetErrNum(code, errCode.ErrNum_);
   return errCode;
}
//---------------------------------------------------------------------------

K_mf(TErrCode) TTwseExcCode::GetXTestCode (ETwseExcCode code)
{
   TErrCode errCode(Ek_XTester);
   
   GetErrNum(code, errCode.ErrNum_);
   return errCode;
}
//---------------------------------------------------------------------------
