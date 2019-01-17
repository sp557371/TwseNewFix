#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxOrderWorker.h"
#include "bxOrder.h"
#include "TwStkOrder.h"
#include "TwStk.h"
#include "KTime.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw;
using namespace Kway::Tw::bx;
using namespace Kway::Tw::Stk;
/////////////////////////////////////////////////////////////////////////////
K_mf(TCheckResult)  TWkr_T3000::OnPkCheck  (TbxSes& Ses,const THead& pk,
                                        TSize recvSize,TSize* PacketSize)
{
   std::string cmpstr = pk.MessageType_.as_string();
   if(cmpstr == std::string("02") ||
      cmpstr == std::string("04") ||
      cmpstr == std::string("05"))
   {
        *PacketSize = CtlHeadSize;
        return cpr_Continue;
   }
   else
        return cpr_Error;
}
//---------------------------------------------------------------------------
K_mf(void)   TWkr_T3000::OnPkRecv  (TbxSes& Ses,const THead& pk,TReqPtr& ReqPtr)
{
   std::string cmpstr = pk.MessageType_.as_string();
   if(cmpstr == std::string("02")){           //����T040,�h�q��bxSes�æ^��T050
      Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (&pk),sizeof(pk)); 
      Ses.OnWorkerFinished(pc_T040,pk);
   }else if(cmpstr == std::string("05")){      //����T050,�h�q��bxSes,��������
      Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (&pk),sizeof(pk)); 
      Ses.OnWorkerFinished(pc_T050,pk);     //�ʧ@
   }else if(cmpstr == std::string("04")){      //����T060,�h�q��bxSes
      Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (&pk),sizeof(pk)); 
      Ses.OnWorkerFinished(pc_T060,pk);
   }
}
//---------------------------------------------------------------------------
K_mf(bool)   TWkr_T3000::SendReq    (TbxSes& Ses,TPacketCode code,TReqPtr& ReqPtr)
{
   THead pk;
   Kway::TKTime Time;
   pk.SubSysName_ = Ses.GetSubSys();
   pk.FunctionCode_.assign("00");
   Time = Time.Now();           //���o�{�b�ɶ�
   pk.MessageTime_.assign(Time.as_string(false));
   pk.StatusCode_.assign("00");
   switch(code)
   {
        case pc_T040:                                   //�e�XT040
                pk.MessageType_.assign("02");
                Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, &pk,sizeof(pk)); 
                return  Ses.SendPk(pk,sizeof(pk));
        case pc_T050:
                pk.MessageType_.assign("05");           //�e�XT050
                Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, &pk,sizeof(pk)); 
                return  Ses.SendPk(pk,sizeof(pk));
        case pc_T060:
                pk.MessageType_.assign("04");           //�e�XT060
                Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, &pk,sizeof(pk)); 
                return  Ses.SendPk(pk,sizeof(pk));
        default:
                return false;
   }
}
/////////////////////////////////////////////////////////////////////////////
K_mf(TCheckResult)  TWkr_T4000::OnPkCheck  (TbxSes& Ses,const THead& pk,
                                        TSize recvSize,TSize* PacketSize)
{
   std::string cmpstr = pk.MessageType_.as_string();
   if(cmpstr == std::string("04") ||
      cmpstr == std::string("09") ||
      cmpstr == std::string("06"))
   {
        *PacketSize = CtlHeadSize;
        return cpr_Continue;
   }
   else
        return cpr_Error;
}
//---------------------------------------------------------------------------
K_mf(void)   TWkr_T4000::OnPkRecv  (TbxSes& Ses,const THead& pk,TReqPtr& ReqPtr)
{
   std::string cmpstr = pk.MessageType_.as_string();
   if(cmpstr == std::string("04")){           //����T040,�h�q��bxSes�æ^��T050
      Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (&pk),sizeof(pk));
      Ses.OnWorkerFinished(pc_T040,pk);
   }else if(cmpstr == std::string("09")){      //����T050,�h�q��bxSes,��������
      Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (&pk),sizeof(pk));
      Ses.OnWorkerFinished(pc_T050,pk);     //�ʧ@
   }else if(cmpstr == std::string("06")){      //����T060,�h�q��bxSes
      Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (&pk),sizeof(pk));
      Ses.OnWorkerFinished(pc_T060,pk);
   }
}
//---------------------------------------------------------------------------
K_mf(bool)   TWkr_T4000::SendReq    (TbxSes& Ses,TPacketCode code,TReqPtr& ReqPtr)
{
   THead pk;
   Kway::TKTime Time;
   pk.SubSysName_ = Ses.GetSubSys();
   pk.FunctionCode_.assign("00");
   Time = Time.Now();           //���o�{�b�ɶ�
   pk.MessageTime_.assign(Time.as_string(false));
   pk.StatusCode_.assign("00");
   switch(code)
   {
        case pc_T040:                                   //�e�XT040
                pk.MessageType_.assign("04");
                Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, &pk,sizeof(pk));
                return  Ses.SendPk(pk,sizeof(pk));
        case pc_T050:
                pk.MessageType_.assign("09");           //�e�XT050
                Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, &pk,sizeof(pk));
                return  Ses.SendPk(pk,sizeof(pk));
        case pc_T060:
                pk.MessageType_.assign("06");           //�e�XT060
                Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, &pk,sizeof(pk));
                return  Ses.SendPk(pk,sizeof(pk));
        default:
                return false;
   }
}
/////////////////////////////////////////////////////////////////////////////
K_mf(TCheckResult)  TWkr_TOrder::OnPkCheck  (TbxSes& Ses,const THead& pk,
                                        TSize recvSize,TSize* PacketSize)
{
   switch(pk.MessageType_.as_int()){
      case 0: *PacketSize = szT010; return cpr_Continue;
      case 1: *PacketSize = szT020; return cpr_Continue;
      case 3: *PacketSize = CtlHeadSize; return cpr_Continue;
      default: return cpr_Error;
   }
}
//---------------------------------------------------------------------------
K_mf(void)   TWkr_TOrder::OnPkRecv  (TbxSes& Ses,const THead& pk,TReqPtr& ReqPtr)
{
   TStkQty TseBfQty;
   switch (pk.MessageType_.as_int()) {
      case 1: {                           //����T020�h�N������ƶ�JReqRec��,�æ^��bxSes
         TT020        T020 = *(TT020*)&pk;
         Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, &T020,sizeof(TT020)); 
         if(T020.BSCode_.as_string() != std::string("B") && T020.BSCode_.as_string() != std::string("S")){
            Ses.GetMgr()->GetRecorder()->WriteLog(Ses.ID_, "Invalid Packet!"); 
            return; 
         }
         if(ReqPtr.get() == 0){
            //Ses.GetMgr()->GetRecorder()->WriteLog(Ses.ID_, "Invalid Packet, not expected!"); 
            Ses.OrdUpdate(pc_T020,*(THead*)(&T020));
            Ses.OnWorkerFinished(pc_T050,pk);
            return;
         }
         TOrdHnd& OrdHnd = ReqPtr->GetOrdHnd();
         TStkOrder& StkOrder = OrdHnd.GetModify();
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
            if(ReqPtr->GetOrdCmdID() != cmd || T020.OrderNo_.as_string() != StkOrder.TseOrdID_.as_string(true)){
               StkOrder.LastTseCode_ = 0;
               Ses.OnWorkerFinished(pc_T070E,*(THead*)(&T020));
               return;
            }
         }
         // 20091216 �]��fix �W��R��ɵL�b����OType, �G�b���j����J�w����s�u�ɵL�������
         StkOrder.IvacKey_.SetIvacNo(TIvacNo(T020.IvacNo_.as_string()));  
         StkOrder.OType_.assign(T020.OrderType_.as_string());//20091109 �L���󵥩����Ҧ^�����O
         if(StkOrder.TseOrigTime_.IsInvalid()) {           //�Ĥ@������^��
            StkOrder.Pri_.assign(((double)T020.Pri_.as_int())/100);
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
               StkOrder.LastOCmd_ = cid_ChgQty;                //��q
               StkOrder.LastOkOCmd_ = StkOrder.LastOCmd_;       
               break;
            case 4:
               StkOrder.LastOCmd_ = cid_Del;                   //�R��
               StkOrder.LastOkOCmd_ = StkOrder.LastOCmd_;  
               break;
            case 5:
               if(StkOrder.LastTseCode_ == 198){ // �_�u��d��
                  StkOrder.LastTseCode_ = 0;
                  switch(ReqPtr->GetOrdCmdID()){
                     case cid_New:
                        StkOrder.LastOCmd_   = ReqPtr->GetOrdCmdID();  
                        StkOrder.LastOkOCmd_ = StkOrder.LastOCmd_;    
                        StkOrder.AfQty_      = StkOrder.Qty_;
                        Ses.OnWorkerFinished(pc_T020,*(THead*)(&T020)); 
                        return;
                     case cid_Del:
                        Ses.OnWorkerFinished(pc_T070E,*(THead*)(&T020));
                        return;
                     case cid_ChgQty:
                        if(StkOrder.LeftQty() == T020.AfQty_.as_int()){
                           Ses.OnWorkerFinished(pc_T070E,*(THead*)(&T020));
                        }else{
                           StkOrder.LastTseCode_ = 198;
                           StkOrder.LostQty_ = StkOrder.LeftQty() - T020.AfQty_.as_int();
                           StkOrder.MatchQty_= StkOrder.DealQty_;
                           Ses.OnWorkerFinished(pc_T030,pk);
                        }
                        return;
                     default:break;
                  }
               }else
                  StkOrder.LastOCmd_ = cid_Qry;                   //�d��
               break;
            default:break;
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
         // 940930��Ѯɬq���B�z�覡(�n�Ҷq�_�u�κ|����^���������D...)
         // 940930�ץ��D�s��^����AfQty_, BfQty_, MatchQty_  �i�ƳB�z
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
      Ses.OnWorkerFinished(pc_T020,*(THead*)(&T020));
      break;
      }
   case 3:                              //����T030,�h�q��bxSes
      Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (TT030*)(&pk),sizeof(pk)); 
      Ses.OnWorkerFinished(pc_T030,pk);
      break;
   }
}
//---------------------------------------------------------------------------
K_mf(bool)   TWkr_TOrder::SendReq    (TbxSes& Ses,TPacketCode code,TReqPtr& ReqPtr)
{
   Kway::TKTime Time;
   TStkQty      qty;
   TOType       OType;
   std::string  TempStr;
   TOrdCmdID OrdCmdID = ReqPtr->GetOrdCmdID();
   TOrdHnd& OrdHnd = ReqPtr->GetOrdHnd();
   const TStkOrder* StkOrder = OrdHnd.GetConst();
   TT010 T010;
   T010.SubSysName_ = Ses.GetSubSys();
   T010.MessageType_.assign("00");
   Time = Time.Now();           //���o�{�b�ɶ�
   T010.MessageTime_.assign(Time.as_string(false));
   T010.StatusCode_.assign("00");
   if(StkOrder->LastTseCode_ == 197){ // �ϥ�T070 �d��
      OrdHnd.GetModify().LastTseCode_ = 196; // 196:��pvc �����d��
      T010.FunctionCode_.assign("00");               
      T010.MessageType_.assign("06");
//      T010.BrokerId_.assign(StkOrder->LastMsg_.as_string().substr(0,4));
//      T010.PvcId_.assign(StkOrder->LastMsg_.as_string().substr(5,2));
      T010.BrokerId_.assign(StkOrder->SrcBrkID_.as_string());
      memcpy(&T010.PvcId_, &StkOrder->DstBrkID_, sizeof(TPvcID));
      Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, &T010,sizeof(TT070)); 
      return  Ses.SendPk(T010,sizeof(TT070));
   }
   switch(code){
      case pc_T010:                                   //�e�XT010(�e�U��J�T��)
          // ���M�wFunctionCode
         if(StkOrder->LastTseCode_ == 199){
            OrdHnd.GetModify().LastTseCode_ = 198;  // 198:�_�u��Hquery �覡�d��
            T010.FunctionCode_.assign("05");        //�d��
            qty = StkOrder->Qty_;
         }else{
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
                  T010.FunctionCode_.assign("04");                //�R��
                  qty = StkOrder->AfQty_;
                  break;
               case cid_ChgQty:
                  T010.FunctionCode_.assign("03");                //��q
                  qty = (static_cast<TReqChgQtyBase*>(ReqPtr.get()))->GetReqQty();
                  if(qty < 0){
                     int tmp = qty;
                     tmp = tmp * (-1); 
                     qty = tmp;
                  } else if(qty == 0)
                     qty = StkOrder->AfQty_; 
                  break;
               default:
                  return false;
            }
         }

         // ����J�e�U���
         T010.BrokerId_.assign(StkOrder->IvacKey_.GetBrokerID().as_string());
         T010.PvcId_ = Ses.GetPvcID();
/*
         if(StkOrder->TseOrdID_.IsInvalid()){
            char buf[16];
            sprintf(buf,"%c%04d",StkOrder->TseOrdID_.GetTermID()[0],StkOrder->TseOrdID_.GetSeqNo());
            T010.OrderNo_.assign(buf);
         }else
*/
            T010.OrderNo_.assign(StkOrder->TseOrdID_.as_string(true));
         T010.IvacNo_.assign(StkOrder->IvacKey_.GetIvacNo().as_string(0,true));
         // 20080901 eric �]��20081006 �e�U�q��ק�
         T010.IvacNo_Flag_.assign(StkOrder->TseOrderLine_.as_string());
         T010.StkNo_.assign(StkOrder->StkNo_.as_string());
         T010.Pri_.assign(StkOrder->Pri_.as_format(4,2));
         T010.Qty_.assign(qty.as_format(3,0));
         T010.ExchageCode_.assign(StkOrder->ExceedBnd_?"3":"0");
         T010.BSCode_.assign(StkOrder->BSCode_.as_string());
         if(StkOrder->OType_ == ot_Unknow)
            T010.OrderType_.assign("0");
         else
            T010.OrderType_.set(0,(char)StkOrder->OType_.GetLegal());
         Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, &T010,sizeof(T010)); 
         return  Ses.SendPk(T010,sizeof(T010));
      Default:
         return false;
   }
}
//---------------------------------------------------------------------------
K_mf(TCheckResult)  TWkr_OOrder::OnPkCheck  (TbxSes& Ses,const THead& pk,
                                        TSize recvSize,TSize* PacketSize)
{
   std::string cmpstr = pk.MessageType_.as_string();
   if(cmpstr == std::string("00"))             //T010(���O���i�ব�������@)
   {
        *PacketSize = sizeof(TO010);
        return cpr_Continue;
   }
   if(cmpstr == std::string("01"))             //T020(�e�U�^��)
   {
        *PacketSize = sizeof(TO020);
        return cpr_Continue;
   }
   if(cmpstr == std::string("03"))             //T010(���~�T��)
   {
        *PacketSize = CtlHeadSize;
        return cpr_Continue;
   }
   else
        return cpr_Error;
}
//---------------------------------------------------------------------------
K_mf(void)   TWkr_OOrder::OnPkRecv  (TbxSes& Ses,const THead& pk,TReqPtr& ReqPtr)
{
   TStkQty TseBfQty;
   switch (pk.MessageType_.as_int()) {
      case 1: {                           //����T020�h�N������ƶ�JReqRec��,�æ^��bxSes
         TO020        O020 = *(TO020*)&pk;
         Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, &O020,sizeof(TO020)); 
         if(O020.BSCode_.as_string() != std::string("B") && O020.BSCode_.as_string() != std::string("S"))
            return;
         if(ReqPtr.get() == 0){
            Ses.OrdUpdate(pc_O020,*(THead*)(&O020));
            Ses.OnWorkerFinished(pc_T050,pk);
            return;
         }
         TOrdHnd& OrdHnd = ReqPtr->GetOrdHnd();
         TStkOrder& StkOrder = OrdHnd.GetModify();
         StkOrder.LastTseCode_ = O020.StatusCode_.as_int();
          // 20091216 �]��fix �W��R��ɵL�b����OType, �G�b���j����J�w����s�u�ɵL�������
         StkOrder.IvacKey_.SetIvacNo(TIvacNo(O020.IvacNo_.as_string()));  
         StkOrder.OType_.assignT(ot_Gn);//20091216 �s�ѥu��O�{��
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
               if(StkOrder.LastTseCode_ == 198){ // �_�u��d��
                  StkOrder.LastTseCode_ = 0;
                  switch(ReqPtr->GetOrdCmdID()){
                     case cid_New:
                        StkOrder.LastOCmd_   = ReqPtr->GetOrdCmdID();  
                        StkOrder.LastOkOCmd_ = StkOrder.LastOCmd_;    
                        StkOrder.AfQty_      = StkOrder.Qty_;
                        Ses.OnWorkerFinished(pc_O020,*(THead*)(&O020)); 
                        return;
                     case cid_Del:
                        Ses.OnWorkerFinished(pc_T070E,*(THead*)(&O020));
                        return;
                     case cid_ChgQty:
                        if(StkOrder.LeftQty() == O020.AfQty_.as_int()){
                           Ses.OnWorkerFinished(pc_T070E,*(THead*)(&O020));
                        }else
                           Ses.OnWorkerFinished(pc_O030,pk);
                        return;
                     default:break;
                  }
               }else
                  StkOrder.LastOCmd_ = cid_Qry;                   //�d��
               break;
            default:break;
         }
      StkOrder.BfQty_.assign(O020.BfQty_.as_string());
      StkOrder.AfQty_.assign(O020.AfQty_.as_string());
      StkOrder.Pri_.assign(((double)O020.Pri_.as_int())/100);
      Ses.OnWorkerFinished(pc_O020,*(THead*)(&O020));
      break;
      }
   case 3:                              //����T030,�h�q��bxSes
      Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (&pk),sizeof(pk)); 
      Ses.OnWorkerFinished(pc_O030,pk);
      break;
   }
}
//---------------------------------------------------------------------------
K_mf(bool)   TWkr_OOrder::SendReq    (TbxSes& Ses,TPacketCode code,TReqPtr& ReqPtr)
{
   TO010 O010;
   Kway::TKTime Time;

   TOrdCmdID OrdCmdID = ReqPtr->GetOrdCmdID();
   TOrdHnd& OrdHnd = ReqPtr->GetOrdHnd();
   TStkOrder& StkOrder = OrdHnd.GetModify();
   O010.SubSysName_ = Ses.GetSubSys();
   O010.MessageType_.assign("00");
   Time = Time.Now();           //���o�{�b�ɶ�
   O010.MessageTime_.assign(Time.as_string(false));
   O010.StatusCode_.assign("00");
   switch(code){
      case pc_T010:                                   //�e�XO010(�e�U��J�T��)
                // ���M�wFunctionCode
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
                  O010.FunctionCode_.assign("04");                //�R��
                  O010.Qty_.assign(std::string("00000000"));
                  break;
               case cid_ChgQty:
                  O010.FunctionCode_.assign("03");                //��q
                  Kway::Tw::Stk::TStkQty qty1;
                  qty1 = (static_cast<TReqChgQtyBase*>(ReqPtr.get()))->GetReqQty();
/*
                  if(qty1 < 0){
                     int tmp = qty1;
                     tmp = tmp * (-1);
                     qty1 = tmp;
                  } else if(qty1 == 0)
                     qty1 = StkOrder.AfQty_;
                  Kway::Tw::Stk::TStkQty qty;
                  qty = StkOrder.AfQty_;
                  qty -= qty1;
*/
                  O010.Qty_.assign(qty1.as_format(8,0));
                  break;
            }
         }
                // ����J�e�U���
         O010.BrokerId_.assign(StkOrder.IvacKey_.GetBrokerID().as_string());
         O010.PvcId_ = Ses.GetPvcID();
/*
         if(StkOrder.TseOrdID_.IsInvalid()){
            char buf[16];
            sprintf(buf,"%c%04d",StkOrder.TseOrdID_.GetTermID()[0],StkOrder.TseOrdID_.GetSeqNo());
            O010.OrderNo_.assign(buf);
         }else
*/
            O010.OrderNo_.assign(StkOrder.TseOrdID_.as_string(true));
         O010.IvacNo_.assign(StkOrder.IvacKey_.GetIvacNo().as_string(0,true));
         O010.StkNo_.assign(StkOrder.StkNo_.as_string());
         O010.Pri_.assign(StkOrder.Pri_.as_format(4,2));
         O010.BSCode_.assign(StkOrder.BSCode_.as_string());
         O010.TradeKind_.assign("2");
         Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, &O010,sizeof(O010)); 
         return  Ses.SendPk(O010,sizeof(O010));
      default:
         return false;
   }
}
//---------------------------------------------------------------------------
K_mf(TCheckResult)  TWkr_Auction::OnPkCheck  (TbxSes& Ses,const THead& pk,TSize recvSize,TSize* PacketSize)
{
   switch(pk.MessageType_.as_int()){
      case 0: *PacketSize = sizeof(TA010);
              return cpr_Continue;
      case 1: *PacketSize = sizeof(TA020);
              return cpr_Continue;
      case 2: *PacketSize = CtlHeadSize;
              return cpr_Continue;
      case 3: *PacketSize = CtlHeadSize;
              return cpr_Continue;
      case 5: *PacketSize = CtlHeadSize;
              return cpr_Continue;
   }
   return cpr_Error;
}
//---------------------------------------------------------------------------
K_mf(void)   TWkr_Auction::OnPkRecv  (TbxSes& Ses,const THead& pk,TReqPtr& ReqPtr)
{
   switch (pk.MessageType_.as_int()) {
      case 1: {                           //����A020�h�N������ƶ�JReqRec��,�æ^��bxSes
         TA020        A020 = *(TA020*)&pk;
         Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, &A020,sizeof(TA020)); 
         if(ReqPtr.get() == 0){
            Ses.GetMgr()->GetRecorder()->WriteLog(Ses.ID_, "Invalid Packet, not expected!"); 
            return;
         }
         TOrdHnd& OrdHnd = ReqPtr->GetOrdHnd();
         TStkOrder& StkOrder = OrdHnd.GetModify();
         StkOrder.LastTseCode_ = A020.StatusCode_.as_int();
         if(StkOrder.TseOrigTime_.IsInvalid()) {           //�Ĥ@������^��
            StkOrder.Pri_.assign(((double)A020.AfPri_.as_int())/100);
            StkOrder.TseOType_ = StkOrder.OType_;
            StkOrder.TseOrigTime_.assign(A020.OrderTime_.as_string());
         }
         StkOrder.TseLastTime_.assign(A020.OrderTime_.as_string());
         switch (A020.FunctionCode_.as_int()) {
            case 1:
               StkOrder.LastOCmd_ = cid_New;                   //�e�U
               break;
            case 2:
               StkOrder.LastOCmd_ = cid_Del;                   //�R��
               break;
            case 3:
               StkOrder.LastOCmd_ = cid_ChgQty;                //��q
               break;
            case 4:
               StkOrder.LastOCmd_ = cid_Qry;                   //�d��
               break;
            default:break;
         }
         if (StkOrder.LastOCmd_ != cid_Qry)
            StkOrder.BfQty_ = A020.BfQty_.as_int();
         StkOrder.AfQty_ = A020.AfQty_.as_int();
         Ses.OnWorkerFinished(pc_A020,*(THead*)(&A020));
         break;
      }
      case 3:                              //����A030,�h�q��bxSes�e�U����
         Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (&pk),sizeof(pk)); 
         Ses.OnWorkerFinished(pc_A030,pk);
         break;
      case 5:                              //����A050,�h�q��bxSes�e�U����
         Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (&pk),sizeof(pk)); 
         Ses.OnWorkerFinished(pc_A050,pk);
         break;
   }
}
//---------------------------------------------------------------------------
K_mf(bool)   TWkr_Auction::SendReq    (TbxSes& Ses,TPacketCode code,TReqPtr& ReqPtr)
{
   switch(code){
      case pc_A010:{
         TA010 A010;
         TStkQty qty;
         TOrdHnd& OrdHnd = ReqPtr->GetOrdHnd();
         const TStkOrder* StkOrder = OrdHnd.GetConst();
         A010.SubSysName_ = Ses.GetSubSys();
         A010.MessageType_.assign("00");
         A010.MessageTime_.assign(TKTime::Now().as_string(false));
         A010.StatusCode_.assign("00");
         // ���M�wFunctionCode
         switch(ReqPtr->GetOrdCmdID()){
            case cid_New:
                 A010.FunctionCode_.assign("01");               //�R�i
                 qty = StkOrder->Qty_;
                 break;
            case cid_Del:
                 A010.FunctionCode_.assign("02");               //����
                 qty = StkOrder->BfQty_;
                 break;
            case cid_ChgQty:
                 A010.FunctionCode_.assign("03");                //���
                 qty = (static_cast<TReqChgQtyBase*>(ReqPtr.get()))->GetReqQty();
                 if(qty < 0){
                    int tmp = qty;
                    tmp = tmp * (-1); 
                    qty = tmp;
                 } else if(qty == 0)
                    qty = StkOrder->BfQty_; 
                 else 
                    qty = StkOrder->AfQty_ - qty;
                 break;
            case cid_Qry:
                 A010.FunctionCode_.assign("04");                //�d��
                 qty = StkOrder->Qty_;
                 break;
            default:
                 return false;
         }
      
         // ����J�e�U���
         A010.BrokerId_.assign(StkOrder->IvacKey_.GetBrokerID().as_string());
         A010.PvcId_ = Ses.GetPvcID();
         A010.OrderNo_.assign(StkOrder->TseOrdID_.as_string(true));
         A010.IvacNo_.assign(StkOrder->IvacKey_.GetIvacNo().as_string(0,true));
         A010.StkNo_.assign(StkOrder->StkNo_.as_string());
         A010.Pri_.assign(StkOrder->Pri_.as_format(4,2));
         A010.Qty_.assign(qty.as_format(12,0));
         Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, &A010,sizeof(A010)); 
         return  Ses.SendPk(A010,sizeof(A010));
      }
      case pc_A040:{
         THead pk;
         pk.SubSysName_ = Ses.GetSubSys();
         pk.FunctionCode_.assign("00");
         pk.MessageTime_.assign(TKTime::Now().as_string(false));
         pk.StatusCode_.assign("00");
         pk.MessageType_.assign("02");
         Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, &pk,sizeof(pk));
         return  Ses.SendPk(pk,sizeof(pk));
      }
   }
}
//---------------------------------------------------------------------------
K_mf(TCheckResult)  TWkr_Lend::OnPkCheck  (TbxSes& Ses,const THead& pk,TSize recvSize,TSize* PacketSize)
{
   switch(pk.MessageType_.as_int()){
      case 0: *PacketSize = sizeof(TV010);
              return cpr_Continue;
      case 1: *PacketSize = sizeof(TV020);
              return cpr_Continue;
      case 2: *PacketSize = CtlHeadSize;
              return cpr_Continue;
      case 3: *PacketSize = CtlHeadSize;
              return cpr_Continue;
      case 5: *PacketSize = CtlHeadSize;
              return cpr_Continue;
   }
   return cpr_Error;
}
//---------------------------------------------------------------------------
K_mf(void)   TWkr_Lend::OnPkRecv  (TbxSes& Ses,const THead& pk,TReqPtr& ReqPtr)
{
   switch (pk.MessageType_.as_int()) {
      case 1: {                           //����V020�h�N������ƶ�JReqRec��,�æ^��bxSes
         TV020        V020 = *(TV020*)&pk;
         Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, &V020,sizeof(TV020)); 
         if(ReqPtr.get() == 0){
            Ses.GetMgr()->GetRecorder()->WriteLog(Ses.ID_, "Invalid Packet, not expected!"); 
            return;
         }
         TOrdHnd& OrdHnd = ReqPtr->GetOrdHnd();
         TStkOrder& StkOrder = OrdHnd.GetModify();
         StkOrder.LastTseCode_ = V020.StatusCode_.as_int();
         if(StkOrder.TseOrigTime_.IsInvalid()) {           //�Ĥ@������^��
            StkOrder.Pri_.assign(((double)V020.Pri_.as_int())/100);
            StkOrder.TseOType_ = StkOrder.OType_;
            StkOrder.TseOrigTime_.assign(V020.OrderTime_.as_string());
         }
         StkOrder.TseLastTime_.assign(V020.OrderTime_.as_string());
         switch (V020.FunctionCode_.as_int()) {
            case 1:
               StkOrder.LastOCmd_ = cid_New;                   //�e�U
               break;
            case 2:
               StkOrder.LastOCmd_ = cid_Del;                   //�R��
               break;
            case 3:
               StkOrder.LastOCmd_ = cid_ChgQty;                //��q
               break;
            case 4:
               StkOrder.LastOCmd_ = cid_Qry;                   //�d��
               break;
            default:break;
         }
         if (StkOrder.LastOCmd_ != cid_Qry)
            StkOrder.BfQty_ = V020.BfQty_.as_int();
         StkOrder.AfQty_ = V020.AfQty_.as_int();
         Ses.OnWorkerFinished(pc_V020,*(THead*)(&V020));
         break;
      }
      case 3:                              //����V030,�h�q��bxSes�e�U����
         Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (&pk),sizeof(pk)); 
         Ses.OnWorkerFinished(pc_V030,pk);
         break;
      case 5:                              //����V050,�h�q��bxSes�e�U����
         Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (&pk),sizeof(pk)); 
         Ses.OnWorkerFinished(pc_V050,pk);
         break;
   }
}
//---------------------------------------------------------------------------
K_mf(bool)   TWkr_Lend::SendReq    (TbxSes& Ses,TPacketCode code,TReqPtr& ReqPtr)
{
   switch(code){
      case pc_V010:{
         TV010 V010;
         TStkQty qty;
         TOrdHnd& OrdHnd = ReqPtr->GetOrdHnd();
         const TStkOrder* StkOrder = OrdHnd.GetConst();
         V010.SubSysName_ = Ses.GetSubSys();
         V010.MessageType_.assign("00");
         V010.MessageTime_.assign(TKTime::Now().as_string(false));
         V010.StatusCode_.assign("00");
         // ���M�wFunctionCode
         switch(ReqPtr->GetOrdCmdID()){
            case cid_New:
                 V010.FunctionCode_.assign("01");               //�R�i
                 qty = StkOrder->Qty_;
                 break;
            case cid_Del:
                 V010.FunctionCode_.assign("02");               //����
                 qty = 0;
                 break;
            case cid_ChgQty:
                 V010.FunctionCode_.assign("03");                //���
                 qty = (static_cast<TReqChgQtyBase*>(ReqPtr.get()))->GetReqQty();
                 if(qty < 0){
                    int tmp = qty;
                    tmp = tmp * (-1); 
                    qty = tmp;
                 } 
                 if(qty > 0)
                    qty = StkOrder->AfQty_ - qty; 
                 break;
            case cid_Qry:
                 V010.FunctionCode_.assign("04");                //�d��
                 qty = StkOrder->Qty_;
                 break;
            default:
                 return false;
         }
      
         // ����J�e�U���
         V010.BrokerId_.assign(StkOrder->IvacKey_.GetBrokerID().as_string());
         V010.PvcId_ = Ses.GetPvcID();
         V010.OrderNo_.assign(StkOrder->TseOrdID_.as_string(true));
         V010.IvacNo_.assign(StkOrder->IvacKey_.GetIvacNo().as_string(0,true));
         V010.StkNo_.assign(StkOrder->StkNo_.as_string());
         V010.Pri_.assign(StkOrder->Pri_.as_format(5,2)); // �����쬰 1/100 ��
         V010.Qty_.assign(qty.as_format(6,0));
         V010.Keep_.assign("Y");      // �ӷ�? ���Ȯɳ���'Y' 
         Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, &V010,sizeof(V010)); 
         return  Ses.SendPk(V010,sizeof(V010));
      }
      case pc_V040:{
         THead pk;
         pk.SubSysName_ = Ses.GetSubSys();
         pk.FunctionCode_.assign("00");
         pk.MessageTime_.assign(TKTime::Now().as_string(false));
         pk.StatusCode_.assign("00");
         pk.MessageType_.assign("02");
         Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, &pk,sizeof(pk));
         return  Ses.SendPk(pk,sizeof(pk));
      }
   }
}
//---------------------------------------------------------------------------
K_mf(TCheckResult)  TWkr_Tend::OnPkCheck  (TbxSes& Ses,const THead& pk,TSize recvSize,TSize* PacketSize)
{
   switch(pk.MessageType_.as_int()){
      case 0: *PacketSize = sizeof(TV010);
              return cpr_Continue;
      case 1: *PacketSize = sizeof(TV020);
              return cpr_Continue;
      case 2: *PacketSize = CtlHeadSize;
              return cpr_Continue;
      case 3: *PacketSize = CtlHeadSize;
              return cpr_Continue;
      case 5: *PacketSize = CtlHeadSize;
              return cpr_Continue;
   }
   return cpr_Error;
}
//---------------------------------------------------------------------------
K_mf(void)   TWkr_Tend::OnPkRecv  (TbxSes& Ses,const THead& pk,TReqPtr& ReqPtr)
{
   switch (pk.MessageType_.as_int()) {
      case 1: {                           //����E020�h�N������ƶ�JReqRec��,�æ^��bxSes
         TE020        E020 = *(TE020*)&pk;
         Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, &E020,sizeof(TE020)); 
         if(ReqPtr.get() == 0){
            Ses.GetMgr()->GetRecorder()->WriteLog(Ses.ID_, "Invalid Packet, not expected!"); 
            return;
         }
         TOrdHnd& OrdHnd = ReqPtr->GetOrdHnd();
         TStkOrder& StkOrder = OrdHnd.GetModify();
         StkOrder.LastTseCode_ = E020.StatusCode_.as_int();
         if(StkOrder.TseOrigTime_.IsInvalid()) {           //�Ĥ@������^��
            StkOrder.Pri_.assign(((double)E020.AfPri_.as_int())/100);
            StkOrder.TseOType_ = StkOrder.OType_;
            StkOrder.TseOrigTime_.assign(E020.OrderTime_.as_string());
         }
         StkOrder.TseLastTime_.assign(E020.OrderTime_.as_string());
         switch (E020.FunctionCode_.as_int()) {
            case 1:
               StkOrder.LastOCmd_ = cid_New;                   //�e�U
               break;
            case 2:
               StkOrder.LastOCmd_ = cid_Del;                   //�R��
               break;
            case 3:
               StkOrder.LastOCmd_ = cid_ChgQty;                //��q
               break;
            case 4:
               StkOrder.LastOCmd_ = cid_Qry;                   //�d��
               break;
            default:break;
         }
         if (StkOrder.LastOCmd_ != cid_Qry)
            StkOrder.BfQty_ = E020.BfQty_.as_int();
         StkOrder.AfQty_ = E020.AfQty_.as_int();
         Ses.OnWorkerFinished(pc_E020,*(THead*)(&E020));
         break;
      }
      case 3:                              //����E030,�h�q��bxSes�e�U����
         Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (&pk),sizeof(pk)); 
         Ses.OnWorkerFinished(pc_E030,pk);
         break;
      case 5:                              //����E050,�h�q��bxSes�e�U����
         Ses.GetMgr()->GetRecorder()->WriteAFSD(Ses.ID_, (&pk),sizeof(pk)); 
         Ses.OnWorkerFinished(pc_E050,pk);
         break;
   }
}
//---------------------------------------------------------------------------
K_mf(bool)   TWkr_Tend::SendReq    (TbxSes& Ses,TPacketCode code,TReqPtr& ReqPtr)
{
   switch(code){
      case pc_E010:{
         TE010 E010;
         TStkQty qty;
         TOrdHnd& OrdHnd = ReqPtr->GetOrdHnd();
         const TStkOrder* StkOrder = OrdHnd.GetConst();
         E010.SubSysName_ = Ses.GetSubSys();
         E010.MessageType_.assign("00");
         E010.MessageTime_.assign(TKTime::Now().as_string(false));
         E010.StatusCode_.assign("00");
         // ���M�wFunctionCode
         switch(ReqPtr->GetOrdCmdID()){
            case cid_New:
                 E010.FunctionCode_.assign("01");               //�R�i
                 qty = StkOrder->Qty_;
                 break;
            case cid_Del:
                 E010.FunctionCode_.assign("02");               //����
                 qty = StkOrder->BfQty_;
                 break;
            case cid_ChgQty:
                 E010.FunctionCode_.assign("03");                //���
                 qty = (static_cast<TReqChgQtyBase*>(ReqPtr.get()))->GetReqQty();
                 if(qty < 0){
                    int tmp = qty;
                    tmp = tmp * (-1); 
                    qty = tmp;
                 } else if(qty == 0)
                    qty = StkOrder->BfQty_; 
                 break;
            case cid_Qry:
                 E010.FunctionCode_.assign("04");                //�d��
                 qty = StkOrder->Qty_;
                 break;
            default:
                 return false;
         }
      
         // ����J�e�U���
         E010.BrokerId_.assign(StkOrder->IvacKey_.GetBrokerID().as_string());
         E010.PvcId_ = Ses.GetPvcID();
         E010.OrderNo_.assign(StkOrder->TseOrdID_.as_string(true));
         E010.IvacNo_.assign(StkOrder->IvacKey_.GetIvacNo().as_string(0,true));
         E010.StkNo_.assign(StkOrder->StkNo_.as_string());
         E010.Pri_.assign(StkOrder->Pri_.as_format(4,2));
         E010.Qty_.assign(qty.as_format(12,0));
         qty = StkOrder->Flags_;
         E010.StkSeqNo_.assign(qty.as_format(2,0));
         Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, &E010,sizeof(E010)); 
         return  Ses.SendPk(E010,sizeof(E010));
      }
      case pc_E040:{
         THead pk;
         pk.SubSysName_ = Ses.GetSubSys();
         pk.FunctionCode_.assign("00");
         pk.MessageTime_.assign(TKTime::Now().as_string(false));
         pk.StatusCode_.assign("00");
         pk.MessageType_.assign("02");
         Ses.GetMgr()->GetRecorder()->WriteBFSD(Ses.ID_, &pk,sizeof(pk));
         return  Ses.SendPk(pk,sizeof(pk));
      }
   }
}
//---------------------------------------------------------------------------

