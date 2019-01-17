//---------------------------------------------------------------------------
#pragma hdrstop
#include <stdio.h>
#include "stdSettingP.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
using namespace Kstl;
using namespace Kway;
//---------------------------------------------------------------------------
K_ctor TSettingPage::TSettingPage  (const char* fileName, const char* logname)
   : Recorder_(Settings_, fileName)
{
   if(logname)
      LogName_ = std::string(logname);
   else
      LogName_ = std::string("");
}
//---------------------------------------------------------------------------
K_dtor TSettingPage::~TSettingPage  ()
{
}
//---------------------------------------------------------------------------
K_mf(bool) TSettingPage::SettingApply  (TstdSettings* orig)
{
   if(orig && (!LogName_.empty()))
      Log_ = new TTableLogger<TstdFieldMaker>(LogName_.c_str());
   for(TSettingi iter = Settings_.begin(); iter != Settings_.end();iter++){
      if(orig && iter != Settings_.end()){  // ²§°Ê°O¿ý
         TSettingi oiter = orig->find(iter->first);
         if(oiter != orig->end() && iter->second != oiter->second && Log_){
            char item[256];
            sprintf(item,"%d.%s",iter->first,iter->second.Item_.as_string().c_str());
            if(iter->second.Used_ != oiter->second.Used_)
               Log_->WriteMod(std::string(item),1,std::string(oiter->second.Used_?"Y":"N"),
                                                  std::string( iter->second.Used_?"Y":"N"), VA_);
            if(iter->second.Setting_ != oiter->second.Setting_)
               Log_->WriteMod(std::string(item),3,oiter->second.Setting_.as_string(),
                                                   iter->second.Setting_.as_string(), VA_);
         }  
      }
      bool* b = reinterpret_cast<bool*>(iter->second.IsUse_);
      *b = iter->second.Used_;
      switch (iter->second.Type_) {
         case ebool: {
            bool* t = reinterpret_cast<bool*>((iter->second.Variable_));
            *t = (iter->second.Setting_.as_string()[0] == 'Y'? true:false);
            break; }
         case ebyte: {
            typedef Kstl::fixed_num<Kway::byte> Tbyte;
            Tbyte* t = reinterpret_cast<Tbyte*>((iter->second.Variable_));
            t->assign(iter->second.Setting_.as_string());
            break; }
         case eword: {
            Kway::word* t = reinterpret_cast<Kway::word*>((iter->second.Variable_));
            *t = iter->second.Setting_.as_int();
            break; }
         case eTKTime:{
            TKTime* t = reinterpret_cast<TKTime*>((iter->second.Variable_));
            t->assign(iter->second.Setting_.as_string());
            break;}
         case eTKDate:{
            TKDate* t = reinterpret_cast<TKDate*>((iter->second.Variable_));
            t->assign(iter->second.Setting_.as_string());
            break;}
         case eTStkQty:{
            TStkQty* t = reinterpret_cast<TStkQty*>((iter->second.Variable_));
            t->assign(iter->second.Setting_.as_string());
            break;}
         case eTStkPri:{
            TStkPri* t = reinterpret_cast<TStkPri*>((iter->second.Variable_));
            t->assign(iter->second.Setting_.as_string());
            break;}
         case eTStkAmt:{
            TStkAmt* t = reinterpret_cast<TStkAmt*>((iter->second.Variable_));
            t->assign(iter->second.Setting_.as_string());
            break;}
         case echar_ary:{
            TDesp* t = reinterpret_cast<TDesp*>((iter->second.Variable_));
            t->assign(iter->second.Setting_.as_string());
            break;}
         case eNone:
         default:break;
      }
   }
   if(orig && Log_)
      delete Log_;
   return true;
}
//---------------------------------------------------------------------------
K_mf(bool) TSettingPage::SettingCancel  ()
{
   return true;
}
//---------------------------------------------------------------------------
K_mf(void) TSettingPage::Reg(void* isuse, const char* item, const char* setting, eType vtype, void* variable)
{
   for(TSettingi iter = Settings_.begin(); iter != Settings_.end();iter++){
      if(std::string(item) == iter->second.Item_.as_string()){
         TSettingu u = Settings_.begin_update(iter);
         *((bool*)isuse) = u->Used_;
         u->IsUse_    = ((bool*)isuse);
         u->Type_     = vtype;
         u->Variable_ = variable;
         if(variable == 0)
            u->Setting_.assign("N/A");
         return;
      }
   }
   TKey Sz_;
   Sz_ = Settings_.size();
   SstdSetting rec;
   rec.Used_     = *((bool*)isuse);
   rec.Item_.assign(item);
   if(variable == 0) 
      rec.Setting_.assign("N/A");
   else
      rec.Setting_.assign(setting);
   rec.IsUse_    = ((bool*)isuse);
   rec.Type_     = vtype;
   rec.Variable_ = variable;
   Settings_.insert(std::make_pair(Sz_,rec));
   return;
}
//---------------------------------------------------------------------------
K_mf(TViewArea*) TSettingPage::GetVA  ()
{
   TViewControls   vcs;
   vcs.set(vco_Setting);
   TstdVA* SVA = (new TstdVA(this, &this_type::SettingApply, vcs, &Settings_));
   return (SVA);
}
//---------------------------------------------------------------------------
K_mf(TaViewArea) TSettingPage::CreateNewVA  ()
{
   TViewControls   vcs;
   vcs.set(vco_Setting);
   TstdVA* SVA = (new TstdVA(this, &this_type::SettingApply, vcs, &Settings_));
   VA_ = SVA;
   return TaViewArea(SVA);
}
//---------------------------------------------------------------------------

























