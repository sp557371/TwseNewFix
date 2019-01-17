#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FTSysSetting.h"
#include "SysMgrObjs.h"
#include "ExtDef.h"
#include "ExtFuncs.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx;
const std::string BROKER_FMT = "%B";
const std::string MARKET_FMT = "%M";
//---------------------------------------------------------------------------
K_fn(void) Kway::Tw::Bx::MakePath(std::string& path)
{
   #ifdef K_WINDOWS
   #  define SPL_ERR    '/'
   #  define SPL_OK     '\\'
   #else
   #  define SPL_ERR    '\\'
   #  define SPL_OK     '/'
   #endif
   if(path[path.length()-1] != SPL_OK)
     path += SPL_OK;
}
//---------------------------------------------------------------------------
K_fn(TFTSysSetting&) Kway::Tw::Bx::GetSysSetting()
{
  static TFTSysSetting ftSysSetting;
  return ftSysSetting;
}
//---------------------------------------------------------------------------
static const char* SetNameStr[] = {
  "下載檔案暫存路徑",
  "上傳檔案暫存路徑",
  "成交回報檔名",
  "連線錯誤記錄檔名",
  "傳送要求間隔時間",
  "檔案傳送作業逾時時間",
};
static const char* SetMemoStr[] = {
  "系統暫存檔儲存路徑",
  "回存路徑",
  "成交檔名 (%B :券商代號, %M :市場別)",
  "連線問題的LOG存放路徑與檔名",
  "前一個傳送工作結束後的等待時間(秒)",
  "檔案傳送後多久未得到交易所回覆(分)",
};
//---------------------------------------------------------------------------
struct TSetNameProp : public Kway::TVAGetEditProp
{
   virtual K_mf(TEditStyle)   GetEnums  (const Kway::TViewArea&, const Kway::TViewCursor* cursor,
     size_t fieldNo, Kway::TStrings* result, bool& selectOnly)
   {
     if(result)
     {
       result->clear();
       if(cursor->DistanceBegin() < numofele(SetNameStr))
         result->push_back(SetNameStr[cursor->DistanceBegin()]);
     }
     selectOnly = true;
     return es_EnumsByKey;
   }
};
static TSetNameProp  SetNameProp;
/////////////////////////////////////////////////////////////////////////////
K_mf(const Kway::TFieldDesp&) TFTSysSetsVAC::TFTSysSetsFieldMaker::GetFieldDesp (field_n_type n) const
{
  static Kway::TFieldDesp f[] =
  {
    {"項目", 5,  Kway::ta_xLeft_yCenter,  Kway::ta_xLeft_yCenter, Kway::tfs_Null,    },
    {"名稱", 20, Kway::ta_xLeft_yCenter,  Kway::ta_xLeft_yCenter, Kway::tfs_CanEdit, 0, &SetNameProp },
    {"設定", 40, Kway::ta_xLeft_yCenter,  Kway::ta_xLeft_yCenter, Kway::tfs_CanEdit, },
    {"備註", 40, Kway::ta_xLeft_yCenter,  Kway::ta_xLeft_yCenter, Kway::tfs_Null,    },
  };
  return f[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TFTSysSetsVAC::TFTSysSetsFieldMaker::Get (const iterator& i,field_n_type fieldNo) const
{
  switch(fieldNo)
  {
    case sf_ID:     return i->first.as_string();
    case sf_Name:   return i->second.Name_.as_string();
    case sf_Data:   return i->second.Data_.as_string();
    case sf_Memo:   return i->second.Memo_.as_string();
  }
  return std::string();
}
//---------------------------------------------------------------------------
K_mf(void) TFTSysSetsVAC::TFTSysSetsFieldMaker::Put (TEvContainer& c, iterator& iter, field_n_type fieldNo, const std::string& str)
{
  if(fieldNo == 0)
    return;
  TEvContainer::updater u = c.begin_update(iter);
  switch(fieldNo)
  {
    case sf_Name:
      u->Name_.assign(str);
      for(size_t i=0; i<numofele(SetNameStr); ++i)
        if(std::string(SetNameStr[i]) == str) {
          u->Memo_.assign(SetMemoStr[i]);
          break;
        }
      break;
    case sf_Data:   u->Data_.assign(str);   break;
  }
}
/////////////////////////////////////////////////////////////////////////////
K_mf(const Kway::TFieldDesp&) TFTModeVAC::TFTModeFieldMaker::GetFieldDesp (field_n_type n) const
{
  static Kway::TFieldDesp f[] =
  {
    {"File Code"    , 10, Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Fixed,   },
    {"Append Mode"  , 12, Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
    {"Record Length", 12, Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
    {"Delay Time"   , 12, Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
    {"不等待回覆"   , 10, Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
    {"備註"         , 40, Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  , Kway::tfs_CanEdit, },
  };
  return f[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TFTModeVAC::TFTModeFieldMaker::Get (const iterator& i,field_n_type fieldNo) const
{
  switch(fieldNo)
  {
    case 0:     return i->first.as_string();
    case 1:     return (i->second.IsAppend_?"Y":" ");
    case 2:     return i->second.RecLen_.as_string();
    case 3:     return i->second.DelayTime_.as_string();
    case 4:     return (i->second.Ackless_?"Y":" ");
    case 5:     return i->second.Remark_.as_string();
  }
  return std::string();
}
//---------------------------------------------------------------------------
K_mf(void) TFTModeVAC::TFTModeFieldMaker::Put (TEvContainer& c, iterator& iter, field_n_type fieldNo, const std::string& str)
{
  if(fieldNo == 0)
    return;
  TEvContainer::updater u = c.begin_update(iter);
  switch(fieldNo)
  {
    case 1: if(str[0] == 'Y' || str[0] == 'y')
                u->IsAppend_ = true;
            else
                u->IsAppend_ = false;
            break;
    case 2: u->RecLen_.assign(str);   break;
    case 3: u->DelayTime_.assign(str);   break;
    case 4: u->Ackless_ = (str[0] == 'Y' || str[0] == 'y'); break;
    case 5: u->Remark_.assign(str);   break;
  }
}
/////////////////////////////////////////////////////////////////////////////
K_ctor TFTSysSetting::TFTSysSetting()
{
   static bool isCreate;
   if(isCreate == false){
      SysInit_ = new TSettingPage(std::string(GetProcConfPath()+"/SysInit.rec").c_str(),
                                  std::string(GetProcLogPath()+"/ModifyLog/SysInit.log").c_str());
      SysInit_->Reg(&UseHostID_    , "主機序號(不可重複)","0",ebyte,&HostID_);
      SysInit_->SettingApply();
      SetHostID(HostID_);
      CreateVAC();
      isCreate = true;
   }
   FTModeRec_ = new TFTModeRec(FTMode_, std::string(GetProcConfPath()+"/FTMode.rec").c_str());
}
//---------------------------------------------------------------------------
K_dtor TFTSysSetting::~TFTSysSetting()
{
//   if(Inn_)
//      delete Inn_;
   if(Recorder_)
      delete Recorder_;
   delete Map_;
   DestoryVAC();
   delete FTModeRec_;
   delete SysInit_;
}
//---------------------------------------------------------------------------
K_mf(void) TFTSysSetting::CreateVAC (void) //建立遠端VAC
{
//   Inn_ = new TFTInn(".\\Settings\\SysSet.rec", 128);
//   Recorder_ = new TFTSysRec(*Inn_);
//   Recorder_->BindInn(*Inn_);
   Map_ = new TFTSysSets();
   Recorder_ = new TFTSysRec(*Map_,std::string(GetProcConfPath()+"/SysSet.rec").c_str());
//   Map_ = Recorder_->GetContainer();
   VAC_ = new TFTSysSetsVAC(*Map_);
   FTModeVA_ = new TFTModeVAC(FTMode_);
   VAMgr_.Add("0.系統設定", *VAC_);
   VAMgr_.Add("1.檔案模式", *FTModeVA_);
   VAMgr_.Add("2.啟動設定", *SysInit_);

   const Kway::TSysMgrObjs& sys = Kway::GetSysMgrObjs();
//   sys.SysVAMgr_->Add("系統設定", VAMgr_);
   LoadDefault();
}
//---------------------------------------------------------------------------
K_mf(void) TFTSysSetting::DestoryVAC(void)
{
   const Kway::TSysMgrObjs& sys = Kway::GetSysMgrObjs();
   sys.SysVAMgr_->Del(VAMgr_);
   VAMgr_.Del(*SysInit_);
   VAMgr_.Del(*VAC_);
   VAMgr_.Del(*FTModeVA_);
   if(VAC_)
      delete VAC_;
}
//---------------------------------------------------------------------------
K_mf(std::string) TFTSysSetting::GetSetting(TSysSettingItem item) const
{
   switch(item){
      case ssi_TempSave:
         return ChangeYMD(std::string(GetProcLogPath()+"download").c_str());
      case ssi_SaveTo:
         return ChangeYMD(GetProcLogPath().c_str());
      default:{
         TFTSysSets::iterator i = Map_->find(item);
         return (i==Map_->end() ? std::string():i->second.Data_.as_string());
      }
   }
}
//---------------------------------------------------------------------------
K_mf(void) TFTSysSetting::LoadDefault()
{
  TFTSysSets::iterator i;
  TFTSysSets::updater  u;
  for(int i=1;i<=numofele(SetNameStr);i++)
  {
    if(Map_->find(i) == Map_->end())
    {
      u = Map_->begin_update(Map_->fetch(i));
      u->Name_.assign(SetNameStr[i-1]);
      u->Data_.assign("");
      u->Memo_.assign(SetMemoStr[i-1]);
    }
  }
}
//---------------------------------------------------------------------------
K_mf(std::string) TFTSysSetting::GetFormatedStr(const std::string& str,
   const std::string& broker, KStk::EMarket market) const
{
   std::string output = str;
   std::string::size_type pos;
   while((pos = output.find(BROKER_FMT)) != std::string::npos) {
      output.erase (pos, BROKER_FMT.length());
      output.insert(pos, broker);
   }
   while((pos = output.find(MARKET_FMT)) != std::string::npos) {
      output.erase (pos, MARKET_FMT.length());
      output.insert(pos, market==KStk::m_Tse ? "TSE":"OTC");
   }
   return output;
}
//---------------------------------------------------------------------------
K_mf(bool) TFTSysSetting::IsAppendMode(TFileCode fcode)
{
   TFTMode::iterator iter = FTMode_.find(fcode);
   if(iter != FTMode_.end() && iter->second.IsAppend_)
      return true;
   return false;
}
//---------------------------------------------------------------------------
K_mf(size_t) TFTSysSetting::GetRecLength(TFileCode fcode)
{
   TFTMode::iterator iter = FTMode_.find(fcode);
   if(iter != FTMode_.end() && iter->second.RecLen_ > 0)
      return (Kway::word)iter->second.RecLen_;
   return 0;
}
//---------------------------------------------------------------------------
K_mf(size_t) TFTSysSetting::GetDelayTime(TFileCode fcode)
{
   TFTMode::iterator iter = FTMode_.find(fcode);
   if(iter != FTMode_.end() && iter->second.DelayTime_ > 0)
      return (Kway::word)iter->second.DelayTime_;
   return 0;
}
//---------------------------------------------------------------------------
K_mf(bool) TFTSysSetting::IsAckless(TFileCode fcode)
{
   TFTMode::iterator iter = FTMode_.find(fcode);
   if(iter != FTMode_.end() && iter->second.Ackless_)
      return true;
   return false;
}
//---------------------------------------------------------------------------



