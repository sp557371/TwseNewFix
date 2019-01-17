//---------------------------------------------------------------------------
#pragma hdrstop

#include "ExcMgr.h"
#include "ExtDef.h"
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
struct STLevUseTypes : public TVAGetEditProp
{
   //Debug: 傳入沒有使用的參數?
   K_mf(TEditStyle) GetEnums(const TViewArea&, const TViewCursor*, size_t, TStrings* result, bool& selectOnly)
   {
      if(result)
         std::copy(LevUseTypeStr, LevUseTypeStr+numofele(LevUseTypeStr), std::back_inserter(*result));
      selectOnly = true;
      return es_AllRecsEqual;
   }
};
//---------------------------------------------------------------------------
static STLevUseTypes LevUseTypes;
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&) TErrMsgDFieldMaker::GetFieldDesp(size_t n) const
{
   static Kway::TFieldDesp FdDsp[]=
   {
     { "事件代碼"    , 5,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null   , },
     { "事件等級"    , 5,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, 0 , &LevUseTypes},
     { "發送警示"    , 5,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
     { "群組別"      , 5,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
     { "事件說明"    , 60, Kway::ta_xLeft_yCenter  ,  Kway::ta_xLeft_yCenter  , Kway::tfs_CanEdit, },
     { "採取措施"    , 60, Kway::ta_xLeft_yCenter  ,  Kway::ta_xLeft_yCenter  , Kway::tfs_CanEdit, },
     { "備註"        , 20, Kway::ta_xLeft_yCenter  ,  Kway::ta_xLeft_yCenter  , Kway::tfs_CanEdit, },
   };
   if(GDef_ && n == 2){
      FdDsp[n].GetEditProp_ = (TVAGetEditProp*)&Keys_;
   }
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TErrMsgDFieldMaker::Get(const TEvDi& i, size_t fieldNo) const
{
   switch(fieldNo)
   {
      case  0:  return i->first.as_string();
      case  1:  return i->second.EvLevel_.as_string();
      case  2:  return i->second.Alert_?"Y":"";
      case  3:  return i->second.ObjID_.as_string();
      case  4:  return i->second.EvMsg_.as_string();
      case  5:  return i->second.Action_.as_string();
      case  6:  return i->second.Remark_.as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(void) TErrMsgDFieldMaker::Put(TEvMapD& c, TEvDi& iter, size_t fieldNo, const std::string& str)
{
   if(fieldNo==0)
      return;
   TEvDu u = c.begin_update(iter);
   switch(fieldNo)
   {
      case  1:  u->EvLevel_.assign(str);                       break;
      case  2:  u->Alert_ = (str[0]=='Y' || str[0] == 'y');    break;
      case  3:  u->ObjID_.assign(str);                         break;
      case  4:  u->EvMsg_.assign(str);                         break;
      case  5:  u->Action_.assign(str);                        break;
      case  6:  u->Remark_.assign(str);                        break;
   }
}
//=============================================================================
K_mf(const Kway::TFieldDesp&)  TErrMsgVAC::TErrMsgFieldMaker::GetFieldDesp  (field_n_type n) const
{
   static Kway::TFieldDesp f[] =
   {
      {"系統事件代號",  12 , Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter, Kway::tfs_Null,},
   };
   return f[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TErrMsgVAC::TErrMsgFieldMaker::Get (const iterator& i, field_n_type fieldNo) const
{
   switch(fieldNo){
      case  0:  if(i->first.as_string()==std::string("XC"))
                   return "XC-XController";
                else if(i->first.as_string()==std::string("XM"))
                   return "XM-XMonitor";
                else if(i->first.as_string()==std::string("XT"))
                   return "XT-XTester";
                else if(i->first.as_string()==std::string("XX"))
                   return "XX-XTwse";
                else if(i->first.as_string()==std::string("XF"))
                   return "XF-XFT";
                else if(i->first.as_string()==std::string("XA"))
                   return "XA-CronJobAgent";
                else
                   return i->first.as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(size_t) TErrMsgVAC::GetDetails(const char**& a)
{
   static const char* desp[] = {"子系統-事件代碼"};
   a = desp;
   return numofele(desp);
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*)TErrMsgVAC::GetDetailVACreater(size_t detailID)
{
   switch(detailID){
      case 0:return &DetailCreater_;
   }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*) TErrMsgVAC::OnMasterViewRowMoved
   (Kway::TViewArea& masterVA,Kway::TViewArea*& detailVA, size_t detailID)
{
   TMsgViewer& va=static_cast<TMsgViewer&>(masterVA);
   const TMsgViewer::cursor_type *cur=va.GetCurRow();
   TErrMsgDViewer *DetailView =static_cast<TErrMsgDViewer*>(detailVA);
   if(cur->IsInvalid())
      DetailView->SetContainer(0);
   else
      DetailView->SetContainer(&(cur->GetIterator()->second));
   return inherited::DetailVACreaterNoChange;
}
//=============================================================================
// 事件記錄器的fieldmaker
//=============================================================================
K_mf(const Kway::TFieldDesp&) TEvLogFieldMaker::GetFieldDesp(size_t n) const
{
   static Kway::TFieldDesp FdDsp[]=
   {
     { "序號"        , 6,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Fixed , },
     { "群組別"      , 5,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "子系統"      ,10,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "事件代碼"    , 5,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "事件等級"    , 5,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "發生時間"    ,23,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "事件說明"    ,30,  Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  , Kway::tfs_Null  , },
     { "訊息"        ,20,  Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  , Kway::tfs_Null  , },
     { "採取措施"    ,30,  Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  , Kway::tfs_Null  , },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TEvLogFieldMaker::Get(const TEvLogi& i, size_t fieldNo) const
{
   switch(fieldNo)
   {
      case  0: {char buf[32];
                sprintf(buf,"%d",i->first);
                return std::string(buf);
               }
      case  1:  return i->second.ObjID_.as_string();
      case  2:  return i->second.SubSys_.as_string();
      case  3:  return i->second.EvCode_.as_string();
      case  4:  return i->second.EvLevel_.as_string();
      case  5:  return i->second.Time_.as_string(TKDate::sf_ELong);
      case  6:  return i->second.EvMsg_.as_string();
      case  7:  return i->second.ExtMsg_.as_string();
      case  8:  return i->second.Action_.as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_ctor TExcMgr::TExcMgr(bool isHB, bool dolog, bool agent) :inherited()
{
   ErrMap_   = new TEvMap();
   Recorder_ = new TEvRecorder(ErrMap_,(char*)std::string(GetConfPath()+"EventMsg.rec").c_str());
   GDRecorder_ = new TGDRecorder(*GroupDef_,(char*)std::string(GetConfPath()+"GroupDef.rec").c_str());
   IsAgent_ = agent;
   Count_ = 0;
   ObjVA_      = new TObjVAC(ObjMap_);
   switch(GetProcMkt()){
      case Kway::Tw::Stk::m_Tse: EvAlert_.Market_.assign("T"); break;
      case Kway::Tw::Stk::m_Otc: EvAlert_.Market_.assign("O"); break;
      default                  : EvAlert_.Market_.assign(" "); break;
   }
   EvAlert_.ProgName_.assign(GetProgName());
   EvAlert_.ProcName_.assign(GetProgName());
   EvAlert_.Host_.clear();
   if(agent){
      delete Recorder_;
      delete GDRecorder_;
      EvSesMgr_ = new TEvSesMgr(EvCenter_, true);
//      VAMgr_.Add(GetCount()+".系統狀態檢視",*ObjVA_);
      VAMgr_.Add(GetCount()+".事件通知連線",EvSesVAMgr_);
      RegExcMgrObj(EvSesMgr_);
   }else{
      EvCenter_ = new TEvCenter();
//      VAMgr_.Add(GetCount()+".訊息中心",*EvCenter_->GetVA());
      ErrVA_    = new TErrMsgVAC(*ErrMap_, GroupDef_);
      GroupDefVA_ = new TGroupDefVACr(GroupDef_);
      VAMgr_.Add(GetCount()+".事件代碼對照表",*ErrVA_);
//      VAMgr_.Add(GetCount()+".群組類別定義",*GroupDefVA_);
      EvSesMgr_ = new TEvSesMgr(EvCenter_, false);
      VAMgr_.Add(GetCount()+".事件接收連線",EvSesVAMgr_);
      VAMgr_.Add(GetCount()+".TMP系統事件查詢",LocalVAMgr_);
//      LocalVAMgr_.Add("1.系統狀態檢視",*ObjVA_);
   }
   EvSesVAMgr_.Add("1.連線狀態",EvSesMgr_->GetStatusVACreater());
   EvSesVAMgr_.Add("2.連線設定",EvSesMgr_->GetSettingVACreater());
   DoLog_ = dolog;
   if(DoLog_){
      EvLogs_ = new TEvLogs();
      EvLogVA_ = new TEvLogVACr(EvLogs_);
      EvLogRecorder_ = new TLogRecorder(*EvLogs_,(char*)std::string(GetProcLogPath()+"EvLogs.rec").c_str());
      if(agent)
         VAMgr_.Add(GetCount()+".事件記錄器",*EvLogVA_);
      else
         LocalVAMgr_.Add("1.事件記錄器",*EvLogVA_);
      EvLogFile_.Open(std::string(GetProcLogPath()+"EvLogs.log").c_str(),(TFileMode)(fmOpenAlways|fmAppend));
      EvLogFile_.Seek(0,fsEnd);
   }else{
      EvLogs_        = 0;
      EvLogVA_       = 0;
      EvLogRecorder_ = 0;
   }
   HB_.ErrKind_.assign("00");
   HB_.ErrNum_.assign("0000");
//   Timer_= new TExcTimer(this);
//   if(isHB)
//      Timer_->Start(60*1000, &this_type::OnTimer);
}
//---------------------------------------------------------------------------
K_ctor TExcMgr::~TExcMgr()
{
   EvLogFile_.Close();
//   Timer_->Stop();
//   delete Timer_;
   if(DoLog_){
      delete EvLogRecorder_;
      delete EvLogs_;
      if(IsAgent_)
         VAMgr_.Del(*EvLogVA_);
      else
         LocalVAMgr_.Del(*EvLogVA_);
      delete EvLogVA_;
   }
   if(!IsAgent_){
      VAMgr_.Del(*ErrVA_);
      VAMgr_.Del(*GroupDefVA_);
      LocalVAMgr_.Del(*ObjVA_);
      VAMgr_.Del(LocalVAMgr_);
//      VAMgr_.Del(*EvCenter_->GetVA());
      delete GDRecorder_;
      delete GroupDefVA_;
      delete ErrVA_;
      delete Recorder_;
   }else{
      VAMgr_.Del(*ObjVA_);
      DelExcMgrObj(EvSesMgr_);
   }
   VAMgr_.Del(EvSesVAMgr_);
   EvSesVAMgr_.Del(EvSesMgr_->GetStatusVACreater());
   EvSesVAMgr_.Del(EvSesMgr_->GetSettingVACreater());
   delete ObjVA_;
   delete EvSesMgr_;
   if(!IsAgent_)
      delete EvCenter_;
   delete ErrMap_;
}
//---------------------------------------------------------------------------
K_mf(bool) TExcMgr::GetErrDef(TErrCode& ercode, TErrLevel& erlevel, TObjID& objid, std::string& ermsg, std::string& action,bool& alert)
{
   TEvi iter = ErrMap_->find(ercode.ErrKind_);
   if(iter == ErrMap_->end()){
      if(ercode == HB_){
         ermsg   = std::string("Alive!");
         return true;
      }
      return false;
   }
   TEvMapD* mapd = (TEvMapD*)(&iter->second);
   TEvDi L = mapd->find(ercode.ErrNum_);
   if(L == mapd->end()){
      if(ercode == HB_){
         ermsg   = std::string("Alive!");
         return true;
      }
      return false;
   }
   objid   = L->second.ObjID_;
   erlevel = L->second.EvLevel_;
   ermsg   = L->second.EvMsg_.as_string();
   action  = L->second.Action_.as_string();
   alert   = L->second.Alert_;
   return true;
}
//---------------------------------------------------------------------------
K_mf(void) TExcMgr::WriteEvLog(TObjID objid, TErrCode ercode, TErrLevel erlevel, 
                               std::string& msg, std::string& action, std::string& obj, std::string& extmsg)
{
   if(ercode == HB_)
      return;
   SEvLog  evlog;
   evlog.Time_    = TKDateTimeMS::Now();
   evlog.ObjID_   = objid;
   evlog.EvCode_  = ercode;
   evlog.EvLevel_ = erlevel;
   evlog.Object_.assign(obj);
   evlog.EvMsg_.assign(msg);
   evlog.ExtMsg_.assign(extmsg);
   evlog.Action_.assign(action);
   if(EvLogs_)
      EvLogs_->insert(std::make_pair(EvLogs_->size(),evlog));
   if(EvLogFile_.IsOpened()){
      char buf[256];
      sprintf(buf,"%s %-4s %s %s %-20s %-60s %-60s %-30s",
                  evlog.Time_.as_string(TKDate::sf_ELong).c_str(),
                  evlog.ObjID_.as_string().c_str(),
                  evlog.EvCode_.as_string().c_str(),
                  evlog.EvLevel_.as_string().c_str(),
                  evlog.Object_.as_string().c_str(),
                  msg.c_str(), action.c_str(),
                  extmsg.c_str());
      EvLogFile_.WriteLN(buf);
   }
}
//---------------------------------------------------------------------------
K_mf(void) TExcMgr::OnTimer(TTimerBase*)
{
   Alert(HB_,"HB");
}
//---------------------------------------------------------------------------
/*
K_mf(void) TExcMgr::Alert(const char* ec, std::string obj)
{
   TErrCode    ercode;
   ercode.assign(ec);
   Alert(ercode,obj);
}
*/
//---------------------------------------------------------------------------
K_mf(void) TExcMgr::Alert(const char* ec, std::string obj, std::string s)
{
   TErrCode    ercode;
   ercode.assign(ec);
   Alert(ercode,obj,s);
}
//---------------------------------------------------------------------------
K_mf(void) TExcMgr::Alert(TErrCode ercode, std::string obj, std::string s)
{
   inherited::Alert(ercode,obj,s);
   if(ercode.as_string() == "000000")
      return;
   if(!IsAgent_ && EvCenter_){
      EvAlert_.DateTime_.assign(TKDateTimeMS::Now().as_string());
      EvAlert_.EvCode_.assign(ercode.as_string());
      EvAlert_.Object_.assign(obj);
      EvAlert_.Remark_.assign(s);
      EvCenter_->AddEv(EvAlert_);
   }
}
//---------------------------------------------------------------------------
K_mf(void) TExcMgr::CmdReq(std::string& procname, std::string& objname, Kway::word cmdid, std::string msg)
{
   EvSesMgr_->SendCmdReq(procname, objname, cmdid, msg);
}  
//---------------------------------------------------------------------------


