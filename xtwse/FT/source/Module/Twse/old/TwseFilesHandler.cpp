/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseFilesHandler.h"
#include "ExtDef.h"
#include "KTime.h"
//---------------------------------------------------------------------------
using namespace Kway;
//using namespace Kway::Tw::Bx::FT;
using namespace Kway::Tw::Stk;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
K_ctor  TwseFilesHandler::TwseFilesHandler() 
   : Timer_(this)
{
   std::string  confPath = GetConfPath();
   TKIniFile*  iniFile = new TKIniFile(std::string(confPath+GMainIni).c_str());
   IsK05Ready_ = iniFile->ReadString(GMain, GK05Ready, "");
      
   std::string  fn = confPath + "K03.bin";
   K03Map_ = new TK03Map();
   K03Recorder_ = new TK03Recorder(*K03Map_, fn.c_str());
   
   fprintf(stderr, "K03Map size=%d\n", K03Map_->size());

   K05Map_ = new TK05Map();
   
   TKDate  date;
   date.assign(IsK05Ready_);
   if(date == TKDate::Now()) { //避免程式crash後,K05Map_消失
     RebuildK05Map(*iniFile);
   }

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
   
   delete iniFile;
}
//---------------------------------------------------------------------------
K_dtor  TwseFilesHandler::~TwseFilesHandler()
{
   if(SesIDsMap_)     delete SesIDsMap_;
   if(PortsMap_)      delete PortsMap_;
   if(K03Recorder_)   delete K03Recorder_;
   if(K03Map_)        delete K03Map_;
   if(K05Map_)        delete K05Map_;
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::StartHandleAFile(TMarket& mkt, std::string& fname, std::string& path)
{   
   //std::string  fullFN = path + "/" + fname;
   TFile*  file = new TFile(path.c_str(), TFileMode(fmRead));

   TStringV  v;
   v.clear();
   
   TFileType  type;
   type.assign(fname);
   
   fprintf(stderr, "file-size=%d\n", file->GetFileSize());
   if(file->IsOpened()==false || file->GetFileSize()<=0 || Read2Container(file, type, v)==false) {
      delete file;
      return;
   }
   
   fprintf(stderr, "file-size=%d\n", file->GetFileSize());
   
   switch(static_cast<EFileType>(type)) {
      case ft_K01:   ProcessK01(mkt, type, v);   break;
      case ft_K02:   break;
      case ft_K03:   ProcessK03(mkt, type, v);   break;
      case ft_K04:   ProcessK04(mkt, type, v);   break;
      case ft_K05:   ProcessK05(mkt, type, v, path);   break;
      case ft_K06:   ProcessK06(mkt, type, v);   break;
      case ft_K07:   ProcessK07(mkt, type, v);   break;
      case ft_K08:   ProcessK08(mkt, type, v);   break;
   }
   delete file;
}
//---------------------------------------------------------------------------
K_mf(bool)  TwseFilesHandler::Read2Container(TFile* file, TFileType& type, TStringV& v)
{
   fprintf(stderr, "FileLen=%d\n", file->GetFileSize());
   //std::string  buffer;
   int  i     = 1;
   int  size1 = type.GetDataSize(1);
   int  size2 = type.GetDataSize();
   fprintf(stderr, "Read2Container----datasize=%d\n", type.GetDataSize());
   char buffer[std::max(size1,size2)+1];
   
   file->Seek(0, fsBegin);
      fprintf(stderr, "FileLen=%d\n", file->GetFileSize());
   while(file->IsEOF() == false) {
      fprintf(stderr, "CurPos=%d", file->GetCurPos());
      fprintf(stderr, "FileLen=%d\n", file->GetFileSize());
     if(i == 1) {
        fprintf(stderr, "Read2Container----5.1\n");
         if(file->Read(buffer, size1) == size1) {
            fprintf(stderr, "Read2Container----5.1.1   buffer=%s\n", buffer);
              v.push_back(buffer);
            fprintf(stderr, "Read2Container----5.1.2\n");
       }
       ++i;
     } else { 
         fprintf(stderr, "Read2Container size2=%d----5.2\n", size2);
           if(file->Read(buffer, size2) == size2) {
            fprintf(stderr, "Read2Container----5.2.1, buffer=%s\n", buffer);
              v.push_back(buffer);
       }
      }
   }
   
   fprintf(stderr, "Read2Container----6, size=%d\n", v.size());
   return v.size() > 1;
   //return true;
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessK01(TMarket& mkt, TFileType& type, TStringV& v)
{
   int  i = 0;
   SK01Data2  data;  
   
   std::vector<SK01Data2>  vK01Data2;
   vK01Data2.clear();
   
   std::string  applyDate;
   for(TStringVI  iter=v.begin(); iter!=v.end(); ++iter) {
      if(i == 0) { //Data1不處理
        ++i;
       applyDate = (*iter).substr(0,8);
       continue;
     }
fprintf(stderr, "ProcessK01--->size=%d\n", type.GetDataSize());
fprintf(stderr, "source=%s\n", (*iter).c_str());
     memcpy(&data, (*iter).c_str(), type.GetDataSize());
     vK01Data2.push_back(data);
   } 
   //讀取Main.ini,取得各路徑(Appia.ini, XTester.ini)
   std::string  confPath = GetConfPath();
fprintf(stderr, "ProcessK01--->confPath=%s\n", confPath.c_str());
   TKIniFile*  iniFile = new TKIniFile(std::string(confPath+GMainIni).c_str());
   std::string  xtwse = iniFile->ReadString(GMain, GTesterPath, confPath);
   std::string  appia = iniFile->ReadString(GMain, GAppiaPath, confPath);
   xtwse += "/XTester.ini"; //設定XTester.ini
   appia += "/twse.ini";  //設定Appia.ini
   
   TKIniFile*  xtwseIni = new TKIniFile(xtwse.c_str());
   TKIniFile*  appiaIni = new TKIniFile(appia.c_str());
   
   std::vector<SK01Data2>::iterator  iter = vK01Data2.begin();
   for(; iter!=vK01Data2.end(); ++iter) {
      SetK01IniFile(*iter, *iniFile, *xtwseIni, *appiaIni, applyDate);
   }
   
   delete  appiaIni;
   delete  xtwseIni;
   delete  iniFile;
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
       if(date != today)
          return;
       nextDate = (*iter).substr(8, 8);
       continue;
     }
     memcpy(&data, (*iter).c_str(), type.GetDataSize());
     //*data = reinterpret_cast<SK04Data2>(*iter);
     vK04Data2.push_back(data);
   } 
   //讀取Main.ini,取得路徑(TradeSegment.ini)
   std::string  confPath = GetConfPath();
   TKIniFile*  iniFile  = new TKIniFile(std::string(confPath + GMainIni).c_str());
   TKIniFile*  tradeIni = new TKIniFile(std::string(confPath + GTradeSegIni).c_str());
   
   //寫入nextDate to Main.ini
   iniFile->WriteString(GMain, GNextDate, nextDate);
   
   std::vector<SK04Data2>::iterator  iter = vK04Data2.begin();
   for(; iter!=vK04Data2.end(); ++iter) {
      SetK04IniFile(*iter, *tradeIni, mkt);
   }
   
   delete  tradeIni;
   delete  iniFile;
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessK03(TMarket& mkt, TFileType& type, TStringV& v)
{
   //K03收到比對是否有K05,再比對K05是否有存在,不存在則先保留,若存在則找K04的next date是否與生效日同,相同則設定
   int  i = 0;
   SK03Data2  data;  

   std::vector<SK03Data2>  vK03Data2;
   vK03Data2.clear();

   std::string  applyDate;
   TKDate  today = TKDate::Now();
   for(TStringVI  iter=v.begin(); iter!=v.end(); ++iter) {
      if(i == 0) { //Data1不處理
         ++i;
         applyDate = (*iter).substr(0,8);
         TKDate  date;
         date.assign(applyDate);
         if(date != today)
            return;
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      //*data = reinterpret_cast<SK03Data2>(*iter);
      vK03Data2.push_back(data);
   } 

   //讀取Main.ini,取得路徑()
   std::string  confPath = GetConfPath();
   TKIniFile*   iniFile  = new TKIniFile(std::string(confPath+GMainIni).c_str());
   std::string  appia = iniFile->ReadString(GMain, GAppiaPath, confPath);
   appia += "/twse.ini";  //設定Appia.ini

   TKIniFile*  appiaIni = new TKIniFile(appia.c_str());
   
   std::vector<SK03Data2>::iterator  iter = vK03Data2.begin();
   for(; iter!=vK03Data2.end(); ++iter) {
      ConfigOrSave(*iter, *iniFile, *appiaIni);
   }   

   delete appiaIni;
   delete iniFile;
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessK05(TMarket& mkt, TFileType& type, TStringV& v, std::string& fullname)
{
   int  i = 0;
   SK05Data2  data;  

   std::vector<SK05Data2>  vK05Data2;
   vK05Data2.clear();

   std::string  applyDate;
   TKDate  today = TKDate::Now();
   for(TStringVI  iter=v.begin(); iter!=v.end(); ++iter) {
      if(i == 0) { //Data1不處理
         ++i;
         applyDate = (*iter).substr(0,8);
         TKDate  date;
         date.assign(applyDate);
         if(date != today)
            return;
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      //*data = reinterpret_cast<SK05Data2>(*iter);
      vK05Data2.push_back(data);
   } 
   
   //讀取Main.ini,取得路徑()
   std::string  confPath = GetConfPath();
   TKIniFile*   iniFile  = new TKIniFile(std::string(confPath+GMainIni).c_str());
   std::string  appia = iniFile->ReadString(GMain, GAppiaPath, confPath);
   appia += "/twse.ini";  //設定Appia.ini

   TKIniFile*  appiaIni = new TKIniFile(appia.c_str());
   
   std::vector<SK05Data2>::iterator  iter = vK05Data2.begin();
   for(; iter!=vK05Data2.end(); ++iter) {
      ConfigOrSave(mkt, *iter, *iniFile, *appiaIni);
   }   

   IsK05Ready_ = TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0);
   iniFile->WriteString(GMain, GK05Ready, IsK05Ready_);
   //iniFile->WriteString(GMain, mkt==m_Tse?GTK05Path:GOK05Path, path);
   iniFile->WriteString(GMain, mkt==m_Tse?GTK05File:GOK05File, fullname);
   delete appiaIni;
   delete iniFile;
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
         if(date != today)
            return;
         continue;
      }
      memcpy(&data, (*iter).c_str(), type.GetDataSize());
      //*data = reinterpret_cast<SK06Data2>(*iter);
      vK06Data2.push_back(data);
   } 
   
   //讀取Main.ini,取得路徑
   std::string  confPath = GetConfPath();
   TKIniFile*  iniFile  = new TKIniFile(std::string(confPath+GMainIni).c_str());
   
   std::vector<SK06Data2>::iterator  iter = vK06Data2.begin();
   for(; iter!=vK06Data2.end(); ++iter) {
      iniFile->WriteInt(GMain, GWorkCode, iter->WorkCode_.as_int());
   }
   delete iniFile;
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessK07(TMarket& mkt, TFileType& type, TStringV& v)
{

}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ProcessK08(TMarket& mkt, TFileType& type, TStringV& v)
{
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::SetK01IniFile(SK01Data2& k01, TKIniFile& main, TKIniFile& xtwse, TKIniFile& appia, std::string& applyDate)
{
   if(k01.Market_.empty() || (k01.Market_[0]!='T' && k01.Market_[0]!='O') || k01.Broker_.empty() || k01.TmpPVCID_.empty())
      return;
     
   std::string  tmp = k01.Broker_.as_string();
   if(tmp.length() < 4)
      return;

   std::string  key = k01.Market_.as_string() + k01.Broker_.as_string().substr(0,4) + k01.TmpPVCID_.as_string();
   fprintf(stderr, "key=%s-%s-%s\n", k01.Market_.as_string().c_str(), k01.Broker_.as_string().substr(0,4).c_str(), k01.TmpPVCID_.as_string().c_str());
   std::string  connS, key1;
   
   switch(k01.WhichType()) {
      case est_None:   return;
      case est_Appia:  
         connS = "[" + key + "]";
         //新增至[Main]
         appia.WriteString(GMain, GAConnection, connS);
         //新增至[middleware]
         appia.WriteStrings(GMidTester, GMidSesID, key);
         //新增至[connection]
         appia.WriteString(key, GConFixVer, GFIX42);
         appia.WriteString(key, GNetAddress, k01.BrokerIP_.as_string());
         appia.WriteString(key, GLocalFirmID, k01.Market_[0]!='T'?GTwse:GOtc);
         appia.WriteString(key, GBrokerFirmID, key);
         appia.WriteString(key, GHeartbeatInt, "30");
         appia.WriteString(key, GStartSeqNum, "1");
         appia.WriteString(key, GTradingSes, GTradingSesTB);
         //設定回覆K02需要的額外資訊
         xtwse.WriteStrings(GSummary, GSesID, key);
         xtwse.WriteString(key, GType, k01.Type_.as_string());
         xtwse.WriteString(key, GSeqNo, k01.ApplySeqNo_.as_string());
         //xtwse.WriteString(key, GAPCode, k01.APCode_.as_string());
         xtwse.WriteString(key, GPortName, k01.PortName_.as_string());
         xtwse.WriteString(key, GSourceIP, k01.BrokerIP_.as_string());
         xtwse.WriteString(key, GApplyDate, applyDate);
         xtwse.WriteString(key, GPwd, k01.Passwd_.as_string());
         xtwse.WriteString(key, GSPort, k01.PVCNo_.as_string());
         //寫入保留不足資訊給轉移Production用
         key1 = key + "_TMP";
         main.WriteString(key1, GNetAddress, k01.BrokerIP_.as_string());
         break;
      case est_Tmp:
         //tester設定必須項目
         xtwse.WriteStrings(GSummary, GSesID, key);
         xtwse.WriteString(key, GPwd, k01.Passwd_.as_string());
         xtwse.WriteString(key, GSPort, k01.PVCNo_.as_string());
         xtwse.WriteString(key, GType, k01.Type_.as_string());
         xtwse.WriteString(key, GAPCode, k01.APCode_.as_string());
         //寫入保留不足資訊給轉移Production用
         key1 = k01.Market_.as_string() + k01.Broker_.as_string().substr(0,4) + k01.FixPVCID_.as_string() + "_TMP";
         connS= k01.Market_.as_string() + k01.Broker_.as_string().substr(0,4) + k01.TmpPVCID_.as_string();
         main.WriteString(key1, connS, k01.PVCNo_.as_string()); //記錄Source Port以 Mkt+Brk+TmpPVCID當key
         break;
   }
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
K_mf(void)  TwseFilesHandler::ConfigOrSave(SK03Data2& k03, TKIniFile& main, TKIniFile& appia)
{
   if(k03.Market_.empty() || (k03.Market_[0]!='T' && k03.Market_[0]!='O') || k03.Broker_.empty() || 
      k03.TmpPVCID_.empty() || (k03.WorkCode_[0]!='I' && k03.WorkCode_[0]!='D'))
      return;
   
   std::string  tmp = k03.Broker_.as_string();
   if(tmp.length() < 4)
      return;
     
   SKey4Prod  keyProd;
   keyProd.Type_ = k03.Type_;
   keyProd.SesID_.Market_ = k03.Market_;
   keyProd.SesID_.Broker_.assign(k03.Broker_.as_string().substr(0,4));
   keyProd.SesID_.PVCID_ = k03.TmpPVCID_;//k03.WhichType()==est_Appia ? k03.TmpPVCID_ : k03.FixPVCID_;
   
   std::string  socketPortSesID = keyProd.SesID_.Market_.as_string() + keyProd.SesID_.Broker_.as_string() + (k03.WhichType()==est_Appia ? k03.TmpPVCID_.as_string() : k03.FixPVCID_.as_string());
   std::string  sessionID = keyProd.GetSesID();
   std::string  mbrkID    = keyProd.SesID_.Market_.as_string() + keyProd.SesID_.Broker_.as_string();

   if(k03.WorkCode_[0] == 'D') { //刪除直接執行,不需理會K05 or K04
      TK03Map::iterator  iter = K03Map_->find(keyProd);
      if(iter != K03Map_->end())
         K03Map_->erase(keyProd);
      //刪除Main.ini
      main.EraseSection(sessionID); //erase [sessionID] block
      main.EraseValue(mbrkID, GFixSes, sessionID); //erase [mbrkID]'s Fis
      //刪除appia.ini
      appia.EraseSection(sessionID); //erase connection block [sessionID]
      appia.EraseValue(GMain, GAConnection, sessionID); //erase connection in [Main]
      appia.EraseSection(std::string("m" + sessionID.substr(1, std::string::npos))); //erase middleware block
   } else { //新增需核對K05以及K04
      std::string  nextDate = main.ReadString(GMain, GNextDate, "");
      if(nextDate.empty())
         return;
      
      TKDate  next, effDate;
      next.assign(nextDate);
      effDate.assign(k03.EffDate_.as_string());
   
      if(next >= effDate) { //設定
         TK05Map::iterator  iter_k05 = K05Map_->find(keyProd);
         if(iter_k05 == K05Map_->end()) {
            fprintf(stderr, "InsertK03Map()--->1, key=%s-%s-%s-%s\n", keyProd.Type_.as_string().c_str(), keyProd.SesID_.Market_.as_string().c_str(), keyProd.SesID_.Broker_.as_string().c_str(), keyProd.SesID_.PVCID_.as_string().c_str());
			InsertK03Map(keyProd, k03);
            return;
      }
      fprintf(stderr, "iter_k05->Type_=%s, k03.Type_=%s, iter_k05->FixPVCID_=%s, k03.FixPVCID_=%s, iter_k05->PVCName_=%s, k03.PVCName_=%s, iter_k05->APCode_=%s, k03.APCode_=%s, iter_k05->TmpPVCID_=%s, k03.TmpPVCID_=%s\n",
	           iter_k05->second.Type_.as_string().c_str(), k03.Type_.as_string().c_str(), iter_k05->second.FixPVCID_.as_string().c_str(), k03.FixPVCID_.as_string().c_str(), 
			   iter_k05->second.PVCName_.as_string().c_str(), k03.PVCName_.as_string().c_str(), iter_k05->second.APCode_.as_string().c_str(), k03.APCode_.as_string().c_str(),
			   iter_k05->second.TmpPVCID_.as_string().c_str(), k03.TmpPVCID_.as_string().c_str());
	  if(iter_k05->second.Type_==k03.Type_ && iter_k05->second.FixPVCID_==k03.FixPVCID_ && iter_k05->second.TmpPVCID_==k03.TmpPVCID_) {
         //iter_k05->second.PVCName_==k03.PVCName_ && iter_k05->second.APCode_==k03.APCode_ && iter_k05->second.TmpPVCID_==k03.TmpPVCID_) {
         std::string  key = socketPortSesID + "_TMP";
         std::string  tmp = "[" + sessionID + "]";
         std::string  tmp1= main.ReadString(GMain, GMWBegPort, "12000");
		 fprintf(stderr, "key=%s, sessionID=%s, begPort=%s\n", key.c_str(), sessionID.c_str(), tmp1.c_str());
         std::string  mwsid;
         //寫入Main.ini, twse.ini 
         switch(k03.WhichType()) {
            case est_None:   
               //alert
               return;
            case est_Appia: 
               //新增至[Main]
               appia.WriteStrings(GMain, GAConnection, tmp);
               //新增至[middleware]
               mwsid = "m" + sessionID.substr(1, std::string::npos);
               appia.WriteStrings(mwsid, GMidSesID, sessionID);
               appia.WriteString(mwsid, GMidType, GSocket);
               appia.WriteString(mwsid, GSocketPort, GetSocketPort(tmp1, socketPortSesID)); //自行分配
               appia.WriteString(mwsid, GMidGlobalMsg, "OFF");
               appia.WriteString(mwsid, GMidMsgAckPayload, "OFF_POSITIVE_ACKS");
               appia.WriteString(mwsid, GMidSesEvent, "OFF");
               //新增至[connection]
               appia.WriteString(sessionID, GConFixVer, GFIX42); 
			   debug = main.ReadString(key, GNetAddress, "");
			   fprintf(stderr, "NetAddress=%s\n", debug.c_str());
               appia.WriteString(sessionID, GNetAddress, debug); //到暫存Block
               appia.WriteString(sessionID, GLocalFirmID, k03.Market_[0]!='T'?GTwse:GOtc);
               appia.WriteString(sessionID, GBrokerFirmID, sessionID);
               appia.WriteString(sessionID, GHeartbeatInt, "30");
               appia.WriteString(sessionID, GStartSeqNum, "1");
               appia.WriteString(sessionID, GTradingSes, GTradingSesB);
               //將FIX pwd寫入Main.ini
               main.WriteString(sessionID, GPwd, iter_k05->second.Passwd_.as_string());
               break;
            case est_Tmp: 
               tmp = keyProd.SesID_.Market_.as_string() + keyProd.SesID_.Broker_.as_string() + k03.FixPVCID_.as_string();
               main.WriteStrings(GMain, GBroker, keyProd.SesID_.Broker_.as_string());
			   if(k03.APCode_[0] == '3') { //成回線路設定 [T1099]
                  main.WriteStrings(mbrkID, GFixSes, tmp);
                  main.WriteString(mbrkID, GMatPwd, iter_k05->second.Passwd_.as_string());
                  main.WriteString(mbrkID, GMatSesID, k03.TmpPVCID_.as_string());
				  debug = main.ReadString(key, sessionID, "");
				  fprintf(stderr, "NetAddress=%s\n", debug.c_str());
                  main.WriteString(mbrkID, GMatSrcPort, debug);
               } else { //委託線路設定 
                  main.WriteString(tmp, GTMPID, k03.TmpPVCID_.as_string());
                  debug = GetSocketPort(tmp1, socketPortSesID);
				  fprintf(stderr, "tmp1=%s, socketPortSesID=%s, port=%s\n", tmp1.c_str(), socketPortSesID.c_str(), debug.c_str());
				  main.WriteString(tmp, GAppiaMWPort, debug); //自行分配
                  tmp1 = sessionID + "-" + k03.TmpPVCID_.as_string(); //[T109901-A1]
				  debug = main.ReadString(key, sessionID, "");
				  fprintf(stderr, "key=%s, sessionID=%s, srcport=%s\n", key.c_str(), sessionID.c_str(), debug.c_str());
                  main.WriteString(tmp1, GSPort, debug);
                  main.WriteString(tmp1, GPwd, iter_k05->second.Passwd_.as_string());
               }
               break;
            } //switch(...)
         } else {
            fprintf(stderr, "InsertK03Map()--->2, key=%s-%s-%s-%s\n", keyProd.Type_.as_string().c_str(), keyProd.SesID_.Market_.as_string().c_str(), keyProd.SesID_.Broker_.as_string().c_str(), keyProd.SesID_.PVCID_.as_string().c_str());
		    InsertK03Map(keyProd, k03);
            //alert 
         }
      } else { //等待next
         fprintf(stderr, "InsertK03Map()--->3, key=%s-%s-%s-%s\n", keyProd.Type_.as_string().c_str(), keyProd.SesID_.Market_.as_string().c_str(), keyProd.SesID_.Broker_.as_string().c_str(), keyProd.SesID_.PVCID_.as_string().c_str());
		 InsertK03Map(keyProd, k03);
      }
   }
   fprintf(stderr, "2-->K03Map size=%d\n", K03Map_->size());
}
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::ConfigOrSave(TMarket& mkt, SK05Data2& k05, TKIniFile& main, TKIniFile& appia)
{
   if((k05.Type_[0]!='K' && k05.Type_[0]!='F') || k05.Broker_.empty() || k05.TmpPVCID_.empty())
      return;
     
   SKey4Prod  keyProd;
   keyProd.Type_ = k05.Type_;
   keyProd.SesID_.Market_.assign(mkt.as_string());
   keyProd.SesID_.Broker_.assign(k05.Broker_.as_string());
   keyProd.SesID_.PVCID_ = k05.TmpPVCID_;//k05.WhichType()==est_Appia ? k05.TmpPVCID_ : k05.FixPVCID_;
   
   TK03Map::iterator  iter_k03 = K03Map_->find(keyProd);
   if(iter_k03 == K03Map_->end()) {
      fprintf(stderr, "InsertK05Map()-->1, key=%s-%s-%s-%s\n", keyProd.Type_.as_string().c_str(), keyProd.SesID_.Market_.as_string().c_str(), keyProd.SesID_.Broker_.as_string().c_str(), keyProd.SesID_.PVCID_.as_string().c_str());
	  InsertK05Map(keyProd, k05);
      return;
   }
   std::string  socketPortSesID = keyProd.SesID_.Market_.as_string() + keyProd.SesID_.Broker_.as_string() + (k05.WhichType()==est_Appia ? k05.TmpPVCID_.as_string() : k05.FixPVCID_.as_string());
   std::string  sessionID = keyProd.GetSesID();
   std::string  mbrkID     = keyProd.SesID_.Market_.as_string() + keyProd.SesID_.Broker_.as_string();

   if(iter_k03->second.WorkCode_[0] == 'D') { //刪除直接執行,不需理會K05 or K04
      K03Map_->erase(keyProd);
      //刪除Main.ini
      main.EraseSection(sessionID); //erase [sessionID] block
      main.EraseValue(mbrkID, GFixSes, sessionID); //erase [mbrkID]'s Fis
      //刪除appia.ini
      appia.EraseSection(sessionID); //erase connection block [sessionID]
      appia.EraseValue(GMain, GAConnection, sessionID); //erase connection in [Main]
      appia.EraseSection(std::string("m" + sessionID.substr(1, std::string::npos))); //erase middleware block
   } else { //新增需核對K05以及K04
      std::string  nextDate = main.ReadString(GMain, GNextDate, "");
      if(nextDate.empty())
         return;
      TKDate  next, effDate;
      next.assign(nextDate);
      effDate.assign(iter_k03->second.EffDate_.as_string());
   
      if(next >= effDate) { //設定
         fprintf(stderr, "k05.Type_=%s, iter_k03->Type_=%s, k05.FixPVCID_=%s, iter_k03->FixPVCID_=%s, k05.PVCName_=%s, iter_k03->PVCName_=%s, k05.APCode_=%s, iter_k03->APCode_=%s, k05.TmpPVCID_=%s, iter_k03->TmpPVCID_=%s\n",
	            k05.Type_.as_string().c_str(), iter_k03->second.Type_.as_string().c_str(), k05.FixPVCID_.as_string().c_str(), iter_k03->second.FixPVCID_.as_string().c_str(), 
			   k05.PVCName_.as_string().c_str(), iter_k03->second.PVCName_.as_string().c_str(), k05.APCode_.as_string().c_str(), iter_k03->second.APCode_.as_string().c_str(),
			   k05.TmpPVCID_.as_string().c_str(), iter_k03->second.TmpPVCID_.as_string().c_str());
         if(k05.Type_==iter_k03->second.Type_ && k05.FixPVCID_==iter_k03->second.FixPVCID_ && k05.TmpPVCID_==iter_k03->second.TmpPVCID_) {
         //k05.PVCName_==iter_k03->second.PVCName_ && k05.APCode_==iter_k03->second.APCode_ && k05.TmpPVCID_==iter_k03->second.TmpPVCID_) {
         std::string  key = socketPortSesID + "_TMP";
         std::string  tmp = "[" + sessionID + "]";
         std::string  tmp1= main.ReadString(GMain, GMWBegPort, "12000");
		 fprintf(stderr, "key=%s, sessionID=%s, begPort=%s\n", key.c_str(), sessionID.c_str(), tmp1.c_str());
         InsertK05Map(keyProd, k05);
         std::string  mwsid;
         //寫入Main.ini, twse.ini 
         switch(k05.WhichType()) {
            case est_None:   
               //alert
               return;
            case est_Appia: 
               //新增至[Main]
               appia.WriteStrings(GMain, GAConnection, tmp);
               //新增至[middleware]
               mwsid = "m" + sessionID.substr(1, std::string::npos);
               appia.WriteStrings(mwsid, GMidSesID, sessionID);
               appia.WriteString(mwsid, GMidType, GSocket);
			   debug = GetSocketPort(tmp1, socketPortSesID);
			   fprintf(stderr, "tmp1=%s, socketPortSesID=%s, socketport=%s\n", tmp1.c_str(), socketPortSesID.c_str(), debug.c_str());
               appia.WriteString(mwsid, GSocketPort, debug); //自行分配
               appia.WriteString(mwsid, GMidGlobalMsg, "OFF");
               appia.WriteString(mwsid, GMidMsgAckPayload, "OFF_POSITIVE_ACKS");
               appia.WriteString(mwsid, GMidSesEvent, "OFF");
               //新增至[connection]
               appia.WriteString(sessionID, GConFixVer, GFIX42); 
			   debug = main.ReadString(key, GNetAddress, "");
			   fprintf(stderr, "key=%s, sessionID=%s, netaddr=%s\n", key.c_str(), sessionID.c_str(), debug.c_str());
               appia.WriteString(sessionID, GNetAddress, debug); //到暫存Block
               appia.WriteString(sessionID, GLocalFirmID, iter_k03->second.Market_[0]!='T'?GTwse:GOtc);
               appia.WriteString(sessionID, GBrokerFirmID, sessionID);
               appia.WriteString(sessionID, GHeartbeatInt, "30");
               appia.WriteString(sessionID, GStartSeqNum, "1");
               appia.WriteString(sessionID, GTradingSes, GTradingSesB);
               //將FIX pwd寫入Main.ini
               main.WriteString(sessionID, GPwd, k05.Passwd_.as_string());
               break;
            case est_Tmp: 
               tmp = keyProd.SesID_.Market_.as_string() + keyProd.SesID_.Broker_.as_string() + k05.FixPVCID_.as_string();
               main.WriteStrings(GMain, GBroker, keyProd.SesID_.Broker_.as_string());
               if(k05.APCode_[0] == '3') { //成回線路設定 [T1099]
                  main.WriteStrings(mbrkID, GFixSes, tmp);
                  main.WriteString(mbrkID, GMatPwd, k05.Passwd_.as_string());
                  main.WriteString(mbrkID, GMatSesID, k05.TmpPVCID_.as_string());
                  debug = main.ReadString(key, sessionID, "");
				  fprintf(stderr, "3-->key=%s, sessionID=%s, srcp=%s\n", key.c_str(), sessionID.c_str(), debug.c_str());
				  main.WriteString(mbrkID, GMatSrcPort, debug);
               } else { //委託線路設定 
                  main.WriteString(tmp, GTMPID, k05.TmpPVCID_.as_string());
                  debug = GetSocketPort(tmp1, socketPortSesID);
				  fprintf(stderr, "0-->tmp1=%s, socketPortSesID=%s, appia_srcp=%s\n", tmp1.c_str(), socketPortSesID.c_str(), debug.c_str());
				  main.WriteString(tmp, GAppiaMWPort, debug); //自行分配
                  tmp1 = sessionID + "-" + k05.TmpPVCID_.as_string(); //[T109901-A1]
                  debug = main.ReadString(key, sessionID, "");
				  fprintf(stderr, "0-->key=%s, sessionID=%s, srcp=%s\n", key.c_str(), sessionID.c_str(), debug.c_str());
				  main.WriteString(tmp1, GSPort, debug);
                  main.WriteString(tmp1, GPwd, k05.Passwd_.as_string());
               }
               break;
            } //switch(...)
            //將設定好的K03移除
            fprintf(stderr, "erase key=%s-%s-%s-%s\n", keyProd.Type_.as_string().c_str(), keyProd.SesID_.Market_.as_string().c_str(), keyProd.SesID_.Broker_.as_string().c_str(), keyProd.SesID_.PVCID_.as_string().c_str());
			K03Map_->erase(keyProd);
			fprintf(stderr, "erase-->K03Map size=%d\n", K03Map_->size());
         } else {
            fprintf(stderr, "InsertK05Map()-->2, key=%s-%s-%s-%s\n", keyProd.Type_.as_string().c_str(), keyProd.SesID_.Market_.as_string().c_str(), keyProd.SesID_.Broker_.as_string().c_str(), keyProd.SesID_.PVCID_.as_string().c_str());
            InsertK05Map(keyProd, k05);
            //alert 
         }
      } else { //等待next
         fprintf(stderr, "InsertK05Map()-->3, key=%s-%s-%s-%s\n", keyProd.Type_.as_string().c_str(), keyProd.SesID_.Market_.as_string().c_str(), keyProd.SesID_.Broker_.as_string().c_str(), keyProd.SesID_.PVCID_.as_string().c_str());
         InsertK05Map(keyProd, k05);
      }
   } 
}
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::InsertK03Map(SKey4Prod& key, SK03Data2& value)
{
   TK03Map::iterator  iter = K03Map_->fetch(key);
   TK03Map::updater   upd  = K03Map_->begin_update(iter);
   *upd = value;
   fprintf(stderr, "InsertK03Map-->K03Map size=%d\n", K03Map_->size());

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
   //std::string  TsePath = main.ReadString(GMain, GTK05Path, "");
   std::string  TseFile = main.ReadString(GMain, GTK05File, "");
   //std::string  OtcPath = main.ReadString(GMain, GOK05Path, "");
   std::string  OtcFile = main.ReadString(GMain, GOK05File, "");
   std::string  fType("K05");

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
}
//---------------------------------------------------------------------------
/*
K_mf(bool)  TwseFilesHandler::FillSK01Data2(SK01Data2* data, std::string& source)
{
   if(source.size() != sizeof(SK01Data2))
      return false;
   std::string::size_type  pos = 1;
   data->TestNo_.assign(source.substr(0, 8));
   data->PVCNo_.assign(source.substr(8, 5));
   data->Market_.assign(source.substr(13, 1));
   data->IP_X25_.assign(source.substr(14, 1));
   data->Broker_.assign(source.substr(15, 10));
   data->Passwd_.assign(source.substr(25, 4));
   data->PortName_.assign(source.substr(29, 26));
   data->ApplySeqNo_.assign(source.substr(55, 4));
   datd->BrokerIP_.assign(source.substr(59, 15));
   return true;
}
*/
//---------------------------------------------------------------------------
K_mf(void)  TwseFilesHandler::OnTimer(Kway::TTimerBase* Sender)
{

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
