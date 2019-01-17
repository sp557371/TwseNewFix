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
      tec_AppiaMwLinkReady,            /* 2000  appia MW �s�u���\                */
      tec_AppiaMwLinkFail,             /* 2010  appia MW �s�u����                */
      tec_AppiaMwLinkFails,            /* 2011  appia MW �s��⦸�s�u����        */
      tec_AppiaMwLinkBroken,           /* 2012  appia MW �_�u                    */
      tec_AppiaMwAlreadyLogon,         /* 2013  appia MW ���Ƶn�J                */
      tec_AppiaMwSessionIDNotMatch,    /* 2014  TMP SessionID �]�w�PAppia MW���� */
      tec_AppiaMwFixMsgSeqNumLess,     /* 2015  TMP FixMsgSeqNum �j�� MW ��      */
      tec_AppiaMwWaitTmpSesReady,      /* 2016  ����TMPSes�n�J�@�~������         */
      tec_AppiaMwWaitTmpSesReadyAll,   /* 2017  ����TMPSes�n�J�@�~����������     */
      tec_AppiaMwOutboundMsgReject,    /* 2018  Appia MW Outbound Message Reject */
      tec_AppiaMwOrderCountOver,       /* 2019  �e�U���ƨ�F�W��                 */
      //
      tec_InMsgSeqNumEqual,         /* 2051  ��J�T���Ǹ��ۦP                 */
      tec_InMsgSeqNumLess,          /* 2052  ��J�T���Ǹ� TMP ���p            */
      tec_InMsgSeqNumAbove,         /* 2053  ��J�T���Ǹ� TMP ���j            */
      tec_InMsgSeqNumRcvOK,         /* 2054  ��J�T���Ǹ��^�ɵ���             */
      tec_OutMsgSeqNumEqual,        /* 2061  ��X�T���Ǹ��ۦP                 */
      tec_OutMsgSeqNumLess,         /* 2062  ��X�T���Ǹ� TMP ���p            */
      tec_OutMsgSeqNumAbove,        /* 2063  ��X�T���Ǹ� TMP ���j            */
      tec_OutMsgSeqNumRcvOK,        /* 2064  ��X�T���Ǹ��^�ɵ���             */

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
      tec_FixSesTestOK,             /* 2200  FIX connection ���ճq�L, ���������]�w */
      tec_FixSesTestFail,           /* 2201  FIX connection ���ե���               */
      tec_FixSesTestPOK,            /* 2202  FIX connection ����s�u,��@�s�u���ճq�L*/

      //===========================================
      // for XCtrl
      tec_TandemLinkReady,          /* 3000  �����Ӧ� Tandem �ݪ� Command ��e�s�u */
      tec_TandemRecvCmd,            /* 3001  �����Ӧ� Tandem �ݪ� Command          */
      tec_TandemLinkBroken,         /* 3010  Tandem �ݪ� Command ��e�s�u�_�u      */
      
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
   static const std::string  LinkState;      // 1.�s�u���A
   static const std::string  InMsgSeqNum;    // 2.��J�T���Ǹ�
   static const std::string  OutMsgSeqNum;   // 3.��X�T���Ǹ�
   static const std::string  ObMsgRej;       // 4.��X�T�����~
   static const std::string  OrdCnt;         // 5.�e�U����

public:
   static K_mf(TErrCode) GetXTwseCode (ETwseExcCode);
   static K_mf(TErrCode) GetXCtrlCode (ETwseExcCode);
   static K_mf(TErrCode) GetXTestCode (ETwseExcCode);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseExcCode_h_)
