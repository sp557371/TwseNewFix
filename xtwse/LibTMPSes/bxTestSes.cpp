#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxTestSes.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw;
using namespace Kway::Tw::bx;
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
K_dtor TbxTestSes::~TbxTestSes  ()
{
   SetState(sp_Destroying, "Destroying");
}
//---------------------------------------------------------------------------
K_mf(void)    TbxTestSes::ReStartLink()         //�s�u�l�t�έ��s�s�u
{
   TKey key;
   TWorker* worker;
   StopTimer();
   SetLinkTimeOut(5000);
   if(Market_ == Stk::m_Tse){
      key.assign("1010");
   }else{
      key.assign("9110");
   }
   worker = Workers_.GetWorker(key);
   if(worker != 0){
      worker->LinkReq(*this,pc_L010,ApCode_);          //�e�XL010
      SetState(sp_WaitLinkReady,"Start Linking");
      StartTimer(LinkTimeOut_);
   }
   else
      SetState(sp_Error,std::string("Can't find the worker"));
}
//---------------------------------------------------------------------------
K_mf(void)    TbxTestSes::OnLinkWorkerFinished(TPacketCode PkCode, const THead& pk)
{
   TKey key;
   TWorker* worker;
   TLinkErrorCode       errCode =  static_cast<TLinkErrorCode>(pk.StatusCode_.as_int());
   StopTimer   ();
   if(pk.StatusCode_.as_int() > 0){
      std::string msg = GetErrMsg(std::string(pk.SubSysName_.as_string()+pk.StatusCode_.as_string()).c_str());
      switch(pk.StatusCode_.as_int()){
         case 86: // ���ճq�L
            if(GetExcMgr())
               GetExcMgr()->Alert("XT1001",SesTermID_);
            Close();
            MgrPtr_->DelSetting(GetMarket(),GetBrokerID(),GetPvcID());
            OnStateChanged(msg);
            return;
         case 04: // Key value error
            if(GetExcMgr())
               GetExcMgr()->Alert("XT1011",SesTermID_);
            OnStateChanged(msg);
            return;
         default:
            OnStateChanged(msg);
            return;
      }
   }
   switch(PkCode){
      case pc_L010:
      //����L010 �i��O��l�s�u,�]���i��O����ҭn�D���s�s�u
         switch(errCode){
            case lec_Success:               //status code ok �^��L020
            //���p�G�w��Mgr���U�FSub System Ses �n������
               DelSubSysReg();
               if(Market_ == Stk::m_Tse)
                  key.assign("1010");
               else
                  key.assign("9110");
               worker = Workers_.GetWorker(key);
               if(worker != 0){
                  worker->LinkReq(*this,pc_L020,ApCode_);          //�e�XL020
                  SetState(sp_WaitLinkReady,"Start Linking");
                  StartTimer(LinkTimeOut_);
               }else
                  SetState(sp_Error,std::string("Can't find the worker"));
               break;
            case lec_SysNotReady:                   //����ɶ�����,�@������A�s�u
               SetState(sp_TimeEarly,MgrPtr_->GetMsg(pk.SubSysName_.as_string(),pk.StatusCode_.as_string()));
               StartTimer(LinkTimeOut_);
               break;
            case lec_TimeErr:
               SetState(sp_Error,MgrPtr_->GetMsg(pk.SubSysName_.as_string(),pk.StatusCode_.as_string()));
            default:
               SetState(sp_WaitLinkReady,MgrPtr_->GetMsg(pk.SubSysName_.as_string(),pk.StatusCode_.as_string()));
               StartTimer(LinkTimeOut_); 
               break;
          }
          break;
       case pc_L020:
          SetState(sp_WaitLinkReady,"Wait for Login Message");
          StartTimer(LinkTimeOut_);
          break;
       case pc_L030:
          if(State_ != sp_WaitLinkReady){
             ReStartLink();
             break;
          }
          switch(errCode){
             case lec_Success:               //�e�X�n���T��
                if(Market_ == Stk::m_Tse)
                   key.assign("1020");
                else
                   key.assign("9120");
                worker = Workers_.GetWorker(key);
                if(worker != 0){
                   worker->LinkReq(*this,pc_L040,GetApCode()/*ApCode_*/);          //�e�XL040
                   SetState(sp_WaitLoginReady,"Start Login");
                   StartTimer(LinkTimeOut_);
                }else
                   SetState(sp_Error,std::string("Can't find the worker"));
                break;
             case lec_AppNoErr:              //AppendNo ���~
             case lec_BrkIDErr:              //BrokerID ���~
             case lec_ApCodeErr:             //ApCode ���~
             case lec_KeyValueErr:           //KeyValue ���~
             default:
                SetState(sp_Error,MgrPtr_->GetMsg(pk.SubSysName_.as_string(),pk.StatusCode_.as_string()));
                break;
          }
          break;
       case pc_L040:
          SetState(sp_WaitLoginReady,"Wait for SubSystem Start");
          StartTimer(LinkTimeOut_);
          break;
       case pc_L050:
          if(State_ != sp_WaitLoginReady){
             ReStartLink();
             break;
          }
          switch(errCode){
             case lec_Success:               //�e�X�n���T��
                if(Market_ == Stk::m_Tse)
                   key.assign("1020");
                else
                   key.assign("9120");
                worker = Workers_.GetWorker(key);
                if(worker != 0){
                   worker->LinkReq(*this,pc_L060,ApCode_);          //�e�XL060
                   SetState(sp_WaitSubSysReady,"SubSystem Reday");
                   OnSubSysReady();
                }else
                   SetState(sp_Error,std::string("Can't find the worker"));
                break;
             default:
                SetState(sp_Error,MgrPtr_->GetMsg(pk.SubSysName_.as_string(),pk.StatusCode_.as_string()));
                ReStartLink();
                break;
          }
          break;
       case pc_L060:
          SetState(sp_Ready,"SubSystem Reday");
          StartTimer(LinkTimeOut_);
          break;
       case pc_L070:
          switch(errCode){
             case lec_Success:               //�e�X�T�{���u�T��
                if(Market_ == Stk::m_Tse)
                   key.assign("1030");
                else
                   key.assign("9130");
                worker = Workers_.GetWorker(key);
                if(worker != 0){
                   worker->LinkReq(*this,pc_L080,ApCode_);          //�e�XL080
                   SetState(sp_Signout,"System Logout");
                }else
                   SetState(sp_Error,std::string("Can't find the worker"));
                break;
             default:
                SetState(sp_Error,MgrPtr_->GetMsg(pk.SubSysName_.as_string(),pk.StatusCode_.as_string()));
                ReStartLink();
                break;
          }
          break;
       case pc_L080:
          SetState(sp_Signout,"System Logout");
          break;
   }
}
//---------------------------------------------------------------------------
K_mf(void)    TbxTestSes::AddLinkWorkers()
{
   TKey key;
   if(Market_ == Stk::m_Tse)
      SubSys_.assign("10");
   else
      SubSys_.assign("91");
//   if(Market_ == Stk::m_Tse){
      key.assign("1010");
      Workers_.SetWorker(key,&Wrk_L1010_);
      key.assign("1020");
      Workers_.SetWorker(key,&Wrk_L1020_);
      key.assign("1030");
      Workers_.SetWorker(key,&Wrk_L1030_);
//   }else{
      key.assign("9110");
      Workers_.SetWorker(key,&Wrk_L1010_);
      key.assign("9120");
      Workers_.SetWorker(key,&Wrk_L1020_);
      key.assign("9130");
      Workers_.SetWorker(key,&Wrk_L1030_);
//   }
}
//---------------------------------------------------------------------------
K_mf(void) TbxTestSes::OnMesLinkReady(TMesBase* M_)
{
   FailCount_ = 0;
   if(GetExcMgr())
      GetExcMgr()->Alert("XT1000",SesTermID_);
   GetMgr()->GetRecorder()->WriteLog(ID_, "Link Ready!");

   ReStartLink();
}
//---------------------------------------------------------------------------
K_mf(void)  TbxTestSes::OnTimer  (TTimerBase*)
{
   ReStartLink();
}
//---------------------------------------------------------------------------

