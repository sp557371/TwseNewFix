//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FTNaming.h"
#include "SysMgrObjs.h"
#include "ExtDef.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx::FT;
//---------------------------------------------------------------------------
static const char* NamingModeStr[] = {"依總公司", "依分公司", "流水號"};
//---------------------------------------------------------------------------
K_mf(std::string) TNamingMode::as_string() const
{
  if((unsigned)Data_ >= numofele(NamingModeStr))
    return std::string();
  return std::string(NamingModeStr[Data_]);
}
//---------------------------------------------------------------------------
K_mf(void) TNamingMode::assign(const std::string& str)
{
  const char* cstr = str.c_str();
  for(int i=0; i<numofele(NamingModeStr); ++i)
    if(strcmp(NamingModeStr[i], cstr) == 0)
    {
      Data_ = (ENamingMode)i;
      break;
    }
}
/////////////////////////////////////////////////////////////////////////////
struct TNamingModeProp : public Kway::TVAGetEditProp
{
   virtual K_mf(TEditStyle)   GetEnums  (const Kway::TViewArea&, const Kway::TViewCursor*,
     size_t fieldNo, Kway::TStrings* result, bool& selectOnly)
   {
     if(result)
     {
       if(result->size() < numofele(NamingModeStr))
       {
         result->clear();
         for(int i=0;i<numofele(NamingModeStr);++i)
           result->push_back(NamingModeStr[i]);
       }
     }
     selectOnly = true;
     return es_AllRecsEqual;
   }
};
static TNamingModeProp  NamingModeProp;
/////////////////////////////////////////////////////////////////////////////
K_mf(const Kway::TFieldDesp&) TNamingVAC::TNamingFieldMaker::GetFieldDesp  (field_n_type n)  const
{
  static Kway::TFieldDesp f[] = {
  { "檔案代碼",  8, Kway::ta_xRight_yCenter,  Kway::ta_xRight_yCenter, Kway::tfs_Null,    },
  { "命名原則", 16, Kway::ta_xLeft_yCenter ,  Kway::ta_xLeft_yCenter , Kway::tfs_CanEdit, 0, &NamingModeProp},
  };
  return f[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TNamingVAC::TNamingFieldMaker::Get (const iterator& i, field_n_type fieldNo) const
{
  switch(fieldNo)
  {
    case  0:  return i->first.as_string();
    case  1:  return i->second.NamingMode_.as_string();
  }
  return std::string();
}
//---------------------------------------------------------------------------
K_mf(void) TNamingVAC::TNamingFieldMaker::Put (TEvContainer& c, iterator& iter,
    field_n_type fieldNo, const std::string& str)
{
  if(fieldNo == 0)
    return;
  TEvContainer::updater u = c.begin_update(iter);
  switch(fieldNo)
  {
    case  1:  u->NamingMode_.assign(str);   break;
  }
}
/////////////////////////////////////////////////////////////////////////////
K_ctor TNamingCenter::TNamingCenter()
{
  CreateVAC();
}
//---------------------------------------------------------------------------
K_dtor TNamingCenter::~TNamingCenter()
{
  if(Recorder_)
    delete Recorder_;
  delete Map_;
  DestoryVAC();
}
//---------------------------------------------------------------------------
K_mf(void) TNamingCenter::CreateVAC (void) //建立遠端VAC
{
   Map_ = new TNamingMap();
   Recorder_ = new TNamingInn(*Map_, GetProcConfPath()+"/Naming.rec");
   VAC_ = new TNamingVAC(*Map_);
   VAMgr_.Add("系統設定", *VAC_);

   const Kway::TSysMgrObjs& sys = Kway::GetSysMgrObjs();
   sys.SysVAMgr_->Add("檔案原則", VAMgr_);
}
//---------------------------------------------------------------------------
K_mf(void) TNamingCenter::DestoryVAC(void)
{
  const Kway::TSysMgrObjs& sys = Kway::GetSysMgrObjs();
  sys.SysVAMgr_->Del(VAMgr_);
  VAMgr_.Del(*VAC_);
  if(VAC_)
    delete VAC_;
}
//---------------------------------------------------------------------------


