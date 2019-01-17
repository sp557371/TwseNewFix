#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "SesMgrR.h"
#include "SesServer.h"
//---------------------------------------------------------------------------
using namespace Kway;
//---------------------------------------------------------------------------
K_ctor TSesMgrR::TSesMgrR(const char* fname)
   :TSesMgr()
{
   Log_.Open(fname, (TFileMode)(fmOpenAlways|fmAppend));
}
//---------------------------------------------------------------------------
K_dtor TSesMgrR::~TSesMgrR()
{
   Log_.Close();
}
//---------------------------------------------------------------------------
K_mf(void) TSesMgrR::OnSettingChg (ESettingChg sc, dword settingID, const TSettingRec& rec)
{
   char buf[256];
   std::string uid = Kway::Pk::TSesServer::GetLastOpUser();
   if(uid.empty() || (!Log_.IsOpened()))    // 不是由VA 所產生的修改或開檔失敗則不做記錄
      return;
   sprintf(buf,"%s (%-12s)[%s] %3d-%c; %s; %s; %s; %s\n", TKDateTime::Now().as_string(TKDate::sf_ELong,0).c_str(),
                                                      uid.c_str(),
                                                      sc==sc_Del?"Del":"A/M",
                                                      settingID,
                                                      rec.Enabled_?'Y':'N',
                                                      rec.SesName_.c_str(),
                                                      rec.SesSetting_.c_str(),
                                                      rec.MesName_.c_str(),
                                                      rec.MesSetting_.c_str());
   Log_.Write(buf);
}
//---------------------------------------------------------------------------
