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
   OnStateChanged("�s�u���_!");
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
      OrdHnd.GetModify().LastTseCode_ = 99;  // 99:�ǰe���_�u, ���A�������e�U
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
         SesMsg_.MP_.assign("���");
         break;
      case '7':  
         if(Market_ == Stk::m_Tse)
            SubSys_.assign("32");
         else
            SubSys_.assign("98");
         SesMsg_.MP_.assign("�L��");
         break;
      case '2':
         if(Market_ == Stk::m_Tse)
            SubSys_.assign("40");
         else
            SubSys_.assign("94");
         SesMsg_.MP_.assign("�s��");
         break;
   }
   if(GetExcMgr())
      GetExcMgr()->Alert("XX1001",SesTermID_);
   if(MgrPtr_ != 0)                     //���Τl�t�γs�u���\,�ݥ��VMgr���U
        MgrPtr_->RegOrderSes(this);
   TTseControlHead T060; //�����d��
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
        MgrPtr_->DelOrderSes(this);   // ���Ѱ��u�����U
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
            case 1: {                           //����T020�h�N������ƶ�JReqRec��,�æ^��bxSes
               TT020 T020 = *(TT020*)&pkt;
               if(T020.BSCode_.as_string() != std::string("B") && T020.BSCode_.as_string() != std::string("S")){
                  GetMgr()->GetRecorder()->WriteLog(ID_, "Invalid Packet!"); 
                  ReStartLink();
                  return; 
               }
               if(ReqPtr_.get() == 0){
                  GetMgr()->GetRecorder()->WriteLog(ID_, "T060 ack, do OrdUpdate!"); 
                  OrdUpdate(pc_T020,*(THead*)(&T020));
                  if(WaitCfmReqPtr_.get() == 0)  // 20100629��ܬO���s�Ұʫ᪺�����d�ߧ@�~�^��,�άO�_�u�o�ͮɵL�ǰe�����e�U
                     OnSesFree();
                  else{                          // 20100629��ܬO�ǰe���_�u, ���s�s�u�᪺�����d�ߧ@�~[OrdConfirmTime���O�ɪ��e�U]
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
                        T020.Qty_.as_int() != qty.get_orig()){  // �����d�ߦ^���D�����e�U�^��, �۰ʭ��e
/* 20100714 �q�W�i�ժ��M�w�����e, �H[1098-�e�U���ǰe]�覡�^�� 20100728 ���SubSys+98
                        OnRequest(WaitCfmReqPtr_);
*/
                        TOrdHnd& OrdHnd = WaitCfmReqPtr_->GetOrdHnd();
                        OrdHnd.GetModify().LastTseCode_ = 98;  // 98:�ǰe���_�u, ���A�T�{���ǰe
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
               if(StkOrder.LastTseCode_ == 196){  // T070 ���^��
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
               // 20091216 �]��fix �W��R��ɵL�b����OType, �G�b���j����J�w����s�u�ɵL�������
               //StkOrder.IvacKey_.SetIvacNo(TIvacNo(T020.IvacNo_.as_string()));  
               StkOrder.TMPIvacNo_.Assign(T020.IvacNo_.as_string().c_str());  // 20110407 TMP �^�Ъ��Ȥ�b����JTMPIvacNo_ ��FIX �^�Шϥ�
               //20110407 eric�W�[ BSCode, Price, StkNo ���HTandem �^�Ф��e�мg
               StkOrder.BSCode_.assign(T020.BSCode_.as_string());//20110407 
               StkOrder.Pri_.assign(((double)T020.Pri_.as_int())/100);//20110407 
               StkOrder.StkNo_.assign(T020.StkNo_.as_string());//20110407 
               StkOrder.OType_.assign(T020.OrderType_.as_string());//20091109 �L���󵥩����Ҧ^�����O
               StkOrder.SfGroup_.assign(T020.ExchageCode_.as_string()); // 20110325 �]�L��ɬqtandem���ˬd,�ҥH���Htandem�^�����O����
               StkOrder.TMPBfQty_ = T020.BfQty_.as_int();  // 20110408 �O�dTMP�^�Ъ�BfQty & AfQty 
               StkOrder.TMPAfQty_ = T020.AfQty_.as_int();
               if(StkOrder.TseOrigTime_.IsInvalid()) {           //�Ĥ@������^��
                  StkOrder.TseOType_ = StkOrder.OType_.GetLegal();
                  StkOrder.TseOrigTime_.assign(T020.OrderTime_.as_string());
               }
               StkOrder.TseLastTime_.assign(T020.OrderTime_.as_string());
               switch (T020.FunctionCode_.as_int()) {
                  case 1:
                  case 2:
                     StkOrder.LastOCmd_   = cid_New;                   //�e�U
                     StkOrder.LastOkOCmd_ = StkOrder.LastOCmd_;        
                     break;
                  case 3:
                  case 7:
                     StkOrder.LastOCmd_ = cid_ChgQty;                //��q
                     StkOrder.LastOkOCmd_ = StkOrder.LastOCmd_;       
                     break;
                  case 4:
                  case 6:
                     StkOrder.LastOCmd_ = cid_Del;                   //�R��
                     StkOrder.LastOkOCmd_ = StkOrder.LastOCmd_;  
                     break;
                  case 5:
                     StkOrder.LastOCmd_ = cid_Qry;                   //�d��
                     break;
                  default:break;
               }
               switch (T020.FunctionCode_.as_int()) {               // 20100705 �y�ʶq���Ѫ̧R���q
                  case 6:
                     StkOrder.LastTseCode_ = 255;
                     ReqPtr_->Update("�R��w����!", of_Finish);
                     ReqPtr_.reset();
                     OnSesFree();
                     return;
                  case 7:
                     StkOrder.LastTseCode_ = 255;
                     ReqPtr_->Update("��q�w����!", of_Finish);
                     ReqPtr_.reset();
                     OnSesFree();
                     return;
                  default: break;
               }
               StkOrder.LastTseCode_ = T020.StatusCode_.as_int();
               if(T020.SubSysName_.as_int()==32 || T020.SubSysName_.as_int()==98) {
               // �L��L�e�U�ɬq�|����^�������D
                  if (StkOrder.LastOCmd_ == cid_Qry){
                     StkOrder.AfQty_   = T020.AfQty_.as_int(); 
                  }else{
                     StkOrder.BfQty_   = T020.BfQty_.as_int();
                     StkOrder.AfQty_   = T020.AfQty_.as_int(); 
                  }
               } else { 
                  if (StkOrder.LastOCmd_ != cid_New){
                     if(StkOrder.BfQty_.is_0() && StkOrder.AfQty_.is_0()){
                        if(StkOrder.Qty_.is_0()) // ��ܬO�Ѧ���^���Ӳ��ͪ��e�U, 
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
                  }else   // �s�e�U�^��������JAfQty_�Y�i 
                     StkOrder.AfQty_ = T020.AfQty_.as_int();
               }  //if (T020.
               switch (ReqPtr_->GetOrdCmdID()){
                  case cid_New:
                     ReqPtr_->Update("�e�UOK!", of_Finish);
                     break;
                  case cid_ChgQty:
                     ReqPtr_->Update("��qOK!", of_Finish);
                     break;
                  case cid_Del:
                     ReqPtr_->Update("�R��OK!", of_Finish);
                     break;
                  case cid_Qry:
                     ReqPtr_->Update("�d��OK!", of_Finish);
                     break;
                  default: break;
               }
               ReqPtr_.reset();
               OnSesFree();
               return;
            }
            case 3:{                              //����T030,�h�q��bxSes
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
                     StkOrder.LastOCmd_ = cid_New;                   //�e�U
                     break;
                  case 3:
                  case 7:
                     StkOrder.LastOCmd_ = cid_ChgQty;                //��q
                     break;
                  case 4:
                  case 6:
                     StkOrder.LastOCmd_ = cid_Del;                   //�R��
                     break;
                  case 5:
                     StkOrder.LastOCmd_ = cid_Qry;                   //�d��
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
                // 20091211 �֭p�U�L�O���~����
               bool ErrCnt = false;
               ReqPtr_->Update(MgrPtr_->GetMsg(SubSys_.as_string(),pkt.StatusCode_.as_string(),ErrCnt),
                               ReqPtr_->GetOrdCmdID()==cid_New?of_AbortOrdClear:of_AbortOrdNoChange);
               ReqPtr_.reset();
               if(ErrCnt)
                  MgrPtr_->IncreaseErrCnt(SubSys_.as_int());
               OnSesFree();
               break;
            }
            case 5: // Heatbeat ack & �����d�ߵL�O�����^��
               if(WaitCfmReqPtr_.get() == 0)
                  OnSesFree();
               else{
                  OrdTimer_->Stop();
                  TOrdHnd& OrdHnd = WaitCfmReqPtr_->GetOrdHnd();
                  OrdHnd.GetModify().LastTseCode_ = 98;  // 98:�ǰe���_�u, ���A�T�{���ǰe
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
      case 94:{ //�s�ѩe�U�^��
         TStkQty TseBfQty;
         switch (pkt.MessageType_.as_int()) {
            case 1: {                           //����T020�h�N������ƶ�JReqRec��,�æ^��bxSes
               TO020        O020 = *(TO020*)&pkt;
               if(O020.BSCode_.as_string() != std::string("B") && O020.BSCode_.as_string() != std::string("S")){
                  ReStartLink();
                  return;
               }
               if(ReqPtr_.get() == 0){
                  OrdUpdate(pc_O020,*(THead*)(&O020));
                  if(WaitCfmReqPtr_.get() == 0)  // 20100629��ܬO���s�Ұʫ᪺�����d�ߧ@�~�^��,�άO�_�u�o�ͮɵL�ǰe�����e�U
                     OnSesFree();
                  else{                          // 20100629��ܬO�ǰe���_�u, ���s�s�u�᪺�����d�ߧ@�~[OrdConfirmTime���O�ɪ��e�U]
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
                        O020.Qty_.as_int() != qty.get_orig()){  // �����d�ߦ^���D�����e�U�^��, �۰ʭ��e
/* 20100714 �q�W�i�ժ��M�w�����e, �H[1098-�e�U���ǰe]�覡�^��  20100728 ���SubSys+98
                        OnRequest(WaitCfmReqPtr_);
*/
                        TOrdHnd& OrdHnd = WaitCfmReqPtr_->GetOrdHnd();
                        OrdHnd.GetModify().LastTseCode_ = 98;  // 98:�ǰe���_�u, ���A�T�{���ǰe
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
               StkOrder.TMPIvacNo_.Assign(O020.IvacNo_.as_string().c_str());  // 20110407 TMP �^�Ъ��Ȥ�b����JTMPIvacNo_ ��FIX �^�Шϥ�
               //20110407 eric�W�[ BSCode, Price, StkNo ���HTandem �^�Ф��e�мg
               StkOrder.BSCode_.assign(O020.BSCode_.as_string());//20110407 
               StkOrder.Pri_.assign(((double)O020.Pri_.as_int())/100);//20110407 
               StkOrder.StkNo_.assign(O020.StkNo_.as_string());//20110407 
                // 20091216 �]��fix �W��R��ɵL�b����OType, �G�b���j����J�w����s�u�ɵL�������
               //StkOrder.IvacKey_.SetIvacNo(TIvacNo(O020.IvacNo_.as_string()));  
               StkOrder.OType_.assignT(ot_Gn);//20091216 �s�ѥu��O�{��
               StkOrder.SfGroup_.assign(O020.TradeKind_.as_string()); // 20110325 �Htandem�^�����O����
               StkOrder.TMPBfQty_ = O020.BfQty_.as_int();  // 20110408 �O�dTMP�^�Ъ�BfQty & AfQty 
               StkOrder.TMPAfQty_ = O020.AfQty_.as_int();
               if(StkOrder.TseOrigTime_.IsInvalid()) {           //�Ĥ@������^��
                  StkOrder.TseOrdID_.assign(O020.OrderNo_.as_string());
                  StkOrder.TseOType_ = StkOrder.OType_;
                  StkOrder.TseOrigTime_.assign(O020.OrderTime_.as_string());
               }
               StkOrder.TseLastTime_.assign(O020.OrderTime_.as_string());
               switch (O020.FunctionCode_.as_int()) {
                  case 1:
                  case 2:
                     StkOrder.LastOCmd_ = cid_New;                   //�e�U
                     StkOrder.LastOkOCmd_ = StkOrder.LastOCmd_;       
                     break;
                  case 3:
                     StkOrder.LastOCmd_ = cid_ChgQty;                //��q
                     StkOrder.LastOkOCmd_ = StkOrder.LastOCmd_;       
                     break;
                  case 4:
                     StkOrder.LastOCmd_ = cid_Del;                   //�R��
                     StkOrder.LastOkOCmd_ = StkOrder.LastOCmd_;       
                     break;
                  case 5:
                     StkOrder.LastOCmd_ = cid_Qry;                   //�d��
                     break;
                  default:break;
               }
               StkOrder.BfQty_.assign(O020.BfQty_.as_string());
               StkOrder.AfQty_.assign(O020.AfQty_.as_string());
               StkOrder.Pri_.assign(((double)O020.Pri_.as_int())/100);
               switch (StkOrder.LastOCmd_) {
                  case cid_New:
                     ReqPtr_->Update("�e�UOK!", of_Finish);
                     break;
                  case cid_ChgQty:
                     ReqPtr_->Update("��qOK!", of_Finish);
                     break;
                  case cid_Del:
                     ReqPtr_->Update("�R��OK!", of_Finish);
                     break;
                  case cid_Qry: {
                     ReqPtr_->Update("�d��OK!", of_Finish);
                     break;
                     }
                  default:
                     break;
               }
               ReqPtr_.reset();
               OnSesFree();
               break;
            }
            case 3:{                              //����O030
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
                     StkOrder.LastOCmd_ = cid_New;                   //�e�U
                     break;
                  case 3:
                     StkOrder.LastOCmd_ = cid_ChgQty;                //��q
                     break;
                  case 4:
                     StkOrder.LastOCmd_ = cid_Del;                   //�R��
                     break;
                  case 5:
                     StkOrder.LastOCmd_ = cid_Qry;                   //�d��
                     break;
                  default:break;
               }
               if(pkt.StatusCode_.as_int() == 1) // Time is over 950209
                  GetMgr()->SetMarketClose(this);   
               // 20091211 �֭p�U�L�O���~����
               bool ErrCnt = false;
               ReqPtr_->Update(MgrPtr_->GetMsg(SubSys_.as_string(),pkt.StatusCode_.as_string(),ErrCnt),
                                StkOrder.LastOCmd_==cid_New?of_AbortOrdClear:of_AbortOrdNoChange);
               ReqPtr_.reset();
               if(ErrCnt)
                   MgrPtr_->IncreaseErrCnt(SubSys_.as_int());
               OnSesFree();
               break;
            }
            case 9:                              //����O080
               if(WaitCfmReqPtr_.get() == 0)     // 20100628 �P�_�O�_�n�۰ʭ��e
                  OnSesFree();
               else{
                  OrdTimer_->Stop();
                  TOrdHnd& OrdHnd = WaitCfmReqPtr_->GetOrdHnd();
                  OrdHnd.GetModify().LastTseCode_ = 98;  // 98:�ǰe���_�u, ���A�T�{���ǰe
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
   OnStateChanged("�_�u�F!");
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
   ReqPtr_ = ReqPtr;            //����request �ۤv�O�d�@��
   ReqPtr_->Update("�e�U�ǰe��",of_S_ing);

   TStkQty      qty;
   TOType       OType;
   TOrdCmdID OrdCmdID = ReqPtr->GetOrdCmdID();
   TOrdHnd& OrdHnd = ReqPtr->GetOrdHnd();
   if(ApCode_[0] == '2'){ // �s��
      TO010 O010;
      Kway::TKTime Time;
      TStkOrder& StkOrder = OrdHnd.GetModify();
      O010.SubSysName_ = GetSubSys();
      O010.MessageType_.assign("00");
      Time = Time.Now();           //���o�{�b�ɶ�
      O010.MessageTime_.assign(Time.as_string(false));
      O010.StatusCode_.assign("00");
      if(StkOrder.LastTseCode_ == 199){
         OrdHnd.GetModify().LastTseCode_ = 198;  // 198:�_�u��Hquery �覡�d��
         O010.FunctionCode_.assign("05");        //�d��
         O010.Qty_.assign(StkOrder.Qty_.as_format(8,0));
      }else{
         switch(OrdCmdID){
            case cid_New:
               if(StkOrder.BSCode_ == bsc_Buy)
                  O010.FunctionCode_.assign("01");        //�R�i
               else
                  O010.FunctionCode_.assign("02");        //��X
               O010.Qty_.assign(StkOrder.Qty_.as_format(8,0));
               break;
            case cid_Qry:
               O010.FunctionCode_.assign("05");                //�d��
               O010.Qty_.assign(StkOrder.Qty_.as_format(8,0));
               break;
            case cid_Del:
               if(StkOrder.WrntMktMkr_)
                  O010.FunctionCode_.assign("06");
               else
                  O010.FunctionCode_.assign("04");                //�R��
               O010.Qty_.assign(std::string("00000000"));
               break;
            case cid_ChgQty:
               if(StkOrder.WrntMktMkr_)
                  O010.FunctionCode_.assign("07");                //��q
               else
                  O010.FunctionCode_.assign("03");                //��q
               Kway::Tw::Stk::TStkQty qty1;
               qty1 = (static_cast<TOrdFlow::TReqChgQtyBase*>(ReqPtr.get()))->GetReqQty();
               O010.Qty_.assign(qty1.as_format(8,0));
               break;
         }
      }
             // ����J�e�U���
      O010.BrokerId_.assign(StkOrder.IvacKey_.GetBrokerID().as_string());
      O010.PvcId_ = GetPvcID();
      O010.OrderNo_.assign(StkOrder.TseOrdID_.as_string(true));
      O010.IvacNo_.assign(StkOrder.IvacKey_.GetIvacNo().as_string(0,true));
      O010.StkNo_.assign(StkOrder.StkNo_.as_string());
      O010.Pri_.assign(StkOrder.Pri_.as_format(4,2));
      O010.BSCode_.assign(StkOrder.BSCode_.as_string());
// 20110323 eric �אּ�ϥΥ�tag 10002 ��J SfGroup_ ����
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
      if(StkOrder->LastTseCode_ == 197){ // �ϥ�T070 �d��
         OrdHnd.GetModify().LastTseCode_ = 196; // 196:��pvc �����d��
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
               T010.FunctionCode_.assign("01");        //�R�i
            else
               T010.FunctionCode_.assign("02");        //��X
            qty = StkOrder->Qty_;
            break;
         case cid_Qry:
            T010.FunctionCode_.assign("05");                //�d��
            qty = StkOrder->Qty_;
            break;
         case cid_Del:
            if(StkOrder->WrntMktMkr_)
               T010.FunctionCode_.assign("06");
            else
               T010.FunctionCode_.assign("04");                //�R��
            qty = StkOrder->AfQty_;
            break;
         case cid_ChgQty:
            if(StkOrder->WrntMktMkr_)
               T010.FunctionCode_.assign("07");                //��q
            else
               T010.FunctionCode_.assign("03");                //��q
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
   
      // ����J�e�U���
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
// 20110323 eric �אּ�ϥΥ�tag 10002 ��JSfGroup_ ����
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
      ord.TMPIvacNo_.Assign(O020->IvacNo_.as_string().c_str());  // 20110407 TMP �^�Ъ��Ȥ�b����JTMPIvacNo_ ��FIX �^�Шϥ�
      //20110407 eric�W�[ BSCode, Price, StkNo ���HTandem �^�Ф��e�мg
      ord.BSCode_.assign(O020->BSCode_.as_string());//20110407 
      ord.Pri_.assign(((double)O020->Pri_.as_int())/100);//20110407 
      ord.StkNo_.assign(O020->StkNo_.as_string());//20110407 
      ord.SfGroup_.assign(O020->TradeKind_.as_string()); // 20110325 �Htandem�^�����O����
      ord.TMPBfQty_ = O020->BfQty_.as_int();  // 20110408 �O�dTMP�^�Ъ�BfQty & AfQty 
      ord.TMPAfQty_ = O020->AfQty_.as_int();
      switch(O020->FunctionCode_.as_int()){
         case 1:
         case 2:
            if(ord.TseOrigTime_.IsInvalid() && ord.TseLastTime_.IsInvalid() && ord.AfQty_.is_0()){
               ord.AfQty_       = AfQty;
               ord.TseOrigTime_ = OrdTime;
               ord.LastOCmd_    = cid_New;
               ord.LastOkOCmd_  = cid_New;
               ord.LastMsg_.assign("�e�UOK!");
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
               ord.LastMsg_.assign((O020->FunctionCode_.as_int()==4?"�R��OK!":"��qOK!"));
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
      ord.TMPIvacNo_.Assign(T020->IvacNo_.as_string().c_str());  // 20110407 TMP �^�Ъ��Ȥ�b����JTMPIvacNo_ ��FIX �^�Шϥ�
      //20110407 eric�W�[ BSCode, Price, StkNo ���HTandem �^�Ф��e�мg
      ord.BSCode_.assign(T020->BSCode_.as_string());//20110407 
      ord.Pri_.assign(((double)T020->Pri_.as_int())/100);//20110407 
      ord.StkNo_.assign(T020->StkNo_.as_string());//20110407 
      ord.OType_.assign(T020->OrderType_.as_string());//20091109 �L���󵥩����Ҧ^�����O
      ord.SfGroup_.assign(T020->ExchageCode_.as_string()); // 20110325 �]�L��ɬqtandem���ˬd,�ҥH���Htandem�^�����O����
      ord.TMPBfQty_ = T020->BfQty_.as_int();  // 20110408 �O�dTMP�^�Ъ�BfQty & AfQty 
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
               ord.LastMsg_.assign("�e�UOK!");
               hnd.FlushUpdate(uf_OrdFlow);
            }else
               return;
            break;
         case 3:
         case 4:
            if(ord.TseLastTime_.IsInvalid() || (ord.TseLastTime_ < OrdTime && AfQty < ord.TseAfQty())){
               if(ord.BfQty_.is_0() && ord.AfQty_.is_0()){
                  if(ord.Qty_.is_0()) // ��ܬO�Ѧ���^���Ӳ��ͪ��e�U, 
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
               ord.LastMsg_.assign((T020->FunctionCode_.as_int()==4?"�R��OK!":"��qOK!"));
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
