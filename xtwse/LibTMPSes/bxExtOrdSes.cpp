#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxExtOrdSes.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw;
using namespace Kway::Tw::bx;
using namespace Kway::Tw::Stk;
//---------------------------------------------------------------------------
static char Msg_PVCBroken[]= "0403";   //委託中斷線
static char Msg_Timeout[]  = "0404";   //委託逾時
/////////////////////////////////////////////////////////////////////////////
K_ctor  TbxAuctSes::TbxAuctSes  (TbxSesMgr* SesMgr,Stk::TMarket mkt)
   :inherited(SesMgr)
{
   SetOrderTimeOut(90*1000);         //交易子系統之TimeOut 時間為90秒
   if(MgrPtr_ != 0){
      Recorder = MgrPtr_->GetRecorder();
      MgrPtr_->RegbxSes(this);

   }
   
   SetMarket(mkt);
   if(mkt == Stk::m_Tse)
      SubSys_.assign("70");
   else
      SubSys_.assign("93");
   SetApCode(TApCode(std::string("5")));
}
//---------------------------------------------------------------------------
K_dtor TbxAuctSes::~TbxAuctSes  ()
{
   DelSubSysReg();
   if(MgrPtr_ != 0)
      MgrPtr_->DelbxSes(this);
   SetState(sp_Destroying, "Destroying");
}
//---------------------------------------------------------------------------
K_mf(bool)  TbxAuctSes::SettingApply  (const std::string& setting)
{
   if(inherited::SettingApply(setting))
      SetApCode(TApCode(std::string("5")));
   else
      return false;
   return true;
}
//---------------------------------------------------------------------------
K_mf(void) TbxAuctSes::OnSubSysReady()
{
   StartTimer(OrderTimeOut_);
   SesMsg_.MP_.assign("拍賣");
   AddWorkers();
   if(MgrPtr_ != 0)                     //應用子系統連線成功,需先向Mgr註冊
        MgrPtr_->RegOrderSes(this);
   if(GetExcMgr())
      GetExcMgr()->Alert("XX1201",ID_.as_string());
   OnSesFree();
}
//---------------------------------------------------------------------------
K_mf(void)    TbxAuctSes::DelSubSysReg()
{
   if(ReqPtr_.get()!=0){
      if(ReqPtr_->GetOrdCmdID() == cid_New){
         const TStkOrder* Ord =  (ReqPtr_->GetOrdHnd().GetConst());
         TTseControlHead T030;
         T030.SubSysName_ = GetSubSys();
         T030.FunctionCode_.assign("01");
         T030.MessageType_.assign("03");
         T030.MessageTime_.assign(TKTime::Now().as_string(false));
         T030.StatusCode_.assign("00");
         Recorder->WriteBSOK(&T030,(TStkOrder&)*Ord,BrokerID_,PvcID_,true);
      }
      ReqPtr_->Update(std::string(GetErrMsg(Msg_PVCBroken)),of_Sent);
      ReqPtr_.reset();
   }
   if(MgrPtr_ != 0)
      MgrPtr_->DelOrderSes(this);
}
//---------------------------------------------------------------------------
K_mf(void)    TbxAuctSes::OnSesFree()
{
   ShowState(sp_WaitForOrder);
   if(MgrPtr_ != 0)
        MgrPtr_->OnSesFree(this,Market_);
   StartTimer(OrderTimeOut_);
}
//---------------------------------------------------------------------------
K_mf(void)    TbxAuctSes::ShowState(EState es)
{
   switch(es){
      case sp_WaitForOrder:
         SesMsg_.msg_.assign("Ready!");
         break;
      case sp_WaitForOrdReply:
         SesMsg_.msg_.assign("Sending!");
         break;
      case sp_Wait050:
         SesMsg_.msg_.assign("Waiting 050!");
         break;
      case sp_Error:
         SesMsg_.msg_.assign("Error!");
         break;
      default:return;
   }
   SetState(es, SesMsg_.as_string());
}
//---------------------------------------------------------------------------
K_mf(void)    TbxAuctSes::OnRequest(TReqPtr ReqPtr)
{
   TStkOrder& StkOrder = ReqPtr->GetOrdHnd().GetModify();

   StopTimer();
   TKey key;
   TWorker* worker;
   ReqPtr_ = ReqPtr;            //收到request 自己保留一份
   //↓先設定狀態以免新委託再進來
   ReqPtr_->Update("委託傳送中",of_S_ing);
   ShowState(sp_WaitForOrdReply);
   key.assign(SubSys_.as_string()+"01");
   worker = Workers_.GetWorker(key);
   worker->SendReq(static_cast<TbxSes&>(*this),pc_A010,ReqPtr_);          //送出A010
   StartTimer(OrderTimeOut_);
}
//---------------------------------------------------------------------------
K_mf(void)    TbxAuctSes::OnTimer           (TTimerBase*)
{
   StopTimer   ();
   switch(State_)
   {
        case sp_WaitLinkReady:
                GetMgr()->GetRecorder()->WriteLog(ID_, "Link Time out, restart session!");
                DelSubSysReg();
                ReStartLink();
                break;
        case sp_WaitLoginReady:
                GetMgr()->GetRecorder()->WriteLog(ID_, "Link Time out, restart session!");
                DelSubSysReg();
                ReStartLink();
                break;
        case sp_WaitSubSysReady:
                GetMgr()->GetRecorder()->WriteLog(ID_, "Link Time out, restart session!");
                DelSubSysReg();
                ReStartLink();
                break;
        case sp_WaitForOrder: {          //Heart beat
                TKey key;
                TWorker* worker;
                key.assign(SubSys_.as_string()+"00");
                worker = Workers_.GetWorker(key);
                worker->SendReq(*this,pc_A040,ReqPtr_);
                ShowState(sp_Wait050);
                StartTimer(OrderTimeOut_);
                break;  }
        case sp_WaitForOrdReply:        //timeout 沒收到委託回報
                if(ReqPtr_->GetOrdCmdID() == cid_New){
                   const TStkOrder* Ord =  (ReqPtr_->GetOrdHnd().GetConst());
                   TTseControlHead T030;
                   T030.SubSysName_ = GetSubSys();
                   T030.FunctionCode_.assign("01");
                   T030.MessageType_.assign("03"); 
                   T030.MessageTime_.assign(TKTime::Now().as_string(false));
                   T030.StatusCode_.assign("00");
                   Recorder->WriteBSOK(&T030,(TStkOrder&)*Ord,BrokerID_,PvcID_,true);
                }
                ReqPtr_->Update(std::string(GetErrMsg(Msg_Timeout)),of_AbortOrdUnknow);
                ReqPtr_.reset();
                GetMgr()->GetRecorder()->WriteLog(ID_, "Order Time out, restart session!");
                DelSubSysReg();
                ReStartLink();
                break;
        case sp_Wait050:
                GetMgr()->GetRecorder()->WriteLog(ID_, "040 time out, restart session!");
                DelSubSysReg();
                ReStartLink();
                break;
        case sp_Error:
                break;
        case sp_Signout:
                break;
        default:
                SetState(sp_Error,"Unknown Error");
                break;
   }
}
//---------------------------------------------------------------------------
K_mf(void)    TbxAuctSes::OnWorkerFinished(TPacketCode PkCode,const THead& pk)
{
   StopTimer   ();
   switch(PkCode){
      case pc_A010:
           DelSubSysReg();
           break;
      case pc_A020:  {         //收到委託回報完成委託
           TStkOrder&  StkOrder = ReqPtr_->GetOrdHnd().GetModify();
           switch (StkOrder.LastOCmd_) {
              case cid_New:
                   ReqPtr_->Update("拍賣委託OK!", of_Finish);
                   break;
              case cid_ChgQty:
                   ReqPtr_->Update("拍賣改量OK!", of_Finish);
                   break;
              case cid_Del:
                   ReqPtr_->Update("拍賣刪單OK!", of_Finish);
                   break;
              case cid_Qry: {
                   ReqPtr_->Update("拍賣查詢OK!", of_Finish);
                   break;
              }
              default:
                   break;
           }
           Recorder->WriteBSOK((TA020*)(&pk),StkOrder);
           ReqPtr_.reset();
           OnSesFree();
           break;
           }
      case pc_A030:  {         //收到錯誤回覆
                //↓填入錯誤訊息,並放棄這筆委託
           if(ReqPtr_.get() == 0)
              return; 
           TStkOrder&  StkOrder = ReqPtr_->GetOrdHnd().GetModify();
           StkOrder.LastTseCode_ = pk.StatusCode_.as_int();
           if(!StkOrder.IsTseOK())
              StkOrder.TseOType_ = StkOrder.OType_;
           switch (pk.FunctionCode_.as_int()) {
              case 1:
              case 2:
                   StkOrder.LastOCmd_ = cid_New;                   //委託
                   break;
              case 3:
                   StkOrder.LastOCmd_ = cid_ChgQty;                //改量
                   break;
              case 4:
                   StkOrder.LastOCmd_ = cid_Del;                   //刪單
                   break;
              case 5:
                   StkOrder.LastOCmd_ = cid_Qry;                   //查詢
                   break;
              default:break;
           }
           if(ReqPtr_.get() != 0){
              if(pk.StatusCode_.as_int() == 1) // Time is over 950209
                 GetMgr()->SetMarketClose(this);   
              ReqPtr_->Update(MgrPtr_->GetMsg(SubSys_.as_string(),pk.StatusCode_.as_string()),
                           StkOrder.LastOCmd_==cid_New?of_AbortOrdClear:of_AbortOrdNoChange);
              ReqPtr_.reset();
           }
           Recorder->WriteBSOK((TTseControlHead*)(&pk),StkOrder,BrokerID_,PvcID_);
           OnSesFree();
           break;
          }
      case pc_A050:           //收到連線確認回覆
           OnSesFree();
           break;
   }
}
//---------------------------------------------------------------------------
K_mf(void)    TbxAuctSes::AddWorkers()
{
   TKey key;
   key.assign(SubSys_.as_string()+"00");
   Workers_.SetWorker(key,&Wrk_Auction_);
   key.assign(SubSys_.as_string()+"01");
   Workers_.SetWorker(key,&Wrk_Auction_);
   key.assign(SubSys_.as_string()+"02");
   Workers_.SetWorker(key,&Wrk_Auction_);
   key.assign(SubSys_.as_string()+"03");
   Workers_.SetWorker(key,&Wrk_Auction_);
   key.assign(SubSys_.as_string()+"04");
   Workers_.SetWorker(key,&Wrk_Auction_);
   key.assign(SubSys_.as_string()+"05");
   Workers_.SetWorker(key,&Wrk_Auction_);
}
//---------------------------------------------------------------------------
//                                   標借作業 
/////////////////////////////////////////////////////////////////////////////
K_ctor  TbxLendSes::TbxLendSes  (TbxSesMgr* SesMgr,Stk::TMarket mkt)
   :inherited(SesMgr)
{
   SetOrderTimeOut(90*1000);        
   if(MgrPtr_ != 0){
      Recorder = MgrPtr_->GetRecorder();
      MgrPtr_->RegbxSes(this);
   }
   SetMarket(mkt);
   if(mkt == Stk::m_Tse)
      SubSys_.assign("31");
   else
      SubSys_.assign("90");
   SetApCode(TApCode(std::string("4")));
}
//---------------------------------------------------------------------------
K_dtor TbxLendSes::~TbxLendSes  ()
{
   DelSubSysReg();
   if(MgrPtr_ != 0)
      MgrPtr_->DelbxSes(this);
   
   SetState(sp_Destroying, "Destroying");
}
//---------------------------------------------------------------------------
K_mf(bool)  TbxLendSes::SettingApply  (const std::string& setting)
{
   if(inherited::SettingApply(setting))
      SetApCode(TApCode(std::string("4")));
   else
      return false;
   return true;
}
//---------------------------------------------------------------------------
K_mf(void) TbxLendSes::OnSubSysReady()
{
   StartTimer(OrderTimeOut_);
   SesMsg_.MP_.assign("標借");
   AddWorkers();
   if(MgrPtr_ != 0)                     //應用子系統連線成功,需先向Mgr註冊
        MgrPtr_->RegOrderSes(this);
   if(GetExcMgr())
      GetExcMgr()->Alert("XX1301",ID_.as_string());
   OnSesFree();
}
//---------------------------------------------------------------------------
K_mf(void)    TbxLendSes::DelSubSysReg()
{
   if(ReqPtr_.get()!=0){
      if(ReqPtr_->GetOrdCmdID() == cid_New){
         const TStkOrder* Ord =  (ReqPtr_->GetOrdHnd().GetConst());
         TTseControlHead T030;
         T030.SubSysName_ = GetSubSys();
         T030.FunctionCode_.assign("01");
         T030.MessageType_.assign("03");
         T030.MessageTime_.assign(TKTime::Now().as_string(false));
         T030.StatusCode_.assign("00");
         Recorder->WriteBSOK(&T030,(TStkOrder&)*Ord,BrokerID_,PvcID_,true);
      }
      ReqPtr_->Update(std::string(GetErrMsg(Msg_PVCBroken)),of_Sent);
      ReqPtr_.reset();
   }
   if(MgrPtr_ != 0)
      MgrPtr_->DelOrderSes(this);
}
//---------------------------------------------------------------------------
K_mf(void)    TbxLendSes::OnSesFree()
{
   ShowState(sp_WaitForOrder);
   if(MgrPtr_ != 0)
        MgrPtr_->OnSesFree(this,Market_);
   StartTimer(OrderTimeOut_);
}
//---------------------------------------------------------------------------
K_mf(void)    TbxLendSes::ShowState(EState es)
{
   switch(es){
      case sp_WaitForOrder:
         SesMsg_.msg_.assign("Ready!");
         break;
      case sp_WaitForOrdReply:
         SesMsg_.msg_.assign("Sending!");
         break;
      case sp_Wait050:
         SesMsg_.msg_.assign("Waiting 050!");
         break;
      case sp_Error:
         SesMsg_.msg_.assign("Error!");
         break;
      default:return;
   }
   SetState(es, SesMsg_.as_string());
}
//---------------------------------------------------------------------------
K_mf(void)    TbxLendSes::OnRequest(TReqPtr ReqPtr)
{
   TStkOrder& StkOrder = ReqPtr->GetOrdHnd().GetModify();

   StopTimer();
   TKey key;
   TWorker* worker;
   ReqPtr_ = ReqPtr;            //收到request 自己保留一份
   //↓先設定狀態以免新委託再進來
   ReqPtr_->Update("委託傳送中",of_S_ing);
   ShowState(sp_WaitForOrdReply);
   key.assign(SubSys_.as_string()+"01");
   worker = Workers_.GetWorker(key);
   worker->SendReq(static_cast<TbxSes&>(*this),pc_V010,ReqPtr_);          //送出A010
   StartTimer(OrderTimeOut_);
}
//---------------------------------------------------------------------------
K_mf(void)    TbxLendSes::OnTimer           (TTimerBase*)
{
   StopTimer   ();
   switch(State_)
   {
        case sp_WaitLinkReady:
                GetMgr()->GetRecorder()->WriteLog(ID_, "Link Time out, restart session!");
                
                DelSubSysReg();
                ReStartLink();
                break;
        case sp_WaitLoginReady:
                GetMgr()->GetRecorder()->WriteLog(ID_, "Link Time out, restart session!");
                
                DelSubSysReg();
                ReStartLink();
                break;
        case sp_WaitSubSysReady:
                GetMgr()->GetRecorder()->WriteLog(ID_, "Link Time out, restart session!");
                
                DelSubSysReg();
                ReStartLink();
                break;
        case sp_WaitForOrder: {          //Heart beat
                TKey key;
                TWorker* worker;
                key.assign(SubSys_.as_string()+"00");
                worker = Workers_.GetWorker(key);
                worker->SendReq(*this,pc_V040,ReqPtr_);
                ShowState(sp_Wait050);
                StartTimer(OrderTimeOut_);
                break;  }
        case sp_WaitForOrdReply:        //timeout 沒收到委託回報
                if(ReqPtr_->GetOrdCmdID() == cid_New){
                   const TStkOrder* Ord =  (ReqPtr_->GetOrdHnd().GetConst());
                   TTseControlHead T030;
                   T030.SubSysName_ = GetSubSys();
                   T030.FunctionCode_.assign("01");
                   T030.MessageType_.assign("03"); 
                   T030.MessageTime_.assign(TKTime::Now().as_string(false));
                   T030.StatusCode_.assign("00");
                   Recorder->WriteBSOK(&T030,(TStkOrder&)*Ord,BrokerID_,PvcID_,true);
                }
                ReqPtr_->Update(std::string(GetErrMsg(Msg_Timeout)),of_AbortOrdUnknow);
                ReqPtr_.reset();
                GetMgr()->GetRecorder()->WriteLog(ID_, "Order Time out, restart session!");
                
                DelSubSysReg();
                ReStartLink();
                break;
        case sp_Wait050:
                GetMgr()->GetRecorder()->WriteLog(ID_, "040 time out, restart session!");
                
                DelSubSysReg();
                ReStartLink();
                break;
        case sp_Error:
                break;
        case sp_Signout:
                break;
        default:
                SetState(sp_Error,"Unknown Error");
                break;
   }
}
//---------------------------------------------------------------------------
K_mf(void)    TbxLendSes::OnWorkerFinished(TPacketCode PkCode,const THead& pk)
{
   StopTimer   ();
   switch(PkCode){
      case pc_V010:
           DelSubSysReg();
           break;
      case pc_V020:  {         //收到委託回報完成委託
           TStkOrder&  StkOrder = ReqPtr_->GetOrdHnd().GetModify();
           switch (StkOrder.LastOCmd_) {
              case cid_New:
                   ReqPtr_->Update("標借委託OK!", of_Finish);
                   break;
              case cid_ChgQty:
                   ReqPtr_->Update("標借改量OK!", of_Finish);
                   break;
              case cid_Del:
                   ReqPtr_->Update("標借刪單OK!", of_Finish);
                   break;
              case cid_Qry: {
                   ReqPtr_->Update("標借查詢OK!", of_Finish);
                   break;
              }
              default:
                   break;
           }
           Recorder->WriteBSOK((TV020*)(&pk),StkOrder);
           ReqPtr_.reset();
           OnSesFree();
           break;
           }
      case pc_V030:  {         //收到錯誤回覆
                //↓填入錯誤訊息,並放棄這筆委託
           if(ReqPtr_.get() == 0)
              return; 
           TStkOrder&  StkOrder = ReqPtr_->GetOrdHnd().GetModify();
           StkOrder.LastTseCode_ = pk.StatusCode_.as_int();
           if(!StkOrder.IsTseOK())
              StkOrder.TseOType_ = StkOrder.OType_;
           switch (pk.FunctionCode_.as_int()) {
              case 1:
              case 2:
                   StkOrder.LastOCmd_ = cid_New;                   //委託
                   break;
              case 3:
                   StkOrder.LastOCmd_ = cid_ChgQty;                //改量
                   break;
              case 4:
                   StkOrder.LastOCmd_ = cid_Del;                   //刪單
                   break;
              case 5:
                   StkOrder.LastOCmd_ = cid_Qry;                   //查詢
                   break;
              default:break;
           }
           if(ReqPtr_.get() != 0){
              if(pk.StatusCode_.as_int() == 1) // Time is over 950209
                 GetMgr()->SetMarketClose(this);   
              ReqPtr_->Update(MgrPtr_->GetMsg(SubSys_.as_string(),pk.StatusCode_.as_string()),
                           StkOrder.LastOCmd_==cid_New?of_AbortOrdClear:of_AbortOrdNoChange);
              ReqPtr_.reset();
           }
           Recorder->WriteBSOK((TTseControlHead*)(&pk),StkOrder,BrokerID_,PvcID_);
           OnSesFree();
           break;
          }
      case pc_V050:           //收到連線確認回覆
           OnSesFree();
           break;
   }
}
//---------------------------------------------------------------------------
K_mf(void)    TbxLendSes::AddWorkers()
{
   TKey key;
   key.assign(SubSys_.as_string()+"00");
   Workers_.SetWorker(key,&Wrk_Lend_);
   key.assign(SubSys_.as_string()+"01");
   Workers_.SetWorker(key,&Wrk_Lend_);
   key.assign(SubSys_.as_string()+"02");
   Workers_.SetWorker(key,&Wrk_Lend_);
   key.assign(SubSys_.as_string()+"03");
   Workers_.SetWorker(key,&Wrk_Lend_);
   key.assign(SubSys_.as_string()+"04");
   Workers_.SetWorker(key,&Wrk_Lend_);
   key.assign(SubSys_.as_string()+"05");
   Workers_.SetWorker(key,&Wrk_Lend_);
}
//---------------------------------------------------------------------------
//                                  標購作業
/////////////////////////////////////////////////////////////////////////////
K_ctor  TbxTendSes::TbxTendSes  (TbxSesMgr* SesMgr,Stk::TMarket mkt)
   :inherited(SesMgr)
{
   SetOrderTimeOut(90*1000);        
   if(MgrPtr_ != 0){
      Recorder = MgrPtr_->GetRecorder();
      MgrPtr_->RegbxSes(this);
   }
   SetMarket(mkt);
   if(mkt == m_Tse){
      SubSys_.assign("41");
      SetApCode(TApCode(std::string("6")));
   }else{ 
      SubSys_.assign("97");
      SetApCode(TApCode(std::string("8")));
   }
}
//---------------------------------------------------------------------------
K_dtor TbxTendSes::~TbxTendSes  ()
{
   DelSubSysReg();
   if(MgrPtr_ != 0)
      MgrPtr_->DelbxSes(this);
   
   SetState(sp_Destroying, "Destroying");
}
//---------------------------------------------------------------------------
K_mf(bool)  TbxTendSes::SettingApply  (const std::string& setting)
{
   if(inherited::SettingApply(setting)){
      if(GetMarket() == m_Tse)
         SetApCode(TApCode(std::string("6")));
      else 
         SetApCode(TApCode(std::string("8")));
   }else
      return false;
   return true;
}
//---------------------------------------------------------------------------
K_mf(void) TbxTendSes::OnSubSysReady()
{
   StartTimer(OrderTimeOut_);
   SesMsg_.MP_.assign("標購");
   AddWorkers();
   if(MgrPtr_ != 0)                     //應用子系統連線成功,需先向Mgr註冊
        MgrPtr_->RegOrderSes(this);
   if(GetExcMgr())
      GetExcMgr()->Alert("XX1401",ID_.as_string());
   OnSesFree();
}
//---------------------------------------------------------------------------
K_mf(void)    TbxTendSes::DelSubSysReg()
{
   if(ReqPtr_.get()!=0){
      if(ReqPtr_->GetOrdCmdID() == cid_New){
         const TStkOrder* Ord =  (ReqPtr_->GetOrdHnd().GetConst());
         TTseControlHead T030;
         T030.SubSysName_ = GetSubSys();
         T030.FunctionCode_.assign("01");
         T030.MessageType_.assign("03");
         T030.MessageTime_.assign(TKTime::Now().as_string(false));
         T030.StatusCode_.assign("00");
         Recorder->WriteBSOK(&T030,(TStkOrder&)*Ord,BrokerID_,PvcID_,true);
      }
      ReqPtr_->Update(std::string(GetErrMsg(Msg_PVCBroken)),of_Sent);
      ReqPtr_.reset();
   }
   if(MgrPtr_ != 0)
      MgrPtr_->DelOrderSes(this);
}
//---------------------------------------------------------------------------
K_mf(void)    TbxTendSes::OnSesFree()
{
   ShowState(sp_WaitForOrder);
   if(MgrPtr_ != 0)
        MgrPtr_->OnSesFree(this,Market_);
   StartTimer(OrderTimeOut_);
}
//---------------------------------------------------------------------------
K_mf(void)    TbxTendSes::ShowState(EState es)
{
   switch(es){
      case sp_WaitForOrder:
         SesMsg_.msg_.assign("Ready!");
         break;
      case sp_WaitForOrdReply:
         SesMsg_.msg_.assign("Sending!");
         break;
      case sp_Wait050:
         SesMsg_.msg_.assign("Waiting 050!");
         break;
      case sp_Error:
         SesMsg_.msg_.assign("Error!");
         break;
      default:return;
   }
   SetState(es, SesMsg_.as_string());
}
//---------------------------------------------------------------------------
K_mf(void)    TbxTendSes::OnRequest(TReqPtr ReqPtr)
{
   TStkOrder& StkOrder = ReqPtr->GetOrdHnd().GetModify();

   StopTimer();
   TKey key;
   TWorker* worker;
   ReqPtr_ = ReqPtr;            //收到request 自己保留一份
   //↓先設定狀態以免新委託再進來
   ReqPtr_->Update("委託傳送中",of_S_ing);
   ShowState(sp_WaitForOrdReply);
   key.assign(SubSys_.as_string()+"01");
   worker = Workers_.GetWorker(key);
   worker->SendReq(static_cast<TbxSes&>(*this),pc_E010,ReqPtr_);          //送出E010
   StartTimer(OrderTimeOut_);
}
//---------------------------------------------------------------------------
K_mf(void)    TbxTendSes::OnTimer           (TTimerBase*)
{
   StopTimer   ();
   switch(State_)
   {
        case sp_WaitLinkReady:
                GetMgr()->GetRecorder()->WriteLog(ID_, "Link Time out, restart session!");
                
                DelSubSysReg();
                ReStartLink();
                break;
        case sp_WaitLoginReady:
                GetMgr()->GetRecorder()->WriteLog(ID_, "Link Time out, restart session!");
                
                DelSubSysReg();
                ReStartLink();
                break;
        case sp_WaitSubSysReady:
                GetMgr()->GetRecorder()->WriteLog(ID_, "Link Time out, restart session!");
                
                DelSubSysReg();
                ReStartLink();
                break;
        case sp_WaitForOrder: {          //Heart beat
                TKey key;
                TWorker* worker;
                key.assign(SubSys_.as_string()+"00");
                worker = Workers_.GetWorker(key);
                worker->SendReq(*this,pc_E040,ReqPtr_);
                ShowState(sp_Wait050);
                StartTimer(OrderTimeOut_);
                break;  }
        case sp_WaitForOrdReply:        //timeout 沒收到委託回報
                if(ReqPtr_->GetOrdCmdID() == cid_New){
                   const TStkOrder* Ord =  (ReqPtr_->GetOrdHnd().GetConst());
                   TTseControlHead T030;
                   T030.SubSysName_ = GetSubSys();
                   T030.FunctionCode_.assign("01");
                   T030.MessageType_.assign("03"); 
                   T030.MessageTime_.assign(TKTime::Now().as_string(false));
                   T030.StatusCode_.assign("00");
                   Recorder->WriteBSOK(&T030,(TStkOrder&)*Ord,BrokerID_,PvcID_,true);
                }
                ReqPtr_->Update(std::string(GetErrMsg(Msg_Timeout)),of_AbortOrdUnknow);
                ReqPtr_.reset();
                GetMgr()->GetRecorder()->WriteLog(ID_, "Order Time out, restart session!");
                
                DelSubSysReg();
                ReStartLink();
                break;
        case sp_Wait050:
                GetMgr()->GetRecorder()->WriteLog(ID_, "040 time out, restart session!");
                
                DelSubSysReg();
                ReStartLink();
                break;
        case sp_Error:
                break;
        case sp_Signout:
                break;
        default:
                SetState(sp_Error,"Unknown Error");
                break;
   }
}
//---------------------------------------------------------------------------
K_mf(void)    TbxTendSes::OnWorkerFinished(TPacketCode PkCode,const THead& pk)
{
   StopTimer   ();
   switch(PkCode){
      case pc_E010:
           DelSubSysReg();
           break;
      case pc_E020:  {         //收到委託回報完成委託
           TStkOrder&  StkOrder = ReqPtr_->GetOrdHnd().GetModify();
           switch (StkOrder.LastOCmd_) {
              case cid_New:
                   ReqPtr_->Update("標購委託OK!", of_Finish);
                   break;
              case cid_ChgQty:
                   ReqPtr_->Update("標購改量OK!", of_Finish);
                   break;
              case cid_Del:
                   ReqPtr_->Update("標購刪單OK!", of_Finish);
                   break;
              case cid_Qry: {
                   ReqPtr_->Update("標購查詢OK!", of_Finish);
                   break;
              }
              default:
                   break;
           }
           Recorder->WriteBSOK((TE020*)(&pk),StkOrder);
           ReqPtr_.reset();
           OnSesFree();
           break;
           }
      case pc_E030:  {         //收到錯誤回覆
                //↓填入錯誤訊息,並放棄這筆委託
           if(ReqPtr_.get() == 0)
              return; 
           TStkOrder&  StkOrder = ReqPtr_->GetOrdHnd().GetModify();
           StkOrder.LastTseCode_ = pk.StatusCode_.as_int();
           if(!StkOrder.IsTseOK())
              StkOrder.TseOType_ = StkOrder.OType_;
           switch (pk.FunctionCode_.as_int()) {
              case 1:
              case 2:
                   StkOrder.LastOCmd_ = cid_New;                   //委託
                   break;
              case 3:
                   StkOrder.LastOCmd_ = cid_ChgQty;                //改量
                   break;
              case 4:
                   StkOrder.LastOCmd_ = cid_Del;                   //刪單
                   break;
              case 5:
                   StkOrder.LastOCmd_ = cid_Qry;                   //查詢
                   break;
              default:break;
           }
           if(ReqPtr_.get() != 0){
              if(pk.StatusCode_.as_int() == 1) // Time is over 950209
                 GetMgr()->SetMarketClose(this);   
              ReqPtr_->Update(MgrPtr_->GetMsg(SubSys_.as_string(),pk.StatusCode_.as_string()),
                           StkOrder.LastOCmd_==cid_New?of_AbortOrdClear:of_AbortOrdNoChange);
              ReqPtr_.reset();
           }
           Recorder->WriteBSOK((TTseControlHead*)(&pk),StkOrder,BrokerID_,PvcID_);
           OnSesFree();
           break;
          }
      case pc_E050:           //收到連線確認回覆
           OnSesFree();
           break;
   }
}
//---------------------------------------------------------------------------
K_mf(void)    TbxTendSes::AddWorkers()
{
   TKey key;
   key.assign(SubSys_.as_string()+"00");
   Workers_.SetWorker(key,&Wrk_Tend_);
   key.assign(SubSys_.as_string()+"01");
   Workers_.SetWorker(key,&Wrk_Tend_);
   key.assign(SubSys_.as_string()+"02");
   Workers_.SetWorker(key,&Wrk_Tend_);
   key.assign(SubSys_.as_string()+"03");
   Workers_.SetWorker(key,&Wrk_Tend_);
   key.assign(SubSys_.as_string()+"04");
   Workers_.SetWorker(key,&Wrk_Tend_);
   key.assign(SubSys_.as_string()+"05");
   Workers_.SetWorker(key,&Wrk_Tend_);
}
//---------------------------------------------------------------------------
