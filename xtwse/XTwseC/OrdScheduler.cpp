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
static char Msg_TNoPVC[]   = "1005";   //無可用線路, 借用system not ready                                 
static char Msg_ONoPVC[]   = "9105";   //無可用線路, 借用system not ready                                 
static char Msg_PVCBroken[]= "0403";   //委託中斷線, 須要與 bxOrderSes.cpp 內的定義相同
//---------------------------------------------------------------------------
K_ctor  TScheduler::TScheduler(TOrdCenter& OrdCenter/*, TSysInfoPage* sip*/)
{
   OrdCenter_       = &OrdCenter;
   MarketPh_        = ts_Full;  // 先預設為整股交易時段
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
   req->Update("委託排隊中", of_Q_ing);
   if(tk == tk_Odd){
      switch(mkt){
         case m_Tse:             //上市
            switch(eop){
               case 3:   TseQue_.OddJam_.push(req);        break;
               default:  TseQue_.Odd_.push(req);           break;
            }
            SetQS(m_Tse, tk_Odd, TseQue_.Odd_.size());
            break;
         case m_Otc:             //上櫃
            switch(eop){
               case 3:   OtcQue_.OddJam_.push(req);        break;
               default:  OtcQue_.Odd_.push(req);           break;
            }
            SetQS(m_Otc, tk_Odd, OtcQue_.Odd_.size());
            break;
         case m_OtcR:            //興櫃
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
         case m_Tse:             //上市
            TseQue_.Fix_.push(req);           break;
         case m_Otc:             //上櫃
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
            case m_Tse:             //上市
               TseQue_.Normal_.push(req);
               SetQS(m_Tse, tk_Normal, TseQue_.Normal_.size()+TseQue_.High_.size());
               break;
            case m_Otc:             //上櫃
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
               break;          //興櫃
         }
         break;
   }
}
//---------------------------------------------------------------------------
K_mf(TOrdFlow::TReqPtr) TScheduler::GetOrderFromQ(TQueue& que, Stk::TMarket mkt, ETradeKind tk)
{
   //↓依Priority順序檢查High->Normal->Jam
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
               que.OddJam_.pop();    //處理完畢移除
               if(!que.OddJam_.empty())
                  return  que.OddJam_.front();
               else
                  return TReqPtr(0);
            }else
               return TReqPtr(0);
         }else
            return TReqPtr(0);                      //目前沒有新委託
         break;
      case tk_Fix:
         if(!que.Fix_.empty()){
            TOrdFlow::TReqPtr   req = que.Fix_.front();
            que.Fix_.pop(); 
            return req;
         }else
            return TReqPtr(0);                      //目前沒有新委託
         break;
      case tk_Auct:
         if(!que.Auct_.empty()){
            TOrdFlow::TReqPtr   req = que.Auct_.front();      //取出排隊中的第一筆委託
            que.Auct_.pop();      //取完後即從排隊中移除
            SetQS(mkt, tk_Auct, que.Auct_.size());
            return req;
         }
         break;
      case tk_Lend:
         if(!que.Lend_.empty()){
            TOrdFlow::TReqPtr   req = que.Lend_.front();      //取出排隊中的第一筆委託
            que.Lend_.pop();      //取完後即從排隊中移除
            SetQS(mkt, tk_Lend, que.Lend_.size());
            return req;
         }
         break;
      case tk_Tend:
         if(!que.Tend_.empty()){
            TOrdFlow::TReqPtr   req = que.Tend_.front();      //取出排隊中的第一筆委託
            que.Tend_.pop();     //取完後即從排隊中移除
            SetQS(mkt, tk_Tend, que.Tend_.size());
            return req;
         }
         break;
   }
   return TReqPtr(0);                      //目前沒有新委託
}
//---------------------------------------------------------------------------
K_mf(bool)      TScheduler::OnOrdReq(TReqPtr req)   // 如果有分單功能時,要如何避免將別的主機送
{                                                   // 過來的委託又往回送 ??    930725
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
   if(LineState_.GetQ(mkt ,tk) == 0){                // 無排隊中委託
      if(Sender_->NewOrderAlert(mkt , req, tk))      // 就找找有沒有現成的空線路可傳送委託
         return true;
   }                               
   if(LineState_.GetL(mkt ,tk)){                     // 自己有線路可用但忙線時 
      AddRec(req,mkt,StkOrder->OType_, tk, eop);     // 就排隊慢慢等著吧
      if(DebugLog_ >= 3){
         char buf[512];
         sprintf(buf,"%s OrderNo = %s QSize = %d\n",TKDateTimeMS::Now().as_string(TKDate::sf_ELong).c_str(),
                                      StkOrder->TseOrdID_.as_string().c_str(),TseQue_.Normal_.size());
         QLog_.Write(buf);
      }
      return true;
   }else if(EnableAutoSwitch_){                      // 如果自己沒有線路可用,但允許自動切換
// 20100701 無可用線路時必須支援自動切換線路功能
      AddRec(req,mkt,StkOrder->OType_, tk, eop);
      if(DebugLog_ >= 3){
         char buf[512];
         sprintf(buf,"%s OrderNo = %s QSize = %d\n",TKDateTimeMS::Now().as_string(TKDate::sf_ELong).c_str(),
                                      StkOrder->TseOrdID_.as_string().c_str(),TseQue_.Normal_.size());
         QLog_.Write(buf);
      }
      req->Update(std::string("XTwse:線路自動切換中,請稍侯...!"),of_Q_ing);   
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
               req->GetOrdCmdID()==cid_New?of_AbortOrdClear:of_AbortOrdNoChange);    // 沒線路, 放棄吧!
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
   if(sz > -1){ // -1 表示繞單模組斷線, 需要重新對繞單模組取線路數量
      TotLine = sz;
      SetLS(mkt, tk, sz);
   }else
      TotLine = LineState_.GetL(mkt, tk);
   SetTMPReady(LineState_.GetAll() > 0);
   if(sz > -1){     // 如果是本機的線路所引發的事件才進行下列工作 
      if(sz == 0){    //不成功就丟棄此筆委託
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
   if(ORM_ && ORM_->IsUseORD())                       // 如果系統設定要進行分單
      if(ORM_->SendOrd(req, mkt, tk, LineState_.GetRatio(mkt, tk))) // 就送到繞單模組
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
