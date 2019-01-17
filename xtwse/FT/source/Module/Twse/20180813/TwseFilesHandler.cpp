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
static std::string  gConfigPath, gTmpPath, gAppiaPath, gForwarderPatn, gPath, gXTesterPath;
//---------------------------------------------------------------------------
TKIniFile* GetIniFile(TKIniFileMap* iniMap, std::string& key, std::string filename)
{   
   TKIniFileMap::iterator iter = iniMap->find(key);
   if(iter == iniMap->end()){
      filename = filename+"."+key;
      TKIniFile* ini = new TKIniFile(filename.c_str());
      iter = iniMap->fetch(key);
      TKIniFileMap::updater upd = iniMap->begin_update(iter);
      *upd = ini;
      return ini;
   }
   return iter->second;
}
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
K_mf(std::string)  GetAppiaHomePath()
{
   TKIniFile*   iniFile  = new TKIniFile(std::string(GetConfPath()+GMainIni).c_str());
   std::string  appiaHomePath = iniFile->ReadString(GMain, GAppiaHomePath, "./");
   delete iniFile;
   
   if(appiaHomePath[appiaHomePath.length()-1]!='/' && appiaHomePath[appiaHomePath.length()-1]!='\\')
      appiaHomePath += "/";
   return appiaHomePath;
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
   //因應tese.ini改成多群組輸出,將GIsLocked改寫到main.ini
   //gAppiaPath = GetAppiaPath() + GAppiaIni;
   //TKIniFile*   iniFile  = new TKIniFile(gAppiaPath.c_str());
   //bool  isLocked = iniFile->ReadBool(GKway, GIsLocked, false);
   //delete iniFile;
   //return bool(isLocked);

   gAppiaPath = GetAppiaPath() + GAppiaIni;
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*   iniFile  = new TKIniFile(gTmpPath.c_str());
   bool  isLocked = iniFile->ReadBool(GAppia, GIsLocked, false);
   delete iniFile;
   return bool(isLocked);   
}
//---------------------------------------------------------------------------
K_mf(bool)  GetF11Done()
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
      tmp = iniFile->ReadString(GXFT, GTF11Done, "");
   } else if(mkt == otc) {
      tmp = iniFile->ReadString(GXFT, GOF11Done, "");
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
/*
K_mf(bool)  GetF13Done()
{
   // 20130305交易所取消F13傳輸,改到GenerateF10內使用F05的資料來更新Fix版本
   return true;
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
      tmp = iniFile->ReadString(GXFT, GTF13Done, "");
   } else if(mkt == otc) {
      tmp = iniFile->ReadString(GXFT, GOF13Done, "");
   }
   
   delete iniFile;
   
   if(tmp.empty()) {
      return false;
   } else {
      date.assign(tmp);
      return(date == today);
   }
}
*/
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
K_ctor  TwseFilesHandler::TwseFilesHandler() 
   : Timer_(this)
   //, WKTimer_(this)
   , F10Timer_(this)
   , F11Timer_(this)
   //, F13Timer_(this)
{
   Mgr_ = NULL;
   //std::string  tmpPath = GetTmpPath();
   //TKIniFile*  iniFile = new TKIniFile(std::string(tmpPath+GMainIni).c_str());
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*  iniFile = new TKIniFile(gTmpPath.c_str());
   IsTF05Ready_ = iniFile->ReadString(GXFT, GTF05Ready, "");
   IsOF05Ready_ = iniFile->ReadString(GXFT, GOF05Ready, "");
   //IsTF05Ready_ = iniFile->ReadString(GMain, GTF05Ready, "");
   //IsOF05Ready_ = iniFile->ReadString(GMain, GOF05Ready, "");
   IsTF03Ready_ = iniFile->ReadString(GXFT, GTF03Ready, "");
   IsOF03Ready_ = iniFile->ReadString(GXFT, GOF03Ready, "");
   IsTF11Ready_ = iniFile->ReadString(GXFT, GTF11Ready, "");
   IsOF11Ready_ = iniFile->ReadString(GXFT, GOF11Ready, "");
   IsTF11Done_  = iniFile->ReadString(GXFT, GTF11Done, "");
   IsOF11Done_  = iniFile->ReadString(GXFT, GOF11Done, "");
/*
   IsTF13Ready_ = iniFile->ReadString(GXFT, GTF13Ready, "");
   IsOF13Ready_ = iniFile->ReadString(GXFT, GOF13Ready, "");
   IsTF13Done_  = iniFile->ReadString(GXFT, GTF13Done, "");
   IsOF13Done_  = iniFile->ReadString(GXFT, GOF13Done, "");
*/   
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
   fn = logPath + "F01Map.bin"; // $home/Twse/Logs/yymmdd/F01Map.bin
   F01Map_      = new TF01Map();
   F01Recorder_ = new TF01Recorder(*F01Map_, fn.c_str());
   
   fn = logPath + "F11Map.bin"; // $home/Twse/Logs/yymmdd/F11Map.bin
   F11Map_      = new TF11Map();
   F11Recorder_ = new TF11Recorder(*F11Map_, fn.c_str());
   //fprintf(stderr, "F11Map_=%d\n", F11Map_->size());
/*   
   fn = logPath + "F13Map.bin"; // $home/Twse/Logs/yymmdd/F13Map.bin
   F13Map_      = new TF13Map();
   F13Recorder_ = new TF13Recorder(*F13Map_, fn.c_str());
   //fprintf(stderr, "F13Map_=%d\n", F13Map_->size());
*/
   fn = confPath + "F03.bin"; // $home/Twse/Settings/F03.bin
   F03Map_ = new TF03Map();
   F03Recorder_ = new TF03Recorder(*F03Map_, fn.c_str());
   CheckF03Map(); //移除過期的F03Map
   
   //fprintf(stderr, "F03Map size=%d\n", F03Map_->size());

   F05Map_ = new TF05Map();
   
   TKDate  dateT, dateO;
   TKDate  today = TKDate::Now();
   if(!IsTF05Ready_.empty())
      dateT.assign(IsTF05Ready_);
   if(!IsOF05Ready_.empty())
      dateO.assign(IsOF05Ready_);
   if(dateT==today || dateO==today) { //避免程式crash後,F05Map_消失
      RebuildF05Map(*iniFile);
   }
   //確認F11是否已經處理過
   Kway::Tw::Stk::TMarket  procMkt = GetProcMkt();
   TMarket  twse, otc(m_Otc);
   if(F11Map_->size() > 0) {
      //Kway::Tw::Stk::TMarket  procMkt = GetProcMkt();
      //TMarket  twse, otc(m_Otc);
      if(procMkt == twse) {
         if(!IsTF11Done_.empty()) {
            dateT.assign(IsTF11Done_);
            if(dateT != today)
               StartF11Timer(30);
         }    
      } else if(procMkt == otc) {
         if(!IsOF11Done_.empty()) {
            dateO.assign(IsOF11Done_);
            if(dateO != today)
               StartF11Timer(30);
         }
      }
   }
/*   
   //確認F13是否已經處理過
   if(F13Map_->size() > 0) {
      //Kway::Tw::Stk::TMarket  procMkt = GetProcMkt();
      //TMarket  twse, otc(m_Otc);
      if(procMkt == twse) {
         if(!IsTF13Done_.empty()) {
            dateT.assign(IsTF13Done_);
            if(dateT != today)
               StartF13Timer(30);
         }    
      } else if(procMkt == otc) {
         if(!IsOF13Done_.empty()) {
            dateO.assign(IsOF13Done_);
            if(dateO != today)
               StartF13Timer(30);
         }
      }
   }
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
   if(F01Recorder_) {
      delete F01Recorder_;
      F01Recorder_ = NULL;
   }
   if(F01Map_) {
      delete F01Map_;
      F01Map_ = NULL;
   }
   if(F03Recorder_) {
      delete F03Recorder_;
      F03Recorder_ = NULL;
   }
   if(F03Map_) {
      delete F03Map_;
      F03Map_ = NULL;
   }
   if(F05Map_) {
      delete F05Map_;
      F05Map_ = NULL;
   }
   if(F11Recorder_) {
      delete F11Recorder_;
      F11Recorder_ = NULL;
   }
   if(F11Map_) {
      delete F11Map_;
      F11Map_ = NULL;
   }
/*   
   if(F13Recorder_) {
      delete F13Recorder_;
      F13Recorder_ = NULL;
   }
   if(F13Map_) {
      delete F13Map_;
      F13Map_ = NULL;
   }
*/   
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
   
// --> 雅萍要求，非F01, F03, F04, F05, F06, F07, F08, F11, F14, F15, F16, F17 不處理, 請以 XF1005 非預定處理之檔案回覆
// 20130502 add by k288
   if(type == ft_None) {
      if(Mgr_ = GetExcMgr())
         Mgr_->Alert("XF1005", fname, "");
      WriteMsg(fname.c_str(), "非預定處理之檔案.");
      delete file;
      return;
   }
// <-- 雅萍要求，非F01, F03, F04, F05, F06, F07, F08, F11, F14, F15, F16, F17 不處理, 請以 XF1005 非預定處理之檔案回覆
      
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
      if(type!=ft_F03 && type!=ft_F05 && type!=ft_F11) {// && type!=ft_F12 && type!=ft_F13) { //F03, F05, F11, F12, F13可能是空的, 20100924 eric 還有 F12
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
   case ft_F01:   ProcessF01(mkt, type, v);   break;
   //case ft_K02:   break;
   case ft_F03:   ProcessF03(mkt, type, v, path);   break;
   case ft_F04:   ProcessF04(mkt, type, v);   break;
   case ft_F05:   ProcessF05(mkt, type, v, path, isRebuild);   break;
   case ft_F06:   ProcessF06(mkt, type, v);   break;
   case ft_F07:   ProcessF07(mkt, type, v);   break;
   case ft_F08:   ProcessF08(mkt, type, v);   break;
   //case ft_K09:   ProcessK09(mkt, type, v);   break;
   case ft_F11:   ProcessF11(mkt, type, v);   break;
   // 20130305交易所取消F12傳輸,改由F05的資料來產生流量檔
   //case ft_F12:   ProcessF12(mkt, type, v);   break;
   // 20130305交易所取消F13傳輸,改到GenerateF10內使用F05的資料來更新Fix版本
   //case ft_F13:   ProcessF13(mkt, type, v);   break;
   //case ft_F12:   break;
   //case ft_F13:   break;
   case ft_F14:   break;
   case ft_F15:   break;
   case ft_F16:   break;
   case ft_F17:   break;
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
   return ((type == ft_F04)? (v.size()>0) : (v.size()>1));
   //return ((type==ft_K09 || type == ft_F04)? (v.size()>0) : (v.size()>1));
   //return v.size() > 1;
   //return true;
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessF01(TMarket& mkt, TFileType& type, TStringV& v)
{
   F01Map_->clear(); //[Add]20110523: 重覆傳輸F01,避免先前資料殘留
   int  i = 0;
   SF01Data2  data;     
   std::string  applyDate;
   
   char  buf[256] = {0};

   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gTmpPath.c_str());
   for(TStringVI  iter=v.begin(); iter!=v.end(); ++iter) {
      if(i == 0) { //Data1不處理         
         ++i;
         applyDate = (*iter).substr(0,8);
         TKDate  date;
         date.assign(applyDate);
         TKDate  today = TKDate::Now();
         if(date != today) {
            if(Mgr_ = GetExcMgr())
               Mgr_->Alert("XF1007", "F01", "F01檔案日期錯誤");
            sprintf(buf, "F01檔案日期錯誤-%s", date.as_string().c_str());
            WriteMsg("F01", buf);
            delete iniFile;
            return;
         }
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      
      if(ChkNInsertData(data, applyDate, *iniFile) == false) {
         if(Mgr_ = GetExcMgr()) //檔案重要欄位資料有誤
            Mgr_->Alert("XF1006", "F01", "");
      }
   } 
   delete iniFile;
   ConfigTester();
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessF04(TMarket& mkt, TFileType& type, TStringV& v)
{
   TStringVI  iter=v.begin(); 
   if(iter == v.end()) {
      return;
   }
   std::string  applyDate, nextDate;
   TKDate  today = TKDate::Now();
   
   applyDate = (*iter).substr(0,8);
   TKDate  date;
   date.assign(applyDate);
   if(date != today) {
      if(Mgr_ = GetExcMgr())
         Mgr_->Alert("XF1007", "F04", "F04檔案日期錯誤");
      WriteMsg("F04", "F04檔案日期錯誤");
      return;
   }
   nextDate = (*iter).substr(8, 8);
         
   std::string  confPath = GetConfPath();
   TKIniFile*  iniFile  = new TKIniFile(std::string(confPath + GMainIni).c_str());

   char  buf[256] = {0};
   sprintf(buf, "Start to Configure F04, T=%s", iniFile->GetFilePath().c_str());
   WriteMsg("F04", buf);
   
   //寫入nextDate to Main.ini
   iniFile->WriteString(GXFT, GNextDate, nextDate);
   delete  iniFile;
/*20130321因應交易所要求, 取消寫入NextDate檔   
   //寫入nextDate to $home/Twse/Settings/NextDate, 供控制啟動系統script使用
   std::string  name = GetConfigPath() + "NextDate";
   TFile*  file = new TFile(name.c_str(), TFileMode(fmReadWrite|fmCreatePath));
   if(file->IsOpened() == false)
      file->Open(name.c_str(), TFileMode(fmReadWrite|fmCreatePath));
   file->Seek(0, fsBegin);
   file->Write(nextDate.c_str());
   delete file;
*/   
/* 移到F05處理
   //寫入nextDate to twse.ini(appia)
   std::string  appia = GetAppiaPath() + GAppiaIni;
   TKIniFile*  appiaIni = new TKIniFile(appia.c_str());
   appiaIni->WriteString("KWAY", "Trading_date", nextDate);
   delete  appiaIni;
*/   

   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert("XF1100", "F04", "F04檔案處理完成");
   }
   WriteMsg("F04", "F04檔案處理完成");
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessF03(TMarket& mkt, TFileType& type, TStringV& v, std::string& fullname)
{
   CheckF03Today();  //重新傳送F03時,先刪除當天送過的F03Map資訊
   //F03收到比對是否有F05,再比對F05是否有存在,不存在則先保留,若存在則找F04的next date是否與生效日同,相同則設定
   dword  i = 0;
   SF03Data2  data;  
   std::string  applyDate;
   TKDate  today = TKDate::Now();
   char  buf[256] = {0};
   
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
               Mgr_->Alert("XF1007", "F03", "F03檔案日期錯誤");
            sprintf(buf, "F03檔案日期錯誤-%s", date.as_string().c_str());
            WriteMsg("F03", buf);
            delete iniFile;
            return;
         }
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      if(ChkNInsertData(data, today, i, *iniFile) == false) {
         if(Mgr_ = GetExcMgr()) //檔案重要欄位資料有誤
            Mgr_->Alert("XF1006", "F03", "");
      }
      ++i;
   }

   //讀取Main.ini,取得路徑()
   //gTmpPath = GetTmpPath() + GMainIni;
   //TKIniFile*   iniFile = new TKIniFile(gTmpPath.c_str());
   if(mkt == m_Tse)
      IsTF03Ready_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   else
      IsOF03Ready_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   
   iniFile->WriteString(GXFT, mkt==m_Tse?GTF03Ready:GOF03Ready, mkt==m_Tse?IsTF03Ready_:IsOF03Ready_);
   delete iniFile;
   
   if(CheckF03orF05(mkt, TFileType(ft_F05))) {
      if(GetAppiaLocked() == false) {
         ConfigProduction();
      } else {
         WriteMsg("F03", "Appia IsLocked=true!");
         StartTimer(30);
      }
   } else {
      if(Mgr_ = GetExcMgr()) //F05檔案不存在
         Mgr_->Alert("XF1008", "F03", "");
      WriteMsg("F03", "F05檔案不存在");
   }
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessF05(TMarket& mkt, TFileType& type, TStringV& v, std::string& fullname, bool isRebuild)
{
   int  i = 0;
   SF05Data2  data;
   std::string  applyDate;
   TKDate  today = TKDate::Now();
   char  buf[256] = {0};
   
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
               Mgr_->Alert("XF1007", "F05", "F05檔案日期錯誤");
            sprintf(buf, "F05檔案日期錯誤-%s", date.as_string().c_str());
            WriteMsg("F05", buf);
            return;
         }
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      if(ChkNInsertData(data, mkt, *iniFile) == false) {
         if(Mgr_ = GetExcMgr()) //檔案重要欄位資料有誤
         Mgr_->Alert("XF1006", "F05", "");
      }
   } 
   delete iniFile;
      
   if(isRebuild)
      return;
   
   //讀取Main.ini,取得路徑()
   gTmpPath = GetTmpPath() + GMainIni;
   //TKIniFile*   iniFile = new TKIniFile(gTmpPath.c_str());
   iniFile = new TKIniFile(gTmpPath.c_str());
   if(mkt == m_Tse)
      IsTF05Ready_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   else
      IsOF05Ready_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   
   iniFile->WriteString(GXFT, mkt==m_Tse?GTF05Ready:GOF05Ready, mkt==m_Tse?IsTF05Ready_:IsOF05Ready_);
   iniFile->WriteString(GXFT, mkt==m_Tse?GTF05File:GOF05File, fullname);
/*   
   // 更新SESSIONID-APPIAGROUP 及 Appia-FWGroup的對應
   iniFile->EraseSection(GSIDAG);
//   iniFile->EraseSection(GAppiaFWGroup);
   SKey4Prod  key;   
   for(TF05Map::iterator iter_f05=F05Map_->begin(); iter_f05!=F05Map_->end(); ++iter_f05) {
      key  = iter_f05->first;
      data = iter_f05->second;
      iniFile->WriteStrings(GSIDAG, data.AppiaGroup_.as_string(), key.GetSesID());
//      iniFile->WriteString(GAppiaFWGroup, data.AppiaGroup_.as_string(), data.FWGroup_.as_string());
   }
*/   
   delete iniFile;
      
   if(CheckF03orF05(mkt, TFileType(ft_F03))) {
      if(GetAppiaLocked() == false)
         ConfigProduction();
      else {
         WriteMsg("F05", "Appia IsLocked=true!");
         StartTimer(30);
      }
   } else {
      if(Mgr_ = GetExcMgr()) //F03檔案不存在
         Mgr_->Alert("XF1009", "F05", "");
      WriteMsg("F05", "F03檔案不存在");
   }
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessF06(TMarket& mkt, TFileType& type, TStringV& v)
{
   int  i = 0;
   SF06Data2  data;  

   std::vector<SF06Data2>  vF06Data2;
   vF06Data2.clear();

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
               Mgr_->Alert("XF1007", "F06", "F06檔案日期錯誤");
            WriteMsg("F06", "F06檔案日期錯誤");
            return;
         }
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      vF06Data2.push_back(data);
   } 
   
   //讀取Main.ini,取得路徑
   //std::string  confPath = GetConfigPath();
   //TKIniFile*  iniFile  = new TKIniFile(std::string(confPath+GMainIni).c_str());
   gConfigPath = GetConfigPath() + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gConfigPath.c_str());
   
   std::vector<SF06Data2>::iterator  iter = vF06Data2.begin();
   int wk;
   std::string  fn = GetLogPath() + "WorkCode"; // $home/Twse/Logs/yyyymmdd/WorkCode
   //std::string  fn_mkt = GetAppiaPath() + "WorkCode"+ (mkt==m_Tse?".T":".O");
   std::string  fn_mkt = GetTmpPath() + "WorkCode"+ (mkt==m_Tse?".T":".O");
   std::string  name;
   for(; iter!=vF06Data2.end(); ++iter) {
      char buf[16] = {0};
      wk = iter->WorkCode_.as_int();
      TKTime time = TKTime::Now();
      TKTimeMS ms = TKTimeMS::Now();
      sprintf(buf, "%s%s = %03d", time.as_string(false).c_str(), ms.as_string().c_str(), wk);
      //20100921交易所電規要求拿掉WorkCode 寫入Main.ini
      //iniFile->WriteInt(GMain, GWorkCode, wk);
      //寫入一個byte給shell script使用,分別寫兩個地方 1.$home/Twse/Logs/yyyymmdd/WorkCode  2.$home/Backup/update/WorkCode.T(O)
      for(int i=0; i<2; ++i) {
         name = i==0 ? fn : fn_mkt;
         TFile*  file = new TFile(name.c_str(), TFileMode(fmWrite|fmAppend|fmOpenAlways|fmCreatePath)); //[Bug]20110208: Add fmTrunc to ensure the data will not have any residual number
         if(file->IsOpened() == false)
            file->Open(name.c_str(), TFileMode(fmWrite|fmAppend|fmOpenAlways|fmCreatePath)); //[Bug]20110208: Add fmTrunc to ensure the data will not have any residual number
         file->Seek(file->GetFileSize(), fsBegin);
         file->WriteLN(buf);
         delete file;
/*
         name = i==0 ? fn : fn_mkt;
         TFile*  file = new TFile(name.c_str(), TFileMode(fmReadWrite|fmCreatePath|fmTrunc)); //[Bug]20110208: Add fmTrunc to ensure the data will not have any residual number
         if(file->IsOpened() == false)
            file->Open(name.c_str(), TFileMode(fmReadWrite|fmCreatePath|fmTrunc)); //[Bug]20110208: Add fmTrunc to ensure the data will not have any residual number
         file->Seek(0, fsBegin);
         file->Write(buf);
         delete file;
*/         
      }
      //SetWKReceived(wk);
   }
   delete iniFile;

   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert("XF1100", "F06", "F06檔案處理完成");
   }
   WriteMsg("F06", "F06檔案處理完成");
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessF07(TMarket& mkt, TFileType& type, TStringV& v)
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
      std::string  Tag40_;    //‘1’ Market Price (Reserved) ‘2’ Limit Price
      std::string  Tag59_;
      std::string  Tag44_;
      std::string  Tag10000_;
      std::string  Tag10001_; //0=Odd-lot
      std::string  Tag10002_; //0,3=Regular/Fixed-price ; 2,3=Odd-lot
      // 20121127 交易所修改電文規範,增加 TwseRejStaleOrd：逾時註記。
      // “Y” 檢查TransactTime是否逾時。
      // “N” 不檢查TransactTime。
      // 定價/零股時證交所忽略此欄位
      std::string  Tag10004_; 

      K_ctor SPkt() : MsgType_("D"), Tag21_("1"), Tag57_("0"), Tag38_("1"), Tag40_("2"), Tag59_("0"), Tag10000_("1"), Tag10001_("0"), Tag10002_("0"), Tag10004_("N") {}
      K_mf(std::string)  ToString() { 
         char buf = 0x01;
         char newline = 0x0A;
         return std::string("35="+MsgType_+buf+"49="+Tag49_+buf+"56="+Tag56_+buf+"50="+BrokerID_+buf+"57="+Tag57_+buf+"11="+Tag11_+buf+
                            "1="+Account_+buf+"37="+OrderID_+buf+"21="+Tag21_+buf+"55="+Tag55_+buf+"54="+Tag54_+buf+"60="+Tag60_+buf+
                            "38="+Tag38_+buf+"40="+Tag40_+buf+"59="+Tag59_+buf+"44="+Tag44_+buf+"10000="+Tag10000_+buf+"10001="+Tag10001_+
                            buf+"10002="+Tag10002_+buf+"10004="+Tag10004_+buf+newline); 
      }
   };
   
   static  int tag11 = 1;
   int  i = 0;
   SF07Data2  data;  

   std::vector<SF07Data2>  vF07Data2;
   vF07Data2.clear();
   
   for(TStringVI  iter=v.begin(); iter!=v.end(); ++iter) {
      if(i == 0) { //Data1不處理
         ++i;
         TKDate  today = TKDate::Now();
         std::string  applyDate = (*iter).substr(0,8);
         TKDate  date;
         date.assign(applyDate);
         if(date != today) {
            if(Mgr_ = GetExcMgr())
               Mgr_->Alert("XF1007", "F07", "F07檔案日期錯誤");
            WriteMsg("F07", "F07檔案日期錯誤");
            return;
         }
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      vF07Data2.push_back(data);
   }
   
   gConfigPath = GetConfigPath() + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gConfigPath.c_str());

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
         std::string  f07;
         char  buf[32] = {0};
         std::string  fn = GetLogPath() + "F07" + "." + (*iterSes); //$Home/Twse/Logs/yyyymmdd/F07.T1110X1
         TFile*  file = new TFile(fn.c_str(), TFileMode(fmReadWrite|fmCreatePath|fmAppendBit));
         file->Seek(0, fsBegin);
         //pkt.Tag49_ = (mkt==m_Tse ? "T" : "O") + brk + "X1";
         pkt.Tag49_ = *iterSes;
         pkt.Tag56_ = mkt==m_Tse ? "XTAI" : "ROCO";
         pkt.BrokerID_ = brk;
         pkt.Account_ = GetTradeAccount(brk, brk);  
         for(std::vector<SF07Data2>::iterator iter=vF07Data2.begin(); iter!=vF07Data2.end(); ++iter) {
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
                  // --> 20140716 modify by k288 for 區分新舊版Fix主機, 將tag37第一碼改為2
                  //sprintf(buf, "%c%04d", GetSeq2Alpha(tag11/10000 +1), (tag11%10000));
                  sprintf(buf, "%c%04d", GetSeq2Alpha(tag11/10000 +2), (tag11%10000));
                  // <-- 20140716 modify by k288 for 區分新舊版Fix主機, 將tag37第一碼改為2
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
                     // 20121127 交易所修改電文規範,將定價的Tag40由'Z'改為'2'
                     pkt.Tag40_ = "2";
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
                  f07.clear();
                  f07 = pkt.ToString();
                  file->Write(f07.c_str());
                  //file->Write(newline);
               }
            }  
         }
         delete file;
      }
   }  
   //delete file;

   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert("XF1100", "F07", "F07檔案處理完成");
   }
   WriteMsg("F07", "F07檔案處理完成");
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessF08(TMarket& mkt, TFileType& type, TStringV& v)
{
   int  i = 0;
   SF08Data2  data;  

   std::vector<SF08Data2>  vF08Data2;
   vF08Data2.clear();
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
               Mgr_->Alert("XF1007", "F08", "F08檔案日期錯誤");
            sprintf(buf, "F08檔案日期錯誤-%s", date.as_string().c_str());
            WriteMsg("F08", buf);
            return;
         }
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      vF08Data2.push_back(data);
   } 
   
   //讀取Main.ini,取得路徑
   std::string fwpasswords = GetAppiaPath() + GPassWordsIni;   // 設定Passwords.ini
   TKIniFile* passwordsIni = new TKIniFile(fwpasswords.c_str());
   fwpasswords = GetAppiaPath() + GPWD_Update + ".ini";   // 設定passwords_update.ini
   TKIniFile* pwd_updateIni = new TKIniFile(fwpasswords.c_str());
   pwd_updateIni->EraseSection(GPASSWORDS);
   
   bool flag_update = false;
   std::string  sessionID, pwd, oldPwd, newPwd;
   std::vector<SF08Data2>::iterator  iter = vF08Data2.begin();
   for(; iter!=vF08Data2.end(); ++iter) {
      sessionID = mkt.as_string() + iter->Broker_.as_string() + iter->FixPVCID_.as_string();
      oldPwd    = iter->OldPasswd_.as_string();
      newPwd    = iter->Passwd_.as_string();
      pwd       = passwordsIni->ReadString(GPASSWORDS, sessionID, "");
      
      if(pwd.empty() || pwd!=oldPwd) {
         if(Mgr_ = GetExcMgr()) {
            sprintf(buf, "FIX Session ID =%s, %s", sessionID.c_str(), pwd.empty()?"無此設定之Session ID":"F08-OLD-PASSWORD不正確");
            Mgr_->Alert("XF1015", "F08", buf);
            WriteMsg("F08", buf);
         }
      } else {
         passwordsIni->WriteString(GPASSWORDS, sessionID, newPwd);
         pwd_updateIni->WriteString(GPASSWORDS, sessionID, newPwd);
         flag_update = true;
      }
   }
   delete passwordsIni;
   delete pwd_updateIni;

   if(flag_update == true) {
      char buf[256] = {0};
      std::string name = GetAppiaPath() + GPWD_Update;   // 更新passwords_update, 通知 shell script 已有密碼更新
      TKDateTime datetime = TKDateTime::Now();
      sprintf(buf, "%s", datetime.as_string(TKDate::sf_ELong, 0, false).substr(0, 13).c_str());

      TFile*  file = new TFile(name.c_str(), TFileMode(fmWrite|fmAppend|fmOpenAlways|fmCreatePath)); 
      if(file->IsOpened() == false)
         file->Open(name.c_str(), TFileMode(fmWrite|fmAppend|fmOpenAlways|fmCreatePath)); 
      file->Seek(file->GetFileSize(), fsBegin);
      file->WriteLN(buf);
      delete file;
   }
      
   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert("XF1100", "F08", "F08檔案處理完成");
   }
   WriteMsg("F08", "F08檔案處理完成");
}
//---------------------------------------------------------------------------
/*
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
*/
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessF11(TMarket& mkt, TFileType& type, TStringV& v)
{
   //fprintf(stderr, "ProcessF11-->1\n");
   int  i = 0;
   SF11Data2  data;
   
   typedef  std::vector<SF11Data2>  VecF11Data2;
   typedef  VecF11Data2::iterator   VecF11Data2I;
   VecF11Data2  vF11Data2;
   vF11Data2.clear();
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
               Mgr_->Alert("XF1007", "F11", "F11檔案日期錯誤");
            sprintf(buf, "F11檔案日期錯誤-%s", date.as_string().c_str());
            WriteMsg("F11", buf);
            return;
         }
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      vF11Data2.push_back(data);
   }
   
   TF11MapI  iter_F11;
   TF11MapU  upd_F11;
   for(VecF11Data2I  iter = vF11Data2.begin(); iter!=vF11Data2.end(); ++iter) {
      //iter_F11 = F11Map_->fetch(data.BfBroker_);
      iter_F11 = F11Map_->fetch(iter->BfBroker_);
      upd_F11  = F11Map_->begin_update(iter_F11);
      *upd_F11 = iter->AfBroker_; 
      //*upd_F11 = data.AfBroker_; 
      //fprintf(stderr, "data.BfBroker_=%s, data.AfBroker_=%s", data.BfBroker_.as_string().c_str(), data.AfBroker_.as_string().c_str());
   }
   
   //讀取Main.ini,取得路徑()
   //std::string  tmpPath = GetTmpPath();
   //TKIniFile*   iniFile  = new TKIniFile(std::string(tmpPath+GMainIni).c_str());
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gTmpPath.c_str());
   if(mkt == m_Tse)
      IsTF11Ready_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   else
      IsOF11Ready_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   
   iniFile->WriteString(GXFT, mkt==m_Tse?GTF11Ready:GOF11Ready, mkt==m_Tse?IsTF11Ready_:IsOF11Ready_);
   delete iniFile;

   if(GetAppiaLocked() == false) {
      ConfigF11();
   } else {
      WriteMsg("F11", "Appia IsLocked=true!");
      StartF11Timer(30);
   }
}
//---------------------------------------------------------------------------
/*
K_mf(void)  TwseFilesHandler::ProcessF12(TMarket& mkt, TFileType& type, TStringV& v)
{
   int  i = 0;
   SF12Data2  data;
   std::string  applyDate, tmp, settingStr;
   int  multiplierS;
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
               Mgr_->Alert("XF1007", "F12", "F12檔案日期錯誤");
            sprintf(buf, "F12檔案日期錯誤-%s", date.as_string().c_str());
            WriteMsg("F12", buf);
            return;
         }
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
            Mgr_->Alert("XF1007", "F12", buf);
         WriteMsg("F12", buf);
         return;
      }
      settingStr += tmp + ":"; //分隔
   }
   iniFile->WriteString(GXFT, GMultiplierS, settingStr);
   delete iniFile;

   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert("XF1100", "F12", "F12檔案處理完成");
   }
   WriteMsg("F12", "F12檔案處理完成");
}
*/
//---------------------------------------------------------------------------
/*
K_mf(void)  TwseFilesHandler::ProcessF13(TMarket& mkt, TFileType& type, TStringV& v)
{
   //fprintf(stderr, "ProcessF13-->1\n");
   int  i = 0;
   SF13Data2  data;
   
   typedef  std::vector<SF13Data2>  VecF13Data2;
   typedef  VecF13Data2::iterator   VecF13Data2I;
   VecF13Data2  vF13Data2;
   vF13Data2.clear();
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
               Mgr_->Alert("XF1007", "F13", "F13檔案日期錯誤");
            sprintf(buf, "F13檔案日期錯誤-%s", date.as_string().c_str());
            WriteMsg("F13", buf);
            return;
         }
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      vF13Data2.push_back(data);
   }
   
   TF13MapI  iter_F13;
   TF13MapU  upd_F13;
   SF13Key   key;
   SF13Value value;
   for(VecF13Data2I  iter = vF13Data2.begin(); iter!=vF13Data2.end(); ++iter) {
      key.Broker_.assign(iter->Broker_.as_string().substr(0,4));
      key.FixPVCID_.assign(iter->FixPVCID_.as_string());
      iter_F13 = F13Map_->fetch(key);
      upd_F13  = F13Map_->begin_update(iter_F13);
      value.BfFixVer_.assign(iter->BfFixVer_.as_string());
      value.AfFixVer_.assign(iter->AfFixVer_.as_string());
      *upd_F13 = value; 
   }
   
   //讀取Main.ini,取得路徑()
   //std::string  tmpPath = GetTmpPath();
   //TKIniFile*   iniFile  = new TKIniFile(std::string(tmpPath+GMainIni).c_str());
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gTmpPath.c_str());
   if(mkt == m_Tse)
      IsTF13Ready_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   else
      IsOF13Ready_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   
   iniFile->WriteString(GXFT, mkt==m_Tse?GTF13Ready:GOF13Ready, mkt==m_Tse?IsTF13Ready_:IsOF13Ready_);
   delete iniFile;

   if(GetAppiaLocked() == false) {
      ConfigF13();
   } else {
      WriteMsg("F13", "Appia IsLocked=true!");
      StartF13Timer(30);
   }
}
*/
//---------------------------------------------------------------------------
// --> 20180806 modify by k288 for 測試線路與正式線路發生重覆
K_mf(bool)  TwseFilesHandler::SetF01IniFile(SF01Data2& f01, TKIniFile& main, TKIniFile& xtwse, std::string& applyDate)
{
   char  buf[256] = {0};
   gAppiaPath = GetAppiaPath() + GAppiaIni + "." + f01.AppiaGroup_.as_string();  //設定Appia.ini
   sprintf(buf, "Start to Configure XTester, T=%s, A=%s", xtwse.GetFilePath().c_str(), gAppiaPath.c_str());
   WriteMsg(GXFT, buf);
   
   std::string  broker = f01.Broker_.as_string().substr(0,4);
   std::string  key = f01.Market_.as_string() + broker + f01.FixPVCID_.as_string(); //T5380X1
   //fprintf(stderr, "key=%s-%s-%s\n", f01.Market_.as_string().c_str(), broker, f01.FixPVCID_.as_string().c_str());
   std::string  connS, key1, key2;
   std::string  appiaKey = ConvertTand2Appia(key); //T5U3U8UAUXU1U
   std::string  verSrc, fixVer;
   
   sprintf(buf, "ID=%s", key.c_str());

   std::string mBroker =  f01.Market_.as_string() + broker;
   TStrings Sess;
   bool result = false;
   int sesCnt = main.ReadStrings(mBroker, GFixSes, Sess);
   for(int i = 0; i < sesCnt; i++)
   {
      if(Sess[i] == key)
      {
         result = true;
         break;
      }
   }
   if(result)
   {
      if(Mgr_ = GetExcMgr()) {
         sprintf(buf, "測試線路已存在於正式線路中, ID=%s", key.c_str());
         Mgr_->Alert("XF1050", "F01", buf);
         return true;
      }
   }
   
   switch(f01.WhichType()) 
   {
      case est_None:   
         if(Mgr_ = GetExcMgr()) {
            sprintf(buf, "XTester設定失敗, ID=%s, F01-TYPE錯誤", key.c_str());
            Mgr_->Alert("XF1006", "F01", buf);
            WriteMsg(GXFT, buf);
         }
         return false;
      case est_NewFix:
      {
         connS = "[" + appiaKey + "]"; //[T5U3U8UAUXU1U]
         //取得middleware name
         key2  = (f01.Market_[0]=='T'?xtwse.ReadString(GSummary, "AppiaMWName", "mTester_T"):xtwse.ReadString(GSummary, "AppiaMWName", "mTester_O"));
         key1  = "[" + key2 + "]";
         sprintf(buf, "Set Appia, appK=%s, mw=%s, id=%s", appiaKey.c_str(), key1.c_str(), key.c_str());
         WriteMsg(GXFT, buf);
         verSrc = f01.FixVersion_.as_string();
         fixVer.clear();
         if(GetAppiaFixVerString(verSrc, fixVer) == false) {
            if(Mgr_ = GetExcMgr()) {
               sprintf(buf, "XTester設定Appia失敗, ID=%s,   F01-FIX-VERSION錯誤", key.c_str());
               Mgr_->Alert("XF1006", "F01", buf);
               sprintf(buf, "FIX version error, id=%s,  appK=%s, recvVer=%s", key.c_str(), appiaKey.c_str(), verSrc.c_str());
               WriteMsg(GXFT, buf);
            }
            return false;
         }
         
         TKIniFile*  appiaIni = NULL;
         std::string appiaGroup = xtwse.ReadString(key, GAppiaGroup, "");
         if(appiaGroup.empty() == false && appiaGroup != f01.AppiaGroup_.as_string())
         {
            appiaIni = new TKIniFile(std::string(GetAppiaPath() + GAppiaIni + "." + appiaGroup).c_str());
            if(appiaIni != NULL)
            {               
               appiaIni->EraseValue(GAppiaMain, GAConnection, appiaKey);
               appiaIni->EraseSection(appiaKey);
               appiaIni->EraseValue(key2, GMidSesID, appiaKey); 
               delete appiaIni;
            }
         }
         appiaIni = new TKIniFile(gAppiaPath.c_str());
         
         //新增至[Main]
         appiaIni->WriteStrings(GAppiaMain, GAConnection, connS);
         appiaIni->WriteStrings(GAppiaMain, GMiddleware, key1);
         //新增至[middleware]
         appiaIni->WriteStrings(key2, GMidSesID, appiaKey); 
         //新增至[connection]
         appiaIni->WriteString(appiaKey, GConFixVer, fixVer); //填入FIX版本
         appiaIni->EraseIndent(appiaKey, GNetAddress);
         if(f01.IP1_.empty() == false && f01.IP1_.as_string() != "               ")
            appiaIni->WriteStrings(appiaKey, GNetAddress, f01.IP1_.as_string());
         if(f01.IP2_.empty() == false && f01.IP2_.as_string() != "               ")
            appiaIni->WriteStrings(appiaKey, GNetAddress, f01.IP2_.as_string());
         appiaIni->WriteString(appiaKey, GRemotePort, f01.PVCNo_.as_string()); //[Add]20110523
         appiaIni->WriteString(appiaKey, GBrokerFirmID, key); //[T5U3U8UAUXU1U] remote_firm_id=T5380X1
         //20100714 by eric 測試線路亦由AppiaParam.ini取得共用設定
         WriteDefaultAppia(*appiaIni, appiaKey, false);
         delete appiaIni;
         appiaIni = NULL;
         
         //設定回覆F02需要的額外資訊
         xtwse.WriteStrings(GSummary, GSesID, key);
         xtwse.EraseSection(key);
         xtwse.WriteString(key, GTestNo, f01.TestNo_.as_string());
         xtwse.WriteString(key, GRemotePort, f01.PVCNo_.as_string()); 
         xtwse.WriteString(key, GAPCode, f01.APCode_.as_string());
         xtwse.WriteString(key, GPwd, f01.Passwd_.as_string());
         xtwse.WriteString(key, GType, f01.Type_.as_string());
         xtwse.WriteString(key, GConFixVer, verSrc==std::string("1")?"42":"44"); //for cooper using
         xtwse.WriteString(key, GSourceIP1, f01.IP1_.as_string());
         xtwse.WriteString(key, GSourceIP2, f01.IP2_.as_string());
         xtwse.WriteString(key, GFWGroup, f01.FWGroup_.as_string());
         xtwse.WriteString(key, GAppiaGroup, f01.AppiaGroup_.as_string());
         xtwse.WriteString(key, GIP, f01.DestIP_.as_string());
         xtwse.WriteString(key, GListPort1, f01.ListPort1_.as_string());
         xtwse.WriteString(key, GListPort2, f01.ListPort2_.as_string());
         xtwse.WriteString(key, GSeqNo, f01.ApplySeqNo_.as_string());
         xtwse.WriteString(key, GApplyDate, applyDate);
         if(Mgr_ = GetExcMgr()) {
            Mgr_->Alert("XF1041", "F01", buf);
         }
         break;
      }
   }
   return true;
}
/*K_mf(bool)  TwseFilesHandler::SetF01IniFile(SF01Data2& f01, TKIniFile& main, TKIniFile& xtwse, TKIniFile& appia, std::string& applyDate)
{
   std::string  broker = f01.Broker_.as_string().substr(0,4);
   std::string  key = f01.Market_.as_string() + broker + f01.FixPVCID_.as_string(); //T5380X1
   //fprintf(stderr, "key=%s-%s-%s\n", f01.Market_.as_string().c_str(), broker, f01.FixPVCID_.as_string().c_str());
   std::string  connS, key1, key2;
   std::string  appiaKey = ConvertTand2Appia(key); //T5U3U8UAUXU1U
   std::string  verSrc, fixVer;
   
   char  buf[256] = {0};
   sprintf(buf, "ID=%s", key.c_str());
   
   switch(f01.WhichType()) {
   case est_None:   
      if(Mgr_ = GetExcMgr()) {
         sprintf(buf, "XTester設定失敗, ID=%s, F01-TYPE錯誤", key.c_str());
         Mgr_->Alert("XF1006", "F01", buf);
         WriteMsg(GXFT, buf);
      }
      return false;
   case est_NewFix:
      connS = "[" + appiaKey + "]"; //[T5U3U8UAUXU1U]
      //取得middleware name
      key2  = (f01.Market_[0]=='T'?xtwse.ReadString(GSummary, "AppiaMWName", "mTester_T"):xtwse.ReadString(GSummary, "AppiaMWName", "mTester_O"));
      key1  = "[" + key2 + "]";
      //key1  = "[" + (f01.Market_[0]=='T'?xtwse.ReadString(GSummary, "AppiaMWName", "mTester_T"):xtwse.ReadString(GSummary, "AppiaMWName", "mTester_O")) + "]";//"[mTester]";
      sprintf(buf, "Set Appia, appK=%s, mw=%s, id=%s", appiaKey.c_str(), key1.c_str(), key.c_str());
      WriteMsg(GXFT, buf);
      verSrc = f01.FixVersion_.as_string();
      fixVer.clear();
      if(GetAppiaFixVerString(verSrc, fixVer) == false) {
         if(Mgr_ = GetExcMgr()) {
            sprintf(buf, "XTester設定Appia失敗, ID=%s,   F01-FIX-VERSION錯誤", key.c_str());
            Mgr_->Alert("XF1006", "F01", buf);
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
      appia.EraseIndent(appiaKey, GNetAddress);
      if(f01.IP1_.empty() == false && f01.IP1_.as_string() != "               ")
         appia.WriteStrings(appiaKey, GNetAddress, f01.IP1_.as_string());
      if(f01.IP2_.empty() == false && f01.IP2_.as_string() != "               ")
         appia.WriteStrings(appiaKey, GNetAddress, f01.IP2_.as_string());
      appia.WriteString(appiaKey, GRemotePort, f01.PVCNo_.as_string()); //[Add]20110523
      appia.WriteString(appiaKey, GBrokerFirmID, key); //[T5U3U8UAUXU1U] remote_firm_id=T5380X1
      //20100714 by eric 測試線路亦由AppiaParam.ini取得共用設定
      WriteDefaultAppia(appia, appiaKey, false);
      //設定回覆F02需要的額外資訊
      xtwse.WriteStrings(GSummary, GSesID, key);
      xtwse.EraseSection(key);
      xtwse.WriteString(key, GTestNo, f01.TestNo_.as_string());
      xtwse.WriteString(key, GRemotePort, f01.PVCNo_.as_string()); 
      xtwse.WriteString(key, GAPCode, f01.APCode_.as_string());
      xtwse.WriteString(key, GPwd, f01.Passwd_.as_string());
      xtwse.WriteString(key, GType, f01.Type_.as_string());
      xtwse.WriteString(key, GConFixVer, verSrc==std::string("1")?"42":"44"); //for cooper using
      xtwse.WriteString(key, GSourceIP1, f01.IP1_.as_string());
      xtwse.WriteString(key, GSourceIP2, f01.IP2_.as_string());
      xtwse.WriteString(key, GFWGroup, f01.FWGroup_.as_string());
      xtwse.WriteString(key, GAppiaGroup, f01.AppiaGroup_.as_string());
      xtwse.WriteString(key, GIP, f01.DestIP_.as_string());
      xtwse.WriteString(key, GListPort1, f01.ListPort1_.as_string());
      xtwse.WriteString(key, GListPort2, f01.ListPort2_.as_string());
      xtwse.WriteString(key, GSeqNo, f01.ApplySeqNo_.as_string());
      xtwse.WriteString(key, GApplyDate, applyDate);
      if(Mgr_ = GetExcMgr()) {
         Mgr_->Alert("XF1041", "F01", buf);
      }
      break;
   }
   return true;
}
*/
// <-- 20180806 modify by k288 for 測試線路與正式線路發生重覆
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
   //gAppiaPath   = GetAppiaPath();
   
   if(gXTesterPath[gXTesterPath.length()-1]!='/' && gXTesterPath[gXTesterPath.length()-1]!='\\')
      gXTesterPath += "/";
   gXTesterPath += "XTester.ini"; //設定XTester.ini
   //gAppiaPath += GAppiaIni;  //設定Appia.ini
   
   //TKIniFile*  xtwseIni = new TKIniFile(xtwse.c_str());
   //TKIniFile*  appiaIni = new TKIniFile(appia.c_str());
   TKIniFile*  xtwseIni = new TKIniFile(gXTesterPath.c_str());
   
   std::string  applyDate;
   bool         isOK = true;
   bool         boolV;
   
// --> 20180806 modify by k288 for 測試線路與正式線路發生重覆
   for(TF01Map::iterator iter=F01Map_->begin(); iter!=F01Map_->end(); ++iter) {
      applyDate = iter->second.ApplyDate_.as_string(Kway::TKDate::sf_ELong, 0);
      boolV = SetF01IniFile(iter->second.F01Data2_, *iniFile, *xtwseIni, applyDate);
      if(boolV == false)
         isOK = boolV;
   }
/*
   for(TF01Map::iterator iter=F01Map_->begin(); iter!=F01Map_->end(); ++iter) {
      gAppiaPath = GetAppiaPath() + GAppiaIni + "." + iter->second.F01Data2_.AppiaGroup_.as_string();  //設定Appia.ini
      TKIniFile*  appiaIni = new TKIniFile(gAppiaPath.c_str());
   
      sprintf(buf, "Start to Configure XTester, T=%s, A=%s", xtwseIni->GetFilePath().c_str(), appiaIni->GetFilePath().c_str());
      WriteMsg(GXFT, buf);
      
      applyDate = iter->second.ApplyDate_.as_string(Kway::TKDate::sf_ELong, 0);
      boolV = SetF01IniFile(iter->second.F01Data2_, *iniFile, *xtwseIni, *appiaIni, applyDate);
      if(boolV == false)
         isOK = boolV;
      delete  appiaIni;
   }
*/   
// <-- 20180806 modify by k288 for 測試線路與正式線路發生重覆
   
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
   WriteMsg(GXFT, "Got F03 n F05, Start to Configure Production.");
   //Lock檔案使用權的旗標
   //std::string  appia = GetAppiaPath();
   //appia += GAppiaIni;  //設定Appia.ini
   //TKIniFile*  ini = new TKIniFile(appia.c_str());
   //ini->WriteBool(GKway, GIsLocked, true);
   //delete ini;
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*  ini = new TKIniFile(gTmpPath.c_str());
   ini->WriteBool(GAppia, GIsLocked, true);
   delete ini;
   
   TKIniFile*   iniFile = new TKIniFile(gTmpPath.c_str());
   std::string  appia = GetAppiaPath();
   appia += GAppiaIni;  //設定Appia.ini
   //TKIniFile*  appiaIni = new TKIniFile(appia.c_str());
   
   std::string fwpasswords = GetAppiaPath();
   fwpasswords += GPassWordsIni;   // 設定Passwords.ini
   TKIniFile* passwordsIni = new TKIniFile(fwpasswords.c_str());
   
   char  buf[256] = {0};
   //sprintf(buf, "Access Files P=%s, A=%s", passwordsIni->GetFilePath().c_str(), appiaIni->GetFilePath().c_str());
   //WriteMsg(GXFT, buf);
   
   TKIniFileMap* appiaIniMap = new TKIniFileMap();
   TKIniFile*    appiaIni;
   std::string   appiaGroup;
   
   //SKey4Prod  key;
   SKey4ProdSort  key;
   SF03Data2  value;
   bool  isOK = true;
   bool  boolV;
   //以F03以及F05Map建立Production設定
   for(TF03Map::iterator iter=F03Map_->begin(); iter!=F03Map_->end(); ++iter) {
      key   = iter->first;
      value = iter->second.F03Data2_;
      appiaGroup = value.AppiaGroup_.as_string();
      appiaIni = GetIniFile(appiaIniMap, appiaGroup, appia);
      sprintf(buf, "Access Files P=%s, A=%s", passwordsIni->GetFilePath().c_str(), appiaIni->GetFilePath().c_str());
      WriteMsg(GXFT, buf);
      boolV = ConfigOrSave(key, value, *iniFile, *appiaIni, *passwordsIni);
      if(boolV == false) {
         isOK = boolV;
         WriteMsg(GXFT, "Config F03/F05 Error.");
      }
   }
   //建立流量控管檔案ef-config.xml以及throttle.xml
   //boolV = ConfigThrottle(*iniFile, *appiaIni);  //改由GenerateF10再設定xml
   //delete iniFile;
   //delete appiaIni;
   for(TKIniFileMap::iterator iter = appiaIniMap->begin(); iter != appiaIniMap->end(); iter++){
      delete iter->second;
      iter->second = NULL;
   }
   delete appiaIniMap;
   
// --> 20131031 交易所決定將密碼檔passwords.ini每天統一由F05的資料重新產生
   //建立Forwarder需要的passwords.ini
   ConfigPasswords(*iniFile, *passwordsIni);
// <-- 20131031 交易所決定將密碼檔passwords.ini每天統一由F05的資料重新產生

   //建立Forwarder需要的appia-smp.ini.xxx
   ConfigAppiaSmp(*iniFile);
   delete passwordsIni;
   delete iniFile;
   
   //if(boolV == false)
   //   isOK = boolV;
   //特別要另外重建F01的設定
   boolV = ConfigTester();
   if(boolV == false)
      isOK = boolV;
   //釋放檔案使用權的旗標
   //appiaIni = new TKIniFile(appia.c_str());
   //appiaIni->WriteBool(GKway, GIsLocked, false);
   //delete appiaIni;

   gTmpPath = GetTmpPath() + GMainIni;
   ini = new TKIniFile(gTmpPath.c_str());
   ini->WriteBool(GAppia, GIsLocked, false);
   delete ini;
   
   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert(isOK?"XF1100":"XF1099", "正式環境", "");
      sprintf(buf, "Config Prod %s!", isOK?"OK":"Failed");
      WriteMsg(GXFT, buf);
   }
   if(GetF11Done())// && GetF13Done())
      GenerateF10();
   else
      StartF10Timer(30);
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ConfigF11()
{
   WriteMsg("F11", "Start to Configure F11.");
   char buf[256] = {0};
   Kway::Tw::Stk::TMarket  procMkt = GetProcMkt();
   TMarket  twse, otc(m_Otc);
   std::string  mkt = procMkt.as_string();
   //讀取Main.ini,取得路徑
   //Lock檔案使用權的旗標
   //std::string  appia = GetAppiaPath() + GAppiaIni;  //設定Appia.ini
   //TKIniFile*  ini = new TKIniFile(appia.c_str());
   //ini->WriteBool(GKway, GIsLocked, true);
   //delete ini;

   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*  ini = new TKIniFile(gTmpPath.c_str());
   ini->WriteBool(GAppia, GIsLocked, true);
   delete ini;
   
   TKIniFile*   mainIni = new TKIniFile(gTmpPath.c_str());
   //TKIniFile*  appiaIni = new TKIniFile(appia.c_str());

   std::string fwpasswords = GetAppiaPath() + GPassWordsIni;   // 設定Passwords.ini
   TKIniFile* passwordsIni = new TKIniFile(fwpasswords.c_str());
   
   std::string  nextDate = mainIni->ReadString(GXFT, GNextDate, ""); //F11要變更所有Block的ApplyDate
   sprintf(buf, "Get [XFT] NextDate=%s", nextDate.empty()?"Empty":nextDate.c_str());
   WriteMsg("F11", buf);
   
   TStrings  Brks, SesIDs, tmpIDs, NewSesIDs;
   TStrings::iterator  iter_cat;
   std::string  bfBrk, afBrk, bfBrkMkt, afBrkMkt;
   
   mainIni->ReadStrings(GMain, GBroker, Brks);   //讀出所有BrokerID
  
   std::string::iterator  iter_str;
   std::string  tmp, tmp1, tmp2, tmp3, tmp4;
   //將F11裡的內容取出
   //VecF11Data2I  iter = vF11Data2.begin();
   //fprintf(stderr, "2.F11Map_=%d\n", F11Map_->size());
   for(TF11MapI  iter=F11Map_->begin(); iter!=F11Map_->end(); ++iter) {
      bfBrk = iter->first.as_string();//BfBroker_.as_string();  //1200
      //fprintf(stderr, "bfBrk=%s\n", bfBrk.c_str());
      iter_cat = std::find(Brks.begin(), Brks.end(), bfBrk);
      if(iter_cat == Brks.end()) {
         //Alert錯誤訊息
         sprintf(buf, "TMP設定檔無原始券商=%s", bfBrk.c_str());
         if(Mgr_ = GetExcMgr())
            Mgr_->Alert("XF1006", "F11", buf);
         WriteMsg("F11", buf);
         continue;
      }
      afBrk = iter->second.as_string();//iter->AfBroker_.as_string(); //111A
//->因應雅萍反應，原始券商存在，受讓券商不存在，才更新設定檔      
      bfBrkMkt = mkt + bfBrk; //T1200
      afBrkMkt = mkt + afBrk; //T111A
      mainIni->ReadStrings(bfBrkMkt, GFixSes, SesIDs);  //T1200-->T1200X1, T1200X2
      if(mainIni->CopySection(bfBrkMkt, afBrkMkt) == false) { 
         //Alert錯誤訊息,存續券商代號已存在
         sprintf(buf, "TMP設定檔已存在,存續券商=%s", afBrkMkt.c_str());
         if(Mgr_ = GetExcMgr())
            Mgr_->Alert("XF1006", "F11", buf);
         WriteMsg("F11", buf);
         continue;
      }
      //寫入新的Broker, 刪除舊的
      mainIni->WriteStrings(GMain, GBroker, afBrk); //111A
      mainIni->EraseValue(GMain, GBroker, bfBrk);   //1200
      mainIni->EraseSection(bfBrkMkt); //刪除原Section

/*
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
            Mgr_->Alert("XF1006", "F11", buf);
         WriteMsg("F11", buf);
         continue;
      }
      mainIni->EraseSection(bfBrkMkt); //刪除原Section
*/      
//<-因應雅萍反應，原始券商存在，受讓券商不存在，才更新設定檔      

// --> 20131217 delete by k288 for 配合要求將 MkrBrkID(T111A) 內的 ApplyDate=yyyymmdd 刪除
/*
      if(nextDate.empty() == false) //寫入新的ApplyDate, T111A
         mainIni->WriteString(afBrkMkt, GApplyDate, nextDate);
*/         
// <-- 20131217 delete by k288 for 配合要求將 MkrBrkID(T111A) 內的 ApplyDate=yyyymmdd 刪除
      //HINTS:修改新Section寫入新的FixSes,刪除舊的FixSes      
      for(iter_cat=SesIDs.begin(); iter_cat!=SesIDs.end(); ++iter_cat) {
         tmp = *iter_cat;
         if(tmp.length() < afBrkMkt.length()) {
            //不應有這樣的情況,放棄此筆
            sprintf(buf, "原FixSesID=%s,(%d)長度小於,新Broker=%s, ", afBrkMkt.c_str(), tmp.length(), afBrkMkt.c_str());
            if(Mgr_ = GetExcMgr())
               Mgr_->Alert("XF1006", "F11", buf);
            WriteMsg("F11", buf);
            continue;
         }
         iter_str = tmp.begin();
         tmp.replace(iter_str, iter_str+afBrkMkt.length(), afBrkMkt);  //T1200X1 replace T111A --> T111AX1
         mainIni->WriteStrings(afBrkMkt, GFixSes, tmp); //新的FixSesID
         mainIni->EraseValue(afBrkMkt, GFixSes, *iter_cat); //刪除舊的FixSesID
         
         //mainIni->ReadStrings(*iter_cat, GTMPID, tmpIDs); //讀出所有該FixSesID的TMPID
         //先Copy整個Section
         if(mainIni->CopySection(*iter_cat, tmp) == false) { 
            //Alert錯誤訊息,新FixSesID代號已存在
            sprintf(buf, "TMP設定檔已存在,新FixSesID=%s", tmp.c_str());
            if(Mgr_ = GetExcMgr())
               Mgr_->Alert("XF1006", "F11", buf);
            WriteMsg("F11", buf);
            continue;
         }
         mainIni->EraseSection(*iter_cat); //刪除原Section
         if(nextDate.empty() == false) //寫入新的ApplyDate, T111AX1
            mainIni->WriteString(tmp, GApplyDate, nextDate);

         //更新Appia
         std::string appiaGroup = mainIni->ReadString(tmp, GAppiaGroup, "");
         if(appiaGroup.empty()) {
            //Alert錯誤訊息
            sprintf(buf, "TMP設定檔FixSesID=%s無原始appiaGroup", tmp.c_str());
            if(Mgr_ = GetExcMgr())
               Mgr_->Alert("XF1006", "F11", buf);
            WriteMsg("F11", buf);
            continue;
         }
         mainIni->WriteStrings(GSIDAG, appiaGroup, tmp);  //[SESSIONID-APPIAGROUP]寫入新的FixSesID
         mainIni->EraseValue(GSIDAG, appiaGroup, *iter_cat);  //[SESSIONID-APPIAGROUP]刪除舊的FixSesID

         std::string appia = GetAppiaPath() + GAppiaIni + "." + appiaGroup;  //設定Appia.ini
         TKIniFile*  appiaIni = new TKIniFile(appia.c_str());
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
               Mgr_->Alert("XF1006", "F11", buf);
            WriteMsg("F11", buf);
            continue;
         }
         appiaIni->EraseSection(tmp3);  //刪除原FixSesID的connection block
         appiaIni->WriteString(tmp4, GBrokerFirmID, tmp);  //寫入新的remote_firm_id
         delete appiaIni;
         
         //更新 passwords.ini
         std::string pwd = passwordsIni->ReadString(GPASSWORDS, *iter_cat, "");
         if(pwd.empty() == false)
            passwordsIni->WriteString(GPASSWORDS, tmp, pwd);
         passwordsIni->EraseValue(GPASSWORDS, *iter_cat, pwd);
      }
   }

   //釋放檔案使用權的旗標
   //appiaIni->WriteBool(GKway, GIsLocked, false);
   mainIni->WriteBool(GAppia, GIsLocked, false);
   //寫入做完指令
   if(procMkt == m_Tse)
      IsTF11Done_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   else
      IsOF11Done_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   mainIni->WriteString(GXFT, procMkt==m_Tse?GTF11Done:GOF11Done, procMkt==m_Tse?IsTF11Done_:IsOF11Done_);
   delete mainIni;
   delete passwordsIni;
   
   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert("XF1100", "F11", "F11檔案處理完成");
   }
   WriteMsg("F11", "End of Configuration.");
}
//---------------------------------------------------------------------------
//K_mf(void)  TwseFilesHandler::ConfigF13()
//{
//   WriteMsg("F13", "Start to Configure F13.");
//   char buf[256] = {0};
//   Kway::Tw::Stk::TMarket  procMkt = GetProcMkt();
//   TMarket  twse, otc(m_Otc);
//   std::string  mkt = procMkt.as_string();
//   //讀取Main.ini,取得路徑
//   //Lock檔案使用權的旗標
//   //std::string  appia = GetAppiaPath() + GAppiaIni;  //設定Appia.ini
//   //TKIniFile*  ini = new TKIniFile(appia.c_str());
//   //ini->WriteBool(GKway, GIsLocked, true);
//   //delete ini;
//
//   gTmpPath = GetTmpPath() + GMainIni;
//   TKIniFile*  ini = new TKIniFile(gTmpPath.c_str());
//   ini->WriteBool(GAppia, GIsLocked, true);
//   delete ini;
//
//   TKIniFile*   mainIni = new TKIniFile(gTmpPath.c_str());
//   //TKIniFile*  appiaIni = new TKIniFile(appia.c_str());
//
//   std::string  nextDate = mainIni->ReadString(GXFT, GNextDate, ""); //F13要變更所有Block的ApplyDate
//   sprintf(buf, "Get [XFT] NextDate=%s", nextDate.empty()?"Empty":nextDate.c_str());
//   WriteMsg("F13", buf);
//
//   TStrings  Brks, SesIDs;
//   TStrings::iterator  iter_cat;
//   std::string  broker, brokerMkt, sesID, tmp, tmp1;
//   std::string  bfVer, afVer, tmpSrc, tmpVer;
//
//   mainIni->ReadStrings(GMain, GBroker, Brks);   //讀出所有BrokerID
//   for(TF13MapI  iter=F13Map_->begin(); iter!=F13Map_->end(); ++iter) {
//      broker = iter->first.Broker_.as_string(); //1200
//      iter_cat = std::find(Brks.begin(), Brks.end(), broker);
//      if(iter_cat == Brks.end()) {
//         //Alert錯誤訊息
//         sprintf(buf, "TMP設定檔無原始券商=%s", broker.c_str());
//         if(Mgr_ = GetExcMgr())
//            Mgr_->Alert("XF1006", "F13", buf);
//         WriteMsg("F13", buf);
//         continue;
//      }
//      sesID = mkt + iter->first.as_string(); //T1200X1
//      brokerMkt = mkt + broker; //T1200
//      //版本檢查,有效性與差異性
//      tmpSrc = iter->second.BfFixVer_.as_string();
//      tmpVer.clear();
//      if(GetAppiaFixVerString(tmpSrc, tmpVer) == false) {
//         //Alert錯誤訊息
//         sprintf(buf, "F13設定檔 F13-VER-BEFORE 欄位有誤, sid=%s, bfV=%s", sesID.c_str(), tmpSrc.c_str());
//         if(Mgr_ = GetExcMgr())
//            Mgr_->Alert("XF1006", "F13", buf);
//         WriteMsg("F13", buf);
//         continue;
//      }
//      bfVer = tmpVer;
//      tmpSrc = iter->second.AfFixVer_.as_string();
//      tmpVer.clear();
//      if(GetAppiaFixVerString(tmpSrc, tmpVer) == false) {
//         //Alert錯誤訊息
//         sprintf(buf, "F13設定檔 F13-VER-AFTER 欄位有誤, sid=%s, afV=%s", sesID.c_str(), tmpSrc.c_str());
//         if(Mgr_ = GetExcMgr())
//            Mgr_->Alert("XF1006", "F13", buf);
//         WriteMsg("F13", buf);
//         continue;
//      }
//      afVer = tmpVer;
//      /* 應該可以不檢查,避免真的有需要使用到
//      if(bfVer == afVer) {
//         //Alert錯誤訊息
//         sprintf(buf, "F13設定檔 F13-VER-BEFORE 與 F13-VER-AFTER 相同, sid=%s, afV=%s", sesID.c_str(), tmpSrc.c_str());
//         if(Mgr_ = GetExcMgr())
//            Mgr_->Alert("XF1006", "F13", buf);
//         WriteMsg("F13", buf);
//         continue;
//      }
//      */
//      //更新Main.ini之ApplyDate
//      mainIni->ReadStrings(brokerMkt, GFixSes, SesIDs);  //T1200-->T1200X1, T1200X2
//      iter_cat = std::find(SesIDs.begin(), SesIDs.end(), sesID);
//      if(iter_cat == SesIDs.end()) {
//         //Alert錯誤訊息
//         sprintf(buf, "TMP設定檔無原始FixSesID=%s", sesID.c_str());
//         if(Mgr_ = GetExcMgr())
//            Mgr_->Alert("XF1006", "F13", buf);
//         WriteMsg("F13", buf);
//         continue;
//      }
//      //if(nextDate.empty() == false) //寫入新的ApplyDate, T1200X1
//      //   mainIni->WriteString(sesID, GApplyDate, nextDate); //20110316: TWSE說不需要更新
//      //更新Appia
//      std::string appiaGroup = mainIni->ReadString(sesID, GAppiaGroup, "");
//      if(appiaGroup.empty()) {
//         //Alert錯誤訊息
//         sprintf(buf, "TMP設定檔FixSesID=%s無原始appiaGroup", sesID.c_str());
//         if(Mgr_ = GetExcMgr())
//            Mgr_->Alert("XF1006", "F13", buf);
//         WriteMsg("F13", buf);
//         continue;
//      }
//      std::string appia = GetAppiaPath() + GAppiaIni + "." + appiaGroup;  //設定Appia.ini
//      TKIniFile*  appiaIni = new TKIniFile(appia.c_str());
//
//      tmp  = ConvertTand2Appia(sesID);  //轉換原的FixSesID to Appia
//      tmp1 = appiaIni->ReadString(tmp, GConFixVer, "");
//      if(tmp1.empty()) {
//         //Alert錯誤訊息
//         sprintf(buf, "Appia無原始FIX版本訊息,FixSesID=%s", tmp.c_str());
//         if(Mgr_ = GetExcMgr())
//            Mgr_->Alert("XF1006", "F13", buf);
//         WriteMsg("F13", buf);
//         continue;
//      }
//      if(tmp1 != bfVer) { //原設定版本不符,僅通知並直接設定為新版本
//         //Alert錯誤訊息
//         //sprintf(buf, "Appia設定檔原始FIX版本與 F13-VER-BEFORE 不同, FixSesID=%s, appVer=%s", tmp.c_str(), tmp1.c_str());
//         sprintf(buf, "Appia VER與 F13-VER-BEFORE 不同, FixSesID=%s, appVer=%s, F13=%s", tmp.c_str(), tmp1.c_str(), bfVer.c_str());
//         if(Mgr_ = GetExcMgr())
//            Mgr_->Alert("XF1006", "F13", buf);
//         WriteMsg("F13", buf);
//      }
//      //mainIni->WriteString(sesID, GConFixVer, tmpSrc==std::string("1")?"42":"44"); //for cooper using
//      appiaIni->WriteString(tmp, GConFixVer, afVer);
//      sprintf(buf, "FixSesID=%s, %s -> %s", tmp.c_str(), tmp1.c_str(), afVer.c_str());
//      WriteMsg("F13", buf);
//      delete appiaIni;
//   }
//
//   //釋放檔案使用權的旗標
//   //appiaIni->WriteBool(GKway, GIsLocked, false);
//   mainIni->WriteBool(GAppia, GIsLocked, false);
//   //寫入做完指令
//   tmp = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
//   if(procMkt == m_Tse)
//      IsTF13Done_ = tmp;
//   else
//      IsOF13Done_ = tmp;
//   //fprintf(stderr, "mkt=%s", procMkt==m_Tse?"Tse":"Otc");
//   mainIni->WriteString(GXFT, procMkt==m_Tse?GTF13Done:GOF13Done, procMkt==m_Tse?IsTF13Done_:IsOF13Done_);
//   delete mainIni;
//   //delete appiaIni;
//
//   if(Mgr_ = GetExcMgr()) {
//      Mgr_->Alert("XF1100", "F13", "F13檔案處理完成");
//   }
//   WriteMsg("F13", "End of Configuration.");
//}
//---------------------------------------------------------------------------
K_mf(bool)  TwseFilesHandler::ConfigOrSave(SKey4ProdSort& keyProd, SF03Data2& f03, TKIniFile& main, TKIniFile& appia, TKIniFile& passwords)
{
   char  buf[512] = {0};
   std::string  tmp, tmp1, tmp2;
   std::string  sessionID = keyProd.GetSesID();
   std::string  socketPortSesID = keyProd.GetSesID();
   std::string  mbrkID    = keyProd.SesID_.Market_.as_string() + keyProd.SesID_.Broker_.as_string();
   std::string  appiaKey  = ConvertTand2Appia(sessionID); //T5U3U8UAUXU1U
   sprintf(buf, "socPSID=%s, sid=%s, mbrk=%s, appK=%s", socketPortSesID.c_str(), sessionID.c_str(), mbrkID.c_str(), appiaKey.c_str());
   WriteMsg(GXFT, buf);

   SKey4Prod  key4F05;
   key4F05.Type_ = keyProd.Type_;
   key4F05.SesID_= keyProd.SesID_;
   
   TStrings  TmpSesIDs;
   
   if(f03.WorkCode_[0] == 'D') { //刪除直接執行,不需理會F05 or F04
      SetOnlineDate(keyProd);
      switch(f03.WhichType()) {
      case est_None: 
         //alert
         if(Mgr_ = GetExcMgr()) //檔案重要欄位資料有誤
            Mgr_->Alert("XF1006", "Prod", "");
         WriteMsg(GXFT, "Erase-F03 Type_ Error");
         return false;
      case est_NewFix:  //刪除appia.ini以及Passwords.ini
         //刪除appia.ini
         appia.EraseSection(appiaKey); //erase connection block [sessionID]
         tmp = "[" + appiaKey + "]";
         appia.EraseValue(GAppiaMain, GAConnection, tmp); //erase connection in [Main]
         sprintf(buf, "Erase Appia %s and %s", appiaKey.c_str(), tmp.c_str());
         WriteMsg(GXFT, buf);
// --> 20131031 交易所決定將密碼檔passwords.ini每天統一由F05的資料重新產生，所以將異動passwords.ini的動作移到F10產生流量檔之前執行         
//         //刪除passwords.ini
//         passwords.EraseValue(GPASSWORDS, sessionID, f03.Passwd_.as_string());
//         sprintf(buf, "Erase PassWord, sid=%s pw=%s", sessionID.c_str(), f03.Passwd_.as_string().c_str());
//         WriteMsg(GXFT, buf);
// <-- 20131031 交易所決定將密碼檔passwords.ini每天統一由F05的資料重新產生，所以將異動passwords.ini的動作移到F10產生流量檔之前執行         
         //刪除main.ini
         main.EraseValue(GSIDAG, f03.AppiaGroup_.as_string(), sessionID);  //[SESSIONID-APPIAGROUP]
         main.EraseSection(sessionID); //[T140001]
         main.EraseValue(mbrkID, GFixSes, sessionID); //[T1400]-->FixSes
         if(main.ReadStrings(mbrkID, GFixSes, TmpSesIDs) == 0) {
            main.EraseSection(mbrkID);
// --> 20140224 將[T1400] 內沒有FixSes時，要將[main]內的 Broker = 1400 刪除
            main.EraseValue(GMain, GBroker, keyProd.SesID_.Broker_.as_string());
// <-- 20140224 將[T1400] 內沒有FixSes時，要將[main]內的 Broker = 1400 刪除
         }
         sprintf(buf, "Erase Main, Section=%s", sessionID.c_str());
         WriteMsg(GXFT, buf);
         break;
      }
      if(Mgr_ = GetExcMgr()) { //刪除成功
         Mgr_->Alert("XF1022", "Prod", ""); //Appia
// --> 20131031 交易所決定將密碼檔passwords.ini每天統一由F05的資料重新產生，所以將異動passwords.ini的動作移到F10產生流量檔之前執行         
//         Mgr_->Alert("XF1023", "Prod", ""); //Passwords
// <-- 20131031 交易所決定將密碼檔passwords.ini每天統一由F05的資料重新產生，所以將異動passwords.ini的動作移到F10產生流量檔之前執行         
      }
// --> 20130506 雅萍通知流量檔再度由F03改到F05      
//   } else if(f03.WorkCode_[0] == 'I' || f03.WorkCode_[0] == 'U') { //新增需核對F05以及F04     
// <-- 20130506 雅萍通知流量檔再度由F03改到F05
   } else if(f03.WorkCode_[0] == 'I') { //新增需核對F05以及F04     
      std::string  nextDate = main.ReadString(GXFT, GNextDate, "");
      if(nextDate.empty()) {
         if(Mgr_ = GetExcMgr()) //F04之次交易日不存在
            Mgr_->Alert("XF1010", "Prod", "");
         WriteMsg(GXFT, "Insert-F04之次交易日不存在");
         return false;
      }
      
      TKDate  next, effDate;
      next.assign(nextDate);
      effDate.assign(f03.EffDate_.as_string());
   
      if(next >= effDate) { //設定
         TF05Map::iterator  iter_f05 = F05Map_->find(key4F05);
         if(iter_f05 == F05Map_->end()) {
            if(Mgr_ = GetExcMgr()) //找不到對應之F05資料
                    Mgr_->Alert("XF1013", "Prod", "");
            sprintf(buf, "Insert-無F05資料-M=%s, B=%s, pvc=%s", key4F05.SesID_.Market_.as_string().c_str(), key4F05.SesID_.Broker_.as_string().c_str(), key4F05.SesID_.PVCID_.as_string().c_str());
            WriteMsg(GXFT, buf);
            return false;
         }
         if(iter_f05->second.Type_==f03.Type_ && iter_f05->second.PVCID_==f03.PVCID_ && 
            iter_f05->second.PVCName_==f03.PVCName_ && iter_f05->second.Passwd_==f03.Passwd_ &&
            iter_f05->second.FWGroup_==f03.FWGroup_ && iter_f05->second.AppiaGroup_==f03.AppiaGroup_) {

            tmp2= "[" + appiaKey + "]";
            std::string  src, ver;
            //寫入Passwords.ini, twse.ini 
            switch(f03.WhichType()) {
            case est_None: 
               //alert
               if(Mgr_ = GetExcMgr()) //檔案重要欄位資料有誤
                  Mgr_->Alert("XF1006", "Prod", "F03");
               WriteMsg(GXFT, "Insert-F03 Type_ Error");
               return false;
            case est_NewFix:
// --> 20130506 雅萍通知流量檔再度由F03改到F05      
//               if(f03.WorkCode_[0] == 'U') {
//                  main.WriteInt(sessionID, GTotalFlow, atoi(f03.TotalFlow_.as_string().c_str()));
//                  break;
//               }                  
// <-- 20130506 雅萍通知流量檔再度由F03改到F05      
               src = f03.FixVersion_.as_string();
               if(GetAppiaFixVerString(src, ver) == false) {
                  if(Mgr_ = GetExcMgr()) {
                     sprintf(buf, "設定Appia失敗, appK=%s,   F03-FIX-VERSION錯誤", appiaKey.c_str());
                     Mgr_->Alert("XF1006", "Prod", buf);
                     sprintf(buf, "FIX version error, appK=%s, recvVer=%s", appiaKey.c_str(), src.c_str());
                     WriteMsg(GXFT, buf);
                  }
                  return false;
               }
               //新增至[Main]
               appia.WriteStrings(GAppiaMain, GAConnection, tmp2);
               //新增至[connection]
               appia.WriteString(appiaKey, GConFixVer, ver);
               WriteDefaultAppia(appia, appiaKey); //由ini檔指定寫入的key與value
               appia.EraseIndent(appiaKey, GNetAddress);
               if(f03.IP1_.empty() == false && f03.IP1_.as_string() != "               ")
                  appia.WriteStrings(appiaKey, GNetAddress, f03.IP1_.as_string());
               if(f03.IP2_.empty() == false && f03.IP2_.as_string() != "               ")
                  appia.WriteStrings(appiaKey, GNetAddress, f03.IP2_.as_string());
               appia.WriteString(appiaKey, GRemotePort, f03.SrcPort_.as_string()); //[Add]20110523
               appia.WriteString(appiaKey, GLocalFirmID, f03.Market_[0]=='T'?GTwse:GOtc);
               appia.WriteString(appiaKey, GBrokerFirmID, sessionID);

// --> 20131031 交易所決定將密碼檔passwords.ini每天統一由F05的資料重新產生，所以將異動passwords.ini的動作移到F10產生流量檔之前執行         
//               //將FIX pwd寫入Passwords.ini
//               passwords.WriteString(GPASSWORDS, iter_f05->first.GetSesID(), iter_f05->second.Passwd_.as_string());
// <-- 20131031 交易所決定將密碼檔passwords.ini每天統一由F05的資料重新產生，所以將異動passwords.ini的動作移到F10產生流量檔之前執行         
               main.WriteStrings(GMain, GBroker, keyProd.SesID_.Broker_.as_string());
               main.WriteStrings(mbrkID, GFixSes, sessionID);
// --> 20131217 delete by k288 for 配合要求將 MkrBrkID(T111A) 內的 ApplyDate=yyyymmdd 刪除
//               main.WriteString(mbrkID, GApplyDate, f03.EffDate_.as_string());
// <-- 20131217 delete by k288 for 配合要求將 MkrBrkID(T111A) 內的 ApplyDate=yyyymmdd 刪除
               main.WriteStrings(GSIDAG, f03.AppiaGroup_.as_string(), sessionID);  //[SESSIONID-APPIAGROUP]
               
               //將其他未填的資料寫入Main.ini, 供F10使用
               main.WriteString(sessionID, GLineNo, f03.LineNo_.as_string());
               main.WriteString(sessionID, GTermID, f03.PVCName_.as_string());
               main.WriteString(sessionID, GAPCode, f03.APCode_.as_string());
               main.WriteString(sessionID, GFWGroup, f03.FWGroup_.as_string());
               main.WriteString(sessionID, GAppiaGroup, f03.AppiaGroup_.as_string());
               //將出現的appia群組寫入main.ini內的main供後面使用
               main.WriteStrings(GMain, GAppiaGroup, f03.AppiaGroup_.as_string());
               main.WriteString(sessionID, GIP, f03.DestIP_.as_string());
               main.WriteString(sessionID, GListPort1, f03.ListPort1_.as_string());
               main.WriteString(sessionID, GListPort2, f03.ListPort2_.as_string());
               main.WriteInt(sessionID, GTotalFlow, atoi(f03.TotalFlow_.as_string().c_str()));
               main.WriteString(sessionID, GApplyDate, f03.EffDate_.as_string());
               
               if(Mgr_ = GetExcMgr()) //Appia正式環境，連線新增OK
                  Mgr_->Alert("XF1020", "Prod", "");
               sprintf(buf, "Insert-Appia key=%s, sid=%s", appiaKey.c_str(), sessionID.c_str());
               WriteMsg(GXFT, buf);
               break;
            } //switch(...)
            SetOnlineDate(keyProd);
         } else {
            if(Mgr_ = GetExcMgr()) //F03與F05相同欄位資料不符
               Mgr_->Alert("XF1012", "Prod", "");
            sprintf(buf, "Insert-F03與F05相同欄位資料不符-M=%s, B=%s, pvc=%s", keyProd.SesID_.Market_.as_string().c_str(), keyProd.SesID_.Broker_.as_string().c_str(), keyProd.SesID_.PVCID_.as_string().c_str());
            WriteMsg(GXFT, buf);
            return false;
                      //InsertF03Map(keyProd, value);
         }
      } else { //等待next
         if(Mgr_ = GetExcMgr()) //生效日晚於次交易日
            Mgr_->Alert("XF1011", "Prod", "");
         sprintf(buf, "Insert-生效日晚於次交易日-M=%s, B=%s, pvc=%s", keyProd.SesID_.Market_.as_string().c_str(), keyProd.SesID_.Broker_.as_string().c_str(), keyProd.SesID_.PVCID_.as_string().c_str());
         WriteMsg(GXFT, buf);
         return false;
      }
// 雅萍通知,交易所取消F12後,流量檔資料來源改由F03新增及異動
// 所以合併到上方 f03.WorkCode_[0] == 'I'統一處理
// 20130506 雅萍通知流量檔再度由F03改到F05      
   } else if(f03.WorkCode_[0] == 'U') { 
      //變更更新訊息流量,因為流量檔統一由F12產生,所以這裡不做任何處理
   } else {
      //alert
      if(Mgr_ = GetExcMgr()) //檔案重要欄位資料有誤
         Mgr_->Alert("XF1006", "Prod", "非預期之F03-WK-CODE內容");
      WriteMsg(GXFT, "非預期之F03-WK-CODE");
   }
   return true;
   //fprintf(stderr, "2-->F03Map size=%d\n", F03Map_->size());
}
//---------------------------------------------------------------------------
K_mf(bool)  TwseFilesHandler::ChkNInsertData(SF01Data2& f01, std::string& applyDate, TKIniFile& main)
{
   char buf[256] = {0};
   if(f01.Market_.empty() || (f01.Market_[0]!='T' && f01.Market_[0]!='O') || f01.Broker_.empty() || f01.FixPVCID_.empty()) {
      WriteMsg("F01", "Mkt, Brk or Pvc Data Error.");
      return false;
   }
     
   std::string  tmp = f01.Broker_.as_string();
   if(tmp.length() < 4) {
      WriteMsg("F01", "Broker ID Length Error.");
      return false;
   }
   tmp = main.ReadString(GAppiaFWGroup, f01.AppiaGroup_.as_string(), "");
   if(tmp.empty()) {
      sprintf(buf, "查無 AppiaGroup %s.", f01.AppiaGroup_.as_string().c_str());
      WriteMsg("F01", buf);
      return false;
   }
      
   if(tmp != f01.FWGroup_.as_string()) {
      sprintf(buf, "%s 的 FWGroup %s 與查到的 %s 不符", f01.AppiaGroup_.as_string().c_str(),
         f01.FWGroup_.as_string().c_str(), tmp.c_str());
      WriteMsg("F01", buf);
      return false;
   }
   
   std::string  broker = f01.Broker_.as_string().substr(0,4);

   SKey4Prod  key;
   key.Type_ = f01.Type_;
   key.SesID_.Market_ = f01.Market_;
   key.SesID_.Broker_.assign(broker);
   key.SesID_.PVCID_ = f01.FixPVCID_;
   
   SF01Values  v;
   v.ApplyDate_.assign(applyDate);
   v.F01Data2_ = f01;
   
   TF01Map::iterator  iter = F01Map_->fetch(key);
   TF01Map::updater   upd  = F01Map_->begin_update(iter);
   *upd = v;

   return true;
}
//---------------------------------------------------------------------------
K_mf(bool)  TwseFilesHandler::ChkNInsertData(SF03Data2& f03, TKDate& today, dword& seq, TKIniFile& main)
{
   char buf[256] = {0};
   if(f03.Market_.empty() || (f03.Market_[0]!='T' && f03.Market_[0]!='O') || f03.Broker_.empty() || 
      f03.PVCID_.empty() || (f03.WorkCode_[0]!='I' && f03.WorkCode_[0]!='D' && f03.WorkCode_[0]!='U') || 
      f03.FWGroup_.empty() || f03.AppiaGroup_.empty()) {
      WriteMsg("F03", "Mkt, Brk, Pvc, Wk, FWGroup or AppiaGroup Data Error.");
      return false;
   }
   
   std::string  tmp = f03.Broker_.as_string();
   if(tmp.length() < 4) {
      WriteMsg("F03", "Broker ID Length Error.");
      return false;
   }
   tmp = main.ReadString(GAppiaFWGroup, f03.AppiaGroup_.as_string(), "");
   if(tmp.empty()) {
      sprintf(buf, "查無 AppiaGroup %s.", f03.AppiaGroup_.as_string().c_str());
      WriteMsg("F03", buf);
      return false;
   }
      
   if(tmp != f03.FWGroup_.as_string()) {
      sprintf(buf, "%s 的 FWGroup %s 與查到的 %s 不符", f03.AppiaGroup_.as_string().c_str(),
         f03.FWGroup_.as_string().c_str(), tmp.c_str());
      WriteMsg("F03", buf);
      return false;
   }
   
   SKey4ProdSort  keyProd;
   keyProd.Date_ = today;
   keyProd.Seq_  = seq;
   keyProd.Type_ = f03.Type_;
   keyProd.SesID_.Market_ = f03.Market_;
   keyProd.SesID_.Broker_.assign(f03.Broker_.as_string());
   keyProd.SesID_.PVCID_ = f03.PVCID_;
   SProdValues  value;
   value.SettingDate_ = today;  //設定日
   value.F03Data2_ = f03;
   
   InsertF03Map(keyProd, value);
   return true;
}
//---------------------------------------------------------------------------
K_mf(bool)  TwseFilesHandler::ChkNInsertData(SF05Data2& f05, TMarket& mkt, TKIniFile& main)
{
   char  buf[256] = {0};
   if(f05.Type_[0]!='N' || f05.Broker_.empty() || f05.PVCID_.empty() || 
      f05.FWGroup_.empty() || f05.AppiaGroup_.empty()) {
      WriteMsg("F05", "Type, Brk, Pvc, FWGroup or AppiaGroup Data Error.");
      return false;
   }
     
   std::string tmp = main.ReadString(GAppiaFWGroup, f05.AppiaGroup_.as_string(), "");
   if(tmp.empty()) {
      sprintf(buf, "查無 AppiaGroup %s.", f05.AppiaGroup_.as_string().c_str());
      WriteMsg("F05", buf);
      return false;
   }
      
   if(tmp != f05.FWGroup_.as_string()) {
      sprintf(buf, "%s 的 FWGroup %s 與查到的 %s 不符", f05.AppiaGroup_.as_string().c_str(),
         f05.FWGroup_.as_string().c_str(), tmp.c_str());
      WriteMsg("F05", buf);
      return false;
   }
   
   SKey4Prod  keyProd;
   keyProd.Type_ = f05.Type_;
   keyProd.SesID_.Market_.assign(mkt.as_string());
   keyProd.SesID_.Broker_.assign(f05.Broker_.as_string());
   keyProd.SesID_.PVCID_ = f05.PVCID_;
   
   InsertF05Map(keyProd, f05);

   return true;
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::CheckF03Today()
{
   std::vector<SKey4ProdSort>  Vec;
   Vec.clear();
   TKDate  today = TKDate::Now();
   TF03Map::iterator   iter_F03;
   for(iter_F03 = F03Map_->begin(); iter_F03!=F03Map_->end(); ++iter_F03) {
      if(iter_F03->second.SettingDate_ == today)
         Vec.push_back(iter_F03->first);
   }
   for(std::vector<SKey4ProdSort>::iterator  iter=Vec.begin(); iter!=Vec.end(); ++iter) {
      iter_F03 = F03Map_->find(*iter);
      if(iter_F03 != F03Map_->end())
         F03Map_->erase(*iter);
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
K_mf(void)  TwseFilesHandler::InsertF03Map(SKey4ProdSort& key, SProdValues& value)
{
   TF03Map::iterator  iter = F03Map_->fetch(key);
   TF03Map::updater   upd  = F03Map_->begin_update(iter);
   *upd = value;
   //fprintf(stderr, "InsertF03Map-->F03Map size=%d\n", F03Map_->size());
}
//---------------------------------------------------------------------------
//K_mf(void)  TwseFilesHandler::SetOnlineDate(SKey4Prod& key)
K_mf(void)  TwseFilesHandler::SetOnlineDate(SKey4ProdSort& key)
{
   TF03Map::iterator  iter = F03Map_->find(key);
   if(iter == F03Map_->end())
      return;

   TF03Map::updater   upd  = F03Map_->begin_update(iter);
   upd->OnlineDate_ = TKDate::Now();
}
//---------------------------------------------------------------------------
K_mf(bool)  TwseFilesHandler::CheckF03orF05(TMarket& mkt, TFileType type)
{
   if(type!=ft_F03 && type!=ft_F05)
      return false;
   
   TMarket  twse, otc(m_Otc);
   TKDate   date;
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gTmpPath.c_str());
   if(mkt == twse) {         
      if(type == ft_F03) {
         IsTF03Ready_ = iniFile->ReadString(GXFT, GTF03Ready, "");
         if(IsTF03Ready_.empty())
            return false;
         date.assign(IsTF03Ready_);
         if(date != TKDate::Now())
            return false;
      } else {
         IsTF05Ready_ = iniFile->ReadString(GXFT, GTF05Ready, "");
         if(IsTF05Ready_.empty())
            return false;
         date.assign(IsTF05Ready_);
         if(date != TKDate::Now())
            return false;
      }
   } else if(mkt == otc) {
      if(type == ft_F03) {
         IsOF03Ready_ = iniFile->ReadString(GXFT, GOF03Ready, "");
         if(IsOF03Ready_.empty())
            return false;
         date.assign(IsOF03Ready_);
         if(date != TKDate::Now())
            return false;
      } else {
         IsOF05Ready_ = iniFile->ReadString(GXFT, GOF05Ready, "");
         if(IsOF05Ready_.empty())
            return false;
         date.assign(IsOF05Ready_);
         if(date != TKDate::Now())
            return false;
      }
   } else
      return false;
   
   delete iniFile;
   return true;
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::CheckF03Map()
{
   TKDate  today = TKDate::Now();
   std::vector<SKey4ProdSort>  map;
   for(TF03Map::iterator  iter = F03Map_->begin(); iter!=F03Map_->end(); ++iter) {
      if(iter->second.OnlineDate_.IsInvalid())
                continue;
             //if(iter->second.OnlineDate_ > today) //這個應該永遠也不會有
             if(iter->second.OnlineDate_ < today)  //改為小於當天的移除,移除過期的
                map.push_back(iter->first);
   }
   
   //for(std::vector<SKey4Prod>::iterator  iter=map.begin(); iter!=map.end(); ++iter)
   for(std::vector<SKey4ProdSort>::iterator  iter=map.begin(); iter!=map.end(); ++iter)
      F03Map_->erase(*iter);
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::InsertF05Map(SKey4Prod& key, SF05Data2& value)
{
   TF05Map::iterator  iter = F05Map_->fetch(key);
   TF05Map::updater   upd  = F05Map_->begin_update(iter);
   *upd = value;
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::RebuildF05Map(TKIniFile& main)
{
   Kway::Tw::Stk::TMarket  procMkt = GetProcMkt();
   std::string  fType("F05");
   TMarket  twse, otc(m_Otc);
   std::string  File;
   if(procMkt == twse) {
      File = main.ReadString(GXFT, GTF05File, "");
   } else if(procMkt == otc) {
      File = main.ReadString(GXFT, GOF05File, "");
   }
   
   if(File.empty() == false) {
      StartHandleAFile(procMkt, fType, File, true);
   }
   
   WriteMsg(GXFT, "RebuildF05Map Done!");
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
// --> 20131031 交易所決定將密碼檔passwords.ini每天統一由F05的資料重新產生
K_mf(void)  TwseFilesHandler::ConfigPasswords(TKIniFile& main, TKIniFile& passwords)
{
   WriteMsg(GXFT, "Start to Configure passwords.ini.");
   passwords.EraseSection(GPASSWORDS);

   char buf[256] = {0};
   TStrings  AGs, Sess;
   std::string SID;
   main.ReadStrings(GMain, GAppiaGroup, AGs);
   
   SKey4Prod  key4F05;
   key4F05.Type_.assign("N");

   // -->讀取每天早上灌單測試用的Fix SesIDs
   Kway::Tw::Stk::TMarket  procMkt = GetProcMkt();
   std::string  mkt = procMkt.as_string();
   TStrings  Testbrks, TestsesIDs, tmpIDs;
   bool result;
   main.ReadStrings(GMain, "TestBroker", Testbrks);   
   for(TStrings::iterator iter_brks = Testbrks.begin(); iter_brks != Testbrks.end(); ++iter_brks) {
      std::string  mktBrk = mkt + (*iter_brks);
      main.ReadStrings(mktBrk, GFixSes, tmpIDs);
      for(TStrings::iterator iter_tmp = tmpIDs.begin(); iter_tmp != tmpIDs.end(); ++iter_tmp) {
         result = false;
         for(TStrings::iterator iter_IDs = TestsesIDs.begin(); iter_IDs != TestsesIDs.end(); ++iter_IDs) {
            if(*iter_tmp == *iter_IDs){
               result = true;
               break;
            }
         }
         if(result == false) {
            TestsesIDs.push_back(*iter_tmp);
         }
      }       
   }
   // <--讀取每天早上灌單測試用的Fix SesIDs
   for(TStrings::iterator iterAGs = AGs.begin(); iterAGs != AGs.end(); ++iterAGs) {
      std::string AppiaGroup = *iterAGs;
      main.ReadStrings(GSIDAG, AppiaGroup, Sess);
         
      for(TStrings::iterator iterSess = Sess.begin(); iterSess != Sess.end(); ++iterSess) {
         SID = *iterSess;
         // -->排除每天早上灌單測試用的Fix SesIDs
         result = false;
         for(TStrings::iterator iter_IDs = TestsesIDs.begin(); iter_IDs != TestsesIDs.end(); ++iter_IDs) {
            if(SID == *iter_IDs){
               result = true;
               break;
            }
         }
         if(result == true) {
            continue;
         }
         // <--排除每天早上灌單測試用的Fix SesIDs
         
         key4F05.SesID_.Market_.assign(SID.substr(0, 1));
         key4F05.SesID_.Broker_.assign(SID.substr(1, 4));
         key4F05.SesID_.PVCID_.assign(SID.substr(5, 2));
         TF05Map::iterator  iter_f05 = F05Map_->find(key4F05);
         if(iter_f05 == F05Map_->end()) {
            sprintf(buf, "F05查無 %s%s%s 的資料", key4F05.SesID_.Market_.as_string().c_str(), key4F05.SesID_.Broker_.as_string().c_str(), key4F05.SesID_.PVCID_.as_string().c_str());
            if(Mgr_ = GetExcMgr()) //找不到對應之F05資料
               Mgr_->Alert("XF1013", "Prod", "");
            WriteMsg(GXFT, buf);
            continue;
         }  
         passwords.WriteString(GPASSWORDS, iter_f05->first.GetSesID(), iter_f05->second.Passwd_.as_string());
      }
   }
   
}
// <-- 20131031 交易所決定將密碼檔passwords.ini每天統一由F05的資料重新產生
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ConfigAppiaSmp(TKIniFile& main)
{
   WriteMsg(GXFT, "Start to Configure appia-smp.ini.xxx.");

   typedef Kstl::ev_map<std::string, std::string>     TSmpMap;
   
   char buf[256] = {0};
   TStrings  AGs, Sess;
   std::string SID;
   main.ReadStrings(GMain, GAppiaGroup, AGs);
   
   SKey4Prod  key4F05;
   key4F05.Type_.assign("N");

   // -->讀取每天早上灌單測試用的Fix SesIDs
   Kway::Tw::Stk::TMarket  procMkt = GetProcMkt();
   std::string  mkt = procMkt.as_string();
   TStrings  Testbrks, TestsesIDs, tmpIDs;
   bool result;
   main.ReadStrings(GMain, "TestBroker", Testbrks);   
   for(TStrings::iterator iter_brks = Testbrks.begin(); iter_brks != Testbrks.end(); ++iter_brks) {
      std::string  mktBrk = mkt + (*iter_brks);
      main.ReadStrings(mktBrk, GFixSes, tmpIDs);
      for(TStrings::iterator iter_tmp = tmpIDs.begin(); iter_tmp != tmpIDs.end(); ++iter_tmp) {
         result = false;
         for(TStrings::iterator iter_IDs = TestsesIDs.begin(); iter_IDs != TestsesIDs.end(); ++iter_IDs) {
            if(*iter_tmp == *iter_IDs){
               result = true;
               break;
            }
         }
         if(result == false) {
            TestsesIDs.push_back(*iter_tmp);
         }
      }       
   }
   // <--讀取每天早上灌單測試用的Fix SesIDs

   for(TStrings::iterator iterAGs = AGs.begin(); iterAGs != AGs.end(); ++iterAGs) {
      std::string AppiaGroup = *iterAGs;
      main.ReadStrings(GSIDAG, AppiaGroup, Sess);
         
      TSmpMap* SmpMap_ = new TSmpMap();
      for(TStrings::iterator iterSess = Sess.begin(); iterSess != Sess.end(); ++iterSess) {
         SID = *iterSess;
         // -->排除每天早上灌單測試用的Fix SesIDs
         result = false;
         for(TStrings::iterator iter_IDs = TestsesIDs.begin(); iter_IDs != TestsesIDs.end(); ++iter_IDs) {
            if(SID == *iter_IDs){
               result = true;
               break;
            }
         }
         if(result == true) {
            continue;
         }
         // <--排除每天早上灌單測試用的Fix SesIDs
         
         key4F05.SesID_.Market_.assign(SID.substr(0, 1));
         key4F05.SesID_.Broker_.assign(SID.substr(1, 4));
         key4F05.SesID_.PVCID_.assign(SID.substr(5, 2));
         TF05Map::iterator  iter_f05 = F05Map_->find(key4F05);
         if(iter_f05 == F05Map_->end()) {
            sprintf(buf, "F05查無 %s%s%s 的資料", key4F05.SesID_.Market_.as_string().c_str(), key4F05.SesID_.Broker_.as_string().c_str(), key4F05.SesID_.PVCID_.as_string().c_str());
            if(Mgr_ = GetExcMgr()) //找不到對應之F05資料
               Mgr_->Alert("XF1013", "Prod", "");
            WriteMsg(GXFT, buf);
            continue;
         }  
         std::string SmpGroup = iter_f05->second.SMGroup_.as_string();         
         TSmpMap::iterator iter_smp = SmpMap_->fetch(SmpGroup);
         std::string SessionIDs = iter_smp->second;
         if(SessionIDs.empty()) 
            SessionIDs = SID;
         else
            SessionIDs = SessionIDs + ":" + SID;
         iter_smp->second = SessionIDs;
      }
      std::string filename = GetAppiaPath() + GAppiaSmpIni + "." + AppiaGroup;
      TKIniFile* AppiaSmpini = new TKIniFile(filename.c_str());
      AppiaSmpini->EraseSection(GSMPSESSION);
      for(TSmpMap::iterator iter_smp = SmpMap_->begin(); iter_smp != SmpMap_->end(); iter_smp++) {
         std::string SMGroup = iter_smp->first;
         AppiaSmpini->WriteString(GSMPSESSION, SMGroup, iter_smp->second);
      }
      delete AppiaSmpini;
      delete SmpMap_;
   }
         
         
         
/*   
typedef std::string                              sAppiaGroup;
typedef std::string                              sSMGroup;
typedef std::string                              sSessionID;
typedef TDMapValue<sSessionID>                   TSessionID;   //第2層的 Value
typedef TDMap<sAppiaGroup, sSMGroup, TSessionID> TSMList;      //第2層的 Map
typedef Kstl::ev_map<sAppiaGroup, TSMList>       TAppiaSMList; //第1層的 Map

   if(F05Map_->size() <= 0)
      return;

   WriteMsg(GXFT, "Start to Configure appia-smp.ini.xxx.");
   SKey4Prod  key;   
   SF05Data2  value;
   TAppiaSMList* AppiaSMList_ = new TAppiaSMList();
  
   for(TF05Map::iterator iter_f05=F05Map_->begin(); iter_f05!=F05Map_->end(); ++iter_f05) {
      key   = iter_f05->first;
      value = iter_f05->second;
      TAppiaSMList::iterator iter = AppiaSMList_->fetch(value.AppiaGroup_.as_string());
      TSMList::iterator L = iter->second.fetch(value.SMGroup_.as_string());
      sSessionID SessionIDs = L->second;
      if(SessionIDs.empty()) 
         SessionIDs = key.GetSesID();
      else
         SessionIDs = SessionIDs + ":" + key.GetSesID();
      TSMList::updater u = iter->second.begin_update(L);
      u->assign(SessionIDs);
   }
   
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*  iniFile = new TKIniFile(gTmpPath.c_str());
   std::string nextDate = iniFile->ReadString(GXFT, GNextDate, "");
   delete  iniFile;
   
   for(TAppiaSMList::iterator iter = AppiaSMList_->begin(); iter != AppiaSMList_->end(); iter++) {
      std::string filename = GetAppiaPath() + GAppiaSmpIni + "." + iter->first;
      TKIniFile* AppiaSmpini = new TKIniFile(filename.c_str());
      for(TSMList::iterator L = iter->second.begin(); L != iter->second.end(); L++) {
         std::string SMGroup = L->first;
         AppiaSmpini->WriteString(GSMPSESSION, SMGroup, L->second);
      }
      delete AppiaSmpini;
   }    
   delete AppiaSMList_;
*/   
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ConfigFixVer(TKIniFile& main)
{
   char buf[256] = {0};
   TStrings  AGs, Sess;
   std::string SID;
   main.ReadStrings(GMain, GAppiaGroup, AGs);
   
   SKey4Prod  key4F05;
   key4F05.Type_.assign("N");

   // -->讀取每天早上灌單測試用的Fix SesIDs
   Kway::Tw::Stk::TMarket  procMkt = GetProcMkt();
   std::string  mkt = procMkt.as_string();
   TStrings  Testbrks, TestsesIDs, tmpIDs;
   bool result;
   main.ReadStrings(GMain, "TestBroker", Testbrks);   
   for(TStrings::iterator iter_brks = Testbrks.begin(); iter_brks != Testbrks.end(); ++iter_brks) {
      std::string  mktBrk = mkt + (*iter_brks);
      main.ReadStrings(mktBrk, GFixSes, tmpIDs);
      for(TStrings::iterator iter_tmp = tmpIDs.begin(); iter_tmp != tmpIDs.end(); ++iter_tmp) {
         result = false;
         for(TStrings::iterator iter_IDs = TestsesIDs.begin(); iter_IDs != TestsesIDs.end(); ++iter_IDs) {
            if(*iter_tmp == *iter_IDs){
               result = true;
               break;
            }
         }
         if(result == false) {
            TestsesIDs.push_back(*iter_tmp);
         }
      }       
   }
   // <--讀取每天早上灌單測試用的Fix SesIDs

   for(TStrings::iterator iterAGs = AGs.begin(); iterAGs != AGs.end(); ++iterAGs) {
      std::string AppiaGroup = *iterAGs;
      if(main.ReadStrings(GSIDAG, AppiaGroup, Sess) <= 0) 
         continue;
         
      std::string appia = GetAppiaPath() + GAppiaIni + "." + AppiaGroup;  //設定Appia.ini
      TKIniFile* appiaIni = new TKIniFile(appia.c_str());
      TStrings connS;
      appiaIni->ReadStrings(GAppiaMain, GAConnection, connS);
      for(TStrings::iterator iterSess = Sess.begin(); iterSess != Sess.end(); ++iterSess) {
         SID = *iterSess;
         // -->排除每天早上灌單測試用的Fix SesIDs
         result = false;
         for(TStrings::iterator iter_IDs = TestsesIDs.begin(); iter_IDs != TestsesIDs.end(); ++iter_IDs) {
            if(SID == *iter_IDs){
               result = true;
               break;
            }
         }
         if(result == true) {
            continue;
         }
         // <--排除每天早上灌單測試用的Fix SesIDs
         key4F05.SesID_.Market_.assign(SID.substr(0, 1));
         key4F05.SesID_.Broker_.assign(SID.substr(1, 4));
         key4F05.SesID_.PVCID_.assign(SID.substr(5, 2));
         TF05Map::iterator  iter_f05 = F05Map_->find(key4F05);
         if(iter_f05 == F05Map_->end()) {
            sprintf(buf, "F05查無 %s%s%s 的資料", key4F05.SesID_.Market_.as_string().c_str(), key4F05.SesID_.Broker_.as_string().c_str(), key4F05.SesID_.PVCID_.as_string().c_str());
            if(Mgr_ = GetExcMgr()) //找不到對應之F05資料
               Mgr_->Alert("XF1013", "Prod", "");
            WriteMsg(GXFT, buf);
            continue;
         }         
         std::string tmp1, tmp2;
         std::string  src, ver;
         tmp1 = ConvertTand2Appia(SID);  //轉換原的FixSesID to Appia
         tmp2 = "[" + tmp1 + "]";
         TStrings::iterator iter_cat = std::find(connS.begin(), connS.end(), tmp2);
         if(iter_cat == connS.end()) {
            //Alert錯誤訊息
            if(Mgr_ = GetExcMgr()) //twse.ini內查無FixSesID的資料
               Mgr_->Alert("XF1013", "Prod", "");
            sprintf(buf, "%s 內查無 %s 的資料", appia.c_str(), tmp2.c_str());
            WriteMsg(GXFT, buf);
         }
         appiaIni->WriteStrings(GAppiaMain, GAConnection, tmp2);
         src = iter_f05->second.FixVersion_.as_string();
         if(GetAppiaFixVerString(src, ver) == false) {
            if(Mgr_ = GetExcMgr()) {
               sprintf(buf, "設定Appia失敗, appK=%s, F05-FIX-VERSION錯誤", tmp1.c_str());
               Mgr_->Alert("XF1006", "Prod", buf);
               sprintf(buf, "FIX version error, appK=%s, recvVer=%s", tmp1.c_str(), src.c_str());
               WriteMsg(GXFT, buf);
            }
            continue;
         }
         appiaIni->WriteString(tmp1, GConFixVer, ver);
      }
      delete appiaIni;
   }
}
//---------------------------------------------------------------------------
K_mf(bool)  TwseFilesHandler::ConfigThrottle(TKIniFile& main)
{
   char buf[4096] = {0};
   WriteMsg(GXFT, "Start to Configure ef-config.xml.");
   //建立流量控管檔案ef-config.xml以及throttle.xml  
   
   TSesIDsMap  map; //SesID--Number of PVC --> T140001--5
   TSesIDsMap::iterator  iter;
   TStrings  AGs, Sess;
   std::string SID;
   main.ReadStrings(GMain, GAppiaGroup, AGs);
   std::string appiaHomePath = GetAppiaHomePath();
   std::string appiaPath     = GetAppiaPath();
   std::string throttlePath;
   
// 雅萍通知,交易所取消F12後,流量檔資料來源改由F03新增及異動
// 20130506 雅萍通知流量檔再度由F03改到F05      
   SKey4Prod  key4F05;
   key4F05.Type_.assign("N");

   // -->讀取每天早上灌單測試用的Fix SesIDs
   Kway::Tw::Stk::TMarket  procMkt = GetProcMkt();
   std::string  mkt = procMkt.as_string();
   TStrings  Testbrks, TestsesIDs, tmpIDs;
   bool result;
   main.ReadStrings(GMain, "TestBroker", Testbrks);   
   for(TStrings::iterator iter_brks = Testbrks.begin(); iter_brks != Testbrks.end(); ++iter_brks) {
      std::string  mktBrk = mkt + (*iter_brks);
      main.ReadStrings(mktBrk, GFixSes, tmpIDs);
      for(TStrings::iterator iter_tmp = tmpIDs.begin(); iter_tmp != tmpIDs.end(); ++iter_tmp) {
         result = false;
         for(TStrings::iterator iter_IDs = TestsesIDs.begin(); iter_IDs != TestsesIDs.end(); ++iter_IDs) {
            if(*iter_tmp == *iter_IDs){
               result = true;
               break;
            }
         }
         if(result == false) {
            TestsesIDs.push_back(*iter_tmp);
         }
      }       
   }
   // <--讀取每天早上灌單測試用的Fix SesIDs

   for(TStrings::iterator iterAGs = AGs.begin(); iterAGs != AGs.end(); ++iterAGs) {
      std::string AppiaGroup = *iterAGs;
      std::string FWGroup = main.ReadString(GAppiaFWGroup, AppiaGroup, "");

      main.ReadStrings(GSIDAG, AppiaGroup, Sess);

      map.clear();
//      for(TStrings::iterator iterSess = Sess.begin(); iterSess != Sess.end(); ++iterSess) {
//         SID = *iterSess;
//         int totalflow = main.ReadInt(SID, GTotalFlow, 0);
//         iter = map.find(SID);
//         if(iter == map.end()) {
//            TSesIDsMap::value_type  v(SID, totalflow);
//            map.insert(v);
//         } else {
//            iter->second = totalflow;
//         }
//      }
      for(TStrings::iterator iterSess = Sess.begin(); iterSess != Sess.end(); ++iterSess) {
         SID = *iterSess;
         // -->排除每天早上灌單測試用的Fix SesIDs
         result = false;
         for(TStrings::iterator iter_IDs = TestsesIDs.begin(); iter_IDs != TestsesIDs.end(); ++iter_IDs) {
            if(SID == *iter_IDs){
               result = true;
               break;
            }
         }
         if(result == true) {
            continue;
         }
         // <--排除每天早上灌單測試用的Fix SesIDs
         
         key4F05.SesID_.Market_.assign(SID.substr(0, 1));
         key4F05.SesID_.Broker_.assign(SID.substr(1, 4));
         key4F05.SesID_.PVCID_.assign(SID.substr(5, 2));
         TF05Map::iterator  iter_f05 = F05Map_->find(key4F05);
         if(iter_f05 == F05Map_->end()) {
            sprintf(buf, "F05查無 %s%s%s 的資料", key4F05.SesID_.Market_.as_string().c_str(), key4F05.SesID_.Broker_.as_string().c_str(), key4F05.SesID_.PVCID_.as_string().c_str());
            if(Mgr_ = GetExcMgr()) //找不到對應之F05資料
               Mgr_->Alert("XF1013", "Prod", "");
            WriteMsg(GXFT, buf);
            continue;
         }
         std::string str_tf = iter_f05->second.TotalFlow_.as_string();
         int totalflow = atoi(str_tf.c_str());
         iter = map.find(SID);
         if(iter == map.end()) {
            TSesIDsMap::value_type  v(SID, totalflow);
            map.insert(v);
         } else {
            iter->second = totalflow;
         }
         main.WriteString(SID, GTotalFlow, str_tf);
      }

      if(FWGroup.empty()) {
         // 理論上不會發生這種情況
         sprintf(buf, "查無與 %s 對應的 FWGroup", AppiaGroup.c_str());
         if(Mgr_ = GetExcMgr()) 
            Mgr_->Alert("XF1013", "Prod", buf);
         WriteMsg(GXFT, buf);
         throttlePath = appiaHomePath + "conf/" + AppiaGroup + "/throttle.xml";
      }
      else {
         throttlePath = appiaHomePath + "conf/" + FWGroup + "/" +AppiaGroup + "/throttle.xml";
      }

      std::string  ef_path = appiaPath + "ef-config.xml." + AppiaGroup;      
      
      std::string  sample_ef = GetConfigPath() + "ef-config.sample";
      std::string  thro_path = appiaPath + "throttle.xml." + AppiaGroup;
      sprintf(buf, "map_size=%d, ef=%s ; thro=%s", map.size(), ef_path.c_str(), thro_path.c_str());
      WriteMsg(GXFT, buf);
      //sprintf(buf, "檢查%s或%s", ef_path.c_str(), thro_path.c_str());
      sprintf(buf, "檢查%s", ef_path.c_str());
      TiXmlDocument  xml;
      if(xml.LoadFile(ef_path.c_str()) == false) {
         // 檔案不存在, 拷貝自範例檔
         TiXmlDocument  sample_xml;
         if(sample_xml.LoadFile(sample_ef.c_str()) == false) {
            //Alert
            if(Mgr_ = GetExcMgr())
               Mgr_->Alert("XF1014", "Prod", buf);
            WriteMsg(GXFT, buf);
            return false;
         }
         sample_xml.SaveFile(ef_path.c_str());
         xml.LoadFile(ef_path.c_str());         
      }
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
                           WriteMsg(GXFT, tmp.c_str());
                           xml.SaveFile(); //寫入ef-config.xml
                           sprintf(buf, "Finished %s configuration.", ef_path.c_str());
                           WriteMsg(GXFT, buf);
                           result = Throttling(map, thro_path); //寫入throttle.xml
                           if(result == false) {
                              return false;
                           }
                        } //for(param)
                     }
                  }
               } //for(plugin-filter)
               TiXmlElement*  args = plugin->FirstChildElement("plugin-args");
               if(args == NULL) {
                  args = new TiXmlElement("plugin-args");                  
               }
               TiXmlElement* param = args->FirstChildElement("param");
               if(param == NULL) {
                  param = new TiXmlElement("param");
               }
               for(param; param; param=param->NextSiblingElement("param")) {
                  if(param->Attribute("name") == NULL)
                     continue;
                  name = param->Attribute("name");
                  if(name != std::string("configuration"))
                      continue;
                  param->SetAttribute("value", throttlePath.c_str());
               }               
               xml.SaveFile(); //寫入ef-config.xml
            }
         }
      } //for(plugin)
   }
   //Alert
   if(result == true)
      return true;
   if(Mgr_ = GetExcMgr())
      Mgr_->Alert("XF1014", "Prod", buf);
   WriteMsg(GXFT, buf);
   return false;
}
//---------------------------------------------------------------------------
//K_mf(bool)  TwseFilesHandler::Throttling(TSesIDsMap& map, std::string& path, const char* buf)
K_mf(bool)  TwseFilesHandler::Throttling(TSesIDsMap& map, std::string& thro_path)
{
   /*
   <throttle-plugin>
      <session name="T1160">
         <inbound tps="3000"/>
         <outbound tps="3000"/>
      </session>
   </throttle-plugin>
   */
   char buf[256] = {0};
   sprintf(buf, "Start to Configure %s", thro_path.c_str());
   WriteMsg(GXFT, buf);
   Kway::Tw::Stk::TMarket  mkt = GetProcMkt();
   std::string  mktS = mkt.as_string();
   std::string  appiaKey, tandKey;
   std::string  path = GetAppiaPath();
   std::string  sample_thro = GetConfigPath() + "throttle.sample";
   sprintf(buf, "檢查%s", thro_path.c_str());
   //設定throttle.xml 
   TiXmlDocument  xml;
   if(xml.LoadFile(thro_path.c_str()) == false) {
      // 檔案不存在, 拷貝自範例檔
      TiXmlDocument  sample_xml;
      if(sample_xml.LoadFile(sample_thro.c_str()) == false) {
         //Alert
         if(Mgr_ = GetExcMgr())
            Mgr_->Alert("XF1014", "Prod", buf);
         WriteMsg(GXFT, buf);
         return false;
      }
      sample_xml.SaveFile(thro_path.c_str());
      xml.LoadFile(thro_path.c_str());
   }
   
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
      sprintf(buf, "1.Finished %s configuration.", thro_path.c_str());
      WriteMsg(GXFT, buf);
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
   
   sprintf(buf, "2.Finished %s configuration.", thro_path.c_str());
   WriteMsg(GXFT, buf);
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
K_mf(void)  TwseFilesHandler::OnF10Timer(Kway::TTimerBase* Sender)
{
   StopF10Timer();
   if(GetAppiaLocked()==false && GetF11Done())// && GetF13Done())
      GenerateF10();
   else {
      WriteMsg(GXFT, "F10Timer-Appia IsLocked or F11 undo");
      StartF10Timer(30);
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
      sprintf(buf, "F06-WK-CODE=%i, 超過設定時間=%s,未收到", i, iter->second.CheckTime_.as_string().c_str());
      u->Memo_.assign(buf);
      if(Mgr_ = GetExcMgr())
         Mgr_->Alert("XF1050", buf);
      WriteMsg(GXFT, buf);
   }
   StartWKTimer(30);
}
*/
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::StartF11Timer(size_t sec)
{ 
   F11Timer_.Start(sec*1000, &this_type::OnF11Timer);
   WriteMsg(GXFT, "F11 Timer Start!");
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::StopF11Timer()
{ 
   F11Timer_.Stop();
   WriteMsg(GXFT, "F11 Timer Stop!");
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::OnF11Timer(Kway::TTimerBase* Sender)
{
   StopF11Timer();

   if(GetAppiaLocked() == false) {
      ConfigF11();
   } else {
      WriteMsg(GXFT, "twse.ini IsLocked=true!");
      StartF11Timer(30);
   }
}
//---------------------------------------------------------------------------
/*
K_mf(void)  TwseFilesHandler::StartF13Timer(size_t sec)
{ 
   F13Timer_.Start(sec*1000, &this_type::OnF13Timer);
   WriteMsg(GXFT, "F13 Timer Start!");
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::StopF13Timer()
{ 
   F13Timer_.Stop();
   WriteMsg(GXFT, "F13 Timer Stop!");
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::OnF13Timer(Kway::TTimerBase* Sender)
{
   StopF13Timer();

   if(GetAppiaLocked() == false) {
      ConfigF13();
   } else {
      WriteMsg(GXFT, "twse.ini IsLocked=true!");
      StartF13Timer(30);
   }
}
*/
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
   std::string  msg = TKTime::Now().as_string() + ":" + "收到F06-WK-CODE";
   u->Memo_.assign(msg);
}
*/
//---------------------------------------------------------------------------
#include "FTJobDef.h"
#include "FTCenter.h"
#include "bxSesMgr.h"
#include "FTJobMap2d.h"
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::GenerateF10()
{
   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert("XF1000", "F10", "F10檔案上傳通知");
   }
   WriteMsg(GXFT, "Start to upload F10.");
   char  buf[256] = {0};
   gTmpPath = GetTmpPath() + GMainIni;
   TKIniFile*   iniFile = new TKIniFile(gTmpPath.c_str());
   gAppiaPath = GetAppiaPath() + GAppiaIni;
   std::string fwpasswords = GetAppiaPath() + GPassWordsIni;
   TKIniFile* passwordsIni = new TKIniFile(fwpasswords.c_str());
   //檢查更新Fix版本
   ConfigFixVer(*iniFile);
   
   //設定流量控管檔案ef-config.xml以及throttle.xml
   bool  boolV = ConfigThrottle(*iniFile);
   iniFile->Flush();
   passwordsIni->Flush();
   
   if(boolV == false) {
      if(Mgr_ = GetExcMgr()) {
         Mgr_->Alert("XF1099", "正式環境", "流量控管設定失敗(ef-config.xml/throttle.xml)");
      }
      WriteMsg(GXFT, "PROD-流量控管設定失敗(ef-config.xml/throttle.xml)");
   }
   std::string  Brk = iniFile->ReadString(GXFT, GBroker, "000F");
   
   SF10Data1  header;
   header.Date_.assign(Kway::TKDate::Now().as_string(TKDate::sf_ELong, 0));
  
   TKIniFileMap* appiaIniMap = new TKIniFileMap();
   TKIniFileMap* smpIniMap = new TKIniFileMap();
   std::vector<SF10Data2>  vF10Data2;
   vF10Data2.clear();
   SF10Data2    data;

   TStrings     Brks, Sess;
   std::string  BrkMkt;
   std::string  appia    = GetAppiaPath() + GAppiaIni;
   std::string  appiasmp = GetAppiaPath() + GAppiaSmpIni;
   Kway::Tw::Stk::TMarket  procMkt = GetProcMkt();
   std::string  market = procMkt.as_string();
   int  BrkCnt = iniFile->ReadStrings(GMain, GBroker, Brks);
   
   if(BrkCnt > 0) { // 避免無資料進for出現未預期的錯誤
      for(int L = 0; L < BrkCnt; ++L) {
         data.Clear();
         BrkMkt = market + Brks[L]; //T1400
         int SesCnt = iniFile->ReadStrings(BrkMkt, GFixSes, Sess);
         if(SesCnt <= 0) // 避免無資料進for出現未預期的錯誤
            continue;
         for(int i=0; i<SesCnt; ++i) { //Sess[i] --> T1400X1
            std::string appiaGroup = iniFile->ReadString(Sess[i], GAppiaGroup, "");
            if(appiaGroup.empty())
               continue;
            TKIniFile* appiaIni = GetIniFile(appiaIniMap, appiaGroup, appia);
            TKIniFile* AppiaSmpini = GetIniFile(smpIniMap, appiaGroup, appiasmp);
            sprintf(buf, "FixSes = %s Path, T=%s, A=%s S=%s", Sess[i].c_str(), iniFile->GetFilePath().c_str(), 
               appiaIni->GetFilePath().c_str(), AppiaSmpini->GetFilePath().c_str());
            WriteMsg(GXFT, buf);
            appiaIni->Flush();
            AppiaSmpini->Flush();

            std::string  appiaSID = ConvertTand2Appia(Sess[i]);
            data.PVCName_.assign(iniFile->ReadString(Sess[i], GTermID, ""));
            data.Broker_.assign(Sess[i].substr(1, 4));
            data.APCode_.assign(iniFile->ReadString(Sess[i], GAPCode, " "));
            data.PVCID_.assign(Sess[i].substr(5, 2));
            data.Passwd_.assign(passwordsIni->ReadString(GPASSWORDS, Sess[i], ""));
            data.FWGroup_.assign(iniFile->ReadString(Sess[i], GFWGroup, ""));
            data.AppiaGroup_.assign(iniFile->ReadString(Sess[i], GAppiaGroup, ""));
         
            char tmpKey[3] = {0};
            std::string tmpStr;
            data.SMGroup_.assign("  ");
            for(int k = 1; k <= 10; k++) {
               sprintf(tmpKey, "%02d", k);
               tmpStr = AppiaSmpini->ReadString(GSMPSESSION, tmpKey, "");
               if(tmpStr.size() == 0) {
                  continue;
               }
               if(tmpStr.find(Sess[i]) != std::string::npos) {
                  data.SMGroup_.assign(tmpKey);
                  break;
               }
            }           
            data.Type_.assign("N");
            tmpStr = appiaIni->ReadString(appiaSID, GConFixVer, "");
            data.FixVersion_.assign(GetTwseFixVerCode(tmpStr));
            TStrings IPs;
            appiaIni->ReadStrings(appiaSID, GNetAddress, IPs);
            if(IPs.size() >= 1) {
               data.IP1_.assign(FillIPString(IPs[0]));
            }
            else{
               data.IP1_.assign("               ");
            }
            if(IPs.size() >= 2) {
               data.IP2_.assign(FillIPString(IPs[1]));
            }
            else{
               data.IP2_.assign("               ");
            }
            data.SrcPort_.assign(appiaIni->ReadString(appiaSID, GRemotePort, "")); 
            data.DestIP_.assign(iniFile->ReadString(Sess[i], GIP, ""));
            data.ListPort1_.assign(iniFile->ReadString(Sess[i], GListPort1, ""));
            data.ListPort2_.assign(iniFile->ReadString(Sess[i], GListPort2, ""));
            data.TotalFlow_.assign(iniFile->ReadString(Sess[i], GTotalFlow, "0"));
            vF10Data2.push_back(data);
         } //for(int i=0;
      } //for(int L = 0
   } //if(BrkCnt > 0)
   for(TKIniFileMap::iterator iter = appiaIniMap->begin(); iter != appiaIniMap->end(); iter++){
      delete iter->second;
      iter->second = NULL;
   }
   delete appiaIniMap;
   for(TKIniFileMap::iterator iter = smpIniMap->begin(); iter != smpIniMap->end(); iter++){
      delete iter->second;
      iter->second = NULL;
   }
   delete smpIniMap;
   delete passwordsIni;
   delete iniFile;
   

/* 以下產生F10的方式會連測試線路的資料一起寫入, 這是不對的, 所以改掉
   TStrings     agS, connS;
   SF10Data2    data;
   int  AGCnt = iniFile->ReadStrings(GMain, GAppiaGroup, agS);
   for(int i = 0; i < AGCnt; i++) {      
      std::string  appia = GetAppiaPath() + GAppiaIni + "." + agS[i];
      TKIniFile* appiaIni = new TKIniFile(appia.c_str());
      std::string appiasmp = GetAppiaPath() + GAppiaSmpIni + "." + agS[i];
      TKIniFile* AppiaSmpini = new TKIniFile(appiasmp.c_str());
      sprintf(buf, "Paht, T=%s, A=%s S=%s", iniFile->GetFilePath().c_str(), 
         appiaIni->GetFilePath().c_str(), AppiaSmpini->GetFilePath().c_str());
      WriteMsg(GXFT, buf);
      appiaIni->Flush();
      AppiaSmpini->Flush();
      
      int connCnt = appiaIni->ReadStrings(GAppiaMain, GAConnection, connS);
      for(int j = 0; j < connCnt; j++) {
         if(connS[j].size() < 15) {
            continue;
         }
         std::string appiaKey  = connS[j].substr(1, 13);
         std::string SessionID = ConvertAppia2Tand(appiaKey);
         data.PVCName_.assign(iniFile->ReadString(SessionID, GTermID, ""));
         data.Broker_.assign(SessionID.substr(1, 4));
         data.APCode_.assign(" ");
         data.PVCID_.assign(SessionID.substr(5, 2));
         data.Passwd_.assign(passwordsIni->ReadString(GPASSWORDS, SessionID, ""));
         data.FWGroup_.assign(iniFile->ReadString(SessionID, GFWGroup, ""));
         data.AppiaGroup_.assign(iniFile->ReadString(SessionID, GAppiaGroup, "     "));
         
         char tmpKey[3] = {0};
         std::string tmpStr;
         data.SMGroup_.assign("  ");
         for(int k = 0; k <= 10; k++) {
            sprintf(tmpKey, "%02d", k);
            tmpStr = AppiaSmpini->ReadString(GSMPSESSION, tmpKey, "");
            if(tmpStr.size() == 0) {
               continue;
            }
            if(tmpStr.find(SessionID) != std::string::npos) {
               data.SMGroup_.assign(tmpKey);
               break;
            }
         }           
         data.Type_.assign("N");
         tmpStr = appiaIni->ReadString(appiaKey, GConFixVer, "");
         data.FixVersion_.assign(GetTwseFixVerCode(tmpStr));
         TStrings IPs;
         appiaIni->ReadStrings(appiaKey, GNetAddress, IPs);
         if(IPs.size() >= 1) {
            data.IP1_.assign(FillIPString(IPs[0]));
         }
         else{
            data.IP1_.assign("");
         }
         if(IPs.size() >= 2) {
            data.IP2_.assign(FillIPString(IPs[1]));
         }
         else{
            data.IP2_.assign("");
         }
         data.SrcPort_.assign(appiaIni->ReadString(appiaKey, GRemotePort, "")); 
         data.DestIP_.assign(iniFile->ReadString(SessionID, GIP, ""));
         data.ListPort1_.assign(iniFile->ReadString(SessionID, GListPort1, ""));
         data.ListPort2_.assign(iniFile->ReadString(SessionID, GListPort2, ""));
         data.TotalFlow_.assign(iniFile->ReadString(SessionID, GTotalFlow, ""));
         vF10Data2.push_back(data);
      }
      delete appiaIni;
      delete AppiaSmpini;
   }
   delete iniFile;
*/   
   //寫入F10
   std::string  f10;
   std::string  fn = GetProcLogPath();//GetLogPath();
//   Kway::Tw::Stk::TMarket  procMkt = GetProcMkt();
//   std::string market = procMkt.as_string();
   fn += "F10." + market;   // $home/Twse/Logs/yymmdd/XFT/F10.T (O)
   TFile*  file = new TFile(fn.c_str(), TFileMode(fmReadWrite|fmCreatePath|fmTrunc));
   std::string  content = header.ToString();
   file->Seek(0, fsBegin);
   file->Write(content.c_str()); //Write header

// --> 20180806 add by k288 for 測試線路與正式線路發生重覆
   bool result = true;
   if(vF10Data2.empty() == false) {
      std::vector<SF10Data2>::iterator  iter = vF10Data2.begin();
      for(; iter!=vF10Data2.end(); ++iter) {
         if(file->IsOpened() == false)
            file->Open(fn.c_str(), TFileMode(fmReadWrite|fmCreatePath|fmTrunc));
         f10 = (*iter).ToString();
         file->Write(f10.c_str());
         
         SKey4Prod  key4F05;
         key4F05.Type_ = iter->Type_;
         key4F05.SesID_.Market_.assign(market);
         key4F05.SesID_.Broker_.assign(iter->Broker_.as_string());
         key4F05.SesID_.PVCID_ = iter->PVCID_;

         TF05Map::iterator iter_f05 = F05Map_->find(key4F05);
         if(iter_f05 == F05Map_->end())
         {
            result = false;
            sprintf(buf, "FixSes = %s ,在F05中找不到對應的資料", key4F05.SesID_.as_string().c_str());
            if(Mgr_ = GetExcMgr()) {
               Mgr_->Alert("XF1051", "F10", buf);
            }
            WriteMsg(GXFT, buf);
            continue;
         }
         if(iter_f05->second.ToString() != f10)
         {
            result = false;
            sprintf(buf, "FixSes = %s ,在F10與F05資料不相同", key4F05.SesID_.as_string().c_str());
            if(Mgr_ = GetExcMgr()) {
               Mgr_->Alert("XF1052", "F10", buf);
            }
            WriteMsg(GXFT, buf);
            continue;
         }
      }
   }

   //上傳F10
   if(result)
   {
      using namespace Kway::Tw::Bx::FT;
      TFTNewJob  job;
      job.SeqNo_.assign0();
      job.SourceID_.assign(Brk);
      job.DestID_ = TSEID_;
      job.Market_ = procMkt;
      job.FileCode_.assign("F10");
      job.Type_ = ftt_File;
      job.Mode_ = ftm_Report;
      job.SrcData_.assign(std::string("F10.")+market);
      job.RecLen_ = file->GetFileSize();
      job.CreateTime_ = Kway::TKTime::Now(); 
      
      FTSender_->FileUpload(job);
   
      if(Mgr_ = GetExcMgr()) {
         Mgr_->Alert("XF1100", "F10", "F10檔案上傳完成");
      }
   }
   delete file;   
   WriteMsg(GXFT, "End of F10 upload.");
/*
   if(vF10Data2.empty() == false) {
      std::vector<SF10Data2>::iterator  iter = vF10Data2.begin();
      for(; iter!=vF10Data2.end(); ++iter) {
         if(file->IsOpened() == false)
            file->Open(fn.c_str(), TFileMode(fmReadWrite|fmCreatePath|fmTrunc));
         f10 = (*iter).ToString();
         file->Write(f10.c_str());
      }
   }

   //上傳F10
   using namespace Kway::Tw::Bx::FT;
   TFTNewJob  job;
   job.SeqNo_.assign0();
   job.SourceID_.assign(Brk);
   job.DestID_ = TSEID_;
   job.Market_ = procMkt;
   job.FileCode_.assign("F10");
   job.Type_ = ftt_File;
   job.Mode_ = ftm_Report;
   job.SrcData_.assign(std::string("F10.")+market);
   job.RecLen_ = file->GetFileSize();
   job.CreateTime_ = Kway::TKTime::Now(); 

   FTSender_->FileUpload(job);
   delete file;   
   
   if(Mgr_ = GetExcMgr()) {
      Mgr_->Alert("XF1100", "F10", "F10檔案上傳完成");
   }
   WriteMsg(GXFT, "End of F10 upload.");
*/   
// <-- 20180806 add by k288 for 測試線路與正式線路發生重覆
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
