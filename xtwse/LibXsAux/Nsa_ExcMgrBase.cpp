/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "Nsa_ExcMgrBase.h"
#include "Nsa_ExcMgr.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TErrLevel impl
/////////////////////////////////////////////////////////////////////////////
const char* TErrLevel::LevUseTypeStr_[] = {
   "1.訊息",
   "2.警告",
   "3.錯誤"
};
//---------------------------------------------------------------------------

K_mf(std::string) TErrLevel::as_string () const
{
   if ((unsigned)Data_ >= numofele(LevUseTypeStr_))
      return std::string();
   return std::string(LevUseTypeStr_[Data_]);
}
//---------------------------------------------------------------------------

K_mf(void) TErrLevel::assign (const std::string& str)
{
   const size_t cnt = numofele(LevUseTypeStr_);
   const char* cstr = str.c_str();

   for (size_t L=0; L<cnt; ++L) {
      if (strcmp(LevUseTypeStr_[L], cstr) == 0) {
         Data_ = (EErrLevel)L;
         break;
      }
   }
}


//////////////////////////////////////////////////////////////////////////////
// class TGroupKind impl
//////////////////////////////////////////////////////////////////////////////
const char* TGroupKind::GroupKindStr_[] = {
   "1.ALL/ALL",
   "2.ONE/ALL",
};
//---------------------------------------------------------------------------

K_mf(std::string) TGroupKind::as_string() const
{
   if((unsigned)Data_ >= numofele(GroupKindStr_))
      return std::string();
   return std::string(GroupKindStr_[Data_]);
}
//---------------------------------------------------------------------------

K_mf(void) TGroupKind::assign(const std::string& str)
{
   const size_t cnd  = numofele(GroupKindStr_);
   const char*  cstr = str.c_str();

   for (size_t L=0; L<cnd; ++L) {
      if(strcmp(GroupKindStr_[L], cstr) == 0) {
         Data_ = (EGroupKind)L;
         break;
      }
   }
}





//---------------------------------------------------------------------------
struct SGroupKinds : public TVAGetEditProp
{
   K_mf(TEditStyle) GetEnums(const TViewArea&, const TViewCursor*, size_t,TStrings* result, bool& selectOnly)
   {
      if (result) {
         std::copy(TGroupKind::GroupKindStr_,
                   TGroupKind::GroupKindStr_+numofele(TGroupKind::GroupKindStr_),
                   std::back_inserter(*result));
      }
      selectOnly = true;
      return es_AllRecsEqual;
   }
};
//////////////////////////////////////////////////////////////////////////////
static SGroupKinds GroupKinds;
//////////////////////////////////////////////////////////////////////////////
K_mf(const TFieldDesp&) TGroupDefFieldMaker::GetFieldDesp(size_t n) const
{
   static TFieldDesp FdDsp[] = {
      { "群組名稱", 12, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Fixed,                  },
      { "群組分類", 16, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, 0, &GroupKinds },
      { "備註"    , 60, ta_xLeft_yCenter,   ta_xLeft_yCenter  , tfs_CanEdit,                },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TGroupDefFieldMaker::Get(const iterator& i, size_t fieldNo) const
{
   switch(fieldNo) {
   case 0:  return i->first.as_string();
   case 1:  return i->second.GroupKind_.as_string();
   case 2:  return i->second.Remark_   .as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(void) TGroupDefFieldMaker::Put(TEvContainer& c, iterator& iter, size_t fieldNo
   , const std::string& str)
{
   if(fieldNo==0)
      return;
   TEvContainer::updater u = c.begin_update(iter);
   switch(fieldNo) {
   case 1:  u->GroupKind_.assign(str);  break;
   case 2:  u->Remark_   .assign(str);  break;
   }
}
//////////////////////////////////////////////////////////////////////////////
K_mf(const TFieldDesp&) TMsgVACr::TMsgFieldMaker::GetFieldDesp (size_t n) const
{
   static TFieldDesp FdDsp[] = {
      { "物件名稱",     20, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null, },
      { "最後狀態",      8, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null, },
      { "最後錯誤訊息", 30, ta_xCenter_yCenter, ta_xLeft_yCenter,   tfs_Null, },
      { "更新時間",     25, ta_xCenter_yCenter, ta_xLeft_yCenter,   tfs_Null, },
      { "詳細資料",     30, ta_xCenter_yCenter, ta_xLeft_yCenter,   tfs_Null, },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TMsgVACr::TMsgFieldMaker::Get(const iterator& i, size_t fieldNo) const
{
   switch(fieldNo) {
   case  0:  return (i->first.empty() ? std::string("N/A"):i->first.as_string());
   case  1:  return i->second.MsgErrLevel_.as_string();
   case  2:  return i->second.MsgErrCode_ .as_string() + "-" + i->second.Msg_.as_string();
   case  3:  return i->second.MsgTime_    .as_string(TKDate::sf_ELong);
   case  4:  return i->second.MsgDetail_;
   }
   return std::string();
}
//////////////////////////////////////////////////////////////////////////////
K_mf(const TFieldDesp&) TObjVAC::TObjFieldMaker::GetFieldDesp (field_n_type n) const
{
   static TFieldDesp f[] = {
      { "群組代號", 8, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Fixed, },
      { "群組狀態", 8, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null,  },
   };
   return f[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TObjVAC::TObjFieldMaker::Get (const iterator& i, field_n_type fieldNo) const
{
   switch(fieldNo) {
   case 0:  return i->first.as_string();
   case 1:
      switch(i->second.ObjErrLevel_) {
      case emlGreen:  return "正常";
      case emlYellow: return "警告";
      case emlRed:    return "錯誤";
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
K_mf(TVACreater*)TObjVAC::GetDetailVACreater(size_t detailID)
{
   switch(detailID) {
   case 0:return &DetailCreater_;
   }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(TVACreater*) TObjVAC::OnMasterViewRowMoved (TViewArea& masterVA, TViewArea*& detailVA, size_t detailID)
{
   TObjViewer& va = static_cast<TObjViewer&>(masterVA);
   const TObjViewer::cursor_type *cur = va.GetCurRow();
   TMsgVACr::TMsgViewer *DetailView = static_cast<TMsgVACr::TMsgViewer*>(detailVA);
   if(cur->IsInvalid())
      DetailView->SetContainer(0);
   else
      DetailView->SetContainer(&cur->GetIterator()->second.MsgMap_);
   return inherited::DetailVACreaterNoChange;
}


///////////////////////////////////////////////////////////////////////
// class TExcMgrBase impl
///////////////////////////////////////////////////////////////////////
K_mf(TErrLevel) TExcMgrBase::GetSysState (const TErrCodeSet& ercode, const TErrLevel&   erlevel,  const TObjID&      objid,
                                          const std::string& msg,    const std::string& msgOwner, const std::string& msgDetail)
{
   SObjStat& ObjStat = ObjMap_.fetch(objid)->second;
   TEvMsgKey MsgKey(msgOwner);

   TEvMsgMap::updater u          = ObjStat.MsgMap_.begin_update(ObjStat.MsgMap_.fetch(MsgKey));
   SMsgValue&         udMsgValue = *u;

   udMsgValue.MsgErrLevel_ = erlevel;
   udMsgValue.MsgErrCode_  = ercode;
   udMsgValue.MsgTime_     = TKDateTimeMS::Now();
   udMsgValue.Msg_         .assign(msg);
   udMsgValue.MsgDetail_   = msgDetail;

   SysState_ = erlevel;

   const TGroupKind gk = GetGroupKind(objid);
   TErrLevel nErrLevel = erlevel;
   if(TEvMsgMap* map = &ObjStat.MsgMap_) {
      TEvMsgMap::const_iterator iend = map->end();
      for(TEvMsgMap::const_iterator i= map->begin(); i!=iend; ++i) {
         if(gk == egkOne && i->second.MsgErrLevel_ == emlGreen) {
            nErrLevel = i->second.MsgErrLevel_;
            break;
         }
         if(nErrLevel < i->second.MsgErrLevel_ && i->second.MsgErrLevel_ != emlNotDef)
            nErrLevel = i->second.MsgErrLevel_;
      }
   }

   if(nErrLevel != ObjStat.ObjErrLevel_) {
      TObjMap::updater upd = ObjMap_.begin_update(ObjMap_.fetch(objid));
      upd->ObjErrLevel_ = nErrLevel;
   }

   TObjMap::const_iterator iend = ObjMap_.end();
   for(TObjMap::const_iterator i= ObjMap_.begin(); i!=iend; ++i) {
      if(SysState_ < i->second.ObjErrLevel_ && i->second.ObjErrLevel_ != emlNotDef)
         SysState_ = i->second.ObjErrLevel_;
   }
   return SysState_;
}
//---------------------------------------------------------------------------
K_mf(TGroupKind) TExcMgrBase::GetGroupKind(const TObjID& objid)
{
   TGroupDef::const_iterator ifind = GroupDef_.find(objid);
   return (ifind != GroupDef_.end() ? ifind->second.GroupKind_:TGroupKind());
}
//---------------------------------------------------------------------------
K_mf(void) TExcMgrBase::RegExcMgrObj(TExcMgrObj* obj)
{
   if(!MgrObjs_.empty())
      if(std::find(MgrObjs_.begin(), MgrObjs_.end(), obj) != MgrObjs_.end())
         return;

   MgrObjs_.push_back(obj);
}
//---------------------------------------------------------------------------
K_mf(void) TExcMgrBase::DelExcMgrObj(TExcMgrObj* obj)
{
   if(!MgrObjs_.empty()) {
      TMgrObjs::iterator L = std::find(MgrObjs_.begin(), MgrObjs_.end(), obj);
      if(L != MgrObjs_.end())
         MgrObjs_.erase(L);
   }
}
//---------------------------------------------------------------------------

K_mf(void) TExcMgrBase::Alert (const char* ec, const std::string& msgOwner, const std::string& msgDetail)
{
   TErrCodeSet ercode;

   ercode.ErrKind_.assign(ec);
   ercode.ErrNum_ .assign(ec+1);
   Alert(ercode, msgOwner, msgDetail);
}
//---------------------------------------------------------------------------

K_mf(void) TExcMgrBase::Alert (const TErrCodeSet& ercode, const std::string& msgOwner, const std::string& msgDetail)
{
   TErrLevel   erlevel;
   TObjID      objid;
   TErrLevel   sysstat;
   std::string ermsg;

   if (!GetErrDef(ercode, erlevel, objid, ermsg)) {
      erlevel = emlNotDef;
      ermsg   = std::string("未定義的錯誤代碼!");
   }
   
   if (ercode.as_string() == "0000")     // alive message
      sysstat = SysState_;
   else
      sysstat = GetSysState(ercode, erlevel, objid, ermsg, msgOwner, msgDetail);

   if (DoLog_)
      WriteEvLog(objid, ercode, erlevel, ermsg, msgOwner, msgDetail);

   if (!MgrObjs_.empty()) {
      TMgrObjs::const_iterator iend = MgrObjs_.end();
      for (TMgrObjs::const_iterator i= MgrObjs_.begin(); i!=iend; ++i) {
         if (*i) {
            (*i)->DoAlert(sysstat, ercode, erlevel, ermsg, msgOwner, msgDetail);
         }
      }
   }
}
//---------------------------------------------------------------------------

K_mf(TExcMgrBase::TExcMgrBaseRef) TExcMgrBase::GetEventMgr ()
{
   static TExcMgrBaseRef ref(new TExcMgr);
   return ref;
}
//---------------------------------------------------------------------------

