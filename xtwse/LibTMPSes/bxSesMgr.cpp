#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxSesMgr.h"
#include "bxOrderSes.h"
#include "bxExtOrdSes.h"
//#include "bxRptSes.h"
#include "PMachSes.h"
#include "TwStkOrdFlow.h"
#include "MesTSEClient.h"
#include "KIniFile.h"
#include "DRSes.h"
#include "bxTestSes.h"
#include "ResetErrCnt.h"
#include "TseOrdSes.h"
#include "TseMatSes.h"
//---------------------------------------------------------------------------
#if !defined(__BORLANDC__)
#include "EvDevTseTcp.h"
#endif
//---------------------------------------------------------------------------
#include "SyncEv.h"
#include "SysMgrObjs.h"
#include "MesRegs.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw;
using namespace Kway::Tw::Stk;
using namespace Kway::Tw::bx;
/////////////////////////////////////////////////////////////////////////////
static char Msg_MarketClosed[]  = "0405";   //己收盤
static char Msg_OrderFail[]     = "0406";   //異常斷線,委託失敗
static Kway::byte DebugLog_;
//---------------------------------------------------------------------------
#if !defined(__BORLANDC__)
static TMesPtr  TEvDevTseTcp_Factory  ()  { return TMesPtr(new TMesTseTcpClient(DebugLog_==4));     }
static TMesReg    TEvDevTseTcp_Reg  = { "Ev-TSEClient", TEvDevTseTcp_Factory,  false, 0, 0 };
#endif
//---------------------------------------------------------------------------
static TMesPtr  TMesTSEClient_Factory  ()  { return TMesPtr(new TMesTSEClient());     }
static TMesReg    TMesTSEClient_Reg  = { "TSEClient", TMesTSEClient_Factory,  false, 0, 0 };
//---------------------------------------------------------------------------
   static const TMesReg*   MesRegs[] = { &TMesTSEClient_Reg,
                                         #if !defined(__BORLANDC__)
                                         &TEvDevTseTcp_Reg,
                                         #endif
   	                                   };
//---------------------------------------------------------------------------
   static TSesBase* TbxTseSes_Creater(const TSesReg&,TSesMgr& SesMgr)
   {
      TbxSesMgr *Mgr=static_cast<TbxSesMgr*>(&SesMgr);
//      TSesBase  *Rtn=static_cast<TSesBase*>(new TbxOrderSes(Mgr,Stk::m_Tse));
      TSesBase  *Rtn=static_cast<TSesBase*>(new TTseOrdSes(Mgr,Stk::m_Tse));
      return Rtn;
   }
//---------------------------------------------------------------------------
static TSesBase* TbxOtcSes_Creater(const TSesReg&,TSesMgr& SesMgr)
{
   TbxSesMgr *Mgr=static_cast<TbxSesMgr*>(&SesMgr);
//   TSesBase  *Rtn=static_cast<TSesBase*>(new TbxOrderSes(Mgr,Stk::m_Otc));
   TSesBase  *Rtn=static_cast<TSesBase*>(new TTseOrdSes(Mgr,Stk::m_Otc));
   return Rtn;
}
//---------------------------------------------------------------------------
static TSesBase*      TbxTseRptSes_Creater  (const TSesReg&,TSesMgr& SesMgr){
   TTseMatSes* SesPtr = new TTseMatSes((TbxSesMgr*)&SesMgr,Stk::m_Tse);
   return (TSesBase*)(SesPtr);
}
//---------------------------------------------------------------------------
   static TSesBase*      TbxOtcRptSes_Creater  (const TSesReg&,TSesMgr& SesMgr){
      TTseMatSes* SesPtr = new TTseMatSes((TbxSesMgr*)&SesMgr,Stk::m_Otc);
      return (TSesBase*)(SesPtr);
   }
//---------------------------------------------------------------------------
static TSesBase* TbxAuctSes_Creater(const TSesReg&,TSesMgr& SesMgr)
{
   TbxSesMgr *Mgr=static_cast<TbxSesMgr*>(&SesMgr);
   TSesBase  *Rtn=static_cast<TSesBase*>(new TbxAuctSes(Mgr,Stk::m_Tse));
   return Rtn;
}
//---------------------------------------------------------------------------
static TSesBase* TbxOAuctSes_Creater(const TSesReg&,TSesMgr& SesMgr)
{
   TbxSesMgr *Mgr=static_cast<TbxSesMgr*>(&SesMgr);
   TSesBase  *Rtn=static_cast<TSesBase*>(new TbxAuctSes(Mgr,Stk::m_Otc));
   return Rtn;
}
//---------------------------------------------------------------------------
static TSesBase* TbxLendSes_Creater(const TSesReg&,TSesMgr& SesMgr)
{
   TbxSesMgr *Mgr=static_cast<TbxSesMgr*>(&SesMgr);
   TSesBase  *Rtn=static_cast<TSesBase*>(new TbxLendSes(Mgr,Stk::m_Tse));
   return Rtn;
}
//---------------------------------------------------------------------------
static TSesBase* TbxOLendSes_Creater(const TSesReg&,TSesMgr& SesMgr)
{
   TbxSesMgr *Mgr=static_cast<TbxSesMgr*>(&SesMgr);
   TSesBase  *Rtn=static_cast<TSesBase*>(new TbxLendSes(Mgr,Stk::m_Otc));
   return Rtn;
}
//---------------------------------------------------------------------------
static TSesBase* TbxTendSes_Creater(const TSesReg&,TSesMgr& SesMgr)
{
   TbxSesMgr *Mgr=static_cast<TbxSesMgr*>(&SesMgr);
   TSesBase  *Rtn=static_cast<TSesBase*>(new TbxTendSes(Mgr,Stk::m_Tse));
   return Rtn;
}
//---------------------------------------------------------------------------
static TSesBase* TbxOTendSes_Creater(const TSesReg&,TSesMgr& SesMgr)
{
   TbxSesMgr *Mgr=static_cast<TbxSesMgr*>(&SesMgr);
   TSesBase  *Rtn=static_cast<TSesBase*>(new TbxTendSes(Mgr,Stk::m_Otc));
   return Rtn;
}
//---------------------------------------------------------------------------
static TSesBase*  PMachSesCreater(const TSesReg&,TSesMgr& SesMgr)
{
   TPMachSesLN *Ses=new TPMachSesLN(&SesMgr);
   return static_cast<TSesBase*>(Ses);
}
//---------------------------------------------------------------------------
static TSesBase*  TDrSesCreater(const TSesReg&,TSesMgr& SesMgr)
{
   TDRSesLN *Ses=new TDRSesLN(&SesMgr,Stk::m_Tse);
   return static_cast<TSesBase*>(Ses);
}
//---------------------------------------------------------------------------
static TSesBase*  ODrSesCreater(const TSesReg&,TSesMgr& SesMgr)
{
   TDRSesLN *Ses=new TDRSesLN(&SesMgr,Stk::m_Otc);
   return static_cast<TSesBase*>(Ses);
}
//---------------------------------------------------------------------------
static TSesBase* TbxTestSes_Creater(const TSesReg&,TSesMgr& SesMgr)
{
   TbxSesMgr *Mgr=static_cast<TbxSesMgr*>(&SesMgr);
   TSesBase  *Rtn=static_cast<TSesBase*>(new TbxTestSes(Mgr,Stk::m_Tse));
   return Rtn;
}
//---------------------------------------------------------------------------
static TSesBase* TbxOTestSes_Creater(const TSesReg&,TSesMgr& SesMgr)
{
   TbxSesMgr *Mgr=static_cast<TbxSesMgr*>(&SesMgr);
   TSesBase  *Rtn=static_cast<TSesBase*>(new TbxTestSes(Mgr,Stk::m_Otc));
   return Rtn;
}
//---------------------------------------------------------------------------
static TSesBase* TResetErrCntSes_Creater(const TSesReg&,TSesMgr& SesMgr)
{
   TbxSesMgr *Mgr=static_cast<TbxSesMgr*>(&SesMgr);
   TSesBase  *Rtn=static_cast<TSesBase*>(new TResetCntSes(Mgr));
   return Rtn;
}
//---------------------------------------------------------------------------
static const TSesReg  SesRegs[] =
{
   {"上市普通委託",TbxTseSes_Creater},
   {"上櫃普通委託",TbxOtcSes_Creater},
   {"上市成交回報",TbxTseRptSes_Creater},
   {"上櫃成交回報",TbxOtcRptSes_Creater},
   {"上市拍賣委託",TbxAuctSes_Creater},
//   {"上櫃拍賣委託",TbxOAuctSes_Creater},      // 上櫃拍賣作業是否己有支援,待查(若有則要修改subsysname)
   {"上市標借委託",TbxLendSes_Creater},
   {"上櫃標借委託",TbxOLendSes_Creater},
   {"上市標購委託",TbxTendSes_Creater},
   {"上櫃標購委託",TbxOTendSes_Creater},
   {"成交讀檔",PMachSesCreater,},
   {"上市委託讀檔",TDrSesCreater,},
   {"上櫃委託讀檔",ODrSesCreater,},
   {"上市線路測試",TbxTestSes_Creater,},
   {"上櫃線路測試",TbxOTestSes_Creater,},
   {"錯誤次數重置",TResetErrCntSes_Creater,},
};

/////////////////////////////////////////////////////////////////////////////
K_ctor TbxSesMgr::TbxSesMgr  (TSchedulerBase* sb, TOrdCenter* oc, bool istester)
   :inherited(std::string(GetProcLogPath()+"/ModifyLog/bxSesMgr.log").c_str())
{
   Scheduler_ = sb;
   OrdCenter_ = oc;
   IsTester_ = istester;
   MsgCntRec_= 0;
   NormalConf_ = new SLineConf(this);
   OddConf_    = new SLineConf(this);
   FixConf_    = new SLineConf(this);
   AuctConf_   = new SLineConf(this);
   LendConf_   = new SLineConf(this);
   TendConf_   = new SLineConf(this);
   Reg(Kway::TMesRegs::GetDefaultMesRegs());
   Reg(MesRegs, MesRegs+numofele(MesRegs));
   Reg(SesRegs, SesRegs+numofele(SesRegs));
   DelayTimer_ = new TbxTimer(this);
   Reqs_       = new TReqs();
   TFile Log_;
   Log_.Open(std::string(GetProcLogPath()+"version.log").c_str(), (TFileMode)(fmRandomAccess|fmOpenAlways));
   Log_.Seek(Log_.GetFileSize(), fsBegin);
   char Buf[256];
   sprintf(Buf,"%s libTMPSession loaded, version: 1.0.1.0, last modified date 2010-07-26.",
           TKTime::Now().as_string().c_str());
   Log_.WriteLN(Buf);
   Log_.Close();
#ifdef SUNLINK
   Reg(&Kway::TMesX25::GetReg());
#endif

   if(sb)
      MarketPh_ = sb->GetMarketPh();
   ResendOnMatch_       = false; // 20070517預設撮合中刪改求失敗時,不自動重送刪改
   NormalConf_->Active_ = true;
   NormalConf_->Wait_   = 3;
   OddConf_->Active_    = true;
   OddConf_->Wait_      = 3;
   FixConf_->Active_    = true;
   FixConf_->Wait_      = 3;
   AuctConf_->Active_   = true;
   AuctConf_->Wait_     = 3;
   LendConf_->Active_   = true;
   LendConf_->Wait_     = 3;
   TendConf_->Active_   = true;
   TendConf_->Wait_     = 3;
   AutoResend_          = false;
   PMachLN_             = true;
   AutoRequest_         = true;
   AutoJudge_           = false;
   ResendOnFail_        = false;
   Recorder_ = new TbxRecorder(BSOKFmt_);
   BrkSeqRec_ = new TBrkSeqRec(BrkMktSeq_, std::string(GetProcLogPath()+"BrkSeq.rec").c_str());
   ErrCntRec_ = new TErrCntRec(TrxErrCnt_, std::string(GetProcLogPath()+"TrxErrCnt.rec").c_str());
   if(IsTester_)
      LoadTesterConf();
   else
      LoadMainConf();
   bxVA_.Add("1.連線狀態", GetStatusVACreater());
   bxVA_.Add("2.連線設定", GetSettingVACreater());
   const TSysMgrObjs&   sys = GetSysMgrObjs();
   sys.SysVAMgr_->Add("TMP連線模組", bxVA_);
   for(int L = 0; L < 3; L++)
      for(int i=0; i<tk_None; i++){
         MarketFlag_[L][i] = false;  
         ExceedErrCnt_[L][i] = false;
      }
   if(GetExcMgr())
      GetExcMgr()->RegCmdMgrObj(this,"bxSesMgr");
   for(int it = tk_Normal; it <= tk_Tend; it++){
      Busy_.Set(m_Tse,(ETradeKind)it, false);
      Busy_.Set(m_Otc,(ETradeKind)it, false);
      Busy_.Set(m_OtcR,(ETradeKind)it, false);
      Busy_.SetWait(m_Tse,(ETradeKind)it, false);
      Busy_.SetWait(m_Otc,(ETradeKind)it, false);
      Busy_.SetWait(m_OtcR,(ETradeKind)it, false);
   }
   LineChgRec_.TradeKind_ = tk_None;
   LineChgRec_.BeingUsed_ = false;
}
//---------------------------------------------------------------------------
K_ctor TbxSesMgr::~TbxSesMgr  ()
{
   GetExcMgr()->DelCmdMgrObj(this);
   const TSysMgrObjs&   sys = GetSysMgrObjs();
   sys.SysVAMgr_->Del(bxVA_);
   bxVA_.Del(GetStatusVACreater());
   bxVA_.Del(GetSettingVACreater());
   delete NormalConf_;
   delete OddConf_;
   delete FixConf_;
   delete AuctConf_;
   delete LendConf_;
   delete TendConf_;
   delete Recorder_;
   delete DelayTimer_;
   while(Reqs_->size()){
      TReqsi iter = Reqs_->begin();
      TOrdFlow::TReqPtr& req = iter->second;
      req->Update(std::string(GetErrMsg(Msg_OrderFail)), of_Sent);
      req.reset();
      Reqs_->erase(iter);
   }
   delete Reqs_;
   delete BrkSeqRec_;
   delete ErrCntRec_;
   if(MsgCntRec_)
      delete MsgCntRec_;
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::OnDelayTimer(TTimerBase* tb)  // 委託斷線自動查單失敗單處理,以判斷DelayTime_秒內
{                                                   // 有無成交回報來判斷委託是否成功,新單無成交則視為委託失敗
   if(Reqs_->size() > 0){                           // 20090407刪單改量需另行判斷
      TReqsi iter = Reqs_->begin();
      while(1){
         if((TKTime::Now() - iter->first.Time_) > DelayTime_){
            TOrdHnd Hnd = OrdCenter_->GetOrdHnd(iter->first.Key_,TStkQty(0),uf_OrdFlow,TSysOrdID(0));
            TReqPtr& req = iter->second;
            const TStkOrder* OCOrd;
            TOrdCmdID cmd = req->GetOrdCmdID();
            if(!Hnd.is_invalid())
               OCOrd = Hnd.GetConst();
            if(OCOrd != 0 && (((!OCOrd->DealQty_.is_0()) && cmd == cid_New) || 
               ((cmd == cid_Del || cmd == cid_ChgQty) && OCOrd->DealQty_ < OCOrd->AfQty_))){
               TStkOrder& Order = req->GetOrdHnd().GetModify();
               switch(cmd){
                  case cid_New:
                     Order.AfQty_ = Order.Qty_;
                     Order.TseOrigTime_ = TKTime::Now();
                     Order.TseLastTime_ = TKTime::Now();
                     iter->second->Update(GetHostIDStr()+"委託OK!",of_Finish);
                     break;
                  case cid_Del:
                     Order.BfQty_ = Order.AfQty_;
                     Order.AfQty_ = Order.DealQty_;
                     Order.TseLastTime_ = TKTime::Now();
                     iter->second->Update(GetHostIDStr()+"刪單OK!",of_Finish);
                     break;
                  case cid_ChgQty:{
                     TStkQty qty = Order.LostQty_ - (Order.DealQty_ - Order.MatchQty_); // 檢查有無新成交回報
                     if(qty > 0){ // 狀況不明的數量大於新到達的成交量,判斷為改量ok
                        TStkQty reqqty = (static_cast<TReqChgQtyBase*>(req.get()))->GetReqQty(); // 刪減量
                        if(reqqty < 0){
                           int tmp = reqqty;
                           tmp = tmp * (-1); 
                           reqqty = tmp;
                        }else if(reqqty == 0)
                           reqqty = Order.AfQty_; 
                        if(qty > reqqty){//計算出的刪減量大於原始要求的刪淢量時, 應如何處理??維持不明狀態/重新等待??
                           char msg[32],unit_[2];
                           TStkQty lqty = Order.LeftQty()- qty;
                           int uqty = ((lqty%1000)? 1:1000);
                           strcpy(unit_ ,((lqty%1000)?"股":"張"));
                           sprintf(msg,"己成交%d%s, %d%s未成交, %d%s狀態不明!", 
                                   Order.DealQty_/uqty, unit_, lqty/uqty , unit_, 
                                   qty/uqty, unit_);
                           iter->second->Update(GetHostIDStr()+msg, of_Finish);
                           break;
                        }
                        Order.BfQty_  = Order.AfQty_;
                        Order.AfQty_ -= qty;
                        Order.TseLastTime_ = TKTime::Now();
                        iter->second->Update(GetHostIDStr()+"改量OK!",of_Finish);
                        break;
                     }else{      // 狀況不明的數量等於新到達的成交量判斷為改量失敗
                        if(Order.LeftQty() > 0 && IsResendOnFail()){//尚有剩量且設定要重送要求
                           ETradeKind tk;
                           switch(MarketPh_){
                              case ts_Full: tk = tk_Normal; break;
                              case ts_Odd : tk = tk_Odd   ; break;
                              case ts_Fix : tk = (OCOrd->Qty_%1000==0?tk_Fix:tk_Odd); break;
                              default: break;
                           }
                           if(!NewOrderAlert(OCOrd->Market_, req, tk))
                              DoAutoRequest(req, OCOrd->Market_, tk);
                           Reqs_->erase(iter);
                           if(Reqs_->size() > 0){
                              iter = Reqs_->begin();
                              continue;
                           }   
                        }else   // 減量失敗且[交易所己無剩量]或[設定不重送],直接回覆改量失敗
                           req->Update(std::string(GetErrMsg(Msg_OrderFail)),of_AbortOrdNoChange);
                     }
                     break;
                  }
                  default: continue;
               }
            }else{
               // 20090407 查詢失敗自動重送機制, [新委託]或[刪改單但仍有剩量]時才要處理
               if(IsResendOnFail() && (cmd == cid_New || (cmd != cid_New && OCOrd->LeftQty() > 0))){  
                  ETradeKind tk;
                  switch(MarketPh_){
                     case ts_Full: tk = tk_Normal; break;
                     case ts_Odd : tk = tk_Odd   ; break;
                     case ts_Fix : tk = (OCOrd->Qty_%1000==0?tk_Fix:tk_Odd); break;
                     default: break;
                  }
                  if(!NewOrderAlert(OCOrd->Market_, req, tk))
                     DoAutoRequest(req, OCOrd->Market_, tk);
                  Reqs_->erase(iter);
                  if(Reqs_->size() > 0){
                     iter = Reqs_->begin();
                     continue;
                  }else
                     break;
               }else{
                  req->Update(std::string(GetErrMsg(Msg_OrderFail)),
                              req->GetOrdCmdID()==cid_New?of_AbortOrdClear:of_AbortOrdNoChange);
               }
            }
            req->GetOrdHnd().FlushUpdate(uf_OrdFlow);
            req.reset();
            Reqs_->erase(iter);
            if(Reqs_->size() > 0)
               iter = Reqs_->begin();
            else
               break;
         }else
            break;
      }
   }
   if(Reqs_->size() == 0)
      DelayTimer_->Stop();
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::DoRequestFail  (TReqPtr& ReqPtr)
{
   const TStkOrder* Ord = ReqPtr->GetOrdHnd().GetConst();
   TTseOrdKey key(Ord->IvacKey_.GetBrokerID(), Ord->Market_, Ord->TseOrdID_);
   SOrdKeyTime KeyTime(key,TKTime::Now());
   TReqsi iter = Reqs_->find(KeyTime);
   if(iter == Reqs_->end())
      Reqs_->insert(std::make_pair(KeyTime,ReqPtr));
   iter = Reqs_->find(KeyTime);
   if(Reqs_->size() == 1)
      DelayTimer_->Start(1000,&this_type::OnDelayTimer);
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::OnTimer(TTimerBase* tb)
{
   TMarket    mkt;
   ETradeKind tk;
   if((NormalConf_->TimerT_) == tb){
      NormalConf_->TimerT_->Stop();
      mkt = m_Tse;
      tk  = tk_Normal;
   } else if((NormalConf_->TimerO_) == tb){
      NormalConf_->TimerO_->Stop();
      mkt = m_Otc;
      tk  = tk_Normal;
   } else if((OddConf_->TimerT_) == tb){
      OddConf_->TimerT_->Stop();
      mkt = m_Tse;
      tk  = tk_Odd;
   } else if((OddConf_->TimerO_) == tb){
      OddConf_->TimerO_->Stop();
      mkt = m_Otc;
      tk  = tk_Odd;
   } else if((FixConf_->TimerT_) == tb){
      FixConf_->TimerT_->Stop();
      mkt = m_Tse;
      tk  = tk_Fix;
   } else if((FixConf_->TimerO_) == tb){
      FixConf_->TimerO_->Stop();
      mkt = m_Otc;
      tk  = tk_Fix;
   } else if((AuctConf_->TimerT_) == tb){
      AuctConf_->TimerT_->Stop();
      mkt = m_Tse;
      tk  = tk_Auct;
   } else if((AuctConf_->TimerO_) == tb){
      AuctConf_->TimerO_->Stop();
      mkt = m_Otc;
      tk  = tk_Auct;
   } else if((LendConf_->TimerT_) == tb){
      LendConf_->TimerT_->Stop();
      mkt = m_Tse;
      tk  = tk_Lend;
   } else if((LendConf_->TimerO_) == tb){
      LendConf_->TimerO_->Stop();
      mkt = m_Otc;
      tk  = tk_Lend;
   } else if((TendConf_->TimerT_) == tb){
      TendConf_->TimerT_->Stop();
      mkt = m_Tse;
      tk  = tk_Tend;
   } else if((TendConf_->TimerO_) == tb){
      TendConf_->TimerO_->Stop();
      mkt = m_Otc;
      tk  = tk_Tend;
   }
   if(Busy_.Test(mkt,tk)){
      Scheduler_->SetLineState(mkt, tk, GetSesNum(mkt,tk));  // 超過60秒仍無法啟動線路,放棄所有等待中的委託
      Busy_.Set(mkt,tk,false);
      Busy_.SetWait(mkt,tk,false);
      for(int it = tk_Normal; it <= tk_Tend; it++)
         if(Busy_.TestWait(mkt,(ETradeKind)it)){
            SwitchLine(mkt, (ETradeKind)it);
            return;
         }
      LineChgRec_.TradeKind_ = tk_None;
      LineChgRec_.BeingUsed_ = false;
      RecoverLine(mkt,tk);                   // 並將線路恢復回原委託線路
   }else if(Busy_.TestWait(mkt,tk)) {         
      SwitchLine(mkt,tk);
   }else{  // 全部處理完成, 恢復為原盤別
      Busy_.Set(mkt,tk,false);
      Busy_.SetWait(mkt,tk,false);
      LineChgRec_.TradeKind_ = tk_None;
      LineChgRec_.BeingUsed_ = false;
      RecoverLine(mkt,tk);                   // 並將線路恢復回原委託線路
  }
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::ResetTimer(TMarket mkt, ETradeKind tk, Kway::word sec, bool isstop)
{
//   if(!IsAutoStart(tk_Auct))
//      return;
   switch(tk){
      case tk_Normal: 
         switch(mkt){
            case m_Tse: NormalConf_->TimerT_->Stop(); 
                        if(!isstop)
                           NormalConf_->TimerT_->Start(sec*1000, &this_type::OnTimer); break;
            case m_Otc: NormalConf_->TimerO_->Stop();
                        if(!isstop)
                           NormalConf_->TimerO_->Start(sec*1000, &this_type::OnTimer); break;
         }
         break;
      case tk_Odd: 
         switch(mkt){
            case m_Tse: OddConf_->TimerT_->Stop();
                        if(!isstop)
                           OddConf_->TimerT_->Start(sec*1000, &this_type::OnTimer); break;
            case m_Otc: OddConf_->TimerO_->Stop();
                        if(!isstop)
                           OddConf_->TimerO_->Start(sec*1000, &this_type::OnTimer); break;
         }
         break;
      case tk_Fix: 
         switch(mkt){
            case m_Tse: FixConf_->TimerT_->Stop();
                        if(!isstop)
                           FixConf_->TimerT_->Start(sec*1000, &this_type::OnTimer); break;
            case m_Otc: FixConf_->TimerO_->Stop();
                        if(!isstop)
                           FixConf_->TimerO_->Start(sec*1000, &this_type::OnTimer); break;
         }
         break;
      case tk_Auct: 
         switch(mkt){
            case m_Tse: AuctConf_->TimerT_->Stop(); 
                        if(!isstop)
                           AuctConf_->TimerT_->Start(sec*1000, &this_type::OnTimer); break;
            case m_Otc: AuctConf_->TimerO_->Stop();
                        if(!isstop)
                           AuctConf_->TimerO_->Start(sec*1000, &this_type::OnTimer); break;
         }
         break;
      case tk_Lend: 
         switch(mkt){
            case m_Tse: LendConf_->TimerT_->Stop();
                        if(!isstop)
                           LendConf_->TimerT_->Start(sec*1000, &this_type::OnTimer); break;
            case m_Otc: LendConf_->TimerO_->Stop();
                        if(!isstop)
                           LendConf_->TimerO_->Start(sec*1000, &this_type::OnTimer); break;
         }
         break;
      case tk_Tend: 
         switch(mkt){
            case m_Tse: TendConf_->TimerT_->Stop();
                        if(!isstop)
                           TendConf_->TimerT_->Start(sec*1000, &this_type::OnTimer); break;
            case m_Otc: TendConf_->TimerO_->Stop();
                        if(!isstop)
                           TendConf_->TimerO_->Start(sec*1000, &this_type::OnTimer); break;
         }
         break;
   }
}
//---------------------------------------------------------------------------
K_mf(bool) TbxSesMgr::IsAutoStart(ETradeKind tk)
{
   return true;
   switch(tk){
      case tk_Auct: return AuctConf_->Active_;
      case tk_Lend: return LendConf_->Active_;
      case tk_Tend: return TendConf_->Active_;
   }
   return false;
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::SetMarketPh(TSegDefs* segs)
{
   SegDefs_ = *segs;
   SegDefs_.Reset();
   for(TSesRegi L = OrdSesRegs_.begin(); L != OrdSesRegs_.end(); L++){
      switch((*L)->GetMarket()){
         case m_Tse: 
            SegDefs_.Tse_.TotalL_ += 1;
            break;
         case m_Otc:
            SegDefs_.Otc_.TotalL_ += 1;
            break;
      }
   }
   for(int L = 0; L <= tk_Fix; L++){
      if(SegDefs_.Tse_.Seg_[L].IsOpen_)
         SegDefs_.Tse_.Seg_[L].TargetL_ = SegDefs_.Tse_.TotalL_ * 
                                          SegDefs_.Tse_.Seg_[L].Weight_ / SegDefs_.Tse_.GetTotWeight();
      if(SegDefs_.Otc_.Seg_[L].IsOpen_)
         SegDefs_.Otc_.Seg_[L].TargetL_ = SegDefs_.Otc_.TotalL_ * 
                                          SegDefs_.Otc_.Seg_[L].Weight_ / SegDefs_.Otc_.GetTotWeight();
   }
   int Diff = SegDefs_.Tse_.TotalL_ - (SegDefs_.Tse_.Seg_[tk_Normal].TargetL_ + 
                                       SegDefs_.Tse_.Seg_[tk_Odd].TargetL_ + 
                                       SegDefs_.Tse_.Seg_[tk_Fix].TargetL_ );
   while(Diff > 0){
      if(!(SegDefs_.Tse_.IsOpen())) // 若無任何segment
         break;
      SegDefs_.Tse_.Seg_[SegDefs_.Tse_.GetMinRatio()].TargetL_ ++;
      Diff--;
   }
   Diff = SegDefs_.Otc_.TotalL_ - (SegDefs_.Otc_.Seg_[tk_Normal].TargetL_ + 
                                       SegDefs_.Otc_.Seg_[tk_Odd].TargetL_ + 
                                       SegDefs_.Otc_.Seg_[tk_Fix].TargetL_ );
   while(Diff > 0){
      if(!(SegDefs_.Otc_.IsOpen()))
         break;
      SegDefs_.Otc_.Seg_[SegDefs_.Otc_.GetMinRatio()].TargetL_ ++;
      Diff--;
   }
   for(TSesRegi L = OrdSesRegs_.begin(); L != OrdSesRegs_.end(); L++){
      switch((*L)->GetMarket()){
         case m_Tse:{ 
            if(!(SegDefs_.Tse_.IsOpen())) // 無任何交易時段, 則不異動目前狀態
               break;
            ETradeKind tk = tk_Normal;  // 預設為普通交易時段
            for(int L1 = 0; L1 <= tk_Fix; L1++){
               if(SegDefs_.Tse_.Seg_[L1].CurrentL_ < SegDefs_.Tse_.Seg_[L1].TargetL_){
                  tk = (ETradeKind)L1;
                  break;
               }
            }
            if(tk != GetSesTK(*L)){
               TApCode apcode;
               switch(tk){
                  case tk_Odd: apcode.assign("2"); break;
                  case tk_Fix: apcode.assign("7"); break;
                  default:     apcode.assign("0"); break;
               }
               TTseOrdSes* SesPtr;    
               SesPtr = reinterpret_cast<TTseOrdSes*>(*L); 
               SesPtr->SetApCode(apcode);
               ((TTseOrdSes*)SesPtr)->ReStartLink();
            }
            SegDefs_.Tse_.Seg_[tk].CurrentL_ += 1;
            break;
         }
         case m_Otc:{
            if(!(SegDefs_.Otc_.IsOpen())) // 無任何交易時段, 則不異動目前狀態
               break;
            ETradeKind tk = tk_Normal;
            for(int L1 = 0; L1 <= tk_Fix; L1++){
               if(SegDefs_.Otc_.Seg_[L1].CurrentL_ < SegDefs_.Otc_.Seg_[L1].TargetL_){
                  tk = (ETradeKind)L1;
                  break;
               }
            }
            if(tk != GetSesTK(*L)){
               TApCode apcode;
               switch(tk){
                  case tk_Odd: apcode.assign("2"); break;
                  case tk_Fix: apcode.assign("7"); break;
                  default:     apcode.assign("0"); break;
               }
               TTseOrdSes* SesPtr;    
               SesPtr = reinterpret_cast<TTseOrdSes*>(*L); 
               SesPtr->SetApCode(apcode);
               ((TTseOrdSes*)SesPtr)->ReStartLink();
            }
            SegDefs_.Otc_.Seg_[tk].CurrentL_ += 1;
            break;
         }
      }
   }
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::SetMarketPh(TTrxSegment mph)  //換盤作業
{
   if(MarketPh_ > mph)
      return;
   TApCode apcode;
   switch(mph){
      case ts_Fix:   apcode.assign("7"); break;
      case ts_Odd:   apcode.assign("2"); break;
      default:       apcode.assign("0"); break;
   }
   TbxOrderSes* SesPtr;
   if(OrdSesRegs_.size() != 0){
      for(int L = OrdSesRegs_.size()-1; L >= 0; L--){
         SesPtr = reinterpret_cast<TbxOrderSes*>(OrdSesRegs_.at(L));
         if(SesPtr && (!SesPtr->IsSending())){  // 如果線路正在傳送委託中, 則先不切換, 等到OnSesFree(..)時, 再行判斷是否切換
            if(mph == ts_Fix && (!SesPtr->IsFix())){  // 盤後時段, 非盤後線路的處理方式
               if(MarketPh_ == ts_Full){       // 略過零股時段,直接啟動盤後時應自動將零股線路帶起
                  TApCode apc;
                  apc.assign("2");
                  SesPtr->SetApCode(apc);
                  ((TbxLinkSes*)SesPtr)->ReStartLink();
                }
               continue;
            }
            SesPtr->SetApCode(apcode);
            ((TbxLinkSes*)SesPtr)->ReStartLink();
         }
      }
   }
   MarketPh_ = mph;
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::RegbxSes(TbxSes* SesPtr)
{
   TSesRegi pos = std::find(OrdSesRegs_.begin(),OrdSesRegs_.end(),SesPtr);
   if(pos == OrdSesRegs_.end())          //先檢查有沒相同的Ses 已註冊,若有則不再重覆註冊
       OrdSesRegs_.push_back(SesPtr);
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::DelbxSes(TbxSes* SesPtr)
{
   TSesRegi pos = std::find(OrdSesRegs_.begin(),OrdSesRegs_.end(),SesPtr);
   if(pos != OrdSesRegs_.end())
      OrdSesRegs_.erase(pos);
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::RegOrderSes(TbxSes* SesPtr)
{
   TMarket mkt = SesPtr->GetMarket();
   ETradeKind tk = GetSesTK(SesPtr);
   std::string procname = "XCtrl";
   std::string objname  = "LineErrCntMgr";
   char buf[32];
   switch(mkt){
      case m_Tse:
         RegSes(SesPtr, TseSes_, mkt, tk, true);
         break;
      case m_Otc:
         RegSes(SesPtr, OtcSes_, mkt, tk, true);
         break;
      case m_OtcR:
         RegSes(SesPtr, OtcRSes_, mkt, tk, true);
         break;
   }
   for(TTrxErrCnt::iterator L = TrxErrCnt_.begin(); L != TrxErrCnt_.end(); L++){
	   ETradeKind tk = GetMktTKBySub(L->first, mkt);
      int lines = (mkt==m_Tse?SegDefs_.Tse_:SegDefs_.Otc_).Seg_[tk].TargetL_;
      if(lines <= 0)
         lines = 1;
      sprintf(buf,"%4s%7s%02d%04d%04d",GetProcBrkID().as_string().c_str(),GetProcName().c_str(),
                                   L->first, L->second, (lines*LineErrCnt_));
	   GetExcMgr()->CmdReq(procname,objname,1,std::string(buf));
      if((L->second) > 0 && (L->second) >= (lines*LineErrCnt_))
	      ExceedErrCnt_[mkt==m_Tse?0:1][tk] = true;
	   else
	      ExceedErrCnt_[mkt==m_Tse?0:1][tk] = false;
	}
	// 20100629 當啟動後第一次完成所有設定線路AP層連線後, 將此旗標設為TRUE
	if(OrdSesRegs_.size() == (TseSes_.GetReadyLines() + OtcSes_.GetReadyLines() + OtcRSes_.GetReadyLines())) 
	   SetAllTMPReady(true);
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::DelOrderSes(TbxSes* SesPtr)
{
   TMarket mkt = SesPtr->GetMarket();
   ETradeKind tk = GetSesTK(SesPtr);
   switch(mkt){
      case m_Tse:
         RegSes(SesPtr, TseSes_, mkt, tk, false);
         break;
      case m_Otc:
         RegSes(SesPtr, OtcSes_, mkt, tk, false);
         break;
      case m_OtcR:
         RegSes(SesPtr, OtcRSes_, mkt, tk, false);
         break;
   }
}
//---------------------------------------------------------------------------
K_mf(ETradeKind) TbxSesMgr::GetSesTK(TbxSes* SesPtr)
{
   ETradeKind tk;
   switch(SesPtr->GetSubSys().as_int()){
      case 30:     // 上市整股
      case 93:     // 上櫃整股
         tk = tk_Normal;
         break;
      case 32:     // 上市盤後
      case 98:     // 上櫃盤後
         tk = tk_Fix;
         break;
      case 40:     // 上市零股
      case 94:     // 上櫃零股
         tk = tk_Odd;
         break;
      case 70:     // 上市拍賣
         tk = tk_Auct;
         break;
      case 31:     // 上市標借
      case 90:     // 上櫃標借
         tk = tk_Lend;
         break;
      case 41:     // 上市標購
      case 97:     // 上櫃標購
         tk = tk_Tend;
         break;
      default:
         tk = tk_Normal;
         break;
   }
   return tk;
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::RegSes(TbxSes* SesPtr, TTrSesRegs& SesRegs, TMarket mkt, ETradeKind tk, bool isAdd)
{
// 20100721 避免因為SendOrdInterval 到達而主動作線路切換, 導至全部委託失敗, 所以在WAIT 狀態時, 主動將線路數量加1
   int linewait = ((Busy_.TestWait(mkt,tk)&&(!isAdd))?1:0); 
   switch(tk){
      case tk_Normal:
         if(RegSesDetail(SesPtr, SesRegs.Normal_, isAdd) && Scheduler_)
            Scheduler_->SetLineState(mkt, tk, SesRegs.Normal_.size()+linewait);
         break;   
      case tk_Fix   :
         if(RegSesDetail(SesPtr, SesRegs.Fix_, isAdd) && Scheduler_)
            Scheduler_->SetLineState(mkt, tk, SesRegs.Fix_.size()+linewait);
         break;   
      case tk_Odd:
         if(RegSesDetail(SesPtr, SesRegs.Odd_, isAdd) && Scheduler_)
            Scheduler_->SetLineState(mkt, tk, SesRegs.Odd_.size()+linewait);
         break;   
      case tk_Auct:
         if(RegSesDetail(SesPtr, SesRegs.Auct_, isAdd) && Scheduler_)
            Scheduler_->SetLineState(mkt, tk, SesRegs.Auct_.size()+linewait);
         break;   
      case tk_Lend:
         if(RegSesDetail(SesPtr, SesRegs.Lend_, isAdd) && Scheduler_)
            Scheduler_->SetLineState(mkt, tk, SesRegs.Lend_.size()+linewait);
         break;   
      case tk_Tend:
         if(RegSesDetail(SesPtr, SesRegs.Tend_, isAdd) && Scheduler_)
            Scheduler_->SetLineState(mkt, tk, SesRegs.Tend_.size()+linewait);
         break;
   }  
}
//---------------------------------------------------------------------------
K_mf(bool) TbxSesMgr::RegSesDetail(TbxSes* SesPtr, TbxSesRegs& SesRegs, bool isAdd)
{
   TSesRegi pos = std::find(SesRegs.begin(),SesRegs.end(),SesPtr);
   if(isAdd){
      if(pos == SesRegs.end())
         SesRegs.push_back(SesPtr);
      else 
         return false;
   }else{
      if(pos != SesRegs.end())
         SesRegs.erase(pos);
      else 
         return false;
   }
   return true;
}
//---------------------------------------------------------------------------
K_mf(TbxSes*) TbxSesMgr::GetSes(TTrSesRegs& SesRegs, ETradeKind tk)
{
   switch(tk){
      case tk_Fix   : return GetSesDetail(SesRegs.Fix_);
      case tk_Normal: return GetSesDetail(SesRegs.Normal_);
      case tk_Odd   : return GetSesDetail(SesRegs.Odd_);
      case tk_Auct  : return GetSesDetail(SesRegs.Auct_  );
      case tk_Lend  : return GetSesDetail(SesRegs.Lend_  );
      case tk_Tend  : return GetSesDetail(SesRegs.Tend_  );
   }  
}
//---------------------------------------------------------------------------
K_mf(TbxSes*) TbxSesMgr::GetSesDetail(TbxSesRegs& SesRegs)
{
   TbxSes* SesPtr;
   if(SesRegs.size() != 0)
      for(TSesRegi pos = SesRegs.begin();pos != SesRegs.end(); ++pos){
         SesPtr = static_cast<TbxSes*>(*pos);
         if(SesPtr->IsFree())
             return SesPtr;
      }
   return 0;
}
//---------------------------------------------------------------------------
K_mf(TbxSes*) TbxSesMgr::GetOrderSes(Stk::TMarket mkt, ETradeKind tk)
{
   switch(mkt){
      case m_Tse:  return GetSes(TseSes_,  tk);
      case m_Otc:  return GetSes(OtcSes_,  tk);
      case m_OtcR: return GetSes(OtcRSes_, tk);
   }
}
//---------------------------------------------------------------------------
K_mf(Kway::word) TbxSesMgr::GetSesNum(Stk::TMarket mkt, ETradeKind tk)
{
   switch(mkt){
      case m_Tse:  return GetSesNumDetail(TseSes_,  tk);
      case m_Otc:  return GetSesNumDetail(OtcSes_,  tk);
      case m_OtcR: return GetSesNumDetail(OtcRSes_, tk);
   }
   return 0;
}
//---------------------------------------------------------------------------
K_mf(Kway::word) TbxSesMgr::GetSesNumDetail(TTrSesRegs& SesRegs, ETradeKind tk)
{
   switch(tk){
      case tk_Fix   : return  SesRegs.Fix_   .size();
      case tk_Normal: return  SesRegs.Normal_.size();
      case tk_Odd   : return  SesRegs.Odd_   .size();
      case tk_Auct  : return  SesRegs.Auct_  .size();
      case tk_Lend  : return  SesRegs.Lend_  .size();
      case tk_Tend  : return  SesRegs.Tend_  .size();
   } 
   return 0; 
}
//---------------------------------------------------------------------------
K_mf(bool) TbxSesMgr::NewOrderAlert(Stk::TMarket mkt, TReqPtr& ReqPtr, ETradeKind tk)
{
   if(MarketFlag_[mkt==m_Tse?0:1][tk]/* && ReqPtr->GetOrdCmdID() != cid_Qry*/){
      TStkOrder&  StkOrder = ReqPtr->GetOrdHnd().GetModify();
      StkOrder.LastTseCode_ = 1;
      StkOrder.LastOCmd_ = ReqPtr->GetOrdCmdID();
      char buf[16];
      sprintf(buf,"%02d%02d",GetSubSystemTK(mkt,tk), StkOrder.LastTseCode_);
      ReqPtr->Update(std::string(GetErrMsg(buf)),ReqPtr->GetOrdCmdID()==cid_New?of_AbortOrdClear:of_AbortOrdNoChange); 
      return true;
   }
   if(ExceedErrCnt_[mkt==m_Tse?0:1][tk]){ // 檢查交易盤別erroc count 是否超過
      TStkOrder&  StkOrder = ReqPtr->GetOrdHnd().GetModify();
      StkOrder.LastTseCode_ = 77;
      StkOrder.LastOCmd_ = ReqPtr->GetOrdCmdID();
      char buf[16];
      sprintf(buf,"%02d%02d",GetSubSystemTK(mkt,tk), StkOrder.LastTseCode_);
      ReqPtr->Update(std::string(GetErrMsg(buf)),ReqPtr->GetOrdCmdID()==cid_New?of_AbortOrdClear:of_AbortOrdNoChange); 
      return true;
   }
   if(tk==tk_Normal){ // 檢查委託的SendingTime 是否逾時, 僅整股時段需要做此檢查
      const TStkOrder*  StkOrder = ReqPtr->GetOrdHnd().GetConst();
      int timegap = TKDateTimeMS::Now() - StkOrder->SendingTime_;
      if(MaxTimeGap_ > 0 && abs(timegap) > (MaxTimeGap_ * 1000)){
         TStkOrder&  StkOrder = ReqPtr->GetOrdHnd().GetModify();
         StkOrder.LastTseCode_ = 97;
         StkOrder.LastOCmd_ = ReqPtr->GetOrdCmdID();
         char buf[16];
         sprintf(buf,"%02d%02d",GetSubSystemTK(mkt,tk), StkOrder.LastTseCode_);
         ReqPtr->Update(std::string(GetErrMsg(buf)),ReqPtr->GetOrdCmdID()==cid_New?of_AbortOrdClear:of_AbortOrdNoChange); 
         return true;
      }
   }
   if (!Busy_.Test(mkt,tk)){
      TbxSes* SesPtr = GetOrderSes(mkt, tk);
      if(SesPtr != 0){
         if(ReqPtr.get() != 0){
            switch(tk){
               case tk_Odd   :
               case tk_Fix   :
               case tk_Normal: (dynamic_cast<TTseOrdSes*>(SesPtr))->OnRequest(ReqPtr); break;
               case tk_Auct  : (dynamic_cast<TbxAuctSes*> (SesPtr))->OnRequest(ReqPtr); 
                               ResetTimer(mkt,tk_Auct); break;
               case tk_Lend  : (dynamic_cast<TbxLendSes*> (SesPtr))->OnRequest(ReqPtr); 
                               ResetTimer(mkt,tk_Lend); break;
               case tk_Tend  : (dynamic_cast<TbxTendSes*> (SesPtr))->OnRequest(ReqPtr); 
                               ResetTimer(mkt,tk_Tend); break;
            }
            return true;
         }
      } else{
         if(GetSesNum(mkt, tk)){
            Busy_.Set(mkt,tk,true);
            return false;
         }
//       20110518 eric: 有登記這個盤別的線路, 但全部連線失敗;表示現在無法進行此盤別的交易
//         放棄委託;
         if(OrdSesRegs_.size())
            for(int pos = OrdSesRegs_.size()-1;pos >= 0; pos--){
               TbxSes* Ses = static_cast<TbxSes*>(OrdSesRegs_.at(pos));
               if(tk == GetSesTK(Ses) && Ses->GetMarket() == mkt){   //有相同盤別的線路
                  TStkOrder&  StkOrder = ReqPtr->GetOrdHnd().GetModify();
                  StkOrder.LastTseCode_ = 5;
                  StkOrder.LastOCmd_ = ReqPtr->GetOrdCmdID();
                  char buf[16];
                  sprintf(buf,"%02d%02d",mkt==m_Tse?10:91, StkOrder.LastTseCode_);
                  ReqPtr->Update(std::string(GetErrMsg(buf)),ReqPtr->GetOrdCmdID()==cid_New?of_AbortOrdClear:of_AbortOrdNoChange); 
                  return true;
               }
            }
         if(LineChgRec_.TradeKind_ != tk && EnableAutoSwitch_){
            Busy_.Set(mkt,tk,false);
            Busy_.SetWait(mkt,tk,true);
            if(LineChgRec_.TradeKind_ == tk_None)
               SwitchLine(mkt, tk);
         }
      }
   }
   return false;
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::OnSesFree(TSesBase* Ses, Stk::TMarket mkt)
{
   ETradeKind tk = GetSesTK((TbxSes*)Ses);
   if(MarketPh_ > ts_Full && tk <= tk_Fix){   // 判斷是否要進行線路切換, 拍賣標借標購不作此判斷
      if(tk == tk_Normal && MarketPh_ == ts_Odd){ 
         TApCode apc;
         apc.assign("2");
         ((TbxSes*)Ses)->SetApCode(apc);
         ((TbxLinkSes*)Ses)->ReStartLink();
         return;
      }
      if(MarketPh_ == ts_Fix && ((TbxSes*)Ses)->IsFix() && tk != tk_Fix){
         TApCode apc;
         apc.assign("7");
         ((TbxSes*)Ses)->SetApCode(apc);
         ((TbxLinkSes*)Ses)->ReStartLink();
         return;
      }
   }
   // 20100721 送單超過設定時限後, 需主動將線路使用權交出予其他盤別使用
   if(tk == LineChgRec_.TradeKind_){
      if(LineChgRec_.BeingUsed_){
         if(SendOrderInterval_ < (TKTime::Now() - LineChgRec_.BeginTime_)){
            LineChgRec_.TradeKind_ = tk_None;
            LineChgRec_.BeingUsed_ = false;
            Busy_.SetWait(mkt,tk,true);
            Busy_.Set(mkt,tk,false);
            int tk_now = tk;
            tk_now++;
            for(int it = tk_now; it <= tk_Tend; it++)
               if(Busy_.TestWait(mkt,(ETradeKind)it)){
                  ResetTimer(mkt, (ETradeKind)it,0);
                  return;
               }
            for(int it = tk_Normal; it < tk; it++)
               if(Busy_.TestWait(mkt,(ETradeKind)it)){
                  ResetTimer(mkt, (ETradeKind)it,0);
                  return;
               }
            LineChgRec_.TradeKind_ = tk;
            LineChgRec_.BeingUsed_ = true;
            Busy_.SetWait(mkt,tk,false);
            Busy_.Set(mkt,tk,true);
            LineChgRec_.BeginTime_ = TKTime::Now();
         }
      }else{
         Busy_.SetWait(mkt,tk,false);
         LineChgRec_.BeingUsed_ = true;
         LineChgRec_.BeginTime_ = TKTime::Now();
         ResetTimer(mkt, tk,0,true);  // 只是為了停止TIMER
      }
   }
   TReqPtr ReqPtr = Scheduler_->GetOrderByMkt(mkt, tk);
   while((ReqPtr.get() != 0) && ExceedErrCnt_[mkt==m_Tse?0:1][tk]){ // 檢查交易盤別erroc count 是否超過
      TStkOrder&  StkOrder = ReqPtr->GetOrdHnd().GetModify();
      StkOrder.LastTseCode_ = 77;
      StkOrder.LastOCmd_ = ReqPtr->GetOrdCmdID();
      char buf[16];
      sprintf(buf,"%02d%02d",GetSubSystemTK(mkt,tk), StkOrder.LastTseCode_);
      ReqPtr->Update(std::string(GetErrMsg(buf)),ReqPtr->GetOrdCmdID()==cid_New?of_AbortOrdClear:of_AbortOrdNoChange); 
      ReqPtr.reset();
      ReqPtr = Scheduler_->GetOrderByMkt(mkt, tk);
   }
   if(tk==tk_Normal){ // 檢查委託的SendingTime 是否逾時, 僅整股時段需要做此檢查
      while((ReqPtr.get() != 0) && (MaxTimeGap_ > 0) && (abs(TKDateTimeMS::Now() - ReqPtr->GetOrdHnd().GetConst()->SendingTime_) > (MaxTimeGap_ * 1000))){
         TStkOrder&  StkOrder = ReqPtr->GetOrdHnd().GetModify();
         StkOrder.LastTseCode_ = 97;
         StkOrder.LastOCmd_ = ReqPtr->GetOrdCmdID();
         char buf[16];
         sprintf(buf,"%02d%02d",GetSubSystemTK(mkt,tk), StkOrder.LastTseCode_);
         ReqPtr->Update(std::string(GetErrMsg(buf)),ReqPtr->GetOrdCmdID()==cid_New?of_AbortOrdClear:of_AbortOrdNoChange); 
         ReqPtr.reset();
         ReqPtr = Scheduler_->GetOrderByMkt(mkt, tk);
      }
   }
   if(ReqPtr.get() != 0){
      switch(tk){
      	case tk_Fix   :
      	case tk_Odd   :
         case tk_Normal: //(dynamic_cast<TbxOrderSes*>(Ses))->OnRequest(ReqPtr); 
                         (dynamic_cast<TTseOrdSes*>(Ses))->OnRequest(ReqPtr); 
                         break;
         case tk_Auct  : (dynamic_cast<TbxAuctSes*> (Ses))->OnRequest(ReqPtr); 
                         ResetTimer(mkt,tk_Auct); 
                         break;
         case tk_Lend  : (dynamic_cast<TbxLendSes*> (Ses))->OnRequest(ReqPtr); 
                         ResetTimer(mkt,tk_Lend); 
                         break;
         case tk_Tend  : (dynamic_cast<TbxTendSes*> (Ses))->OnRequest(ReqPtr); 
                         ResetTimer(mkt,tk_Tend); 
                         break;
      }
   }else{
      bool dorecover = false;
      if(tk == LineChgRec_.TradeKind_){
         if(LineChgRec_.BeingUsed_){
            LineChgRec_.TradeKind_ = tk_None;
            LineChgRec_.BeingUsed_ = false;
            dorecover = true;
         }
      }
      Busy_.Set(mkt, tk, false);
      Busy_.SetWait(mkt,tk,false);
      if(LineChgRec_.TradeKind_ != tk_None)
         return;
      int tk_now = tk;
      tk_now++;
      for(int it = tk_now; it <= tk_Tend; it++){
         if(Busy_.TestWait(mkt,(ETradeKind)it)){
            ResetTimer(mkt, (ETradeKind)it,0);
            return;
         }
      }
      for(int it = tk_Normal; it < tk; it++){
         if(Busy_.TestWait(mkt,(ETradeKind)it)){
            ResetTimer(mkt, (ETradeKind)it,0);
            return;
         }
      }
      if(dorecover)
         ResetTimer(mkt,tk,0);
   }
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::SwitchLine(Stk::TMarket mkt, ETradeKind tk)
{
   ETradeKind otk;
   TbxSes* Ses;
   TbxSes* NewSes;
   TApCode apcode;
   if(OrdSesRegs_.size() == 0){  //完全無線路
      Busy_.Set(mkt,tk,true);
      Busy_.SetWait(mkt,tk,false);
      ResetTimer(mkt,tk,1);     // 開始計時,1秒內放棄所有排隊中的委託
      return;
   }else{
      for(int pos = OrdSesRegs_.size()-1;pos >= 0; pos--){
         Ses = static_cast<TbxSes*>(OrdSesRegs_.at(pos));
         otk = GetSesTK(Ses);
         if(Ses->GetMarket() == mkt)   //有符合的線路可供切換
            break;
         if(pos == 0){                                //有線路但全都不符合, 則放棄所有委託
            Busy_.Set(mkt,tk,true);
            Busy_.SetWait(mkt,tk,false);
            ResetTimer(mkt,tk,1);     // 開始計時,1秒內放棄所有排隊中的委託
            return;
         }
      }
   }
   if(OrdSesRegs_.size()){
      for(int pos = OrdSesRegs_.size()-1;pos >= 0; pos--){
         Ses = static_cast<TbxSes*>(OrdSesRegs_.at(pos));
         otk = GetSesTK(Ses);
         if(Ses->GetMarket() == mkt/* && otk <= tk_Fix */&& (!Ses->IsSending())){
            TaViewArea VA = GetStatusVACreater().CreateNewVA();
            VA->MoveCurRow(0, vcmfEnd);
            TbxSes* bsEnd = static_cast<TbxSes*>(GetCurrentSes(VA.get()));
            while(bsEnd == 0){
               VA->MoveCurRow(-1, vcmfCurrent);
               bsEnd = static_cast<TbxSes*>(GetCurrentSes(VA.get()));
            }
            VA->MoveCurRow(0, vcmfBegin);
            TbxSes* bs = static_cast<TbxSes*>(GetCurrentSes(VA.get()));
            while(bs != bsEnd && Ses != bs){
               VA->MoveCurRow(1, vcmfCurrent);
               bs = static_cast<TbxSes*>(GetCurrentSes(VA.get()));
            }
            std::string SesStr_;
            if(bs != Ses)
               return;
            Busy_.SetWait(mkt, tk, false);
            switch(mkt){
               case m_Tse: SesStr_ = std::string("上市"); break;
               case m_Otc: SesStr_ = std::string("上櫃"); break;
               default   : return;
            }
            switch(tk){
               case tk_Normal:
                  NewSes = static_cast<TbxSes*>(new TTseOrdSes(this, mkt));
                  SesStr_ = SesStr_ + std::string("普通委託");
                  apcode.assign("0");
                  break;
               case tk_Odd:
                  NewSes = static_cast<TbxSes*>(new TTseOrdSes(this, mkt));
                  SesStr_ = SesStr_ + std::string("普通委託");
                  apcode.assign("2");
                  break;
               case tk_Fix:
                  NewSes = static_cast<TbxSes*>(new TTseOrdSes(this, mkt));
                  SesStr_ = SesStr_ + std::string("普通委託");
                  apcode.assign("7");
                  break;
               case tk_Auct:
             	   NewSes = static_cast<TbxSes*>(new TbxAuctSes(this, mkt));
             	   apcode.assign("5");
                  SesStr_ = SesStr_ + std::string("拍賣委託");
            	   break;
               case tk_Lend:
             	   NewSes = static_cast<TbxSes*>(new TbxLendSes(this, mkt));
             	   apcode.assign("4");
                  SesStr_ = SesStr_ + std::string("標借委託");
            	   break;
               case tk_Tend:
             	   NewSes = static_cast<TbxSes*>(new TbxTendSes(this, mkt));
                  switch(mkt){
                     case m_Tse: apcode.assign("6"); break;
                     case m_Otc: apcode.assign("8"); break;
                     default   : return;
                  }
                  SesStr_ = SesStr_ + std::string("標購委託");
            	   break;
               default: return;
            } 
            NewSes->SetMes(Ses->GetMes());
            NewSes->SetEvHandler(Ses->GetbxSesEvHandler());
            NewSes->SettingApply(Ses->GetSetting());
            Ses->SetEvHandler((TSesEvHandler*)0);
            TViewCursor* cur = VA->GetCurRow()->Clone();
            if(!cur->IsInvalid()){
               cur->PutStr(1, SesStr_, VA.get());
            }
            delete cur;
            delete Ses;
            Ses = NewSes;
            Busy_.Set(mkt,tk,true);
            ResetTimer(mkt,tk,LineChgInterval_);     // 開始計時, LineChgInterval_秒內無法啟動則放棄所有排隊中的委託
            LineChgRec_.TradeKind_ = tk;
            LineChgRec_.BeingUsed_ = false;
            Ses->SetApCode(apcode);
            Ses->Open();
            ((TbxLinkSes*)Ses)->ReStartLink();
            return;;
         }
      }
   }
   Busy_.SetWait(mkt, tk, true);
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::RecoverLine(Stk::TMarket mkt, ETradeKind tk)
{
   ETradeKind otk;
   TbxSes* Ses;
   TbxSes* NewSes;
   std::string SesStr_;
   TApCode apcode,origapcode;
   switch(MarketPh_){     // 線路還原時要設定到正確的交易時段
      case ts_Fix:   apcode.assign("7"); break;
      case ts_Odd:   apcode.assign("2"); break;
      default:       apcode.assign("0"); break;
   }
   switch(tk){
   	case tk_Normal: origapcode.assign("0"); break;
   	case tk_Odd:    origapcode.assign("2"); break;
   	case tk_Fix:    origapcode.assign("7"); break;
   	case tk_Auct:   origapcode.assign(mkt==Kway::Tw::Stk::m_Tse?"5":"x"); break;
   	case tk_Lend:   origapcode.assign("4"); break;
   	case tk_Tend:   origapcode.assign(mkt==Kway::Tw::Stk::m_Tse?"6":"8"); break;
      default:        origapcode.assign("x"); break;
   }  
   TaViewArea VA = GetStatusVACreater().CreateNewVA();
   if(OrdSesRegs_.size())
      for(int pos = OrdSesRegs_.size()-1;pos >= 0; pos--){
         Ses = static_cast<TbxSes*>(OrdSesRegs_.at(pos));
         otk = GetSesTK(Ses);
         if(Ses->GetMarket() == mkt && Ses->GetApCode() == origapcode/* && otk == tk && IsAutoStart(tk)*/){
            VA->MoveCurRow(0, vcmfEnd);
            TbxSes* bsEnd = static_cast<TbxSes*>(GetCurrentSes(VA.get()));
            while(bsEnd == 0){
               VA->MoveCurRow(-1, vcmfCurrent);
               bsEnd = static_cast<TbxSes*>(GetCurrentSes(VA.get()));
            }
            VA->MoveCurRow(0, vcmfBegin);
            TbxSes* bs = static_cast<TbxSes*>(GetCurrentSes(VA.get()));
            while(bs != bsEnd && Ses != bs){
               VA->MoveCurRow(1, vcmfCurrent);
               bs = static_cast<TbxSes*>(GetCurrentSes(VA.get()));
            }
            if(bs != Ses)
               return;
            if(MarketPh_ == ts_Fix && Ses->IsFix() == false) // 判斷零股及盤後時段重疊時的處理方式
               apcode.assign("2");
            switch(mkt){
               case m_Tse: SesStr_ = std::string("上市普通委託"); break;
               case m_Otc: SesStr_ = std::string("上櫃普通委託"); break;
               default   : return; 
            }
            NewSes = static_cast<TbxSes*>(new TTseOrdSes(this, mkt));
            NewSes->SetMes(Ses->GetMes());
            NewSes->SetEvHandler(Ses->GetbxSesEvHandler());
            NewSes->SettingApply(Ses->GetSetting());
            Ses->SetEvHandler((TSesEvHandler*)0);
            TViewCursor* cur = VA->GetCurRow()->Clone();
            if(!cur->IsInvalid()){
               cur->PutStr(1, SesStr_, VA.get());
            }
            delete cur;
            delete Ses;
            Ses = NewSes;
            Ses->SetApCode(apcode);
            Ses->Open();
            ((TbxLinkSes*)Ses)->ReStartLink();
         }
      }
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::ResetScheduler ()
{
   Scheduler_ = (TSchedulerBase*)0;
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::SetMarketClose(TbxSes* SesPtr)
{
   ETradeKind tk = GetSesTK(SesPtr);
   MarketFlag_[SesPtr->GetMarket()==m_Tse?0:1][tk] = true;
   TReqPtr ReqPtr; 
   while((ReqPtr = Scheduler_->GetOrderByMkt(SesPtr->GetMarket(), tk)).get() != 0){
      TStkOrder&  StkOrder = ReqPtr->GetOrdHnd().GetModify();
      StkOrder.LastTseCode_ = 1;
      StkOrder.LastOCmd_ = ReqPtr->GetOrdCmdID();
      char buf[16];
      sprintf(buf,"%02d%02d",GetSubSystemTK(SesPtr->GetMarket(),tk), StkOrder.LastTseCode_);
      ReqPtr->Update(std::string(GetErrMsg(buf)), ReqPtr->GetOrdCmdID()==cid_New?of_AbortOrdClear:of_AbortOrdNoChange); 
   }
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::SetBrkSeq(TBrokerID brk, TMarket mkt, Kway::dword lastnum)
{
	TBrkMktSeq::iterator iter = BrkMktSeq_.fetch(TBrkMkt(brk, mkt));
	if(iter->second < lastnum){
	   TBrkMktSeq::updater u = BrkMktSeq_.begin_update(iter);
	   *u = lastnum;
	}
}
//---------------------------------------------------------------------------
K_mf(Kway::dword) TbxSesMgr::GetBrkSeq(TBrokerID brk, TMarket mkt)
{
	TBrkMktSeq::iterator iter = BrkMktSeq_.find(TBrkMkt(brk, mkt));
	if(iter == BrkMktSeq_.end()){
	   iter = BrkMktSeq_.fetch(TBrkMkt(brk, mkt));
	   TBrkMktSeq::updater u = BrkMktSeq_.begin_update(iter);
	   *u = 0;
	   return 1;
	}
	return iter->second + 1;
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::LoadMainConf()
{
   TKIniFile DRIniFile(std::string(GetConfPath()+"/DR.ini").c_str());
   std::string tmp = DRIniFile.ReadString("Conf","DR","N");
   std::string CommParam,SesParam;
   if(tmp[0]=='Y' || tmp[0]=='y'){
      CommParam = GetLogPath()+GetProcName()+".DR;,102400*10";
      AddSetting(std::string(GetProcMkt()==m_Tse?"上市委託讀檔":"上櫃委託讀檔"),std::string(),
                 std::string("File"), CommParam);
   }
   TKIniFile IniFile(std::string(GetConfPath()+"/Main.ini").c_str());
   std::string ProcName = GetProcName();
   DebugLog_   = IniFile.ReadInt("Main","DebugLog",1);
   FailCount_  = IniFile.ReadInt("Main","TMPFailCount",3);
   LineErrCnt_ = IniFile.ReadInt("Main","LineErrorCount",20);
   OrdConfirmTime_ = IniFile.ReadInt("Main","OrdConfirmTime",30);    // 20100629委託傳送中斷線[多久以內]重連狀態確認後須重送
   MaxTimeGap_ = IniFile.ReadInt("Main","MaxTimeGap",0);             // 20100720檢核委託SendingTime與送單時的時間差是否合理, 0 = 不檢查
   LineChgInterval_    = IniFile.ReadInt("Main","LineChgInterval",10); // 20100721 線路自動切換等待時間(秒)
   SendOrderInterval_  =IniFile.ReadInt("Main","SendOrderInterval",10);// 20100721 取得線路使用權後, 可連續使用時間(秒)
   int retryinterval   = IniFile.ReadInt("Main","SocketRetryInterval",0); // 20100720 Socket Retry Interval 由Main.ini 設定,0: 使用預設值
   tmp= IniFile.ReadString("Main","EnableAutoSwitch","N");
   EnableAutoSwitch_   = (tmp[0]=='Y' || tmp[0]=='y');
   std::string tseaddr = IniFile.ReadString("Main","TseOrdAddr","10.12.3.22:1027");
   std::string otcaddr = IniFile.ReadString("Main","OtcOrdAddr","10.12.3.32:1029");
   std::string tsebindaddr = IniFile.ReadString("Main","TseSrcAddr","");
   std::string otcbindaddr = IniFile.ReadString("Main","OtcSrcAddr","");
   std::string commdev = IniFile.ReadString("Main","CommDev","Ev-TSEClient");
   std::string comext;
   if(retryinterval > 0){
   	char extbuf[16];
   	sprintf(extbuf,"M0R%d",retryinterval);
   	comext = std::string(extbuf);
   }else
   	comext = std::string("M0");
   bool istse = (GetProcMkt()==m_Tse);
   tmp = (istse?"T":"O")+GetProcBrkID().as_string();
   TStrings strs;
   int cnt = IniFile.ReadStrings(ProcName,"TMPID",strs);
   for(int L = 0; L < cnt; L++){
      std::string activestr = IniFile.ReadString(ProcName+"-"+strs[L],"Active","Y");
      if (activestr[0] != 'Y' && activestr[0] != 'y')
         continue;
      std::string srcport  = IniFile.ReadString(ProcName+"-"+strs[L],"SourcePort","");
      std::string passwd = IniFile.ReadString(ProcName+"-"+strs[L],"Password","");
      std::string termid = IniFile.ReadString(ProcName+"-"+strs[L],"TermID","");
      TBrkMktSesID brkmktid(TBrkID(GetProcBrkID().as_string()),GetProcMkt(),TPvcID(strs[L]));
      SesTermID_.insert(make_pair(brkmktid, TTermID(termid)));
      if(istse)
         CommParam = tseaddr+","+(tsebindaddr.empty()?(srcport+comext):(comext+" B"+tsebindaddr+":"+srcport));
      else
         CommParam = otcaddr+","+(otcbindaddr.empty()?(srcport+comext):(comext+" B"+otcbindaddr+":"+srcport));
      AddSetting(std::string(istse?"上市普通委託":"上櫃普通委託"),
                 GetProcBrkID().as_string()+","+passwd+","+strs[L],commdev, CommParam);
   }
   cnt = IniFile.ReadStrings(tmp,"FixSes",strs);
   if(cnt > 0){
      if(ProcName==strs[0]){ // 如果是第一個連線, 則必須將成回的參數讀入並建立成回連線
         std::string activestr = IniFile.ReadString(tmp,"Active","Y");
         if (activestr[0] == 'Y' || activestr[0] == 'y'){
            std::string apcode = IniFile.ReadString(tmp,"MatApCode","A");
            std::string termid = IniFile.ReadString(tmp,"TermID","");
            std::string pvcid  = IniFile.ReadString(tmp,"MatSesID","");
            std::string passwd = IniFile.ReadString(tmp,"MatPassword","");
            if(passwd != std::string()){
               TBrkMktSesID brkmktid(TBrkID(GetProcBrkID().as_string()),GetProcMkt(),TPvcID(pvcid));
               SesTermID_.insert(make_pair(brkmktid, TTermID(termid)));
               SesParam  = GetProcBrkID().as_string()+","+passwd+","+
                           pvcid+(apcode=="3"?",3":",A");
               CommParam = IniFile.ReadString("Main",istse?"TseMatAddr":"OtcMatAddr","")+","+
                           (istse?(tsebindaddr.empty()?"":std::string(comext+" B"+tsebindaddr+":")):
                                  (otcbindaddr.empty()?"":std::string(comext+" B"+otcbindaddr+":")))+
                           IniFile.ReadString(tmp,"MatSrcPort","")+comext;
               AddSetting(std::string(istse?"上市成交回報":"上櫃成交回報"), SesParam,
                          commdev, CommParam);
            }
         }
      }else{                 // 否則必須建立成回讀檔的session , 讀檔位置在第一個連線的Log裏
         CommParam = GetLogPath()+"/"+strs[0]+"/PMach."+(istse?"T":"O")+";,204800*10";
         AddSetting(std::string("成交讀檔"),std::string(),
                    std::string("File"), CommParam);
      }
   } // DR 讀檔
   CommParam = GetLogPath()+"/K09";
   AddSetting(std::string("錯誤次數重置"),std::string(),std::string("File"), CommParam);
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::LoadTesterConf()
{
   TKIniFile IniFile(std::string(GetConfPath()+"/XTester.ini").c_str());
   TStrings strs;
   DebugLog_   = false;
   std::string tseaddr = IniFile.ReadString("Summary","TseOrdAddr","10.12.3.22:1027");
   std::string otcaddr = IniFile.ReadString("Summary","OtcOrdAddr","10.12.3.32:1029");
   std::string commdev = IniFile.ReadString("Summary","CommDev","Ev-TSEClient");
   std::string CommParam,SesParam,BrkID,PvcID;
   std::string tsebindaddr = IniFile.ReadString("Summary","TseSrcAddr","");
   std::string otcbindaddr = IniFile.ReadString("Summary","OtcSrcAddr","");
   int cnt = IniFile.ReadStrings("Summary","SesID",strs);
   for(int L = 0; L < cnt; L++){
      std::string tmp = IniFile.ReadString(strs[L],"Type","");
      if(tmp[0]=='K'){
         std::string srcport = IniFile.ReadString(strs[L],"SourcePort","");
         std::string passwd  = IniFile.ReadString(strs[L],"Password","");
         std::string termid  = IniFile.ReadString(strs[L],"TermID","");
         BrkID = strs[L].substr(1,4);
         PvcID = strs[L].substr(5,2);
         TBrkMktSesID brkmktid(TBrkID(BrkID),GetProcMkt(),TPvcID(PvcID));
         SesTermID_.insert(make_pair(brkmktid, TTermID(termid)));
         bool istse = (strs[L][0]=='T');
         AddSetting(std::string(istse?"上市線路測試":"上櫃線路測試"),
                    BrkID+","+passwd+","+PvcID+","+IniFile.ReadString(strs[L],"APCode","0"),
                    commdev,(istse?tseaddr:otcaddr)+","+
                    (istse?(tsebindaddr.empty()?"":std::string("B"+tsebindaddr+":")):
                    (otcbindaddr.empty()?"":std::string("B"+otcbindaddr+":")))+srcport);
      }
   }
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::DelSetting(TMarket mkt,TBrkID brk,TPvcID pvcid)
{
   std::string tmp = mkt.as_string()+brk.as_string()+pvcid.as_string();
   TKIniFile IniFile(std::string(GetConfPath()+"/XTester.ini").c_str());
   IniFile.EraseValue("Summary","SesID",tmp);
   IniFile.EraseSection(tmp);
}
//---------------------------------------------------------------------------
K_mf(bool) TbxSesMgr::IncreaseErrCnt(int subsys)
{
	TTrxErrCnt::iterator iter = TrxErrCnt_.find(subsys);
   std::string procname = "XCtrl";
   std::string objname  = "LineErrCntMgr";
   char buf[32];
   TMarket mkt;
	ETradeKind tk = GetMktTKBySub(subsys, mkt);
   int lines = (mkt==m_Tse?SegDefs_.Tse_:SegDefs_.Otc_).Seg_[tk].TargetL_;
   if(lines <= 0)
      lines = 1;
	if(iter == TrxErrCnt_.end()){
	   iter = TrxErrCnt_.fetch(subsys);
	   TTrxErrCnt::updater u = TrxErrCnt_.begin_update(iter);
	   *u = 1;
      sprintf(buf,"%4s%7s%02d%04d%04d",GetProcBrkID().as_string().c_str(),GetProcName().c_str(),
                                   subsys, *u, (lines*LineErrCnt_));
	   GetExcMgr()->CmdReq(procname,objname,1,std::string(buf));
	   return true;
	}
   TTrxErrCnt::updater u = TrxErrCnt_.begin_update(iter);
	(*u) = (*u)+1;
   sprintf(buf,"%4s%7s%02d%04d%04d",GetProcBrkID().as_string().c_str(),GetProcName().c_str(),
                                   subsys, *u, (lines*LineErrCnt_));
	GetExcMgr()->CmdReq(procname,objname,1,std::string(buf));
	if((*u) >= (lines*LineErrCnt_))
	   ExceedErrCnt_[mkt==m_Tse?0:1][tk] = true;
	else
	   ExceedErrCnt_[mkt==m_Tse?0:1][tk] = false;
	return true;
}
//---------------------------------------------------------------------------
K_mf(bool) TbxSesMgr::ResetErrCnt(int subsys, Kway::word count)
{
   std::string procname = "XCtrl";
   std::string objname  = "LineErrCntMgr";
   char buf[32];
   TTrxErrCnt::iterator iter = TrxErrCnt_.fetch(subsys);
   TTrxErrCnt::updater u = TrxErrCnt_.begin_update(iter);
	*u = count;
	TMarket mkt;
	ETradeKind tk = GetMktTKBySub(subsys, mkt);
   int lines = (mkt==m_Tse?SegDefs_.Tse_:SegDefs_.Otc_).Seg_[tk].TargetL_;
   if(lines <= 0)
      lines = 1;
   sprintf(buf,"%4s%7s%02d%04d%04d",GetProcBrkID().as_string().c_str(),GetProcName().c_str(),
                                   subsys, *u, (lines*LineErrCnt_));
	GetExcMgr()->CmdReq(procname,objname,1,std::string(buf));
	if((*u) >= (lines*LineErrCnt_))
	   ExceedErrCnt_[mkt==m_Tse?0:1][tk] = true;
	else
	   ExceedErrCnt_[mkt==m_Tse?0:1][tk] = false;
	return true;
}
//---------------------------------------------------------------------------
K_mf(Kway::byte) TbxSesMgr::GetDebugLevel()
{
   return DebugLog_;
}
//---------------------------------------------------------------------------
K_mf(bool) TbxSesMgr::SetMsgCnt(int cnt)
{
   //務必記得在initial 時必須先做GetMsgCnt 才會create recorder;
   TMsgCnt::iterator iter = MsgCnt_.fetch(0);
   TMsgCnt::updater  u    = MsgCnt_.begin_update(iter);
   (*u) = cnt;
   return true;
}
//---------------------------------------------------------------------------
K_mf(int) TbxSesMgr::GetMsgCnt()
{
   if(MsgCntRec_ == 0)
      MsgCntRec_ = new TMsgCntRec(MsgCnt_, std::string(GetProcLogPath()+"RstMsgCnt.rec").c_str());
   TMsgCnt::iterator iter = MsgCnt_.find(0);
   if(iter == MsgCnt_.end()){
      iter = MsgCnt_.fetch(0);
      TMsgCnt::updater u = MsgCnt_.begin_update(iter);
      (*u) = 0;
      return 0;
   }
   return iter->second;
}
//---------------------------------------------------------------------------
K_mf(bool) TbxSesMgr::DoCommand(Kway::word cmdid, std::string msg)
{
   switch(cmdid){
      case 1:
         ReloadConf();
         break;
      case 2:{
         struct{
         	char_ary<char,4,' '> brokerid;
         	char_ary<char,7,' '> sessionid;
         	char_ary<char,2,' '> subsys;
         	char_ary<char,4,' '> count;
         	char_ary<char,4,' '> limit;
         } ErrCnt_;
         memcpy(&ErrCnt_, msg.c_str(), sizeof(ErrCnt_));
         ResetErrCnt(ErrCnt_.subsys.as_int(), ErrCnt_.count.as_int());
         break;
      }
      default:
         break;
   }
}
//---------------------------------------------------------------------------
K_mf(void) TbxSesMgr::ReloadConf()
{
   TbxDatas bxDatas_;
   char*    Pos;
   char*    StartPos;
   TaViewArea VA = GetStatusVACreater().CreateNewVA();
   VA->MoveCurRow(0, vcmfBegin);
   TbxSes* Ses = dynamic_cast<TbxSes*>(GetCurrentSes(VA.get()));
   TbxData bxd;
   while(Ses != 0){
      if(Ses->GetPvcID().as_string().length() < 2){
         VA->MoveCurRow(1, vcmfCurrent);
         Ses = dynamic_cast<TbxSes*>(GetCurrentSes(VA.get()));
         continue;
      }
      bxd.Active_ = (Ses->GetMes()->GetLinkState() != mls_Closed);
      bxd.Passwd_ = Ses->GetPassWord();
      std::string MesStr = Ses->GetMes()->GetSetting();
      StartPos = (char*)MesStr.c_str();
      if((Pos = strchr(StartPos, 'N')) == 0){
         if((Pos = strchr(StartPos, ',')) == 0)
            bxd.SrcPort_ = 0;
         else{
            bxd.SrcPort_ = atoi(Pos+1);
            StartPos = Pos + 1;
            if((Pos = strchr(StartPos, 'B')) == 0){
            }else{
               if((Pos = strchr(StartPos, ':')) == 0)
                  bxd.SrcPort_ = 0;
               else
                  bxd.SrcPort_ = atoi(Pos+1);
            }
         }
      }else
         bxd.SrcPort_ = atoi(Pos+1);
      bxd.Ses_ = Ses;
      bxDatas_.insert(std::make_pair(Ses->GetPvcID(), bxd));
      VA->MoveCurRow(1, vcmfCurrent);
      Ses = dynamic_cast<TbxSes*>(GetCurrentSes(VA.get()));
   }
   VA->MoveCurRow(-1, vcmfEnd);
   Ses = dynamic_cast<TbxSes*>(GetCurrentSes(VA.get()));
   while(Ses != 0){
      if(Ses->GetPvcID().as_string().length() < 2){
         VA->MoveCurRow(-1, vcmfCurrent);
         Ses = dynamic_cast<TbxSes*>(GetCurrentSes(VA.get()));
         continue;
      }
      bxd.Active_ = (Ses->GetMes()->GetLinkState() != mls_Closed);
      bxd.Passwd_ = Ses->GetPassWord();
      std::string MesStr = Ses->GetMes()->GetSetting();
      StartPos = (char*)MesStr.c_str();
      if((Pos = strchr(StartPos, 'N')) == 0){
         if((Pos = strchr(StartPos, ',')) == 0)
            bxd.SrcPort_ = 0;
         else{
            bxd.SrcPort_ = atoi(Pos+1);
            StartPos = Pos + 1;
            if((Pos = strchr(StartPos, 'B')) == 0){
            }else{
               if((Pos = strchr(StartPos, ':')) == 0)
                  bxd.SrcPort_ = 0;
               else
                  bxd.SrcPort_ = atoi(Pos+1);
            }
         }
      }else
         bxd.SrcPort_ = atoi(Pos+1);
      bxd.Ses_ = Ses;
      bxDatas_.insert(std::make_pair(Ses->GetPvcID(), bxd));
      VA->MoveCurRow(-1, vcmfCurrent);
      Ses = dynamic_cast<TbxSes*>(GetCurrentSes(VA.get()));
   }
   TKIniFile IniFile(std::string(GetConfPath()+"/Main.ini").c_str());
   std::string tseaddr = IniFile.ReadString("Main","TseOrdAddr","10.12.3.22:1027");
   std::string otcaddr = IniFile.ReadString("Main","OtcOrdAddr","10.12.3.32:1029");
   std::string commdev = IniFile.ReadString("Main","CommDev","Ev-TSEClient");
   std::string tsebindaddr = IniFile.ReadString("Main","TseSrcAddr","");
   std::string otcbindaddr = IniFile.ReadString("Main","OtcSrcAddr","");
   std::string ProcName = GetProcName();
   int retryinterval = IniFile.ReadInt("Main","SocketRetryInterval",0); // 20100720 Socket Retry Interval 由Main.ini 設定,0: 使用預設值
   std::string comext;
   if(retryinterval > 0){
   	char extbuf[16];
   	sprintf(extbuf,"M0R%d",retryinterval);
   	comext = std::string(extbuf);
   }else
   	comext = std::string("M0");
   TStrings strs;
   bool istse = (GetProcMkt()==m_Tse);
   std::string tmp = (istse?"T":"O")+GetProcBrkID().as_string();
   int cnt = IniFile.ReadStrings(ProcName,"TMPID",strs);
   for(int L = 0; L < cnt; L++){
      std::string activestr = (IniFile.ReadString(ProcName+"-"+strs[L],"Active","Y"));
      bool active = (activestr[0] == 'Y' || activestr[0] == 'y');
      char_ary<char,6,' '> srcport(IniFile.ReadString(ProcName+"-"+strs[L],"SourcePort",""));
      char_ary<char,6,' '> passwd(IniFile.ReadString(ProcName+"-"+strs[L],"Password",""));
      TbxDatas::iterator iter = bxDatas_.find(TPvcID(strs[L]));
      if(iter == bxDatas_.end()){
         if(!active)
            continue;
         std::string termid = IniFile.ReadString(ProcName+"-"+strs[L],"TermID","");
         TBrkMktSesID brkmktid(TBrkID(GetProcBrkID().as_string()),GetProcMkt(),TPvcID(strs[L]));
         SesTermID_.insert(make_pair(brkmktid, TTermID(termid)));
         std::string CommParam;
         if(istse)
            CommParam = tseaddr+","+(tsebindaddr.empty()?(srcport.as_string()+comext):(comext+" B"+tsebindaddr+":"+srcport.as_string()));
         else
            CommParam = otcaddr+","+(otcbindaddr.empty()?(srcport.as_string()+comext):(comext+" B"+otcbindaddr+":"+srcport.as_string()));
         AddSetting(std::string(istse?"上市普通委託":"上櫃普通委託"),
                    GetProcBrkID().as_string()+","+passwd.as_string()+","+strs[L],commdev, CommParam);
         continue;
      }else{
         if(passwd.as_int() == iter->second.Passwd_ && srcport.as_int() == iter->second.SrcPort_ && 
            active == iter->second.Active_)
            continue;
         iter->second.Ses_->Close();
         if(!active)
            continue;
         if(passwd.as_int() != iter->second.Passwd_){
            iter->second.Ses_->SetPassWord(passwd.as_int());
            tmp = GetProcBrkID().as_string()+","+passwd.as_string()+","+strs[L];
            iter->second.Ses_->SettingApply(tmp);
         }
         if(srcport.as_int() != iter->second.SrcPort_){
            std::string MesStr = iter->second.Ses_->GetMes()->GetSetting();
            StartPos = (char*)MesStr.c_str();
            if((Pos = strchr(StartPos, 'N')) == 0){
               if((Pos = strchr(StartPos, ',')) != 0){
                  StartPos = Pos;
                  if((Pos = strchr(StartPos, 'B')) != 0){
                     if((Pos = strchr(StartPos, ':')) != 0)
                        *Pos = 0;
                     MesStr = std::string(MesStr.c_str());
                     MesStr += ":";
                     }else{
                        *StartPos = 0;
                        MesStr = std::string(MesStr.c_str());
                        MesStr += ",";
                     }
               }else
                  MesStr += ",";
               MesStr += srcport.as_string();
            }else{
               *Pos = 0;
               MesStr = std::string(MesStr.c_str());
               MesStr += "N"+srcport.as_string();
            }
            iter->second.Ses_->GetMes()->SettingApply(MesStr);
         }
         iter->second.Ses_->Open();
         continue;
      }
   }
   for(TbxDatas::iterator iter = bxDatas_.begin(); iter != bxDatas_.end(); iter++){
      char apcode = iter->second.Ses_->GetApCode()[0];
      if(apcode == '3' || apcode == 'A')
         continue;
      std::string pvcid = iter->second.Ses_->GetPvcID().as_string();
      bool found = false;
      for(int L = 0; L < cnt; L++){
         if(strs[L] == pvcid){
            found = true;
            break;
         }
      }
      if(found)
         continue;
      iter->second.Ses_->Close();
      delete iter->second.Ses_;
   }
   TSegDefs sesdef = SegDefs_;
   SetMarketPh(&sesdef);
   tmp = (istse?"T":"O")+GetProcBrkID().as_string();
   cnt = IniFile.ReadStrings(tmp,"FixSes",strs);
   if(cnt > 0){
      if(ProcName==strs[0]){ // 如果是第一個連線, 則必須將成回的參數讀入並建立成回連線
         tseaddr = IniFile.ReadString("Main","TseMatAddr","10.12.3.22:1027");
         otcaddr = IniFile.ReadString("Main","OtcMatAddr","10.12.3.32:1029");
         TPvcID pvcid(IniFile.ReadString(tmp,"MatSesID",""));
         if(pvcid.as_string().length() != 2)
            return;
         std::string apcode = IniFile.ReadString(tmp,"MatApCode","A");
         TbxDatas::iterator iter = bxDatas_.find(pvcid);
         std::string activestr = (IniFile.ReadString(tmp,"Active","Y"));
         bool active = (activestr[0] == 'Y' || activestr[0] == 'y');
         char_ary<char,6,' '> srcport(IniFile.ReadString(tmp,"MatSrcPort",""));
         char_ary<char,6,' '> passwd(IniFile.ReadString(tmp,"MatPassword",""));
         if(iter == bxDatas_.end()){
            if(!active || passwd.empty())
               return;
            std::string termid = IniFile.ReadString(tmp,"TermID","");
            TBrkMktSesID brkmktid(TBrkID(GetProcBrkID().as_string()),GetProcMkt(),pvcid);
            SesTermID_.insert(make_pair(brkmktid, TTermID(termid)));
            std::string SesParam  = GetProcBrkID().as_string()+","+passwd.as_string()+","+
                        IniFile.ReadString(tmp,"MatSesID","")+(apcode=="3"?",3":",A");
            std::string CommParam;
            if(istse)
               CommParam = tseaddr+","+(tsebindaddr.empty()?srcport.as_string():(comext+" B"+tsebindaddr+":"+srcport.as_string()+comext));
            else
               CommParam = otcaddr+","+(otcbindaddr.empty()?srcport.as_string():(comext+" B"+otcbindaddr+":"+srcport.as_string()+comext));
            AddSetting(std::string(istse?"上市成交回報":"上櫃成交回報"), SesParam,commdev, CommParam);
         }else{
            if(passwd.as_int() == iter->second.Passwd_ && srcport.as_int() == iter->second.SrcPort_ && 
               active == iter->second.Active_)
               return;
            iter->second.Ses_->Close();
            if(!active)
               return;
            if(passwd.as_int() != iter->second.Passwd_){
               iter->second.Ses_->SetPassWord(passwd.as_int());
               tmp = GetProcBrkID().as_string()+","+passwd.as_string()+","+pvcid.as_string()+",A";
               iter->second.Ses_->SettingApply(tmp);
            }
            if(srcport.as_int() != iter->second.SrcPort_){
               std::string MesStr = iter->second.Ses_->GetMes()->GetSetting();
               StartPos = (char*)MesStr.c_str();
               if((Pos = strchr(StartPos, 'N')) == 0){
                  if((Pos = strchr(StartPos, ',')) != 0){
                     StartPos = Pos;
                     if((Pos = strchr(StartPos, 'B')) != 0){
                        if((Pos = strchr(StartPos, ':')) != 0)
                           *Pos = 0;
                        MesStr = std::string(MesStr.c_str());
                        MesStr += ":";
                     }else{
                        *StartPos = 0;
                        MesStr = std::string(MesStr.c_str());
                        MesStr += ",";
                     }
                  }else
                     MesStr += ",";
                  MesStr += srcport.as_string();
               }else{
                  *Pos = 0;
                  MesStr = std::string(MesStr.c_str());
                  MesStr += "N"+srcport.as_string();
               }
               iter->second.Ses_->GetMes()->SettingApply(MesStr);
            }
            iter->second.Ses_->Open();
         }
      }
   } 
}
//---------------------------------------------------------------------------
K_mf(std::string) TbxSesMgr::GetTermID(TBrkID brkid, TMarket mkt, TPvcID pvcid)
{
   TBrkMktSesID brkseskey(brkid, mkt, pvcid);
   TSesTermID::iterator iter = SesTermID_.find(brkseskey);
   if(iter != SesTermID_.end())
      return iter->second.as_string();
   return std::string();
}
//---------------------------------------------------------------------------


