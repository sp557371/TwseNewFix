//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FixSesMgr.h"
#include "MesRegs.h"
#include "SesFix.h"
#include "KIniFile.h"
#include <unistd.h>
#include "KTime.h"
 //---------------------------------------------------------------------------
K_fn(Kway::Tw::Fix::Stk::TFixSesMgr&)  Kway::Tw::Fix::Stk::GetFixSesMgr()
{
   static TFixSesMgr  FixSesMgr;
   return FixSesMgr;
}
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw::Fix::Stk;
//---------------------------------------------------------------------------
static Kway::TSesBase*  TFixSes42_Creater(const Kway::TSesReg&, Kway::TSesMgr& SesMgr)
{
   TStkSesFix* ses = new TStkSesFix("FIX.4.2");
   return static_cast<Kway::TSesBase*>(ses);
}
//---------------------------------------------------------------------------
static Kway::TSesBase*  TFixSes44_Creater(const Kway::TSesReg&, Kway::TSesMgr& SesMgr)
{
   TStkSesFix* ses = new TStkSesFix("FIX.4.4");
   return static_cast<Kway::TSesBase*>(ses);
}
//---------------------------------------------------------------------------
static const Kway::TSesReg  SesRegs[] = {
    {"TWSE-FIX.4.2連線", TFixSes42_Creater },
    {"TWSE-FIX.4.4連線", TFixSes44_Creater },
};

/////////////////////////////////////////////////////////////////////////////
// class TFixSesMgr::TSendOrdImpl impl
/////////////////////////////////////////////////////////////////////////////
K_ctor  TFixSesMgr::TSendOrdImpl::TSendOrdImpl(TFixSesMgr* fsm, const std::string& brkID, TOrderV& orderV)
   : FixSesMgr_ (fsm)
   , OrderV_    (orderV)
   , BrokerID_  (brkID)
   , SleepTime_ (FixSesMgr_->SleepTime_)
   , EachBatch_ (FixSesMgr_->EachBatch_)
   , CurrBatch_ (0)
   , CurrOrderI_(OrderV_.begin())
   , Timer_     (this)
   , NextClock_ (0)
{
   // HP-UX 上的 Timer 每 500 毫秒才會來ㄧ次
   // 利用自己判斷時間看看會不會比較準
   Timer_.Start(0, &this_type::OnTimer);
}
//---------------------------------------------------------------------------
K_mf(void)  TFixSesMgr::TSendOrdImpl::OnTimer(TTimerBase*)
{
   // 檢查時間是否到了沒
   Kway::int64  currClock = TTimerBase::GetClockMS();
   if(currClock < NextClock_)
      return;

   // 檢查這條 FixSes 的狀態
   TStkSesFix*  ses = FixSesMgr_->GetTradSes(BrokerID_);
   if(!ses || ses->IsLogonOK() == false) {
      // FixSes 有問題,在等一下在做囉
      NextClock_ = (currClock + 100);
      return;
   }

   TOrderVI iter = CurrOrderI_;
   int      cnt  = 0;

   for(; iter!=OrderV_.end(); ++iter) {
      if(cnt == EachBatch_) {
         CurrOrderI_ = iter;  // 下ㄧ次從這筆開始傳送
         break;
      }
      ses->SendOrder(*iter);
      ++cnt;
   }

   if(iter == OrderV_.end()) {
      // 代表委託都送完了,工作結束
      Timer_.Stop();
   } else {
      NextClock_ = (TTimerBase::GetClockMS() + SleepTime_);
   }
}
/////////////////////////////////////////////////////////////////////////////
K_ctor  TFixSesMgr::TFixSesMgr()
   : TSesMgr()
{
   Map_.clear();
   Orders_.clear();
   StartSesMgr();
}
//---------------------------------------------------------------------------
K_dtor  TFixSesMgr::~TFixSesMgr()
{
   // cooper 10.09.25 {
   SendOrdImplA_.clear();
   // } cooper 10.09.25
   VAMgr_.Del(GetSettingVACreater());
   VAMgr_.Del(GetStatusVACreater());
   Del(SesRegs);
   Del(Kway::TMesRegs::GetDefaultMesRegs());
   Map_.clear();
}
//---------------------------------------------------------------------------
K_mf(bool)  TFixSesMgr::StartSesMgr()
{
   Reg(Kway::TMesRegs::GetDefaultMesRegs());
   Reg(SesRegs, SesRegs+numofele(SesRegs));
   VAMgr_.Add("連線設定", GetSettingVACreater());
   VAMgr_.Add("連線狀態", GetStatusVACreater());

   Timer_ = new TOTimer(this);
   Timer_->Start(10000, &this_type::OnTimer);
   OrderTimer_ = new TOTimer(this);
   StartOrderTimer(40);
   return true;
}
//---------------------------------------------------------------------------
K_mf(void)  TFixSesMgr::OnTimer(TTimerBase*)
{
   Timer_->Stop();
   std::string  sesName;//("TWSE-FIX.4.2連線");
   //std::string  mesName("TCPClient");
   std::string  mesName("EvDevTcpCli");
   TKIniFile*   ini = new TKIniFile("./Settings/FixSesMgr.ini");

   std::string  sesID, setting, ip, sep(";"), comm("COMMON");
   std::string  Tag56, HeartBeatInt, BrokerID, PWD, EndT, Log, tmp, tmp2, verStr;
   int  ver;

   ver          = ini->ReadInt(comm, "FIXVer", 0);
   tmp2         = ini->ReadString(comm, "BrokerID", "");
   EndT         = ini->ReadString(comm, "EndTime", "");
   Log          = ini->ReadString(comm, "WriteLog", "");
   ip           = ini->ReadString(comm, "IP:Port", "");
   HeartBeatInt = ini->ReadString(comm, "HeartBeatInt", "");
   Tag56        = ini->ReadString(comm, "Tag56", "");
   PWD          = ini->ReadString(comm, "PWD", "");
   SleepTime_   = ini->ReadInt(comm, "Interval", 0);
   EachBatch_   = ini->ReadInt(comm, "EachBatch", 0);
   if(SleepTime_ < 0)   SleepTime_ = 0;
   if(EachBatch_ < 0)   EachBatch_ = 0;
   if(ver != 1)
      ver = 0;

   Kway::TKIniFile::TSections*   secs= ini->GetSections();
   Kway::TKIniFile::TSection*    section;
   
   fprintf(stderr, "BrokerID=%s, EndTime=%s, WriteLog=%s, IP:Port=%s, HeartBeatInt=%s, Tag56=%s, PWD=%s\n", tmp2.c_str(), EndT.c_str(), Log.c_str(),
           ip.c_str(), HeartBeatInt.c_str(), Tag56.c_str(), PWD.c_str());
   
   for(Kway::TKIniFile::TSections::iterator iter=secs->begin(); iter!=secs->end(); ++iter) {
      fprintf(stderr, "Section=%s\n", iter->first.c_str());
      if(iter->first==std::string("COMMON") || iter->first==std::string("Main"))
         continue;
      sesID = iter->first;
      //個別設定
      verStr = ini->ReadString(sesID, "FIXVer", "");
      if(Tag56.empty())
         Tag56 = ini->ReadString(sesID, "Tag56", "");
      if(HeartBeatInt.empty())
         HeartBeatInt = ini->ReadString(sesID, "HeartBeatInt", "");
      if(tmp2 == "1")
         BrokerID  = sesID.substr(1,4);
      else
         BrokerID  = ini->ReadString(sesID, "BrokerID", "");
      if(PWD.empty())
         PWD       = ini->ReadString(sesID, "PWD", "");
      if(EndT.empty())
         EndT      = ini->ReadString(sesID, "EndTime", "");
      if(Log.empty())
         Log       = ini->ReadString(sesID, "WriteLog", "1");
      if(ip.empty())
         ip        = ini->ReadString(sesID, "IP:Port", "");
      setting = sesID + sep + Tag56 + sep + HeartBeatInt + sep + BrokerID + sep + PWD + sep + EndT + sep + Log + sep;
      if(verStr.empty()) {
         sesName = ver==0 ? "TWSE-FIX.4.2連線" : "TWSE-FIX.4.4連線";
      } else {
         int  a = atoi(verStr.c_str());
         sesName = (a != 1) ? "TWSE-FIX.4.2連線" : "TWSE-FIX.4.4連線";
      }
      inherited::AddSetting(sesName, setting.c_str(), mesName, ip.c_str());
   }
   delete ini;
   delete Timer_;
}
//---------------------------------------------------------------------------
K_mf(void)  TFixSesMgr::OnOrderTimer(TTimerBase*)
{
   fprintf(stderr, "OnOrderTimer-->1\n");
   OrderTimer_->Stop();
   TKIniFile*  ini = new TKIniFile("./Settings/FixSesMgr.ini");
   std::string  fn = ini->ReadString("Main", "OrderFile", "");
   if(fn.empty()) {
      fprintf(stderr, "OnOrderTimer-->2\n");
      delete ini;
      StartOrderTimer(20);
      return;
   }
   Kway::TFile*  file = new Kway::TFile(fn.c_str(), Kway::TFileMode(Kway::fmRead));
   if(file->IsOpened()==false || file->GetFileSize()<=0) {
      fprintf(stderr, "OnOrderTimer-->3\n");
      delete ini;
      delete file;
      StartOrderTimer(20);
      return;
   }
   fprintf(stderr, "OnOrderTimer-->4\n");
   char buffer[255], lnChar;
   //char newline[] = {0x0A};
   //const char soh[] = {0x01};
   char soh[2] = {0};
   soh[0] = 0x01;
   std::string str;
   std::string tmpS;
   std::string::size_type pos; 
   Orders_.clear();
   while(file->IsEOF() == false) {
      fprintf(stderr, "OnOrderTimer-->ifle->IsEOF() == false\n");
      if(file->ReadLN(buffer, sizeof(buffer)-1, lnChar) > 0) {
         fprintf(stderr, "OnOrderTimer-->ReadLN\n");
         if(lnChar!='\r' && lnChar!='\n') {
            fprintf(stderr, "OnOrderTimer-->ReadLN---continue\n");
            continue;
         }
         str = std::string(buffer);
         tmpS= std::string(soh) + std::string("50=");
         pos = str.find(tmpS);
         if(pos != std::string::npos) {
            std::string  tmp = str.substr(pos+4, std::string::npos);
            pos = tmp.find_first_of(soh);
            if(pos != std::string::npos) {
               tmp = tmp.substr(0, pos);
               TOrderMapI           iter = Orders_.fetch(tmp);
               TOrderMap::updater   upd  = Orders_.begin_update(iter);
               upd->push_back(str);
               
            }
         }
      }    
   }
   fprintf(stderr, "OnOrderTimer-->5\n");
   if(SendOrders())
      delete OrderTimer_;      
   fprintf(stderr, "OnOrderTimer-->6\n");
   delete ini;
   delete file;
   //delete OrderTimer_;
}
//---------------------------------------------------------------------------
K_mf(bool)  TFixSesMgr::IsSesIDExist(TStkSesFix* ses)
{
   const SSesRegKey  key(ses->GetBrokerID(), ses->GetSesID());
   TSesMapI  iter = Map_.find(key);
   return (iter != Map_.end());
}
//---------------------------------------------------------------------------
K_mf(bool)  TFixSesMgr::IsOnlineSesID(TStkSesFix* ses)
{
   const SSesRegKey  key(ses->GetBrokerID(), ses->GetSesID());
   TSesMapI  iter = Map_.find(key);
   if(iter == Map_.end())
      return false;
   if(ses == iter->second)
      return true;
   return false;
}
//---------------------------------------------------------------------------
K_mf(void)  TFixSesMgr::RegTradSes(TStkSesFix* ses)
{
   char  buf[128] = {0};
   const SSesRegKey  key(ses->GetBrokerID(), ses->GetSesID());

   TSesMapI  iter = Map_.find(key);
   if(iter != Map_.end()) {
      
      std::string  msg;
      sprintf(buf, "<%s,%s>%s", key.BrokerID_.c_str(), key.SesID_.c_str(), "已經存在!");
      msg = buf;
      return;
   }

   TSesMap::updater upd;
   iter = Map_.fetch(key);
   upd = Map_.begin_update(iter);
   *upd = ses;
}
//---------------------------------------------------------------------------
K_mf(void)  TFixSesMgr::UnRegTradSes(TStkSesFix* ses)
{
   const SSesRegKey  key(ses->GetBrokerID(), ses->GetSesID());

   TSesMapI  iter = Map_.find(key);
   if(iter != Map_.end()) {
      Map_.erase(key);
   }
}
//---------------------------------------------------------------------------
K_mf(TStkSesFix*)  TFixSesMgr::GetTradSes(const std::string& brokerID)
{
   TSesMapI  iter_begin = Map_.begin();
   TSesMapI  iter_end   = Map_.end();
   for(TSesMapI iter=iter_begin; iter!=iter_end; ++iter) {
      if(iter->first.BrokerID_==brokerID && iter->second->IsLogonOK())
         return iter->second;
   }

   std::string  brkTo3  = brokerID.substr(0,3);
   for(TSesMapI iter=iter_begin; iter!=iter_end; ++iter) {
      std::string  brk3 = iter->first.BrokerID_.substr(0,3);
      if(brk3==brkTo3 && iter->second->IsLogonOK()) {
         // Log message
         return iter->second;
      }
   }
   char buf[128] = {0};
   std::string  msg;
   sprintf(buf, "%s%s%s", "券商代號", brokerID.c_str(), "尚未登入亦找不到其他可用的券商!");
   msg = buf;
   return NULL;
}
//---------------------------------------------------------------------------
K_mf(bool)  TFixSesMgr::SendOrders()
{
   fprintf(stderr, "SendOrders()--->1\n");
   typedef  std::vector<std::string>  TOrderV;
   typedef  TOrderV::iterator         TOrderVI;
   TStkSesFix*  ses;
   bool  isSend(false);
   char  buf[128] = {0};
   
   for(TOrderMapI iter=Orders_.begin(); iter!=Orders_.end(); ++iter) {
      ses = GetTradSes(iter->first);
      if(!ses || ses->IsLogonOK()==false) {
         std::string  msg;
         sprintf(buf, "%s%s%s", "券商代號", iter->first.c_str(), "尚未登入!");
         msg = buf;
         fprintf(stderr, "SendOrders()--->2, %s\n", buf);
         continue;
      }
      fprintf(stderr, "SendOrders()--->2.1");
      isSend = true;
      TOrderV&  v = iter->second;
      int counter = 0;
      for(TOrderVI i=v.begin(); i!=v.end(); ++i) {
         fprintf(stderr, "SendOrders()--->3\n");
         if(EachBatch_ == 0) {
            ses->SendOrder(*i);
         } 
         // cooper 10.09.25 { 如果需要依據時間間隔傳送的委託,下面來處理
         //else {
         //   ses->SendOrder(*i);
         //   ++counter;
         //   fprintf(stderr, "counter=%d, EachBatch_=%d, Now=%s\n", counter, EachBatch_, Kway::TKDateTimeMS::Now().as_string().c_str());
         //   if(counter == EachBatch_) {
         //      fprintf(stderr, "Sleep()--->1\n");
         //      usleep(SleepTime_);
         //      fprintf(stderr, "Sleep()--->2\n");
         //      counter = 0;
         //   }
         //   fprintf(stderr, "Now=%s\n", Kway::TKDateTimeMS::Now().as_string().c_str());
         //}
         // } cooper 10.09.25
      }
   }
   fprintf(stderr, "SendOrders()--->4, isSend=%s\n", isSend?"T":"N");
   
   // cooper 10.09.25 {
   if (isSend && EachBatch_ != 0) {
      TSendOrdImpl* impl;
      
      for (TOrderMapI iter=Orders_.begin(); iter!=Orders_.end(); ++iter) {
         impl = new TSendOrdImpl(this, iter->first, iter->second);
         SendOrdImplA_.push_back(TSendOrdImplWsRef(impl));
      }
   }
   // } cooper 10.09.25
   
   if(isSend == false)
      StartOrderTimer(20);
   return bool(isSend);
}
//---------------------------------------------------------------------------
K_mf(void)  TFixSesMgr::Init(Kway::Signon::TSignonSvrDB* SG)
{
   SignonDB_  = SG;
}
//---------------------------------------------------------------------------

