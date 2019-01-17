//---------------------------------------------------------------------------
#include "TseOrdSes.h"
#include "TwStkOrder.h"
//===================================================================
using namespace Kway;
using namespace Kway::Tw::Stk;
using namespace Kway::Tw::bx;
//===================================================================
K_ctor TTseOrdSes::TTseOrdSes(TbxSesMgr* sm, TMarket mkt)
   :inherited(sm)
{
   Timer_       = new TOrdTimer(this);
   OrdTimer_    = new TOrdTimer(this);
   Ready_  = false;
   Sending_= false;
   IsFix_  = false;
   if(MgrPtr_ != 0){
      MgrPtr_->RegbxSes(this);
   }
   SetMarket(mkt);
   SetApCode(TApCode(std::string("0")));
}
//===================================================================
K_dtor TTseOrdSes::~TTseOrdSes()
{
   Timer_->Stop();
   OrdTimer_->Stop();
   OnStateChanged("連線中斷!");
   delete Timer_;
   delete OrdTimer_;
   DelSubSysReg();
   if(MgrPtr_ != 0)
      MgrPtr_->DelbxSes(this);
}
//===================================================================
K_mf(void)  TTseOrdSes::ResetTimer  (size_t sec)
{
   Kway::word interval;
   Timer_->Stop();
   switch(State_){
      case sp_WaitLinkReady:
         interval = 5;
         break;
      case sp_WaitForOrder:
         interval = 50;
         break;
      case sp_WaitForOrdReply:
         interval = 90;
         break;
   }
   Timer_->Start(interval*1000, &this_type::OnTimer);
}
//---------------------------------------------------------------------------
K_mf(void)  TTseOrdSes::OnTimer  (TTimerBase*)
{
   switch(State_){
      case sp_WaitLinkReady:
         ReStartLink();
         return;
      case sp_WaitForOrder:{
         TTseControlHead F341_;
         F341_.SubSysName_.assign(SubSys_.as_string());
         F341_.FunctionCode_.assign("00");
         F341_.MessageType_.assign(ApCode_[0]=='2'?"04":"02");
         F341_.MessageTime_.assign(TKTime::Now().as_string(false));
         F341_.StatusCode_.assign("00");
         SendPkt((TbxRec&)F341_);
         break;
      }
      case sp_WaitForOrdReply:
         if(ReqPtr_.get()!= 0)
            ReqPtr_.reset();
         GetMgr()->GetRecorder()->WriteLog(ID_, "Order Time out, restart session!");
         ReStartLink();
         return;
   }
}
//---------------------------------------------------------------------------
K_mf(void)  TTseOrdSes::StartOrdTimer  ()
{
   OrdTimer_->Start(MgrPtr_->GetConfirmTime()*1000, &this_type::OnOrdTimer);
}
//---------------------------------------------------------------------------
K_mf(void)  TTseOrdSes::OnOrdTimer  (TTimerBase*)
{
   OrdTimer_->Stop();
   if(WaitCfmReqPtr_.get()!= 0){
      TOrdHnd& OrdHnd = WaitCfmReqPtr_->GetOrdHnd();
      OrdHnd.GetModify().LastTseCode_ = 99;  // 99:傳送中斷線, 狀態不明的委託
      bool ErrCnt;
      WaitCfmReqPtr_->Update(MgrPtr_->GetMsg(SubSys_.as_string(),"99",ErrCnt),
                      WaitCfmReqPtr_->GetOrdCmdID() == cid_New?of_AbortOrdClear:of_AbortOrdNoChange);
      WaitCfmReqPtr_.reset();
   }
}
//---------------------------------------------------------------------------
K_mf(void) TTseOrdSes::OnSubSysReady()
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
   if(GetExcMgr())
      GetExcMgr()->Alert("XX1001",SesTermID_);
   if(MgrPtr_ != 0)                     //應用子系統連線成功,需先向Mgr註冊
        MgrPtr_->RegOrderSes(this);
   TTseControlHead T060; //末筆查詢
   T060.SubSysName_.assign(SubSys_.as_string());
   T060.FunctionCode_.assign("00");
   T060.MessageType_.assign(ApCode_[0]=='2'?"06":"04");
   T060.MessageTime_.assign(TKTime::Now().as_string(false));
   T060.StatusCode_.assign("00");
   SendPkt((TbxRec&)T060);
}
//---------------------------------------------------------------------------
K_mf(void)    TTseOrdSes::DelSubSysReg()
{
   Ready_ = false;
   if(MgrPtr_ != 0)
        MgrPtr_->DelOrderSes(this);   // 先解除線路註冊
   if(ReqPtr_.get() != 0){
      StartOrdTimer();
      WaitCfmReqPtr_ = ReqPtr_;
      ReqPtr_.reset();
   }
}
//---------------------------------------------------------------------------
K_mf(void) TTseOrdSes::OnPktRecv(TbxRec& pkt)
{
   Sending_ = false;
   GetMgr()->GetRecorder()->WriteAFSD(ID_, &pkt, GetPacketSize(&pkt)); 
   switch (pkt.SubSysName_.as_int()){
      case 10:
      case 91:{
         if(pkt.StatusCode_.as_int() > 0){
            bool ErrCnt = false;
            std::string ErrMsg = MgrPtr_->GetMsg(pkt.SubSysName_.as_string(),pkt.StatusCode_.as_string(),ErrCnt);
            if(ReqPtr_.get() != 0){
               ReqPtr_->Update(ErrMsg, ReqPtr_->GetOrdCmdID()==cid_New?of_AbortOrdClear:of_AbortOrdNoChange);
               ReqPtr_.reset();
            }
            if(GetExcMgr()){
               if(GetMgr()->IsTester())
                  GetExcMgr()->Alert("XT1002",SesTermID_,ErrMsg);
               else
                  GetExcMgr()->Alert("XX1002",SesTermID_,ErrMsg);
            }
            return;
         }
         switch(pkt.MessageType_.as_int()){
            case 0:{ //Recv L010 then Send L010 Back
               SetState(sp_WaitLinkReady,"Sending L010!");
               TTseControlHead L110_ = *(TTseControlHead*)&pkt;
               L110_.FunctionCode_.assign("10");
               L110_.MessageType_.assign("01");
               L110_.MessageTime_.assign(TKTime::Now().as_string(false));
               L110_.StatusCode_.assign("00");
               SendPkt((TbxRec&)L110_);
               return;
            }
            case 1:{
               return;
            }
            case 2:{ //Recv L020 then Send L030 Back
               SetState(sp_WaitLinkReady,"Sending L030!");
               TL040 L140_ = *(TL040*)&pkt;
               L140_.FunctionCode_.assign("20");
               L140_.MessageType_.assign("03");
               L140_.MessageTime_.assign(TKTime::Now().as_string(false));
               L140_.StatusCode_.assign("00");
               L140_.BrokerID_.assign(BrokerID_.as_string());
               L140_.ApCode_.assign(ApCode_.as_string());
               int keyv = ((L140_.AppendNo_.as_int() * PassWord_) / 100) % 100;
               char buf[16];
               sprintf(buf,"%02d",keyv);
               L140_.KeyValue_.assign(buf);
               SendPkt((TbxRec&)L140_);
               return;
            }
            case 3: {
               return;
            }
            case 4: { //Recv L040 then Send L050 Back
               SetState(sp_WaitLinkReady,"Sending L050!");
               TTseControlHead L160_ = *(TTseControlHead*)&pkt;
               L160_.FunctionCode_.assign("20");
               L160_.MessageType_.assign("05");
               L160_.MessageTime_.assign(TKTime::Now().as_string(false));
               L160_.StatusCode_.assign("00");
               char buf[32];
               SendPkt((TbxRec&)L160_);
               Ready_ = true;
               State_ = sp_WaitForOrder;
               OnSubSysReady();
               sprintf(buf,"%s-Ready!",SesMsg_.MP_.as_string().c_str());
               OnStateChanged(buf);
               return;
            }
         }
         return;
      }
      case 30:
      case 32:
      case 93:
      case 98:{
         TStkQty TseBfQty;
         switch (pkt.MessageType_.as_int()) {
            case 1: {                           //收到T020則將相關資料填入ReqRec中,並回覆bxSes
               TT020 T020 = *(TT020*)&pkt;
               if(T020.BSCode_.as_string() != std::string("B") && T020.BSCode_.as_string() != std::string("S")){
                  GetMgr()->GetRecorder()->WriteLog(ID_, "Invalid Packet!"); 
                  ReStartLink();
                  return; 
               }
               if(ReqPtr_.get() == 0){
                  GetMgr()->GetRecorder()->WriteLog(ID_, "T060 ack, do OrdUpdate!"); 
                  OrdUpdate(pc_T020,*(THead*)(&T020));
                  if(WaitCfmReqPtr_.get() == 0)  // 20100629表示是重新啟動後的末筆查詢作業回覆,或是斷線發生時無傳送中的委託
                     OnSesFree();
                  else{                          // 20100629表示是傳送中斷線, 重新連線後的末筆查詢作業[OrdConfirmTime未逾時的委託]
                     OrdTimer_->Stop();
                     TOrdHnd& OrdHnd = WaitCfmReqPtr_->GetOrdHnd();
                     TStkOrder& StkOrder = OrdHnd.GetModify();
                     TStkQty qty;
                     TOrdCmdID cmd;
                     switch (T020.FunctionCode_.as_int()) {
                        case 1:
                        case 2: cmd = cid_New;    
                                qty = StkOrder.Qty_;
                                break;
                        case 3: cmd = cid_ChgQty; 
                                qty = (static_cast<TOrdFlow::TReqChgQtyBase*>(WaitCfmReqPtr_.get()))->GetReqQty();
                                if(qty < 0){
                                   int tmp = qty;
                                   tmp = tmp * (-1); 
                                   qty = tmp;
                                } else if(qty == 0)
                                  qty = StkOrder.AfQty_; 
                                break;
                        case 4: cmd = cid_Del;    
                                qty = T020.Qty_.as_int();
                                break;
                        case 5: cmd = cid_Qry;    
                                qty = T020.Qty_.as_int();
                                break;
                        default:break;
                     }
                     if(WaitCfmReqPtr_->GetOrdCmdID() != cmd || T020.OrderNo_.as_string() != StkOrder.TseOrdID_.as_string(true) ||
                        T020.Qty_.as_int() != qty.get_orig()){  // 末筆查詢回報非此筆委託回報, 自動重送
/* 20100714 電規張組長決定不重送, 以[1098-委託未傳送]方式回應 20100728 改用SubSys+98
                        OnRequest(WaitCfmReqPtr_);
*/
                        TOrdHnd& OrdHnd = WaitCfmReqPtr_->GetOrdHnd();
                        OrdHnd.GetModify().LastTseCode_ = 98;  // 98:傳送中斷線, 狀態確認未傳送
                        bool ErrCnt;
                        WaitCfmReqPtr_->Update(MgrPtr_->GetMsg(SubSys_.as_string(),"98",ErrCnt),
                                               WaitCfmReqPtr_->GetOrdCmdID() == cid_New?of_AbortOrdClear:of_AbortOrdNoChange);
                     }
                     WaitCfmReqPtr_.reset();
                     OnSesFree();
                  }
                  return;
               }
               TOrdHnd& OrdHnd = ReqPtr_->GetOrdHnd();
               TStkOrder& StkOrder = OrdHnd.GetModify();
/*
               if(StkOrder.LastTseCode_ == 196){  // T070 的回覆
                  TOrdCmdID cmd;
                  switch (T020.FunctionCode_.as_int()) {
                     case 1:
                     case 2: cmd = cid_New;    break;
                     case 3: cmd = cid_ChgQty; break;
                     case 4: cmd = cid_Del;    break;
                     case 5: cmd = cid_Qry;    break;
                     default:break;
                  }
                  if(ReqPtr_->GetOrdCmdID() != cmd || T020.OrderNo_.as_string() != StkOrder.TseOrdID_.as_string(true)){
                     StkOrder.LastTseCode_ = 0;
                     GetMgr()->GetRecorder()->WriteLog(ID_, "Not expected packet!"); 
                     OnSesFree();
                     return;
                  }
               }
*/
               // 20091216 因為fix 規格刪單時無帳號及OType, 故在此強迫填入預防跨連線時無相關資料
               //StkOrder.IvacKey_.SetIvacNo(TIvacNo(T020.IvacNo_.as_string()));  
               StkOrder.TMPIvacNo_.Assign(T020.IvacNo_.as_string().c_str());  // 20110407 TMP 回覆的客戶帳號填入TMPIvacNo_ 供FIX 回覆使用
               //20110407 eric增加 BSCode, Price, StkNo 欄位以Tandem 回覆內容覆寫
               StkOrder.BSCode_.assign(T020.BSCode_.as_string());//20110407 
               StkOrder.Pri_.assign(((double)T020.Pri_.as_int())/100);//20110407 
               StkOrder.StkNo_.assign(T020.StkNo_.as_string());//20110407 
               StkOrder.OType_.assign(T020.OrderType_.as_string());//20091109 無條件等於交易所回覆類別
               StkOrder.SfGroup_.assign(T020.ExchageCode_.as_string()); // 20110325 因盤後時段tandem不檢查,所以須以tandem回覆類別為準
               StkOrder.TMPBfQty_ = T020.BfQty_.as_int();  // 20110408 保留TMP回覆的BfQty & AfQty 
               StkOrder.TMPAfQty_ = T020.AfQty_.as_int();
               if(StkOrder.TseOrigTime_.IsInvalid()) {           //第一次收到回覆
                  StkOrder.TseOType_ = StkOrder.OType_.GetLegal();
                  StkOrder.TseOrigTime_.assign(T020.OrderTime_.as_string());
               }
               StkOrder.TseLastTime_.assign(T020.OrderTime_.as_string());
               switch (T020.FunctionCode_.as_int()) {
                  case 1:
                  case 2:
                     StkOrder.LastOCmd_   = cid_New;                   //委託
                     StkOrder.LastOkOCmd_ = StkOrder.LastOCmd_;        
                     break;
                  case 3:
                  case 7:
                     StkOrder.LastOCmd_ = cid_ChgQty;                //改量
                     StkOrder.LastOkOCmd_ = StkOrder.LastOCmd_;       
                     break;
                  case 4:
                  case 6:
                     StkOrder.LastOCmd_ = cid_Del;                   //刪單
                     StkOrder.LastOkOCmd_ = StkOrder.LastOCmd_;  
                     break;
                  case 5:
                     StkOrder.LastOCmd_ = cid_Qry;                   //查詢
                     break;
                  default:break;
               }
               switch (T020.FunctionCode_.as_int()) {               // 20100705 流動量提供者刪單減量
                  case 6:
                     StkOrder.LastTseCode_ = 255;
                     ReqPtr_->Update("刪單已接收!", of_Finish);
                     ReqPtr_.reset();
                     OnSesFree();
                     return;
                  case 7:
                     StkOrder.LastTseCode_ = 255;
                     ReqPtr_->Update("改量已接收!", of_Finish);
                     ReqPtr_.reset();
                     OnSesFree();
                     return;
                  default: break;
               }
               StkOrder.LastTseCode_ = T020.StatusCode_.as_int();
               if(T020.SubSysName_.as_int()==32 || T020.SubSysName_.as_int()==98) {
               // 盤後無委託時段漏成交回報的問題
                  if (StkOrder.LastOCmd_ == cid_Qry){
                     StkOrder.AfQty_   = T020.AfQty_.as_int(); 
                  }else{
                     StkOrder.BfQty_   = T020.BfQty_.as_int();
                     StkOrder.AfQty_   = T020.AfQty_.as_int(); 
                  }
               } else { 
                  if (StkOrder.LastOCmd_ != cid_New){
                     if(StkOrder.BfQty_.is_0() && StkOrder.AfQty_.is_0()){
                        if(StkOrder.Qty_.is_0()) // 表示是由成交回報而產生的委託, 
                           StkOrder.MatchQty_ = StkOrder.DealQty_;
                        else
                           StkOrder.MatchQty_ = StkOrder.Qty_;
                     }else
                        StkOrder.MatchQty_ = StkOrder.AfQty_;
                     if (StkOrder.LastOCmd_ == cid_Qry){
                        if(!StkOrder.MatchQty_.is_0() && StkOrder.MatchQty_ != StkOrder.DealQty_)
                           StkOrder.MatchQty_ -= T020.AfQty_.as_int();
                        if(T020.AfQty_.as_int()> StkOrder.TseAfQty() || 
                           T020.AfQty_.as_int()> StkOrder.AfQty_){
                           StkOrder.AfQty_ = T020.AfQty_.as_int();
                           StkOrder.AfQty_ += StkOrder.MatchQty_;
                        }
                     }else{
                        if(!StkOrder.MatchQty_.is_0() && StkOrder.MatchQty_ != StkOrder.DealQty_)
                           StkOrder.MatchQty_ -= T020.BfQty_.as_int();
                        StkOrder.AfQty_   = T020.AfQty_.as_int(); 
                        StkOrder.AfQty_  += StkOrder.MatchQty_; 
                        StkOrder.BfQty_   = T020.BfQty_.as_int();
                        StkOrder.BfQty_  += StkOrder.MatchQty_;
                     }
                  }else   // 新委託回報直接填入AfQty_即可 
                     StkOrder.AfQty_ = T020.AfQty_.as_int();
               }  //if (T020.
               switch (ReqPtr_->GetOrdCmdID()){
                  case cid_New:
                     ReqPtr_->Update("委託OK!", of_Finish);
                     break;
                  case cid_ChgQty:
                     ReqPtr_->Update("改量OK!", of_Finish);
                     break;
                  case cid_Del:
                     ReqPtr_->Update("刪單OK!", of_Finish);
                     break;
                  case cid_Qry:
                     ReqPtr_->Update("查詢OK!", of_Finish);
                     break;
                  default: break;
               }
               ReqPtr_.reset();
               OnSesFree();
               return;
            }
            case 3:{                              //收到T030,則通知bxSes
               if(ReqPtr_.get() == 0){
                  OnSesFree();
                  return; 
               }
               TStkOrder&  StkOrder = ReqPtr_->GetOrdHnd().GetModify();
               StkOrder.LastTseCode_ = ((TT030*)(&pkt))->StatusCode_.as_int();
               if(!StkOrder.IsTseOK())
                  StkOrder.TseOType_ = StkOrder.OType_.GetLegal();
               switch (((TT030*)(&pkt))->FunctionCode_.as_int()) {
                  case 1:
                  case 2:
                     StkOrder.LastOCmd_ = cid_New;                   //委託
                     break;
                  case 3:
                  case 7:
                     StkOrder.LastOCmd_ = cid_ChgQty;                //改量
                     break;
                  case 4:
                  case 6:
                     StkOrder.LastOCmd_ = cid_Del;                   //刪單
                     break;
                  case 5:
                     StkOrder.LastOCmd_ = cid_Qry;                   //查詢
                     break;
                  default:break;
               }
               switch(pkt.StatusCode_.as_int()){
                  case 1:
                     GetMgr()->SetMarketClose(this);
                     break;
                  case 2:
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
               ReqPtr_->Update(MgrPtr_->GetMsg(SubSys_.as_string(),pkt.StatusCode_.as_string(),ErrCnt),
                               ReqPtr_->GetOrdCmdID()==cid_New?of_AbortOrdClear:of_AbortOrdNoChange);
               ReqPtr_.reset();
               if(ErrCnt)
                  MgrPtr_->IncreaseErrCnt(SubSys_.as_int());
               OnSesFree();
               break;
            }
            case 5: // Heatbeat ack & 末筆查詢無記錄的回報
               if(WaitCfmReqPtr_.get() == 0)
                  OnSesFree();
               else{
                  OrdTimer_->Stop();
                  TOrdHnd& OrdHnd = WaitCfmReqPtr_->GetOrdHnd();
                  OrdHnd.GetModify().LastTseCode_ = 98;  // 98:傳送中斷線, 狀態確認未傳送
                  bool ErrCnt;
                  WaitCfmReqPtr_->Update(MgrPtr_->GetMsg(SubSys_.as_string(),"98",ErrCnt),
                                         WaitCfmReqPtr_->GetOrdCmdID() == cid_New?of_AbortOrdClear:of_AbortOrdNoChange);
//                  OnRequest(WaitCfmReqPtr_);
                  WaitCfmReqPtr_.reset();
               }
               break;
            default:   return;
         }
         return;
      }
      case 40:
      case 94:{ //零股委託回報
         TStkQty TseBfQty;
         switch (pkt.MessageType_.as_int()) {
            case 1: {                           //收到T020則將相關資料填入ReqRec中,並回覆bxSes
               TO020        O020 = *(TO020*)&pkt;
               if(O020.BSCode_.as_string() != std::string("B") && O020.BSCode_.as_string() != std::string("S")){
                  ReStartLink();
                  return;
               }
               if(ReqPtr_.get() == 0){
                  OrdUpdate(pc_O020,*(THead*)(&O020));
                  if(WaitCfmReqPtr_.get() == 0)  // 20100629表示是重新啟動後的末筆查詢作業回覆,或是斷線發生時無傳送中的委託
                     OnSesFree();
                  else{                          // 20100629表示是傳送中斷線, 重新連線後的末筆查詢作業[OrdConfirmTime未逾時的委託]
                     OrdTimer_->Stop();
                     TOrdHnd& OrdHnd = WaitCfmReqPtr_->GetOrdHnd();
                     TStkOrder& StkOrder = OrdHnd.GetModify();
                     TStkQty qty;
                     TOrdCmdID cmd;
                     switch (O020.FunctionCode_.as_int()) {
                        case 1:
                        case 2: cmd = cid_New;    
                                qty = StkOrder.Qty_;
                                break;
                        case 3: cmd = cid_ChgQty; 
                                qty = (static_cast<TOrdFlow::TReqChgQtyBase*>(WaitCfmReqPtr_.get()))->GetReqQty();
                                break;
                        case 4: cmd = cid_Del;    
                                qty = O020.Qty_.as_int();
                                break;
                        case 5: cmd = cid_Qry;    
                                qty = O020.Qty_.as_int();
                                break;
                        default:break;
                     }
                     if(WaitCfmReqPtr_->GetOrdCmdID() != cmd || O020.OrderNo_.as_string() != StkOrder.TseOrdID_.as_string(true) ||
                        O020.Qty_.as_int() != qty.get_orig()){  // 末筆查詢回報非此筆委託回報, 自動重送
/* 20100714 電規張組長決定不重送, 以[1098-委託未傳送]方式回應  20100728 改用SubSys+98
                        OnRequest(WaitCfmReqPtr_);
*/
                        TOrdHnd& OrdHnd = WaitCfmReqPtr_->GetOrdHnd();
                        OrdHnd.GetModify().LastTseCode_ = 98;  // 98:傳送中斷線, 狀態確認未傳送
                        bool ErrCnt;
                        WaitCfmReqPtr_->Update(MgrPtr_->GetMsg(SubSys_.as_string(),"98",ErrCnt),
                                               WaitCfmReqPtr_->GetOrdCmdID() == cid_New?of_AbortOrdClear:of_AbortOrdNoChange);
                     }
                     WaitCfmReqPtr_.reset();
                     OnSesFree();
                  }
                  return;
               }
               TOrdHnd& OrdHnd = ReqPtr_->GetOrdHnd();
               TStkOrder& StkOrder = OrdHnd.GetModify();
               StkOrder.LastTseCode_ = O020.StatusCode_.as_int();
               StkOrder.TMPIvacNo_.Assign(O020.IvacNo_.as_string().c_str());  // 20110407 TMP 回覆的客戶帳號填入TMPIvacNo_ 供FIX 回覆使用
               //20110407 eric增加 BSCode, Price, StkNo 欄位以Tandem 回覆內容覆寫
               StkOrder.BSCode_.assign(O020.BSCode_.as_string());//20110407 
               StkOrder.Pri_.assign(((double)O020.Pri_.as_int())/100);//20110407 
               StkOrder.StkNo_.assign(O020.StkNo_.as_string());//20110407 
                // 20091216 因為fix 規格刪單時無帳號及OType, 故在此強迫填入預防跨連線時無相關資料
               //StkOrder.IvacKey_.SetIvacNo(TIvacNo(O020.IvacNo_.as_string()));  
               StkOrder.OType_.assignT(ot_Gn);//20091216 零股只能是現股
               StkOrder.SfGroup_.assign(O020.TradeKind_.as_string()); // 20110325 以tandem回覆類別為準
               StkOrder.TMPBfQty_ = O020.BfQty_.as_int();  // 20110408 保留TMP回覆的BfQty & AfQty 
               StkOrder.TMPAfQty_ = O020.AfQty_.as_int();
               if(StkOrder.TseOrigTime_.IsInvalid()) {           //第一次收到回覆
                  StkOrder.TseOrdID_.assign(O020.OrderNo_.as_string());
                  StkOrder.TseOType_ = StkOrder.OType_;
                  StkOrder.TseOrigTime_.assign(O020.OrderTime_.as_string());
               }
               StkOrder.TseLastTime_.assign(O020.OrderTime_.as_string());
               switch (O020.FunctionCode_.as_int()) {
                  case 1:
                  case 2:
                     StkOrder.LastOCmd_ = cid_New;                   //委託
                     StkOrder.LastOkOCmd_ = StkOrder.LastOCmd_;       
                     break;
                  case 3:
                     StkOrder.LastOCmd_ = cid_ChgQty;                //改量
                     StkOrder.LastOkOCmd_ = StkOrder.LastOCmd_;       
                     break;
                  case 4:
                     StkOrder.LastOCmd_ = cid_Del;                   //刪單
                     StkOrder.LastOkOCmd_ = StkOrder.LastOCmd_;       
                     break;
                  case 5:
                     StkOrder.LastOCmd_ = cid_Qry;                   //查詢
                     break;
                  default:break;
               }
               StkOrder.BfQty_.assign(O020.BfQty_.as_string());
               StkOrder.AfQty_.assign(O020.AfQty_.as_string());
               StkOrder.Pri_.assign(((double)O020.Pri_.as_int())/100);
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
               ReqPtr_.reset();
               OnSesFree();
               break;
            }
            case 3:{                              //收到O030
               if(ReqPtr_.get() == 0){
                  OnSesFree();
                  return;
               }
               TStkOrder&  StkOrder = ReqPtr_->GetOrdHnd().GetModify();
               StkOrder.LastTseCode_ = ((TT030*)(&pkt))->StatusCode_.as_int();
               if(!StkOrder.IsTseOK())
                  StkOrder.TseOType_ = StkOrder.OType_.GetLegal();
               switch (((TT030*)(&pkt))->FunctionCode_.as_int()) {
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
               if(pkt.StatusCode_.as_int() == 1) // Time is over 950209
                  GetMgr()->SetMarketClose(this);   
               // 20091211 累計各盤別錯誤次數
               bool ErrCnt = false;
               ReqPtr_->Update(MgrPtr_->GetMsg(SubSys_.as_string(),pkt.StatusCode_.as_string(),ErrCnt),
                                StkOrder.LastOCmd_==cid_New?of_AbortOrdClear:of_AbortOrdNoChange);
               ReqPtr_.reset();
               if(ErrCnt)
                   MgrPtr_->IncreaseErrCnt(SubSys_.as_int());
               OnSesFree();
               break;
            }
            case 9:                              //收到O080
               if(WaitCfmReqPtr_.get() == 0)     // 20100628 判斷是否要自動重送
                  OnSesFree();
               else{
                  OrdTimer_->Stop();
                  TOrdHnd& OrdHnd = WaitCfmReqPtr_->GetOrdHnd();
                  OrdHnd.GetModify().LastTseCode_ = 98;  // 98:傳送中斷線, 狀態確認未傳送
                  bool ErrCnt;
                  WaitCfmReqPtr_->Update(MgrPtr_->GetMsg(SubSys_.as_string(),"98",ErrCnt),
                                         WaitCfmReqPtr_->GetOrdCmdID() == cid_New?of_AbortOrdClear:of_AbortOrdNoChange);
//                  OnRequest(WaitCfmReqPtr_);
                  WaitCfmReqPtr_.reset();
               }
               break;
            default: return;
         }
         return;
      }
   } // switch (pkt.SubSysName_....
}
//===================================================================
K_mf(void) TTseOrdSes::OnMesLinkReady(TMesBase* M_)
{
   FailCount_ = 0;
   if(GetExcMgr())
      if(GetMgr()->IsTester())
         GetExcMgr()->Alert("XT1000",SesTermID_);
      else
         GetExcMgr()->Alert("XX1000",SesTermID_);
   GetMgr()->GetRecorder()->WriteLog(ID_, "Link Ready!");

   Ready_ = false;
   
   char buf[256];
   TTseControlHead L010_;
   L010_.SubSysName_.assign(Market_ == m_Tse?"10":"91");
   L010_.FunctionCode_.assign("10");
   L010_.MessageType_.assign("00");
   L010_.MessageTime_.assign(TKTime::Now().as_string(false));
   L010_.StatusCode_.assign("00");
   SendPkt((TbxRec&)L010_);
   SetState(sp_WaitLinkReady,"Sending L010!");
}
//===================================================================
K_mf(void) TTseOrdSes::OnMesLinkBroken(TMesBase*, const std::string&)
{
   Ready_ = false;
   Timer_->Stop();
   DelSubSysReg();
   OnStateChanged("斷線了!");
   if(GetExcMgr())
      if(GetMgr()->IsTester())
         GetExcMgr()->Alert("XX1012",SesTermID_);
      else
         GetExcMgr()->Alert("XX1012",SesTermID_);
   GetMgr()->GetRecorder()->WriteLog(ID_, "Link Broken!");
}
//===================================================================
K_mf(void) TTseOrdSes::SendPkt(TbxRec& pkt)
{
	Sending_ = true;
   ResetTimer(60);
   GetMgr()->GetRecorder()->WriteBFSD(ID_, &pkt, GetPacketSize(&pkt)); 
	Send(pkt);
}
//---------------------------------------------------------------------------
K_mf(void)    TTseOrdSes::OnRequest(TReqPtr& ReqPtr)
{
   StopTimer();
   ReqPtr_ = ReqPtr;            //收到request 自己保留一份
   ReqPtr_->Update("委託傳送中",of_S_ing);

   TStkQty      qty;
   TOType       OType;
   TOrdCmdID OrdCmdID = ReqPtr->GetOrdCmdID();
   TOrdHnd& OrdHnd = ReqPtr->GetOrdHnd();
   if(ApCode_[0] == '2'){ // 零股
      TO010 O010;
      Kway::TKTime Time;
      TStkOrder& StkOrder = OrdHnd.GetModify();
      O010.SubSysName_ = GetSubSys();
      O010.MessageType_.assign("00");
      Time = Time.Now();           //取得現在時間
      O010.MessageTime_.assign(Time.as_string(false));
      O010.StatusCode_.assign("00");
      if(StkOrder.LastTseCode_ == 199){
         OrdHnd.GetModify().LastTseCode_ = 198;  // 198:斷線後以query 方式查詢
         O010.FunctionCode_.assign("05");        //查詢
         O010.Qty_.assign(StkOrder.Qty_.as_format(8,0));
      }else{
         switch(OrdCmdID){
            case cid_New:
               if(StkOrder.BSCode_ == bsc_Buy)
                  O010.FunctionCode_.assign("01");        //買進
               else
                  O010.FunctionCode_.assign("02");        //賣出
               O010.Qty_.assign(StkOrder.Qty_.as_format(8,0));
               break;
            case cid_Qry:
               O010.FunctionCode_.assign("05");                //查詢
               O010.Qty_.assign(StkOrder.Qty_.as_format(8,0));
               break;
            case cid_Del:
               if(StkOrder.WrntMktMkr_)
                  O010.FunctionCode_.assign("06");
               else
                  O010.FunctionCode_.assign("04");                //刪單
               O010.Qty_.assign(std::string("00000000"));
               break;
            case cid_ChgQty:
               if(StkOrder.WrntMktMkr_)
                  O010.FunctionCode_.assign("07");                //減量
               else
                  O010.FunctionCode_.assign("03");                //減量
               Kway::Tw::Stk::TStkQty qty1;
               qty1 = (static_cast<TOrdFlow::TReqChgQtyBase*>(ReqPtr.get()))->GetReqQty();
               O010.Qty_.assign(qty1.as_format(8,0));
               break;
         }
      }
             // ↓填入委託資料
      O010.BrokerId_.assign(StkOrder.IvacKey_.GetBrokerID().as_string());
      O010.PvcId_ = GetPvcID();
      O010.OrderNo_.assign(StkOrder.TseOrdID_.as_string(true));
      O010.IvacNo_.assign(StkOrder.IvacKey_.GetIvacNo().as_string(0,true));
      O010.StkNo_.assign(StkOrder.StkNo_.as_string());
      O010.Pri_.assign(StkOrder.Pri_.as_format(4,2));
      O010.BSCode_.assign(StkOrder.BSCode_.as_string());
// 20110323 eric 改為使用由tag 10002 填入 SfGroup_ 的值
      O010.TradeKind_.assign(StkOrder.SfGroup_.as_string());
//      O010.TradeKind_.assign("2");
      State_ = sp_WaitForOrdReply;
      SendPkt((TbxRec&)O010);
      return;
   }else{
      const TStkOrder* StkOrder = OrdHnd.GetConst();
      TT010 T010;
      T010.SubSysName_ = GetSubSys();
      T010.MessageType_.assign("00");
      T010.MessageTime_.assign(Kway::TKTime::Now().as_string(false));
      T010.StatusCode_.assign("00");
      if(StkOrder->LastTseCode_ == 197){ // 使用T070 查詢
         OrdHnd.GetModify().LastTseCode_ = 196; // 196:跨pvc 末筆查詢
         T010.FunctionCode_.assign("00");               
         T010.MessageType_.assign("06");
         T010.BrokerId_.assign(StkOrder->SrcBrkID_.as_string());
         memcpy(&T010.PvcId_, &StkOrder->DstBrkID_, sizeof(TPvcID));
         State_ = sp_WaitForOrdReply;
         SendPkt((TbxRec&)T010);
         return;
      }
      switch(OrdCmdID){
         case cid_New:
            if(StkOrder->BSCode_ == bsc_Buy)
               T010.FunctionCode_.assign("01");        //買進
            else
               T010.FunctionCode_.assign("02");        //賣出
            qty = StkOrder->Qty_;
            break;
         case cid_Qry:
            T010.FunctionCode_.assign("05");                //查詢
            qty = StkOrder->Qty_;
            break;
         case cid_Del:
            if(StkOrder->WrntMktMkr_)
               T010.FunctionCode_.assign("06");
            else
               T010.FunctionCode_.assign("04");                //刪單
            qty = StkOrder->AfQty_;
            break;
         case cid_ChgQty:
            if(StkOrder->WrntMktMkr_)
               T010.FunctionCode_.assign("07");                //減量
            else
               T010.FunctionCode_.assign("03");                //減量
            qty = (static_cast<TOrdFlow::TReqChgQtyBase*>(ReqPtr.get()))->GetReqQty();
            if(qty < 0){
               int tmp = qty;
               tmp = tmp * (-1); 
               qty = tmp;
            } else if(qty == 0)
               qty = StkOrder->AfQty_; 
            break;
         default:
            return;
      
      }
   
      // ↓填入委託資料
      T010.BrokerId_.assign(StkOrder->IvacKey_.GetBrokerID().as_string());
      T010.PvcId_ = GetPvcID();
      T010.OrderNo_.assign(StkOrder->TseOrdID_.as_string(true));
      T010.IvacNo_.assign(StkOrder->IvacKey_.GetIvacNo().as_string(0,true));
      if(OrdCmdID == cid_New)
         T010.IvacNo_Flag_.assign(StkOrder->TseOrderLine_.as_string());
      else
         T010.IvacNo_Flag_.set(0,StkOrder->SubAcc_[0]);
//      T010.IvacNo_Flag_.assign("1");
      T010.StkNo_.assign(StkOrder->StkNo_.as_string());
      T010.Pri_.assign(StkOrder->Pri_.as_format(4,2));
      T010.Qty_.assign(qty.as_format(3,0));
// 20110323 eric 改為使用由tag 10002 填入SfGroup_ 的值
      T010.ExchageCode_.assign(StkOrder->SfGroup_.as_string());
//      T010.ExchageCode_.assign(StkOrder->ExceedBnd_?"3":"0");
      T010.BSCode_.assign(StkOrder->BSCode_.as_string());
      if(StkOrder->OType_ == ot_Unknow)
         T010.OrderType_.assign("0");
      else
         T010.OrderType_.set(0,(char)StkOrder->OType_.GetLegal());
      State_ = sp_WaitForOrdReply;
      SendPkt((TbxRec&)T010);
      return;
   }
}
//---------------------------------------------------------------------------
K_mf(void) TTseOrdSes::OrdUpdate(TPacketCode code,const THead& pk)
{
   if(code == pc_O020){
      TO020* O020 = (TO020*)(&pk);
      TTseOrdKey OrdKey = TTseOrdKey(TBrokerID(O020->BrokerId_.as_string()),GetMarket(),
                                      TTseOrdID(O020->OrderNo_.as_string()));
      TOrdHnd hnd = GetMgr()->GetOrdCenter()->GetOrdHnd(OrdKey, TStkQty(0), uf_OrdFlow, TSysOrdID(0));
      if(hnd.is_invalid())
         return;
      TStkOrder& ord = hnd.GetModify();
      TKTime OrdTime(O020->OrderTime_.as_int());
      TStkQty BfQty(O020->BfQty_.as_int());
      TStkQty AfQty(O020->AfQty_.as_int());
      ord.TMPIvacNo_.Assign(O020->IvacNo_.as_string().c_str());  // 20110407 TMP 回覆的客戶帳號填入TMPIvacNo_ 供FIX 回覆使用
      //20110407 eric增加 BSCode, Price, StkNo 欄位以Tandem 回覆內容覆寫
      ord.BSCode_.assign(O020->BSCode_.as_string());//20110407 
      ord.Pri_.assign(((double)O020->Pri_.as_int())/100);//20110407 
      ord.StkNo_.assign(O020->StkNo_.as_string());//20110407 
      ord.SfGroup_.assign(O020->TradeKind_.as_string()); // 20110325 以tandem回覆類別為準
      ord.TMPBfQty_ = O020->BfQty_.as_int();  // 20110408 保留TMP回覆的BfQty & AfQty 
      ord.TMPAfQty_ = O020->AfQty_.as_int();
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
      ord.FlowID_.SetFlow(of_Finish);
      ord.FlowID_.SetStep(0);
   }else{
      TT020* T020 = (TT020*)(&pk);
      TTseOrdKey OrdKey = TTseOrdKey(TBrokerID(T020->BrokerId_.as_string()),GetMarket(),
                                      TTseOrdID(T020->OrderNo_.as_string()));
      TOrdHnd hnd = GetMgr()->GetOrdCenter()->GetOrdHnd(OrdKey, TStkQty(0), uf_OrdFlow, TSysOrdID(0));
      if(hnd.is_invalid())
         return;
      TStkOrder& ord = hnd.GetModify();
      TKTime OrdTime(T020->OrderTime_.as_int());
      TStkQty BfQty(T020->BfQty_.as_int());
      TStkQty AfQty(T020->AfQty_.as_int());
      ord.TMPIvacNo_.Assign(T020->IvacNo_.as_string().c_str());  // 20110407 TMP 回覆的客戶帳號填入TMPIvacNo_ 供FIX 回覆使用
      //20110407 eric增加 BSCode, Price, StkNo 欄位以Tandem 回覆內容覆寫
      ord.BSCode_.assign(T020->BSCode_.as_string());//20110407 
      ord.Pri_.assign(((double)T020->Pri_.as_int())/100);//20110407 
      ord.StkNo_.assign(T020->StkNo_.as_string());//20110407 
      ord.OType_.assign(T020->OrderType_.as_string());//20091109 無條件等於交易所回覆類別
      ord.SfGroup_.assign(T020->ExchageCode_.as_string()); // 20110325 因盤後時段tandem不檢查,所以須以tandem回覆類別為準
      ord.TMPBfQty_ = T020->BfQty_.as_int();  // 20110408 保留TMP回覆的BfQty & AfQty 
      ord.TMPAfQty_ = T020->AfQty_.as_int();
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
      ord.FlowID_.SetFlow(of_Finish);
      ord.FlowID_.SetStep(0);
   }
}
//---------------------------------------------------------------------------
K_mf(void)    TTseOrdSes::OnSesFree()
{
   if(State_ == sp_WaitLinkReady)
      return;
   State_ = sp_WaitForOrder;
   if(MgrPtr_ != 0)
      MgrPtr_->OnSesFree(this,Market_);
}
//---------------------------------------------------------------------------
K_mf(void)    TTseOrdSes::ReStartLink()
{
   SetState(sp_WaitLinkReady,"Sending L010!");
   DelSubSysReg();
   TTseControlHead L010_;
   L010_.SubSysName_.assign(Market_ == m_Tse?"10":"91");
   L010_.FunctionCode_.assign("10");
   L010_.MessageType_.assign("00");
   L010_.MessageTime_.assign(TKTime::Now().as_string(false));
   L010_.StatusCode_.assign("00");
   SendPkt((TbxRec&)L010_);
}
//---------------------------------------------------------------------------
