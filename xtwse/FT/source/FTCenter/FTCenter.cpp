//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxSesMgr.h"
#include "SysMgrObjs.h"
#include "FTSchedule.h"
#include "FTSysSetting.h"
#include "FTCenter.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx;
using namespace Kway::Tw::Bx::FT;
//---------------------------------------------------------------------------
K_fn(const TFTCenter&) Kway::Tw::Bx::FT::GetCenter()
{
  static TFTCenter ftCenter;
  return ftCenter;
}
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&) TJobsVACr::TDJobsFieldMaker::GetFieldDesp  (field_n_type n) const
{
  static Kway::TFieldDesp f[] = {
  { "序號"    , 10, Kway::ta_xRight_yCenter,  Kway::ta_xRight_yCenter, Kway::tfs_Null, },
  { "作業序號", 10, Kway::ta_xRight_yCenter,  Kway::ta_xRight_yCenter, Kway::tfs_Null, },
  { "券商代號", 10, Kway::ta_xLeft_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_Null, },
//  { "市場別"  ,  6, Kway::ta_xLeft_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_Null, },
  { "檔案代碼",  6, Kway::ta_xLeft_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_Null, },
  { "類別",      6, Kway::ta_xLeft_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_Null, },
  { "資料來源", 20, Kway::ta_xLeft_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_Null, },
  { "資料標的", 20, Kway::ta_xLeft_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_Null, },
  { "資料長度", 10, Kway::ta_xLeft_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_Null, },
  { "建立時間", 10, Kway::ta_xLeft_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_Null, },
  { "傳送時間", 10, Kway::ta_xLeft_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_Null, },
  { "回覆時間", 10, Kway::ta_xLeft_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_Null, },
  { "備註"    , 30, Kway::ta_xLeft_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_Null, },
  };
  return f[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TJobsVACr::TDJobsFieldMaker::Get (const iterator& i, field_n_type fieldNo) const
{
  switch(fieldNo)
  {
    case ftcfn_Seqno:       return i->first .as_string();
    case ftcfn_WorkSeqno:   return i->second.SeqNo_      .as_string();
    case ftcfn_BrkID:       return i->second.SourceID_   .as_string();
//    case ftcfn_Market:      return i->second.Market_     .as_string();
    case ftcfn_FileCode:    return i->second.FileCode_   .as_string();
    case ftcfn_Type:        return i->second.Type_       .as_string();
    case ftcfn_SrcData:     return i->second.SrcData_    .as_string();
    case ftcfn_DstData:     return i->second.DstData_    .as_string();
    case ftcfn_RecLen:      return i->second.RecLen_     .as_string();
    case ftcfn_CreateTime:  return i->second.CreateTime_ .as_string();
    case ftcfn_SendTime:    return i->second.SendTime_   .as_string();
    case ftcfn_ReplyTime:   return i->second.ReplyTime_  .as_string();
    case ftcfn_Memo:        return i->second.LastMsg_    .as_string();
  }
  return std::string();
}
//---------------------------------------------------------------------------
K_mf(void) TJobsVACr::TDJobsFieldMaker::Put (TEvContainer& c, iterator& iter,
    field_n_type fieldNo, const std::string& str)
{
  if(fieldNo == 0)
    return;
  TEvContainer::updater   u = c.begin_update(iter);
  switch(fieldNo)
  {
    case ftcfn_WorkSeqno:   u->SeqNo_     .assign(str);   break;
    case ftcfn_BrkID:       u->SourceID_  .assign(str);   break;
//    case ftcfn_Market:      u->Market_    .assign(str);   break;
    case ftcfn_FileCode:    u->FileCode_  .assign(str);   break;
    case ftcfn_Type:        u->Type_      .assign(str);   break;
    case ftcfn_SrcData:     u->SrcData_   .assign(str);   break;
    case ftcfn_DstData:     u->DstData_   .assign(str);   break;
    case ftcfn_RecLen:      u->RecLen_    .assign(str);   break;
    case ftcfn_CreateTime:  u->CreateTime_.assign(str);   break;
    case ftcfn_SendTime:    u->SendTime_  .assign(str);   break;
    case ftcfn_ReplyTime:   u->ReplyTime_ .assign(str);   break;
    case ftcfn_Memo:        u->LastMsg_   .assign(str);   break;
  }
}
/////////////////////////////////////////////////////////////////////////////
K_mf(const Kway::TFieldDesp&)  TJobsVAC::TMJobsFieldMaker::GetFieldDesp  (field_n_type n) const
{
  static Kway::TFieldDesp f[] = {
    {"券商-市場別", 12, Kway::ta_xRight_yCenter,  Kway::ta_xRight_yCenter, Kway::tfs_Null, }
  };
  return f[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TJobsVAC::TMJobsFieldMaker::Get (const iterator& i, field_n_type fieldNo) const
{
  switch(fieldNo)
  {
    case 0:  return i->first.as_string();
  }
  return std::string();
}
//---------------------------------------------------------------------------
K_mf(void) TJobsVAC::TMJobsFieldMaker::Put (TEvContainer& c, iterator& iter,
    field_n_type fieldNo, const std::string& str)
{
  if(fieldNo == 0)
    return;
}
/////////////////////////////////////////////////////////////////////////////
K_mf(size_t) TJobsVAC::GetDetails   (const char**& a)
{
  static const char* desp[] = {"檔案傳輸中心"};
  a = desp;
  return numofele(desp);
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*) TJobsVAC::GetDetailVACreater (size_t detailID)
{
  switch(detailID)
  {
    case 0:return &DetailCreater_;
  }
  return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*) TJobsVAC::OnMasterViewRowMoved (Kway::TViewArea& masterVA
    , Kway::TViewArea*& detailVA, size_t detailID)
{
  TMJobsViewer&  va = static_cast<TMJobsViewer&>(masterVA);
  const TMJobsViewer::cursor_type* cur = va.GetCurRow();
  static_cast<TJobsVACr::TDJobsViewer*>(detailVA)->SetContainer( (cur==0 || cur->IsInvalid()
                              ? 0 : &(cur->GetIterator()->second.Jobs_)) );
  return inherited::DetailVACreaterNoChange;
}
/////////////////////////////////////////////////////////////////////////////
K_ctor TFTCenter::TFTCenter()
{
  // 避免在 GetCenter() 時,重覆CreateVAC() 
  static bool isCreate;
  if(isCreate == false)
  {
    isCreate = true;
    CreateVAC();
  }
}
//---------------------------------------------------------------------------
K_dtor TFTCenter::~TFTCenter()
{
  if(Job2d_)
    delete Job2d_;
  DestoryVAC();
}
//---------------------------------------------------------------------------
K_mf(void) TFTCenter::OnRecover(void)
{
   TSenderMgr& send = const_cast<TSenderMgr&>(GetSender());
   for(TBrkJobs::iterator ai=AllJobs_.begin(); ai!=AllJobs_.end(); ai++){
      TFtJobs& FtJobs = ai->second.Jobs_;
      for(TFtJobs::iterator fi=FtJobs.begin(); fi!=FtJobs.end(); fi++){
         BrkSeqMapI i = BrkSeq_.find(TBrkMkt(fi->second.SourceID_, fi->second.Market_));
         if(i == BrkSeq_.end())
            i = BrkSeq_.insert(i, BrkSeqMap::value_type(TBrkMkt(fi->second.SourceID_, fi->second.Market_), fi->second.SeqNo_));
         else if(fi->second.SeqNo_ > i->second)
            i->second = fi->second.SeqNo_;
         switch(fi->second.Flow_){
            case fft_Q_ing:{
               TFtJobs::updater u = FtJobs.begin_update(fi);
               u->Flow_ = fft_Abort;
               u->LastMsg_.assign("＊最後狀態:排隊中~該作業未傳送＊");
               break;
            }
            case fft_S_ing:{
               TFtJobs::updater u = FtJobs.begin_update(fi);
               u->Flow_ = fft_Abort;
               u->LastMsg_.assign("＊最後狀態:傳送中~該作業未傳送完成＊");
               break;
            }
            case fft_R_ing:{
               TFtJobs::updater u = FtJobs.begin_update(fi);
               if(u->Mode_ != ftm_Report){
                  u->Flow_ = fft_Abort;
                  u->LastMsg_.assign("＊最後狀態:接收中~該作業未接收完成＊");
               }else{
                  u->Flow_ = fft_W_ing;
                  u->LastMsg_.assign("＊最後狀態:接收中~繼續等候回報＊");
               }
               break;
            }
            case fft_W_ing:
               GetBxMgr()->AddRpyQue(ai->first, fi->second.FileCode_, fi->first);
               continue;
            default:
               continue;
         }
         send.SendNotify(fi->second);
      }
   }
}
//---------------------------------------------------------------------------
K_mf(void) TFTCenter::CreateVAC (void) //建立遠端VAC
{
//  std::string FN = std::string(".\\Logs\\%4y%2m%2d\\ft.bin");
  std::string FN = GetProcLogPath()+"/ft.bin";
  Job2d_ = new TJob2d(&AllJobs_, FN, 4096);
  Job2d_->BindInn();
  OnRecover();
  VAC_ = new TJobsVAC(AllJobs_);
//  VAMgr_.Add("檔案傳輸工作", *VAC_);

  const Kway::TSysMgrObjs& sys = Kway::GetSysMgrObjs();
  sys.SysVAMgr_->Add("資料中心", *VAC_);
//  sys.SysVAMgr_->Add("資料中心", VAMgr_);
}
//---------------------------------------------------------------------------
K_mf(void) TFTCenter::DestoryVAC(void)
{
  const Kway::TSysMgrObjs& sys = Kway::GetSysMgrObjs();
  sys.SysVAMgr_->Del(*VAC_);
//  VAMgr_.Del(*VAC_);
  if(VAC_)
    delete VAC_;
}
//---------------------------------------------------------------------------
K_mf(TSysID) TFTCenter::DoReq(const TFTNewJob& job)
{
  TFTJob work = job;
  work.Flow_  = fft_P_ing;
  work.LastMsg_.assign(work.Flow_.as_string());

  TBrkJobs::iterator iter;
  iter = AllJobs_.fetch(TBrkMkt(job.SourceID_, job.Market_));
  int seqno = iter->second.Jobs_.size();
  TFtJobs::iterator i = iter->second.Jobs_.fetch(++seqno);
  TFtJobs::updater upd = iter->second.Jobs_.begin_update(i);
  upd->SeqNo_    = job.SeqNo_;
  upd->SourceID_ = job.SourceID_;
  upd->DestID_   = job.DestID_;
  upd->Market_   = job.Market_;
  upd->FileCode_ = job.FileCode_;
  upd->Type_     = job.Type_;
  upd->Mode_     = job.Mode_;
  upd->SrcSeqNo_ = job.SrcSeqNo_;
  upd->SrcData_  = job.SrcData_;
  upd->DstData_  = job.DstData_;
  upd->RecLen_   = job.RecLen_;
  upd->CreateTime_=job.CreateTime_;
  upd->EnvMode_  = job.EnvMode_;
  upd->Flow_     = fft_P_ing;
  upd->LastMsg_.assign(upd->Flow_.as_string());
  return TSysID(seqno);
}
//---------------------------------------------------------------------------
K_mf(TSysID) TFTCenter::DoS10Msg(const TMarket& mrk, const TF050& f050)
{
  TBrkJobs::iterator iter;
  iter = AllJobs_.fetch(TBrkMkt(f050.DestID_,mrk));
  int seqno = iter->second.Jobs_.size();
  TFtJobs::iterator i = iter->second.Jobs_.fetch(++seqno);
  TFtJobs::updater upd = iter->second.Jobs_.begin_update(i);
  upd->SeqNo_    = 0;
  upd->SourceID_ = f050.SourceID_;
  upd->DestID_   = f050.DestID_;
  upd->Market_   = mrk;
  upd->FileCode_ = f050.FileCode_;
  upd->Type_     = ftt_Msg;
  upd->Mode_     = ftm_TSEMsg;
  upd->SrcData_  = f050.Msg_;
  upd->DstData_  .clear();
  upd->RecLen_   = 0;
  upd->CreateTime_=TKTime::Now();
  upd->ReplyTime_=TKTime::Now();
  upd->Flow_     = fft_Finish;
  upd->LastMsg_.assign("S10 公告");
  return TSysID(seqno);
}
//---------------------------------------------------------------------------
/*K_mf(TSysID) TFTCenter::DoReq(const TBrkID& brk, const TMarket& mrk,
    const TFileCode& code, const char* msg)
{
  TBrkJobs::iterator iter;
  iter = AllJobs_.fetch(brk);
  int seqno = iter->second.Jobs_.size();
  TFtJobs::iterator i = iter->second.Jobs_.fetch(++seqno);
  TFtJobs::updater upd = iter->second.Jobs_.begin_update(i);
  return TSysID(seqno);
}*/
//---------------------------------------------------------------------------
K_mf(bool) TFTCenter::GetHnd(const TBrkID& brk, const TMarket& mkt, const TSysID& sysid, const TFTJob** job)
{
  TBrkJobs::iterator iter;
  iter = AllJobs_.find(TBrkMkt(brk, mkt));
  if(iter == AllJobs_.end())
    return false;
  TFtJobs::iterator i = iter->second.Jobs_.find(sysid);
  if(i == iter->second.Jobs_.end())
    return false;
  i = iter->second.Jobs_.fetch(sysid);
  *job = &i->second;
  return true;
}
//---------------------------------------------------------------------------
K_mf(TSeqNo) TFTCenter::FetchSeqNo(const TBrkID& brk, const TMarket& mkt)
{
   TSeqNo seqno = GetHostID()>=THostID(2)?9000:8000;
   seqno += (mkt == m_Tse?0:500);
   BrkSeqMapI i = BrkSeq_.find(TBrkMkt(brk, mkt));
   if(i == BrkSeq_.end())
      i = BrkSeq_.insert(i, BrkSeqMap::value_type(TBrkMkt(brk, mkt), seqno));
   if(seqno > i->second)
      i->second = seqno; 
   seqno = ++i->second;
   return seqno;
}
//---------------------------------------------------------------------------
K_mf(TSysID) TFTCenter::FetchNextQing(const TBrkID& brk, const TMarket& mkt, TFileCode code)
{
   TBrkJobs::iterator iter;
   iter = AllJobs_.find(TBrkMkt(brk,mkt));
   if(iter == AllJobs_.end())
      return 0;
   if(!code.empty()){
      for(TFtJobs::iterator i = iter->second.Jobs_.end(); i != iter->second.Jobs_.begin();)
         if((--i)->second.Flow_ == fft_Q_ing &&  code == i->second.FileCode_)
            return i->first;
   }
   for(TFtJobs::iterator i = iter->second.Jobs_.end(); i != iter->second.Jobs_.begin();)
      if((--i)->second.Flow_ == fft_Q_ing)
         return i->first;
   return 0;
}
//---------------------------------------------------------------------------
K_mf(void) TFTCenter::UpdateFTStatus(const TBrkID& brk, const TMarket& mkt, TSysID sysid, EFTFlowID flow, TStCode code, const char* msg)
{
  TFtJobs::updater uper;
  if(GetJobHnd(brk, mkt, sysid, uper))
  {
    switch(flow)
    {
      case fft_S_ing:   uper->SendTime_ = Kway::TKTime::Now();   break;
      case fft_R_ing:   uper->ReplyTime_= Kway::TKTime::Now();   break;
      case fft_Finish:
      case fft_Abort:
      {
        char buf[3];
        sprintf(buf, "%02d", code);
        uper->LastCode_.assign(buf);
        break;
      }
    }
    uper->Flow_ = flow;
    if(uper->FileCode_ != TFileCode("S10"))
    {
      if(flow == fft_Abort){
        uper->LastMsg_.assign(TKTime::Now().as_string()+" "+msg);
        if(uper->Mode_ == ftm_Schedule){
           if(uper->Type_ == ftt_Msg)    // 下載要求
              (const_cast<TScheduleCenter*>(&GetSchedule()))->UpdateSchedule(uper->SrcSeqNo_, msg);
           if(uper->Type_ == ftt_File)   //  上傳要求
              (const_cast<TScheduleCenter*>(&GetSchedule()))->UpdateULoadSch(uper->SrcSeqNo_, msg);
        }
      }else
        uper->LastMsg_.assign(TKTime::Now().as_string()+" "+uper->Flow_.as_string());
    }
  }
}
//---------------------------------------------------------------------------
K_mf(void) TFTCenter::UpdateFTFlow(const TBrkID& brk, const TMarket& mkt, TSysID sysid, EFTFlowID flow, const std::string& errmsg)
{
  TFtJobs::updater uper;
  if(GetJobHnd(brk, mkt, sysid, uper))
  {
    uper->Flow_ = flow;
    if(flow == fft_Abort){
      uper->ReplyTime_ = TKTime::Now();
      if(uper->Mode_ == ftm_Schedule){
         if(uper->Type_ == ftt_Msg)    // 下載要求
            (const_cast<TScheduleCenter*>(&GetSchedule()))->UpdateSchedule(uper->SrcSeqNo_, errmsg.c_str());
         if(uper->Type_ == ftt_File)   //  上傳要求
            (const_cast<TScheduleCenter*>(&GetSchedule()))->UpdateULoadSch(uper->SrcSeqNo_, errmsg.c_str());
      }
    }
    uper->LastMsg_.assign(errmsg.empty() ? uper->Flow_.as_string():errmsg);
  }
}
//---------------------------------------------------------------------------
K_mf(void) TFTCenter::UpdateDstData(const TBrkID& brk, const TMarket& mkt, TSysID sysid, const std::string& data)
{
  TFtJobs::updater uper;
  if(GetJobHnd(brk, mkt, sysid, uper))
    uper->DstData_.assign(data);
}
//---------------------------------------------------------------------------
K_mf(bool) TFTCenter::GetJobHnd(const TBrkID& brk, const TMarket& mkt, TSysID sysid, TFtJobs::updater& uper)
{
  TBrkJobs::iterator iter = AllJobs_.find(TBrkMkt(brk, mkt));
  if(iter == AllJobs_.end())
    return false;
  TFtJobs::iterator i = iter->second.Jobs_.find(sysid);
  if(i == iter->second.Jobs_.end())
    return false;
  uper = iter->second.Jobs_.begin_update(i);
  return true;
}
//---------------------------------------------------------------------------
K_mf(bool) TFTCenter::UpdateFTCenter(TBrkMkt& brkmkt, TFileCode& code)
{
   int interval = atoi(GetSysSetting().GetSetting(ssi_TimeoutInt).c_str());
   if(interval <= 0)
      return false;
   for(TBrkJobs::iterator iter = AllJobs_.begin(); iter != AllJobs_.end(); iter++){
      brkmkt = iter->first;
      for(TFtJobs::iterator i = iter->second.Jobs_.begin(); i != iter->second.Jobs_.end(); ++i)
        if(i->second.Flow_ == fft_S_ing || i->second.Flow_ == fft_W_ing || i->second.Flow_ == fft_P_ing)
           if((TKTime::Now() - (i->second.Flow_==fft_P_ing?i->second.CreateTime_:i->second.SendTime_)) > interval*60){   // 作業逾時
              char buf[64];
              sprintf(buf,"作業逾時[超過%d分鐘無回應]",interval);
              code = i->second.FileCode_;
              UpdateFTFlow(iter->first.BrokerID_, iter->first.Market_, i->first, fft_Abort, 
                           std::string(buf));
              TFtJobs::updater u = iter->second.Jobs_.begin_update(i);
              u->LastMsg_.assign(buf);
              u->Flow_ = fft_Timeout;
              TSenderMgr& send = const_cast<TSenderMgr&>(GetSender());
              send.SendNotify(i->second);
              return true;
           }   
   }
   return false;
}
//---------------------------------------------------------------------------
K_mf(Kway::word) TFTCenter::GetQJobs(TBrkMkt bkmt, TFileCode fc)
{
   Kway::word QJobs = 0;
   TBrkJobs::iterator iter = AllJobs_.find(bkmt);
   if(iter == AllJobs_.end())
      return 0;
   for(TFtJobs::iterator i = iter->second.Jobs_.end(); i != iter->second.Jobs_.begin();)
      if((--i)->second.Flow_ == fft_Q_ing && (fc == TFileCode() || i->second.FileCode_ == fc ))
         QJobs++;
   return QJobs;
}
//---------------------------------------------------------------------------
K_mf(TSysID) TFTCenter::FindFirstWaiting(const TBrkID& brk, const TMarket& mkt, TFileCode code)
{
   TBrkJobs::iterator iter;
   iter = AllJobs_.find(TBrkMkt(brk,mkt));
   if(iter == AllJobs_.end())
      return 0;
   for(TFtJobs::iterator i = iter->second.Jobs_.end(); i != iter->second.Jobs_.begin();)
      if((--i)->second.Flow_ == fft_W_ing &&  code == i->second.FileCode_)
         return i->first;
   return 0;
}
//---------------------------------------------------------------------------


