//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "SetsVAC.h"
#include "SysMgrObjs.h"
#include "ExtDef.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx;
static  const char**  SetNames_;
static  const char**  SetMemos_;
static  size_t        SetCnt_;
//---------------------------------------------------------------------------
struct TModuleSetNameProp : public Kway::TVAGetEditProp
{
   virtual K_mf(TEditStyle)   GetEnums  (const Kway::TViewArea&, const Kway::TViewCursor* cursor,
     size_t fieldNo, Kway::TStrings* result, bool& selectOnly)
   {
     if(result)
     {
       result->clear();
       if(cursor->DistanceBegin() < SetCnt_)
         result->push_back(SetNames_[cursor->DistanceBegin()]);
     }
     selectOnly = true;
     return es_EnumsByKey;
   }
};
//---------------------------------------------------------------------------
static TModuleSetNameProp  ModuleSetNameProp;
/////////////////////////////////////////////////////////////////////////////
K_mf(const Kway::TFieldDesp&) TSetsVAC::TSetsFieldMaker::GetFieldDesp (field_n_type n) const
{
  static Kway::TFieldDesp f[] =
  {
    {"項目", 5,  Kway::ta_xLeft_yCenter,  Kway::ta_xLeft_yCenter, Kway::tfs_Null,    },
    {"名稱", 30, Kway::ta_xLeft_yCenter,  Kway::ta_xLeft_yCenter, Kway::tfs_CanEdit, 0, &ModuleSetNameProp },
    {"設定", 40, Kway::ta_xLeft_yCenter,  Kway::ta_xLeft_yCenter, Kway::tfs_CanEdit, },
    {"備註", 50, Kway::ta_xLeft_yCenter,  Kway::ta_xLeft_yCenter, Kway::tfs_Null,    },
  };
  return f[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TSetsVAC::TSetsFieldMaker::Get (const iterator& i,field_n_type fieldNo) const
{
  switch(fieldNo)
  {
    case if_ID:     return i->first.as_string();
    case if_Name:   return i->second.Name_.as_string();
    case if_Data:   return i->second.Data_.as_string();
    case if_Memo:   return i->second.Memo_.as_string();
  }
  return std::string();
}
//---------------------------------------------------------------------------
K_mf(void) TSetsVAC::TSetsFieldMaker::Put (TEvContainer& c, iterator& iter, field_n_type fieldNo, const std::string& str)
{
  if(fieldNo == 0)
    return;
  TEvContainer::updater u = c.begin_update(iter);
  switch(fieldNo)
  {
    case if_Name:
      u->Name_.assign(str);
      for(size_t i=0; i<SetCnt_; ++i)
        if(std::string(SetNames_[i]) == str) {
          u->Memo_.assign(SetMemos_[i]);
          break;
        }
      break;
    case if_Data:   u->Data_.assign(str);   break;
  }
}
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
K_ctor TSettingBase::TSettingBase(const std::string& vaName, const std::string& recName)
    : Timer_  (this),
      VAName_ (vaName),
      RecName_(recName)
{
   Timer_.Start(1000*2, &this_type::OnTimer);  // 2秒後才CreateVAC();
}
//---------------------------------------------------------------------------
K_dtor TSettingBase::~TSettingBase()
{
//  if(Inn_)
//    delete Inn_;
   if(Recorder_)
      delete Recorder_;
   delete Map_;
   DestoryVAC();
}
//---------------------------------------------------------------------------
K_mf(void) TSettingBase::OnTimer(Kway::TTimerBase*)
{
   AssignItemStr(SetNames_, SetMemos_, SetCnt_);
   CreateVAC();
   Timer_.Stop();
}
//---------------------------------------------------------------------------
K_mf(void) TSettingBase::CreateVAC (void) //建立遠端VAC
{
//   Inn_ = new TSetsInn(".\\Settings\\" + RecName_, 128);
//   Recorder_ = new TSetsRec(*Inn_);
//   Recorder_->BindInn(*Inn_);
   Map_ = new TItemSets();
   Recorder_ = new TSetsRec(*Map_, std::string(GetProcConfPath()+ RecName_).c_str());
//   Map_ = Recorder_->GetContainer();
   VAC_ = new TSetsVAC(*Map_);
   VAMgr_.Add(VAName_, *VAC_);

   const Kway::TSysMgrObjs& sys = Kway::GetSysMgrObjs();
   sys.SysVAMgr_->Add("模組設定", VAMgr_);
   LoadDefault();
}
//---------------------------------------------------------------------------
K_mf(void) TSettingBase::DestoryVAC(void)
{
  const Kway::TSysMgrObjs& sys = Kway::GetSysMgrObjs();
  sys.SysVAMgr_->Del(VAMgr_);
  VAMgr_.Del(*VAC_);
  if(VAC_)
    delete VAC_;
}
//---------------------------------------------------------------------------
K_mf(void) TSettingBase::LoadDefault()
{
  TItemSets::iterator i;
  TItemSets::updater  u;
  for(size_t i=1;i<=SetCnt_;i++)
  {
    if(Map_->find(i) == Map_->end())
    {
      u = Map_->begin_update(Map_->fetch(i));
      u->Name_.assign(SetNames_[i-1]);
      u->Data_.assign("");
      u->Memo_.assign(SetMemos_[i-1]);
    }
  }
}
//---------------------------------------------------------------------------

