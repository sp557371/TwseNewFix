#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxSesMgr.h"
/*
#ifdef __BORLANDC__
  #include "FevcViewer.h"
  #include "FevcViewerMgr.h"
  #include "ExtBCB.h"
#endif
*/
#include "MesTCPClientTSE.h"
#ifdef SUNLINK
   #include "MesX25.h"
   #include "MesX25EvDev.h"
#endif
#include "SyncEv.h"
#include "SysMgrObjs.h"
#include "MesRegs.h"
#include "bxLinkSes.h"
#include "bxFTSes.h"
#include "bxDRSes.h"
#include "FTSysSetting.h"
//#include "FileLog.h"
#include "NotifyMgr.h"
#if !defined(__BORLANDC__)
#include "EvDevTseTcp.h"
#endif
#include "KIniFile.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx;
using namespace Kway::Tw::Bx::FT;
using namespace Kway::Tw::Bx::DR;
//---------------------------------------------------------------------------
static TbxFTSesMgr* bxSesMgr_;
//---------------------------------------------------------------------------
K_fn(TbxFTSesMgr*) Kway::Tw::Bx::GetBxMgr()
{
   if(bxSesMgr_ == 0){
   	bxSesMgr_ = new TbxFTSesMgr();
//      Kway::SetFTSender((TFTSenderBase*)bxSesMgr_);
   }
   return bxSesMgr_;
}
//---------------------------------------------------------------------------
static Kway::TMesPtr  TMesTSEClient_Factory()  { return Kway::TMesPtr(new Kway::TMesTCPClientTSE()); }
static Kway::TMesReg  TMesTSEClient_Reg      = { "TSEClient", TMesTSEClient_Factory, false, 0, 0 };
#ifdef SUNLINK
static Kway::TMesPtr  TMesX25_Factory()        { return Kway::TMesPtr(new Kway::TMesX25()); }
static Kway::TMesPtr  TMesX25EvDev_Factory()   { return Kway::TMesPtr(new Kway::TMesX25EvDev()); }
static Kway::TMesReg  TMesX25_Reg            = { "X25", TMesX25_Factory, false, 0, 0 };
static Kway::TMesReg  TMesX25EvDev_Reg       = { "EvDevX25", TMesX25EvDev_Factory, false, 0, 0 };
#endif
//---------------------------------------------------------------------------
#if !defined(__BORLANDC__)
static TMesPtr  TEvDevTseTcp_Factory  ()  { return TMesPtr(new TMesTseTcpClient());     }
static TMesReg    TEvDevTseTcp_Reg  = { "Ev-TSEClient", TEvDevTseTcp_Factory,  false, 0, 0 };
#endif
//---------------------------------------------------------------------------
static const Kway::TMesReg* MesRegs[] = { &TMesTSEClient_Reg,
                                         #if !defined(__BORLANDC__)
                                         &TEvDevTseTcp_Reg,
                                         #endif
#ifdef SUNLINK
					  &TMesX25_Reg, 
					  &TMesX25EvDev_Reg,
#endif
};
//---------------------------------------------------------------------------
static Kway::TSesBase* TbxFTTseSendSes_Creater(const Kway::TSesReg&, Kway::TSesMgr& SesMgr)
{
  return dynamic_cast<Kway::TSesBase*>(new TbxFTSes(static_cast<TbxFTSesMgr*>(&SesMgr), KStk::m_Tse));
}
//---------------------------------------------------------------------------
static Kway::TSesBase* TbxFTOtcSendSes_Creater(const Kway::TSesReg&, Kway::TSesMgr& SesMgr)
{
  return dynamic_cast<Kway::TSesBase*>(new TbxFTSes(static_cast<TbxFTSesMgr*>(&SesMgr), KStk::m_Otc));
}
//---------------------------------------------------------------------------
static Kway::TSesBase* TbxFTTseRecvSes_Creater(const Kway::TSesReg&, Kway::TSesMgr& SesMgr)
{
  return dynamic_cast<Kway::TSesBase*>(new TbxFTSes(static_cast<TbxFTSesMgr*>(&SesMgr), KStk::m_Tse, false));
}
//---------------------------------------------------------------------------
static Kway::TSesBase* TbxFTOtcRecvSes_Creater(const Kway::TSesReg&, Kway::TSesMgr& SesMgr)
{
  return dynamic_cast<Kway::TSesBase*>(new TbxFTSes(static_cast<TbxFTSesMgr*>(&SesMgr), KStk::m_Otc, false));
}
//---------------------------------------------------------------------------
static Kway::TSesBase* TbxDRTseSes_Creater(const Kway::TSesReg&, Kway::TSesMgr& SesMgr)
{
  return dynamic_cast<Kway::TSesBase*>(new TbxDRSes(static_cast<TbxFTSesMgr*>(&SesMgr), KStk::m_Tse));
}
//---------------------------------------------------------------------------
static Kway::TSesBase* TbxDROtcSes_Creater(const Kway::TSesReg&, Kway::TSesMgr& SesMgr)
{
  return dynamic_cast<Kway::TSesBase*>(new TbxDRSes(static_cast<TbxFTSesMgr*>(&SesMgr), KStk::m_Otc));
}
//---------------------------------------------------------------------------
static const Kway::TSesReg SesRegs[] =
{
    {"[上市]檔案傳輸(送)",  TbxFTTseSendSes_Creater,    },
    {"[上市]檔案傳輸(收)",  TbxFTTseRecvSes_Creater,    },
    {"[上櫃]檔案傳輸(送)",  TbxFTOtcSendSes_Creater,    },
    {"[上櫃]檔案傳輸(收)",  TbxFTOtcRecvSes_Creater,    },
    {"[上市]DR委託回報",    TbxDRTseSes_Creater,        },
    {"[上櫃]DR委託回報",    TbxDROtcSes_Creater,        },
};
/////////////////////////////////////////////////////////////////////////////
K_ctor TbxFTSesMgr::TbxFTSesMgr()
    : inherited(),
      inherited_Handler(),
      Timer_(this),
      FTSes_ (new TFTMap),
      RptSes_(new TRptSesMap),
      PMachSes_(new TPMachSesMap)
{
// 20130523 add by k288 for 在交易所 ibm 主機上 FTrm_ 初使值不會為 0 
   FTrm_ = 0;
   
   static bool isCreate;
   if(isCreate == false){
      SeqNo_ = 0;
      Timer_.Start(5*1000, &this_type::OnRegTimer);
      PvcTable_ = new TBrkPvcVAC();
//      Setting_ = new TSettingPage("./Settings/FTSetting.rec","./Logs/%4y%2m%2d/ModifyLog/FTSetting.log");
      Setting_ = new TSettingPage(std::string(GetProcConfPath()+"/FTSetting.rec").c_str(),
                                  std::string(GetProcLogPath()+"/ModifyLog/FTSetting.log").c_str());
      IsCompleteLog_ = false;
      Setting_->Reg(&IsCompleteLog_  ,"記錄完整收送封包內容","");
      Setting_->SettingApply();
      const TSysMgrObjs& sys = GetSysMgrObjs();
      VAMgr_.Add("0.連線狀態", GetStatusVACreater());
      VAMgr_.Add("1.連線設定", GetSettingVACreater());
      VAMgr_.Add("2.PVC 配置表", *PvcTable_);
      VAMgr_.Add("3.連線管理", *Setting_);
      sys.SysVAMgr_->Add("交易所連線", VAMgr_);
      std::string fAlarm = GetProcLogPath() + "/Alarm.log";
      std::string fPVC   = GetProcLogPath() + "/PVC.rec";
      std::string fname  = GetProcLogPath() + "/bx.log";
      TFileMode fmode = TFileMode(fmWrite|fmAppend|fmOpenAlways|fmCreatePath);
      Log_.Open(fname.c_str(), fmode);
      AlarmLog_.Open(fAlarm.c_str(), fmode);
      PVCLog_  .Open(fPVC.c_str(), TFileMode(fmWrite|fmOpenAlways|fmCreatePath|fmTrunc));
      isCreate = true;
      Reg(MesRegs, MesRegs+numofele(MesRegs));
      Reg(SesRegs, SesRegs+numofele(SesRegs));
   }
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::OnRegTimer(Kway::TTimerBase* m)
{
   Timer_.Stop();
// 20091229 改由Main.ini 讀取設定檔
//   ReloadSetting(GetProcConfPath()+"/bx.Ses");
   TKIniFile IniFile(std::string(GetConfPath()+"/Main.ini").c_str());
   FailCount_ = IniFile.ReadInt("Main","TMPFailCount",3);
   TStrings Brks, Sess, Pvcs;
   std::string tmp, BrkMkt;
   std::string BrkID;
   std::string DstAddr, SrcAddr;
   int BrkCnt = IniFile.ReadStrings("XFT","Broker",Brks);
   for(int L = 0; L < BrkCnt; L++){
      for(int mkts = 0; mkts < 2; mkts++){
         TMarket mkt = (mkts==0?m_Tse:m_Otc);
         if(mkt == m_Tse){
            DstAddr = IniFile.ReadString("FT","TseSvrAddr","");
            if(DstAddr.empty())
               DstAddr = IniFile.ReadString("Main","TseOrdAddr","10.12.3.21:1001");
            SrcAddr = IniFile.ReadString("FT","TseSrcAddr","");
            if(SrcAddr.empty())
               SrcAddr = IniFile.ReadString("Main","TseSrcAddr","");
         }else{
            DstAddr = IniFile.ReadString("FT","OtcSvrAddr","");
            if(DstAddr.empty())
               DstAddr = IniFile.ReadString("Main","OtcOrdAddr","10.12.3.31:1001");
            SrcAddr = IniFile.ReadString("FT","OtcSrcAddr","");
            if(SrcAddr.empty())
               SrcAddr = IniFile.ReadString("Main","OtcSrcAddr","");
         }
         for(int dirs = 0; dirs < 2; dirs++){
            BrkMkt = (mkt==m_Tse?"T":"O")+Brks[L]+(dirs==0?"-FTSnd":"-FTRecv");
            std::string tmpid    = IniFile.ReadString(BrkMkt,"TMPID","");
            if(tmpid.empty())
               continue;
            std::string srcport  = IniFile.ReadString(BrkMkt,"SourcePort","");
            std::string passwd   = IniFile.ReadString(BrkMkt,"Password","");
            AddSetting(std::string(SesRegs[mkts*2+dirs].Name_),
                       Brks[L]+","+passwd+","+tmpid,"Ev-TSEClient",
                       DstAddr+","+(SrcAddr.empty()?"":"B"+SrcAddr+":")+srcport);
         }
      }
   }
   TKIniFile DRIniFile(std::string(GetConfPath()+"/DR.ini").c_str());
   tmp   = DRIniFile.ReadString("Conf","DR","N");
   BrkID = DRIniFile.ReadString("Conf","Broker","000K");
   if(tmp[0]=='Y' || tmp[0]=='y'){
      DR::TFixSesID FixSesID;
      BrkCnt = IniFile.ReadStrings("Main","Broker",Brks);
      for(int L = 0; L < BrkCnt; L++){
         for(int mkts = 0; mkts < 2; mkts++){
            TMarket mkt = (mkts==0?m_Tse:m_Otc);
            BrkMkt = (mkt==m_Tse?"T":"O")+Brks[L];
            int SesCnt = IniFile.ReadStrings(BrkMkt,"FixSes", Sess);
            for(int i = 0; i < SesCnt; i++){
               int cnt = IniFile.ReadStrings(Sess[i],"TMPID",Pvcs);
               for(int L2 = 0; L2 < cnt; L2++){
                  DR::TPvcID pvcid(Pvcs[L2].c_str());
                  DR::TBrkMPvc BrkMPvc(TBrkID(Brks[L].c_str()), mkt, pvcid);
                  FixSesID.assign(Sess[i]);
                  FixSesMap_.insert(std::make_pair(BrkMPvc, FixSesID));
               }
            }
         }
      }
      DstAddr = IniFile.ReadString("FT","TseSvrAddr","");
      if(DstAddr.empty())
         DstAddr = IniFile.ReadString("Main","TseOrdAddr","10.12.3.21:1001");
      SrcAddr = IniFile.ReadString("FT","TseSrcAddr","");
      if(SrcAddr.empty())
         SrcAddr = IniFile.ReadString("Main","TseSrcAddr","");


      BrkCnt = DRIniFile.ReadStrings("TWSESes","TMPID",Brks);
      for(int L = 0; L < BrkCnt; L++){
         tmp="TWSE-"+Brks[L];
         std::string srcport  = DRIniFile.ReadString(tmp,"SourcePort","");
         std::string passwd   = DRIniFile.ReadString(tmp,"Password","");
         AddSetting("[上市]DR委託回報",BrkID+","+passwd+","+Brks[L],"Ev-TSEClient",
                    DstAddr+","+(SrcAddr.empty()?"":"B"+SrcAddr+":")+srcport);
      }

      DstAddr = IniFile.ReadString("FT","OtcSvrAddr","");
      if(DstAddr.empty())
         DstAddr = IniFile.ReadString("Main","OtcOrdAddr","10.12.3.31:1001");
      SrcAddr = IniFile.ReadString("FT","OtcSrcAddr","");
      if(SrcAddr.empty())
         SrcAddr = IniFile.ReadString("Main","OtcSrcAddr","");

      BrkCnt = DRIniFile.ReadStrings("OTCSes","TMPID",Brks);
      for(int L = 0; L < BrkCnt; L++){
         tmp="OTC-"+Brks[L];
         std::string srcport  = DRIniFile.ReadString(tmp,"SourcePort","");
         std::string passwd   = DRIniFile.ReadString(tmp,"Password","");
         AddSetting("[上櫃]DR委託回報",BrkID+","+passwd+","+Brks[L],"Ev-TSEClient",
                    DstAddr+","+(SrcAddr.empty()?"":"B"+SrcAddr+":")+srcport);
      }
   }
   Timer_.Start(60*1000, &this_type::OnTimer);
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::OnTimer(Kway::TTimerBase* m)
{
   TFTCenter& ft   = const_cast<TFTCenter&>(GetCenter());
   TBrkMkt   brk;
   TFileCode code;
   while(ft.UpdateFTCenter(brk, code)){
      RmRpyQue(brk, code, true);
      TFTMapI i = FTSes_->find(brk);
      if(i != FTSes_->end())
         for(TFTVecI iter = i->second.FTVec_.begin(); iter != i->second.FTVec_.end(); ++iter){
            TbxFTSes* ses = dynamic_cast<TbxFTSes*>(*iter);
            if(ses->GetMarket() == brk.Market_ && ses->IsSend())
               ses->SetState(TbxFTSes::efsStop,std::string("Time out!"));
         }
      FetchNextJob(brk.BrokerID_, brk.Market_, code);
   }
}
//---------------------------------------------------------------------------
K_ctor TbxFTSesMgr::~TbxFTSesMgr  ()
{
   Timer_.Stop();
   Log_.Close();
   const TSysMgrObjs& sys = GetSysMgrObjs();
   sys.SysVAMgr_->Del(VAMgr_);
   VAMgr_.Del(GetStatusVACreater());
   VAMgr_.Del(GetSettingVACreater());
   VAMgr_.Del(*PvcTable_);
   VAMgr_.Del(*Setting_);
   delete PvcTable_;
   if(FTSes_) {
      delete FTSes_;
      FTSes_ = NULL;
   }
   if(RptSes_) {
      delete RptSes_;
      RptSes_ = NULL;
   }
   delete PMachSes_;
   delete Setting_;
}
//---------------------------------------------------------------------------
K_mf(bool) TbxFTSesMgr::WriteAlarm(const void* pkt)
{
   if(AlarmLog_.IsOpened())
      AlarmLog_.Write((char*)pkt);
   return true;
}
//---------------------------------------------------------------------------
K_mf(int) TbxFTSesMgr::WritePVCStatus(int id, const void* pkt)
{
   int ret_id = id;
   bool bFound = false;
   if(id == 0) {
      ret_id = RecIDSet_.size()+1;
      RecIDSet_.insert(ret_id);
   }
   else if(RecIDSet_.find(id) == RecIDSet_.end())
      RecIDSet_.insert(ret_id);
   else
      bFound = true;
   if(PVCLog_.IsOpened()) {
      if(bFound)
         PVCLog_.Seek((id-1)*strlen((char*)pkt), Kway::fsBegin);
      else
         PVCLog_.Seek(0, Kway::fsEnd);
      PVCLog_.Write((char*)pkt);
      PVCLog_.Flush();
   }
   return ret_id;
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::RegFTSes(const TBrkID& brk, TbxSesBase* ses)
{
   //先檢查有沒相同的Ses 已註冊,若有則不再重覆註冊
   if(FTSes_==NULL)
      return;
   TbxFTSes* regSes = dynamic_cast<TbxFTSes*>(ses);
   TBrkMkt bm(brk, regSes->GetMarket());
   TFTMapI i = FTSes_->find(bm);
   if(i == FTSes_->end())
      i = FTSes_->insert(i, TFTMap::value_type(bm, TFTVecs()));
   else{
      TFTVecI iter = std::find(i->second.FTVec_.begin(), i->second.FTVec_.end(), ses);
      if(iter != i->second.FTVec_.end())
         return;
   }
   i->second.FTVec_.push_back(ses);
   if(regSes->IsSend())
      i->second.Send_++;
   else
      i->second.Recv_++;
   DoSendState(bm);
   FetchNextJob(brk, regSes->GetMarket());
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::DelFTSes(const TBrkID& brk, TbxSesBase* ses)
{
   if(FTSes_==NULL)
      return;
   TbxFTSes* regSes = dynamic_cast<TbxFTSes*>(ses);
   TBrkMkt bm(brk, regSes->GetMarket());
   TFTMapI i = FTSes_->find(bm);
   if(i == FTSes_->end())
      return;
 
   TFTVecI iter = std::find(i->second.FTVec_.begin(), i->second.FTVec_.end(), ses);
   if(iter == i->second.FTVec_.end())
      return;
   if(regSes->IsSend())
      i->second.Send_--;
   else
      i->second.Recv_--;
   i->second.FTVec_.erase(iter);
   DoSendState(bm);
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::RegRptSes(TbxSesBase* ses)
{
   if(RptSes_==NULL)
      return;
   //先檢查有沒相同的Ses 已註冊,若有則不再重覆註冊
   TbxLinkSes* regSes = dynamic_cast<TbxLinkSes*>(ses);
   if(regSes) {
      TRptKey key = TRptKey(regSes->GetBrokerID(),regSes->GetMarket());
      (*RptSes_)[key] = ses;
   }
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::DelRptSes(TbxSesBase* ses)
{
   //先檢查有沒相同的Ses 已註冊,若有則不再重覆註冊
   if(RptSes_==NULL)
      return;
   TbxLinkSes* regSes = dynamic_cast<TbxLinkSes*>(ses);
   if(regSes) {
      TRptKey key = TRptKey(regSes->GetBrokerID(),regSes->GetMarket());
      TRptSesMapI i = RptSes_->find(key);
      if(i != RptSes_->end())
         RptSes_->erase(i);
   }
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::RegPMachSes(TSesBase* ses, bool t)
{
/*
   if(PMachSes_ == NULL || ses == NULL)
      return;
   //先檢查有沒相同的Ses 已註冊,若有則不再重覆註冊
   TRptKey key;
   if(t){
      TPMachSesLN* regSes = dynamic_cast<TPMachSesLN*>(ses);
      key = TRptKey(regSes->GetBrokerID(),regSes->GetMarket());
   }else{
      TPMachSes* regSes = dynamic_cast<TPMachSes*>(ses);
      key = TRptKey(regSes->GetBrokerID(),regSes->GetMarket());
   }    
   if(PMachSes_->find(key) != PMachSes_->end())
      return; 
   SPMachSes data;
   data.Ses_  = ses;
   data.IsLn_ = t;
   PMachSes_->insert(std::make_pair(key, data));
*/
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::DelPMachSes(TSesBase* ses, bool t)
{
/*
   //先檢查有沒相同的Ses 已註冊,若有則不再重覆註冊
   if(PMachSes_ == NULL || ses == NULL)
      return;
   TRptKey key;
   if(t){
      TPMachSesLN* regSes = dynamic_cast<TPMachSesLN*>(ses);
      key = TRptKey(regSes->GetBrokerID(),regSes->GetMarket());
   }else{
      TPMachSes* regSes = dynamic_cast<TPMachSes*>(ses);
      key = TRptKey(regSes->GetBrokerID(),regSes->GetMarket());
   }    
   TPMachSesMapI i = PMachSes_->find(key);
   if(i != PMachSes_->end())
      PMachSes_->erase(i);
*/
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::DoSend(const TBrkID& brk, const TMarket& mkt, const TSysID& sysid)
{
   TFTCenter&  ft = const_cast<TFTCenter&> (GetCenter());
   TSenderMgr& snd= const_cast<TSenderMgr&>(GetSender());
   const TFTJob* job;
   bool sending; 
   TRptKey brkmkt(brk,mkt);
   if(ft.GetHnd(brk, mkt, sysid, &job)){
   	sending =(GetRpyID(brkmkt, job->FileCode_) != TSysID(0)); //相同的作業是否正在處理中
   	if(FTrm_ && FTrm_->IsUseORM()){      // 先判斷此項作業是否在其他台主機作業中, 如果是的話,則應將同一作業集中處理
   	   int hid = FTrm_->TestHandle(brkmkt,job->FileCode_);
   	   if(hid > -1)
   	      if(FTrm_->SendReq(job, hid)){
   	         ft.UpdateFTFlow(brk, mkt, sysid, fft_Routed, TKTime::Now().as_string()+"-"+"此項作業己繞送處理");
   	         return;
   	      }
   	}
   }else
      return;
   TFTMapI i = FTSes_->find(brkmkt);
   if(i == FTSes_->end() || i->second.Send_ == 0){
      if(FTrm_ && FTrm_->IsUseORM()){
      	// 20080402這裏要判斷並處理繞送, 同時要考量到沒有正在處理中的相同要求才能繞送
         if((!sending) && FTrm_->SendReq(job)){
            ft.UpdateFTFlow(brk, mkt, sysid, fft_Routed, TKTime::Now().as_string()+"-"+"此項作業己繞送處理");
            FetchNextJob(brk, mkt);
            return;
         }
         int hid;
         if(sending && FTrm_->GetState(brkmkt, hid) >= elsSend){
         	// 有正在傳送中的相同作業則按一般流程進入排隊,待前一作業完成後再進行繞送
         }else{
            ft.UpdateFTFlow(brk, mkt, sysid, fft_Abort, TKTime::Now().as_string()+"-"+"該券商無可用線路");
            snd.SendNotify(*job);
            return;
         }
      }else{
         ft.UpdateFTFlow(brk, mkt, sysid, fft_Abort, TKTime::Now().as_string()+"-"+"該券商無可用線路");
         snd.SendNotify(*job);
         return;
      }
   }
   if(i->second.Recv_ == 0 && i->second.Send_ > 0 && (!sending)){ // local 只有ft-s 無ft-r 判斷是否需要繞送
      if(FTrm_ && FTrm_->IsUseORM()){        // 有繞送模組且其他主機有線路,而且沒有正在處理中的相同作業,就繞送吧
         int hid;       // 否則就按一般正常流程進行
         eLState els = FTrm_->GetState(brkmkt, hid);
         if((!sending) && els == elsAll){
            if(FTrm_->SendReq(job,hid)){
               ft.UpdateFTFlow(brk, mkt, sysid, fft_Routed, TKTime::Now().as_string()+"-"+"此項作業己繞送處理");
               return;
            }
         }
      }
   }
   if(!sending){ //無未回覆的相同作業
      if(FTrm_ && FTrm_->IsUseORD()){   // Load Balance 作業
         TFTRpyQueI i = FTRpyQue_.find(brkmkt);
         if(i != FTRpyQue_.end() && ft.GetQJobs(brkmkt,job->FileCode_) == 0){  //busy && 沒有相同的作業在排隊中
            int localq = i->second.size() + ft.GetQJobs(brkmkt);               // 才可以考慮繞送
            int host = FTrm_->Dispatchable(brkmkt, localq,job->FileCode_);
            if(host > -1)
               if(FTrm_->SendReq(job,host)){
                  ft.UpdateFTFlow(brk, mkt, sysid, fft_Routed, TKTime::Now().as_string()+"-"+"此項作業己繞送處理");
                  return;
               }
         }
      }
      for(TFTVecI iter = i->second.FTVec_.begin(); iter != i->second.FTVec_.end(); ++iter){
         TbxFTSes* ses = dynamic_cast<TbxFTSes*>(*iter);
         if(ses->GetMarket() == job->Market_ && ses->IsSend() && ses->IsFree()){
            if(job->Type_ == ftt_File){
               if(job->FileCode_.as_string() != "B37"){
                  if(DoSourceFileCopy(*job, job->SrcData_.as_string()) == false){
                     ft.UpdateFTFlow(brk, mkt, sysid, fft_Abort, TKTime::Now().as_string()+"-"+"檔案來源有誤,或複製檔案失敗");
                     snd.SendNotify(*job);
                     return;
                  }
               }
               if(ses->AllocNewFT(sysid, *job) == NULL){
                  ft.UpdateFTFlow(brk, mkt, sysid, fft_Abort, TKTime::Now().as_string()+"-"+"檔案內容有誤,或提供的每筆資料大小錯誤");
                  snd.SendNotify(*job);
                  return;
               } 
               AddRpyQue(brkmkt, job->FileCode_, sysid);
               if(ses->DoWorker(ses->GetWorkKey(ckFTStart), mgF010, TbxData()))
                  ses->StartWaitReplyTimer();
            }else{ // message
               ses->AllocNewMsg(sysid, *job);
               AddRpyQue(brkmkt, job->FileCode_, sysid);
               if(ses->DoWorker(ses->GetWorkKey(ckFTMsg), mgF050, TbxData()))
                  ses->StartWaitReplyTimer();
            }
            return;
         }
      }
   }
   // queue
   ft.UpdateFTFlow(brk, mkt, sysid, fft_Q_ing, "排隊中");
   if(FTrm_)
      DoSendState(brkmkt);
   snd.SendNotify(*job);
}
//---------------------------------------------------------------------------
K_mf(bool) TbxFTSesMgr::AddRpyQue(const TRptKey& brk, const TFileCode& code, const TSysID& id)
{
   if(const_cast<TFTSysSetting&>(GetSysSetting()).IsAckless(code))
      return false;
   TFTRpyQueI i = FTRpyQue_.find(brk);
   TFileCode get_code = code;
   if(code == TFileCode("L13") || code == TFileCode("L17")) get_code = TFileCode("L11");
   if(code == TFileCode("L14") || code == TFileCode("L18")) get_code = TFileCode("L12");
   if(i == FTRpyQue_.end())  // not found
      i = FTRpyQue_.insert(i, TFTRpyQue::value_type(brk, TFTRpyMap()));
   TFTRpyMapI iter = i->second.find(get_code);
   if(iter != i->second.end())   // exists
      return false;
   SRpyJob RpyJob;
   RpyJob.SysID_ = id;
   RpyJob.Time_.Clear();
   i->second.insert(TFTRpyMap::value_type(get_code, RpyJob));
   if(FTrm_)
      DoSendState(brk);
   return true;
}
//---------------------------------------------------------------------------
K_mf(TSysID)TbxFTSesMgr::GetRpyID (const TRptKey& brk, const TFileCode& code)
{
   if(code == TFileCode("F02") || code == TFileCode("F10"))
      return TSysID(0); // 20100608 上傳K02 不會有回應,但要考慮K02重新上傳的問題 
   if(const_cast<TFTSysSetting&>(GetSysSetting()).IsAckless(code))
      return TSysID(0);
   TFTRpyQueI i = FTRpyQue_.find(brk);
   if(i != FTRpyQue_.end())  // not found
   {
      TFileCode get_code = code;
      std::string code_str = get_code.as_string(); 
      if(code_str == "R40" || code_str == "R50")
         get_code.assign("R30");
      if(code == TFileCode("L13") || code == TFileCode("L17")) get_code = TFileCode("L11");
      if(code == TFileCode("L14") || code == TFileCode("L18")) get_code = TFileCode("L12");
      TFTRpyMapI iter = i->second.find(get_code);
      if(iter != i->second.end())   // exists
         return iter->second.SysID_;
   }
   return TSysID(0);
}
//---------------------------------------------------------------------------
K_mf(void)TbxFTSesMgr::RmRpyQue (const TRptKey& brk, const TFileCode& code, bool anyway)
{
   if(const_cast<TFTSysSetting&>(GetSysSetting()).IsAckless(code))
      return;
   TFTRpyQueI i = FTRpyQue_.find(brk);
   TFileCode get_code = code;
      if(code == TFileCode("L13") || code == TFileCode("L17")) get_code = TFileCode("L11");
      if(code == TFileCode("L14") || code == TFileCode("L18")) get_code = TFileCode("L12");
   if(i != FTRpyQue_.end()){  // not found
      TFTRpyMapI iter = i->second.find(get_code);
      if(iter != i->second.end()){   // exists
         if(anyway){
            i->second.erase(iter);
            if(FTrm_)
               DoSendState(brk);
         }else
            iter->second.Time_ = TKTime::Now();
      }
   }
}
//---------------------------------------------------------------------------
K_mf(void)TbxFTSesMgr::ChkRpyQue(const TRptKey& brk)
{
   TFTRpyQueI i = FTRpyQue_.find(brk);
   if(i != FTRpyQue_.end()){  // not found
      bool refresh = false;
      for(TFTRpyMapI iter = i->second.begin();iter != i->second.end();){
         if((!iter->second.Time_.IsInvalid()) && 
            (TKTime::Now() - iter->second.Time_) >= atoi(GetSysSetting().GetSetting(ssi_FTInterval).c_str())){
            i->second.erase(iter++);
            refresh = true;
         }else
            iter++;
      }
      if(refresh && FTrm_)
         DoSendState(brk);
   }
}
//---------------------------------------------------------------------------
K_mf(bool) TbxFTSesMgr::FetchNextJob(const TBrkID& brk, const TMarket& mkt, TFileCode code)
{
  TFTCenter& ft = const_cast<TFTCenter&>(GetCenter());
  TSysID id = ft.FetchNextQing(brk, mkt, code);
  if(id != TSysID(0))
  {
    DoSend(brk, mkt, id);
    return true;
  }
  return false;
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::RequestRpt(const TBrkID& brk, KStk::TMarket mrk, long start, long end)
{
  char msg[995];
  // StartSeqNo(6) + EndSeqNo(6) + BrokerID(4)
  sprintf(msg, "%06d%06d%04s", start, end, brk.as_string().c_str());

  TFTNewJob job;
  job.SeqNo_      = 0;
  job.SourceID_   = brk;
  job.DestID_     = TSEID_;
  job.Market_     = mrk;
  job.FileCode_   .assign("R30");
  job.Type_       = ftt_Msg;
  job.Mode_       = ftm_SysReq;
  job.SrcData_    .assign(msg);
  job.DstData_    .clear();
  job.RecLen_     = strlen(msg);
  job.CreateTime_ = TKTime::Now();

  TFTCenter& ft   = const_cast<TFTCenter&>(GetCenter());
  TSysID id = ft.DoReq(job);
  RptMap_[id] = TRptKey(brk, mrk);
  DoSend(job.SourceID_, mrk, id);
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::B36Echo(std::string& fname, const TBrkID& brk, KStk::TMarket mkt)
{
   PvcTable_->ReadB36(fname, TBrokerID(brk.as_string()), mkt);
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::B37Echo(std::string& fname, const TBrkID& brk, KStk::TMarket mkt)
{
   PvcTable_->ReadB37E(fname, TBrokerID(brk.as_string()), mkt);
}
//---------------------------------------------------------------------------
// 不用R40的方式來補
K_mf(void) TbxFTSesMgr::RptRecover(const TSysID& id, std::string& fname, const TBrkID& brk, KStk::TMarket mkt)
{
/*
  if(RptSes_==NULL && PMachSes_ == NULL)
     return;
  TRptMapI i = RptMap_.find(id);
  if(i != RptMap_.end())     // found
  {
    TRptSesMapI iter = RptSes_->find(i->second);
    if(iter != RptSes_->end()) {
      TbxRptSes* ses = dynamic_cast<TbxRptSes*>(iter->second);
      if(ses)
         ses->ImpRptFile(fname);
    }else{
      TPMachSesMapI iter2 = PMachSes_->find(TRptKey(brk, mkt));
      if(iter2 != PMachSes_->end()){
         if(iter2->second.IsLn_){
           TPMachSesLN* ses = dynamic_cast<TPMachSesLN*>(iter2->second.Ses_);
           if(ses)
              ses->ImpRptFile(fname);
         }else{
           TPMachSes* ses = dynamic_cast<TPMachSes*>(iter2->second.Ses_);
           if(ses)
              ses->ImpRptFile(fname);
         }
      }
    }
    RptMap_.erase(i);
    return;
  }
  TRptSesMapI iter = RptSes_->find(TRptKey(brk, mkt));
  if(iter != RptSes_->end()) {
    TbxRptSes* ses = dynamic_cast<TbxRptSes*>(iter->second);
    if(ses)
       ses->ImpRptFile(fname);
  }else{
     TPMachSesMapI iter2 = PMachSes_->find(TRptKey(brk, mkt));
     if(iter2 != PMachSes_->end()){
     	  if(iter2->second.IsLn_){
           TPMachSesLN* ses = dynamic_cast<TPMachSesLN*>(iter2->second.Ses_);
           if(ses)
              ses->ImpRptFile(fname);
     	  }else{
           TPMachSes* ses = dynamic_cast<TPMachSes*>(iter2->second.Ses_);
           if(ses)
              ses->ImpRptFile(fname);
        }
     }
  }
*/
}
//---------------------------------------------------------------------------
K_mf(bool) TbxFTSesMgr::DoSourceFileCopy(const TFTJob& job, const std::string& fname)
{
  TFTSysSetting& sysset = const_cast<TFTSysSetting&>(GetSysSetting());
  TDispatchMgr& mgr     = const_cast<TDispatchMgr&>(GetDispatchMgr());
  std::string path = sysset.GetSetting(ssi_SaveTo);
  MakePath(path);
  return mgr.GetSource(job, fname, path+fname);
}
//---------------------------------------------------------------------------
K_mf(bool) TbxFTSesMgr::WriteLog(const char* data, size_t len, TPvcID pvcno, bool input, TBrkID brk)
{
   if(Log_.IsOpened())
   {
     static char buf[4096+64];
     size_t sz = sprintf(buf, "%s %s-%s %s", TKDateTimeMS::Now().as_string(TKDate::sf_ELong).c_str(),
        brk.as_string().c_str(), pvcno.as_string().c_str(), input ? "<==":"==>");
     memcpy(&buf[sz], data, len);
     buf[sz+len] = 0;
     Log_.WriteLN(buf);
     return true;
   }
   return false;
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::WriteS10 (const char* fname, const TFTJob& job)
{
   static Kway::TFile s10;
   static char buf[1024];
   TFileMode fmode = TFileMode(fmWrite|fmAppend|fmOpenAlways|fmCreatePath);
   s10.Open(fname, fmode);
   if(s10.IsOpened())
   {
     sprintf(buf, "%s%-995s", job.ReplyTime_.as_string(false).c_str(), job.SrcData_.as_string().c_str());
     s10.Write(buf);
     s10.Close();
   }
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::ProcessS10(const TFTJob& job)
{
   TDispatchMgr& mgr  = const_cast<TDispatchMgr&>(GetDispatchMgr());
   TFTSysSetting& set = const_cast<TFTSysSetting&>(GetSysSetting());
   std::string srcpath = set.GetSetting(ssi_TempSave);
   MakePath(srcpath);
   std::string fname = srcpath + job.Market_.as_string() + "S10." + job.DestID_.as_string();
   WriteS10(fname.c_str(), job);
   mgr.DoDispatch(job, fname);  // dispatch
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::SendB37(std::string& fname, const TBrkID& brk, KStk::TMarket mkt, Kway::word len)
{
  TFTNewJob job;
  job.SeqNo_      = 0;
  job.SourceID_   = brk;
  job.DestID_     = TSEID_;
  job.Market_     = mkt;
  job.FileCode_   .assign("B37");
  job.Type_       = ftt_File;
  job.Mode_       = ftm_SysReq;
  job.SrcData_    .assign(fname);
  job.DstData_    .clear();
  job.RecLen_     = len;
  job.CreateTime_ = TKTime::Now();

  TFTCenter& ft   = const_cast<TFTCenter&>(GetCenter());
  TSysID id = ft.DoReq(job);
  DoSend(job.SourceID_, mkt, id);
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::DoFileCmd(TSysID id, TFTNewJob job, std::string& cmd)
{
   int seq = id+(++SeqNo_ * 1000);
   JobMap_.insert(std::make_pair(seq, job));
   const_cast<TSenderMgr&>(GetSender()).DoFileCmd(seq, cmd);
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::FileCmdEcho(int seq, bool result)
{
   TJobMap::iterator iter = JobMap_.find(seq);
   TSysID id = seq % 1000;
   if(iter != JobMap_.end()){
      TFTNewJob job = iter->second;
      DoSend(job.SourceID_, job.Market_, id);
      JobMap_.erase(iter);
   }
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::DoSendState(TBrkMkt brkmkt)
{
	TState State(brkmkt);
   TFTMapI i = FTSes_->find(brkmkt);
   if(i != FTSes_->end()){
   	State.Send_ = i->second.Send_;
   	State.Recv_ = i->second.Recv_;
   }
   std::string path3T = GetDispatchMgr().Get3TLocalPath();
   if(path3T != std::string())
      State.LocaltmpPath_.assign(path3T);
   TFTRpyQueI i2 = FTRpyQue_.find(brkmkt);
   if(i2 != FTRpyQue_.end() && i2->second.size() > 0)  // found and not empty
      for(TFTRpyMapI iter = i2->second.begin(); iter != i2->second.end(); iter++){
      	State.FCode_[State.Count_].assign(iter->first.as_string());
      	State.Count_++;
      }
   State.QJobs_ = (const_cast<TFTCenter&>(GetCenter())).GetQJobs(brkmkt);
   if(FTrm_)
      FTrm_->SendState(State);
}
//---------------------------------------------------------------------------
K_mf(bool) TbxFTSesMgr::SetFTrm(FT::TFTRBase* r)
{
	if(FTrm_ && r)
	   return false; 
	FTrm_ = r;
	for(TFTMapI i = FTSes_->begin(); i != FTSes_->end(); i++)
		DoSendState(i->first);
	return true;
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::WriteDRData(KStk::TMarket mkt, const TDRBody& adata)
{
   char brk[5]={0};
   brk[0]=adata.BrokerId_[0];
   brk[1]=adata.BrokerId_[1];
   brk[2]=adata.LineBrk3_[0];
   brk[3]=adata.LineBrk4_[0];
   DR::TFixSesID   fixses;
   TBrkMPvc bmp(TBrkID(brk), mkt, adata.PvcID_);
   TFixSesMap::iterator L = FixSesMap_.find(bmp);
   if(L == FixSesMap_.end()){   // 不是由FIX 對應的TMP 傳送的委託
      if(adata.FunctionCode_[0]!='1' && adata.FunctionCode_[0]!='2'){ // 刪改單
   	   TTseOrdKey tseordkey(TBrokerID(adata.BrokerId_.as_string()),mkt,TTseOrdID(adata.OrderNo_.as_string()));
   	   TDROrds::iterator DROrdi = DROrds_.find(tseordkey);  // 要找看看這筆委託原始單是不是由FIX 委託出去的
   	   if(DROrdi == DROrds_.end())
   	      return;
   	   if(DROrdi->second.IvacNo_!=adata.IvacNo_ || DROrdi->second.StkNo_!=adata.StkNo_ || DROrdi->second.BSCode_!=adata.BSCode_)
   	      return;
   	   fixses = DROrdi->second.FixSes_;
      }else
         return;
   }else
      fixses = L->second;
   TDRFiles::iterator iter = DRFiles_.fetch(fixses);
   if(iter->second == 0 || (!iter->second->IsOpened())){
      char buf[512];
      sprintf(buf,"%s/%s.DR",GetLogPath().c_str(),fixses.as_string().c_str());
      iter->second = new TFile(buf, TFileMode(fmCreatePath|fmOpenAlways|fmAppend));
   }
   if(adata.FunctionCode_[0]=='1' || adata.FunctionCode_[0]=='2'){
   	TTseOrdKey tseordkey(TBrokerID(adata.BrokerId_.as_string()),mkt,TTseOrdID(adata.OrderNo_.as_string()));
   	TDROrdData orddata;
   	orddata.IvacNo_ = adata.IvacNo_;
   	orddata.StkNo_  = adata.StkNo_;
   	orddata.BSCode_ = adata.BSCode_;
   	orddata.FixSes_ = fixses;
   	DROrds_.insert(std::make_pair(tseordkey, orddata));
   }
   iter->second->Write((char*)&adata, sizeof(TDRBody));
   iter->second->Write("\n",1);
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::RegDRSes(TbxSesBase* ses)
{
   TFTVecI pos = std::find(DRSess_.begin(),DRSess_.end(),ses);
   if(pos == DRSess_.end())          //先檢查有沒相同的Ses 已註冊,若有則不再重覆註冊
       DRSess_.push_back(ses);
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::DelDRSes(TbxSesBase* ses)
{
   TFTVecI pos = std::find(DRSess_.begin(),DRSess_.end(),ses);
   if(pos != DRSess_.end())
      DRSess_.erase(pos);
   if(DRSess_.size() == 0){
      TKIniFile DRIniFile(std::string(GetConfPath()+"/DR.ini").c_str());
      DRIniFile.WriteString("FT","DRReadyDate",TKDate::Now().as_string(TKDate::sf_ELong,0));
   }
      
}
//---------------------------------------------------------------------------
K_mf(void) TbxFTSesMgr::FileUpload(FT::TFTNewJob job)
{
   TFTCenter& ftc   = const_cast<TFTCenter&>(GetCenter());
   TSysID  id = ftc.DoReq(job);
   DoSend(job.SourceID_,job.Market_, id);
}
//---------------------------------------------------------------------------
