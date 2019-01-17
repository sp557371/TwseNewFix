/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseFilesHandler.h"
#include "ExtDef.h"
#include "TwStkIvacNo.h"
#include <set>
#include <time.h>
#include "TwseType.h"
#include "Bool3.h"
#include "FTExtDef.h"
//---------------------------------------------------------------------------
using namespace Kway;
//using namespace Kway::Tw::Bx::FT;
using namespace Kway::Tw::Stk;
static std::string  gConfigPath, gTmpPath, gAppiaPath, gPath, gXTesterPath, gTradeSegPath;
//---------------------------------------------------------------------------
bool  GetAppiaFixVerString(std::string& src, std::string& ver)
{
   if(src.empty())
      return false;
   
   if(src == std::string("1")) { //4.2
      ver = GFIX42;
   } else if(src == std::string("2")) { //4.4
      ver = GFIX44;
   } else {
      return false;
   }
   return true;
}
//---------------------------------------------------------------------------
std::string  GetTwseFixVerCode(std::string& src)
{
   if(src.empty())
      return std::string(" ");
   if(src == GFIX42)
      return std::string("1");
   if(src == GFIX44)
      return std::string("2");
   return std::string(" ");
}
//---------------------------------------------------------------------------
std::string  GetFixDateTimeMs(bool utc, bool date, bool time, bool ms, bool log)
{
   std::string  timeStr;
   Kway::TKDateTimeMS  dt = Kway::TKDateTimeMS().Now();
   
   if(utc)
      dt.IncHour(-8); //東八區處理方式,特別注意個時區不同需調整
   
   if(date)
      timeStr.append(dt.GetDate().as_string(Kway::TKDate::sf_ELong, 0)); 
   
   if(time) {
      if(date)
         timeStr.append("-");
      timeStr.append(dt.GetTime().as_string());
   }
   
   if(ms) {
      if(time)
         timeStr.append(".");
      timeStr.append(dt.GetMS().as_string());   
   }
   
   if(log)
      timeStr.append(" ");
   
   return std::string(timeStr);
}
//---------------------------------------------------------------------------
const char*  GmtDateTimeToStr(char* buf, size_t maxlen, struct tm* gmt, bool date, bool time)
{
   if(time == false)
      strftime(buf, maxlen, "%Y%m%d", gmt);     // yyyymmdd (GMT)
   else if(date == false)
      strftime(buf, maxlen, "%H:%M:%S", gmt);   // hh:mm:ss (GMT)
   else
      strftime(buf, maxlen, "%Y%m%d-%H:%M:%S", gmt);   // yyyymmdd-hh:mm:ss (GMT)
   
   return buf;
}
//---------------------------------------------------------------------------
const char*  GetUTCDateTime(bool datestr, bool timestr)
{
   static char  buf[32] = {0};
   time_t  timer;
   struct tm*  gmt;
   //gets time of day
   timer = time(NULL);
   //converts date/time to a GMT structure
   gmt = gmtime(&timer);

   return GmtDateTimeToStr(buf, sizeof(buf), gmt, datestr, timestr);
}
//---------------------------------------------------------------------------
K_mf(std::string)  GetTmpPath()
{
   // path = $home/Twse/Backup/update/
   //std::string  confPath = GetConfPath();
   gConfigPath = GetConfPath();
   gTmpPath = gConfigPath + GMainIni; 
   //TKIniFile*   iniFile  = new TKIniFile(std::string(confPath+GMainIni).c_str());
   TKIniFile*   iniFile  = new TKIniFile(gTmpPath.c_str());
   gPath = iniFile->ReadString(GMain, GTmpPath, "");
   //std::string  path = iniFile->ReadString(GMain, GTmpPath, "");
   delete iniFile; //[Bug]20110523
   if(gPath[gPath.length()-1]!='/' && gPath[gPath.length()-1]!='\\')
      gPath += "/";
   //if(path[path.length()-1]!='/' && path[path.length()-1]!='\\')
   //   path += "/";
   
   //return std::string(gPath);  
   return gPath;  
}
//---------------------------------------------------------------------------
K_mf(std::string)  GetConfigPath()
{
   // path = $home/Twse/Settings/
   /*
   std::string  confPath = GetConfPath();
   
   if(confPath[confPath.length()-1]!='/' && confPath[confPath.length()-1]!='\\')
      confPath += "/";
   return std::string(confPath);
   */
   gConfigPath = GetConfPath();
   if(gConfigPath[gConfigPath.length()-1]!='/' && gConfigPath[gConfigPath.length()-1]!='\\')
      gConfigPath += "/";
   return gConfigPath;    
}
//---------------------------------------------------------------------------
K_mf(std::string)  GetAppiaPath()
{
   // path = $home/Backup/update/
   //std::string  confPath = GetConfigPath();
   gConfigPath = GetConfPath();
   gTmpPath = gConfigPath + GMainIni; 
   //TKIniFile*   iniFile  = new TKIniFile(std::string(confPath+GMainIni).c_str());
   TKIniFile*   iniFile  = new TKIniFile(gTmpPath.c_str());
   //std::string  path     = iniFile->ReadString(GMain, GAppiaPath, confPath);
   gAppiaPath     = iniFile->ReadString(GMain, GAppiaPath, gConfigPath);
   delete iniFile;
   /*
   if(path[path.length()-1]!='/' && path[path.length()-1]!='\\')
      path += "/";
   return std::string(path);
   */
   if(gAppiaPath[gAppiaPath.length()-1]!='/' && gAppiaPath[gAppiaPath.length()-1]!='\\')
      gAppiaPath += "/";
   return gAppiaPath;
}
//---------------------------------------------------------------------------
//T5380X1-->T5U3U8UAUXU1U
K_mf(std::string)  ConvertTand2Appia(std::string& shortStr)
{
   Kway::TTwseSesIDTand  tandem(shortStr);
   return  std::string(tandem.as_string_Apa());
}
//---------------------------------------------------------------------------
//T5U3U8UAUXU1U-->T5380X1
K_mf(std::string)  ConvertAppia2Tand(std::string& longStr)
{
   Kway::TTwseSesIDApa  appia(longStr);
   return std::string(appia.as_string_Tand());
}
//---------------------------------------------------------------------------
K_mf(bool)  GetAppiaLocked()
{
   //std::string  path = GetAppiaPath();
   gAppiaPath = GetAppiaPath() + GAppiaIni;
   //TKIniFile*   iniFile  = new TKIniFile(std::string(path+GAppiaIni).c_str());
   TKIniFile*   iniFile  = new TKIniFile(gAppiaPath.c_str());
   //bool  isLocked = iniFile->ReadBool(GMain, GIsLocked, false);
   //bool  isLocked = iniFile->ReadBool(GAppiaMain, GIsLocked, false);
   bool  isLocked = iniFile->ReadBool(GKway, GIsLocked, false);
   delete iniFile;
   return bool(isLocked);
}
//---------------------------------------------------------------------------
K_mf(bool)  GetK11Done()
{
   TKDate  date;
   TKDate  today = TKDate::Now();
   std::string  tmp;
   Kway::Tw::Stk::TMarket  mkt = GetProcMkt();
   TMarket  twse, otc(m_Otc);
   //std::string  tmpPath = GetTmpPath();
   //TKIniFile*   iniFile = new TKIniFile(std::string(tmpPath+GMainIni).c_str());
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gTmpPath.c_str());
   
   if(mkt == twse) {
      tmp = iniFile->ReadString(GXFT, GTK11Done, "");
   } else if(mkt == otc) {
      tmp = iniFile->ReadString(GXFT, GOK11Done, "");
   }
   
   delete iniFile;
   
   if(tmp.empty()) {
      return false;
   } else {
      date.assign(tmp);
      return(date == today);
   }
}
//---------------------------------------------------------------------------
K_mf(bool)  GetK13Done()
{
   TKDate  date;
   TKDate  today = TKDate::Now();
   std::string  tmp;
   Kway::Tw::Stk::TMarket  mkt = GetProcMkt();
   TMarket  twse, otc(m_Otc);
   //std::string  tmpPath = GetTmpPath();      
   //TKIniFile*   iniFile = new TKIniFile(std::string(tmpPath+GMainIni).c_str());
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gTmpPath.c_str());
   
   if(mkt == twse) {
      tmp = iniFile->ReadString(GXFT, GTK13Done, "");
   } else if(mkt == otc) {
      tmp = iniFile->ReadString(GXFT, GOK13Done, "");
   }
   
   delete iniFile;
   
   if(tmp.empty()) {
      return false;
   } else {
      date.assign(tmp);
      return(date == today);
   }
}
//---------------------------------------------------------------------------
K_mf(std::string)  FillIPString(std::string& ip)
{
    std::string  tmpIP, newIP("000.000.000.000");
   if(ip.empty())
      return std::string(newIP);

   newIP.clear();
   tmpIP = ip;
   std::string  tmp;
   char  buf[4] = {0};
   std::string::size_type  pos;
   int  i = 1;
   for(pos=tmpIP.find("."); pos!=std::string::npos; pos=tmpIP.find(".")) {
      if(i > 4) {
         tmpIP.clear();
         break;
      }
      tmp = tmpIP.substr(0, pos);
      sprintf(buf, "%03d", atoi(tmp.c_str()));
      newIP += buf;
      if(i < 4)
         newIP += ".";
      ++i;
      if(pos+1 != std::string::npos)
         tmpIP = tmpIP.substr(pos+1, std::string::npos);
   }
   if(tmpIP.empty() == false){
//20100924 eric 補上最後一碼為三位
      sprintf(buf, "%03d", atoi(tmpIP.c_str()));
      newIP += buf;
//      newIP += tmpIP;
   }
   return std::string(newIP);
}
//---------------------------------------------------------------------------
/*
K_mf(std::string)  ToString(EWKStatus wk)
{
   static const char  NotYet[] = "尚未收到";
   static const char  NoNeed[] = "不檢查";
   static const char  Arrived[]= "已收到";
   static const char  Alerted[]= "已發出通知";
   
   switch(wk) {
   case wk_NotYet:    return  std::string(NotYet);
   case wk_NoNeed:    return  std::string(NoNeed);
   case wk_Arrived:   return  std::string(Arrived);
   case wk_Alerted:   return  std::string(Alerted);
   }
   return std::string("---");
}
//---------------------------------------------------------------------------
static Kway::TFieldDesp  Desp[] =
{
   {"K06作業代碼", 10, Kway::ta_xLeft_yCenter,  Kway::ta_xLeft_yCenter, Kway::tfs_Null, },
   {"檢查時間",    10, Kway::ta_xLeft_yCenter,  Kway::ta_xLeft_yCenter, Kway::tfs_CanEdit, },
   {"檢查狀態",    10, Kway::ta_xLeft_yCenter,  Kway::ta_xLeft_yCenter, Kway::tfs_Null, },
   {"備註",       50, Kway::ta_xLeft_yCenter,  Kway::ta_xLeft_yCenter, Kway::tfs_Null, },
};
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&)  TWorkCodeVAC::TWorkCodeFieldMaker::GetFieldDesp(field_n_type n) const
{
   return Desp[n];
}
//---------------------------------------------------------------------------
K_mf(TWorkCodeVAC::TWorkCodeFieldMaker::field_n_type)  TWorkCodeVAC::TWorkCodeFieldMaker::GetFieldCount() const
{
   return numofele(Desp);
}
//---------------------------------------------------------------------------
K_mf(std::string)  TWorkCodeVAC::TWorkCodeFieldMaker::Get(const iterator& i,field_n_type fieldNo) const
{
   switch(fieldNo) {
   case 0:   return i->first.as_string();
   case 1:   return i->second.CheckTime_.as_string();
   case 2:   return ToString(i->second.IsChecked_);
   case 3:   return i->second.Memo_.as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(void)  TWorkCodeVAC::TWorkCodeFieldMaker::Put(TEvContainer& c,iterator& iter,field_n_type fieldNo,const std::string& str)
{
   if(fieldNo == 0)
      return;
   
   TKTime  now = TKTime::Now();
   TEvContainer::updater  u = c.begin_update(iter);
   switch(fieldNo) {
   case 1:
      u->CheckTime_.assign(str);   
      if(u->CheckTime_.IsInvalid()) {
         u->IsChecked_ = wk_NoNeed; 
      } else {
         if(u->IsChecked_ == wk_NoNeed) {
            if(u->IsReceived_)
               u->IsChecked_ = wk_Arrived;
            else
               u->IsChecked_ = wk_NotYet;
         }
      }
      break;
   default:   break;
   }
}
*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
K_ctor  TwseFilesHandler::TwseFilesHandler() 
   : Timer_(this)
   //, WKTimer_(this)
   , K10Timer_(this)
   , K11Timer_(this)
   , K13Timer_(this)
{
   Mgr_ = NULL;
   //std::string  tmpPath = GetTmpPath();
   //TKIniFile*  iniFile = new TKIniFile(std::string(tmpPath+GMainIni).c_str());
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*  iniFile = new TKIniFile(gTmpPath.c_str());
   IsTK05Ready_ = iniFile->ReadString(GXFT, GTK05Ready, "");
   IsOK05Ready_ = iniFile->ReadString(GXFT, GOK05Ready, "");
   //IsTK05Ready_ = iniFile->ReadString(GMain, GTK05Ready, "");
   //IsOK05Ready_ = iniFile->ReadString(GMain, GOK05Ready, "");
   //IsK03Ready_ = iniFile->ReadString(GMain, GK03Ready, "");
   IsTK03Ready_ = iniFile->ReadString(GXFT, GTK03Ready, "");
   IsOK03Ready_ = iniFile->ReadString(GXFT, GOK03Ready, "");
   IsTK11Ready_ = iniFile->ReadString(GXFT, GTK11Ready, "");
   IsOK11Ready_ = iniFile->ReadString(GXFT, GOK11Ready, "");
   IsTK11Done_  = iniFile->ReadString(GXFT, GTK11Done, "");
   IsOK11Done_  = iniFile->ReadString(GXFT, GOK11Done, "");

   IsTK13Ready_ = iniFile->ReadString(GXFT, GTK13Ready, "");
   IsOK13Ready_ = iniFile->ReadString(GXFT, GOK13Ready, "");
   IsTK13Done_  = iniFile->ReadString(GXFT, GTK13Done, "");
   IsOK13Done_  = iniFile->ReadString(GXFT, GOK13Done, "");
   
   /* //以不需要主動分配mw port   
   PortsMap_  = new TPortsMap();
   SesIDsMap_ = new TSesIDsMap();
   
   TPortsMap::iterator   iter_port;
   TPortsMap::updater    upd_port;
   TSesIDsMap::iterator  iter_ses;
   TSesIDsMap::updater   upd_ses;
   
   TStrings  Brks, Sess;
   std::string  BrkMkt;
   int  BrkCnt = iniFile->ReadStrings(GMain, GBroker, Brks);
   for(int L = 0; L < BrkCnt; ++L) {
      BrkMkt = "T" + Brks[L];
      int  SesCnt = iniFile->ReadStrings(BrkMkt, GFixSes, Sess);
      for(int i = 0; i < SesCnt; i++) {
         int mwPort = iniFile->ReadInt(Sess[i], GAppiaMWPort, 0);
         if(mwPort == 0)
            continue;
         iter_ses = SesIDsMap_->fetch(Sess[i]);
         upd_ses  = SesIDsMap_->begin_update(iter_ses);
         *upd_ses  = mwPort;
         iter_port= PortsMap_->fetch(mwPort);
         upd_port = PortsMap_->begin_update(iter_port);
         *upd_port = Sess[i];
      }
      BrkMkt = "O"+Brks[L];
      SesCnt = iniFile->ReadStrings(BrkMkt, GFixSes, Sess);
      for(int i = 0; i < SesCnt; ++i) {    
         int mwPort = iniFile->ReadInt(Sess[i], GAppiaMWPort, 0);
         if(mwPort == 0)
            continue;
         iter_ses = SesIDsMap_->fetch(Sess[i]);
         upd_ses  = SesIDsMap_->begin_update(iter_ses);
         *upd_ses  = mwPort;
         iter_port= PortsMap_->fetch(mwPort);
         upd_port = PortsMap_->begin_update(iter_port);
         *upd_port = Sess[i];
      }
   }
   */
   std::string  fn;
   std::string  procLog  = GetProcLogPath(); // $home/Twse/Logs/yymmdd/XFT/TwseFile.log
   fn = procLog + "TwseFile.log";
   Logger_ = new TwseFileLog(fn.c_str());
   WriteMsg(GXFT, "Startup!");
  
   std::string  confPath = GetConfigPath();
   std::string  logPath = GetLogPath();
   fn = logPath + "K01Map.bin"; // $home/Twse/Logs/yymmdd/K01Map.bin
   K01Map_      = new TK01Map();
   K01Recorder_ = new TK01Recorder(*K01Map_, fn.c_str());
   
   fn = logPath + "K11Map.bin"; // $home/Twse/Logs/yymmdd/K11Map.bin
   K11Map_      = new TK11Map();
   K11Recorder_ = new TK11Recorder(*K11Map_, fn.c_str());
   //fprintf(stderr, "K11Map_=%d\n", K11Map_->size());
   
   fn = logPath + "K13Map.bin"; // $home/Twse/Logs/yymmdd/K13Map.bin
   K13Map_      = new TK13Map();
   K13Recorder_ = new TK13Recorder(*K13Map_, fn.c_str());
   //fprintf(stderr, "K13Map_=%d\n", K13Map_->size());

   fn = confPath + "K03.bin"; // $home/Twse/Settings/K03.bin
   K03Map_ = new TK03Map();
   K03Recorder_ = new TK03Recorder(*K03Map_, fn.c_str());
   CheckK03Map(); //移除過期的K03Map
   
   //fprintf(stderr, "K03Map size=%d\n", K03Map_->size());

   K05Map_ = new TK05Map();
   
   TKDate  dateT, dateO;
   TKDate  today = TKDate::Now();
   if(!IsTK05Ready_.empty())
      dateT.assign(IsTK05Ready_);
   if(!IsOK05Ready_.empty())
      dateO.assign(IsOK05Ready_);
   if(dateT==today || dateO==today) { //避免程式crash後,K05Map_消失
      RebuildK05Map(*iniFile);
   }
   //確認K11是否已經處理過
   Kway::Tw::Stk::TMarket  procMkt = GetProcMkt();
   TMarket  twse, otc(m_Otc);
   if(K11Map_->size() > 0) {
      //Kway::Tw::Stk::TMarket  procMkt = GetProcMkt();
      //TMarket  twse, otc(m_Otc);
      if(procMkt == twse) {
         if(!IsTK11Done_.empty()) {
            dateT.assign(IsTK11Done_);
            if(dateT != today)
               StartK11Timer(30);
         }    
      } else if(procMkt == otc) {
         if(!IsOK11Done_.empty()) {
            dateO.assign(IsOK11Done_);
            if(dateO != today)
               StartK11Timer(30);
         }
      }
   }
   //確認K13是否已經處理過
   if(K13Map_->size() > 0) {
      //Kway::Tw::Stk::TMarket  procMkt = GetProcMkt();
      //TMarket  twse, otc(m_Otc);
      if(procMkt == twse) {
         if(!IsTK13Done_.empty()) {
            dateT.assign(IsTK13Done_);
            if(dateT != today)
               StartK13Timer(30);
         }    
      } else if(procMkt == otc) {
         if(!IsOK13Done_.empty()) {
            dateO.assign(IsOK13Done_);
            if(dateO != today)
               StartK13Timer(30);
         }
      }
   }
   /*
   fn = confPath + "K06WorkCode.bin"; // $home/Twse/Settings/K06WorkCode.bin
   WorkCodeMap_      = new TWorkCodeMap;
   WorkCodeRecorder_ = new TWorkCodeRecorder(*WorkCodeMap_, fn.c_str());
   TFile*  file = new TFile(fn.c_str(), TFileMode(fmRead));
   if(file->GetLastModifyTime(false).GetDate() != TKDate::Now())
      CreateWKMap();
   WorkCodeVAC_      = new TWorkCodeVAC(*WorkCodeMap_);   
   //StartWKTimer(30);
   
   delete file;
   */
   delete iniFile;
   //Add(std::string("WK-CODE檢查時間設定"), *WorkCodeVAC_);
}
//---------------------------------------------------------------------------
K_dtor  TwseFilesHandler::~TwseFilesHandler()
{
   WriteMsg(GXFT, "Stop!");
   //StopWKTimer();
   StopTimer();
   
   /*
   //Del(*WorkCodeVAC_);
   if(WorkCodeVAC_) {
      delete WorkCodeVAC_;
      WorkCodeVAC_ = NULL;
   }
   if(WorkCodeRecorder_) {
      delete WorkCodeRecorder_;
      WorkCodeRecorder_ = NULL;
   }
   if(WorkCodeMap_) {
      delete WorkCodeMap_;
      WorkCodeMap_ = NULL;
   }
   */
   /*
   if(SesIDsMap_) {
      delete SesIDsMap_;
      SesIDsMap_ = NULL;
   }
   if(PortsMap_) {
      delete PortsMap_;
      PortsMap_ = NULL;
   }
   */
   if(K01Recorder_) {
      delete K01Recorder_;
      K01Recorder_ = NULL;
   }
   if(K01Map_) {
      delete K01Map_;
      K01Map_ = NULL;
   }
   if(K03Recorder_) {
      delete K03Recorder_;
      K03Recorder_ = NULL;
   }
   if(K03Map_) {
      delete K03Map_;
      K03Map_ = NULL;
   }
   if(K05Map_) {
      delete K05Map_;
      K05Map_ = NULL;
   }
   if(K11Recorder_) {
      delete K11Recorder_;
      K11Recorder_ = NULL;
   }
   if(K11Map_) {
      delete K11Map_;
      K11Map_ = NULL;
   }
   if(K13Recorder_) {
      delete K13Recorder_;
      K13Recorder_ = NULL;
   }
   if(K13Map_) {
      delete K13Map_;
      K13Map_ = NULL;
   }
   if(Logger_) {
      delete Logger_;
      Logger_ = NULL;
   }
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::StartHandleAFile(TMarket& mkt, std::string& fname, std::string& path, bool isRebuild)
{   
   char  buf[256] = {0};
   sprintf(buf, "收到通知: %s, %s, %s", mkt==m_Tse?"上市":"上櫃", fname.c_str(), path.c_str());
   
   if(Mgr_ = GetExcMgr())
      Mgr_->Alert("XF1000", fname, buf);
   WriteMsg(fname.c_str(), buf);
   
   //std::string  fullFN = path + "/" + fname;
   if(path[path.length()-1]=='/' || path[path.length()-1]=='\\' || path[path.length()-1]=='.') {
      if(Mgr_ = GetExcMgr())
         Mgr_->Alert("XF1001", fname, "");
      sprintf(buf, "File Error=%s", fname.c_str());
      WriteMsg(fname.c_str(), buf);
      return;
   }
	
   TFile*  file = new TFile(path.c_str(), TFileMode(fmRead));
   //fprintf(stderr, "path=%s\n", path.c_str());
   TStringV  v;
   v.clear();
   
   TFileType  type;
   type.assign(fname);
   
   //fprintf(stderr, "file-size=%d\n", file->GetFileSize());
   sprintf(buf, "Received File=%d", file->GetFileSize());
   WriteMsg(fname.c_str(), buf);
   
   if(file->IsOpened() == false) { //檔案開啟失敗
      if(Mgr_ = GetExcMgr())
         Mgr_->Alert("XF1002", fname, "");
      WriteMsg(fname.c_str(), "Failed to open file.");
      delete file;
      return;
   }
   if(file->GetFileSize() <= 0) { //檔案大小錯誤
      if(Mgr_ = GetExcMgr())
         Mgr_->Alert("XF1003", fname, "");
      WriteMsg(fname.c_str(), "File size is less than 0.");
      delete file;
      return;
   }
   if(Read2Container(file, type, v) == false) { //檔案無資料
      if(Mgr_ = GetExcMgr())
         Mgr_->Alert("XF1004", fname, "");
      WriteMsg(fname.c_str(), "There is no data in pkt.");
      if(type!=ft_K03 && type!=ft_K05 && type!=ft_K11 && type!=ft_K12 && type!=ft_K13) { //K03, K05, K11, K12, K13可能是空的, 20100924 eric 還有 K12
   	     delete file;
         return;
      }
   }
   /*
   if(file->IsOpened()==false || file->GetFileSize()<=0|| Read2Container(file, type, v)==false) {
      delete file;
      return;
   }
   */
   
   //fprintf(stderr, "file-size=%d\n", file->GetFileSize());
   delete file;
   
   switch(static_cast<EFileType>(type)) {
   case ft_K01:   ProcessK01(mkt, type, v);   break;
   case ft_K02:   break;
   case ft_K03:   ProcessK03(mkt, type, v, path);   break;
   case ft_K04:   ProcessK04(mkt, type, v);   break;
   case ft_K05:   ProcessK05(mkt, type, v, path, isRebuild);   break;
   case ft_K06:   ProcessK06(mkt, type, v);   break;
   case ft_K07:   ProcessK07(mkt, type, v);   break;
   case ft_K08:   ProcessK08(mkt, type, v);   break;
   case ft_K09:   ProcessK09(mkt, type, v);   break;
   case ft_K11:   ProcessK11(mkt, type, v);   break;
   case ft_K12:   ProcessK12(mkt, type, v);   break;
   case ft_K13:   ProcessK13(mkt, type, v);   break;
   default: //非預定處理之檔案
      if(Mgr_ = GetExcMgr())
         Mgr_->Alert("XF1005", fname, "");
      WriteMsg(fname.c_str(), "非預定處理之檔案");
      break;
   }
}
//---------------------------------------------------------------------------
K_mf(bool)  TwseFilesHandler::Read2Container(TFile* file, TFileType& type, TStringV& v)
{
   //fprintf(stderr, "FileLen=%d\n", file->GetFileSize());
   int  i = 1;
   int  size1, size2;
   size1 = type.GetDataSize(1);
   size2 = type.GetDataSize();
   
   if(size1==0 || size2==0) //避免未定義格式,造成無窮迴圈
      return false;
   char buffer[std::max(size1,size2)+1];
   
   file->Seek(0, fsBegin);
   while(file->IsEOF() == false) {
      if(i == 1) {
         if(file->Read(buffer, size1) == size1) {
            v.push_back(buffer);
         }
         ++i;
      } else { 
         if(file->Read(buffer, size2) == size2) {
            v.push_back(buffer);
         }
      }
   }   
   return (type==ft_K09? (v.size()>0) : (v.size()>1));
   //return v.size() > 1;
   //return true;
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessK01(TMarket& mkt, TFileType& type, TStringV& v)
{
   K01Map_->clear(); //[Add]20110523: 重覆傳輸K01,避免先前資料殘留
   int  i = 0;
   SK01Data2  data;     
   std::string  applyDate;
   
   for(TStringVI  iter=v.begin(); iter!=v.end(); ++iter) {
      if(i == 0) { //Data1不處理
         ++i;
         applyDate = (*iter).substr(0,8);
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      if(ChkNInsertData(data, applyDate) == false) {
         if(Mgr_ = GetExcMgr()) //檔案重要欄位資料有誤
            Mgr_->Alert("XF1006", "K01", "");
	     }
   } 
   ConfigTester();
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessK04(TMarket& mkt, TFileType& type, TStringV& v)
{
   int  i = 0;
   SK04Data2  data;  

   std::vector<SK04Data2>  vK04Data2;
   vK04Data2.clear();

   std::string  applyDate, nextDate;
   TKDate  today = TKDate::Now();
   for(TStringVI  iter=v.begin(); iter!=v.end(); ++iter) {
      if(i == 0) { //Data1不處理
         ++i;
         applyDate = (*iter).substr(0,8);
         TKDate  date;
         date.assign(applyDate);
         if(date != today) {
            if(Mgr_ = GetExcMgr())
               Mgr_->Alert("XF1007", "K04", "K04檔案日期錯誤");
            WriteMsg("K04", "K04檔案日期錯誤");
            return;
         }
         nextDate = (*iter).substr(8, 8);
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      vK04Data2.push_back(data);
   } 
   //讀取Main.ini,取得路徑(TradeSegment.ini)
   std::string  confPath = GetConfigPath();
   gTmpPath = confPath + GMainIni;
   gTradeSegPath = confPath + GTradeSegIni;
   TKIniFile*  iniFile  = new TKIniFile(gTmpPath.c_str());
   TKIniFile*  tradeIni = new TKIniFile(gTradeSegPath.c_str());
   //TKIniFile*  iniFile  = new TKIniFile(std::string(confPath + GMainIni).c_str());
   //TKIniFile*  tradeIni = new TKIniFile(std::string(confPath + GTradeSegIni).c_str());
   
   char  buf[256] = {0};
   sprintf(buf, "Start to Configure K04, T=%s, Trd=%s", iniFile->GetFilePath().c_str(), tradeIni->GetFilePath().c_str());
   WriteMsg("K04", buf);
   
   //寫入nextDate to Main.ini
   //iniFile->WriteString(GMain, GNextDate, nextDate);
   iniFile->WriteString(GXFT, GNextDate, nextDate);
   delete  iniFile;
   //寫入nextDate to $home/Twse/Settings/NextDate, 供控制啟動系統script使用
   std::string  name = confPath + "NextDate";
   TFile*  file = new TFile(name.c_str(), TFileMode(fmReadWrite|fmCreatePath));
   if(file->IsOpened() == false)
      file->Open(name.c_str(), TFileMode(fmReadWrite|fmCreatePath));
   file->Seek(0, fsBegin);
   file->Write(nextDate.c_str());
   delete file;
   //寫入nextDate to twse.ini(appia)
   std::string  appia = GetAppiaPath() + GAppiaIni;
   TKIniFile*  appiaIni = new TKIniFile(appia.c_str());
   appiaIni->WriteString("KWAY", "Trading_date", nextDate);
   delete  appiaIni;
   
   std::vector<SK04Data2>::iterator  iter = vK04Data2.begin();
   for(; iter!=vK04Data2.end(); ++iter) {
      SetK04IniFile(*iter, *tradeIni, mkt);
   }
   
   delete  tradeIni;

   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert("XF1100", "K04", "K04檔案處理完成");
   }
   WriteMsg("K04", "K04檔案處理完成");
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessK03(TMarket& mkt, TFileType& type, TStringV& v, std::string& fullname)
{
   CheckK03Today();  //重新傳送K03時,先刪除當天送過的K03Map資訊
   //K03收到比對是否有K05,再比對K05是否有存在,不存在則先保留,若存在則找K04的next date是否與生效日同,相同則設定
   dword  i = 0;
   SK03Data2  data;  
   std::string  applyDate;
   TKDate  today = TKDate::Now();
   char  buf[256] = {0};
   
   for(TStringVI  iter=v.begin(); iter!=v.end(); ++iter) {
      if(i == 0) { //Data1不處理
         ++i;
         applyDate = (*iter).substr(0,8);
         TKDate  date;
         date.assign(applyDate);
         if(date != today) {
            if(Mgr_ = GetExcMgr())
               Mgr_->Alert("XF1007", "K03", "K03檔案日期錯誤");
            sprintf(buf, "K03檔案日期錯誤-%s", date.as_string().c_str());
            WriteMsg("K03", buf);
            return;
         }
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      if(ChkNInsertData(data, today, i) == false) {
         if(Mgr_ = GetExcMgr()) //檔案重要欄位資料有誤
            Mgr_->Alert("XF1006", "K03", "");
      }
      ++i;
   }

   //讀取Main.ini,取得路徑()
   //std::string  tmpPath = GetTmpPath();
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gTmpPath.c_str());
   //TKIniFile*   iniFile = new TKIniFile(std::string(tmpPath+GMainIni).c_str());
   if(mkt == m_Tse)
      IsTK03Ready_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   else
      IsOK03Ready_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   
   iniFile->WriteString(GXFT, mkt==m_Tse?GTK03Ready:GOK03Ready, mkt==m_Tse?IsTK03Ready_:IsOK03Ready_);
   delete iniFile;
   
   if(CheckK03orK05(mkt, TFileType(ft_K05))) {
      if(GetAppiaLocked() == false) {
         ConfigProduction();
      } else {
         WriteMsg("K03", "Appia IsLocked=true!");
         StartTimer(30);
      }
   } else {
      if(Mgr_ = GetExcMgr()) //K05檔案不存在
         Mgr_->Alert("XF1008", "K03", "");
      WriteMsg("K03", "K05檔案不存在");
   }
   
   //iniFile->WriteString(GXFT, mkt==m_Tse?GTK05File:GOK03File, fullname);
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessK05(TMarket& mkt, TFileType& type, TStringV& v, std::string& fullname, bool isRebuild)
{
   int  i = 0;
   SK05Data2  data;
   std::string  applyDate;
   TKDate  today = TKDate::Now();
   char  buf[256] = {0};
   
   for(TStringVI  iter=v.begin(); iter!=v.end(); ++iter) {
      if(i == 0) { //Data1不處理
         ++i;
         applyDate = (*iter).substr(0,8);
         TKDate  date;
         date.assign(applyDate);
         if(date != today) {
            if(Mgr_ = GetExcMgr())
               Mgr_->Alert("XF1007", "K05", "K05檔案日期錯誤");
            sprintf(buf, "K05檔案日期錯誤-%s", date.as_string().c_str());
            WriteMsg("K05", buf);
            return;
         }
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
	     if(ChkNInsertData(data, mkt) == false) {
 	       if(Mgr_ = GetExcMgr()) //檔案重要欄位資料有誤
             Mgr_->Alert("XF1006", "K05", "");
	     }
   } 
   
   if(isRebuild)
      return;
   
   //讀取Main.ini,取得路徑()
   //std::string  tmpPath = GetTmpPath();
   //TKIniFile*   iniFile  = new TKIniFile(std::string(tmpPath+GMainIni).c_str());
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gTmpPath.c_str());
   if(mkt == m_Tse)
      IsTK05Ready_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   else
      IsOK05Ready_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   
   iniFile->WriteString(GXFT, mkt==m_Tse?GTK05Ready:GOK05Ready, mkt==m_Tse?IsTK05Ready_:IsOK05Ready_);
   iniFile->WriteString(GXFT, mkt==m_Tse?GTK05File:GOK05File, fullname);
   //iniFile->WriteString(GMain, mkt==m_Tse?GTK05Ready:GOK05Ready, mkt==m_Tse?IsTK05Ready_:IsOK05Ready_);
   //iniFile->WriteString(GMain, mkt==m_Tse?GTK05File:GOK05File, fullname);
   delete iniFile;
      
   if(CheckK03orK05(mkt, TFileType(ft_K03))) {
   //if(K03Map_->empty() == false) {
      if(GetAppiaLocked() == false)
         ConfigProduction();
      else {
         WriteMsg("K05", "Appia IsLocked=true!");
         StartTimer(30);
      }
   } else {
      if(Mgr_ = GetExcMgr()) //K03檔案不存在
         Mgr_->Alert("XF1009", "K05", "");
      WriteMsg("K05", "K03檔案不存在");
   }
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessK06(TMarket& mkt, TFileType& type, TStringV& v)
{
   int  i = 0;
   SK06Data2  data;  

   std::vector<SK06Data2>  vK06Data2;
   vK06Data2.clear();

   std::string  applyDate;
   TKDate  today = TKDate::Now();
   for(TStringVI  iter=v.begin(); iter!=v.end(); ++iter) {
      if(i == 0) { //Data1不處理
         ++i;
         applyDate = (*iter).substr(0,8);
         TKDate  date;
         date.assign(applyDate);
         if(date != today) {
            if(Mgr_ = GetExcMgr())
               Mgr_->Alert("XF1007", "K06", "K06檔案日期錯誤");
            WriteMsg("K06", "K06檔案日期錯誤");
            return;
         }
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      vK06Data2.push_back(data);
   } 
   
   //讀取Main.ini,取得路徑
   //std::string  confPath = GetConfigPath();
   //TKIniFile*  iniFile  = new TKIniFile(std::string(confPath+GMainIni).c_str());
   gConfigPath = GetConfigPath() + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gConfigPath.c_str());
   
   std::vector<SK06Data2>::iterator  iter = vK06Data2.begin();
   int wk;
   std::string  fn = GetLogPath() + "WorkCode"; // $home/Twse/Logs/yyyymmdd/WorkCode
   //std::string  fn_mkt = GetAppiaPath() + "WorkCode"+ (mkt==m_Tse?".T":".O");
   std::string  fn_mkt = GetTmpPath() + "WorkCode"+ (mkt==m_Tse?".T":".O");
   std::string  name;
   for(; iter!=vK06Data2.end(); ++iter) {
      char buf[2] = {0};
      wk = iter->WorkCode_.as_int();
      sprintf(buf, "%d", wk);
      //20100921交易所電規要求拿掉WorkCode 寫入Main.ini
      //iniFile->WriteInt(GMain, GWorkCode, wk);
      //寫入一個byte給shell script使用,分別寫兩個地方 1.$home/Twse/Logs/yyyymmdd/WorkCode  2.$home/Backup/update/WorkCode.T(O)
      for(int i=0; i<2; ++i) {
         name = i==0 ? fn : fn_mkt;
         TFile*  file = new TFile(name.c_str(), TFileMode(fmReadWrite|fmCreatePath|fmTrunc)); //[Bug]20110208: Add fmTrunc to ensure the data will not have any residual number
         if(file->IsOpened() == false)
            file->Open(name.c_str(), TFileMode(fmReadWrite|fmCreatePath|fmTrunc)); //[Bug]20110208: Add fmTrunc to ensure the data will not have any residual number
         file->Seek(0, fsBegin);
         file->Write(buf);
         delete file;
      }
      //SetWKReceived(wk);
   }
   delete iniFile;

   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert("XF1100", "K06", "K06檔案處理完成");
   }
   WriteMsg("K06", "K06檔案處理完成");
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessK07(TMarket& mkt, TFileType& type, TStringV& v)
{
   struct  SPkt {
      std::string  MsgType_;
      std::string  Tag49_;
      std::string  Tag56_;
      std::string  BrokerID_; // Tag50
      std::string  OrderID_;
      std::string  Account_;
      std::string  Tag11_;
      std::string  Tag21_;    // 1
      std::string  Tag57_;    //盤別, 0=Regular, 2=Odd-lot, 7=Fixed-price
      std::string  Tag55_;
      std::string  Tag54_;
      std::string  Tag60_;
      std::string  Tag38_;    //Regular, Fixed-price為張數, Odd-lot為股數
      std::string  Tag40_;    //2=Regular or Odd-lot, Z=Fixed-price
      std::string  Tag59_;
      std::string  Tag44_;
      std::string  Tag10000_;
      std::string  Tag10001_; //0=Odd-lot
      std::string  Tag10002_; //0,3=Regular/Fixed-price ; 2,3=Odd-lot

      K_ctor SPkt() : MsgType_("D"), Tag21_("1"), Tag57_("0"), Tag38_("1"), Tag40_("2"), Tag59_("0"), Tag10000_("1"), Tag10001_("0"), Tag10002_("0") {}
      K_mf(std::string)  ToString() { 
         char buf = 0x01;
         char newline = 0x0A;
         return std::string("35="+MsgType_+buf+"49="+Tag49_+buf+"56="+Tag56_+buf+"50="+BrokerID_+buf+"57="+Tag57_+buf+"11="+Tag11_+buf+
                            "1="+Account_+buf+"37="+OrderID_+buf+"21="+Tag21_+buf+"55="+Tag55_+buf+"54="+Tag54_+buf+"60="+Tag60_+buf+
                            "38="+Tag38_+buf+"40="+Tag40_+buf+"59="+Tag59_+buf+"44="+Tag44_+buf+"10000="+Tag10000_+buf+"10001="+Tag10001_+
                            buf+"10002="+Tag10002_+buf+newline); 
      }
   };
   
   static  int tag11 = 1;
   int  i = 0;
   SK07Data2  data;  

   std::vector<SK07Data2>  vK07Data2;
   vK07Data2.clear();
   
   for(TStringVI  iter=v.begin(); iter!=v.end(); ++iter) {
      if(i == 0) { //Data1不處理
         ++i;
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      vK07Data2.push_back(data);
   }
   
   //讀取Main.ini,取得路徑
   //SPkt  pkt;
   //std::string  k07;
   //char newline[] = {0x0A};
   //char  buf[32] = {0};
   //std::string  fn = GetLogPath() + "K07"; //$Home/Twse/Logs/yyyymmdd/K07
   //TFile*  file = new TFile(fn.c_str(), TFileMode(fmReadWrite|fmCreatePath|fmAppendBit));
   //file->Seek(0, fsBegin);

   //std::string  confPath = GetConfigPath();
   //TKIniFile*   iniFile  = new TKIniFile(std::string(confPath+GMainIni).c_str());
   gConfigPath = GetConfigPath() + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gConfigPath.c_str());
   //std::string  brk      = iniFile->ReadString(GMain, "TestBroker", "5260");
   TStrings  brks; //[Add]20110418: 因應4.2與4.4,故broker改為Multiple
   iniFile->ReadStrings(GMain, "TestBroker", brks);
   
   for(TStrings::iterator iterBrk=brks.begin(); iterBrk!=brks.end(); ++iterBrk) {
      //[Add]20110418:改為取TestBroker之BrokerID block的FixSes
      std::string  brk = *iterBrk;
      TStrings  sesIDs;
      std::string  mbrk= (mkt==m_Tse ? "T" : "O") + brk;
      iniFile->ReadStrings(mbrk, GFixSes, sesIDs);
      for(TStrings::iterator iterSes=sesIDs.begin(); iterSes!=sesIDs.end(); ++iterSes) {
         SPkt  pkt;
         std::string  k07;
         char  buf[32] = {0};
         std::string  fn = GetLogPath() + "K07" + "." + (*iterSes); //$Home/Twse/Logs/yyyymmdd/K07.T1110X1
         TFile*  file = new TFile(fn.c_str(), TFileMode(fmReadWrite|fmCreatePath|fmAppendBit));
         file->Seek(0, fsBegin);
         //pkt.Tag49_ = (mkt==m_Tse ? "T" : "O") + brk + "X1";
         pkt.Tag49_ = *iterSes;
         pkt.Tag56_ = mkt==m_Tse ? "XTAI" : "ROCO";
         pkt.BrokerID_ = brk;
         pkt.Account_ = GetTradeAccount(brk, brk);  
         for(std::vector<SK07Data2>::iterator iter=vK07Data2.begin(); iter!=vK07Data2.end(); ++iter) {
            //產出檔案給FIX TMeter使用
            pkt.Tag55_ = (*iter).StkNo_.as_string();
            //pkt.Tag60_ = GetFixDateTimeMs(true, true, true, true, false);
            //pkt.Tag60_ = GetUTCDateTime(true, true);
            for(int i=1; i<3; ++i) {
               for(int j=0; j<3; ++j) { //20100810-產生Regular/Odd-Lot/Fixed-price
                  if(j == 2) {
                     if((*iter).TradeOn_.empty() == false) {
                        Kway::TBool3  odd((*iter).TradeOn_.as_string());
                        if(odd.IsTrue() == false)
                           continue;
                     } else
                        continue;
                  }
                  pkt.Tag60_ = GetFixDateTimeMs(true, true, true, true, false);
                  sprintf(buf, "%012d", tag11);
                  pkt.Tag11_ = buf;
                  sprintf(buf, "%c%04d", GetSeq2Alpha(tag11/10000 +1), (tag11%10000));
                  pkt.OrderID_ = buf;
                  pkt.Tag54_ = i==1 ? "1" : "2";
                  pkt.Tag44_ = i==1 ? (*iter).as_price((*iter).PriUp_) : (*iter).as_price((*iter).PriDown_);
                  ++tag11;
                  if(file->IsOpened() == false)
                     file->Open(fn.c_str(), TFileMode(fmReadWrite|fmCreatePath|fmAppendBit));
                  //20100810-Twse要求需要增加盤後,零股,處理特定Tag內容
                  //20100926 eric -電規要求將委託數量改為1
                  switch(j) {
                  case 0:   //Regular
                     pkt.Tag57_ = "0";
                     pkt.Tag38_ = "1";
                     pkt.Tag40_ = "2";
                     pkt.Tag10002_ = "0";
                     break;
                  case 1:   //Fixed-Price
                     pkt.Tag57_ = "7";
                     pkt.Tag38_ = "1";
                     pkt.Tag40_ = "Z";
                     pkt.Tag10002_ = "0";
                     break;
                  case 2:   //Odd-lot
                     pkt.Tag57_ = "2";
                     pkt.Tag38_ = "1";
                     pkt.Tag40_ = "2";
                     pkt.Tag10002_ = "2";
                     break;
                  }
                  //End:20100810
                  k07.clear();
                  k07 = pkt.ToString();
                  file->Write(k07.c_str());
                  //file->Write(newline);
               }
            }  
         }
         delete file;
      }
   }  
   //delete file;

   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert("XF1100", "K07", "K07檔案處理完成");
   }
   WriteMsg("K07", "K07檔案處理完成");
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessK08(TMarket& mkt, TFileType& type, TStringV& v)
{
   int  i = 0;
   SK08Data2  data;  

   std::vector<SK08Data2>  vK08Data2;
   vK08Data2.clear();
   char  buf[256] = {0};

   std::string  applyDate;
   TKDate  today = TKDate::Now();
   for(TStringVI  iter=v.begin(); iter!=v.end(); ++iter) {
      if(i == 0) { //Data1不處理
         ++i;
         applyDate = (*iter).substr(0,8);
         TKDate  date;
         date.assign(applyDate);
         if(date != today) {
            if(Mgr_ = GetExcMgr())
               Mgr_->Alert("XF1007", "K08", "K08檔案日期錯誤");
            sprintf(buf, "K08檔案日期錯誤-%s", date.as_string().c_str());
            WriteMsg("K08", buf);
            return;
         }
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      vK08Data2.push_back(data);
   } 
   
   //讀取Main.ini,取得路徑
   //std::string  confPath = GetConfigPath();
   //TKIniFile*   iniFile  = new TKIniFile(std::string(confPath+GMainIni).c_str());
   gConfigPath = GetConfigPath() + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gConfigPath.c_str());
   
   std::string  sessionID, pwd, oldPwd, newPwd;
   std::vector<SK08Data2>::iterator  iter = vK08Data2.begin();
   for(; iter!=vK08Data2.end(); ++iter) {
      sessionID = mkt.as_string() + iter->Broker_.as_string() + iter->FixPVCID_.as_string();
      oldPwd    = iter->OldPasswd_.as_string();
      newPwd    = iter->Passwd_.as_string();
      pwd       = iniFile->ReadString(sessionID, GPwd, "");
      
      if(pwd.empty() || pwd!=oldPwd) {
         if(Mgr_ = GetExcMgr()) {
            sprintf(buf, "FIX Session ID =%s, %s", sessionID.c_str(), pwd.empty()?"無此設定之Session ID":"K08-OLD-PASSWORD不正確");
            Mgr_->Alert("XF1015", "K08", buf);
            WriteMsg("K08", buf);
         }
      } else {
         iniFile->WriteString(sessionID, GPwd, newPwd);
      }
   }
   delete iniFile;

   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert("XF1100", "K08", "K08檔案處理完成");
   }
   WriteMsg("K08", "K08檔案處理完成");
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessK09(TMarket& mkt, TFileType& type, TStringV& v)
{
   SK09  data;
   std::vector<SK09>  vK09Data;
   vK09Data.clear();
   for(TStringVI  iter=v.begin(); iter!=v.end(); ++iter) {
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      vK09Data.push_back(data);
   } 
   //寫入檔案K09
   std::string  k09;
   char newline[] = {0x0A};
   std::string  fn = GetLogPath();
   fn += "K09";   // $home/Twse/Logs/yymmdd/K09
   TFile*  file = new TFile(fn.c_str(), TFileMode(fmReadWrite|fmCreatePath|fmAppendBit));
   std::vector<SK09>::iterator  iter = vK09Data.begin();
   for(; iter!=vK09Data.end(); ++iter) {
      if(file->IsOpened() == false)
         file->Open(fn.c_str(), TFileMode(fmReadWrite|fmCreatePath|fmAppendBit));
      k09 = (*iter).ToString(); 
      // 20100926 eric 限制寫檔長度, 避免將無用的garbage 也寫入檔案, 造成格式錯亂
      file->Write(k09.c_str(),sizeof(SK09));
      file->Write(newline,1);
   }
   delete file;

   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert("XF1100", "K09", "K09檔案處理完成");
   }
   WriteMsg("K09", "K09檔案處理完成");
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessK11(TMarket& mkt, TFileType& type, TStringV& v)
{
   //fprintf(stderr, "ProcessK11-->1\n");
   int  i = 0;
   SK11Data2  data;
   
   typedef  std::vector<SK11Data2>  VecK11Data2;
   typedef  VecK11Data2::iterator   VecK11Data2I;
   VecK11Data2  vK11Data2;
   vK11Data2.clear();
   char  buf[256] = {0};
   std::string  applyDate;
   TKDate  today = TKDate::Now();
   for(TStringVI  iter=v.begin(); iter!=v.end(); ++iter) {
      if(i == 0) { //Data1不處理
         ++i;
         applyDate = (*iter).substr(0,8);
         TKDate  date;
         date.assign(applyDate);
         if(date != today) {
            if(Mgr_ = GetExcMgr())
               Mgr_->Alert("XF1007", "K11", "K11檔案日期錯誤");
            sprintf(buf, "K11檔案日期錯誤-%s", date.as_string().c_str());
            WriteMsg("K11", buf);
            return;
         }
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      vK11Data2.push_back(data);
   }
   
   TK11MapI  iter_K11;
   TK11MapU  upd_K11;
   for(VecK11Data2I  iter = vK11Data2.begin(); iter!=vK11Data2.end(); ++iter) {
      //iter_K11 = K11Map_->fetch(data.BfBroker_);
      iter_K11 = K11Map_->fetch(iter->BfBroker_);
      upd_K11  = K11Map_->begin_update(iter_K11);
      *upd_K11 = iter->AfBroker_; 
      //*upd_K11 = data.AfBroker_; 
      //fprintf(stderr, "data.BfBroker_=%s, data.AfBroker_=%s", data.BfBroker_.as_string().c_str(), data.AfBroker_.as_string().c_str());
   }
   
   //讀取Main.ini,取得路徑()
   //std::string  tmpPath = GetTmpPath();
   //TKIniFile*   iniFile  = new TKIniFile(std::string(tmpPath+GMainIni).c_str());
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gTmpPath.c_str());
   if(mkt == m_Tse)
      IsTK11Ready_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   else
      IsOK11Ready_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   
   iniFile->WriteString(GXFT, mkt==m_Tse?GTK11Ready:GOK11Ready, mkt==m_Tse?IsTK11Ready_:IsOK11Ready_);
   delete iniFile;

   if(GetAppiaLocked() == false) {
      ConfigK11();
   } else {
      WriteMsg("K11", "Appia IsLocked=true!");
      StartK11Timer(30);
   }
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessK12(TMarket& mkt, TFileType& type, TStringV& v)
{
   int  i = 0;
   SK12Data2  data;
   std::string  applyDate, tmp, settingStr;
   int  multiplier, multiplierS;
   TKDate  today = TKDate::Now();
   char  buf[256] = {0};   
   //讀取Main.ini,取得路徑()
   //std::string  tmpPath = GetTmpPath();
   //TKIniFile*   iniFile = new TKIniFile(std::string(tmpPath+GMainIni).c_str());
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gTmpPath.c_str());

   for(TStringVI  iter=v.begin(); iter!=v.end(); ++iter) {
      if(i == 0) { //Data1不處理
         ++i;
         applyDate = (*iter).substr(0,8);
         TKDate  date;
         date.assign(applyDate);
         if(date != today) {
            if(Mgr_ = GetExcMgr())
               Mgr_->Alert("XF1007", "K12", "K12檔案日期錯誤");
            sprintf(buf, "K12檔案日期錯誤-%s", date.as_string().c_str());
            WriteMsg("K12", buf);
            return;
         }
         tmp = (*iter).substr(8, 4);
         multiplier = atoi(tmp.c_str());
         //if(multiplier <= 0) {
         if(multiplier < 0) {  //20100821:Base_Flow需可以設定為0,表示不限制
            if(Mgr_ = GetExcMgr())
               Mgr_->Alert("XF1007", "K12", "K12 Base Flow錯誤");
            WriteMsg("K12", "K12_Base_Flow錯誤");
            return;
         }
         iniFile->WriteInt(GXFT, GMultiplier, multiplier);
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      settingStr += mkt.as_string() + data.Broker_.as_string() + data.FixPVCID_.as_string();
      settingStr += "/"; //分隔
      tmp = data.TotalFlow_.as_string();
      multiplierS = atoi(tmp.c_str());
      //if(multiplierS <= 0) {
      if(multiplierS < 0) { //20100821:Total Flow為0,表示禁止
         sprintf(buf, "%s%s%s Total Flow Error.", mkt.as_string().c_str(), data.Broker_.as_string().c_str(), data.FixPVCID_.as_string().c_str());
         if(Mgr_ = GetExcMgr())
            Mgr_->Alert("XF1007", "K12", buf);
         WriteMsg("K12", buf);
         return;
      }
      settingStr += tmp + ":"; //分隔
   }
   iniFile->WriteString(GXFT, GMultiplierS, settingStr);
   delete iniFile;

   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert("XF1100", "K12", "K12檔案處理完成");
   }
   WriteMsg("K12", "K12檔案處理完成");
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessK13(TMarket& mkt, TFileType& type, TStringV& v)
{
   //fprintf(stderr, "ProcessK13-->1\n");
   int  i = 0;
   SK13Data2  data;
   
   typedef  std::vector<SK13Data2>  VecK13Data2;
   typedef  VecK13Data2::iterator   VecK13Data2I;
   VecK13Data2  vK13Data2;
   vK13Data2.clear();
   char  buf[256] = {0};
   std::string  applyDate;
   TKDate  today = TKDate::Now();
   for(TStringVI  iter=v.begin(); iter!=v.end(); ++iter) {
      if(i == 0) { //Data1不處理
         ++i;
         applyDate = (*iter).substr(0,8);
         TKDate  date;
         date.assign(applyDate);
         if(date != today) {
            if(Mgr_ = GetExcMgr())
               Mgr_->Alert("XF1007", "K13", "K13檔案日期錯誤");
            sprintf(buf, "K13檔案日期錯誤-%s", date.as_string().c_str());
            WriteMsg("K13", buf);
            return;
         }
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      vK13Data2.push_back(data);
   }
   
   TK13MapI  iter_K13;
   TK13MapU  upd_K13;
   SK13Key   key;
   SK13Value value;
   for(VecK13Data2I  iter = vK13Data2.begin(); iter!=vK13Data2.end(); ++iter) {
      key.Broker_.assign(iter->Broker_.as_string().substr(0,4));
      key.FixPVCID_.assign(iter->FixPVCID_.as_string());
      iter_K13 = K13Map_->fetch(key);
      upd_K13  = K13Map_->begin_update(iter_K13);
      value.BfFixVer_.assign(iter->BfFixVer_.as_string());
      value.AfFixVer_.assign(iter->AfFixVer_.as_string());
      *upd_K13 = value; 
   }
   
   //讀取Main.ini,取得路徑()
   //std::string  tmpPath = GetTmpPath();
   //TKIniFile*   iniFile  = new TKIniFile(std::string(tmpPath+GMainIni).c_str());
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gTmpPath.c_str());
   if(mkt == m_Tse)
      IsTK13Ready_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   else
      IsOK13Ready_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   
   iniFile->WriteString(GXFT, mkt==m_Tse?GTK13Ready:GOK13Ready, mkt==m_Tse?IsTK13Ready_:IsOK13Ready_);
   delete iniFile;

   if(GetAppiaLocked() == false) {
      ConfigK13();
   } else {
      WriteMsg("K13", "Appia IsLocked=true!");
      StartK13Timer(30);
   }
}
//---------------------------------------------------------------------------
K_mf(bool)  TwseFilesHandler::SetK01IniFile(SK01Data2& k01, TKIniFile& main, TKIniFile& xtwse, TKIniFile& appia, std::string& applyDate)
{
   std::string  broker = k01.Broker_.as_string().substr(0,4);
   std::string  key = k01.Market_.as_string() + broker + k01.TmpPVCID_.as_string(); //T5380X1
   //fprintf(stderr, "key=%s-%s-%s\n", k01.Market_.as_string().c_str(), broker, k01.TmpPVCID_.as_string().c_str());
   std::string  connS, key1, key2;
   std::string  appiaKey = ConvertTand2Appia(key); //T5U3U8UAUXU1U
   std::string  verSrc, fixVer;
   
   char  buf[256] = {0};
   sprintf(buf, "ID=%s", key.c_str());
   
   switch(k01.WhichType()) {
   case est_None:   
      if(Mgr_ = GetExcMgr()) {
         sprintf(buf, "XTester設定失敗, ID=%s, K01-TYPE錯誤", key.c_str());
         Mgr_->Alert("XF1006", "K01", buf);
         WriteMsg(GXFT, buf);
	   }
		return false;
   case est_Appia:
      connS = "[" + appiaKey + "]"; //[T5U3U8UAUXU1U]
      //取得middleware name
      key2  = (k01.Market_[0]=='T'?xtwse.ReadString(GSummary, "AppiaMWName", "mTester_T"):xtwse.ReadString(GSummary, "AppiaMWName", "mTester_O"));
      key1  = "[" + key2 + "]";
      //key1  = "[" + (k01.Market_[0]=='T'?xtwse.ReadString(GSummary, "AppiaMWName", "mTester_T"):xtwse.ReadString(GSummary, "AppiaMWName", "mTester_O")) + "]";//"[mTester]";
      sprintf(buf, "Set Appia, appK=%s, mw=%s, id=%s", appiaKey.c_str(), key1.c_str(), key.c_str());
      WriteMsg(GXFT, buf);
      verSrc = k01.FixVersion_.as_string();
      fixVer.clear();
      if(GetAppiaFixVerString(verSrc, fixVer) == false) {
         if(Mgr_ = GetExcMgr()) {
            sprintf(buf, "XTester設定Appia失敗, ID=%s,   K01-FIX-VERSION錯誤", key.c_str());
            Mgr_->Alert("XF1006", "K01", buf);
            sprintf(buf, "FIX version error, id=%s,  appK=%s, recvVer=%s", key.c_str(), appiaKey.c_str(), verSrc.c_str());
            WriteMsg(GXFT, buf);
         }
         return false;
      }
      //新增至[Main]
      appia.WriteStrings(GAppiaMain, GAConnection, connS);
      appia.WriteStrings(GAppiaMain, GMiddleware, key1);
      //新增至[middleware]
      appia.WriteStrings(key2, GMidSesID, appiaKey); 
      //新增至[connection]
      appia.WriteString(appiaKey, GConFixVer, fixVer); //填入FIX版本
      /*eric*///appia.WriteString(appiaKey, GConFixVer, GFIX42);
      appia.WriteString(appiaKey, GNetAddress, k01.BrokerIP_.as_string());
      appia.WriteString(appiaKey, GRemotePort, k01.PVCNo_.as_string()); //[Add]20110523
      /*eric*///appia.WriteString(appiaKey, GLocalFirmID, k01.Market_[0]=='T'?GTwse:GOtc);
      appia.WriteString(appiaKey, GBrokerFirmID, key); //[T5U3U8UAUXU1U] remote_firm_id=T5380X1
      /*eric*///appia.WriteString(appiaKey, GHeartbeatInt, "30");
      /*eric*///appia.WriteString(appiaKey, GStartSeqNum, "1");
      //20100714 by eric 測試線路亦由AppiaParam.ini取得共用設定
      WriteDefaultAppia(appia, appiaKey, false);
      //appia.WriteString(appiaKey, GTradingSes, GTradingSesTB); //[Del]20100617:不需要此部份了
      //設定回覆K02需要的額外資訊
      xtwse.WriteStrings(GSummary, GSesID, key);
      xtwse.WriteString(key, GTestNo, k01.TestNo_.as_string());
      xtwse.WriteString(key, GType, k01.Type_.as_string());
      xtwse.WriteString(key, GSeqNo, k01.ApplySeqNo_.as_string());
      //xtwse.WriteString(key, GAPCode, k01.APCode_.as_string());
      xtwse.WriteString(key, GPortName, k01.PortName_.as_string());
      xtwse.WriteString(key, GSourceIP, k01.BrokerIP_.as_string());
      xtwse.WriteString(key, GApplyDate, applyDate);
      xtwse.WriteString(key, GPwd, k01.Passwd_.as_string());
      xtwse.WriteString(key, GSPort, k01.PVCNo_.as_string());
      xtwse.WriteString(key, GConFixVer, verSrc==std::string("1")?"42":"44"); //for cooper using
      if(Mgr_ = GetExcMgr()) {
         Mgr_->Alert("XF1041", "K01", buf);
      }
		break;
   case est_Tmp:
      sprintf(buf, "Set XTester, key=%s", key.c_str());
      WriteMsg(GXFT, buf);
      //tester設定必須項目
      xtwse.WriteStrings(GSummary, GSesID, key);
      xtwse.WriteString(key, GTestNo, k01.TestNo_.as_string());
      xtwse.WriteString(key, GApplyDate, applyDate);
      xtwse.WriteString(key, GPwd, k01.Passwd_.as_string());
      xtwse.WriteString(key, GSPort, k01.PVCNo_.as_string());
      xtwse.WriteString(key, GType, k01.Type_.as_string());
      xtwse.WriteString(key, GAPCode, k01.APCode_.as_string());
      if(Mgr_ = GetExcMgr()) {
         Mgr_->Alert("XF1040", "K01", buf);
      }
      break;
   }
   return true;
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::SetK04IniFile(SK04Data2& k04, TKIniFile& trade, TMarket& mkt)
{
   char  buf[3] = {0};
   sprintf(buf, "%d", GetSubSystemAP(mkt, k04.APCode_[0]));
   
   std::string  key(buf);
  
   TKTime  time;
   time.assign(k04.BeginTime_.as_string());
   trade.WriteString(key, GBeginTime, time.as_string());
   time.assign(k04.EndTime_.as_string());
   trade.WriteString(key, GEndTime, time.as_string());
}
//---------------------------------------------------------------------------
K_mf(bool)  TwseFilesHandler::ConfigTester()
{
   char  buf[256] = {0};
   //讀取Main.ini,取得各路徑(Appia.ini, XTester.ini)
   std::string  tmpPath = GetTmpPath();
   //TKIniFile*   iniFile = new TKIniFile(std::string(tmpPath+GMainIni).c_str());
   //std::string  xtwse = iniFile->ReadString(GMain, GTesterPath, tmpPath);
   //std::string  appia = GetAppiaPath();//iniFile->ReadString(GMain, GAppiaPath, confPath);
   //if(xtwse[xtwse.length()-1]!='/' && xtwse[xtwse.length()-1]!='\\')
   //   xtwse += "/";
   //xtwse += "XTester.ini"; //設定XTester.ini
   //appia += GAppiaIni;  //設定Appia.ini
   gTmpPath = tmpPath + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gTmpPath.c_str());
   gXTesterPath = iniFile->ReadString(GMain, GTesterPath, tmpPath);
   gAppiaPath   = GetAppiaPath();
   
   if(gXTesterPath[gXTesterPath.length()-1]!='/' && gXTesterPath[gXTesterPath.length()-1]!='\\')
      gXTesterPath += "/";
   gXTesterPath += "XTester.ini"; //設定XTester.ini
   gAppiaPath += GAppiaIni;  //設定Appia.ini
   
   //TKIniFile*  xtwseIni = new TKIniFile(xtwse.c_str());
   //TKIniFile*  appiaIni = new TKIniFile(appia.c_str());
   TKIniFile*  xtwseIni = new TKIniFile(gXTesterPath.c_str());
   TKIniFile*  appiaIni = new TKIniFile(gAppiaPath.c_str());
   
   sprintf(buf, "Start to Configure XTester, T=%s, A=%s", xtwseIni->GetFilePath().c_str(), appiaIni->GetFilePath().c_str());
   WriteMsg(GXFT, buf);
   
   std::string  applyDate;
   bool         isOK = true;
   bool         boolV;
   for(TK01Map::iterator iter=K01Map_->begin(); iter!=K01Map_->end(); ++iter) {
      applyDate = iter->second.ApplyDate_.as_string(Kway::TKDate::sf_ELong, 0);
      boolV = SetK01IniFile(iter->second.K01Data2_, *iniFile, *xtwseIni, *appiaIni, applyDate);
      if(boolV == false)
         isOK = boolV;
   }
   
   delete  appiaIni;
   delete  xtwseIni;
   delete  iniFile;

   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert(isOK?"XF1100":"XF1099", "測試環境", "");
   }
   WriteMsg(GXFT, isOK?"End of XTester Configuration.":"Failure of XTester Configuration.");
   return bool(isOK);
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ConfigProduction()
{
   WriteMsg(GXFT, "Got K03 n K05, Start to Configure Production.");
   //Lock檔案使用權的旗標
   std::string  appia = GetAppiaPath();
   appia += GAppiaIni;  //設定Appia.ini
   TKIniFile*  ini = new TKIniFile(appia.c_str());
   ini->WriteBool(GKway, GIsLocked, true);
   delete ini;

   //std::string  tmpPath = GetTmpPath();
   /* 取消不複製了
   //複製備份檔至Settings下
   char cmd[128] = {0};
   sprintf(cmd, "cp -p ./Backup/Main.ini %s", confPath.c_str());
   pclose(popen(cmd, "r")); 
   */   
   //TKIniFile*   iniFile  = new TKIniFile(std::string(tmpPath+GMainIni).c_str());
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gTmpPath.c_str());
   //std::string  appia = GetAppiaPath();
   //appia += GAppiaIni;  //設定Appia.ini
   TKIniFile*  appiaIni = new TKIniFile(appia.c_str());
   
   char  buf[256] = {0};
   sprintf(buf, "Access Files T=%s, A=%s", iniFile->GetFilePath().c_str(), appiaIni->GetFilePath().c_str());
   WriteMsg(GXFT, buf);
   
   //SKey4Prod  key;
   SKey4ProdSort  key;
   SK03Data2  value;
   bool  isOK = true;
   bool  boolV;
   //以K03以及K05Map建立Production設定
   for(TK03Map::iterator iter=K03Map_->begin(); iter!=K03Map_->end(); ++iter) {
      key   = iter->first;
      value = iter->second.K03Data2_;
      boolV = ConfigOrSave(key, value, *iniFile, *appiaIni);
      if(boolV == false) {
         isOK = boolV;
         WriteMsg(GXFT, "Config K03/K05 Error.");
      }
   }
   //建立流量控管檔案ef-config.xml以及throttle.xml
   //boolV = ConfigThrottle(*iniFile, *appiaIni);  //改由GenerateK10再設定xml
   delete iniFile;
   delete appiaIni;
   
   //if(boolV == false)
   //   isOK = boolV;
   //特別要另外重建K01的設定
   boolV = ConfigTester();
   if(boolV == false)
      isOK = boolV;
   //釋放檔案使用權的旗標
   //appiaIni->WriteBool(GMain, GIsLocked, false);
   //appiaIni->WriteBool(GAppiaMain, GIsLocked, false);
   appiaIni = new TKIniFile(appia.c_str());
   appiaIni->WriteBool(GKway, GIsLocked, false);
   delete appiaIni;
   
   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert(isOK?"XF1100":"XF1099", "正式環境", "");
      sprintf(buf, "Config Prod %s!", isOK?"OK":"Failed");
      WriteMsg(GXFT, buf);
   }
   if(GetK11Done() && GetK13Done())
      GenerateK10();
   else
      StartK10Timer(30);
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ConfigK11()
{
   WriteMsg("K11", "Start to Configure K11.");
   char buf[256] = {0};
   Kway::Tw::Stk::TMarket  procMkt = GetProcMkt();
   TMarket  twse, otc(m_Otc);
   std::string  mkt = procMkt.as_string();
   //讀取Main.ini,取得路徑
   //Lock檔案使用權的旗標
   std::string  appia = GetAppiaPath() + GAppiaIni;  //設定Appia.ini
   TKIniFile*  ini = new TKIniFile(appia.c_str());
   ini->WriteBool(GKway, GIsLocked, true);
   delete ini;

   //std::string  tmpPath = GetTmpPath();
   //TKIniFile*   mainIni = new TKIniFile(std::string(tmpPath+GMainIni).c_str());
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*   mainIni = new TKIniFile(gTmpPath.c_str());
   TKIniFile*  appiaIni = new TKIniFile(appia.c_str());
   
   std::string  nextDate = mainIni->ReadString(GXFT, GNextDate, ""); //K11要變更所有Block的ApplyDate
   sprintf(buf, "Get [XFT] NextDate=%s", nextDate.empty()?"Empty":nextDate.c_str());
   WriteMsg("K11", buf);
   
   TStrings  Brks, SesIDs, tmpIDs, NewSesIDs;
   TStrings::iterator  iter_cat;
   std::string  bfBrk, afBrk, bfBrkMkt, afBrkMkt;
   
   mainIni->ReadStrings(GMain, GBroker, Brks);   //讀出所有BrokerID
  
   std::string::iterator  iter_str;
   std::string  tmp, tmp1, tmp2, tmp3, tmp4;
   //將K11裡的內容取出
   //VecK11Data2I  iter = vK11Data2.begin();
   //fprintf(stderr, "2.K11Map_=%d\n", K11Map_->size());
   for(TK11MapI  iter=K11Map_->begin(); iter!=K11Map_->end(); ++iter) {
      bfBrk = iter->first.as_string();//BfBroker_.as_string();  //1200
      //fprintf(stderr, "bfBrk=%s\n", bfBrk.c_str());
      iter_cat = std::find(Brks.begin(), Brks.end(), bfBrk);
      if(iter_cat == Brks.end()) {
         //Alert錯誤訊息
         sprintf(buf, "TMP設定檔無原始券商=%s", bfBrk.c_str());
         if(Mgr_ = GetExcMgr())
            Mgr_->Alert("XF1006", "K11", buf);
         WriteMsg("K11", buf);
         continue;
      }
      afBrk = iter->second.as_string();//iter->AfBroker_.as_string(); //111A
      //寫入新的Broker, 刪除舊的
      mainIni->WriteStrings(GMain, GBroker, afBrk); //111A
      mainIni->EraseValue(GMain, GBroker, bfBrk);   //1200
      
      bfBrkMkt = mkt + bfBrk; //T1200
      mainIni->ReadStrings(bfBrkMkt, GFixSes, SesIDs);  //T1200-->T1200X1, T1200X2
      afBrkMkt = mkt + afBrk; //T111A
      //先Copy整個Section,再刪除原Section
      if(mainIni->CopySection(bfBrkMkt, afBrkMkt) == false) { 
         //Alert錯誤訊息,存續券商代號已存在
         sprintf(buf, "TMP設定檔已存在,存續券商=%s", afBrkMkt.c_str());
         if(Mgr_ = GetExcMgr())
            Mgr_->Alert("XF1006", "K11", buf);
         WriteMsg("K11", buf);
         continue;
      }
      mainIni->EraseSection(bfBrkMkt); //刪除原Section
      if(nextDate.empty() == false) //寫入新的ApplyDate, T111A
         mainIni->WriteString(afBrkMkt, GApplyDate, nextDate);
      //HINTS:修改新Section寫入新的FixSes,刪除舊的FixSes      
      for(iter_cat=SesIDs.begin(); iter_cat!=SesIDs.end(); ++iter_cat) {
         tmp = *iter_cat;
         if(tmp.length() < afBrkMkt.length()) {
            //不應有這樣的情況,放棄此筆
            sprintf(buf, "原FixSesID=%s,(%d)長度小於,新Broker=%s, ", afBrkMkt.c_str(), tmp.length(), afBrkMkt.c_str());
            if(Mgr_ = GetExcMgr())
               Mgr_->Alert("XF1006", "K11", buf);
            WriteMsg("K11", buf);
            continue;
         }
         iter_str = tmp.begin();
         tmp.replace(iter_str, iter_str+afBrkMkt.length(), afBrkMkt);  //T1200X1 replace T111A --> T111AX1
         //NewSesIDs.push_back(tmp); //新的FixSesID
         mainIni->WriteStrings(afBrkMkt, GFixSes, tmp); //新的FixSesID
         mainIni->EraseValue(afBrkMkt, GFixSes, *iter_cat); //刪除舊的FixSesID
         
         mainIni->ReadStrings(*iter_cat, GTMPID, tmpIDs); //讀出所有該FixSesID的TMPID
         //先Copy整個Section
         if(mainIni->CopySection(*iter_cat, tmp) == false) { 
            //Alert錯誤訊息,新FixSesID代號已存在
            sprintf(buf, "TMP設定檔已存在,新FixSesID=%s", tmp.c_str());
            if(Mgr_ = GetExcMgr())
               Mgr_->Alert("XF1006", "K11", buf);
            WriteMsg("K11", buf);
            continue;
         }
         mainIni->EraseSection(*iter_cat); //刪除原Section
         if(nextDate.empty() == false) //寫入新的ApplyDate, T111AX1
            mainIni->WriteString(tmp, GApplyDate, nextDate);
         for(TStrings::iterator  tmpI=tmpIDs.begin(); tmpI!=tmpIDs.end(); ++tmpI) { //01, 02, ....
            tmp1 = *iter_cat + "-" + *tmpI; //原T1200X1-01
            tmp2 = tmp + "-" + *tmpI;       //新T111AX1-01
            if(mainIni->CopySection(tmp1, tmp2) == false) {
               //Alert錯誤訊息,新設定代號已存在
               sprintf(buf, "TMP設定檔已存在,新線路設定=%s", tmp2.c_str());
               if(Mgr_ = GetExcMgr())
                  Mgr_->Alert("XF1006", "K11", buf);
               WriteMsg("K11", buf);
               continue;
            }
            mainIni->EraseSection(tmp1); //刪除原Section
            if(nextDate.empty() == false) //寫入新的ApplyDate, T111AX1
               mainIni->WriteString(tmp2, GApplyDate, nextDate);
         } 
         //更新Appia
         tmp3 = ConvertTand2Appia(*iter_cat);  //轉換原的FixSesID to Appia
         tmp4 = ConvertTand2Appia(tmp);        //轉換新的FixSesID to Appia
         tmp2 = "[" + tmp4 + "]";
         tmp1 = "[" + tmp3 + "]"; 
         appiaIni->WriteStrings(GAppiaMain, GAConnection, tmp2); //新的FixSesID; [T1U1U0U0U1U1U]
         appiaIni->EraseValue(GAppiaMain, GAConnection, tmp1); 
         if(appiaIni->CopySection(tmp3, tmp4) == false) {
            //Alert錯誤訊息,新FixSesID代號已存在
            sprintf(buf, "Appia設定檔已存在,新FixSesID=%s", tmp4.c_str());
            if(Mgr_ = GetExcMgr())
               Mgr_->Alert("XF1006", "K11", buf);
            WriteMsg("K11", buf);
            continue;
         }
         appiaIni->EraseSection(tmp3);  //刪除原FixSesID的connection block
         appiaIni->WriteString(tmp4, GBrokerFirmID, tmp);  //寫入新的remote_firm_id
      }
   }

   //釋放檔案使用權的旗標
   appiaIni->WriteBool(GKway, GIsLocked, false);
   //寫入做完指令
   if(procMkt == m_Tse)
      IsTK11Done_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   else
      IsOK11Done_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   mainIni->WriteString(GXFT, procMkt==m_Tse?GTK11Done:GOK11Done, procMkt==m_Tse?IsTK11Done_:IsOK11Done_);
   delete mainIni;
   delete appiaIni;
   
   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert("XF1100", "K11", "K11檔案處理完成");
   }
   WriteMsg("K11", "End of Configuration.");
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ConfigK13()
{
   WriteMsg("K13", "Start to Configure K13.");
   char buf[256] = {0};
   Kway::Tw::Stk::TMarket  procMkt = GetProcMkt();
   TMarket  twse, otc(m_Otc);
   std::string  mkt = procMkt.as_string();
   //讀取Main.ini,取得路徑
   //Lock檔案使用權的旗標
   std::string  appia = GetAppiaPath() + GAppiaIni;  //設定Appia.ini
   TKIniFile*  ini = new TKIniFile(appia.c_str());
   ini->WriteBool(GKway, GIsLocked, true);
   delete ini;

   //std::string  tmpPath = GetTmpPath();
   //TKIniFile*   mainIni = new TKIniFile(std::string(tmpPath+GMainIni).c_str());
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*   mainIni = new TKIniFile(gTmpPath.c_str());
   TKIniFile*  appiaIni = new TKIniFile(appia.c_str());
   
   std::string  nextDate = mainIni->ReadString(GXFT, GNextDate, ""); //K13要變更所有Block的ApplyDate
   sprintf(buf, "Get [XFT] NextDate=%s", nextDate.empty()?"Empty":nextDate.c_str());
   WriteMsg("K13", buf);
   
   TStrings  Brks, SesIDs;
   TStrings::iterator  iter_cat;
   std::string  broker, brokerMkt, sesID, tmp, tmp1;
   std::string  bfVer, afVer, tmpSrc, tmpVer;

   mainIni->ReadStrings(GMain, GBroker, Brks);   //讀出所有BrokerID
   for(TK13MapI  iter=K13Map_->begin(); iter!=K13Map_->end(); ++iter) {
      broker = iter->first.Broker_.as_string(); //1200
      iter_cat = std::find(Brks.begin(), Brks.end(), broker);
      if(iter_cat == Brks.end()) {
         //Alert錯誤訊息
         sprintf(buf, "TMP設定檔無原始券商=%s", broker.c_str());
         if(Mgr_ = GetExcMgr())
            Mgr_->Alert("XF1006", "K13", buf);
         WriteMsg("K13", buf);
         continue;
      }
      sesID = mkt + iter->first.as_string(); //T1200X1
      brokerMkt = mkt + broker; //T1200
      //版本檢查,有效性與差異性
      tmpSrc = iter->second.BfFixVer_.as_string();
      tmpVer.clear();
      if(GetAppiaFixVerString(tmpSrc, tmpVer) == false) {
         //Alert錯誤訊息
         sprintf(buf, "K13設定檔 K13-VER-BEFORE 欄位有誤, sid=%s, bfV=%s", sesID.c_str(), tmpSrc.c_str());
         if(Mgr_ = GetExcMgr())
            Mgr_->Alert("XF1006", "K13", buf);
         WriteMsg("K13", buf);
         continue;
      }
      bfVer = tmpVer;
      tmpSrc = iter->second.AfFixVer_.as_string();
      tmpVer.clear();
      if(GetAppiaFixVerString(tmpSrc, tmpVer) == false) {
         //Alert錯誤訊息
         sprintf(buf, "K13設定檔 K13-VER-AFTER 欄位有誤, sid=%s, afV=%s", sesID.c_str(), tmpSrc.c_str());
         if(Mgr_ = GetExcMgr())
            Mgr_->Alert("XF1006", "K13", buf);
         WriteMsg("K13", buf);
         continue;
      }
      afVer = tmpVer;
      /* 應該可以不檢查,避免真的有需要使用到
      if(bfVer == afVer) {
         //Alert錯誤訊息
         sprintf(buf, "K13設定檔 K13-VER-BEFORE 與 K13-VER-AFTER 相同, sid=%s, afV=%s", sesID.c_str(), tmpSrc.c_str());
         if(Mgr_ = GetExcMgr())
            Mgr_->Alert("XF1006", "K13", buf);
         WriteMsg("K13", buf);
         continue;
      }
      */
      //更新Main.ini之ApplyDate
      mainIni->ReadStrings(brokerMkt, GFixSes, SesIDs);  //T1200-->T1200X1, T1200X2
      iter_cat = std::find(SesIDs.begin(), SesIDs.end(), sesID);
      if(iter_cat == SesIDs.end()) {
         //Alert錯誤訊息
         sprintf(buf, "TMP設定檔無原始FixSesID=%s", sesID.c_str());
         if(Mgr_ = GetExcMgr())
            Mgr_->Alert("XF1006", "K13", buf);
         WriteMsg("K13", buf);
         continue;
      }
      //if(nextDate.empty() == false) //寫入新的ApplyDate, T1200X1
      //   mainIni->WriteString(sesID, GApplyDate, nextDate); //20110316: TWSE說不需要更新
      //更新Appia
      tmp  = ConvertTand2Appia(sesID);  //轉換原的FixSesID to Appia
      tmp1 = appiaIni->ReadString(tmp, GConFixVer, "");
      if(tmp1.empty()) {
         //Alert錯誤訊息
         sprintf(buf, "Appia無原始FIX版本訊息,FixSesID=%s", tmp.c_str());
         if(Mgr_ = GetExcMgr())
            Mgr_->Alert("XF1006", "K13", buf);
         WriteMsg("K13", buf);
         continue;
      }
      if(tmp1 != bfVer) { //原設定版本不符,僅通知並直接設定為新版本
         //Alert錯誤訊息
         //sprintf(buf, "Appia設定檔原始FIX版本與 K13-VER-BEFORE 不同, FixSesID=%s, appVer=%s", tmp.c_str(), tmp1.c_str());
         sprintf(buf, "Appia VER與 K13-VER-BEFORE 不同, FixSesID=%s, appVer=%s, K13=%s", tmp.c_str(), tmp1.c_str(), bfVer.c_str());
         if(Mgr_ = GetExcMgr())
            Mgr_->Alert("XF1006", "K13", buf);
         WriteMsg("K13", buf);
      }
      mainIni->WriteString(sesID, GConFixVer, tmpSrc==std::string("1")?"42":"44"); //for cooper using
      appiaIni->WriteString(tmp, GConFixVer, afVer);
      sprintf(buf, "FixSesID=%s, %s -> %s", tmp.c_str(), tmp1.c_str(), afVer.c_str());
      WriteMsg("K13", buf);
   }
   
   //釋放檔案使用權的旗標
   appiaIni->WriteBool(GKway, GIsLocked, false);
   //寫入做完指令
   tmp = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   if(procMkt == m_Tse)
      IsTK13Done_ = tmp;
   else
      IsOK13Done_ = tmp;
   //fprintf(stderr, "mkt=%s", procMkt==m_Tse?"Tse":"Otc");
   mainIni->WriteString(GXFT, procMkt==m_Tse?GTK13Done:GOK13Done, procMkt==m_Tse?IsTK13Done_:IsOK13Done_);
   delete mainIni;
   delete appiaIni;
   
   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert("XF1100", "K13", "K13檔案處理完成");
   }
   WriteMsg("K13", "End of Configuration.");
}
//---------------------------------------------------------------------------
//K_mf(bool)  TwseFilesHandler::ConfigOrSave(SKey4Prod& keyProd, SK03Data2& k03, TKIniFile& main, TKIniFile& appia)
K_mf(bool)  TwseFilesHandler::ConfigOrSave(SKey4ProdSort& keyProd, SK03Data2& k03, TKIniFile& main, TKIniFile& appia)
{
   char  buf[512] = {0};
   std::string  tmp, tmp1, tmp2;
   std::string  sessionID = keyProd.SesID_.Market_.as_string() + keyProd.SesID_.Broker_.as_string() + (k03.WhichType()==est_Appia ? k03.TmpPVCID_.as_string() : k03.FixPVCID_.as_string());
   std::string  socketPortSesID = keyProd.GetSesID();
   std::string  mbrkID    = keyProd.SesID_.Market_.as_string() + keyProd.SesID_.Broker_.as_string();
   std::string  appiaKey  = ConvertTand2Appia(sessionID); //T5U3U8UAUXU1U
   sprintf(buf, "socPSID=%s, sid=%s, mbrk=%s, appK=%s", socketPortSesID.c_str(), sessionID.c_str(), mbrkID.c_str(), appiaKey.c_str());
   WriteMsg(GXFT, buf);

   SKey4Prod  key4K05;
   key4K05.Type_ = keyProd.Type_;
   key4K05.SesID_= keyProd.SesID_;
   
   if(k03.WorkCode_[0] == 'D') { //刪除直接執行,不需理會K05 or K04
      SetOnlineDate(keyProd);
      switch(k03.WhichType()) {
      case est_None: 
         //alert
         if(Mgr_ = GetExcMgr()) //檔案重要欄位資料有誤
            Mgr_->Alert("XF1006", "Prod", "");
         WriteMsg(GXFT, "Erase-K03 Type_ Error");
         return false;
      case est_Appia: { //刪除appia.ini以及Main.ini([T140001])
         //刪除appia.ini
         appia.EraseSection(appiaKey); //erase connection block [sessionID]
         tmp = "[" + appiaKey + "]";
         appia.EraseValue(GAppiaMain, GAConnection, tmp); //erase connection in [Main]
         sprintf(buf, "Erase Appia %s and %s", appiaKey.c_str(), tmp.c_str());
         WriteMsg(GXFT, buf);
         /*
         tmp1 = "[m" + sessionID + "]";
         appia.EraseValue(GMain, GMiddleware, tmp1); //erase middleware in [Main]
         appia.EraseSection(std::string("m" + sessionID)); //erase middleware block
         */
         //移除mwport
         //EraseSettingPort(sessionID);
         //刪除Main.ini
         TStrings  tmpStr;
         std::string  tmpids;
         int  count = main.ReadStrings(sessionID, GTMPID, tmpStr); //[T140001]-->TMPID
         if(count > 0) { //[Bug]20100824: 避免無資料進for出現未預期的錯誤
            for(int L=0; L<count; ++L) {
               tmpids += tmpStr[L] + ",";
               tmp = sessionID + "-" + tmpStr[L];
               main.EraseSection(tmp); //[T140001-Z1]
            }
         }
         main.EraseSection(sessionID); //[T140001]
         main.EraseValue(mbrkID, GFixSes, sessionID); //[T1400]-->FixSes
         sprintf(buf, "Erase TMP, TMPIDs=%s sid=%s", tmpids.c_str(), sessionID.c_str());
      }   
      case est_Tmp: //刪除Main.ini資料
         if(k03.APCode_[0] == '3') { //成回線路設定 [T1400]
            main.EraseSection(mbrkID);
            sprintf(buf, "Erase Deal %s", mbrkID.c_str());
         } else if(k03.APCode_[0] == '0') { //[T140001]-->TMPID && [T140001-0A]
            main.EraseValue(sessionID, GTMPID, k03.TmpPVCID_.as_string());
            tmp = sessionID + "-" + k03.TmpPVCID_.as_string();
            main.EraseSection(tmp);
            sprintf(buf, "Erase Order TMPID & Block=%s", tmp.c_str());
         } else {
            if(Mgr_ = GetExcMgr()) //檔案重要欄位資料有誤
               Mgr_->Alert("XF1006", "Prod", "非預期之AP-CODE內容");         
            WriteMsg(GXFT, "執行刪除-K03非預期之AP-CODE");
            return false;
         }
         WriteMsg(GXFT, buf);
         break;
      }
      /*
      //刪除Main.ini
      main.EraseSection(sessionID); //erase [sessionID] block
      main.EraseValue(mbrkID, GFixSes, sessionID); //erase [mbrkID]'s Fis
      //刪除appia.ini
      appia.EraseSection(sessionID); //erase connection block [sessionID]
      appia.EraseValue(GMain, GAConnection, sessionID); //erase connection in [Main]
      appia.EraseSection(std::string("m" + sessionID.substr(1, std::string::npos))); //erase middleware block
      */
      if(Mgr_ = GetExcMgr()) { //刪除成功
         Mgr_->Alert("XF1022", "Prod", ""); //Appia
         Mgr_->Alert("XF1023", "Prod", ""); //TMP
      }
   } else if(k03.WorkCode_[0] == 'I') { //新增需核對K05以及K04     
      std::string  nextDate = main.ReadString(GXFT, GNextDate, "");
      if(nextDate.empty()) {
         if(Mgr_ = GetExcMgr()) //K04之次交易日不存在
            Mgr_->Alert("XF1010", "Prod", "");
         WriteMsg(GXFT, "Insert-K04之次交易日不存在");
         return false;
      }
      
      TKDate  next, effDate;
      next.assign(nextDate);
      effDate.assign(k03.EffDate_.as_string());
   
      if(next >= effDate) { //設定
         //TK05Map::iterator  iter_k05 = K05Map_->find(keyProd);
         TK05Map::iterator  iter_k05 = K05Map_->find(key4K05);
         if(iter_k05 == K05Map_->end()) {
            if(Mgr_ = GetExcMgr()) //找不到對應之K05資料
 	            Mgr_->Alert("XF1013", "Prod", "");
            sprintf(buf, "Insert-無K05資料-M=%s, B=%s, pvc=%s", key4K05.SesID_.Market_.as_string().c_str(), key4K05.SesID_.Broker_.as_string().c_str(), key4K05.SesID_.PVCID_.as_string().c_str());
            WriteMsg(GXFT, buf);
            return false;
         }
         /*
         fprintf(stderr, "iter_k05->Type_=%s, k03.Type_=%s, iter_k05->FixPVCID_=%s, k03.FixPVCID_=%s, iter_k05->PVCName_=%s, k03.PVCName_=%s, iter_k05->APCode_=%s, k03.APCode_=%s, iter_k05->TmpPVCID_=%s, k03.TmpPVCID_=%s\n",
                 iter_k05->second.Type_.as_string().c_str(), k03.Type_.as_string().c_str(), iter_k05->second.FixPVCID_.as_string().c_str(), k03.FixPVCID_.as_string().c_str(), 
                 iter_k05->second.PVCName_.as_string().c_str(), k03.PVCName_.as_string().c_str(), iter_k05->second.APCode_.as_string().c_str(), k03.APCode_.as_string().c_str(),
                 iter_k05->second.TmpPVCID_.as_string().c_str(), k03.TmpPVCID_.as_string().c_str());
         */
         if(iter_k05->second.Type_==k03.Type_ && iter_k05->second.FixPVCID_==k03.FixPVCID_ && 
            iter_k05->second.TmpPVCID_==k03.TmpPVCID_ && iter_k05->second.PVCName_==k03.PVCName_ && iter_k05->second.Passwd_==k03.Passwd_) {
            //tmp = "[" + sessionID + "]";
            tmp2= "[" + appiaKey + "]";
            /*tmp1= main.ReadString(GMain, GMWBegPort, "12000");
		          fprintf(stderr, "sessionID=%s, begPort=%s\n", sessionID.c_str(), tmp1.c_str());
            std::string  mw, mwsid;
            */
            std::string  src, ver;
            //寫入Main.ini, twse.ini 
            switch(k03.WhichType()) {
            case est_None: 
               //alert
               if(Mgr_ = GetExcMgr()) //檔案重要欄位資料有誤
                  Mgr_->Alert("XF1006", "Prod", "K03");
               WriteMsg(GXFT, "Insert-K03 Type_ Error");
               return false;
            case est_Appia:
               src = k03.FixVersion_.as_string();
               if(GetAppiaFixVerString(src, ver) == false) {
                  if(Mgr_ = GetExcMgr()) {
                     sprintf(buf, "設定Appia失敗, appK=%s,   K03-FIX-VERSION錯誤", appiaKey.c_str());
                     Mgr_->Alert("XF1006", "Prod", buf);
                     sprintf(buf, "FIX version error, appK=%s, recvVer=%s", appiaKey.c_str(), src.c_str());
                     WriteMsg(GXFT, buf);
                  }
                  return false;
               }
               //新增至[Main]
               //appia.WriteStrings(GMain, GAConnection, tmp2);
               appia.WriteStrings(GAppiaMain, GAConnection, tmp2);
               //新增至[middleware]
               /* 這個已經不是一對一,所以不用寫入
               //mwsid = "m" + sessionID.substr(1, std::string::npos);
               mwsid = "m" + sessionID;
               mw    = "[" + mwsid + "]";
               appia.WriteStrings(GMain, GMiddleware, mw); //middleware = [mT116001]
               appia.WriteStrings(mwsid, GMidSesID, sessionID);
               appia.WriteString(mwsid, GMidType, GSocket);
               appia.WriteString(mwsid, GSocketPort, GetSocketPort(tmp1, socketPortSesID)); //自行分配
               appia.WriteString(mwsid, GMidGlobalMsg, "OFF");
               appia.WriteString(mwsid, GMidMsgAckPayload, "OFF_POSITIVE_ACKS");
               appia.WriteString(mwsid, GMidSesEvent, "OFF");
               */
               //新增至[connection]
               appia.WriteString(appiaKey, GConFixVer, ver);
               WriteDefaultAppia(appia, appiaKey); //由ini檔指定寫入的key與value
               //appia.WriteString(sessionID, GConFixVer, GFIX42); 
			      /* 這個不用做了
               debug = main.ReadString(key, GNetAddress, "");
			      fprintf(stderr, "NetAddress=%s\n", debug.c_str());
               appia.WriteString(sessionID, GNetAddress, debug); //到暫存Block
               */
               appia.WriteString(appiaKey, GNetAddress, k03.IP_.as_string());
               appia.WriteString(appiaKey, GRemotePort, k03.SrcPort_.as_string()); //[Add]20110523
               appia.WriteString(appiaKey, GLocalFirmID, k03.Market_[0]=='T'?GTwse:GOtc);
               appia.WriteString(appiaKey, GBrokerFirmID, sessionID);
               //appia.WriteString(sessionID, GHeartbeatInt, "30");
               //appia.WriteString(sessionID, GStartSeqNum, "1");
               //appia.WriteString(sessionID, GTradingSes, GTradingSesB);
               //將FIX pwd寫入Main.ini
               main.WriteString(sessionID, GConFixVer, src==std::string("1")?"42":"44"); //for cooper using
               main.WriteString(sessionID, GPwd, iter_k05->second.Passwd_.as_string());
               main.WriteString(sessionID, GTermID, k03.PVCName_.as_string());
               main.WriteString(sessionID, GApplyDate, k03.EffDate_.as_string()); //20100719
               main.WriteString(sessionID, GIP, k03.IP_.as_string()); //20100719
               main.WriteString(sessionID, GLineNo, k03.LineNo_.as_string()); //20100719
               main.WriteString(sessionID, GSPort, k03.SrcPort_.as_string()); //20100719
               //寫入[T1400] --> FixSes
               tmp = sessionID.substr(0, 5);
               main.WriteStrings(tmp, GFixSes, sessionID);
               if(Mgr_ = GetExcMgr()) //Appia正式環境，連線新增OK
                  Mgr_->Alert("XF1020", "Prod", "");
               sprintf(buf, "Insert-Appia key=%s, sid=%s", appiaKey.c_str(), sessionID.c_str());
               WriteMsg(GXFT, buf);
               break;
            case est_Tmp: 
               tmp = keyProd.SesID_.Market_.as_string() + keyProd.SesID_.Broker_.as_string() + k03.FixPVCID_.as_string();
               main.WriteStrings(GMain, GBroker, keyProd.SesID_.Broker_.as_string());
               if(k03.APCode_[0] == '3') { //成回線路設定 [T1099]
                  main.WriteString(mbrkID, GMatPwd, iter_k05->second.Passwd_.as_string());
                  main.WriteString(mbrkID, GMatSesID, k03.TmpPVCID_.as_string());
                  /*
                  debug = main.ReadString(key, sessionID, "");
                  fprintf(stderr, "NetAddress=%s\n", debug.c_str());
                  main.WriteString(mbrkID, GMatSrcPort, debug);
                  */
                  main.WriteString(mbrkID,GMatSrcPort, k03.SrcPort_.as_string());
                  main.WriteString(mbrkID, GTermID, k03.PVCName_.as_string());
                  main.WriteString(mbrkID, GApplyDate, k03.EffDate_.as_string()); //20100719
                  main.WriteString(mbrkID, GIP, k03.IP_.as_string()); //20100719
                  main.WriteString(mbrkID, GLineNo, k03.LineNo_.as_string()); //20100719
                  sprintf(buf, "Insert-Deal %s", mbrkID.c_str());
                  WriteMsg(GXFT, buf);
               } else if(k03.APCode_[0] == '0') { //委託線路設定 
                  //fprintf(stderr, "tmp=%s, k03.TmpPVCID_=%s\n", tmp.c_str(), k03.TmpPVCID_.as_string().c_str());
                  main.WriteStrings(tmp, GTMPID, k03.TmpPVCID_.as_string());
                  /*
                  debug = GetSocketPort(tmp1, socketPortSesID);
                  fprintf(stderr, "tmp1=%s, socketPortSesID=%s, port=%s\n", tmp1.c_str(), socketPortSesID.c_str(), debug.c_str());
                  main.WriteString(tmp, GAppiaMWPort, debug); //自行分配
                  */
                  tmp1 = tmp + "-" + k03.TmpPVCID_.as_string(); //[T109901-A1]
                  /*
                  debug = main.ReadString(key, sessionID, "");
                  fprintf(stderr, "key=%s, sessionID=%s, srcport=%s\n", key.c_str(), sessionID.c_str(), debug.c_str());
                  main.WriteString(tmp1, GSPort, debug);
                  */
                  main.WriteString(tmp1, GSPort, k03.SrcPort_.as_string());
                  main.WriteString(tmp1, GPwd, iter_k05->second.Passwd_.as_string());
                  main.WriteString(tmp1, GTermID, k03.PVCName_.as_string());
                  main.WriteString(tmp1, GApplyDate, k03.EffDate_.as_string()); //20100719
                  main.WriteString(tmp1, GIP, k03.IP_.as_string()); //20100719
                  main.WriteString(tmp1, GLineNo, k03.LineNo_.as_string()); //20100719
                  sprintf(buf, "Insert-Order id=%s", tmp1.c_str());
                  WriteMsg(GXFT, buf);
               } else {
                  if(Mgr_ = GetExcMgr()) //檔案重要欄位資料有誤
                     Mgr_->Alert("XF1006", "Prod", "非預期之K03-AP-CODE內容");         
                  WriteMsg(GXFT, "非預期之K03-AP-CODE");
                  return false;
               }
               if(Mgr_ = GetExcMgr()) //TMP正式環境，連線新增OK
                  Mgr_->Alert("XF1021", "Prod", "");
               break;
            } //switch(...)
            SetOnlineDate(keyProd);
         } else {
            if(Mgr_ = GetExcMgr()) //K03與K05相同欄位資料不符
               Mgr_->Alert("XF1012", "Prod", "");
            sprintf(buf, "Insert-K03與K05相同欄位資料不符-M=%s, B=%s, pvc=%s", keyProd.SesID_.Market_.as_string().c_str(), keyProd.SesID_.Broker_.as_string().c_str(), keyProd.SesID_.PVCID_.as_string().c_str());
            WriteMsg(GXFT, buf);
            return false;
		      //InsertK03Map(keyProd, value);
         }
      } else { //等待next
         if(Mgr_ = GetExcMgr()) //生效日晚於次交易日
            Mgr_->Alert("XF1011", "Prod", "");
         sprintf(buf, "Insert-生效日晚於次交易日-M=%s, B=%s, pvc=%s", keyProd.SesID_.Market_.as_string().c_str(), keyProd.SesID_.Broker_.as_string().c_str(), keyProd.SesID_.PVCID_.as_string().c_str());
         WriteMsg(GXFT, buf);
         return false;
      }
   } else if(k03.WorkCode_[0] == 'U') { //變更TMP SOCKET ID
      if(k03.WhichType() == est_Tmp) {
         tmp = keyProd.SesID_.Market_.as_string() + keyProd.SesID_.Broker_.as_string();// + k03.FixPVCID_.as_string();
         //找出1.市場別 2.券商代號 3.PVC NAME相同的並修改Section名稱
         if(k03.APCode_[0] == '3') { //成回線路更新PVCID, [T1099]
            tmp1 = main.ReadString(tmp, GTermID, "");
            sprintf(buf, "Update-Deal id=%s, pvcN=%s", tmp.c_str(), tmp1.c_str());
            WriteMsg(GXFT, buf);
            if(tmp1 == k03.PVCName_.as_string()) {
               main.WriteString(tmp, GMatSesID, k03.TmpPVCID_.as_string());
            } else {
               if(Mgr_ = GetExcMgr()) //檔案重要欄位資料有誤
                  Mgr_->Alert("XF1006", "Prod", "更新成回: K03-PVC-NAME與原設定不相同");
               WriteMsg(GXFT, "Update-Deal-K03-PVC-NAME與原設定不相同");
            }
         } else if(k03.APCode_[0] == '0') { //委託線路更新PVCID
            TStrings  sesIDs, tmpIDs, tmpStr;
            std::string  tmpS, tmpR;
            std::string::size_type  pos;
            int  count = main.ReadStrings(tmp, GFixSes, sesIDs); //[T140001]
            int  count_tmp = 0;
            if(count > 0) { //[Bug]20100824: 避免無資料進for出現未預期的錯誤
               for(int L = 0; L < count; ++L) {
                  count_tmp = main.ReadStrings(sesIDs[L], GTMPID, tmpStr);
                  if(count_tmp <= 0) //[Bug]20100824: 避免無資料進for出現未預期的錯誤
                     continue;
                  for(int K = 0; K < count_tmp; ++K) {
                     tmpS = sesIDs[L] + "-" + tmpStr[K];
                     tmpIDs.push_back(tmpS); //[T140001-0A]
                  }
               }
            }
            std::string  sid, pvcid;
            for(TStrings::iterator i=tmpIDs.begin(); i!=tmpIDs.end(); ++i) {
               tmpR = main.ReadString(*i, GTermID, "");
               if(tmpR == k03.PVCName_.as_string()) { //檢查PVC-NAME是否相同
                  pos = tmpS.find("-");
                  if(pos == std::string::npos)
                     continue;
                  tmpR = tmpS.substr(0, pos+1);
                  tmpR += k03.TmpPVCID_.as_string();
                  sprintf(buf, "Update-Order id=%s to %s", tmpS.c_str(), tmpR.c_str());
                  WriteMsg(GXFT, buf);
                  if(main.CopySection(tmpS, tmpR)) {  //複製整個section
                     main.EraseSection(tmpS);         //刪除原本的Section
                     if(Mgr_ = GetExcMgr())             //TMP正式環境，連線更新OK
                        Mgr_->Alert("XF1025", "Prod", "");         
                  } else { //複製失敗
                     if(Mgr_ = GetExcMgr()) //TMP正式環境，連線更新失敗
                        Mgr_->Alert("XF1026", "Prod", "");
                     WriteMsg(GXFT, "Update-Order-Copy Section Failed.");
                     return false; 
                  }
                  //fprintf(stderr, "*i=%s -------\n", (*i).c_str());
                  std::string::size_type  pos1 = (*i).find("-");
                  if(pos1 != std::string::npos) { //移除[T140001]之FixSes, 寫入新的FixSes
                     sid   = (*i).substr(0, pos1);
                     pvcid = (*i).substr(pos1+1, std::string::npos);
                     //fprintf(stderr, "sid=%s, pvcid=%s\n", sid.c_str(), pvcid.c_str());
                     main.EraseValue(sid, GTMPID, pvcid);
                     main.WriteStrings(sid, GTMPID, k03.TmpPVCID_.as_string());
                  } else { //正常不會跑這段
                     //alert
                     if(Mgr_ = GetExcMgr()) //TMP正式環境，連線更新失敗
                        Mgr_->Alert("XF1026", "Prod", "");
                     WriteMsg(GXFT, "Update-Order-Cannot Find \"-\" In id.");
                     return false; 
                  }
                  return true;
               }
            }
            //alert
            if(Mgr_ = GetExcMgr()) //TMP正式環境，連線更新失敗
               Mgr_->Alert("XF1026", "Prod", "找不到相同之PVC-NAME");
            WriteMsg(GXFT, "Update-Order-找不到相同之PVC-NAME");
            return false;
         } else {
            //alert
            if(Mgr_ = GetExcMgr()) //檔案重要欄位資料有誤
               Mgr_->Alert("XF1006", "Prod", "非預期之K03-AP-CODE內容");         
            WriteMsg(GXFT, "Update-非預期之K03-AP-CODE");
            return false;
         }
      } else {
         //alert
         if(k03.WhichType() == est_Appia) {
            if(Mgr_ = GetExcMgr()) //檔案重要欄位資料有誤
               Mgr_->Alert("XF1006", "Prod", "不可更新Appia的SessionID");
            WriteMsg(GXFT, "Update-不可更新Appia的SessionID");
         } else {
            if(Mgr_ = GetExcMgr()) //檔案重要欄位資料有誤
               Mgr_->Alert("XF1006", "Prod", "非預期之K03-TYPE內容");
            WriteMsg(GXFT, "Update-非預期之K03-TYPE");
         }
         return false;
      }
   } else {
      //alert
      if(Mgr_ = GetExcMgr()) //檔案重要欄位資料有誤
         Mgr_->Alert("XF1006", "Prod", "非預期之K03-WK-CODE內容");
      WriteMsg(GXFT, "非預期之K03-WK-CODE");
   }
   return true;
   //fprintf(stderr, "2-->K03Map size=%d\n", K03Map_->size());
}
//---------------------------------------------------------------------------
K_mf(bool)  TwseFilesHandler::ChkNInsertData(SK01Data2& k01, std::string& applyDate)
{
   if(k01.Market_.empty() || (k01.Market_[0]!='T' && k01.Market_[0]!='O') || k01.Broker_.empty() || k01.TmpPVCID_.empty()) {
      WriteMsg("K01", "Mkt, Brk or Pvc Data Error.");
      return false;
   }
     
   std::string  tmp = k01.Broker_.as_string();
   if(tmp.length() < 4) {
      WriteMsg("K01", "Broker ID Length Error.");
      return false;
   }
   
   std::string  broker = k01.Broker_.as_string().substr(0,4);

   SKey4Prod  key;
   key.Type_ = k01.Type_;
   key.SesID_.Market_ = k01.Market_;
   key.SesID_.Broker_.assign(broker);
   key.SesID_.PVCID_ = k01.TmpPVCID_;
   SProdValues  value;
   //不保留給K03了
   //InsertK03Map(key, value);
   
   SK01Values  v;
   v.ApplyDate_.assign(applyDate);
   v.K01Data2_ = k01;
   
   TK01Map::iterator  iter = K01Map_->fetch(key);
   TK01Map::updater   upd  = K01Map_->begin_update(iter);
   *upd = v;
   
   return true;
}
//---------------------------------------------------------------------------
K_mf(bool)  TwseFilesHandler::ChkNInsertData(SK03Data2& k03, TKDate& today, dword& seq)
{
   if(k03.Market_.empty() || (k03.Market_[0]!='T' && k03.Market_[0]!='O') || k03.Broker_.empty() || 
      k03.TmpPVCID_.empty() || (k03.WorkCode_[0]!='I' && k03.WorkCode_[0]!='D' && k03.WorkCode_[0]!='U')) {
      WriteMsg("K03", "Mkt, Brk, Pvc or Wk Data Error.");
      return false;
   }
   
   std::string  tmp = k03.Broker_.as_string();
   if(tmp.length() < 4) {
      WriteMsg("K03", "Broker ID Length Error.");
      return false;
   }
   
   //SKey4Prod  keyProd;
   SKey4ProdSort  keyProd;
   keyProd.Date_ = today;
   keyProd.Seq_  = seq;
   keyProd.Type_ = k03.Type_;
   keyProd.SesID_.Market_ = k03.Market_;
   keyProd.SesID_.Broker_.assign(k03.Broker_.as_string().substr(0,4));
   keyProd.SesID_.PVCID_ = k03.TmpPVCID_;
   SProdValues  value;
   //value.SettingDate_ = TKDate::Now();  //設定日
   value.SettingDate_ = today;  //設定日
   value.K03Data2_ = k03;
   //value.FillK03Data2(k03);
   
   InsertK03Map(keyProd, value);
   return true;
}
//---------------------------------------------------------------------------
K_mf(bool)  TwseFilesHandler::ChkNInsertData(SK05Data2& k05, TMarket& mkt)
{
   if((k05.Type_[0]!='K' && k05.Type_[0]!='F') || k05.Broker_.empty() || k05.TmpPVCID_.empty()) {
      WriteMsg("K05", "Type, Brk or Pvc Data Error.");
      return false;
   }
     
   SKey4Prod  keyProd;
   keyProd.Type_ = k05.Type_;
   keyProd.SesID_.Market_.assign(mkt.as_string());
   keyProd.SesID_.Broker_.assign(k05.Broker_.as_string());
   keyProd.SesID_.PVCID_ = k05.TmpPVCID_;
   
   InsertK05Map(keyProd, k05);
   return true;
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::CheckK03Today()
{
   //std::vector<SKey4Prod>  Vec;
   std::vector<SKey4ProdSort>  Vec;
   Vec.clear();
   TKDate  today = TKDate::Now();
   TK03Map::iterator   iter_K03;
   for(iter_K03 = K03Map_->begin(); iter_K03!=K03Map_->end(); ++iter_K03) {
      if(iter_K03->second.SettingDate_ == today)
         Vec.push_back(iter_K03->first);
   }
   //for(std::vector<SKey4Prod>::iterator  iter=Vec.begin(); iter!=Vec.end(); ++iter) {
   for(std::vector<SKey4ProdSort>::iterator  iter=Vec.begin(); iter!=Vec.end(); ++iter) {
      iter_K03 = K03Map_->find(*iter);
      if(iter_K03 != K03Map_->end())
         K03Map_->erase(*iter);
   }
}
//---------------------------------------------------------------------------
/*
K_mf(void)  TwseFilesHandler::EraseSettingPort(std::string& sessionID)
{
   TSesIDsMap::iterator  iter = SesIDsMap_->find(sessionID);
   if(iter == SesIDsMap_->end())
      return;
   
   int  port = iter->second;
   SesIDsMap_->erase(sessionID);
   PortsMap_->erase(port);
}
*/
//---------------------------------------------------------------------------
/*
K_mf(std::string)  TwseFilesHandler::GetSocketPort(std::string& begPort, std::string& sessionID)
{
   fprintf(stderr, "GetSocketPort-->beginP=%s, sesID=%s\n", begPort.c_str(), sessionID.c_str());
   char buf[16] = {0};
   TSesIDsMap::iterator  iter_sid = SesIDsMap_->find(sessionID);
   if(iter_sid != SesIDsMap_->end()) {
      sprintf(buf, "%d", iter_sid->second);
      fprintf(stderr, "GetSessionID and return port=%s\n", buf);
	     return std::string(buf);
   }
   
   iter_sid = SesIDsMap_->fetch(sessionID);
   TSesIDsMap::updater  upd_sid = SesIDsMap_->begin_update(iter_sid);
   
   //沒有對應的Port需自行編列
   int  port = atoi(begPort.c_str());
   fprintf(stderr, "port=%d\n");
   TPortsMap::iterator  iter_port = PortsMap_->find(port);
   TPortsMap::updater   upd_port;
   if(iter_port == PortsMap_->end()) {
      fprintf(stderr, "Cannot find port\n");
	     iter_port= PortsMap_->fetch(port);
      upd_port = PortsMap_->begin_update(iter_port);
      *upd_port= sessionID;
      *upd_sid = port;
   } else {
      fprintf(stderr, "Find port\n");
	     iter_port = ++(PortsMap_->begin());
      bool  isSet(false); 
      for(; iter_port!=PortsMap_->end(); ++iter_port) {
         int  i = iter_port->first - port;
		       fprintf(stderr, "iter_port->first - port=%d\n", i);
		       //if((iter_port->first - port) > 1) {
		       if(i > 1) {
            ++port;
   		       fprintf(stderr, "1--->port=%d\n", port);
            iter_port= PortsMap_->fetch(port);
            upd_port = PortsMap_->begin_update(iter_port);
            *upd_port= sessionID;
            *upd_sid = port;
            isSet = true;
            break;
         }
         ++port;
      }
      //如果還沒有設定,表示到尾還是連續的
      if(isSet == false) {
         ++port;
		       fprintf(stderr, "2--->port=%d\n", port);
		       iter_port= PortsMap_->fetch(port);
         upd_port = PortsMap_->begin_update(iter_port);
         *upd_port= sessionID;
         *upd_sid = port;
      }
   }
   sprintf(buf, "%d", *upd_sid);
   return std::string(buf);
}
*/
//---------------------------------------------------------------------------
//K_mf(void)  TwseFilesHandler::InsertK03Map(SKey4Prod& key, SK03Data2& value)
//K_mf(void)  TwseFilesHandler::InsertK03Map(SKey4Prod& key, SProdValues& value)
K_mf(void)  TwseFilesHandler::InsertK03Map(SKey4ProdSort& key, SProdValues& value)
{
   TK03Map::iterator  iter = K03Map_->fetch(key);
   TK03Map::updater   upd  = K03Map_->begin_update(iter);
   *upd = value;
   //fprintf(stderr, "InsertK03Map-->K03Map size=%d\n", K03Map_->size());
}
//---------------------------------------------------------------------------
//K_mf(void)  TwseFilesHandler::SetOnlineDate(SKey4Prod& key)
K_mf(void)  TwseFilesHandler::SetOnlineDate(SKey4ProdSort& key)
{
   TK03Map::iterator  iter = K03Map_->find(key);
   if(iter == K03Map_->end())
      return;

   TK03Map::updater   upd  = K03Map_->begin_update(iter);
   upd->OnlineDate_ = TKDate::Now();
}
//---------------------------------------------------------------------------
K_mf(bool)  TwseFilesHandler::CheckK03orK05(TMarket& mkt, TFileType type)
{
   if(type!=ft_K03 && type!=ft_K05)
      return false;
   
   TMarket  twse, otc(m_Otc);
   TKDate   date;
   //std::string  tmpPath = GetTmpPath();   
   //TKIniFile*  iniFile = new TKIniFile(std::string(tmpPath+GMainIni).c_str());
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gTmpPath.c_str());
   if(mkt == twse) {         
      if(type == ft_K03) {
         IsTK03Ready_ = iniFile->ReadString(GXFT, GTK03Ready, "");
         if(IsTK03Ready_.empty())
            return false;
         date.assign(IsTK03Ready_);
         if(date != TKDate::Now())
            return false;
      } else {
         IsTK05Ready_ = iniFile->ReadString(GXFT, GTK05Ready, "");
         if(IsTK05Ready_.empty())
            return false;
         date.assign(IsTK05Ready_);
         if(date != TKDate::Now())
            return false;
      }
   } else if(mkt == otc) {
      if(type == ft_K03) {
         IsOK03Ready_ = iniFile->ReadString(GXFT, GOK03Ready, "");
         if(IsOK03Ready_.empty())
            return false;
         date.assign(IsOK03Ready_);
         if(date != TKDate::Now())
            return false;
      } else {
         IsOK05Ready_ = iniFile->ReadString(GXFT, GOK05Ready, "");
         if(IsOK05Ready_.empty())
            return false;
         date.assign(IsOK05Ready_);
         if(date != TKDate::Now())
            return false;
      }
   } else
      return false;
   
   delete iniFile;
   return true;
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::CheckK03Map()
{
   TKDate  today = TKDate::Now();
   //std::vector<SKey4Prod>  map;
   std::vector<SKey4ProdSort>  map;
   for(TK03Map::iterator  iter = K03Map_->begin(); iter!=K03Map_->end(); ++iter) {
      if(iter->second.OnlineDate_.IsInvalid())
	        continue;
	     //if(iter->second.OnlineDate_ > today) //這個應該永遠也不會有
	     if(iter->second.OnlineDate_ < today)  //改為小於當天的移除,移除過期的
	        map.push_back(iter->first);
   }
   
   //for(std::vector<SKey4Prod>::iterator  iter=map.begin(); iter!=map.end(); ++iter)
   for(std::vector<SKey4ProdSort>::iterator  iter=map.begin(); iter!=map.end(); ++iter)
      K03Map_->erase(*iter);
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::InsertK05Map(SKey4Prod& key, SK05Data2& value)
{
   TK05Map::iterator  iter = K05Map_->fetch(key);
   TK05Map::updater   upd  = K05Map_->begin_update(iter);
   *upd = value;
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::RebuildK05Map(TKIniFile& main)
{
   Kway::Tw::Stk::TMarket  procMkt = GetProcMkt();
   std::string  fType("K05");
   TMarket  twse, otc(m_Otc);
   std::string  File;
   if(procMkt == twse) {
      File = main.ReadString(GXFT, GTK05File, "");
   } else if(procMkt == otc) {
      File = main.ReadString(GXFT, GOK05File, "");
   }
   
   if(File.empty() == false) {
      StartHandleAFile(procMkt, fType, File, true);
   }
   
   /*
   std::string  TseFile = main.ReadString(GXFT, GTK05File, "");
   std::string  OtcFile = main.ReadString(GXFT, GOK05File, "");
   //std::string  TsePath = main.ReadString(GMain, GTK05Path, "");
   //std::string  TseFile = main.ReadString(GMain, GTK05File, "");
   //std::string  OtcPath = main.ReadString(GMain, GOK05Path, "");
   //std::string  OtcFile = main.ReadString(GMain, GOK05File, "");
   
   //if(TsePath.empty() == false) {
   if(TseFile.empty() == false) {
      TMarket  mkt;
      StartHandleAFile(mkt, fType, TseFile);
   }
   
   //if(OtcPath.empty() == false) {
   if(OtcFile.empty() == false) {
      TMarket  mkt(m_Otc);
      StartHandleAFile(mkt, fType, OtcFile);
   }
   */
   WriteMsg(GXFT, "RebuildK05Map Done!");
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ReadMultiplierS(TKIniFile& main, TSesIDsMap& map)
{
   std::string  tmp = main.ReadString(GXFT, GMultiplierS, "");
   if(tmp.empty())
      return;
   //拆解GMultiplierS = T1110/20:T1120/30:
   std::string::size_type  pos, pos1;
   std::string  s, s1;
   int  flow;
   for(pos=tmp.find(":"); pos!=std::string::npos; pos=tmp.find(":")) {
      s = tmp.substr(0, pos); //T1110/20
      pos1 = s.find("/");
      if(pos1 == std::string::npos) {
         if(pos+1 == std::string::npos)
            break;
         tmp = tmp.substr(pos+1, std::string::npos);
         continue;
      }
      s1 = s.substr(pos1+1, std::string::npos);
      flow = atoi(s1.c_str());
      //if(flow <=0) {
      if(flow < 0) { //20100823: flow可以為0
         if(pos+1 == std::string::npos)
            break;
         tmp = tmp.substr(pos+1, std::string::npos);
         continue;
      }
      TSesIDsMap::value_type  v(s.substr(0, pos1), flow);
      map.insert(v);
      if(pos+1 == std::string::npos)
         break;
      tmp = tmp.substr(pos+1, std::string::npos);
   }
}
//---------------------------------------------------------------------------
K_mf(bool)  TwseFilesHandler::ConfigThrottle(TKIniFile& main, TKIniFile& appia)
{
   char buf[256] = {0};
   WriteMsg(GXFT, "Start to Configure ef-config.xml.");
   //建立流量控管檔案ef-config.xml以及throttle.xml  
   TSesIDsMap  map; //SesID--Number of PVC --> T140001--5
   map.clear();

   Kway::Tw::Stk::TMarket  mkt = GetProcMkt();
   
   int  multiplier = main.ReadInt(GXFT, GMultiplier, 1);
   sprintf(buf, "Base_Flow=%d", multiplier);
   WriteMsg(GXFT, buf);
   //20100821: Base_Flow為0,表示不控管
   if(multiplier == 0)
      multiplier = -1;
   
   ReadMultiplierS(main, map);  //讀出特別控管的流量
   //讀出所有SessionID
   TStrings  Brks, Sess, Tmps;
   std::string  BrkMkt;
   int  BrkCnt, SesCnt, TmpCnt;
   TSesIDsMap::iterator  iter;
   BrkCnt = main.ReadStrings(GMain, GBroker, Brks);
   sprintf(buf, "Broker_Count=%d", BrkCnt);
   WriteMsg(GXFT, buf);   
   if(BrkCnt > 0) { //[Bug]20100824: 避免無資料進for出現未預期的錯誤
      for(int L = 0; L < BrkCnt; ++L) {
         BrkMkt = mkt.as_string() + Brks[L];
         SesCnt = main.ReadStrings(BrkMkt, GFixSes, Sess);
         if(SesCnt <= 0) //[Bug]20100824: 避免無資料進for出現未預期的錯誤
            continue;
         for(int i = 0; i < SesCnt; ++i) {
            iter = map.find(Sess[i]);
            if(map.find(Sess[i]) == map.end()) {
               TmpCnt = main.ReadStrings(Sess[i], GTMPID, Tmps);
               TSesIDsMap::value_type  v(Sess[i], TmpCnt*multiplier); //流量以PVC數量 乘以 multiplier
               map.insert(v);
            }
         }
      }
      /*
      BrkMkt = "T" + Brks[L];
      int  SesCnt = main.ReadStrings(BrkMkt, GFixSes, Sess);
      for(int i = 0; i < SesCnt; i++) {
         int  TmpCnt = main.ReadStrings(Sess[i], GTMPID, Tmps);
         //TSesIDsMap::value_type  v(Sess[i], TmpCnt*10); //流量以PVC數量 乘以 10倍
         TSesIDsMap::value_type  v(Sess[i], TmpCnt*multiplier); //流量以PVC數量 乘以 multiplier
         map.insert(v);
      }
      BrkMkt = "O"+Brks[L];
      SesCnt = main.ReadStrings(BrkMkt, GFixSes, Sess);
      for(int i = 0; i < SesCnt; ++i) {  
         int  TmpCnt = main.ReadStrings(Sess[i], GTMPID, Tmps);
         //TSesIDsMap::value_type  v(Sess[i], TmpCnt*10); //流量以PVC數量 乘以 10倍
         TSesIDsMap::value_type  v(Sess[i], TmpCnt*multiplier); //流量以PVC數量 乘以 multiplier
         map.insert(v);
      }
      */
   }
   //fprintf(stderr, "map size=%d\n", map.size());
   
   std::string  path = GetAppiaPath();
   std::string  ef_path = path + "ef-config.xml";
   std::string  thro_path = path + "throttle.xml";
   sprintf(buf, "map_size=%d, ef=%s ; thro=%s", map.size(), ef_path.c_str(), thro_path.c_str());
   WriteMsg(GXFT, buf); 
   //sprintf(buf, "檢查%s或%s", ef_path.c_str(), thro_path.c_str());
   sprintf(buf, "檢查%s", ef_path.c_str());//, thro_path.c_str());
   TiXmlDocument  xml;
   xml.LoadFile(ef_path.c_str());
   //<ef-configuration>
   TiXmlElement*  root = xml.RootElement();
   if(root == NULL) {
      //Alert
      if(Mgr_ = GetExcMgr())
         Mgr_->Alert("XF1014", "Prod", buf);
      WriteMsg(GXFT, buf);
      return false;
   }
   //<ef-configuration><plugins>
   TiXmlElement*  plugins = root->FirstChildElement("plugins");
   if(plugins == NULL) {
      //Alert
      if(Mgr_ = GetExcMgr())
         Mgr_->Alert("XF1014", "Prod", buf);
      WriteMsg(GXFT, buf);
      return false;
   }
   //<ef-configuration><plugins><plugin>
   TiXmlElement*  plugin = plugins->FirstChildElement("plugin");
   if(plugin == NULL) {
      //Alert
      if(Mgr_ = GetExcMgr())
         Mgr_->Alert("XF1014", "Prod", buf);
      WriteMsg(GXFT, buf);
      return false;
   }
   std::string  name, code;
   for(plugin; plugin; plugin=plugin->NextSiblingElement("plugin")) {
      if(plugin->Attribute("name")) {
         name = plugin->Attribute("name");
         //fprintf(stderr, "name=%s\n", name.c_str());
         if(name == std::string("Throttle")) {
            //<ef-configuration><plugins><plugin><plugin-filter>
            TiXmlElement*  filter = plugin->FirstChildElement("plugin-filter");
            if(filter == NULL) {
               //Alert
               if(Mgr_ = GetExcMgr())
                  Mgr_->Alert("XF1014", "Prod", buf);
               WriteMsg(GXFT, buf);
               return false;
            }
            for(filter; filter; filter=filter->NextSiblingElement("plugin-filter")) {
               if(filter->Attribute("code")) {
                  code = filter->Attribute("code");
                  //fprintf(stderr, "code=%s\n", code.c_str());
                  if(code == std::string("com.javtech.appia.ef.session.SessionPluginFilter")) {
                     //<ef-configuration><plugins><plugin><plugin-filter><param>
                     TiXmlElement*  param = filter->FirstChildElement("param");
                     if(param == NULL) {
                        //Alert
                        if(Mgr_ = GetExcMgr())
                           Mgr_->Alert("XF1014", "Prod", buf);
                        WriteMsg(GXFT, buf);
                        return false;
                     }
                     for(param; param; param=param->NextSiblingElement("param")) {
                        if(param->Attribute("name") == NULL)
                           continue;
                        name = param->Attribute("name");
                        //fprintf(stderr, "2.name=%s\n", name.c_str());
                        if(name != std::string("SessionName"))
                           continue;
                        if(param->Attribute("value") == NULL) 
                           break;
                        name = param->Attribute("value");
                        //fprintf(stderr, "3.name=%s\n", name.c_str());
                        sprintf(buf, "SessionName-value=%s", name.c_str());
                        WriteMsg(GXFT, buf);
                        std::string  tmp = MakeString(name, map);
                        //fprintf(stderr, "value=%s, tmp=%s\n", name.c_str(), tmp.c_str());                        
                        param->SetAttribute("value", tmp.c_str());
                        sprintf(buf, "new-value=%s", tmp.c_str());
                        WriteMsg(GXFT, buf);
                        xml.SaveFile(); //寫入ef-config.xml
                        WriteMsg(GXFT, "Finished ef-config.xml configuration.");
                        sprintf(buf, "檢查%s", thro_path.c_str());
                        return Throttling(map, thro_path, buf); //寫入throttle.xml
                     } //for(param)
                  }
               }
            } //for(plugin-filter)
         }
      }
   } //for(plugin)
   //Alert
   if(Mgr_ = GetExcMgr())
      Mgr_->Alert("XF1014", "Prod", buf);
   WriteMsg(GXFT, buf);
   return false;
}
//---------------------------------------------------------------------------
K_mf(bool)  TwseFilesHandler::Throttling(TSesIDsMap& map, std::string& path, const char* buf)
{
   /*
   <throttle-plugin>
      <session name="T1160">
         <inbound tps="3000"/>
         <outbound tps="3000"/>
      </session>
   </throttle-plugin>
   */
   WriteMsg(GXFT, "Start to Configure throttle.xml");
   Kway::Tw::Stk::TMarket  mkt = GetProcMkt();
   std::string  mktS = mkt.as_string();
   std::string  appiaKey, tandKey;
   //設定throttle.xml 
   TiXmlDocument  xml;
   xml.LoadFile(path.c_str());
   //<throttle-plugin>
   TiXmlElement*  root = xml.RootElement();
   if(root == NULL) {
      //Alert
      if(Mgr_ = GetExcMgr())
         Mgr_->Alert("XF1014", "Prod", buf);
      WriteMsg(GXFT, buf);
      return false;
   }
   //<throttle-plugin><session> ; 
   TiXmlElement*  session = root->FirstChildElement("session");
   /* 全部刪掉後,重新寫一次 --> 因牽連TWSE與OTC為相同檔案,所以只能處理自己的市場
   for(session; session; session=session->NextSiblingElement("session")) {
       root->RemoveChild(session);
   }
   session = root->FirstChildElement("session");
   */
   TiXmlElement*  ses, *inbound;
   if(session == NULL) { //全部沒有,重寫一遍
      for(TSesIDsMap::iterator iter=map.begin(); iter!=map.end(); ++iter) {
         //20100821:不控管Flow的不設定
         if(iter->second < 0)
            continue;
         ses = new TiXmlElement("session");
         root->LinkEndChild(ses);
         appiaKey = iter->first;
         appiaKey = ConvertTand2Appia(appiaKey); //T5U3U8UAUXU1U
         ses->SetAttribute("name", appiaKey.c_str());
         inbound = new TiXmlElement("inbound");
         ses->LinkEndChild(inbound);
         inbound->SetAttribute("tps", iter->second); 
      }
      xml.SaveFile();
      WriteMsg(GXFT, "1.Finished throttle.xml configuration.");
      return true;
   }
   //先刪除相同市場的設定 session name
   std::string  name;
   for(session; session; session=session->NextSiblingElement("session")) {
      if(session->Attribute("name")) {
         name = session->Attribute("name");
         if(name[0] == mktS[0])
            root->RemoveChild(session);
      }
   }
   //重新加入本市場的設定
   for(TSesIDsMap::iterator iter=map.begin(); iter!=map.end(); ++iter) {
      //20100821:不控管Flow的不設定
      if(iter->second < 0)
         continue;
      ses = new TiXmlElement("session");
      root->LinkEndChild(ses);
      appiaKey = iter->first;
      appiaKey = ConvertTand2Appia(appiaKey); //T5U3U8UAUXU1U
      ses->SetAttribute("name", appiaKey.c_str());
      inbound = new TiXmlElement("inbound");
      ses->LinkEndChild(inbound);
      inbound->SetAttribute("tps", iter->second); 
   }
   xml.SaveFile();
   
   /*
   //先找出有設定過的sessionID,更新inbound
   fprintf(stderr, "orig map=%d\n", map.size());
   std::string  name;
   TSesIDsMap::iterator  iter_ses;
   for(session; session; session=session->NextSiblingElement("session")) {
      if(session->Attribute("name")) {
         name = session->Attribute("name");
         tandKey = ConvertAppia2Tand(name);
         fprintf(stderr, "ses name=%s, tandKey=%s\n", name.c_str(), tandKey.c_str());
         //iter_ses = map.find(name);
         iter_ses = map.find(tandKey);
         if(iter_ses != map.end()) {
            //TiXmlElement*  inbound = session->NextSiblingElement("inbound");
            TiXmlElement*  inbound = session->FirstChildElement("inbound");
            if(inbound == NULL) {
                fprintf(stderr, "inbound == NULL, tps=%d\n", iter_ses->second);
                TiXmlElement*  inbound = new TiXmlElement("inbound");
                session->LinkEndChild(inbound);
                inbound->SetAttribute("tps", iter_ses->second); 
            } else {
                fprintf(stderr, "inbound != NULL, tps=%d\n", iter_ses->second);
                inbound->SetAttribute("tps", iter_ses->second); 
            }
            //map.erase(iter_ses->first);
            //map.erase(name);
            map.erase(tandKey);
            fprintf(stderr, "af-erase map=%d\n", map.size());
         }
      }
   }
   //剩下未設定的,附加在後面
   fprintf(stderr, "left map=%d\n", map.size());
   if(map.size() > 0) {
      for(TSesIDsMap::iterator iter=map.begin(); iter!=map.end(); ++iter) {
         TiXmlElement*  ses = new TiXmlElement("session");
         root->LinkEndChild(ses);
         appiaKey = iter->first;
         appiaKey = ConvertTand2Appia(appiaKey); //T5U3U8UAUXU1U
         ses->SetAttribute("name", appiaKey.c_str());
         //ses->SetAttribute("name", iter->first.c_str());
         TiXmlElement*  inbound = new TiXmlElement("inbound");
         ses->LinkEndChild(inbound);
         inbound->SetAttribute("tps", iter->second); 
      }
   } 
   xml.SaveFile();
   */
   WriteMsg(GXFT, "2.Finished throttle.xml configuration.");
   return true;
}
//---------------------------------------------------------------------------
K_mf(std::string)  TwseFilesHandler::MakeString(std::string& orig, TSesIDsMap& map)
{
   typedef  std::set<std::string>  TStrSet;
   typedef  TStrSet::iterator      TStrSetI;
   
   char  buf[256] = {0};
   WriteMsg(GXFT, "Start to MakeString!");
   Kway::Tw::Stk::TMarket  mkt = GetProcMkt();
   std::string  mktS = mkt.as_string();
   
   TStrSet  ss;
   ss.clear();
   std::string  v = orig;
   std::string::size_type  pos = 0;
   std::string  ses;
   //values=T1160,T1520,T5260,O5920,O9787
   //fprintf(stderr, "v=%s\n", v.c_str());
   std::string  appiaKey;
   //相同市場的原資料不處理,以新資料為準
   for(pos=v.find(","); pos!=std::string::npos; pos=v.find(",")) {
      ses = v.substr(0, pos);
      if(ses[0] != mktS[0]) { //市場不同則保留
         //appiaKey = ConvertTand2Appia(ses); //T5U3U8UAUXU1U, 20120316-W%?, %+3u$#&P.I$#;]-n&h'@ConvertTand2Appia
	 appiaKey = ses;
         ss.insert(appiaKey);
      }
      if(pos+1 == std::string::npos) {
         v.clear();
         break;
      }
      v = v.substr(pos+1, std::string::npos);
   }
   if(v.empty() == false) {
      if(v[0] != mktS[0]) { //市場不同則保留
         //appiaKey = ConvertTand2Appia(v); //T5U3U8UAUXU1U, 20120316-W%?, %+3u$#&P.I$#;]-n&h'@ConvertTand2Appia
	 appiaKey = v;
         ss.insert(appiaKey);
      }
   }
   
   for(TSesIDsMap::iterator iter=map.begin(); iter!=map.end(); ++iter) {
      ses = iter->first;
      appiaKey = ConvertTand2Appia(ses);
      ss.insert(appiaKey);
   }
   sprintf(buf, "Keep Diff Mkt=%d, start to append data", ss.size());
   WriteMsg(GXFT, buf);
   std::string  value;
   for(TStrSetI iter=ss.begin(); iter!=ss.end(); ++iter)
      value += (*iter) + ",";
   if(value.empty() == false)  //[Bug]20100824:避免value是空的
      value.erase(value.length()-1, 1);
   return std::string(value);
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::OnTimer(Kway::TTimerBase* Sender)
{
   StopTimer();

   if(GetAppiaLocked() == false)
      ConfigProduction();
   else {
      WriteMsg(GXFT, "Timer-Appia IsLocked=true!");
      StartTimer(30);
   }
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::OnK10Timer(Kway::TTimerBase* Sender)
{
   StopK10Timer();
   if(GetAppiaLocked()==false && GetK11Done() && GetK13Done())
      GenerateK10();
   else {
      WriteMsg(GXFT, "K10Timer-Appia IsLocked or K11 undo");
      StartK10Timer(30);
   }
}
//---------------------------------------------------------------------------
/*
K_mf(void)  TwseFilesHandler::OnWKTimer(Kway::TTimerBase* Sender)
{
   StopWKTimer();
   TWorkCodeMapI  iter;
   TWorkCodeMapU  u;
   TWorkKey  key;
   TKTime  now = TKTime::Now();
   char buf[128] = {0};
   
   for(int i=1; i<5; ++i) {
      key.assign(i);
      iter = WorkCodeMap_->find(key);
      if(iter == WorkCodeMap_->end())
         continue;
      if(iter->second.CheckTime_.IsInvalid())
         continue;
      if(iter->second.IsReceived_)
         continue;
      if(now < iter->second.CheckTime_)
         continue;
      if(iter->second.IsChecked_==wk_NoNeed || iter->second.IsChecked_==wk_Alerted)
         continue;
      u = WorkCodeMap_->begin_update(iter);
      u->IsChecked_ = wk_Alerted;
      sprintf(buf, "K06-WK-CODE=%i, 超過設定時間=%s,未收到", i, iter->second.CheckTime_.as_string().c_str());
      u->Memo_.assign(buf);
      if(Mgr_ = GetExcMgr())
         Mgr_->Alert("XF1050", buf);
      WriteMsg(GXFT, buf);
   }
   StartWKTimer(30);
}
*/
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::StartK11Timer(size_t sec)
{ 
   K11Timer_.Start(sec*1000, &this_type::OnK11Timer);
   WriteMsg(GXFT, "K11 Timer Start!");
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::StopK11Timer()
{ 
   K11Timer_.Stop();
   WriteMsg(GXFT, "K11 Timer Stop!");
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::OnK11Timer(Kway::TTimerBase* Sender)
{
   StopK11Timer();

   if(GetAppiaLocked() == false) {
      ConfigK11();
   } else {
      WriteMsg(GXFT, "twse.ini IsLocked=true!");
      StartK11Timer(30);
   }
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::StartK13Timer(size_t sec)
{ 
   K13Timer_.Start(sec*1000, &this_type::OnK13Timer);
   WriteMsg(GXFT, "K13 Timer Start!");
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::StopK13Timer()
{ 
   K13Timer_.Stop();
   WriteMsg(GXFT, "K13 Timer Stop!");
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::OnK13Timer(Kway::TTimerBase* Sender)
{
   StopK13Timer();

   if(GetAppiaLocked() == false) {
      ConfigK13();
   } else {
      WriteMsg(GXFT, "twse.ini IsLocked=true!");
      StartK13Timer(30);
   }
}
//---------------------------------------------------------------------------
/*
K_mf(void)  TwseFilesHandler::CreateWKMap()
{
   TWorkCodeMapI  iter;
   TWorkCodeMapU  u;
   TWorkKey  key;
   if(WorkCodeMap_->empty()) {
      for(int i=1; i<5; ++i) {
         key.assign(i);
         iter = WorkCodeMap_->fetch(key);
         u    = WorkCodeMap_->begin_update(iter);
         *u   = SWorkCode(); 
      }
   } else {
      for(int i=1; i<5; ++i) {
         key.assign(i);
         iter = WorkCodeMap_->fetch(key);
         u    = WorkCodeMap_->begin_update(iter);
         u->IsReceived_ = false;
         if(u->CheckTime_.IsInvalid())
            u->IsChecked_ = wk_NoNeed;
         else
            u->IsChecked_ = wk_NotYet;
         u->  Memo_.clear();
      }
   }
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::SetWKReceived(int i)
{
   TWorkKey  key(i);
   TWorkCodeMapI  iter = WorkCodeMap_->find(key);
   if(iter == WorkCodeMap_->end())
      return;
   
   TWorkCodeMapU  u = WorkCodeMap_->begin_update(iter);
   u->IsReceived_ = true;
   u->IsChecked_  = wk_Arrived; 
   std::string  msg = TKTime::Now().as_string() + ":" + "收到K06-WK-CODE";
   u->Memo_.assign(msg);
}
*/
//---------------------------------------------------------------------------
#include "FTJobDef.h"
#include "FTCenter.h"
#include "bxSesMgr.h"
#include "FTJobMap2d.h"
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::GenerateK10()
{
   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert("XF1000", "K10", "K10檔案上傳通知");
   }
   WriteMsg(GXFT, "Start to upload K10.");
   char  buf[256] = {0};
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gTmpPath.c_str());
   gAppiaPath = GetAppiaPath() + GAppiaIni;
   TKIniFile*   appiaIni= new TKIniFile(gAppiaPath.c_str());
   sprintf(buf, "Paht, T=%s, A=%s", iniFile->GetFilePath().c_str(), appiaIni->GetFilePath().c_str());
   WriteMsg(GXFT, buf);
   //再設定一次流量控管檔案ef-config.xml以及throttle.xml,避免K11變更
   bool  boolV = ConfigThrottle(*iniFile, *appiaIni);
   iniFile->Flush();
   appiaIni->Flush();
   
   if(boolV == false) {
      if(Mgr_ = GetExcMgr()) {
         Mgr_->Alert("XF1099", "正式環境", "流量控管設定失敗(ef-config.xml/throttle.xml)");
      }
      WriteMsg(GXFT, "PROD-流量控管設定失敗(ef-config.xml/throttle.xml)");
   }
   std::string  Brk = iniFile->ReadString(GXFT, GBroker, "000K");
   
   SK03Data1  header;
   header.Date_.assign(Kway::TKDate::Now().as_string(TKDate::sf_ELong, 0));
  
   std::vector<SK03Data2>  vK03Data2;
   vK03Data2.clear();

   TStrings     Brks, Sess, Tmps;
   std::string  BrkMkt, market, tmpH, fixSID, tmpStr;
   SK03Data2    data;
   int  BrkCnt = iniFile->ReadStrings(GMain, GBroker, Brks);
   Kway::Tw::Stk::TMarket  procMkt = GetProcMkt();
   bool  isTBrkExist;
   market = procMkt.as_string();
   if(BrkCnt > 0) { //[Bug]20100824: 避免無資料進for出現未預期的錯誤
      for(int L = 0; L < BrkCnt; ++L) {
         data.Clear();
         BrkMkt = market + Brks[L]; //T1400
         //先建立成回K03
         isTBrkExist = iniFile->ReadString(BrkMkt, GMatSesID, "")!=std::string("");
         data.WorkCode_.assign(" "); //要求固定填空白
         data.Type_.assign("K");
         data.PVCName_.assign(iniFile->ReadString(BrkMkt, GTermID, ""));
         data.Broker_.assign(Brks[L]);
         data.Market_.assign(market);
         data.TmpPVCID_.assign(iniFile->ReadString(BrkMkt, GMatSesID, ""));
         data.APCode_.assign("3");
         data.EffDate_.assign(iniFile->ReadString(BrkMkt, GApplyDate, ""));
         data.LineNo_.assign(iniFile->ReadString(BrkMkt, GLineNo, ""));
         data.Passwd_.assign(iniFile->ReadString(BrkMkt, GMatPwd, ""));
         data.SrcPort_.assign(iniFile->ReadString(BrkMkt, GMatSrcPort, ""));
         tmpStr = iniFile->ReadString(BrkMkt, GIP, "");
         data.IP_.assign(FillIPString(tmpStr));
         if(isTBrkExist)
            vK03Data2.push_back(data);
         int  SesCnt = iniFile->ReadStrings(BrkMkt, GFixSes, Sess);
         if(SesCnt <= 0) //[Bug]20100824: 避免無資料進for出現未預期的錯誤
            continue;
         for(int i=0; i<SesCnt; ++i) { //Sess[i] --> T1400X1
            int  tmpCnt = iniFile->ReadStrings(Sess[i], GTMPID, Tmps);
            fixSID = Sess[i].substr(5, 2);
            if(tmpCnt > 0) { //[Bug]20100824: 避免無資料進for出現未預期的錯誤
               for(int t=0; t<tmpCnt; ++t) {
                  tmpH = Sess[i] + "-" + Tmps[t]; //T1400X1-0A
                  //建立委託K03
                  data.Type_.assign("K");
                  data.PVCName_.assign(iniFile->ReadString(tmpH, GTermID, ""));
                  //data.Broker_.assign()
                  //data.Market_ 
                  data.FixPVCID_.assign(fixSID);
                  data.TmpPVCID_.assign(Tmps[t]);
                  data.APCode_.assign("0");
                  data.EffDate_.assign(iniFile->ReadString(tmpH, GApplyDate, ""));
                  data.LineNo_.assign(iniFile->ReadString(tmpH, GLineNo, ""));
                  data.Passwd_.assign(iniFile->ReadString(tmpH, GPwd, ""));
                  data.SrcPort_.assign(iniFile->ReadString(tmpH, GSPort, ""));
                  tmpStr = iniFile->ReadString(tmpH, GIP, "");
                  data.IP_.assign(FillIPString(tmpStr));
                  //data.IP_.assign(iniFile->ReadString(tmpH, GIP, ""));
                  data.FixVersion_.assign(" "); //避免有殘留值
                  vK03Data2.push_back(data);
               }
            }
            //建立FIX K03
            data.Type_.assign("F");
            data.PVCName_.assign(iniFile->ReadString(Sess[i], GTermID, ""));
            data.FixPVCID_.assign(fixSID);
            data.TmpPVCID_.assign(fixSID);
            data.APCode_.assign("0");
            data.EffDate_.assign(iniFile->ReadString(Sess[i], GApplyDate, ""));
            data.LineNo_.assign(iniFile->ReadString(Sess[i], GLineNo, ""));
            data.Passwd_.assign(iniFile->ReadString(Sess[i], GPwd, ""));
            std::string  appiaSID = ConvertTand2Appia(Sess[i]);
            tmpStr = appiaIni->ReadString(appiaSID, GNetAddress, "");
            data.IP_.assign(FillIPString(tmpStr));
            data.SrcPort_.assign(appiaIni->ReadString(appiaSID, GRemotePort, "")); //[Add]20110523:改為AppiaIni裡取得
            tmpStr = appiaIni->ReadString(appiaSID, GConFixVer, "");
            data.FixVersion_.assign(GetTwseFixVerCode(tmpStr));
            vK03Data2.push_back(data);
         }
      }
   }
   //寫入K10
   std::string  k03;
   std::string  fn = GetProcLogPath();//GetLogPath();
   fn += "K10." + market;   // $home/Twse/Logs/yymmdd/XFT/K10.T (O)
   TFile*  file = new TFile(fn.c_str(), TFileMode(fmReadWrite|fmCreatePath|fmTrunc));
   std::string  content = header.ToString();
   file->Seek(0, fsBegin);
   file->Write(content.c_str()); //Write header

   if(vK03Data2.empty() == false) {
      std::vector<SK03Data2>::iterator  iter = vK03Data2.begin();
      for(; iter!=vK03Data2.end(); ++iter) {
         if(file->IsOpened() == false)
            file->Open(fn.c_str(), TFileMode(fmReadWrite|fmCreatePath|fmTrunc));
         k03 = (*iter).ToString(); 
         file->Write(k03.c_str());
      }
   }
   delete iniFile;
   delete appiaIni;

   //上傳K10
   using namespace Kway::Tw::Bx::FT;
   TFTNewJob  job;
   job.SeqNo_.assign0();
   job.SourceID_.assign(Brk);
   job.DestID_ = TSEID_;
   job.Market_ = procMkt;
   job.FileCode_.assign("K10");
   job.Type_ = ftt_File;
   job.Mode_ = ftm_Report;
   job.SrcData_.assign(std::string("K10.")+market);
   job.RecLen_ = file->GetFileSize();
   job.CreateTime_ = Kway::TKTime::Now(); 
/*
   TFTCenter&  ftc = const_cast<TFTCenter&>(GetCenter());
   Kway::Tw::Bx::FT::TSysID  id = ftc.DoReq(job);
   Kway::Tw::Bx::GetBxMgr()->DoSend(job.SourceID_,job.Market_, id);
*/
   FTSender_->FileUpload(job);
   delete file;   
   
   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert("XF1100", "K10", "K10檔案上傳完成");
   }
   WriteMsg(GXFT, "End of K10 upload.");
}
//---------------------------------------------------------------------------
K_mf(std::string)  TwseFilesHandler::GetTradeAccount(std::string& acc, std::string& brk)
{
   using namespace Kway::Tw::Stk;
   std::string  tmpS = acc;
   tmpS += "0";
   TIvacNo  ivacNo(tmpS);
   TBrokerID  brokerID(brk);
   ivacNo.ValidChkCode(brokerID);
   tmpS = ivacNo.as_string(0, true);
   return std::string(tmpS);
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::WriteDefaultAppia(TKIniFile& appia, std::string& sessionid, bool isprod)
{
   /*
   appia.WriteString(sessionID, GConFixVer, GFIX42);
   appia.WriteString(sessionID, GHeartbeatInt, "10");
   appia.WriteString(sessionID, GStartSeqNum, "1");
   appia.WriteString(sessionID, GTradingSes, GTradingSesB);
   */
   std::string  fn = GetConfigPath();
   fn += "AppiaParam.ini";
   TKIniFile*  ini = new TKIniFile(fn.c_str());
   Kway::TKIniFile::TSections*  secs= ini->GetSections();
   Kway::TKIniFile::TSection*   section; 
   std::string  envstr;
   
   if(isprod)
      envstr = std::string("Production");
   else
      envstr = std::string("Test");
      
   for(Kway::TKIniFile::TSectionsI iter=secs->begin(); iter!=secs->end(); ++iter) {
      if(iter->first == envstr) {
      	section = &(iter->second);
         for(Kway::TKIniFile::TSectionI iterSection=section->begin(); iterSection!=section->end(); ++iterSection) {
            for(int L=0; L<iterSection->second.size(); ++L) {
               if(iterSection->first == GConFixVer) //過濾版本訊息,避免設定檔未移除
                  continue;
               appia.WriteString(sessionid, iterSection->first.c_str(), iterSection->second[L].c_str());
            }
         }
      }
   }
   delete ini;
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::WriteMsg(const char* ev, const char* msg)
{
   std::string  tmp = std::string("[") + std::string(ev) + std::string("] ") + std::string(msg);
   Logger_->Log(tmp);
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::SetFTSender(TFTSenderBase* fts)
{
	FTSender_ = fts;
}
//---------------------------------------------------------------------------
