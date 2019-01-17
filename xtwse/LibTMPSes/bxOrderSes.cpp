#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxOrderSes.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw;
using namespace Kway::Tw::bx;
using namespace Kway::Tw::Stk;
//---------------------------------------------------------------------------
static char Msg_PVCBroken[]= "0403";   //委託中斷線
static char Msg_Timeout[]  = "0404";   //委託逾時
static char Msg_ReqFail[]  = "0407";   //20081021委託要求因斷線而失敗
/////////////////////////////////////////////////////////////////////////////
K_ctor  TbxOrderSes::TbxOrderSes  (TbxSesMgr* SesMgr,Stk::TMarket mkt)
   :inherited(SesMgr),
    ResendTimer_(this)
{
   SetOrderTimeOut(90*1000);         //交易子系統之TimeOut 時間為90秒
   if(MgrPtr_ != 0){
      Recorder = MgrPtr_->GetRecorder();
      MgrPtr_->RegbxSes(this);
   }
   SetMarket(mkt);
   SetApCode(TApCode(std::string("0")));
}
//---------------------------------------------------------------------------
K_dtor TbxOrderSes::~TbxOrderSes  ()
{
   DelSubSysReg();
   if(MgrPtr_ != 0)
      MgrPtr_->DelbxSes(this);
   SetState(sp_Destroying, "Destroying");
}
//---------------------------------------------------------------------------
K_mf(void) TbxOrderSes::OnSubSysReady()
{
   StartTimer(OrderTimeOut_);
   switch(ApCode_[0]){
      case '0':
         if(Market_ == Stk::m_Tse)
            SubSys_.assign("30");
         else
            SubSys_.assign("93");
         SesMsg_.MP_.assign("整股");
         break;
      case '7':  
         if(Market_ == Stk::m_Tse)
            SubSys_.assign("32");
         else
            SubSys_.assign("98");
         SesMsg_.MP_.assign("盤後");
         break;
      case '2':
         if(Market_ == Stk::m_Tse)
            SubSys_.assign("40");
         else
            SubSys_.assign("94");
         SesMsg_.MP_.assign("零股");
         break;
   }
   if(GetExcMgr())
      GetExcMgr()->Alert("XX1001",ID_.as_string());
   AddWorkers();
   if(MgrPtr_ != 0)                     //應用子系統連線成功,需先向Mgr註冊
        MgrPtr_->RegOrderSes(this);
//   OnSesFree();
////////////////////////////////////////////////////
   TKey key;
   TWorker* worker;
   key.assign(SubSys_.as_string()+"00");
   worker = Workers_.GetWorker(key);
   worker->SendReq(*this,pc_T060,ReqPtr_);
////////////////////////////////////////////////////
}
//---------------------------------------------------------------------------
K_mf(void)    TbxOrderSes::DelSubSysReg()
{
   StopResendTimer();
   if(ReqPtr_.get()!=0){
      TStkOrder& Ord_ =  (ReqPtr_->GetOrdHnd().GetModify());
      if(ReqPtr_->GetOrdCmdID() != cid_Qry && MgrPtr_->IsAutoRequest()){
         if(GetSubSys().as_int() == 30 &&  MgrPtr_->IsUseT070()){
            if(Ord_.LastTseCode_ != 196 && Ord_.LastTseCode_ != 197){
               memcpy(&Ord_.DstBrkID_,GetPvcID().as_string().c_str(),sizeof(TPvcID));
               Ord_.SrcBrkID_.assign(GetBrokerID().as_string());
            }
            Ord_.LastTseCode_ = 197; // 197 表示新委託斷線使用T070查詢
         }else
            Ord_.LastTseCode_ = 199; // 199 表示新委託斷線使用委託查詢
         ETradeKind tk;
         switch(MgrPtr_->MarketPh_){
            case ts_Full: tk = tk_Normal; break;
            case ts_Odd : tk = tk_Odd   ; break;
            case ts_Fix : if(IsFix_) tk = tk_Fix; else tk = tk_Odd; break;
            default: break;
         }
         ReqPtr_->Update(GetBrokerID().as_string()+"-"+GetPvcID().as_string()+":Auto inquiry processing!",
                         of_S_ing);
         StartResendTimer(3); // 三秒後送出
/*
         if(!MgrPtr_->NewOrderAlert(Market_, ReqPtr_, tk))
            MgrPtr_->DoAutoRequest(ReqPtr_, Market_, tk);
         ReqPtr_.reset();
*/
      }else{
         switch(ReqPtr_->GetOrdCmdID()){
            case cid_New:{
               Ord_.LostQty_ = Ord_.Qty_; 
               TTseControlHead T030;
               T030.SubSysName_ = GetSubSys();
               T030.FunctionCode_.assign(Ord_.BSCode_ == bsc_Buy?"01":"02");
               T030.MessageType_.assign("03");
               T030.MessageTime_.assign(TKTime::Now().as_string(false));
               T030.StatusCode_.assign("00");
#ifdef __LOG__
               Recorder->WriteBSOK(&T030,Ord_,BrokerID_,PvcID_,true);
#endif
               break;
            }
            case cid_Del:
               Ord_.LostQty_ = Ord_.TseAfQty(); 
               break;
            case cid_ChgQty:
               TStkQty qty;
               qty = (static_cast<TOrdFlow::TReqChgQtyBase*>(ReqPtr_.get()))->GetReqQty();
               if(qty < 0){
                  int tmp = qty;
                  tmp = tmp * (-1);
                  qty = tmp;
               } else if(qty == 0)
                  qty = Ord_.TseAfQty();
               Ord_.LostQty_ = qty; 
               break;
         } 
         ReqPtr_->Update(std::string(GetErrMsg(Msg_PVCBroken)),(ReqPtr_->GetOrdCmdID()==cid_Qry?of_AbortOrdUnknow:of_Sent));
         ReqPtr_.reset();
      }
   }
   if(MgrPtr_ != 0)
        MgrPtr_->DelOrderSes(this);   // 先解除線路註冊
}
//---------------------------------------------------------------------------
K_mf(void)  TbxOrderSes::StartResendTimer  (size_t ms)
{
   ResendTimer_.Start(ms*1000, &this_type::OnResendTimer);
}
//---------------------------------------------------------------------------
K_mf(void)  TbxOrderSes::StopResendTimer  ()
{
   ResendTimer_.Stop();
}
//---------------------------------------------------------------------------
K_mf(void)  TbxOrderSes::OnResendTimer  (TTimerBase*)
{
   StopResendTimer();
   if(State_ == sp_WaitToResend)
      OnRequest(ReqPtr_);
   else{
      ETradeKind tk;
      switch(MgrPtr_->MarketPh_){
         case ts_Full: tk = tk_Normal; break;
         case ts_Odd : tk = tk_Odd   ; break;
         case ts_Fix : if(IsFix_) tk = tk_Fix; else tk = tk_Odd; break;
         default: break;
      }
      if(!MgrPtr_->NewOrderAlert(Market_, ReqPtr_, tk))
         MgrPtr_->DoAutoRequest(ReqPtr_, Market_, tk);
      ReqPtr_.reset();
   }
}
//---------------------------------------------------------------------------
K_mf(void)    TbxOrderSes::OnSesFree()
{
   if(MgrPtr_ != 0)
        MgrPtr_->OnSesFree(this,Market_);
   ShowState(sp_WaitForOrder);
   StartTimer(OrderTimeOut_);
}
//---------------------------------------------------------------------------
K_mf(void)    TbxOrderSes::ShowState(EState es)
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
      case sp_WaitToResend:
         SesMsg_.msg_.assign("Waiting to resend!");
         break;
      case sp_Error:
         SesMsg_.msg_.assign("Error!");
         break;
      default:return;
   }
   SetState(es, SesMsg_.as_string());
}
//---------------------------------------------------------------------------
K_mf(TApCode)    TbxOrderSes::GetApCode()
{
   switch(ApCode_[0]){
      case '0':
         if(Market_ == Stk::m_Tse)
            SubSys_.assign("30");
         else
            SubSys_.assign("93");
         SesMsg_.MP_.assign("整股");
         break;
      case '7':  
         if(Market_ == Stk::m_Tse)
            SubSys_.assign("32");
         else
            SubSys_.assign("98");
         SesMsg_.MP_.assign("盤後");
         break;
      case '2':
         if(Market_ == Stk::m_Tse)
            SubSys_.assign("40");
         else
            SubSys_.assign("94");
         SesMsg_.MP_.assign("零股");
         break;
   }
   return ApCode_;
}
//---------------------------------------------------------------------------
K_mf(void)    TbxOrderSes::OnRequest(TReqPtr& ReqPtr)
{
   StopTimer();
   TKey key;
   TWorker* worker;
   ReqPtr_ = ReqPtr;            //收到request 自己保留一份
   //↓先設定狀態以免新委託再進來
   ReqPtr_->Update("委託傳送中",of_S_ing);
   ShowState(sp_WaitForOrdReply);
   key.assign(SubSys_.as_string()+"01");
   worker = Workers_.GetWorker(key);
   if(worker)
      worker->SendReq(static_cast<TbxSes&>(*this),pc_T010,ReqPtr_);          //送出T010
//   ReqPtr.reset(); // 20080530 清除updater,以免無用的updater 累積,影響GetOrdHnd() 的效率
   StartTimer(OrderTimeOut_);
}
//---------------------------------------------------------------------------
K_mf(void)    TbxOrderSes::OnTimer           (TTimerBase*)
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
                worker->SendReq(*this,pc_T040,ReqPtr_);
                ShowState(sp_Wait050);
                StartTimer(OrderTimeOut_);
                break;  }
        case sp_WaitForOrdReply:       //timeout 沒收到委託回報
                if(ReqPtr_->GetOrdCmdID() == cid_New){
                   const TStkOrder* Ord =  (ReqPtr_->GetOrdHnd().GetConst());
                   TTseControlHead T030;
                   T030.SubSysName_ = GetSubSys();
                   T030.FunctionCode_.assign(Ord->BSCode_ == bsc_Buy?"01":"02");
                   T030.MessageType_.assign("03");
                   T030.MessageTime_.assign(TKTime::Now().as_string(false));
                   T030.StatusCode_.assign("00");
#ifdef __LOG__
                   Recorder->WriteBSOK(&T030,(TStkOrder&)*Ord,BrokerID_,PvcID_,true);
#endif
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
K_mf(void)    TbxOrderSes::OnWorkerFinished(TPacketCode PkCode,const THead& pk)
{
   StopTimer   ();
   switch(PkCode)
   {
        case pc_T010:
                DelSubSysReg();
                break;
        case pc_T020:  {         //收到委託回報完成委託
                switch (ReqPtr_->GetOrdCmdID()) {
                   case cid_New:
                      ReqPtr_->Update("委託OK!", of_Finish);
                      break;
                   case cid_ChgQty:
                      ReqPtr_->Update("改量OK!", of_Finish);
                      break;
                   case cid_Del:
                      ReqPtr_->Update("刪單OK!", of_Finish);
                      break;
                   case cid_Qry: {
                      ReqPtr_->Update("查詢OK!", of_Finish);
                      break;
                      }
                   default:
                      break;
                }
#ifdef __LOG__
                Recorder->WriteBSOK((TT020*)(&pk),StkOrder);
#endif
                ReqPtr_.reset();
                OnSesFree();
                break;
                }
        case pc_T030:  {         //收到錯誤回覆
                //↓填入錯誤訊息,並放棄這筆委託
                if(ReqPtr_.get() == 0)
                   return; 
                TStkOrder&  StkOrder = ReqPtr_->GetOrdHnd().GetModify();
                if(StkOrder.LastTseCode_ == 198){
                   StkOrder.LastTseCode_ = 0;
                   TTseControlHead T030;
                   T030.SubSysName_ = GetSubSys();
                   T030.FunctionCode_.assign(StkOrder.BSCode_ == bsc_Buy?"01":"02");
                   T030.MessageType_.assign("03");
                   T030.MessageTime_.assign(TKTime::Now().as_string(false));
                   T030.StatusCode_.assign("00");
#ifdef __LOG__
                   Recorder->WriteBSOK(&T030,StkOrder,BrokerID_,PvcID_,true);
#endif
                   if(MgrPtr_->IsAutoJudge()){
                      MgrPtr_->DoRequestFail(ReqPtr_);
                      ReqPtr_.reset();
                      OnSesFree();
                      return;
                   }else if(StkOrder.LastOCmd_ == cid_ChgQty){
                      OnWorkerFinished(pc_T020,pk); 
                      return;
                   }
                   ReqPtr_->Update(std::string(GetErrMsg(Msg_PVCBroken)),of_Sent);
                   ReqPtr_.reset();
                   OnSesFree();
                   return;
                }
                StkOrder.LastTseCode_ = ((TT030*)(&pk))->StatusCode_.as_int();
                if(!StkOrder.IsTseOK())
                   StkOrder.TseOType_ = StkOrder.OType_.GetLegal();
                switch (((TT030*)(&pk))->FunctionCode_.as_int()) {
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
#ifdef __LOG__
                    Recorder->WriteBSOK((TT030*)&pk,StkOrder,BrokerID_,PvcID_);
#endif
                    switch(pk.StatusCode_.as_int()){
                    case 1:
                       GetMgr()->SetMarketClose(this);
                       break;
                    case 2:
                       if(MgrPtr_->IsAutoResend()){
                          StkOrder.LastTseCode_ = 0; // 20080331 避免當作委託失敗,導至累計委託數量錯誤
                          ReqPtr_->Update(std::string("委託時間未到,等待重送..!"),of_S_ing);
                          StartResendTimer(60 - (pk.MessageTime_.as_int() % 100));
                          ShowState(sp_WaitToResend);
                          return;
                       }
                       break;
                    case 4:
                       if(MgrPtr_->IsResendOnMatch()){
                       	  OnRequest(ReqPtr_);
                          return;
                       }
                       break;
                    case 5:
                       if(ReqPtr_->GetOrdCmdID() == cid_Del || ReqPtr_->GetOrdCmdID() == cid_ChgQty)
                       	  StkOrder.MatchQty_ = StkOrder.AfQty_;
                       break;
                    default: break;
                    }
                    // 20091211 累計各盤別錯誤次數
                    bool ErrCnt = false;
                    ReqPtr_->Update(MgrPtr_->GetMsg(SubSys_.as_string(),pk.StatusCode_.as_string(),ErrCnt),
                                    StkOrder.LastOCmd_==cid_New?of_AbortOrdClear:of_AbortOrdNoChange);
                    ReqPtr_.reset();
                    if(ErrCnt)
                        MgrPtr_->IncreaseErrCnt(SubSys_.as_int());
                }
                OnSesFree();
                break;
                }
        case pc_O020:  {         //收到委託回報完成委託
                TStkOrder&  StkOrder = ReqPtr_->GetOrdHnd().GetModify();
                switch (StkOrder.LastOCmd_) {
                   case cid_New:
                      ReqPtr_->Update("委託OK!", of_Finish);
                      break;
                   case cid_ChgQty:
                      ReqPtr_->Update("改量OK!", of_Finish);
                      break;
                   case cid_Del:
                      ReqPtr_->Update("刪單OK!", of_Finish);
                      break;
                   case cid_Qry: {
                      ReqPtr_->Update("查詢OK!", of_Finish);
                      break;
                      }
                   default:
                      break;
                }
#ifdef __LOG__
                Recorder->WriteBSOK((TO020*)(&pk),StkOrder);
#endif
                ReqPtr_.reset();
                OnSesFree();
                break;
                }
        case pc_O030:  {         //收到錯誤回覆
                //↓填入錯誤訊息,並放棄這筆委託
               if(ReqPtr_.get() == 0)
                  return;
                TStkOrder&  StkOrder = ReqPtr_->GetOrdHnd().GetModify();
                if(StkOrder.LastTseCode_ == 198){
                   StkOrder.LostQty_ = StkOrder.Qty_; 
                   TTseControlHead T030;
                   T030.SubSysName_ = GetSubSys();
                   T030.FunctionCode_.assign(StkOrder.BSCode_ == bsc_Buy?"01":"02");
                   T030.MessageType_.assign("03");
                   T030.MessageTime_.assign(TKTime::Now().as_string(false));
                   T030.StatusCode_.assign("00");
#ifdef __LOG__
                   Recorder->WriteBSOK(&T030,StkOrder,BrokerID_,PvcID_,true);
#endif
                   if(MgrPtr_->IsAutoJudge()){
                      MgrPtr_->DoRequestFail(ReqPtr_);
                      ReqPtr_.reset();
                      OnSesFree();
                      return;
                   }
                   ReqPtr_->Update(std::string(GetErrMsg(Msg_PVCBroken)),of_Sent);
                   ReqPtr_.reset();
                   OnSesFree();
                   return;
                }
                StkOrder.LastTseCode_ = ((TT030*)(&pk))->StatusCode_.as_int();
                if(!StkOrder.IsTseOK())
                   StkOrder.TseOType_ = StkOrder.OType_.GetLegal();
                switch (((TT030*)(&pk))->FunctionCode_.as_int()) {
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
#ifdef __LOG__
                    Recorder->WriteBSOK((TT030*)&pk,StkOrder,BrokerID_,PvcID_);
#endif
                    if(pk.StatusCode_.as_int() == 2 && MgrPtr_->IsAutoResend()){     // Time is too early, 940630
                       StkOrder.LastTseCode_ = 0; // 20080331 避免當作委託失敗,導至累計委託數量錯誤
                       ReqPtr_->Update(std::string("委託時間未到,等待重送..!"),of_S_ing);
                       StartResendTimer(60 - (pk.MessageTime_.as_int() % 100));
                       ShowState(sp_WaitToResend);
                       break;
                    }else{
                       if(pk.StatusCode_.as_int() == 1) // Time is over 950209
                          GetMgr()->SetMarketClose(this);   
                       // 20091211 累計各盤別錯誤次數
                       bool ErrCnt = false;
                       ReqPtr_->Update(MgrPtr_->GetMsg(SubSys_.as_string(),pk.StatusCode_.as_string(),ErrCnt),
                                        StkOrder.LastOCmd_==cid_New?of_AbortOrdClear:of_AbortOrdNoChange);
                       ReqPtr_.reset();
                       if(ErrCnt)
                           MgrPtr_->IncreaseErrCnt(SubSys_.as_int());
                    }
                }
                OnSesFree();
                break;
                }
        case pc_T040:{
                TKey key;
                TWorker* worker;
                if(Market_ == m_Tse)
                   key.assign("1010");
                else
                   key.assign("9110 ");
                worker = Workers_.GetWorker(key);
                if(worker != 0)
                   worker->SendReq(*this,pc_T050,ReqPtr_);          //回覆T050
                ShowState(sp_WaitForOrder);
                StartTimer(OrderTimeOut_);
                break;
                }
        case pc_T050:
                // 此線路的當天第一筆委託失敗, T070查詢也失敗時的處理方式
                if(ReqPtr_.get() != 0 && ReqPtr_->GetOrdHnd().GetConst()->LastTseCode_ == 196){ 
                   if(MgrPtr_->IsResendOnFail()){
                      OnRequest(ReqPtr_);
                      return;
                   }else{
                      TStkOrder&  StkOrder = ReqPtr_->GetOrdHnd().GetModify();
                      ReqPtr_->Update(std::string(GetErrMsg(Msg_ReqFail)),
                                       ReqPtr_->GetOrdCmdID()==cid_New?of_AbortOrdClear:of_AbortOrdNoChange);
                      ReqPtr_.reset();
                   }
                }
                OnSesFree();
                break;
        case pc_T060:
                ShowState(sp_WaitForOrder);
                StartTimer(OrderTimeOut_);
                break;
        case pc_T070E:  // 查詢失敗,直接放棄或再重送一次這個要求???
                if(MgrPtr_->IsResendOnFail())
                   OnRequest(ReqPtr_);
                else{
                   ReqPtr_->Update(std::string(GetErrMsg(Msg_ReqFail)),
                                    ReqPtr_->GetOrdCmdID()==cid_New?of_AbortOrdClear:of_AbortOrdNoChange);
                   ReqPtr_.reset();
                   OnSesFree();
                }
                break;
   }
}
//---------------------------------------------------------------------------
K_mf(void)    TbxOrderSes::AddWorkers()
{
   TKey key;
   if(ApCode_[0] == '2'){
      key.assign(SubSys_.as_string()+"00");
      Workers_.SetWorker(key,&Wrk_T4000_);
      key.assign(SubSys_.as_string()+"01");
      Workers_.SetWorker(key,&Wrk_OOrder_);
      key.assign(SubSys_.as_string()+"02");
      Workers_.SetWorker(key,&Wrk_OOrder_);
      key.assign(SubSys_.as_string()+"03");
      Workers_.SetWorker(key,&Wrk_OOrder_);
      key.assign(SubSys_.as_string()+"04");
      Workers_.SetWorker(key,&Wrk_OOrder_);
      key.assign(SubSys_.as_string()+"05");
      Workers_.SetWorker(key,&Wrk_OOrder_);
   }else {
      key.assign(SubSys_.as_string()+"00");
      Workers_.SetWorker(key,&Wrk_T3000_);
      key.assign(SubSys_.as_string()+"01");
      Workers_.SetWorker(key,&Wrk_TOrder_);
      key.assign(SubSys_.as_string()+"02");
      Workers_.SetWorker(key,&Wrk_TOrder_);
      key.assign(SubSys_.as_string()+"03");
      Workers_.SetWorker(key,&Wrk_TOrder_);
      key.assign(SubSys_.as_string()+"04");
      Workers_.SetWorker(key,&Wrk_TOrder_);
      key.assign(SubSys_.as_string()+"05");
      Workers_.SetWorker(key,&Wrk_TOrder_);
   }
}
//---------------------------------------------------------------------------
K_mf(void) TbxOrderSes::OrdUpdate(TPacketCode code,const THead& pk)
{
   if(code == pc_O020){
      TO020* O020 = (TO020*)(&pk);
      TTseOrdKey OrdKey = TTseOrdKey(TBrokerID(O020->BrokerId_.as_string()),GetMarket(),
                                      TTseOrdID(O020->OrderNo_.as_string()));
      TOrdHnd hnd = MgrPtr_->GetOrdCenter()->GetOrdHnd(OrdKey, TStkQty(0), uf_OrdFlow, TSysOrdID(0));
      if(hnd.is_invalid())
         return;
      TStkOrder& ord = hnd.GetModify();
      TKTime OrdTime(O020->OrderTime_.as_int());
      TStkQty BfQty(O020->BfQty_.as_int());
      TStkQty AfQty(O020->AfQty_.as_int());
      switch(O020->FunctionCode_.as_int()){
         case 1:
         case 2:
            if(ord.TseOrigTime_.IsInvalid() && ord.TseLastTime_.IsInvalid() && ord.AfQty_.is_0()){
               ord.AfQty_       = AfQty;
               ord.TseOrigTime_ = OrdTime;
               ord.LastOCmd_    = cid_New;
               ord.LastOkOCmd_  = cid_New;
               ord.LastMsg_.assign("委託OK!");
            }else
               return;
            break;
         case 3:
         case 4:
            if(ord.TseLastTime_.IsInvalid() || (ord.TseLastTime_ < OrdTime && AfQty < ord.AfQty_)){
               ord.AfQty_       = AfQty; 
               ord.BfQty_       = BfQty;
               ord.TseLastTime_ = OrdTime;
               ord.LastOCmd_    = O020->FunctionCode_.as_int()==4?cid_Del:cid_ChgQty;
               ord.LastOkOCmd_  = ord.LastOCmd_;
               ord.LastMsg_.assign((O020->FunctionCode_.as_int()==4?"刪單OK!":"改量OK!"));
            }else
               return;
            break;
         case 5:
         default: return;
      }
#ifdef __LOG__
      Recorder->WriteBSOK(O020,ord);
#endif
      ord.FlowID_.SetFlow(of_Finish);
      ord.FlowID_.SetStep(0);
   }else{
      TT020* T020 = (TT020*)(&pk);
      TTseOrdKey OrdKey = TTseOrdKey(TBrokerID(T020->BrokerId_.as_string()),GetMarket(),
                                      TTseOrdID(T020->OrderNo_.as_string()));
      TOrdHnd hnd = MgrPtr_->GetOrdCenter()->GetOrdHnd(OrdKey, TStkQty(0), uf_OrdFlow, TSysOrdID(0));
      if(hnd.is_invalid())
         return;
      TStkOrder& ord = hnd.GetModify();
      TKTime OrdTime(T020->OrderTime_.as_int());
      TStkQty BfQty(T020->BfQty_.as_int());
      TStkQty AfQty(T020->AfQty_.as_int());
      switch(T020->FunctionCode_.as_int()){
         case 1:
         case 2:
            if(ord.TseOrigTime_.IsInvalid() && ord.TseLastTime_.IsInvalid() && ord.AfQty_.is_0()){
               ord.AfQty_       = AfQty;
               ord.TseOrigTime_ = OrdTime;
               ord.TseLastTime_ = OrdTime;
               ord.LastOCmd_    = cid_New;
               ord.LastOkOCmd_  = cid_New;
               ord.FlowID_.SetStep(0);
               ord.FlowID_.SetFlow(of_Finish);
               ord.LastMsg_.assign("委託OK!");
               hnd.FlushUpdate(uf_OrdFlow);
            }else
               return;
            break;
         case 3:
         case 4:
            if(ord.TseLastTime_.IsInvalid() || (ord.TseLastTime_ < OrdTime && AfQty < ord.TseAfQty())){
               if(ord.BfQty_.is_0() && ord.AfQty_.is_0()){
                  if(ord.Qty_.is_0()) // 表示是由成交回報而產生的委託, 
                     ord.MatchQty_ = ord.DealQty_;
                  else
                     ord.MatchQty_ = ord.Qty_;
               }else
                  ord.MatchQty_ = ord.AfQty_;
               if(!ord.MatchQty_.is_0() && ord.MatchQty_ != ord.DealQty_)
                  ord.MatchQty_-= BfQty;
               ord.AfQty_       = AfQty; 
               ord.AfQty_      += ord.MatchQty_; 
               ord.BfQty_       = BfQty;
               ord.BfQty_      += ord.MatchQty_;
               ord.TseLastTime_ = OrdTime;
               ord.LastOCmd_    = T020->FunctionCode_.as_int()==4?cid_Del:cid_ChgQty;
               ord.LastOkOCmd_  = ord.LastOCmd_;
               ord.FlowID_.SetStep(0);
               ord.FlowID_.SetFlow(of_Finish);
               ord.LastMsg_.assign((T020->FunctionCode_.as_int()==4?"刪單OK!":"改量OK!"));
               hnd.FlushUpdate(uf_OrdFlow);
            }else
               return;
            break;
         case 5:
         default: return;
      }
#ifdef __LOG__
      Recorder->WriteBSOK(T020,ord);
#endif
      ord.FlowID_.SetFlow(of_Finish);
      ord.FlowID_.SetStep(0);
   }
}
//---------------------------------------------------------------------------
