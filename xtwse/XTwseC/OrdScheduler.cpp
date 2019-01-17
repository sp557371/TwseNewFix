//---------------------------------------------------------------------------
#ifdef __BORLANDC__
   #pragma hdrstop
   #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "OrdScheduler.h"
#include "TwStkOrder.h"
#include "SchedulerBase.cpp"
#include "MsgCenter.h"
#include "KIniFile.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw;
using namespace Kway::Tw::Stk;
//---------------------------------------------------------------------------
static char Msg_TNoPVC[]   = "1005";   //�L�i�νu��, �ɥ�system not ready                                 
static char Msg_ONoPVC[]   = "9105";   //�L�i�νu��, �ɥ�system not ready                                 
static char Msg_PVCBroken[]= "0403";   //�e�U���_�u, ���n�P bxOrderSes.cpp �����w�q�ۦP
//---------------------------------------------------------------------------
K_ctor  TScheduler::TScheduler(TOrdCenter& OrdCenter/*, TSysInfoPage* sip*/)
{
   OrdCenter_       = &OrdCenter;
   MarketPh_        = ts_Full;  // ���w�]����ѥ���ɬq
   Sender_          = 0;
   ORM_             = 0;
//   SysInfo_         = sip;
//   MarketPh_        = SysInfo_->GetMarketPh();
   LineState_.Reset();
   SetScheduler((TSchedulerBase*)this);
   TKIniFile IniFile(std::string(GetConfPath()+"/Main.ini").c_str());
   DebugLog_ = IniFile.ReadInt("Main","DebugLog",1);
   if(DebugLog_ >= 2){
      Log_.Open(std::string(GetProcLogPath()+"OrdSch.log").c_str(), (TFileMode)(fmRandomAccess|fmOpenAlways|fmCreatePath));
      Log_.Seek(0, fsEnd);
   }
   if(DebugLog_ >=3){
      QLog_.Open(std::string(GetProcLogPath()+"Queue.log").c_str(), (TFileMode)(fmRandomAccess|fmOpenAlways|fmCreatePath));
      QLog_.Seek(0, fsEnd);
   }
   std::string tmp     = IniFile.ReadString("Main","EnableAutoSwitch","N");
   EnableAutoSwitch_   = (tmp[0]=='Y' || tmp[0]=='y');
}
//---------------------------------------------------------------------------
K_dtor  TScheduler::~TScheduler()
{
   if(Log_.IsOpened())
      Log_.Close();
   if(QLog_.IsOpened())
      QLog_.Close();
   if(Sender_)
      Sender_->ResetScheduler();
   if(ORM_)
      ORM_->ResetScheduler();
   SetScheduler((TSchedulerBase*)0);
}
//---------------------------------------------------------------------------
K_mf(void) TScheduler::AddRec(TReqPtr& req,Stk::TMarket mkt,TOType ot, ETradeKind tk, eOrdPriority eop)
{
   req->Update("�e�U�ƶ���", of_Q_ing);
   if(tk == tk_Odd){
      switch(mkt){
         case m_Tse:             //�W��
            switch(eop){
               case 3:   TseQue_.OddJam_.push(req);        break;
               default:  TseQue_.Odd_.push(req);           break;
            }
            SetQS(m_Tse, tk_Odd, TseQue_.Odd_.size());
            break;
         case m_Otc:             //�W�d
            switch(eop){
               case 3:   OtcQue_.OddJam_.push(req);        break;
               default:  OtcQue_.Odd_.push(req);           break;
            }
            SetQS(m_Otc, tk_Odd, OtcQue_.Odd_.size());
            break;
         case m_OtcR:            //���d
            switch(eop){
               case 3:   OtcRQue_.OddJam_.push(req);        break;
               default:  OtcRQue_.Odd_.push(req);           break;
            }
            SetQS(m_OtcR, tk_Odd, OtcRQue_.Odd_.size());
            break;
      }
      return;
   }
   if(tk == tk_Fix){
      switch(mkt){
         case m_Tse:             //�W��
            TseQue_.Fix_.push(req);           break;
         case m_Otc:             //�W�d
            OtcQue_.Fix_.push(req);           break;
      }
      return;
   }
   switch(ot){
      case ot_Auct:
         switch(mkt){
            case m_Tse: TseQue_.Auct_.push(req); SetQS(m_Tse, tk_Auct, TseQue_.Auct_.size());    break;
            case m_Otc: OtcQue_.Auct_.push(req); SetQS(m_Otc, tk_Auct, OtcQue_.Auct_.size());    break;
            default:     break;
         }
         break;
      case ot_Lend:
         switch(mkt){
            case m_Tse: TseQue_.Lend_.push(req); SetQS(m_Tse, tk_Lend, TseQue_.Lend_.size());    break;
            case m_Otc: OtcQue_.Lend_.push(req); SetQS(m_Otc, tk_Lend, OtcQue_.Lend_.size());    break;
            default:     break;
         }
         break;
      case ot_Tend:
         switch(mkt){
            case m_Tse: TseQue_.Tend_.push(req); SetQS(m_Tse, tk_Tend, TseQue_.Tend_.size());    break;
            case m_Otc: OtcQue_.Tend_.push(req); SetQS(m_Otc, tk_Tend, OtcQue_.Tend_.size());    break;
            default:     break;
         }
         break;
      default:
         switch(mkt){
            case m_Tse:             //�W��
               TseQue_.Normal_.push(req);
               SetQS(m_Tse, tk_Normal, TseQue_.Normal_.size()+TseQue_.High_.size());
               break;
            case m_Otc:             //�W�d
               switch(eop){
                  case 0:   OtcQue_.High_.push(req);       break;
                  case 1:   OtcQue_.Normal_.push(req);     break;
                  case 2:   OtcQue_.Low_.push(req);        break;
                  case 3:   OtcQue_.Jam_.push(req);        break;
               }
               SetQS(m_Otc, tk_Normal, OtcQue_.Normal_.size()+OtcQue_.High_.size());
               break;
            case m_OtcR:
               switch(eop){
                  case 0:   OtcRQue_.High_.push(req);      break;
                  case 1:   OtcRQue_.Normal_.push(req);    break;
                  case 2:   OtcRQue_.Low_.push(req);       break;
                  case 3:   OtcRQue_.Jam_.push(req);       break;
               }
               SetQS(m_OtcR, tk_Normal, OtcRQue_.Normal_.size()+OtcRQue_.High_.size());
               break;          //���d
         }
         break;
   }
}
//---------------------------------------------------------------------------
K_mf(TOrdFlow::TReqPtr) TScheduler::GetOrderFromQ(TQueue& que, Stk::TMarket mkt, ETradeKind tk)
{
   //����Priority�����ˬdHigh->Normal->Jam
   switch(tk){
      case tk_Normal:
         if(!que.Normal_.empty()){
            TOrdFlow::TReqPtr   req = que.Normal_.front();
            que.Normal_.pop(); 
            return req;
         }
         return TReqPtr(0);  
      case tk_Odd:
         if(!que.Odd_.empty()){
            TOrdFlow::TReqPtr   req = que.Odd_.front();
            que.Odd_.pop(); 
            return req;
         }else if(!que.OddJam_.empty()){
            TOrdFlow::TReqPtr   req = que.OddJam_.front();
            const TStkOrder* StkOrder;
            if(req.get())
               StkOrder = req->GetOrdHnd().GetConst();
            if(LineState_.GetL(mkt ,tk) == 0 || (StkOrder && StkOrder->FlowID_.GetFlow() >= of_Sent)){
               if(LineState_.GetL(mkt ,tk) == 0 && (StkOrder && StkOrder->LastTseCode_ == 199))
                  req->Update(std::string(GetErrMsg(Msg_PVCBroken)),of_Sent);
               que.OddJam_.pop();    //�B�z��������
               if(!que.OddJam_.empty())
                  return  que.OddJam_.front();
               else
                  return TReqPtr(0);
            }else
               return TReqPtr(0);
         }else
            return TReqPtr(0);                      //�ثe�S���s�e�U
         break;
      case tk_Fix:
         if(!que.Fix_.empty()){
            TOrdFlow::TReqPtr   req = que.Fix_.front();
            que.Fix_.pop(); 
            return req;
         }else
            return TReqPtr(0);                      //�ثe�S���s�e�U
         break;
      case tk_Auct:
         if(!que.Auct_.empty()){
            TOrdFlow::TReqPtr   req = que.Auct_.front();      //���X�ƶ������Ĥ@���e�U
            que.Auct_.pop();      //������Y�q�ƶ�������
            SetQS(mkt, tk_Auct, que.Auct_.size());
            return req;
         }
         break;
      case tk_Lend:
         if(!que.Lend_.empty()){
            TOrdFlow::TReqPtr   req = que.Lend_.front();      //���X�ƶ������Ĥ@���e�U
            que.Lend_.pop();      //������Y�q�ƶ�������
            SetQS(mkt, tk_Lend, que.Lend_.size());
            return req;
         }
         break;
      case tk_Tend:
         if(!que.Tend_.empty()){
            TOrdFlow::TReqPtr   req = que.Tend_.front();      //���X�ƶ������Ĥ@���e�U
            que.Tend_.pop();     //������Y�q�ƶ�������
            SetQS(mkt, tk_Tend, que.Tend_.size());
            return req;
         }
         break;
   }
   return TReqPtr(0);                      //�ثe�S���s�e�U
}
//---------------------------------------------------------------------------
K_mf(bool)      TScheduler::OnOrdReq(TReqPtr req)   // �p�G������\���,�n�p���קK�N�O���D���e
{                                                   // �L�Ӫ��e�U�S���^�e ??    930725
   const TStkOrder* StkOrder = req->GetOrdHnd().GetConst();
   if(DebugLog_ >= 2){
      char buf[512];
      sprintf(buf,"%s Cmd= %s, OrderNo = %s\n",TKDateTimeMS::Now().as_string(TKDate::sf_ELong).c_str(),
                                            req->GetOrdCmdID().as_string().c_str(),
                                            StkOrder->TseOrdID_.as_string().c_str());
      Log_.Write(buf);
   }
   ETradeKind tk;
   eOrdPriority eop = eop_Normal;
   TMarket mkt = StkOrder->Market_;
   switch(StkOrder->OType_){
      case 'A': tk = tk_Auct;   break;
      case 'L': tk = tk_Lend;   break;
      case 'T': tk = tk_Tend;   break;
      default : 
         switch(StkOrder->TrxSegment_){
            case ts_Full: tk = tk_Normal; break;
            case ts_Odd : tk = tk_Odd;    break;
            case ts_Fix : tk = tk_Fix;    break;
            default:      tk = tk_Normal; break;
         }
   }
   if(LineState_.GetQ(mkt ,tk) == 0){                // �L�ƶ����e�U
      if(Sender_->NewOrderAlert(mkt , req, tk))      // �N��䦳�S���{�����Žu���i�ǰe�e�U
         return true;
   }                               
   if(LineState_.GetL(mkt ,tk)){                     // �ۤv���u���i�Φ����u�� 
      AddRec(req,mkt,StkOrder->OType_, tk, eop);     // �N�ƶ��C�C���ۧa
      if(DebugLog_ >= 3){
         char buf[512];
         sprintf(buf,"%s OrderNo = %s QSize = %d\n",TKDateTimeMS::Now().as_string(TKDate::sf_ELong).c_str(),
                                      StkOrder->TseOrdID_.as_string().c_str(),TseQue_.Normal_.size());
         QLog_.Write(buf);
      }
      return true;
   }else if(EnableAutoSwitch_){                      // �p�G�ۤv�S���u���i��,�����\�۰ʤ���
// 20100701 �L�i�νu���ɥ����䴩�۰ʤ����u���\��
      AddRec(req,mkt,StkOrder->OType_, tk, eop);
      if(DebugLog_ >= 3){
         char buf[512];
         sprintf(buf,"%s OrderNo = %s QSize = %d\n",TKDateTimeMS::Now().as_string(TKDate::sf_ELong).c_str(),
                                      StkOrder->TseOrdID_.as_string().c_str(),TseQue_.Normal_.size());
         QLog_.Write(buf);
      }
      req->Update(std::string("XTwse:�u���۰ʤ�����,�еy�J...!"),of_Q_ing);   
      return true;
   }
   TStkOrder& ord = req->GetOrdHnd().GetModify();
   if(req->GetOrdCmdID() == cid_New){
      ord.LastTseCode_ = 255;
   }else{   
      ord.LastOCmd_ = req->GetOrdCmdID();
   }
   ord.LastTseCode_ = 05;
   req->Update(std::string(GetErrMsg(mkt==m_Tse?Msg_TNoPVC:Msg_ONoPVC)),
               req->GetOrdCmdID()==cid_New?of_AbortOrdClear:of_AbortOrdNoChange);    // �S�u��, ���a!
   return false;
}
//---------------------------------------------------------------------------
K_mf(TOrdFlow::TReqPtr)   TScheduler::GetOrderByMkt(Stk::TMarket mkt, ETradeKind tk)
{
   TOrdFlow::TReqPtr req;
   switch(mkt){
      case m_Tse:
         req = GetOrderFromQ(TseQue_, mkt, tk);
         if(tk == tk_Normal)
            SetQS(m_Tse, tk_Normal, TseQue_.Normal_.size()+TseQue_.High_.size());
         else if(tk == tk_Odd)
            SetQS(m_Tse, tk_Odd, TseQue_.Odd_.size());
         else if(tk == tk_Fix)
            SetQS(m_Tse, tk_Fix, TseQue_.Fix_.size());
         break;
      case m_Otc:
         req = GetOrderFromQ(OtcQue_, mkt, tk);
         if(tk == tk_Normal)
            SetQS(m_Otc, tk_Normal, OtcQue_.Normal_.size()+OtcQue_.High_.size());
         else if(tk == tk_Odd)
            SetQS(m_Otc, tk_Odd, OtcQue_.Odd_.size());
         else if(tk == tk_Fix)
            SetQS(m_Otc, tk_Fix, OtcQue_.Fix_.size());
         break;
      case m_OtcR:
         req = GetOrderFromQ(OtcRQue_, mkt, tk);
         if(tk == tk_Normal || tk == tk_Fix)
            SetQS(m_OtcR, tk_Normal, OtcRQue_.Normal_.size()+OtcRQue_.High_.size());
         else if(tk == tk_Odd)
            SetQS(m_OtcR, tk_Odd, OtcRQue_.Odd_.size());
         break;
      default:
         req = TReqPtr(0);
         break;
   }
   return req;
}
//---------------------------------------------------------------------------
K_mf(bool) TScheduler::SetMarketPh(TTrxSegment mph)
{
   if (MarketPh_ > mph && mph != ts_Odd)
      return false;
   if(MarketPh_ < mph){
      if(Sender_)
         Sender_->SetMarketPh(mph);
      MarketPh_ = mph;
   }
   return true;
}
//---------------------------------------------------------------------------
K_mf(void) TScheduler::SetMarketPh(TSegDefs* segs)
{
   SegDefs_ = *segs;
   if(Sender_)
      Sender_->SetMarketPh(segs);
}
//---------------------------------------------------------------------------
K_mf(void) TScheduler::SetLineState(Stk::TMarket mkt, ETradeKind tk, int sz)
{
   Kway::word TotLine = 0;
   if(sz > -1){ // -1 ���¶��Ҳ��_�u, �ݭn���s��¶��Ҳը��u���ƶq
      TotLine = sz;
      SetLS(mkt, tk, sz);
   }else
      TotLine = LineState_.GetL(mkt, tk);
   SetTMPReady(LineState_.GetAll() > 0);
   if(sz > -1){     // �p�G�O�������u���Ҥ޵o���ƥ�~�i��U�C�u�@ 
      if(sz == 0){    //�����\�N��󦹵��e�U
         Stk::TOrdFlow::TReqPtr req;
         req = GetOrderByMkt(mkt,tk);
         bool Is199;
         while(req.get() != 0){
            if(req->GetOrdCmdID() == cid_New){
               TStkOrder& ord = req->GetOrdHnd().GetModify();
               if(ord.LastTseCode_ == 199)
                  Is199 = true;
               else
                  Is199 = false;
               ord.LastTseCode_ = 5;
            }
            if(Is199)
               req->Update(std::string(GetErrMsg(Msg_PVCBroken)),of_Sent);
            else
               req->Update(std::string(GetErrMsg(mkt==m_Tse?Msg_TNoPVC:Msg_ONoPVC)),
                                       req->GetOrdCmdID()==cid_New?of_AbortOrdClear:of_AbortOrdNoChange);
            req = GetOrderByMkt(mkt,tk);
         }
      }
   }
}
//---------------------------------------------------------------------------
K_mf(void) TScheduler::DoAutoReq(TReqPtr req, TMarket mkt, ETradeKind tk)
{
   if(ORM_ && ORM_->IsUseORD())                       // �p�G�t�γ]�w�n�i�����
      if(ORM_->SendOrd(req, mkt, tk, LineState_.GetRatio(mkt, tk))) // �N�e��¶��Ҳ�
         return;
   const TStkOrder* StkOrder = req->GetOrdHnd().GetConst();
   AddRec(req,mkt,StkOrder->OType_, tk, (req->IsJamStk()?eop_Jam:eop_High));
}
//---------------------------------------------------------------------------
K_mf(void) TScheduler::DoReOrder(TReqPtr req, TMarket mkt, ETradeKind tk)
{
   if(Sender_->NewOrderAlert(mkt, req, tk))
      return;
   const TStkOrder* StkOrder = req->GetOrdHnd().GetConst();
   AddRec(req,mkt,StkOrder->OType_, tk, (req->IsJamStk()?eop_Jam:eop_High));
}
//---------------------------------------------------------------------------
