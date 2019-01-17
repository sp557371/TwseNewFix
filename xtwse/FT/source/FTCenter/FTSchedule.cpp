//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "SysMgrObjs.h"
#include "bxHeadDef.h"
#include "bxSesMgr.h"
#include "ExcMgrBase.h"
#include "FTSchedule.h"
#include "FTCenter.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx::FT;
//---------------------------------------------------------------------------
K_fn(const TScheduleCenter&) Kway::Tw::Bx::FT::GetSchedule()
{
  static TScheduleCenter ftSchedule;
  return ftSchedule;
}
//---------------------------------------------------------------------------
struct TMarketProp : public Kway::TVAGetEditProp
{
   virtual K_mf(TEditStyle)   GetEnums  (const Kway::TViewArea&, const Kway::TViewCursor*,
     size_t fieldNo, Kway::TStrings* result, bool& selectOnly)
   {
     if(result)
     {
       if(result->size() < 2)
       {
         result->clear();
         result->push_back(KStk::TMarket(KStk::m_Tse).as_string(true));
         result->push_back(KStk::TMarket(KStk::m_Otc).as_string(true));
       }
     }
     selectOnly = true;
     return es_AllRecsEqual;
   }
};
static TMarketProp  MarketProp;
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string)  TSchType::as_string  () const
{
   switch(Data_) {
   case estNone:    return std::string("N-不執行");
   case estToday:   return std::string("T-今日有效");
   case estAlways:  return std::string("Y-預設執行");
   }
   return inherited::as_string();
}
/////////////////////////////////////////////////////////////////////////////
struct TSchTypeProp : public Kway::TVAGetEditProp
{
   virtual K_mf(TEditStyle)   GetEnums  (const Kway::TViewArea&, const Kway::TViewCursor*,
     size_t fieldNo, Kway::TStrings* result, bool& selectOnly)
   {
     if(result)
     {
       if(result->size() < 3)
       {
         result->clear();
         result->push_back(TSchType(estNone).as_string());
         result->push_back(TSchType(estToday).as_string());
         result->push_back(TSchType(estAlways).as_string());
       }
     }
     selectOnly = true;
     return es_AllRecsEqual;
   }
};
static TSchTypeProp  SchTypeProp;
/////////////////////////////////////////////////////////////////////////////
K_mf(const Kway::TFieldDesp&) TScheduleVAC::TScheduleFieldMaker::GetFieldDesp  (field_n_type n)  const
{
  static Kway::TFieldDesp f[] = {
  { "序號",          8, Kway::ta_xRight_yCenter,  Kway::ta_xRight_yCenter, Kway::tfs_Null,    },
  { "排程",         10, Kway::ta_xCenter_yCenter ,  Kway::ta_xCenter_yCenter , Kway::tfs_CanEdit, 0, &SchTypeProp},
  { "立即執行",      8, Kway::ta_xCenter_yCenter ,  Kway::ta_xCenter_yCenter , Kway::tfs_CanEdit  },
  { "檔案代碼",      8, Kway::ta_xCenter_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_CanEdit  },
  { "市場別",        8, Kway::ta_xCenter_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_CanEdit, 0, &MarketProp },
  { "券商代號",      8, Kway::ta_xCenter_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_CanEdit  },
  { "附加訊息",     12, Kway::ta_xCenter_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_CanEdit  },
  { "排程時間",      8, Kway::ta_xCenter_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_CanEdit  },
  { "重試間隔(分)", 10, Kway::ta_xCenter_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_CanEdit  },
  { "重試次數",      8, Kway::ta_xCenter_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_CanEdit  },
  { "上次執行時間", 10, Kway::ta_xCenter_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_Null     },
  { "已執行次數",    8, Kway::ta_xCenter_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_Null     },
  { "完成時間",     10, Kway::ta_xCenter_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_Null     },
  { "備註",         30, Kway::ta_xCenter_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_CanEdit  },
  };
  return f[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TScheduleVAC::TScheduleFieldMaker::Get (const iterator& i, field_n_type fieldNo) const
{

  switch(fieldNo)
  {
    case ftsfn_SeqNo:       return i->first.as_string();
    case ftsfn_SchType:     return i->second.SchType_.as_string();
    case ftsfn_Rightaway:   return (i->second.Rightaway_?"Y":"N");
    case ftsfn_FileCode:    return i->second.FileCode_    .as_string();
    case ftsfn_Market:      return i->second.Market_      .as_string(true);
    case ftsfn_BrkID:       return i->second.BrkID_       .as_string();
    case ftsfn_Msg:         return i->second.Msg_         .as_string();
    case ftsfn_ExecTime:  { char buf[12];
                            sprintf(buf, "%02d:%02d", i->second.ActiveTime_.GetHour(), i->second.ActiveTime_.GetMin());
                            return std::string(buf); }
    case ftsfn_RetryInt:    return i->second.DelayTime_   .as_string();
    case ftsfn_RetryCount:  return i->second.Retry_       .as_string();
    case ftsfn_LastExecTime:return i->second.LastExecTime_.as_string();
    case ftsfn_ExecCount:   return i->second.ExecCount_   .as_string();
    case ftsfn_FinishTime:  return i->second.FinishTime_  .as_string();
    case ftsfn_Memo:        return i->second.Memo_        .as_string();
  }
  return std::string();
}
//---------------------------------------------------------------------------
K_mf(void) TScheduleVAC::TScheduleFieldMaker::Put (TEvContainer& c, iterator& iter,
    field_n_type fieldNo, const std::string& str)
{
  if(fieldNo == 0)
    return;
  TEvContainer::updater u = c.begin_update(iter);
  switch(fieldNo)
  {
    case ftsfn_SchType:     u->SchType_    .assign(str);        break;
    case ftsfn_Rightaway:   u->Rightaway_= (str[0] == 'y' || str[0] == 'Y');break;
    case ftsfn_FileCode:    u->FileCode_    .assign(str);       break;
    case ftsfn_Market:      u->Market_      .assign(str);       break;
    case ftsfn_BrkID:       u->BrkID_       .assign(str);       break;
    case ftsfn_Msg:         u->Msg_         .assign(str);       break;
    case ftsfn_ExecTime:    u->ActiveTime_  .assign(str);       break;
    case ftsfn_RetryInt:    u->DelayTime_   .assign(str);       break;
    case ftsfn_RetryCount:  u->Retry_       .assign(str);       break;
    case ftsfn_LastExecTime:u->LastExecTime_.assign(str);       break;
    case ftsfn_ExecCount:   u->ExecCount_   .assign(str);       break;
    case ftsfn_FinishTime:  u->FinishTime_  .assign(str);       break;
    case ftsfn_Memo:        u->Memo_        .assign(str);       break;
  }
  u->LastModDate_ = TKDate::Now();
}
/////////////////////////////////////////////////////////////////////////////
K_mf(const Kway::TFieldDesp&) TULoadSchVAC::TULoadSchFieldMaker::GetFieldDesp  (field_n_type n)  const
{
  static Kway::TFieldDesp f[] = {
  { "序號",          6, Kway::ta_xRight_yCenter,  Kway::ta_xRight_yCenter, Kway::tfs_Null,    },
  { "排程",         10, Kway::ta_xCenter_yCenter ,  Kway::ta_xCenter_yCenter , Kway::tfs_CanEdit, 0, &SchTypeProp},
  { "立即執行",      8, Kway::ta_xCenter_yCenter ,  Kway::ta_xCenter_yCenter , Kway::tfs_CanEdit  },
  { "檔案代碼",      8, Kway::ta_xCenter_yCenter ,  Kway::ta_xLeft_yCenter   , Kway::tfs_CanEdit  },
  { "市場別",        8, Kway::ta_xCenter_yCenter ,  Kway::ta_xCenter_yCenter , Kway::tfs_CanEdit, 0, &MarketProp },
  { "券商代號",      8, Kway::ta_xCenter_yCenter ,  Kway::ta_xLeft_yCenter   , Kway::tfs_CanEdit  },
  { "排程時間",      8, Kway::ta_xCenter_yCenter ,  Kway::ta_xLeft_yCenter   , Kway::tfs_CanEdit  },
  { "重試間隔(分)", 10, Kway::ta_xCenter_yCenter ,  Kway::ta_xLeft_yCenter   , Kway::tfs_CanEdit  },
  { "重試次數",      8, Kway::ta_xCenter_yCenter ,  Kway::ta_xLeft_yCenter   , Kway::tfs_CanEdit  },
  { "上次執行時間", 10, Kway::ta_xCenter_yCenter ,  Kway::ta_xLeft_yCenter   , Kway::tfs_Null     },
  { "已執行次數",    8, Kway::ta_xCenter_yCenter ,  Kway::ta_xLeft_yCenter   , Kway::tfs_Null     },
  { "完成時間",     10, Kway::ta_xCenter_yCenter ,  Kway::ta_xLeft_yCenter   , Kway::tfs_Null     },
  { "備註",         30, Kway::ta_xCenter_yCenter ,  Kway::ta_xLeft_yCenter   , Kway::tfs_CanEdit  },
  };
  return f[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TULoadSchVAC::TULoadSchFieldMaker::Get (const iterator& i, field_n_type fieldNo) const
{

  switch(fieldNo)
  {
    case 0: return i->first.as_string();
    case 1: return i->second.SchType_.as_string();
    case 2: return (i->second.Rightaway_?"Y":"N");
    case 3: return i->second.FileCode_    .as_string();
    case 4: return i->second.Market_      .as_string(true);
    case 5: return i->second.BrkID_       .as_string();
    case 6:  { char buf[12];
             sprintf(buf, "%02d:%02d", i->second.ActiveTime_.GetHour(), i->second.ActiveTime_.GetMin());
             return std::string(buf); }
    case 7: return i->second.DelayTime_   .as_string();
    case 8: return i->second.Retry_       .as_string();
    case 9: return i->second.LastExecTime_.as_string();
    case 10: return i->second.ExecCount_   .as_string();
    case 11: return i->second.FinishTime_  .as_string();
    case 12: return i->second.Memo_        .as_string();
  }
  return std::string();
}
//---------------------------------------------------------------------------
K_mf(void) TULoadSchVAC::TULoadSchFieldMaker::Put (TEvContainer& c, iterator& iter,
    field_n_type fieldNo, const std::string& str)
{
  if(fieldNo == 0)
    return;
  TEvContainer::updater u = c.begin_update(iter);
  switch(fieldNo)
  {
    case 1:  u->SchType_    .assign(str);        break;
    case 2:  u->Rightaway_= (str[0] == 'y' || str[0] == 'Y');break;
    case 3:  u->FileCode_    .assign(str);       break;
    case 4:  u->Market_      .assign(str);       break;
    case 5:  u->BrkID_       .assign(str);       break;
    case 6:  u->ActiveTime_  .assign(str);       break;
    case 7:  u->DelayTime_   .assign(str);       break;
    case 8:  u->Retry_       .assign(str);       break;
    case 9:  u->LastExecTime_.assign(str);       break;
    case 10: u->ExecCount_   .assign(str);       break;
    case 11: u->FinishTime_  .assign(str);       break;
    case 12: u->Memo_        .assign(str);       break;
  }
  u->LastModDate_ = TKDate::Now();
}
/////////////////////////////////////////////////////////////////////////////
K_ctor TScheduleCenter::TScheduleCenter ()
    : Timer_(this)
{
   static bool isCreate;
   if(isCreate == false){
      isCreate = true;
      CreateVAC();
      StartTimer();
   }
}
//---------------------------------------------------------------------------
K_dtor TScheduleCenter::~TScheduleCenter()
{
//   delete Inn_;
   delete Recorder_;
   delete Map_;
//   delete ULInn_;
   delete ULRecorder_;
   delete ULMap_;
   DestoryVAC();
}
//---------------------------------------------------------------------------
K_mf(void) TScheduleCenter::CreateVAC (void) //建立遠端VAC
{
//   Inn_ = new TSchInn("./Settings/DLoadSch.rec", 0);
   Map_ = new TScheduleMap();
   Recorder_ = new TScheduleInn(*Map_, std::string(GetProcConfPath()+"/DLoadSch.rec").c_str());
//   Recorder_->BindInn(*Inn_);
   ULMap_ = new TULoadSchMap();
   for(TScheduleMap::iterator i=Map_->begin(); i!=Map_->end(); ++i){
      TScheduleMap::updater u = Map_->begin_update(i);
      if(!(i->second.LastDate_ == TKDate::Now())){
         u->LastExecTime_.Clear();
         u->ExecCount_.assign0();
         u->FinishTime_.Clear();
         u->LastDate_ = TKDate::Now();
         u->Memo_.clear();
      }
      if(i->second.SchType_ == estToday && !(i->second.LastModDate_ == TKDate::Now())){
         u->SchType_.assignT(estNone);
      }
   }
   VAC_ = new TScheduleVAC(*Map_);
   VAMgr_.Add("下載排程", *VAC_);
//   ULInn_ = new TSchInn("./Settings/ULoadSch.rec", 128);
   ULRecorder_ = new TULoadSchInn(*ULMap_, std::string(GetProcConfPath()+"/ULoadSch.rec").c_str());
//   ULRecorder_->BindInn(*ULInn_);
//   ULMap_ = ULRecorder_->GetContainer();
   for(TULoadSchMap::iterator i=ULMap_->begin(); i!=ULMap_->end(); ++i){
      TULoadSchMap::updater u = ULMap_->begin_update(i);
      if(!(i->second.LastDate_ == TKDate::Now())){
         u->LastExecTime_.Clear();
         u->ExecCount_.assign0();
         u->FinishTime_.Clear();
         u->LastDate_ = TKDate::Now();
         u->Memo_.clear();
      }
      if(i->second.SchType_ == estToday && !(i->second.LastModDate_ == TKDate::Now())){
         u->SchType_.assignT(estNone);
      }
   }
   ULVAC_ = new TULoadSchVAC(*ULMap_);
   VAMgr_.Add("上傳排程", *ULVAC_);

   const Kway::TSysMgrObjs& sys = Kway::GetSysMgrObjs();
   sys.SysVAMgr_->Add("工作排程", VAMgr_);
}
//---------------------------------------------------------------------------
K_mf(void) TScheduleCenter::DestoryVAC(void)
{

  const Kway::TSysMgrObjs& sys = Kway::GetSysMgrObjs();
  sys.SysVAMgr_->Del(VAMgr_);
  VAMgr_.Del(*VAC_);
  VAMgr_.Del(*ULVAC_);
  if(VAC_)
    delete VAC_;
  if(ULVAC_)
    delete ULVAC_;
}
//---------------------------------------------------------------------------
K_mf(void) TScheduleCenter::OnTimer(Kway::TTimerBase*)
{
   TKTime tm, gtm;
   tm = TKTime::Now();
   tm.SetSec(0);
   for(TScheduleMap::iterator i=Map_->begin(); i!=Map_->end(); ++i){
      if(i->second.Rightaway_ == false){
         if(i->second.FinishTime_.IsInvalid() == false || i->second.SchType_ == estNone)
            continue;
         if((int)i->second.ExecCount_ >= (int)i->second.Retry_)
            continue;
      }
      gtm = i->second.ActiveTime_;
      gtm.IncMin(i->second.ExecCount_*i->second.DelayTime_);
      gtm.SetSec(0);
      if(gtm == tm || i->second.Rightaway_){
         TScheduleMap::updater u = Map_->begin_update(i);
         u->LastExecTime_ = TKTime::Now();
         u->Rightaway_ = false;
         u->ExecCount_++;
         DoReq(i->first, &i->second);
      }
   }
   for(TULoadSchMap::iterator i=ULMap_->begin(); i!=ULMap_->end(); ++i){
      if(i->second.Rightaway_ == false){
         if(i->second.FinishTime_.IsInvalid() == false || i->second.SchType_ == estNone)
            continue;
         if((int)i->second.ExecCount_ >= (int)i->second.Retry_)
            continue;
      }
      gtm = i->second.ActiveTime_;
      gtm.IncMin(i->second.ExecCount_*i->second.DelayTime_);
      gtm.SetSec(0);
      if(gtm == tm || i->second.Rightaway_){
         TULoadSchMap::updater u = ULMap_->begin_update(i);
         u->LastExecTime_ = TKTime::Now();
         u->Rightaway_ = false;
         u->ExecCount_++;
         DoULReq(i->first, &i->second);
      }
   }
}
//---------------------------------------------------------------------------
K_mf(void) TScheduleCenter::DoReq(TSeqNo seq, const FTSchedule* item)
{
   TFTNewJob job;
//  job.SeqNo_    = 0;    // none
   job.SourceID_ .assign(item->BrkID_.as_string());
   job.SeqNo_    = ((TFTCenter&)GetCenter()).FetchSeqNo(job.SourceID_,item->Market_);    // none
   job.DestID_   = TSEID_;
   job.Market_   = item->Market_;
   job.FileCode_ = item->FileCode_;
   job.Type_     = ftt_Msg;
   job.Mode_     = ftm_Schedule;
   job.SrcSeqNo_ = seq;
   job.CreateTime_ = TKTime::Now();
   job.SrcData_.assign(item->Msg_.as_string());
   TFTCenter& ft = const_cast<TFTCenter&>(GetCenter());
   if(GetExcMgr())
      GetExcMgr()->Alert("XF3020",item->FileCode_.as_string());
   TSysID id = ft.DoReq(job);
   GetBxMgr()->DoSend(job.SourceID_, job.Market_, id);
}
//---------------------------------------------------------------------------
K_mf(void) TScheduleCenter::DoULReq(TSeqNo seq, const SULoadSch* item)
{
   TFTNewJob job;
//   job.SeqNo_      = 0;
   job.SourceID_   .assign(item->BrkID_.as_string());
   job.SeqNo_      = ((TFTCenter&)GetCenter()).FetchSeqNo(job.SourceID_,item->Market_);    // none
   job.DestID_     = TSEID_;
   job.Market_     = item->Market_;
   job.FileCode_   = item->FileCode_;
   job.Type_       = ftt_File;
   job.Mode_       = ftm_Schedule;
   job.SrcData_    .clear();    // 小寫全部轉成大寫
   job.SrcSeqNo_   = seq;
//   job.RecLen_     = req->RecLen_.as_int();
   job.CreateTime_ = Kway::TKTime::Now();
   TFTCenter& ft = const_cast<TFTCenter&>(GetCenter());
   if(GetExcMgr())
      GetExcMgr()->Alert("XF3021",item->FileCode_.as_string());
   TSysID id = ft.DoReq(job);
   GetBxMgr()->DoSend(job.SourceID_, job.Market_, id);
}
//---------------------------------------------------------------------------
K_mf(void) TScheduleCenter::UpdateSchedule(TSeqNo seq, const char* msg)
{
   TScheduleMap::iterator i = Map_->find(seq);
   if(i != Map_->end()){
      TScheduleMap::updater  u = Map_->begin_update(i);
      if(msg){
         u->Memo_.assign(msg);
         u->FinishTime_.Clear();
         if(u->Rightaway_)
            u->Rightaway_  = false;
      }else{
         u->FinishTime_ = TKTime::Now();
         u->LastDate_   = TKDate::Now();
         u->Rightaway_  = false;
         u->Memo_.clear();
      }
   }
}
//---------------------------------------------------------------------------
K_mf(void) TScheduleCenter::UpdateULoadSch(TSeqNo seq, const char* msg)
{
   TULoadSchMap::iterator i = ULMap_->find(seq);
   if(i != ULMap_->end()){
      TULoadSchMap::updater  u = ULMap_->begin_update(i);
      if(msg){
         u->Memo_.assign(msg);
         u->FinishTime_.Clear();
         if(u->Rightaway_)
            u->Rightaway_  = false;
      }else{
         u->FinishTime_ = TKTime::Now();
         u->LastDate_   = TKDate::Now();
         u->Rightaway_  = false;
         u->Memo_.clear();
      }
   }
}
/////////////////////////////////////////////////////////////////////////////

