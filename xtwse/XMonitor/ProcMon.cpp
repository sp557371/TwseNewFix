//---------------------------------------------------------------------------
#pragma hdrstop
#include "ProcMon.h"
#include "KIniFile.h"
#include "ExcMgrBase.h"
#include "ExtFuncs.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
using namespace Kstl;
using namespace Kway;
using namespace Kway::Tw::Stk;
//---------------------------------------------------------------------------
K_ctor TProcMon::TProcMon  ()
{
   Timer_ = new TPTimer(this);
   MonConf_ = new TSettingPage(std::string(GetProcConfPath()+"/MonConf.rec").c_str(),
                               std::string(GetProcLogPath()+"/ModifyLog/SysParam.log").c_str()); 
   MonConf_->Reg(&AutoChk_  , "啟用程序監管,每隔(秒)檢查一次", "60",eword,&ChkInterval_); 
   MonConf_->Reg(&AutoStart_, "程序不存在時自動重新啟動,連續n次就發通知", "2",eword,&MaxCount_); 
   MonConf_->SettingApply();
   TKIniFile IniFile(std::string(GetConfPath()+"/Main.ini").c_str());
   ExecPathName_ = IniFile.ReadString("Main","ExecPathName","./XTwse");
   TStrings Brks,Sess;
   std::string BrkMkt;
   int BrkCnt = 0;
// 20100906 eric 修改WorkCode 取得來源及增加WorkCode 81 [假日測試假開盤]的判斷,20100908 取消此做法
//   int WorkCode = IniFile.ReadInt("Main","WorkCode",0);
   FILE* fwk;
//20100908 考量手動執行假開盤的情況,取workcode 位置由 Log 路徑下改為TmpPath的WorkCode.[T/O]
//此做法同時可避免fail-over 之後找不到WorkCode 檔的問題(fail-over 流程會將log 清除)
//   std::string str = ChangeYMD(GetLogPath().c_str())+"WorkCode";
   int WorkCode = 0;
   std::string str = IniFile.ReadString("Main","TmpPath","");
   if(str.empty()){
      if(GetExcMgr())
         GetExcMgr()->Alert("XM0001");
   }else{
      str +="/WorkCode";
      str +=GetProcMkt()==m_Tse?".T":".O";
      fwk=fopen(str.c_str(),"rt");
      char wc[16];
      if(fwk == NULL){
         if(GetExcMgr())
            GetExcMgr()->Alert("XM0001");
      }else{
         fgets(wc,sizeof(wc),fwk);
         WorkCode = atoi(wc);
      }
      fclose(fwk);
   }
   if(WorkCode == 1 || WorkCode == 81){  // 假開盤[假日測試假開盤], 必須啟用 TestBroker 的交易所測試線路
      if(GetExcMgr())
         GetExcMgr()->Alert("XM0002");
      BrkCnt = IniFile.ReadStrings("Main","TestBroker",Brks);
      for(int L = 0; L < BrkCnt; L++){
         BrkMkt = "T"+Brks[L];
         int SesCnt = IniFile.ReadStrings(BrkMkt,"FixSes", Sess);
         for(int i = 0; i < SesCnt; i++){
            AddSession(Sess[i],m_Tse, TBrokerID(Brks[L]));
            usleep(5000);
         }
         BrkMkt = "O"+Brks[L];
         SesCnt = IniFile.ReadStrings(BrkMkt,"FixSes", Sess);
         for(int i = 0; i < SesCnt; i++){
            AddSession(Sess[i],m_Otc, TBrokerID(Brks[L]));
            usleep(5000);
         }
      }
   }else{
      if(GetExcMgr())
         GetExcMgr()->Alert("XM0003");
   }
   // 啟動券商申請線路
   BrkCnt = IniFile.ReadStrings("Main","Broker",Brks);
   for(int L = 0; L < BrkCnt; L++){
      BrkMkt = "T"+Brks[L];
      int SesCnt = IniFile.ReadStrings(BrkMkt,"FixSes", Sess);
      for(int i = 0; i < SesCnt; i++){
         AddSession(Sess[i],m_Tse, TBrokerID(Brks[L]));
         usleep(5000);
      }
      BrkMkt = "O"+Brks[L];
      SesCnt = IniFile.ReadStrings(BrkMkt,"FixSes", Sess);
      for(int i = 0; i < SesCnt; i++){
         AddSession(Sess[i],m_Otc, TBrokerID(Brks[L]));
         usleep(5000);
      }
   }
   Timer_->Start(ChkInterval_*1000, &this_type::OnTimer);
   GetExcMgr()->RegCmdMgrObj(this,"ProcMon");
}
//---------------------------------------------------------------------------
K_dtor TProcMon::~TProcMon  ()
{
   delete Timer_;
   delete MonConf_;
}
//---------------------------------------------------------------------------
K_mf(TaViewArea) TProcMon::CreateNewVA  ()
{
   TViewControls   vcs;
   vcs.set(vco_Setting);
   TProcItemVA* SVA = (new TProcItemVA(&ProcItem_));
   return TaViewArea(SVA);
}
//---------------------------------------------------------------------------
K_mf(void) TProcMon::AddSession(std::string sid, KStk::TMarket mkt, KStk::TBrokerID brk)
{
   TSesID SesID(sid);
   char cmd[128],buf[128];
   TProcItem::iterator iter = ProcItem_.fetch(SesID);
   iter->second.Market_     = mkt;
   iter->second.BrokerID_   = brk;
   iter->second.StopMonit_  = false;
   if(ChknWake(sid,mkt,brk))
      iter->second.LastTime_.assign(TKTime::Now().as_string());
}
//---------------------------------------------------------------------------
K_mf(bool) TProcMon::ChknWake(std::string sid, KStk::TMarket mkt, KStk::TBrokerID brk,bool chk)
{
   char cmd[128],buf[128];
   sprintf(cmd,"ps -ef|grep %s|grep %s|grep -v grep",ExecPathName_.c_str(),sid.c_str());
   FILE* ptr = popen(cmd,"r");
   if(fgets(buf, 128, ptr)==NULL){ // process 不存在, 需wakeup
      if(GetExcMgr())
         GetExcMgr()->Alert(chk?"XM1001":"XM1000",sid);
      pclose(ptr); 
      if(AutoStart_){
         sprintf(cmd,"%s -m%c -b%s -p%s &",ExecPathName_.c_str(),mkt==m_Tse?'T':'O',
                                                  brk.as_string().c_str(), sid.c_str());
         pclose(popen(cmd,"w"));
         usleep(5000);
      }
      return true;
   }
   pclose(ptr); 
   return false;
}
//---------------------------------------------------------------------------
K_mf(void) TProcMon::OnTimer(TTimerBase*)
{
   Timer_->Stop();
   if(AutoChk_){
      char cmd[256],buf[128];
      sprintf(cmd,"ps -ef|grep %s|grep -v grep|wc|awk '{print $1}'",ExecPathName_.c_str());
      FILE* ptr = popen(cmd,"r");
      if(fgets(buf, 128, ptr)!=NULL)
         if(atoi(buf) == ProcItem_.size()){
            TKTime tm = TKTime::Now();
            for(TProcItem::iterator iter = ProcItem_.begin(); iter != ProcItem_.end(); iter++){
               if(iter->second.StopMonit_)
                  continue;
               TProcItem::updater u = ProcItem_.begin_update(iter);
               u->LastChkTime_.assign(tm.as_string());
               u->Count_ = 0;
            }
            Timer_->Start(ChkInterval_*1000, &this_type::OnTimer);
            pclose(ptr);
            return;
         }
      pclose(ptr);
      for(TProcItem::iterator iter = ProcItem_.begin(); iter != ProcItem_.end(); iter++){
         if(iter->second.StopMonit_)
            continue;
         TProcItem::updater u = ProcItem_.begin_update(iter);
         u->LastChkTime_.assign(TKTime::Now().as_string());
         if(ChknWake(iter->first.as_string(),iter->second.Market_,iter->second.BrokerID_,true)){
            u->LastTime_.assign(TKTime::Now().as_string());
            u->Memo_.clear();
            u->Count_++;
            u->TotCount_++;
            if(u->Count_ == MaxCount_){
               if(GetExcMgr())
                  GetExcMgr()->Alert("XM1010",iter->first.as_string());
            }
            continue;
         }
         u->Count_ = 0;
      }
   }
   Timer_->Start(ChkInterval_*1000, &this_type::OnTimer);
}
//---------------------------------------------------------------------------
K_mf(bool) TProcMon::DoCommand(Kway::word cmdid, std::string msg)
{
   TMarket mkt = (msg[0]=='T'?m_Tse:m_Otc);
   TBrokerID brk(msg.substr(1,4));
   switch(cmdid){
      case 1:
         AddSession(msg, mkt, brk);
         break;
      case 2:
         DelSession(msg, mkt, brk);
         break;
      default:
         break;
   }
}
//---------------------------------------------------------------------------
K_mf(void) TProcMon::DelSession(std::string sid, KStk::TMarket mkt, KStk::TBrokerID brk)
{
   TSesID SesID(sid);
   char cmd[128],buf[128];
   TProcItem::iterator iter = ProcItem_.find(SesID);
   if(iter == ProcItem_.end())
      return;
   iter->second.Market_     = mkt;
   iter->second.BrokerID_   = brk;
   iter->second.StopMonit_  = true;
   iter->second.Memo_.assign(TKTime::Now().as_string()+std::string(" 這個連線已經被刪除"));
   sprintf(cmd,"ps -ef|grep %s|grep %s|grep -v grep|awk '{print $2}'|xargs kill -9",ExecPathName_.c_str(),sid.c_str());
   FILE* ptr = popen(cmd,"r");
   pclose(ptr);
}
//---------------------------------------------------------------------------
