#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxSes.h"
#include "MesTSEClient.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw;
using namespace Kway::Tw::bx;
/////////////////////////////////////////////////////////////////////////////
K_mf(TWorker*)  TWorkersBase::SetWorker  (bx::TKey key, TWorker* newWorker)
{
   TWorkerRegs* regs = GetWorkerRegs();
   TWorkerRegs::iterator   i = regs->find(key);
   if(i == regs->end()){
      regs->insert(std::make_pair(key,newWorker));
      return 0;
   }else{
      TWorker* old = i->second;
      i->second = newWorker;
      return old;
   }
}
/////////////////////////////////////////////////////////////////////////////
K_mf(TWorker*)  TWorkers::GetWorker    (bx::TKey key)
{
   TWorkerRegs::iterator   i = Workers_.find(key);
   if(i == Workers_.end())
      return 0;
   else
      return i->second;
}
/////////////////////////////////////////////////////////////////////////////
K_ctor  TbxSes::TbxSes  (TbxSesMgr* SesMgr)
   : RecvPos_(0),
     PkResult_(cpr_Continue),
     SkipBytes_(0),
     Timer_(this),
     RptTimer_(this),
     State_(sp_Creating),
     ReqPtr_(0),
     MgrPtr_(SesMgr),
     IsFix_(false)
{
   OrdCenterPtr_ = SesMgr->GetOrdCenter();
}
//---------------------------------------------------------------------------
K_dtor TbxSes::~TbxSes  ()
{
   GetMgr()->GetRecorder()->WriteLog(ID_, "Session Destroyed!");
   SetState(sp_Destroying, "Destroying");
   RptTimer_.Stop();
}
//---------------------------------------------------------------------------
K_mf(bool)  TbxSes::SettingApply  (const std::string& setting)
{
   SettingStr_ = setting;
//   TApCode apcode;
   //設定範例 BrokerID,PassWord,PVCID   :modified by eric 930331
   //1520,1234,A4
   std::string::size_type   Pos1,Pos2;
   Pos1 = 0;
   Pos2 = 0;
   Pos1 = SettingStr_.find(',',Pos2);
   if(Pos1 != std::string::npos)
      BrokerID_.assign(SettingStr_.substr(0,Pos1));
   else{
      SetState(sp_Error, "Setting Error!");
      return false;
   }
   Pos2 = Pos1 + 1;
   Pos1 = SettingStr_.find(',',Pos2);
   if(Pos1 != std::string::npos)
      PassWord_ = atoi(SettingStr_.substr(Pos2,Pos1-Pos2).c_str());
   else{
      SetState(sp_Error, "Setting Error!");
      return false;
   }
   Pos2 = Pos1 + 1;
   if((SettingStr_.length() - Pos2)<sizeof(PvcID_)){
      SetState(sp_Error, "Setting Error!");
      return false;
   } else{
      Pos1 = SettingStr_.find(',',Pos2);
      if(Pos1 == std::string::npos)
         PvcID_.assign(SettingStr_.substr(Pos2,SettingStr_.length() - Pos2+1));
      else{
         PvcID_.assign(SettingStr_.substr(Pos2,Pos1-Pos2));
         Pos2 = Pos1 + 1;
//         apcode.assign(std::string(&SettingStr_[Pos2]));//.length() - Pos2+1);
         ApCode_.assign(std::string(&SettingStr_[Pos2]));//.length() - Pos2+1);
      }
   }
//   IsFix_ = (ApCode_[0] == '7');
   ID_.assign(BrokerID_.as_string()+"-"+PvcID_.as_string());
   SesTermID_ = ID_.as_string();
   SesTermID_ += "-";
   SesTermID_ += GetMgr()->GetTermID(GetBrokerID(),GetMarket(),GetPvcID());
   return true;
}
//---------------------------------------------------------------------------
K_mf(bool)  TbxSes::SendPk  (const THead& pk,TSize size)
{
   if(GetMes() == 0  ||  !GetMes()->IsLinkReady())
      return false;
   return GetMes()->Send(&pk, size)==size;
}
//---------------------------------------------------------------------------
K_mf(void)  TbxSes::OnMesLinkReady  (TMesBase*  Mes)
{
   FailCount_ = 0;
   if(GetExcMgr())
      if(GetMgr()->IsTester())
         GetExcMgr()->Alert("XT1000",SesTermID_);
      else
         GetExcMgr()->Alert("XX1000",SesTermID_);
   GetMgr()->GetRecorder()->WriteLog(ID_, "Link Ready!");
   TKey key;
   TWorker* worker;
   StopTimer();
   key.assign("1010");
   worker = Workers_.GetWorker(key);
   if(worker != 0){
      worker->LinkReq(*this,pc_L010,ApCode_);          //送出L010
      SetState(sp_WaitLinkReady,"Start Linking");
      StartTimer(30000);
   }else
      SetState(sp_Error,std::string("Can't find the worker"));
}
//---------------------------------------------------------------------------
K_mf(void)  TbxSes::OnMesLinkFail  (TMesBase*, const std::string& reason)
{
   FailCount_++;
   if(GetExcMgr())
      if(GetMgr()->IsTester())
         GetExcMgr()->Alert("XT1010",SesTermID_, reason);
      else{
         if(GetMgr()->GetFailCount() == FailCount_)
            GetExcMgr()->Alert("XX1011",SesTermID_, reason);
         else
            GetExcMgr()->Alert("XX1010",SesTermID_, reason);
      }
   DelSubSysReg();
   char buf[32];
   SetState(sp_Error, "Link fail!");
}
//---------------------------------------------------------------------------
K_mf(void)  TbxSes::OnMesLinkBroken  (TMesBase*, const std::string& reason)
{
   if(GetExcMgr())
      if(GetMgr()->IsTester())
         GetExcMgr()->Alert("XT1012",SesTermID_);
      else
         GetExcMgr()->Alert("XX1012",SesTermID_);
   GetMgr()->GetRecorder()->WriteLog(ID_, "Link Broken!");
   DelSubSysReg();
   RecvPos_ = 0;
   SetState(sp_Error, reason);
   RptTimer_.Stop();
}
//---------------------------------------------------------------------------
K_mf(void)  TbxSes::OnMesRecvReady  (TMesBase* Mes_, size_t)//size
{
   TSize PacketSize_;
   char* RecvBuf = reinterpret_cast<char*>(RecvPk_);
   for(;;) {
      if(Mes_==0)
         return;
      int   reqsz = sizeof(*RecvPk_) - RecvPos_;
      int   rdsz  = Mes_->Recv(RecvBuf + RecvPos_, reqsz);
      if(rdsz <= 0)//已無資料
         break;
      if((RecvPos_ += rdsz) < CtlHeadSize)
         //Header尚未收完
         break;
__CHECK_NEXT_PK:
      //檢查 PacketKey 是否合理
      TKey key = RecvPk_->GetPacketKey();
      size_t   Skips = 0;
      TWorker* worker;
      while((worker = Workers_.GetWorker(key))==0) {//不合理的SubSysName 或 沒有此訊息的處理程序
         if(Skips == 0){ 
            GetMgr()->GetRecorder()->WriteLog(ID_, (char*)RecvPk_);
            GetMgr()->GetRecorder()->WriteLog(ID_, "Invalid header, search next!");
         }
        //尋找下一個合理的 PacketKey
__SKIP_BYTES:
         ++Skips;
         if(--RecvPos_ < CtlHeadSize)
            break;
         key = reinterpret_cast<TbxRec*>(RecvBuf + Skips)->GetPacketKey();
      } //while
      if(Skips) {
         SkipBytes_ += Skips;
         memcpy(RecvBuf, RecvBuf+Skips, RecvPos_);
         if(RecvPos_ < CtlHeadSize)
            continue;
      }
      if(worker) {
         PacketSize_ = 0;
         if(PkResult_ == cpr_HeadOK) {
__CHECK_RECV_FINISH:
            if(static_cast<TSize>(RecvPos_) >= PacketSize_) {
               //訊息已收完
               worker->OnPkRecv(*this,*RecvPk_,ReqPtr_);
               PkResult_ = cpr_Continue;
               Skips = PacketSize_;
               if(Skips < RecvPos_) {
                  memcpy(RecvBuf, RecvBuf+Skips, RecvPos_ -= Skips);
               } else{
                  RecvPos_ = 0;
                  memset(RecvPk_,0,sizeof(TbxRec));
               }
               ResetRptTimer();
               break;
            } // if(RecvPos_ >= RecvPk->GetPkSize()...
         } else {
            switch(PkResult_ = worker->OnPkCheck(*this,*RecvPk_, static_cast<TSize>(RecvPos_),&PacketSize_)) {
            case cpr_HeadOK:
            case cpr_Continue:
                        goto __CHECK_RECV_FINISH;
            case cpr_Error:
                        Skips = 0;
                        goto __SKIP_BYTES;
            } // switch(worker->OnPkCheck()
         } // if(PkResult_ == cpr_HeadOK..else...
      } // if(worker...
      if(rdsz < reqsz)//已將Mes緩衝區的資料收完了
         break;
   } // for(;;...
}
//---------------------------------------------------------------------------
K_mf(void)  TbxSes::StartTimer  (size_t ms)
{
   Timer_.Start(ms, &this_type::OnTimer);
}
//---------------------------------------------------------------------------
K_mf(void)  TbxSes::StopTimer  ()
{
   Timer_.Stop();
}
//---------------------------------------------------------------------------
K_mf(void)  TbxSes::OnTimer  (TTimerBase*)
{
}
//---------------------------------------------------------------------------
K_mf(void)  TbxSes::SetState  (EState newState, const std::string& reason)
{
   if(State_ == newState)
      return;
   State_ = newState;
   OnStateChanged(reason);
}
//---------------------------------------------------------------------------
K_mf(void)  TbxSes::ResetRptTimer  ()
{
   RptTimer_.Stop();
   RptTimer_.Start(1, &this_type::OnRptTimer);
}
//---------------------------------------------------------------------------
K_mf(void)  TbxSes::OnRptTimer  (TTimerBase*)
{
   RptTimer_.Stop();
   OnMesRecvReady(GetMes(), 0);
}
/////////////////////////////////////////////////////////////////////////////

