//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
#include "ExcMgrBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TExcMgrBase* ExcMgrBase;
K_fn(TExcMgrBase*) GetExcMgr()
{
   return ExcMgrBase;
}
//---------------------------------------------------------------------------
K_fn(void) SetExcMgr(TExcMgrBase* ExcMgr)
{
   ExcMgrBase = ExcMgr;
}
//---------------------------------------------------------------------------
K_mf(std::string) TErrLevel::as_string() const
{
   if((unsigned)Data_ >= numofele(LevUseTypeStr))
      return std::string();
   return std::string(LevUseTypeStr[Data_]);
}
//---------------------------------------------------------------------------
K_mf(void) TErrLevel::assign(const std::string& str)
{
   const char* cstr = str.c_str();
   for(int L=0; L<numofele(LevUseTypeStr); ++L){
      if(strcmp(LevUseTypeStr[L],cstr)==0){
         Data_ = (EErrLevel)L;
         break;
      }
   }
}
//---------------------------------------------------------------------------
K_mf(std::string) TGroupKind::as_string() const
{
   if((unsigned)Data_ >= numofele(GroupKindStr))
      return std::string();
   return std::string(GroupKindStr[Data_]);
}
//---------------------------------------------------------------------------
K_mf(void) TGroupKind::assign(const std::string& str)
{
   const char* cstr = str.c_str();
   for(int L=0; L<numofele(GroupKindStr); ++L){
      if(strcmp(GroupKindStr[L],cstr)==0){
         Data_ = (EGroupKind)L;
         break;
      }
   }
}
//---------------------------------------------------------------------------
struct SGroupKinds : public TVAGetEditProp
{
   K_mf(TEditStyle) GetEnums(const TViewArea&, const TViewCursor*, size_t,TStrings* result, bool& selectOnly){
      if(result)
         std::copy(GroupKindStr, GroupKindStr+numofele(GroupKindStr), std::back_inserter(*result));
      selectOnly = true;
      return es_AllRecsEqual;
   }
};
static SGroupKinds GroupKinds;
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&) TGroupDefFieldMaker::GetFieldDesp(size_t n) const
{
   static Kway::TFieldDesp FdDsp[]=
   {
     { "群組名稱"    ,12,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Fixed  , },
     { "群組分類"    ,16,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, 0 , &GroupKinds},
     { "備註"        ,60,  Kway::ta_xLeft_yCenter  ,  Kway::ta_xLeft_yCenter  , Kway::tfs_CanEdit, },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TGroupDefFieldMaker::Get(const iterator& i, size_t fieldNo) const
{
   switch(fieldNo)
   {
      case  0:  return i->first.as_string();
      case  1:  return i->second.GroupKind_.as_string();
      case  2:  return i->second.Remark_.as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(void) TGroupDefFieldMaker::Put(TEvContainer& c, iterator& iter, size_t fieldNo, const std::string& str)
{
   if(fieldNo==0)
      return;
   TEvContainer::updater u = c.begin_update(iter);
   switch(fieldNo)
   {
      case  1:  u->GroupKind_.assign(str);                      break;
      case  2:  u->Remark_.assign(str);                         break;
   }
}
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&) TMsgVACr::TMsgFieldMaker::GetFieldDesp(size_t n) const
{
   static Kway::TFieldDesp FdDsp[]=
   {
     { "物件名稱"    ,20,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null, },
     { "最後狀態"    ,8 ,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null, },
     { "最後訊息"    ,30,  Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  , Kway::tfs_Null, },
     { "更新時間"    ,25,  Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  , Kway::tfs_Null, },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TMsgVACr::TMsgFieldMaker::Get(const TEvMsgMapi& i, size_t fieldNo) const
{
   switch(fieldNo)
   {
      case  0:  return (i->first.empty()?std::string("N/A"):i->first.as_string());
      case  1:  return i->second.MsgErrLevel_.as_string();
      case  2:  return (i->second.MsgErrCode_.as_string()+"-"+i->second.Msg_.as_string());
      case  3:  return i->second.MsgTime_.as_string(TKDate::sf_ELong);
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&)  TObjVAC::TObjFieldMaker::GetFieldDesp  (field_n_type n) const
{
   static Kway::TFieldDesp f[] =
   {
      {"群組代號",  8 , Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Fixed,},
      {"群組狀態",  8 , Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null,},
   };
   return f[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TObjVAC::TObjFieldMaker::Get (const iterator& i, field_n_type fieldNo) const
{
   switch(fieldNo){
      case 0:  return i->first.as_string();
      case 1:  switch(i->second.ObjErrLevel_){
                  case emlGreen:  return std::string("正常");
                  case emlYellow: return std::string("警告");
                  case emlGrey:   return std::string("檢核");
//                  case emlBlue:   return std::string("儲存");
                  case emlRed:    return std::string("錯誤");
                  default: return std::string();
               }
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(size_t) TObjVAC::GetDetails(const char**& a)
{
   static const char* desp[] = {"子系統-事件代碼"};
   a = desp;
   return numofele(desp);
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*)TObjVAC::GetDetailVACreater(size_t detailID)
{
   switch(detailID){
      case 0:return &DetailCreater_;
   }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*) TObjVAC::OnMasterViewRowMoved
   (Kway::TViewArea& masterVA,Kway::TViewArea*& detailVA, size_t detailID)
{
   TObjViewer& va=static_cast<TObjViewer&>(masterVA);
   const TObjViewer::cursor_type *cur=va.GetCurRow();
   TMsgVACr::TMsgViewer *DetailView =static_cast<TMsgVACr::TMsgViewer*>(detailVA);
   if(cur->IsInvalid())
      DetailView->SetContainer(0);
   else
      DetailView->SetContainer(cur->GetIterator()->second.MsgMap_);
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------


