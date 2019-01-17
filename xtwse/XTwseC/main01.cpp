//===========================================================================
//
// main01.h.cpp
//
//===========================================================================
#pragma hdrstop
#include "main01.h"
#include "SysMgrObjs.h"
#include "KIniFile.h"
#include "Nsa_UtilComm.h"
#include "FixStk.h"
#include "TwseVer.h"
#include "TwseType.h"
#include "TwseFstUnit.h"
#pragma package(smart_init)
using namespace Kway;
//===========================================================================
TFieldDesp OrdSysIDDesp=
{
   "系統序號",8,ta_xRight_yCenter,ta_xRight_yCenter,tfs_Fixed,
};
//===========================================================================
TFieldDesp OrdFieldDesp2[]=
{
  { "委託書號", 6, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null, },
  { "系統序號", 8, ta_xRight_yCenter,  ta_xRight_yCenter,  tfs_Null, },
};
//===========================================================================
TFieldDesp OrdFieldDesp[] =
{
   { "券商",         4, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null,    },
   { "委託書號",     6, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null,    },
   { "投資人帳號",   8, ta_xRight_yCenter,  ta_xRight_yCenter,  tfs_Null,    },
   { "商品代號",     6, ta_xLeft_yCenter,   ta_xLeft_yCenter,   tfs_Null,    },
   { "委託單價",     7, ta_xRight_yCenter,  ta_xRight_yCenter,  tfs_Null,    },
   { "委託數量",     8, ta_xRight_yCenter,  ta_xRight_yCenter,  tfs_Null,    },
   { "B/S",          1, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null,    },
   { "類別",         1, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null,    },
   { "盤別",         6, ta_xLeft_yCenter,   ta_xLeft_yCenter,   tfs_Null,    },
   { "線路",         4, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null,    },
   { "來源/使用者", 12, ta_xLeft_yCenter,   ta_xLeft_yCenter,   tfs_Null,    },
   { "來源/時間",    8, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null,    },
   { "來源/ClOrdID", 8, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
   { "交易所/時間",  8, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null,    },
   { "異動時間",     8, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null,    },
   { "BF-數量",      8, ta_xRight_yCenter,  ta_xRight_yCenter,  tfs_Null,    },
   { "AF-數量",      8, ta_xRight_yCenter,  ta_xRight_yCenter,  tfs_Null,    },
   { "Mch數量",      8, ta_xRight_yCenter,  ta_xRight_yCenter,  tfs_Null,    },
   { "成交數量",     8, ta_xRight_yCenter,  ta_xRight_yCenter,  tfs_Null,    },
//   { "成交價金",    12, ta_xRight_yCenter,  ta_xRight_yCenter,  tfs_Null,    },
   { "備註",        40, ta_xLeft_yCenter,   ta_xLeft_yCenter,   tfs_Null,    },
   { "LastOCmd",     9, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null,    },
   { "Flow",         8, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null,    },
   { "LastTseCode", 12, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null,    },
   { "TwseIvacnoFlag", 12,ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null,    },
   { "WrntMktMkr",   11,ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null,    },
   { "SendingTime",  12,ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null,    },
   { "LastOkOCmd",   11,ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null,    },
   { "TwseExCode",    8,ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null,    },
   { "TMPAfQty",      8, ta_xRight_yCenter,  ta_xRight_yCenter,  tfs_Null,    },
   { "TMPIvacNo",     8, ta_xRight_yCenter,  ta_xRight_yCenter,  tfs_Null,    },
   { "SubAcc0",       8,ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_Null,    },
};
//========================================================================
static FIX::Stk::TTradeUnit*  TwseFstUnitCreater (const std::string& sesID, void* param)
{
   Main01* main01 = reinterpret_cast<Main01*>(param);
   return new TTwseFstUnit(sesID, main01->GetSysOrds(), main01->GetOrdCenter(), main01->GetOrdFlow());
   //return new TTwseFstUnit(sesID, main01->GetOrdFlow());
}
//---------------------------------------------------------------------------
K_ctor Main01::Main01(Kway::Signon::TSignonSvrDB* signon)
{
   char Buf[256]; 
   TFile  Log_;
   int    lenBuf;
   Log_.Open(std::string(GetProcLogPath()+"version.log").c_str(), (TFileMode)(fmRandomAccess|fmOpenAlways));
   Log_.Seek(Log_.GetFileSize(), fsBegin);
   sprintf(Buf,"%s XTwse main program loaded, version: 1.2.1, last modified date 2011-05-18.",
           TKTime::Now().as_string().c_str());
   Log_.WriteLN(Buf);
   // cooper {
   std::string strTime = TKTime::Now().as_string();
   
   // libxsFixStk
   lenBuf = sprintf(Buf, "%s LibXsFixStk loaded, version: %s, last modified date %s.\r\n",
                           strTime.c_str(), FIX::Stk::TBaseFunc::GetVersion   ().c_str(),
                                            FIX::Stk::TBaseFunc::GetModifyDate().c_str());
   Log_.Write(Buf, lenBuf);

   // libxsTwse
   lenBuf = sprintf(Buf, "%s LibXsTwse loaded, version: %s, last modified date %s.\r\n",
                           strTime.c_str(), TTwseBaseFunc::GetVersion   ().c_str(),
                                            TTwseBaseFunc::GetModifyDate().c_str());
   Log_.Write(Buf, lenBuf);
   // } cooper
   Log_.Close();

   // cooper { 為了避免讀取 Main.ini 多次
   MainIni_ = new TTwseMainIni;
   MainIni_->InitCont(GetProcName());
   // } cooper

   ExcMgr_ = new TExcMgr(1,1);
   SetExcMgr(ExcMgr_);
   ExcMgr_->Alert("XX0000");
   MsgC_ = new TMsgTable(false); // 錯誤訊息對照表
   SetMsgCenter(MsgC_);
   for(int L = 0; L < numofele(TrxSeg_); L++)
      TrxSeg_[L] = 0;
   ReloadSeg_  = 0;
   SegDefs_    = GetSegDefs();
   LoadTradeSegs();

   TUtilComm::ConsMsg("Load TwOrdCenter");

   OrdCenter_  =new TOrdCenter(GetProcLogPath(), HostID_);
   OrdFlow_    =new TOrdFlow(GetProcLogPath(),*OrdCenter_);
   SysOrds_    =&(OrdCenter_->GetSysOrds());

   // cooper {
   FIX::Stk::TTradeMgr& tradeMgr = FIX::Stk::TTradeMgr::GetInst();
   tradeMgr.Reg(&TwseFstUnitCreater, this);
   tradeMgr.Reload(GetProcLogPath().c_str());
   // } cooper

   ScheduleMgr_= new TScheduleMgr(std::string(GetLogPath()+GetProcName()+"/ModifyLog/ScheduleMgr.log").c_str());
   SetScheduleMgr(ScheduleMgr_);
   DoRegSchedule();
//   SysInfo_    = new TSysInfoPage(ScheduleMgr_, OrdCenter_); 
   Scheduler_  = new TScheduler(*OrdCenter_/*,SysInfo_*/);
   bxSesMgr_   = new Kway::Tw::bx::TbxSesMgr(Scheduler_, OrdCenter_);
   Scheduler_->SetSender((TSenderBase*)bxSesMgr_);
   Scheduler_->SetMarketPh(SegDefs_);
   SetOrdFlow();
   //Kway::ErrLog("<Main01::Main01> new TOrdSesMgr");
   OrdSesMgr_  = new TOrdSesMgr(/*SysInfo_, */OrdFlow_, signon);
   OrdCenterVA_= new TViewAreaMgr();
   SysOrdVACreater_   = new TSysOrdVACreater(OrdCenter_->GetSysOrds());
   BrokerOrdVACreater_= new DfTBrokerVACreater(OrdCenter_->GetBrokerOrds());
//   SysSettingVA_.Add(std::string("0.系統狀態"),*SysInfo_);
//   SysSettingVA_.Add(std::string("1.定時工作管理員"),*ScheduleMgr_);
   SetOrdCenter();

   SetVA();

   // cooper
   if (MainIni_) {
      delete MainIni_;
      MainIni_ = NULL;
   }
}
//---------------------------------------------------------------------------
K_dtor Main01::~Main01()
{
   OrdCenterVA_->Del(*SysOrdVACreater_);
   OrdCenterVA_->Del(*BrokerOrdVACreater_);
   delete SysOrdVACreater_;
   delete BrokerOrdVACreater_;

   const TSysMgrObjs& sys = GetSysMgrObjs();
   sys.SysVAMgr_->Del(*OrdCenterVA_);
   sys.SysVAMgr_->Del(*ScheduleMgr_);
//   sys.SysVAMgr_->Del(*MsgC_->GetVA());
   sys.SysVAMgr_->Del(FIX::Stk::TTradeMgr::GetInst().GetVAC());

   sys.SysVAMgr_->Del(*ExcMgr_->GetVAMgr());
//   SysSettingVA_.Del(*ScheduleMgr_);
//   SysSettingVA_.Del(*SysInfo_);

   delete OrdCenterVA_;
   Scheduler_->SetSender((TSenderBase*)0);
   for(int L = 0; L < numofele(TrxSeg_); L++){
      if(TrxSeg_[L] != 0){
         ScheduleMgr_->Erase((TScheduleBase*)TrxSeg_[L]);
         delete TrxSeg_[L];
      }
   }
   if(ReloadSeg_ != 0){
      ScheduleMgr_->Erase((TScheduleBase*)ReloadSeg_);
      delete ReloadSeg_;
   }
   delete bxSesMgr_;
   delete Scheduler_;
   delete OrdSesMgr_;

   FIX::Stk::TTradeMgr::GetInst().Cleanup();

   delete OrdFlow_;
   delete ScheduleMgr_;
   delete OrdCenter_;
   SetMsgCenter((TMsgTable*)0);
   delete MsgC_;
   if(ExcMgr_)
      ExcMgr_->Alert("XX9999");
   SetExcMgr(0);
   delete ExcMgr_;
   fprintf(stderr, "XTwse closed!\n");
}
//---------------------------------------------------------------------------
K_mf(void) Main01::SetOrdFlow()
{
   if(OrdFlow_==0) return;
   OrdFlow_->AddFlowStep(cid_New, *Scheduler_);
   OrdFlow_->AddFlowStep(cid_Del, *Scheduler_);
   OrdFlow_->AddFlowStep(cid_Qry, *Scheduler_);
   OrdFlow_->AddFlowStep(cid_ChgQty, *Scheduler_);
}
//---------------------------------------------------------------------------
K_mf(void) Main01::SetOrdCenter()
{
   if(OrdCenter_==0) return;
   OrdCenterVA_->Add("全部委託",*SysOrdVACreater_);
   OrdCenterVA_->Add("委託索引",*BrokerOrdVACreater_);
}
//---------------------------------------------------------------------------
K_mf(void) Main01::SetVA()
{
   const TSysMgrObjs&  sys=GetSysMgrObjs();
   sys.SysVAMgr_->Add("委託資料中心",*OrdCenterVA_);
   sys.SysVAMgr_->Add("換盤排程作業",*ScheduleMgr_);
   sys.SysVAMgr_->Add("事件管理員",*ExcMgr_->GetVAMgr());
//   sys.SysVAMgr_->Add("錯誤碼對照表",*MsgC_->GetVA());
   sys.SysVAMgr_->Add("Fix委託資料", FIX::Stk::TTradeMgr::GetInst().GetVAC());

   TStrings vacips;
   //TKIniFile KIniFile(std::string(GetConfPath()+"/Main.ini").c_str());
   //int cnt = KIniFile.ReadStrings("Main", "VACSvrAddr",vacips);
   int cnt = MainIni_->GetVACSvrAddr(vacips);

   for(int L = 0; L < cnt; L++)
      if(!vacips[L].empty()) {
         const char* pos = strchr(vacips[L].c_str(),':');
         if(pos)
            sys.SesMgr_.get()->AddSetting("SignonDB-Mgr",GetProgName()+"-"+GetProcName(),"EvDevTcpCli",vacips[L]);
         else
            sys.SesMgr_.get()->AddSetting("SignonDB-Mgr",GetProgName()+"-"+GetProcName(),"EvDevTcpSvr",vacips[L]);
            
         TUtilComm::ConsMsg("SignonDB-Mgr: %s", vacips[L].c_str());
      }
}
//---------------------------------------------------------------------------
K_mf(void) Main01::LoadTradeSegs()
{
   TKIniFile KIniFile(std::string(GetConfPath()+"/TradeSegment.ini").c_str());
   for(int L = 0; L < tk_None; L++){
      SegDefs_->Tse_.Seg_[L].SubSys_ = GetSubSystemTK(m_Tse, (ETradeKind)L);
      if(SegDefs_->Tse_.Seg_[L].SubSys_.is_0())
         continue;
      std::string section = SegDefs_->Tse_.Seg_[L].SubSys_.as_string();
      SegDefs_->Tse_.Seg_[L].BTime_.assign(KIniFile.ReadString(section, "BTime","00:00:00"));
      if(L != tk_Normal && SegDefs_->Tse_.Seg_[L].BTime_ != TKTime(0)) // 非整股時段則提前5分鐘換盤
         SegDefs_->Tse_.Seg_[L].BTime_.IncMin(-5);
      SegDefs_->Tse_.Seg_[L].ETime_.assign(KIniFile.ReadString(section, "ETime","00:00:00"));
      SegDefs_->Tse_.Seg_[L].Weight_     = KIniFile.ReadInt(section, "Weight", 1);
      SegDefs_->Tse_.Seg_[L].IsOpen_     = (TKTime::Now() < SegDefs_->Tse_.Seg_[L].ETime_ && 
                                       SegDefs_->Tse_.Seg_[L].BTime_ <= TKTime::Now());
      SegDefs_->Otc_.Seg_[L].SubSys_ = GetSubSystemTK(m_Otc, (ETradeKind)L);
      if(SegDefs_->Otc_.Seg_[L].SubSys_.is_0())
         continue;
      section = SegDefs_->Otc_.Seg_[L].SubSys_.as_string();
      SegDefs_->Otc_.Seg_[L].BTime_.assign(KIniFile.ReadString(section, "BTime","00:00:00"));
      if(L != tk_Normal && SegDefs_->Otc_.Seg_[L].BTime_ != TKTime(0)) // 非整股時段則提前5分鐘換盤
         SegDefs_->Otc_.Seg_[L].BTime_.IncMin(-5);
      SegDefs_->Otc_.Seg_[L].ETime_.assign(KIniFile.ReadString(section, "ETime","00:00:00"));
      SegDefs_->Otc_.Seg_[L].Weight_     = KIniFile.ReadInt(section, "Weight", 1);
      SegDefs_->Otc_.Seg_[L].IsOpen_     = (TKTime::Now() < SegDefs_->Otc_.Seg_[L].ETime_ && 
                                       SegDefs_->Otc_.Seg_[L].BTime_ <= TKTime::Now());
   }
}
//---------------------------------------------------------------------------
K_mf(void) Main01::TrSegmentChg(TKMemo& mo)
{
   TKTime ktm = TKTime::Now();
   bool origt, newt, origo, newo;
   for(int L = 0; L < tk_None; L++){
      origt = SegDefs_->Tse_.Seg_[L].IsOpen_;
      SegDefs_->Tse_.Seg_[L].IsOpen_ = (ktm < SegDefs_->Tse_.Seg_[L].ETime_ && SegDefs_->Tse_.Seg_[L].BTime_ <= ktm);
      newt = SegDefs_->Tse_.Seg_[L].IsOpen_;
      origo = SegDefs_->Otc_.Seg_[L].IsOpen_;
      SegDefs_->Otc_.Seg_[L].IsOpen_ = (ktm < SegDefs_->Otc_.Seg_[L].ETime_ && SegDefs_->Otc_.Seg_[L].BTime_ <= ktm);
      newo = SegDefs_->Otc_.Seg_[L].IsOpen_;
      switch(L){
         case tk_Normal:
            if((origt == false && newt == true) || (origo == false && newo == true))
               if(ExcMgr_)
                  ExcMgr_->Alert("XX0011");
            if((origt == true && newt == false) || (origo == true && newo == false))
               if(ExcMgr_)
                  ExcMgr_->Alert("XX0014");
            break;
         case tk_Odd:
            if((origt == false && newt == true) || (origo == false && newo == true))
               if(ExcMgr_)
                  ExcMgr_->Alert("XX0012");
            if((origt == true && newt == false) || (origo == true && newo == false))
               if(ExcMgr_)
                  ExcMgr_->Alert("XX0015");
            break;
         case tk_Fix:
            if((origt == false && newt == true) || (origo == false && newo == true))
               if(ExcMgr_)
                  ExcMgr_->Alert("XX0013");
            if((origt == true && newt == false) || (origo == true && newo == false))
               if(ExcMgr_)
                  ExcMgr_->Alert("XX0016");
            break;
         default:
            break;
      }
   }
   Scheduler_->SetMarketPh(SegDefs_);
   mo.assign("己觸發");
}
//---------------------------------------------------------------------------
K_mf(void) Main01::ReSchedule(TKMemo& mo)
{
   LoadTradeSegs();
   for(int L = 0; L < numofele(TrxSeg_); L++){
      if(TrxSeg_[L] != 0){
         ScheduleMgr_->Erase((TScheduleBase*)TrxSeg_[L]);
         delete TrxSeg_[L];
      }
   }
   DoRegSchedule();
   mo.assign("盤別資訊已重新載入");
}
//---------------------------------------------------------------------------
K_mf(void) Main01::DoRegSchedule()
{
   TKTime nulltime;
   nulltime.assign("00:00:00");
   static const char* TrxStr[] = {"TSE整股交易開始","TSE整股交易結束",
                                  "OTC整股交易開始","OTC整股交易結束",
                                  "TSE零股交易開始","TSE零股交易結束",
                                  "OTC零股交易開始","OTC零股交易結束",
                                  "TSE盤後交易開始","TSE盤後交易結束",
                                  "OTC盤後交易開始","OTC盤後交易結束"};
   for(int tk = 0; tk < 3; tk++){
      if(GetProcMkt() == m_Tse){
         if(SegDefs_->Tse_.Seg_[tk].BTime_ != nulltime){
            TrxSeg_[tk*4]   = new TSchedObj(this,TrxStr[tk*4  ],SegDefs_->Tse_.Seg_[tk].BTime_.as_string().c_str(),&this_type::TrSegmentChg);
            TrxSeg_[tk*4+1] = new TSchedObj(this,TrxStr[tk*4+1],SegDefs_->Tse_.Seg_[tk].ETime_.as_string().c_str(),&this_type::TrSegmentChg);
            ScheduleMgr_->Reg((TScheduleBase*)TrxSeg_[tk*4  ]);
            ScheduleMgr_->Reg((TScheduleBase*)TrxSeg_[tk*4+1]);
         }else{
            TrxSeg_[tk*4]   = 0;
            TrxSeg_[tk*4+1] = 0;
         }
      }else if(GetProcMkt() == m_Otc){
         if(SegDefs_->Otc_.Seg_[tk].BTime_ != nulltime){
            TrxSeg_[tk*4+2] = new TSchedObj(this,TrxStr[tk*4+2],SegDefs_->Otc_.Seg_[tk].BTime_.as_string().c_str(),&this_type::TrSegmentChg);
            TrxSeg_[tk*4+3] = new TSchedObj(this,TrxStr[tk*4+3],SegDefs_->Otc_.Seg_[tk].ETime_.as_string().c_str(),&this_type::TrSegmentChg);
            ScheduleMgr_->Reg((TScheduleBase*)TrxSeg_[tk*4+2]);
            ScheduleMgr_->Reg((TScheduleBase*)TrxSeg_[tk*4+3]);
         }else{
            TrxSeg_[tk*4+2] = 0;
            TrxSeg_[tk*4+3] = 0;
         }
      }
   }
   if(GetProcMkt() == m_Tse){
      if(SegDefs_->Tse_.Seg_[0].BTime_ != nulltime && ReloadSeg_ == 0){
         TKTime ReloadTime = SegDefs_->Tse_.Seg_[0].ETime_;
         ReloadTime.IncMin(1);
         ReloadSeg_ = new TSchedObj(this,"重新載入盤別資訊",ReloadTime.as_string().c_str(),&this_type::ReSchedule);
         ScheduleMgr_->Reg((TScheduleBase*)ReloadSeg_);
      }
   }else{
      if(SegDefs_->Otc_.Seg_[0].BTime_ != nulltime && ReloadSeg_ == 0){
         TKTime ReloadTime = SegDefs_->Otc_.Seg_[0].ETime_;
         ReloadTime.IncMin(1);
         ReloadSeg_ = new TSchedObj(this,"重新載入盤別資訊",ReloadTime.as_string().c_str(),&this_type::ReSchedule);
         ScheduleMgr_->Reg((TScheduleBase*)ReloadSeg_);
      }
   }
}
//---------------------------------------------------------------------------
