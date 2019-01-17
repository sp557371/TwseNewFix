//---------------------------------------------------------------------------
#ifndef bxSesMgrH
#define bxSesMgrH
//---------------------------------------------------------------------------
#include "SesMgr.h"
#include "bxHeadDef.h"
#include "bxLink.h"
#include "FTCenter.h"
#include "NotifyMgr.h"
#include "PvcTable.h"
#include "MsgCenter.h"
#include "FTRMDef.h"
#include "stdSettingP.h"
#include "bxDR.h"
#include <vector>
#include "FTExtDef.h"
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Tw   {
namespace Bx   {
/////////////////////////////////////////////////////////////////////////////
K_class TbxSesBase;
namespace Rpt {
K_class TbxRptSes;
};
//---------------------------------------------------------------------------
typedef FT::TBrkMkt                     TRptKey;
/////////////////////////////////////////////////////////////////////////////
K_class TbxFTSesMgr : public TFTSenderBase, 
                      public FT::TFTCenterHandler
{
//    typedef TSesMgr                         inherited;
    typedef TFTSenderBase                   inherited;
    typedef FT::TFTCenterHandler            inherited_Handler;
    typedef TbxFTSesMgr                     this_type;
    typedef FT::TFTCenter                   TFTCenter;
    typedef FT::TSenderMgr                  TSenderMgr;
    typedef FT::TFileCode                   TFileCode;
    typedef FT::TFTJob                      TFTJob;
    typedef FT::TSysID                      TSysID;
    typedef Kway::Tw::Stk::TMarket          TMarket;
    typedef std::set<int>                   TRecIDSet;
    typedef FT::TBrkMkt                     TRptKey;
    typedef Tw::Bx::TPvcID                  TPvcID;
    struct SPMachSes{
       TSesBase*   Ses_;
       bool        IsLn_;
    };
    typedef std::map<TRptKey, TbxSesBase*>  TRptSesMap;
    typedef TRptSesMap::iterator            TRptSesMapI;
    typedef std::map<TRptKey, SPMachSes>    TPMachSesMap;
    typedef TPMachSesMap::iterator          TPMachSesMapI;
    typedef std::vector<TbxSesBase*>        TFTVec;
    typedef TFTVec::iterator                TFTVecI;
    struct TFTVecs{
       TFTVec      FTVec_;
       Kway::byte  Send_;
       Kway::byte  Recv_;
       K_ctor TFTVecs(){Send_ = 0; Recv_ = 0;}
    };
    
//    typedef std::map<TBrkID, TFTVec>        TFTMap;
    typedef std::map<FT::TBrkMkt, TFTVecs>  TFTMap;
    typedef TFTMap::iterator                TFTMapI;
    struct SRpyJob{
       TSysID     SysID_;
       TKTime     Time_;
    };
//    typedef std::map<TFileCode, TSysID>     TFTRpyMap;
    typedef std::map<TFileCode, SRpyJob>    TFTRpyMap;
    typedef TFTRpyMap::iterator             TFTRpyMapI;
    typedef std::map<TRptKey, TFTRpyMap>    TFTRpyQue;
    typedef TFTRpyQue::iterator             TFTRpyQueI;

    typedef std::map<TSysID, TRptKey>       TRptMap;
    typedef TRptMap::iterator               TRptMapI;
    typedef std::map<int, FT::TFTNewJob>    TJobMap;
    typedef std::map<DR::TBrkMPvc, DR::TFixSesID> TFixSesMap;
    typedef Kstl::ev_map<DR::TFixSesID, TFile*>   TDRFiles;
    struct TDROrdData{
       char_ary<char, 7,' '>  IvacNo_;
       char_ary<char, 6,' '>  StkNo_;
       char_ary<char, 1,' '>  BSCode_;  
       DR::TFixSesID          FixSes_;    
    };
    typedef std::map<Kway::Tw::Stk::TTseOrdKey, TDROrdData> TDROrds;
    FT::TFTRBase*   FTrm_;
    int             SeqNo_;
    TJobMap         JobMap_;
    TFTMap*         FTSes_;
    TRptSesMap*     RptSes_;
    TPMachSesMap*   PMachSes_;
    TFTRpyQue       FTRpyQue_;
    TRptMap         RptMap_;    // 儲存要回補的SysID 與 RptSES 的對應
    TViewAreaMgr    VAMgr_;
    TFile           Log_;
    TFile           AlarmLog_;
    TRecIDSet       RecIDSet_;
    TFile           PVCLog_;
    TBrkPvcVAC*     PvcTable_;
    TSettingPage*   Setting_;
    bool            IsCompleteLog_;
    TDRFiles        DRFiles_;
    TFixSesMap      FixSesMap_;
    Kway::word      FailCount_;
    TDROrds         DROrds_;
    TFTVec          DRSess_;
    Kway::TTimer<this_type>      Timer_;
    K_mf(void)                   OnTimer(TTimerBase*);
    K_mf(void)                   OnRegTimer(TTimerBase*);

   K_mf(void) CreateVAC (void); //建立遠端VAC
   K_mf(void) DestoryVAC(void);
   K_mf(bool) DoSourceFileCopy(const TFTJob&, const std::string&);
   K_mf(void) WriteS10  (const char*, const TFTJob&);
public:
   K_ctor TbxFTSesMgr   ();
   K_dtor ~TbxFTSesMgr  ();
   //↓當FTSes 成功建立且進入交易子系統後,註冊為可用之FTSes
   K_mf(void) RegFTSes(const TBrkID&, TbxSesBase*);
   //↓FTSes 離線或發生Error時請務必呼叫此函式刪除註冊
   K_mf(void) DelFTSes(const TBrkID&, TbxSesBase*);
   //↓當RptSes 成功建立且進入交易子系統後,註冊為可用之FTSes
   K_mf(void) RegRptSes(TbxSesBase*);
   //↓RptSes 離線或發生Error時請務必呼叫此函式刪除註冊
   K_mf(void) DelRptSes(TbxSesBase*);
   K_mf(void) RegPMachSes(TSesBase*,bool t = true);
   K_mf(void) DelPMachSes(TSesBase*,bool t = true);
   virtual K_mf(void) DoSend  (const TBrkID&, const TMarket&, const TSysID&);

   K_mf(bool)  AddRpyQue(const TRptKey&, const TFileCode&, const TSysID&);
   K_mf(void)  RmRpyQue (const TRptKey&, const TFileCode&, bool anyway = false);
   K_mf(TSysID)GetRpyID (const TRptKey&, const TFileCode&);
   K_mf(bool)  FetchNextJob(const TBrkID&, const TMarket&, TFileCode code = TFileCode());
// 不用R40的方式來補
   K_mf(void)  RequestRpt(const TBrkID&, KStk::TMarket, long, long);
   K_mf(void)  RptRecover(const TSysID&, std::string&, const TBrkID&, KStk::TMarket);
   K_mf(bool)  WriteLog (const char*, size_t len, TPvcID pvcno, bool input=true, TBrkID brk = TBrkID());
   K_mf(void)  ProcessS10(const TFTJob&);
   K_mf(bool)  WriteAlarm(const void*);
   K_mf(int)   WritePVCStatus(int, const void*);
   K_mf(const TViewAreaMgr*) GetViewAreaMgr() const { return &VAMgr_; }
   K_mf(void)  B36Echo(std::string&, const TBrkID&, KStk::TMarket);
   K_mf(void)  B37Echo(std::string&, const TBrkID&, KStk::TMarket);
   K_mf(void)  SendB37(std::string&, const TBrkID&, KStk::TMarket, Kway::word);
   K_mf(std::string) GetMsg  (std::string a, std::string b)
                             {return GetErrMsg(TMsgKey(a),TMsgKey(b));}
   K_mf(void)  ChkRpyQue(const TRptKey&);
   K_mf(void)  DoFileCmd(TSysID, FT::TFTNewJob job, std::string&);
   K_mf(void)  FileCmdEcho(int, bool);
   K_mf(void)  DoSendState(FT::TBrkMkt);
   K_mf(bool)  SetFTrm(FT::TFTRBase* r);
   K_mf(bool)  IsCompleteLog(){return IsCompleteLog_;}
   K_mf(void)  WriteDRData(KStk::TMarket, const DR::TDRBody& adata);
   K_mf(Kway::word) GetFailCount(){return FailCount_;}
   //↓20111208 eric: 用來記錄DR 連線是否已回補完畢
   K_mf(void) RegDRSes(TbxSesBase*);
   K_mf(void) DelDRSes(TbxSesBase*);
   K_mf(void) FileUpload(FT::TFTNewJob job);
};
//---------------------------------------------------------------------------
K_fn(TbxFTSesMgr*) GetBxMgr();
/////////////////////////////////////////////////////////////////////////////
};  // Bx
};  // Tw
};  // Kway
//---------------------------------------------------------------------------
#endif

