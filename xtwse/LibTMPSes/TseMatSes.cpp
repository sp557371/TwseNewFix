//---------------------------------------------------------------------------
#include "TwStkOrder.h"
#include "TseMatSes.h"
//===================================================================
using namespace Kway;
using namespace Kway::Tw::Stk;
using namespace Kway::Tw::bx;
//===================================================================
K_ctor TTseMatSes::TTseMatSes(TbxSesMgr* sm, TMarket mkt)
   :inherited(sm)
{
   Timer_       = new TRptTimer(this);
   Ready_  = false;
   Sending_= false;
   RptTimeOut_ = 120;
   SetMarket(mkt);
   if(mkt == Stk::m_Tse)
      SubSys_.assign("50");
   else
      SubSys_.assign("95");
   SeqNo_ = 0;
   if(MgrPtr_ != 0){
      OrdCenterPtr_ = MgrPtr_->GetOrdCenter();
   }
//   SetApCode(TApCode(std::string("3")));
}
//===================================================================
K_dtor TTseMatSes::~TTseMatSes()
{
   Timer_->Stop();
   OnStateChanged("連線中斷!");
   delete Timer_;
   DelSubSysReg();
}
//===================================================================
K_mf(void)  TTseMatSes::OnTimer  (TTimerBase*)
{
   Timer_->Stop();
   SetState(sp_WaitLinkReady,"Sending L010!");
   TTseControlHead L010_;
   L010_.SubSysName_.assign(Market_ == m_Tse?"10":"91");
   L010_.FunctionCode_.assign("10");
   L010_.MessageType_.assign("00");
   L010_.MessageTime_.assign(TKTime::Now().as_string(false));
   L010_.StatusCode_.assign("00");
   SendPkt((TbxRec&)L010_);
}
//---------------------------------------------------------------------------
K_mf(void) TTseMatSes::OnSubSysReady()
{
   SetState(sp_StartRptSys,"Starting Rpt System");
   if(Market_ == Stk::m_Tse)
      SubSys_.assign("50");
   else
      SubSys_.assign("95");
   TR1 R1(GetSubSys());
   char buf[16];
   R1.MessageTime_.assign(TKTime::Now().as_string(false));
   R1.BrokerID_.assign(GetBrokerID().as_string());
   sprintf(buf,"%06d",MgrPtr_->GetBrkSeq(TBrokerID(BrokerID_.as_string()), Market_));
   R1.StartSeq_.assign(buf);
   SendPkt((TbxRec&)R1);
   char Buf[128];
   sprintf(Buf,"%s/PMach.%s",GetProcLogPath().c_str(),(Market_ == Stk::m_Tse?"T":"O"));
   PMach_.Open(Buf, (TFileMode)(fmRandomAccess|fmOpenAlways|fmCreatePath));
   PMach_.Seek(0, fsEnd);
   if(GetExcMgr())
      GetExcMgr()->Alert("XX1001",SesTermID_);
}
//---------------------------------------------------------------------------
K_mf(void)    TTseMatSes::DelSubSysReg()
{
   PMach_.Close();
}
//---------------------------------------------------------------------------
K_mf(void) TTseMatSes::OnPktRecv(TbxRec& pkt)
{
   Timer_->Stop();
   Sending_ = false;
   int sz = GetPacketSize(&pkt);
   GetMgr()->GetRecorder()->WriteAFSD(ID_, &pkt, sz >= 82? 20 : sz); 
   switch (pkt.SubSysName_.as_int()){
      case 10:
      case 91:{
         if(pkt.StatusCode_.as_int() > 0){
            bool ErrCnt = false;
            std::string ErrMsg = MgrPtr_->GetMsg(pkt.SubSysName_.as_string(),pkt.StatusCode_.as_string(),ErrCnt);
            if(GetExcMgr())
               GetExcMgr()->Alert("XX1002",SesTermID_,ErrMsg);
            StartTimer(60);
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
               sprintf(buf,"%s-Ready!",ApCode_.as_string().c_str());
               OnStateChanged(buf);
               SendPkt((TbxRec&)L160_);
               Ready_ = true;
               OnSubSysReady();
               return;
            }
            default: return;
         }
         return;
      }
      case 50:
      case 95:{
         switch(pkt.FunctionCode_.as_int()){
            case 00:{ 
               switch(pkt.MessageType_.as_int()){
                  case 1:
                     SetState(sp_WaitForRpt,"成交-Waitting!");
		     break;	
                  case 4:{
                     TR5 R5(GetSubSys());
                     R5.MessageTime_.assign(TKTime::Now().as_string(false));
                     SendPkt((TbxRec&)R5);
                     break;
                  }
                  default: break;
               }
               break;
            }
            case 10:{
               TR3 R3 = *(TR3*)&pkt;
               char buf[128];
               Kway::Tw::Stk::TOrdCenter::TRpt::TDeal rpt;
               int BodyCount = R3.BodyCount_.as_int();
               if(BodyCount <= 0 || OrdCenterPtr_ == 0)
                  break;
               static EOType  otypeidx[] = { ot_Gn, ot_CrA, ot_DbA, ot_CrS, ot_DbS,
                                             ot_Tse5, ot_Tse6 };
               sprintf(buf,"%-62s",TKDateTimeMS::Now().as_string(TKDate::sf_ELong).c_str());
               PMach_.Write(buf,sizeof(TRptBody));
               if(MgrPtr_->IsPMachLN())
                  PMach_.Write("\n",1);
               for(int i = 0;i < BodyCount;++i){
                  PMach_.Write(&R3.Body_[i], sizeof(TRptBody));
                  if(MgrPtr_->IsPMachLN())
                     PMach_.Write("\n",1);
                  rpt.TseOrdKey_.BrokerID_.assign(R3.Body_[i].BrokerID_.as_string());
                  rpt.TseOrdKey_.Market_ = GetMarket();
                  rpt.TseOrdKey_.TseOrdID_.assign(R3.Body_[i].OrderNo_.as_string());
// 20110516 過濾非由本TMP Handler 進行新單委託的成交回報資料
                  TOrdHnd ordhnd = OrdCenterPtr_->GetOrdHnd(rpt.TseOrdKey_, TStkQty(0), uf_OrdFlow, TSysOrdID(0));
                  if(ordhnd.is_invalid())
                     continue;
                  else{
                  	const TStkOrder* ord = ordhnd.GetConst();
                  	if(ord->Qty_.is_0())
                  	   continue;
                  }
                  rpt.StkNo_.assign(R3.Body_[i].StkNo_.as_string());
                  rpt.Qty_ = R3.Body_[i].MthQty_.as_int();
                  rpt.Pri_ = TStkPri(R3.Body_[i].MthPri_.as_int(),2);
                  rpt.TseTime_.assign(R3.Body_[i].MthTime_.as_string());
                  rpt.TimeMS_ = R3.Body_[i].MthTimems_.as_int() * 10;
                  rpt.BSCode_.assign(R3.Body_[i].BSCode_.as_string());
                  rpt.IvacNo_.assign(R3.Body_[i].IvacNo_.as_string());
                  rpt.OType_.assignT(otypeidx[R3.Body_[i].OrderType_.as_int()]);
                  rpt.BSeqNo_ = R3.Body_[i].SeqNo_.as_int();
                  rpt.MSeqNo_ = R3.Body_[i].RecNo_.as_int();
                  OrdCenterPtr_->RptDeal(rpt/*, soid*/);
               }
               SeqNo_ = R3.Body_[BodyCount-1].SeqNo_.as_int();
               MgrPtr_->SetBrkSeq(TBrokerID(BrokerID_.as_string()), Market_, SeqNo_);
//               sprintf(buf,"成交-%06d",SeqNo_);
//               SetState(sp_WaitForRpt,buf);
               return;
            }
            case 20:{
               TR6 R6 = *(TR6*)&pkt;
               if(GetExcMgr())
                  GetExcMgr()->Alert("XX1013",std::string(Market_==Stk::m_Tse?"TSE":"OTC")+" executions-"+R6.TotalRec_.as_string());
               return;
            }
            default: return;
         }
         return;
      }
   } // switch (pkt.SubSysName_....
}
//===================================================================
K_mf(void) TTseMatSes::OnMesLinkReady(TMesBase* M_)
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
}
//===================================================================
K_mf(void) TTseMatSes::OnMesLinkBroken(TMesBase*, const std::string&)
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
K_mf(void) TTseMatSes::SendPkt(TbxRec& pkt)
{
	Sending_ = true;
   if(Ready_ == false)
      StartTimer(5);
   GetMgr()->GetRecorder()->WriteBFSD(ID_, &pkt, GetPacketSize(&pkt)); 
	Send(pkt);
}
//---------------------------------------------------------------------------
K_mf(void)  TTseMatSes::StartTimer  (size_t sec)
{
   Timer_->Stop();
   Timer_->Start(sec*1000, &this_type::OnTimer);
}
//---------------------------------------------------------------------------
