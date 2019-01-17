#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxRptSes.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw;
using namespace Kway::Tw::bx;
using namespace Kway::Tw::Stk;
/////////////////////////////////////////////////////////////////////////////
/*
   ����:����^���Ҳլ�XTRADE���ɮ׶ǿ�t�Φ@�Ϊ��Ҳ�,���p�G�M�ɮ׶ǿ�P�ɨϥή�,�K�|������^���ɺ|�����D
        �M�ɮ׶ǿ�P�ɨϥή�,�������w�q __bxFT �o��condition
        �o�إHPMap �O���C�@������Ǹ�,�����ɺ|�̾�
        �ɺ|�n�D�C���̦h200��,�Y5�������������ɺ|�h���s�ˬdPMap���L�|�^��,���h�Ұʸɺ|
*/
K_ctor  TbxRptSes::TbxRptSes  (TbxSesMgr* SesMgr,Stk::TMarket mkt)
   :
#ifdef __bxFT
   RecoverTimer_(this),
   Recovering_(false),
#endif
   inherited(SesMgr)
{
   SetRptTimeOut(60000);
   SetMarket(mkt);
   if(mkt == Stk::m_Tse)
      SubSys_.assign("50");
   else
      SubSys_.assign("95");
   SeqNo_ = 0;
   //�]������^���i�঳�h�a���u��,�ҥH�g�ɳ�����bxRecorder����o�سB�z
   if(MgrPtr_ != 0){
      OrdCenterPtr_ = MgrPtr_->GetOrdCenter();
   }
   SetApCode(TApCode(std::string("3")));
   AddWorkers();
}
//---------------------------------------------------------------------------
K_dtor TbxRptSes::~TbxRptSes  ()
{
   SetState(sp_Destroying, "Destroying");
#ifdef __bxFT
   if(MgrPtr_ != 0)
      MgrPtr_->DelRptSes(this);
#endif
   PMach_.Close();
}
//---------------------------------------------------------------------------
K_mf(void)    TbxRptSes::OnSubSysReady()
{
   SetState(sp_StartRptSys,"Starting Rpt System");
   TKey key;
   TWorker* worker;
   if(Market_ == Stk::m_Tse)
      SubSys_.assign("50");
   else
      SubSys_.assign("95");
   key.assign(SubSys_.as_string()+"00");
   worker = Workers_.GetWorker(key);
   if(worker != 0){
//      worker->SendReq(*this,pc_R1,SeqNo_);          //�e�XR1
      worker->SendReq(*this,pc_R1,MgrPtr_->GetBrkSeq(TBrokerID(BrokerID_.as_string()), Market_));          //�e�XR1
   }
   StartTimer(RptTimeOut_);
   char Buf[128];
   sprintf(Buf,"%s/PMach.%s",GetProcLogPath().c_str(),(Market_ == Stk::m_Tse?"T":"O"));
   PMach_.Open(Buf, (TFileMode)(fmRandomAccess|fmOpenAlways|fmCreatePath));
   // �p�G�n���榨��^���ɺ|���ܥ����n���ɮ׶ǿ骺�u��;�b�o�ثإߩҦ�����^�����O��,�@��
   // �ɺ|�^�����̾�
#ifdef __bxFT
   TRptBody RptBody;
   while(!PMach_.IsEOF()){
      PMach_.Read(&RptBody, sizeof(TRptBody));
      PMap_.insert(std::make_pair(RptBody.SeqNo_.as_int(), RptBody.SeqNo_.as_int()));
   }
   SeqNo_ =  RptBody.SeqNo_.as_int(); //�̫�@������^������ӧǸ�
   if(MgrPtr_ != 0)
      MgrPtr_->RegRptSes(this);
#endif
   PMach_.Seek(0, fsEnd);
   if(GetExcMgr())
      GetExcMgr()->Alert("XX1001",ID_.as_string());
}
//---------------------------------------------------------------------------
K_mf(void)    TbxRptSes::ReStartRptSys(TSize SeqNo)
{
   SetState(sp_StartRptSys,"ReStarting Rpt System");
   TKey key;
   TWorker* worker;
   key.assign(SubSys_.as_string()+"00");
   worker = Workers_.GetWorker(key);
   if(worker != 0){
      worker->SendReq(*this,pc_R1,SeqNo);          //�e�XR1
   }
   StartTimer(RptTimeOut_);
}
//---------------------------------------------------------------------------
K_mf(void)    TbxRptSes::OnTimer           (TTimerBase*)
{
   StopTimer   ();
   switch(State_){
        case sp_WaitLinkReady:
                ReStartLink();
                break;
        case sp_WaitLoginReady:
                ReStartLink();
                break;
        case sp_WaitSubSysReady:
                ReStartLink();
                break;
        case sp_StartRptSys:
                ReStartRptSys(SeqNo_);
        case sp_WaitForRpt:
                StartTimer(RptTimeOut_);
                break;
        case sp_RecievingRpt:
                StartTimer(RptTimeOut_);
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
K_mf(void)    TbxRptSes::OnWorkerFinished(TPacketCode PkCode,const THead& pk)
{
   StopTimer   ();
   switch(PkCode){
        case pc_R1:                     //���O���i�ব��R1
                break;
        case pc_R2:                     //����R2,���Ա�������^��
                SetState(sp_WaitForRpt,"����-Waiting!");
                break;
        case pc_R3:{
                SetState(sp_RecievingRpt,"����-Recieving!");
                TR3 R3 = *(TR3*)&pk;
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
                   rpt.StkNo_.assign(R3.Body_[i].StkNo_.as_string());
                   rpt.Qty_ = R3.Body_[i].MthQty_.as_int();
                   rpt.Pri_.assign((double)(R3.Body_[i].MthPri_.as_int())/100);
                   rpt.TseTime_.assign(R3.Body_[i].MthTime_.as_string());
                   rpt.TimeMS_ = R3.Body_[i].MthTimems_.as_int() * 10;
                   rpt.BSCode_.assign(R3.Body_[i].BSCode_.as_string());
                   rpt.TseOrdKey_.TseOrdID_.assign(R3.Body_[i].OrderNo_.as_string());
                   rpt.IvacNo_.assign(R3.Body_[i].IvacNo_.as_string());
                   rpt.BSeqNo_ = R3.Body_[i].SeqNo_.as_int();
                   rpt.MSeqNo_ = R3.Body_[i].RecNo_.as_int();
                   rpt.OType_.assignT(otypeidx[R3.Body_[i].OrderType_.as_int()]);
//                   Kway::word m1 = TKTimeMS::Now().GetMS();
//                   TSysOrdID soid = MgrPtr_->GetBOS(rpt.TseOrdKey_.BrokerID_, rpt.TseOrdKey_.TseOrdID_);
//                   fprintf(stderr,"soid = %d\n",(Kway::dword)soid);
                   OrdCenterPtr_->RptDeal(rpt/*, soid*/);
//                   Kway::word m2 = TKTimeMS::Now().GetMS();
//                   fprintf(stderr,"m1 = %d, m2 = %d, m2-m1 = %d\n", m1, m2, m2-m1);
#ifdef __bxFT
                   PMap_.insert(std::make_pair(R3.Body_[i].SeqNo_.as_int(), R3.Body_[i].SeqNo_.as_int()));
                   if(R3.Body_[i].SeqNo_.as_int() > (SeqNo_ + 1)) // �p�G�Ǹ����s��,�N�Ұʸɺ|
                      CallRecover(SeqNo_ +1, R3.Body_[i].SeqNo_.as_int()-1);
#endif
//                   SeqNo_ = R3.Body_[i].SeqNo_.as_int();
                }
                // 20080519 �ܧ󦨥�^���_�l�Ǹ����k
                SeqNo_ = R3.Body_[BodyCount-1].SeqNo_.as_int();
                MgrPtr_->SetBrkSeq(TBrokerID(BrokerID_.as_string()), Market_, SeqNo_);
                
                SetState(sp_WaitForRpt,"����-Waiting!");
                StartTimer(RptTimeOut_);
                break;
                }
        case pc_R4:
                {
                if(State_ != sp_WaitForRpt){
                   ReStartLink(); 
                   return;
                }
                TKey key;
                TWorker* worker;
                key.assign(SubSys_.as_string()+"00");
                worker = Workers_.GetWorker(key);
                if(worker != 0)
                   worker->SendReq(*this,pc_R5,0);          //�^��R5
                SetState(sp_WaitForRpt,"����-Waiting!");
                StartTimer(RptTimeOut_);
                break;
                }
        case pc_R5:
                StartTimer(RptTimeOut_);
                break;
        case pc_R6:
                TR6 R6 = *(TR6*)&pk;
                GetMgr()->GetRecorder()->WriteAFSD(ID_,&R6,sizeof(TR6));
                if(GetExcMgr())
                   GetExcMgr()->Alert("XX1013",std::string(Market_==Stk::m_Tse?"TSE":"OTC")+" executions-"+R6.TotalRec_.as_string());
                break;
   }
}
//---------------------------------------------------------------------------
K_mf(void)    TbxRptSes::AddWorkers()
{
   TKey key;
   if(Market_ == m_Tse){
      key.assign("5000");
      Workers_.SetWorker(key,&Wrk_T5000_);
      key.assign("5010");
      Workers_.SetWorker(key,&Wrk_T5010_);
      key.assign("5020");
      Workers_.SetWorker(key,&Wrk_T5020_);
   } else {
      key.assign("9500");
      Workers_.SetWorker(key,&Wrk_T5000_);
      key.assign("9510");
      Workers_.SetWorker(key,&Wrk_T5010_);
      key.assign("9520");
      Workers_.SetWorker(key,&Wrk_T5020_);
   }
}
//---------------------------------------------------------------------------
#ifdef __bxFT
K_mf(void)    TbxRptSes::RptRecover(TRptBody Body, bool IsEOF)
{
   if(OrdCenterPtr_ != 0){
      Kway::Tw::Stk::TOrdCenter::TRpt::TDeal rpt;
      rpt.TseOrdKey_.BrokerID_.assign(Body.BrokerID_.as_string());
      rpt.TseOrdKey_.Market_ = GetMarket();
      rpt.StkNo_.assign(Body.StkNo_.as_string());
//      if(Body.ExchageCode_.as_int() != 2)
//         rpt.Qty_ = Body.MthQty_.as_int() * 1000;
//      else
         rpt.Qty_ = Body.MthQty_.as_int();
      rpt.Pri_.assign((double)(Body.MthPri_.as_int())/100);
      rpt.TseTime_.assign(Body.MthTime_.as_string());
      rpt.BSCode_.assign(Body.BSCode_.as_string());
      rpt.TseOrdKey_.TseOrdID_.assign(Body.OrderNo_.as_string());
      rpt.IvacNo_.assign(Body.IvacNo_.as_string());
      rpt.BSeqNo_.assign(Body.SeqNo_.as_string());
      rpt.MSeqNo_.assign(Body.RecNo_.as_string());
      static EOType  otypeidx[] = { ot_Gn, ot_CrA, ot_DbA, ot_CrS, ot_DbS,
                                 ot_Tse5, ot_Tse6 };
      rpt.OType_.assignT(otypeidx[Body.OrderType_.as_int()]);
      OrdCenterPtr_->RptDeal(rpt);
   }
   PMap_.insert(std::make_pair(Body.SeqNo_.as_int(), Body.SeqNo_.as_int()));
   if(IsEOF){            // �n�D���ɺ|�^���v�g�^�ɧ���,�q�Y�ˬd���L�|�^��
      Recovering_ = false;
      RecoverTimer_.Stop();
      TSize LastSeq_(0);
      TPMap::iterator iter;
      for(iter = PMap_.begin(); iter != PMap_.end(); iter++){
         if(iter->first > (LastSeq_ +1)){
            CallRecover(LastSeq_ + 1, iter->first - 1);
            break;
         }
         LastSeq_ = iter->first;
      }
   }
}
//---------------------------------------------------------------------------
K_mf(bool)  TbxRptSes::CallRecover     (TSize fmseq, TSize endseq)
{
   if(Recovering_)
      return false;
   else{
      TSize Recs;
      RecoverTimer_.Start(5*60, &this_type::OnTimer);  //�w�]5����timerout
      Recovering_ = true;
      Recs = endseq - fmseq +1;
      if (Recs > 199)
         Recs = 199;
      if(MgrPtr_ != 0)
         MgrPtr_->ReqRptRecover(GetBrokerID(), fmseq, fmseq + Recs );
      //�b�o�ةI�ssesmgr���ɺ|fn
   }
   return true;
}
//---------------------------------------------------------------------------
K_mf(void)  TbxRptSes::OnRecoverTimer     (TTimerBase*)
{           // �o�X�n�D��5�����������^�ɰʧ@�N���s�Ұʦ^��
   Recovering_ = false;
   RecoverTimer_.Stop();
   TSize LastSeq_(0);
   TPMap::iterator iter;
   for(iter = PMap_.begin(); iter != PMap_.end(); iter++){
      if(iter->first > (LastSeq_ +1)){
         CallRecover(LastSeq_ + 1, iter->first - 1);
         break;
      }
      LastSeq_ = iter->first;
   }
}
//---------------------------------------------------------------------------
#endif

