//---------------------------------------------------------------------------
#ifndef bxDRSesH
#define bxDRSesH
//---------------------------------------------------------------------------
#include "bxLinkSes.h"
#include "bxDR.h"
#include "bxDRWorker.h"
#include "bxSesMgr.h"
//---------------------------------------------------------------------------
namespace Kway  {
namespace Tw    {
namespace Bx    {
namespace DR    {
/////////////////////////////////////////////////////////////////////////////
K_class TbxDRSes : public TbxLinkSes
{
   typedef TbxLinkSes       inherited;
   typedef TbxDRSes        this_type;
   typedef std::set<long>           TSeqSet;
   typedef TSeqSet::iterator        TSeqSetI;
   TbxFTSesMgr& SesMgr_;
   TWork_R5000  R5000_;
   TWork_R5010  R5010_;
   TWork_R5020  R5020_;
   size_t       RptTimeOut_;
   TSeqSet      SeqSet_;
//   TSeqSet      ReSeqSet_;      // 已要求回補, 等候回補結果的SET
   TFile        Pmach_;
   long         LastSeqNo_;     // 最後的成交回報序號
   bool         Recovering_;    // 補漏進行中
   bool         Finish_;        // 己經收到最後一筆訊息了(總筆數訊息), 不用再定時嚐試做最後一筆補漏要求了
   bool         IsNewLine_;     // 設定 PMach 格式是否換行
   std::string  RecoverFName_;
   Kway::TTimer<this_type>  RecoverTimer_;

public:
   enum TRptState
   {
     rpt_Starting=5000, // 開啟RPT作業
     rpt_RptReply,      // RPT已回覆
     rpt_Data,          // 收到資料
     rpt_ReData,        // 補漏
     rpt_LinkChk,       // RPT連線確認訊息
     rpt_Logout,        // RPT作業結束
     rpt_RptEnd,        // RPT回報已結束
   };
protected:
        K_mf(void)  StartupFile     ();
        K_mf(bool)  SettingApply    (const std::string&);
        K_mf(void)  AddWorkers      ();
        K_mf(void)  StartRptSys     ();
        K_mf(void)  OnSubSysReady   (TApKey);
        K_mf(void)  OnSubSysBroken  (TSubSysName,TStCode);
        K_mf(void)  OnMesLinkReady  (TMesBase*);
        K_mf(void)  OnMesLinkBroken (TMesBase*, const std::string&);//清除接收緩衝區
        K_mf(void)  OnMesLinkFail   (TMesBase*, const std::string&);//SetState(sp_Error);
virtual K_mf(void)  DoTimeOut       (TTimerBase*);
        K_mf(void)  UpdateState     ();

public:
        K_ctor  TbxDRSes  (TbxFTSesMgr*, KStk::TMarket);
        K_dtor ~TbxDRSes  ();
        K_mf(TWorkKey) GetWorkKey   (TFunCode);
        K_mf(void)  SetState        (TRptState, const std::string&);
 inline K_mf(void)  StartRptTimer   () { StopRptTimer(); StartTimer(RptTimeOut_);   }
 inline K_mf(void)  StopRptTimer    () { StopTimer();               }
        K_mf(bool)  SendPkt         (const TControlHead&);
        K_mf(bool)  WriteDR         (const TDRBody&);
virtual K_mf(const std::string) SesName() { return std::string("成交回報"); }
        K_mf(void)  DRFinish        ();
};
/////////////////////////////////////////////////////////////////////////////
}; // DR
}; // Bx
}; // Tw
}; // Kway
//---------------------------------------------------------------------------
#endif


