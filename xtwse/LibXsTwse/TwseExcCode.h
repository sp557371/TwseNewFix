/////////////////////////////////////////////////////////////////////////////
#if !defined(_TwseExcCode_h_)
#define _TwseExcCode_h_
//---------------------------------------------------------------------------
#include "ExcMgrBase.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {

/////////////////////////////////////////////////////////////////////////////
// enum ETwseExcCode
/////////////////////////////////////////////////////////////////////////////
enum ETwseExcCode
{
   tec_None,
      //===========================================
      // Middleware
      tec_AppiaMwLinkReady,            /* 2000  appia MW 連線成功                */
      tec_AppiaMwLinkFail,             /* 2010  appia MW 連線失敗                */
      tec_AppiaMwLinkFails,            /* 2011  appia MW 連續兩次連線失敗        */
      tec_AppiaMwLinkBroken,           /* 2012  appia MW 斷線                    */
      tec_AppiaMwAlreadyLogon,         /* 2013  appia MW 重複登入                */
      tec_AppiaMwSessionIDNotMatch,    /* 2014  TMP SessionID 設定與Appia MW不符 */
      tec_AppiaMwFixMsgSeqNumLess,     /* 2015  TMP FixMsgSeqNum 大於 MW 端      */
      tec_AppiaMwWaitTmpSesReady,      /* 2016  等待TMPSes登入作業完成中         */
      tec_AppiaMwWaitTmpSesReadyAll,   /* 2017  等待TMPSes登入作業全部完成中     */
      tec_AppiaMwOutboundMsgReject,    /* 2018  Appia MW Outbound Message Reject */
      tec_AppiaMwOrderCountOver,       /* 2019  委託筆數到達上限                 */
      //
      tec_InMsgSeqNumEqual,         /* 2051  輸入訊息序號相同                 */
      tec_InMsgSeqNumLess,          /* 2052  輸入訊息序號 TMP 較小            */
      tec_InMsgSeqNumAbove,         /* 2053  輸入訊息序號 TMP 較大            */
      tec_InMsgSeqNumRcvOK,         /* 2054  輸入訊息序號回補結束             */
      tec_OutMsgSeqNumEqual,        /* 2061  輸出訊息序號相同                 */
      tec_OutMsgSeqNumLess,         /* 2062  輸出訊息序號 TMP 較小            */
      tec_OutMsgSeqNumAbove,        /* 2063  輸出訊息序號 TMP 較大            */
      tec_OutMsgSeqNumRcvOK,        /* 2064  輸出訊息序號回補結束             */

      //===========================================
      // Fix Session
      tec_FixSessionConnected,      /* 2101  Fix Session Connected            */
      tec_FixSessionDisconnected,   /* 2102  Fix Session Disconnected         */
      tec_FixSessionEndOfDay,       /* 2103  Fix Session End-Of-Day           */
      tec_FixSessionHold,           /* 2104  Fix Session Hold                 */
      tec_FixSessionSuspend,        /* 2105  Fix Session Suspend              */
      tec_FixSessionResume,         /* 2106  Fix Session Resume               */
      tec_FixSessionServiceUp,      /* 2107  Fix Session Service-Up           */
      tec_FixSessionServiceDown,    /* 2108  Fix Session Service-Down         */
      tec_FixSessionStartEndOfDay,  /* 2109  Fix Session Start End-Of-Day     */

      //===========================================
      // for XTester
      tec_FixSesTestOK,             /* 2200  FIX connection 測試通過, 移除相關設定 */
      tec_FixSesTestFail,           /* 2201  FIX connection 測試失敗               */
      tec_FixSesTestPOK,            /* 2202  FIX connection 兩條連線,單一連線測試通過*/

      //===========================================
      // for XCtrl
      tec_TandemLinkReady,          /* 3000  接受來自 Tandem 端的 Command 轉送連線 */
      tec_TandemRecvCmd,            /* 3001  接受來自 Tandem 端的 Command          */
      tec_TandemLinkBroken,         /* 3010  Tandem 端的 Command 轉送連線斷線      */
      
   tec_Total
};

/////////////////////////////////////////////////////////////////////////////
// class TTwseExcCode
/////////////////////////////////////////////////////////////////////////////
K_class TTwseExcCode
{
   static K_mf(bool) GetErrNum (ETwseExcCode, TErrNum&);

public:
   //==============================================
   // Middleware
   static const std::string  LinkState;      // 1.連線狀態
   static const std::string  InMsgSeqNum;    // 2.輸入訊息序號
   static const std::string  OutMsgSeqNum;   // 3.輸出訊息序號
   static const std::string  ObMsgRej;       // 4.輸出訊息錯誤
   static const std::string  OrdCnt;         // 5.委託筆數

public:
   static K_mf(TErrCode) GetXTwseCode (ETwseExcCode);
   static K_mf(TErrCode) GetXCtrlCode (ETwseExcCode);
   static K_mf(TErrCode) GetXTestCode (ETwseExcCode);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseExcCode_h_)
