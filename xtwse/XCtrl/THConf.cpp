#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "THConf.h"
#include "ExtDef.h"
#include "KIniFile.h"
#include "SysMgrObjs.h"
#include "ExcMgrBase.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Twse;
using namespace Kway::Tw::Stk;
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&) TPvcConfFieldMaker::GetFieldDesp(size_t n) const
{
   static Kway::TFieldDesp FdDsp[]=
   {
     { "Socket ID"   , 8,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Fixed    , },
     { "啟用"        , 4,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit  , },
     { "連線密碼"    , 8,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit  , },
     { "Source Port" ,10,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit  , },
     { "Term ID"     ,10,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit  , },
     { "備註"        ,20,  Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  , Kway::tfs_CanEdit  , },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TPvcConfFieldMaker::Get(const TPvcConfsi& i, size_t fieldNo) const
{
   switch(fieldNo)
   {
      case  0:  return i->first.as_string();
      case  1:  return i->second.Active_?"Y":"N";
      case  2:  return i->second.Passwd_.as_string();
      case  3:  return i->second.SrcPort_.as_string();
      case  4:  return i->second.TermID_.as_string();
      case  5:  return i->second.Remark_.as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(void) TPvcConfFieldMaker::Put(TPvcConfs& c, TPvcConfsi& iter, size_t fieldNo, const std::string& str)
{
   TPvcConfs::updater u = c.begin_update(iter);
   switch (fieldNo) {
      case 1: u->Active_ = (str[0]=='Y' || str[0]=='y');  break;
      case 2: u->Passwd_.assign(str);  break;
      case 3: u->SrcPort_.assign(str); break;
      case 4: u->TermID_.assign(str);  break;
      case 5: u->Remark_.assign(str);  break;
   }
}
//---------------------------------------------------------------------------
K_mf(bool) TPvcConfViewer::SettingApply()
{
   // 在此判斷修改之處並進行INI寫入
   if(1){
      TKIniFile IniFile(std::string(GetConfPath()+"/Main.ini").c_str());
      for(TPvcConfsi L = Shadow_.begin(); L != Shadow_.end(); L++){  //判斷新增及修改
         TPvcConfsi iter = Origin_->find(L->first);
         if(iter == Origin_->end()){ // 新建立的設定, 要檢查從[券商代號]開始的所有設定是否己建立
            std::string tmp = FixSesID_.as_string().substr(1,4);
            IniFile.WriteStrings("Main","Broker",tmp);
            tmp = FixSesID_.as_string().substr(0,5);
            if(FixSesID_.as_string().length() == 7)
               IniFile.WriteStrings(tmp,"FixSes",FixSesID_.as_string());
            if(FixSesID_.as_string().length() == 5){ // 成交回報設定
               IniFile.WriteString(FixSesID_.as_string(),"MatSesID",L->first.as_string());
               IniFile.WriteString(FixSesID_.as_string(),"MatPassword",L->second.Passwd_.as_string());
               IniFile.WriteString(FixSesID_.as_string(),"MatSrcPort",L->second.SrcPort_.as_string());
               IniFile.WriteString(FixSesID_.as_string(),"TermID",L->second.TermID_.as_string());
               IniFile.WriteString(FixSesID_.as_string(),"Active",L->second.Active_?"Y":"N");
            }else if(FixSesID_.as_string().length() == 7){ // FIX 連線所對應的TMP
               IniFile.WriteStrings(FixSesID_.as_string(),"TMPID",L->first.as_string());
               IniFile.WriteString(FixSesID_.as_string()+"-"+L->first.as_string(),
                                   "Password",L->second.Passwd_.as_string());
               IniFile.WriteString(FixSesID_.as_string()+"-"+L->first.as_string(),
                                   "SourcePort",L->second.SrcPort_.as_string());
               IniFile.WriteString(FixSesID_.as_string()+"-"+L->first.as_string(),
                                   "TermID",L->second.TermID_.as_string());
               IniFile.WriteString(FixSesID_.as_string()+"-"+L->first.as_string(),
                                   "Active",L->second.Active_?"Y":"N");
            }
            continue;
         }
         // 只是修改設定的話
         if(FixSesID_.as_string().length() == 5){ // 成交回報設定
            if(iter->second.Passwd_ != L->second.Passwd_)
               IniFile.WriteString(FixSesID_.as_string(),"MatPassword",L->second.Passwd_.as_string());
            if(iter->second.SrcPort_ != L->second.SrcPort_)
               IniFile.WriteString(FixSesID_.as_string(),"MatSrcPort",L->second.SrcPort_.as_string());
         }else if(FixSesID_.as_string().length() == 7){ // FIX 連線所對應的TMP
            if(iter->second.Passwd_ != L->second.Passwd_)
               IniFile.WriteString(FixSesID_.as_string()+"-"+L->first.as_string(),
                                   "Password",L->second.Passwd_.as_string());
            if(iter->second.SrcPort_ != L->second.SrcPort_)
               IniFile.WriteString(FixSesID_.as_string()+"-"+L->first.as_string(),
                                   "SourcePort",L->second.SrcPort_.as_string());
            if(iter->second.TermID_ != L->second.TermID_)
               IniFile.WriteString(FixSesID_.as_string()+"-"+L->first.as_string(),
                                   "TermID",L->second.TermID_.as_string());
            if(iter->second.Active_ != L->second.Active_)
               IniFile.WriteString(FixSesID_.as_string()+"-"+L->first.as_string(),
                                   "Active",L->second.Active_?"Y":"N");
         }
      }
      for(TPvcConfsi L = Origin_->begin(); L != Origin_->end(); L++){//判斷刪除
         TPvcConfsi iter = Shadow_.find(L->first);
         if(iter == Shadow_.end()){
            if(FixSesID_.as_string().length() == 5){ // 成交回報設定
            }else if(FixSesID_.as_string().length() == 7){ // FIX 連線所對應的TMP
               IniFile.EraseSection(FixSesID_.as_string()+"-"+L->first.as_string());
               IniFile.EraseValue(FixSesID_.as_string(),"TMPID",L->first.as_string());
               TStrings Tmps;
               int TmpCnt = IniFile.ReadStrings(FixSesID_.as_string(),"TMPID",Tmps);
               if(TmpCnt == 0)
                  IniFile.EraseSection(FixSesID_.as_string());
            }
         }
      }
      *Origin_ = Shadow_;
   }
   std::string procname = FixSesID_.as_string();
   std::string objname  = "bxSesMgr";
   GetExcMgr()->CmdReq(procname,objname,1);
   if(FixSesID_.as_string().length() == 7){//成交回報線路修改不用通知XMonitor
      procname = "XMonitor";
      objname  = "ProcMon";
      GetExcMgr()->CmdReq(procname,objname,1,FixSesID_.as_string());
   }
   return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&) TMWConfFieldMaker::GetFieldDesp(size_t n) const
{
   static Kway::TFieldDesp FdDsp[]=
   {
     { "項次"       ,  4,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Fixed   , },
     { "項目名稱"   , 30,  Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  , Kway::tfs_Null     , },
     { "設定內容"   ,  8,  Kway::ta_xCenter_yCenter,  Kway::ta_xRight_yCenter , Kway::tfs_CanEdit  , },
     { "備註"       , 20,  Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  , Kway::tfs_CanEdit  , },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TMWConfFieldMaker::Get(const TMWConfsi& i, size_t fieldNo) const
{
   std::string items[]={
                         "FIX Session 連線密碼",
                         "Appia MiddleWare Port",
                         "看到這行表示有bug啦!"
                       };
   switch(fieldNo)
   {
      case  0:  return i->first.as_format(1,0);
      case  1:  return items[i->first];
      case  2:  return i->second.Conf_;
      case  3:  return i->first.get_orig()==1?"未設定則使用系統預設值":i->second.Remark_.as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(void) TMWConfFieldMaker::Put(TMWConfs& c, TMWConfsi& iter, size_t fieldNo, const std::string& str)
{
   if(fieldNo < 2)
      return;
   TMWConfs::updater u = c.begin_update(iter);
   switch (fieldNo) {
      case 2: u->Conf_ = str;          break;
      case 3: u->Remark_.assign(str);  break;
   }
}
//---------------------------------------------------------------------------
K_mf(bool) TMWConfViewer::SettingApply()
{
   // 在此判斷修改之處並進行INI寫入
   if(FixSesID_.as_string().length() < 7) // 排除成交回報
      return false;
   if(Shadow_.size() < emw_End)
      return false;
   TKIniFile IniFile(std::string(GetConfPath()+"/Main.ini").c_str());
   std::string items[]={"Password","AppiaMWPort",};
   for(int L = 0; L < emw_End; L++){
      TMWConfsi Lo = Origin_->find(L);
      TMWConfsi Ls = Shadow_.find(L);
      if(Lo == Origin_->end() || Lo->second.Conf_ != Ls->second.Conf_){
         std::string tmp = FixSesID_.as_string().substr(1,4);
         IniFile.WriteStrings("Main","Broker",tmp);
         tmp = FixSesID_.as_string().substr(0,5);
         IniFile.WriteStrings(tmp,"FixSes",FixSesID_.as_string());
         if(Ls->second.Conf_.empty())
            IniFile.EraseValue(FixSesID_.as_string(),items[L],Ls->second.Conf_);
         else
            IniFile.WriteString(FixSesID_.as_string(),items[L],Ls->second.Conf_);
      }
   }
   *Origin_ = Shadow_;
   return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&) TFixIDFieldMaker::GetFieldDesp(size_t n) const
{
   static Kway::TFieldDesp FdDsp[]=
   {
     { "FIX Session ID"      , 10,  Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter, Kway::tfs_Fixed , },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TFixIDFieldMaker::Get(const TTmpConfsi& i, size_t fieldNo) const
{
   switch(fieldNo)
   {
      case  0:  return i->first.as_string().length()==5?(i->first.as_string()+"-成交"):i->first.as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&) TBrkConfFieldMaker::GetFieldDesp(size_t n) const
{
   static Kway::TFieldDesp FdDsp[]=
   {
     { "券商代號"      , 6,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Fixed , },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TBrkConfFieldMaker::Get(const TBrkConfi& i, size_t fieldNo) const
{
   switch(fieldNo)
   {
      case  0:  return i->first.as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
K_mf(size_t) TTmpConfVACr::GetDetails(const char**& a)
{
   static const char* desp[] = {"TMP 連線設定","FIX 密碼及MW Port設定"};
   a = desp;
   return numofele(desp);
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*)TTmpConfVACr::GetDetailVACreater(size_t detailID)
{
   switch(detailID){
     case 0:return &PvcConfVACr_;
     case 1:return &MWConfVACr_;
     }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*) TTmpConfVACr::OnMasterViewRowMoved
   (Kway::TViewArea& masterVA,Kway::TViewArea*& detailVA, size_t detailID)
{
   TTmpConfViewer& va=static_cast<TTmpConfViewer&>(masterVA);
   const TTmpConfViewer::cursor_type *cur=va.GetCurRow();
   if(cur == 0 || cur->IsInvalid()){
      CurFixSesID_ = TFixSesID();
      switch (detailID){
         case 0:{
            TPvcConfViewer *DetailView =static_cast<TPvcConfViewer*>(detailVA);
//            DetailView->SetContainer(0, TFixSesID());
            PvcConfVACr_.SetContainer(0);
            break;
         }
         case 1:{
            TMWConfViewer *DetailView =static_cast<TMWConfViewer*>(detailVA);
//            DetailView->SetContainer(0, TFixSesID());
            MWConfVACr_.SetContainer(0);
            break;
         }
      }
   }else{
      CurFixSesID_ = cur->GetIterator()->first;
      switch (detailID){
         case 0:
            static_cast<TPvcConfViewer*>(detailVA)->SetContainer(&cur->GetIterator()->second.PvcConf_,CurFixSesID_);
            PvcConfVACr_.SetContainer(&cur->GetIterator()->second.PvcConf_);
            break;
         case 1:
            static_cast<TMWConfViewer*>(detailVA)->SetContainer(&cur->GetIterator()->second.MWConf_,CurFixSesID_);
            MWConfVACr_.SetContainer(&cur->GetIterator()->second.MWConf_);
            break;
      }
   }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(void)TTmpConfVACr::TTmpConEvHandler::OnContainerBfErase(const ev_container& c, const ev_iterator& i, size_type)
{
   TTmpConfsi iter = static_cast<const TTmpConfsi&>(i);
   if(iter->first.as_string().length() == 7){//成交回報線路修改不用通知XMonitor
      std::string procname = "XMonitor";
      std::string objname  = "ProcMon";
      GetExcMgr()->CmdReq(procname,objname,2,iter->first.as_string());
   }
   TKIniFile IniFile(std::string(GetConfPath()+"/Main.ini").c_str());
   std::string tmp = iter->first.as_string().substr(0,5);
   IniFile.WriteStrings(tmp,"Del_FixSes",iter->first.as_string());
   IniFile.EraseValue(tmp,"FixSes",iter->first.as_string());
}
//---------------------------------------------------------------------------
K_mf(size_t) TBrkConfVACr::GetDetails(const char**& a)
{
   static const char* desp[] = {"券商代號"};
   a = desp;
   return numofele(desp);
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*)TBrkConfVACr::GetDetailVACreater(size_t detailID)
{
   switch(detailID){
     case 0:return DetailCreater_;
   }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(Kway::TaViewArea) TBrkConfVACr::CreateNewVA(){
   TViewControls   vcs;
   vcs.set(vco_Erase);
   vcs.set(vco_Insert);
//   vcs.reset();
//   if(BrkConf_ == 0)
//      return Kway::TaViewArea(new TBrkConfViewer(0));
//   else
      return Kway::TaViewArea(new TBrkConfViewer(BrkConf_, TBrkConfFieldMaker(),vcs));
}
//---------------------------------------------------------------------------
K_mf(void)TBrkConfVACr::TBrkConEvHandler::OnContainerBfErase(const ev_container& c, const ev_iterator& i, size_type)
{
   TBrkConfi iter = static_cast<const TBrkConfi&>(i);
   TTmpConfs* tmpconf = &(iter->second);
   std::string procname = "XMonitor";
   std::string objname  = "ProcMon";
   for(TTmpConfsi it2 = tmpconf->begin(); it2 != tmpconf->end(); it2++){
      if(it2->first.as_string().length() == 7){//成交回報線路修改不用通知XMonitor
         GetExcMgr()->CmdReq(procname,objname,2,it2->first.as_string());
      }
   }
   TKIniFile IniFile(std::string(GetConfPath()+"/Main.ini").c_str());
   std::string tmp = iter->first.as_string();
   IniFile.WriteStrings("Main","Del_Broker",tmp);
   IniFile.EraseValue("Main","Broker",tmp);
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*) TBrkConfVACr::OnMasterViewRowMoved
   (Kway::TViewArea& masterVA,Kway::TViewArea*& detailVA, size_t detailID)
{
   TBrkConfViewer& va=static_cast<TBrkConfViewer&>(masterVA);
   const TBrkConfViewer::cursor_type *cur=va.GetCurRow();
   TTmpConfViewer *DetailView =static_cast<TTmpConfViewer*>(detailVA);
   if(cur == 0 || cur->IsInvalid()){
      DetailView->SetContainer(0);
      DetailCreater_->SetContainer(0);
   }else{
      if(cur->GetIterator()->second.size()==0){
         std::string SesID;
         if(GetProcMkt() == m_Unknow){
            SesID = "T"+cur->GetIterator()->first.as_string();
            cur->GetIterator()->second.fetch(TFixSesID(SesID));
            SesID = "O"+cur->GetIterator()->first.as_string();
            cur->GetIterator()->second.fetch(TFixSesID(SesID));
         }else{
            SesID = (GetProcMkt()==m_Tse?"T":"O")+cur->GetIterator()->first.as_string();
            cur->GetIterator()->second.fetch(TFixSesID(SesID));
         }
      }
      DetailCreater_->SetContainer(&(cur->GetIterator()->second));
      DetailView->SetContainer(&(cur->GetIterator()->second));
   }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// ErrCntMgr
/////////////////////////////////////////////////////////////////////////////
K_mf(const Kway::TFieldDesp&) TErrCntFieldMaker::GetFieldDesp(size_t n) const
{
   static Kway::TFieldDesp FdDsp[]=
   {
     { "子系統"     ,20,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Fixed  , },
     { "錯誤次數"   , 8,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit    , },
     { "錯誤上限"   , 8,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "備註"        ,20,  Kway::ta_xCenter_yCenter, Kway::ta_xLeft_yCenter  , Kway::tfs_CanEdit  , },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TErrCntFieldMaker::Get(const TSubSysErrCnti& i, size_t fieldNo) const
{
   char buf[32];
   switch(fieldNo){
      case 0:  switch(i->first.as_int()){
              case 10: return i->first.as_string()+"-TSE連線子系統";
			  // 2013/12/17 miguel add
              case 12: return i->first.as_string()+"-TSE連線子系統";
			  // add end
              case 20: return i->first.as_string()+"-TSE檔案傳輸子系統";
              case 30: return i->first.as_string()+"-TSE一般交易子系統";
              case 31: return i->first.as_string()+"-TSE標借交易子系統";
              case 32: return i->first.as_string()+"-TSE盤後交易子系統";
              case 40: return i->first.as_string()+"-TSE零股交易子系統";
              case 41: return i->first.as_string()+"-TSE標購交易子系統";
              case 70: return i->first.as_string()+"-TSE拍賣交易子系統";
              case 91: return i->first.as_string()+"-OTC連線子系統";
              case 92: return i->first.as_string()+"-OTC檔案傳輸子系統";
              case 93: return i->first.as_string()+"-OTC一般交易子系統";
              case 90: return i->first.as_string()+"-OTC標借交易子系統";
              case 98: return i->first.as_string()+"-OTC盤後交易子系統";
              case 94: return i->first.as_string()+"-OTC零股交易子系統";
              case 97: return i->first.as_string()+"-OTC標購交易子系統";
              default: return i->first.as_string();
           }
           break;
      case  1:  return std::string(buf, sprintf(buf,"%d",i->second.ErrCnt_));
      case  2:  return std::string(buf, sprintf(buf,"%d",i->second.ErrLimit_));
      case  3:{
      	if(i->second.ErrCnt_ >= i->second.ErrLimit_)
      	   i->second.Remark_.assign("已到達錯誤次數上限!");
      	else
      	   i->second.Remark_.clear();
      	return i->second.Remark_.as_string();
      }
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(void) TErrCntFieldMaker::Put(TSubSysErrCnt& c, TSubSysErrCnti& iter, size_t fieldNo, const std::string& str)
{
   TSubSysErrCnt::updater u = c.begin_update(iter);
   switch (fieldNo) {
      case 1: u->ErrCnt_ = atoi(str.c_str());  break;
      case 3: u->Remark_.assign(str);  break;
   }
}
//---------------------------------------------------------------------------
K_mf(bool) TErrCntViewer::SettingApply()
{
   std::string procname = FixSesID_.as_string();
   std::string objname  = "bxSesMgr";
   char buf[32];
   for(TSubSysErrCnti iter = Origin_->begin(); iter != Origin_->end(); iter++){
      TSubSysErrCnti it2 = Shadow_.find(iter->first);
      if(it2 != Shadow_.end() && iter->second != it2->second){
         sprintf(buf,"%4s%7s%02d%04d%04d",procname.substr(1,4).c_str(),procname.c_str(),
                                   it2->first.as_int(), it2->second.ErrCnt_, it2->second.ErrLimit_);
	      GetExcMgr()->CmdReq(procname,objname,2,std::string(buf));
      }
   }
   *Origin_ = Shadow_;
   return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&) TFixIDErrCntFieldMaker::GetFieldDesp(size_t n) const
{
   static Kway::TFieldDesp FdDsp[]=
   {
     { "FIX Session ID"      , 10,  Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter, Kway::tfs_Fixed , },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TFixIDErrCntFieldMaker::Get(const TTmpErrCntsi& i, size_t fieldNo) const
{
   switch(fieldNo)
   {
      case  0:  return i->first.as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&) TBrkErrCntFieldMaker::GetFieldDesp(size_t n) const
{
   static Kway::TFieldDesp FdDsp[]=
   {
     { "券商代號"      , 6,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Fixed , },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TBrkErrCntFieldMaker::Get(const TBrkErrCnti& i, size_t fieldNo) const
{
   switch(fieldNo)
   {
      case  0:  return i->first.as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
K_mf(size_t) TTmpErrCntVACr::GetDetails(const char**& a)
{
   static const char* desp[] = {"TMP連線錯誤管理"};
   a = desp;
   return numofele(desp);
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*)TTmpErrCntVACr::GetDetailVACreater(size_t detailID)
{
   switch(detailID){
     case 0:return &ErrCntVACr_;
     }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*) TTmpErrCntVACr::OnMasterViewRowMoved
   (Kway::TViewArea& masterVA,Kway::TViewArea*& detailVA, size_t detailID)
{
   TTmpErrCntViewer& va=static_cast<TTmpErrCntViewer&>(masterVA);
   const TTmpErrCntViewer::cursor_type *cur=va.GetCurRow();
   if(cur == 0 || cur->IsInvalid()){
      CurFixSesID_ = TFixSesID();
      TErrCntViewer *DetailView =static_cast<TErrCntViewer*>(detailVA);
      ErrCntVACr_.SetContainer(0);
   }else{
      CurFixSesID_ = cur->GetIterator()->first;
      static_cast<TErrCntViewer*>(detailVA)->SetContainer(&cur->GetIterator()->second,CurFixSesID_);
      ErrCntVACr_.SetContainer(&cur->GetIterator()->second);
   }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(size_t) TBrkErrCntVACr::GetDetails(const char**& a)
{
   static const char* desp[] = {"券商代號"};
   a = desp;
   return numofele(desp);
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*)TBrkErrCntVACr::GetDetailVACreater(size_t detailID)
{
   switch(detailID){
     case 0:return DetailCreater_;
   }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(Kway::TaViewArea) TBrkErrCntVACr::CreateNewVA(){
   TViewControls   vcs;
   vcs.set(vco_Erase);
   vcs.set(vco_Insert);
   return Kway::TaViewArea(new TBrkErrCntViewer(BrkErrCnt_, TBrkErrCntFieldMaker(),vcs));
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*) TBrkErrCntVACr::OnMasterViewRowMoved
   (Kway::TViewArea& masterVA,Kway::TViewArea*& detailVA, size_t detailID)
{
   TBrkErrCntViewer& va=static_cast<TBrkErrCntViewer&>(masterVA);
   const TBrkErrCntViewer::cursor_type *cur=va.GetCurRow();
   TTmpErrCntViewer *DetailView =static_cast<TTmpErrCntViewer*>(detailVA);
   if(cur == 0 || cur->IsInvalid()){
      DetailView->SetContainer(0);
      DetailCreater_->SetContainer(0);
   }else{
      DetailCreater_->SetContainer(&(cur->GetIterator()->second));
      DetailView->SetContainer(&(cur->GetIterator()->second));
   }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
K_ctor TBrkConfPage::TBrkConfPage()
{
   TKIniFile IniFile(std::string(GetConfPath()+"/Main.ini").c_str());
   TStrings Brks;
   std::string tmp;
   BrkConf_     = new TBrkConf();
   BrkErrCnt_   = new TBrkErrCnt();
   BrkErrCntVACr_ = new TBrkErrCntVACr(BrkErrCnt_);
   int BrkCnt = IniFile.ReadStrings("Main","Broker",Brks);
   // Broker
   if(BrkCnt == 0){
      Brks.push_back(std::string("000K"));
      BrkCnt = 1;
   }
   for(int L = 0; L < BrkCnt; L++){
      TBrkConfi Brki = BrkConf_->fetch(TBrkID(Brks[L]));
      //Market
      for(int mkt = 0; mkt < 2; mkt++){
         tmp = (mkt==0?"T":"O")+Brks[L];
         TStrings Sess;
         int SesCnt = IniFile.ReadStrings(tmp,"FixSes", Sess);
         if(SesCnt == 0)
            continue;
         TTmpConfsi Tmpi = Brki->second.fetch(TFixSesID(tmp));
         // Mat TMP Session
         std::string tmpmat = IniFile.ReadString(tmp,"MatSesID","");
         if(!tmpmat.empty()){
            TPvcConfsi Pvci = Tmpi->second.PvcConf_.fetch(TSocketID(tmpmat));
            tmpmat = IniFile.ReadString(tmp,"MatPassword","");
            Pvci->second.Passwd_.assign(tmpmat);
            tmpmat = IniFile.ReadString(tmp,"MatSrcPort","");
            Pvci->second.SrcPort_.assign(tmpmat);
            tmpmat = IniFile.ReadString(tmp,"Active","Y");
            Pvci->second.Active_ = (tmpmat[0]=='Y' || tmpmat[0] == 'y');
            tmpmat = IniFile.ReadString(tmp,"TermID","");
            Pvci->second.TermID_.assign(tmpmat);
         }
         // FIX Session ID
         for(int i = 0; i < SesCnt; i++){
            TTmpConfsi Tmpi = Brki->second.fetch(TFixSesID(Sess[i]));
            tmp = IniFile.ReadString(Sess[i],"Password","");
            Tmpi->second.MWConf_.SetValue(emw_Passwd,tmp);
            tmp = IniFile.ReadString(Sess[i],"AppiaMWPort","");
            Tmpi->second.MWConf_.SetValue(emw_DstPort,tmp);
            TStrings Tmps;
            int TmpCnt = IniFile.ReadStrings(Sess[i],"TMPID",Tmps);
            // TMP Socket ID
            for(int ts = 0; ts < TmpCnt; ts++){  
               TPvcConfsi Pvci = Tmpi->second.PvcConf_.fetch(TSocketID(Tmps[ts]));
               tmp = IniFile.ReadString(Sess[i]+"-"+Tmps[ts],"Password","");
               Pvci->second.Passwd_.assign(tmp);
               tmp = IniFile.ReadString(Sess[i]+"-"+Tmps[ts],"SourcePort","");
               Pvci->second.SrcPort_.assign(tmp);
               tmp = IniFile.ReadString(Sess[i]+"-"+Tmps[ts],"TermID","");
               Pvci->second.TermID_.assign(tmp);
               tmp = IniFile.ReadString(Sess[i]+"-"+Tmps[ts],"Active","Y");
               Pvci->second.Active_ = (tmp[0]=='Y' || tmp[0] == 'y');
            }
         }
      }
   }
   BrkConfVACr_ = new TBrkConfVACr(BrkConf_);
   if(GetExcMgr())
      GetExcMgr()->RegCmdMgrObj(this,"LineErrCntMgr");
}
//---------------------------------------------------------------------------
K_dtor TBrkConfPage::~TBrkConfPage()
{
   delete BrkConfVACr_;
   delete BrkConf_;
   delete BrkErrCntVACr_;
   delete BrkErrCnt_;
}
//---------------------------------------------------------------------------
K_mf(bool) TBrkConfPage::DoCommand(Kway::word cmdid, std::string msg)
{
   switch(cmdid){
      case 1:{
         struct{
         	TBrkID               brkid;
         	char_ary<char,7,' '> sesid;
         	TSubSys              subsys;
         	TPasswd              count;
         	TPasswd              limit;
         } ErrCnt_;
         memcpy(&ErrCnt_, msg.c_str(), sizeof(ErrCnt_));
         TFixSesID fixid(ErrCnt_.sesid.as_string());
         TSubSysErrCnti iter = BrkErrCnt_->fetch(ErrCnt_.brkid)->second.fetch(fixid)->second.fetch(ErrCnt_.subsys);
         iter->second.ErrCnt_ = ErrCnt_.count.as_int();
         iter->second.ErrLimit_ = ErrCnt_.limit.as_int();
         break;
      }
      default:
         break;
   }
}
//---------------------------------------------------------------------------
