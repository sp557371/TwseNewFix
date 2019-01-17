//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseSets.h"
#include "SysMgrObjs.h"
#include "FTSysSetting.h"
#include <stdlib.h>
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx;
//---------------------------------------------------------------------------
static const char* SetNameStr[] = {
  "K02存放路徑",
  "K10存放路徑",
};
static const char* SetMemoStr[] = {
  "XTester 產生K02檔案存放路徑",
  "XFT 產生K10檔案存放路徑",
};
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseSetting::TTwseSetting()
    : inherited("TWSE設定", "TwseSet.rec")

{
}
//---------------------------------------------------------------------------
K_dtor TTwseSetting::~TTwseSetting()
{
}
//---------------------------------------------------------------------------
K_mf(void) TTwseSetting::AssignItemStr(const char**& setnames, const char**& setmemos, size_t& setcnt)
{
  setnames = SetNameStr;
  setmemos = SetMemoStr;
  setcnt   = std::min(numofele(SetNameStr), numofele(SetMemoStr));
}
//---------------------------------------------------------------------------
K_mf(std::string) TTwseSetting::GetSetting(TTwseSettingItem item) const
{
  TItemSets::iterator i = Map_->find(item);
  return (i==Map_->end() ? std::string():i->second.Data_.as_string());
}
//---------------------------------------------------------------------------
K_mf(bool) TTwseSetting::GetDLCmd(const std::string& fname, const std::string& fdst,
   std::string& cmd, std::string& error) const
{
  return true;
}
//---------------------------------------------------------------------------
K_mf(bool) TTwseSetting::GetULCmd(const std::string& fname, const std::string& fdst,
   std::string& cmd, std::string& error) const
{
   std::string path  = GetSetting(fname==std::string("K02")?dsi_K02Path:dsi_K10Path);
   char buf[256];
   char* ptr = (char*)path.c_str();
   if(strstr(ptr, "%") == NULL){
      sprintf(buf, path.c_str());
   }else{
      char tmp[24];
      TKDate Date = TKDate::Now();
      memset(buf,0,sizeof(buf));
      char* pos;
      char len;
      while((pos = strstr(ptr, "%"))){
         *pos = 0;
         memset(tmp,0,sizeof(tmp));
         strcat(buf, ptr);
         pos++;
         len = *pos;
         pos++;
         switch(*pos){
            case 'y':
               switch(len){
                  case '2': sprintf(tmp,"%02d",Date.GetCYear());  break;
                  case '4': sprintf(tmp,"%04d",Date.GetEYear());break;
                  default:  sprintf(tmp,"%d",Date.GetEYear());  break;
               }
               break;
            case 'm':
               switch(len){
                  case '2': sprintf(tmp,"%02d",Date.GetMon());     break;
                  default:  sprintf(tmp,"%d",Date.GetMon());       break;
               }
               break;
            case 'd':
               switch(len){
                  case '2': sprintf(tmp,"%02d",Date.GetDay());      break;
                  default:  sprintf(tmp,"%d",Date.GetDay());        break;
               }
               break;
            deault:break;
         }
         strcat(buf,tmp);
         ptr = ++pos;
      } //while
      strcat(buf,ptr);
   }
  if(path .empty()) { error = SetNameStr[(fname==std::string("K02")?dsi_K02Path:dsi_K10Path)-1]; return false; }
  char buffer[256];
  sprintf(buffer, "cp %s%s %s", buf, fname.c_str(), fdst.c_str());
  cmd = std::string(buffer);
  return true;
}
//---------------------------------------------------------------------------


