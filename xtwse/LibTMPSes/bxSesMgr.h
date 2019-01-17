//---------------------------------------------------------------------------
#ifndef bxSesMgrH
#define bxSesMgrH
//---------------------------------------------------------------------------
#include "TwStkOrdFlow.h"

#include "SesMgrR.h"
#include "bxDef.h"
#include "bxRecorder.h"
#include "TwStkOrdCenter.h"
#include "MesX25.h"
#include "SchedulerBase.h"
#include "SenderBase.h"
#include "MsgCenter.h" 
//#include "MsgTable.h" 
//#include "MsgTableVA.h" 
#include "stdSettingP.h"
#include "ExcMgrBase.h"
#include <vector>
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace bx {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
  #pragma pack(push,1)
#else
  #pragma pack(1)
#endif
/////////////////////////////////////////////////////////////////////////////
K_class TmktState
{
   K_class Tls{
   public:
      byte  ls_Normal   : 1,  // 整股時段及盤後時段的旗標
            ls_Odd      : 1,  // 零股時段的旗標
            ls_Fix      : 1,  // 盤後時段的旗標
            ls_Auct     : 1,
            ls_Lend     : 1,
            ls_Tend     : 1;
      byte  ls_NormalWait : 1,
            ls_OddWait  : 1,
            ls_FixWait  : 1,
            ls_AuctWait : 1,
            ls_LendWait : 1,
            ls_TendWait : 1;
      inline K_ctor Tls  () : ls_Normal(0), ls_Odd(0), ls_Fix(0), ls_Auct(0), ls_Lend(0), ls_Tend(0), 
                              ls_AuctWait(0), ls_LendWait(0), ls_TendWait(0){}
      inline K_mf(void)   Set(ETradeKind tk, bool busy)
             {
             	switch(tk){
                   case tk_Normal: ls_Normal = busy; break;
                   case tk_Odd   : ls_Odd    = busy; break;
                   case tk_Fix   : ls_Fix    = busy; break;
                   case tk_Auct  : ls_Auct   = busy; break;
                   case tk_Lend  : ls_Lend   = busy; break;
                   case tk_Tend  : ls_Tend   = busy; break;
                }
             }
      inline K_mf(bool)   Test(ETradeKind tk)
             {
             	switch(tk){
                   case tk_Normal: return (ls_Normal);
                   case tk_Odd   : return (ls_Odd);
                   case tk_Fix   : return (ls_Fix);
                   case tk_Auct  : return (ls_Auct);  
                   case tk_Lend  : return (ls_Lend);
                   case tk_Tend  : return (ls_Tend);
                   default: return false;
                }
             }
      inline K_mf(void)   SetWait(ETradeKind tk, bool busy)
             {
             	switch(tk){
                   case tk_Normal: ls_NormalWait = busy; break;
                   case tk_Odd   : ls_OddWait    = busy; break;
                   case tk_Fix   : ls_FixWait    = busy; break;
                   case tk_Auct  : ls_AuctWait   = busy; break;
                   case tk_Lend  : ls_LendWait   = busy; break;
                   case tk_Tend  : ls_TendWait   = busy; break;
                }
             }
      inline K_mf(bool)   TestWait(ETradeKind tk)
             {
             	switch(tk){
                   case tk_Normal: return (ls_NormalWait);  
                   case tk_Odd   : return (ls_OddWait);
                   case tk_Fix   : return (ls_FixWait);
                   case tk_Auct  : return (ls_AuctWait);  
                   case tk_Lend  : return (ls_LendWait);
                   case tk_Tend  : return (ls_TendWait);
                   default: return false;
                }
             }
   };
   Tls   Tse_;
   Tls   Otc_;
   Tls   OtcR_;
public:
   inline K_ctor  TmktState  ()  { };

   inline K_mf(void)    Set       (TMarket mkt, ETradeKind tk, bool busy)
          {
             switch (mkt) {
                using namespace Kway::Tw::Stk;
                case m_Tse:  Tse_.Set(tk, busy);   break;
                case m_Otc:  Otc_.Set(tk, busy);   break;
                case m_OtcR: OtcR_.Set(tk, busy);  break;
             }
          }
   inline K_mf(bool)    Test      (TMarket mkt, ETradeKind tk)
          {
             switch (mkt) {
                using namespace Kway::Tw::Stk;
                case m_Tse:  return(Tse_.Test(tk));
                case m_Otc:  return(Otc_.Test(tk));
                case m_OtcR: return(OtcR_.Test(tk));
                default: return false;
             }
          }
   inline K_mf(void)    SetWait    (TMarket mkt, ETradeKind tk, bool busy)
          {
             switch (mkt) {
                using namespace Kway::Tw::Stk;
                case m_Tse:  Tse_.SetWait(tk, busy);   break;
                case m_Otc:  Otc_.SetWait(tk, busy);   break;
                case m_OtcR: OtcR_.SetWait(tk, busy);  break;
             }
          }
   inline K_mf(bool)    TestWait  (TMarket mkt, ETradeKind tk)
          {
             switch (mkt) {
                using namespace Kway::Tw::Stk;
                case m_Tse:  return(Tse_.TestWait(tk));
                case m_Otc:  return(Otc_.TestWait(tk));
                case m_OtcR: return(OtcR_.TestWait(tk));
                default: return false;
             }
          }
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
  #pragma pack(pop)
#else
  #pragma pack()
#endif

K_class TbxSes;
typedef std::vector<TbxSes*>      TbxSesRegs;
struct TTrSesRegs{
   TbxSesRegs    Normal_;
   TbxSesRegs    Odd_;
   TbxSesRegs    Fix_;
   TbxSesRegs    Auct_;
   TbxSesRegs    Lend_;
   TbxSesRegs    Tend_;
   K_mf(Kway::word) GetReadyLines(){
   	return Normal_.size() + Odd_.size()  +
   	       Fix_.size()    + Auct_.size() +
   	       Lend_.size()   + Tend_.size();
   }
};
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
K_class TbxSesMgr:public TSesMgrR,
                  public TSenderBase,
                  public TCmdMgrObj
{
   typedef TSesMgrR                  inherited;
   typedef TbxSesMgr                 this_type;
   typedef TSenderBase               inherited_Sender;
   typedef TbxSesRegs::iterator      TSesRegi;
   typedef Kway::Tw::Stk::TOrdCenter TOrdCenter;
   typedef Kway::TTimer<this_type>   TbxTimer;
   struct TBrkMkt{
      TBrokerID    BrkID_;	
      TMarket      Market_;
      TBrkMkt() {};
      TBrkMkt(const TBrokerID& a, const TMarket& b)
         : BrkID_(a), Market_(b)
         {}
      K_mf(bool) operator < (const TBrkMkt& a) const
         { return BrkID_==a.BrkID_ ? a.Market_<Market_ : a.BrkID_<BrkID_; }
      K_mf(std::string) as_string()const {return std::string(BrkID_.as_string()+"-"+Market_.as_string());}
      K_mf(void)        assign(const char*){};
   };
/*
   struct TSeqRec{
   	char              Reserve_[128];
   	Kway::dword       LastSeq_;
   };
*/
   typedef Kstl::ev_map<TBrkMkt, Kway::dword>     TBrkMktSeq;
   typedef TStdRecorder<TBrkMktSeq, Kway::dword>  TBrkSeqRec;
   typedef Kstl::ev_map<int, int>         TTrxErrCnt;
   typedef TStdRecorder<TTrxErrCnt, int>  TErrCntRec;
   struct SOrdKeyTime{
      typedef SOrdKeyTime this_type;
      TTseOrdKey     Key_;
      TKTime         Time_;
      K_ctor SOrdKeyTime(const TTseOrdKey K, const TKTime T){
         Key_  = K;
         Time_ = T;
      };
      inline K_mf(bool)      operator < (const this_type a) const {
      	                                 return (Time_ == a.Time_?(Key_ < a.Key_):(Time_ < a.Time_));}
      inline K_mf(this_type) operator = (const this_type a)       {Key_  = a.Key_; 
                                                                   Time_ = a.Time_;
                                                                   return *this;}
   };
   typedef std::map<SOrdKeyTime, TReqPtr> TReqs;
   typedef TReqs::iterator                TReqsi;

   struct SLineConf{
      bool                    Active_;
      TbxTimer*               TimerT_;
      TbxTimer*               TimerO_;
      Kway::word              Wait_;
      
      inline K_ctor SLineConf(this_type* a)
         : TimerT_(new TbxTimer(a)),TimerO_(new TbxTimer(a)){}
      inline K_dtor ~SLineConf(){delete TimerT_; delete TimerO_;}
   };
   typedef Kstl::ev_map<int, int>      TMsgCnt;
   typedef TStdRecorder<TMsgCnt, int>  TMsgCntRec;
   struct TbxData{
      bool       Active_;
      Kway::word Passwd_;
      Kway::word SrcPort_;
      TbxSes*    Ses_;
   };
   typedef std::map<TPvcID, TbxData> TbxDatas;

   struct TBrkMktSesID{
      TBrkID       BrkID_;	
      TMarket      Market_;
      TPvcID       PvcID_;
      TBrkMktSesID() {};
      TBrkMktSesID(const TBrkID& a, const TMarket& b, const TPvcID& c)
         : BrkID_(a), Market_(b), PvcID_(c)
         {}
      K_mf(bool) operator < (const TBrkMktSesID& a) const 
      { return ( BrkID_    == a.BrkID_
               ? (Market_  == a.Market_
                  ? PvcID_  < a.PvcID_
                  : Market_ < a.Market_)
               : BrkID_  < a.BrkID_ ); 
      }
   };
   struct TLineChgRec{
   	ETradeKind     TradeKind_;
      TKTime         BeginTime_;
      bool           BeingUsed_;
   };
   typedef char_ary<char, 7,' '> TTermID;
   typedef Kstl::ev_map<TBrkMktSesID, TTermID>  TSesTermID;
   TSesTermID              SesTermID_;

   Kway::TViewAreaMgr      bxVA_;
   TSettingPage*           SettingP_;
   SLineConf*              NormalConf_;
   SLineConf*              OddConf_;
   SLineConf*              FixConf_;
   SLineConf*              AuctConf_;
   SLineConf*              LendConf_;
   SLineConf*              TendConf_;
   TSchedulerBase*         Scheduler_;
   TOrdCenter*             OrdCenter_;
   TbxSesRegs              OrdSesRegs_;
   TTrSesRegs              TseSes_;
   TTrSesRegs              OtcSes_;
   TTrSesRegs              OtcRSes_;
   TbxRecorder*            Recorder_;
   TmktState               Busy_;
//   TMsgTable*              MsgTable_;
//   TMsgVAC*                MsgVA_;
   bool                    AutoResend_;
   bool                    OVConfig_;      // on viewer configuration 940810
   bool                    BSOK96_;
   Kway::word              BSOKFmt_;
   bool                    UseT070_;         // 20090407 委託斷線時自動查詢方式是否使用T070..目前只有上市整股有支援
   bool                    PMachLN_;
   bool                    MarketFlag_[3][tk_None];
   bool                    ExceedErrCnt_[3][tk_None];
   bool                    AutoRequest_;   // 新單斷線後自動查詢
   bool                    AutoJudge_;
   bool                    ResendOnMatch_;
   bool                    ResendOnFail_;
   bool                    IsTester_;
   Kway::word              OrdConfirmTime_;
   Kway::word              DelayTime_;
   TbxTimer*               DelayTimer_;
   TReqs*                  Reqs_;
   Kway::word              FailCount_;
   //↓依據市場別取得可用之OrderSes
   TMsgCnt                 MsgCnt_;
   TMsgCntRec*             MsgCntRec_;
   int                     MaxTimeGap_;
   int                     LineChgInterval_;
   int                     SendOrderInterval_;
   TLineChgRec             LineChgRec_;
   bool                    EnableAutoSwitch_; // 20100726 是否允許依委託自動切換盤別
//   TbxDatas                bxDatas_;

   
   
   K_mf(TbxSes*)           GetOrderSes    (TMarket mkt, ETradeKind tk);
   inline K_mf(ETradeKind) GetSesTK       (TbxSes* SesPtr);
   inline K_mf(void)       RegSes         (TbxSes* SesPtr, TTrSesRegs& SesRegs, TMarket mkt, ETradeKind tk, bool isAdd);
   inline K_mf(bool)       RegSesDetail   (TbxSes* SesPtr, TbxSesRegs& SesRegs, bool isAdd);
   inline K_mf(TbxSes*)    GetSes         (TTrSesRegs& SesRegs, ETradeKind tk);
   inline K_mf(TbxSes*)    GetSesDetail   (TbxSesRegs& SesRegs);
   inline K_mf(Kway::word) GetSesNum      (TMarket mkt, ETradeKind tk);
   inline K_mf(Kway::word) GetSesNumDetail(TTrSesRegs& SesRegs, ETradeKind tk);
   K_mf(void)              SwitchLine     (TMarket mkt, ETradeKind tk);
   K_mf(void)              RecoverLine    (TMarket mkt, ETradeKind tk);
   K_mf(void)              ResetTimer     (TMarket mkt, ETradeKind tk, Kway::word sec = 0, bool isstop = false);
   K_mf(void)              OnTimer        (Kway::TTimerBase*);
   K_mf(void)              OnDelayTimer   (Kway::TTimerBase*);
   K_mf(void)              LoadMainConf   ();
   K_mf(void)              LoadTesterConf ();
#ifdef __bxFT
   TSesRegs        FTSesRegs_;
   TSesRegs        RptSesRegs_;
#endif
   TBrkMktSeq              BrkMktSeq_;
   TBrkSeqRec*             BrkSeqRec_;
   TTrxErrCnt              TrxErrCnt_;
   TErrCntRec*             ErrCntRec_;
   TSegDefs                SegDefs_;
   Kway::word              LineErrCnt_;
//   TBrkOrdSeqMap           BOSMap_; // 20080523
public:
   TTrxSegment        MarketPh_;
   K_ctor TbxSesMgr   (TSchedulerBase*, TOrdCenter*, bool istester = false);
   K_dtor ~TbxSesMgr  ();
   K_mf(std::string)      GetMsg       (std::string a, std::string b, bool& errcnt)
                                         {return GetErrMsg(TMsgKey(a),TMsgKey(b), errcnt);}
   K_mf(std::string)      GetMsg       (std::string a, std::string b)
                                         {return GetErrMsg(TMsgKey(a),TMsgKey(b));}
   K_mf(void)             SetMarketPh  (TTrxSegment);
   K_mf(void)             SetMarketPh  (TSegDefs*);
   K_mf(TTrxSegment)      GetMarketPh  (){return MarketPh_;}
   //↓當OrderSes 建立後,註冊可管理之OrderSes
   K_mf(void)             RegbxSes     (TbxSes* SesPtr);
   K_mf(void)             DelbxSes     (TbxSes* SesPtr);
   //↓當OrderSes 成功建立且進入交易子系統後,註冊為可用之OrderSes
   K_mf(void)             RegOrderSes  (TbxSes* SesPtr);
   //↓OrderSes 離線或發生Error時請務必呼叫此函式刪除註冊
   K_mf(void)             DelOrderSes  (TbxSes* SesPtr);
   //↓當委託處理完畢,呼叫OnSesFree 告知Mgr並取得下筆委託
   K_mf(void)             OnSesFree    (TSesBase*, TMarket mkt);
   K_mf(void)             SetScheduler (TSchedulerBase* sb){Scheduler_ = sb;}
   K_mf(void)             ResetScheduler ();   
#ifdef __bxFT
   K_mf(void)             RegFToutSes  (TbxSes* SesPtr);
   K_mf(void)             DelFToutSes  (TbxSes* SesPtr);
   K_mf(void)             RegRptSes    (TbxSes* SesPtr);
   K_mf(void)             DelRptSes    (TbxSes* SesPtr);
   K_mf(void)             ReqRptRecover(TBrkID, TSize, TSize);
   K_mf(void)             DoRptRecover (TBrkID, TMarket, TRptBody, bool);
#endif
   //↓新委託通知
   K_mf(bool)             NewOrderAlert(TMarket mkt, TReqPtr& ReqPtr, ETradeKind tk);
   inline K_mf(TbxRecorder*) GetRecorder()  {return Recorder_;}
   inline K_mf(TOrdCenter*)  GetOrdCenter() {return OrdCenter_;}
   inline K_mf(bool)         IsAutoResend() {return AutoResend_;}
   K_mf(void)             SetMarketClose (TbxSes* SesPtr);
   K_mf(bool)             IsPMachLN (){return PMachLN_;}
   K_mf(bool)             IsAutoStart    (ETradeKind tk);
   K_mf(bool)             IsAutoRequest  (){return AutoRequest_;}
   K_mf(bool)             IsUseT070      (){return UseT070_;}
   K_mf(bool)             IsAutoJudge    (){return AutoJudge_;}
   K_mf(bool)             IsResendOnMatch(){return ResendOnMatch_;}
   K_mf(bool)             IsResendOnFail (){return ResendOnFail_;}
   K_mf(bool)             DoAutoRequest  (TReqPtr& ReqPtr, TMarket mkt, ETradeKind tk)
                                          {Scheduler_->DoAutoReq(ReqPtr, mkt, tk);}
   K_mf(void)             DoRequestFail  (TReqPtr& ReqPtr);
   K_mf(void)             SetBrkSeq      (TBrokerID, TMarket, Kway::dword);
   K_mf(Kway::dword)      GetBrkSeq      (TBrokerID, TMarket);
   K_mf(void)             DelSetting     (TMarket,TBrkID,TPvcID);
   K_mf(Kway::word)       GetFailCount   (){return FailCount_;}
   K_mf(bool)             IsTester       (){return IsTester_;}
   K_mf(bool)             IncreaseErrCnt (int subsys);
   K_mf(bool)             ResetErrCnt    (int subsys, Kway::word count = 0);
   K_mf(bool)             SetMsgCnt      (int cnt);
   K_mf(int)              GetMsgCnt      ();
   K_mf(Kway::byte)       GetDebugLevel  ();
   K_mf(bool)             DoCommand      (Kway::word cmdid, std::string msg = "");
   K_mf(void)             ReloadConf     ();
   K_mf(std::string)      GetTermID      (TBrkID, TMarket, TPvcID);
   K_mf(Kway::word)       GetConfirmTime (){return OrdConfirmTime_;}
};
//---------------------------------------------------------------------------
/*
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
  #pragma pack(pop)
#else
  #pragma pack()
#endif
*/
//---------------------------------------------------------------------------
}
}
}
#endif
