/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "CtrlSesMgr.h"
#include "MesRegs.h"
#include "ExtDef.h"
#include "CtrlSesTandem.h"
#include "CtrlSesAppia.h"
#include "CtrlIniFile.h"

//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// Session Creater
/////////////////////////////////////////////////////////////////////////////
static TSesBase* SesTandemCreater (const TSesReg&, TSesMgr& sesMgr)
{
   TCtrlSesMgr& ctrlSesMgr = static_cast<TCtrlSesMgr&>(sesMgr);
   return new TCtrlSesTandem(&ctrlSesMgr);
}
//---------------------------------------------------------------------------

static TSesBase* SesAppiaCreater (const TSesReg&, TSesMgr& sesMgr)
{
   TCtrlSesMgr& ctrlSesMgr = static_cast<TCtrlSesMgr&>(sesMgr);
   return new TCtrlSesAppia(&ctrlSesMgr);
}
//---------------------------------------------------------------------------

static const TSesReg SesRegs[] = {
   { "XCtrl Tandem連線", SesTandemCreater },
   { "XCtrl Appia連線",  SesAppiaCreater  },
};


/////////////////////////////////////////////////////////////////////////////
// class TAppiaSesMgr impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TCtrlSesMgr::TCtrlSesMgr ()
   : inherited    (std::string(GetProcLogPath()+"/ModifyLog/AppiaSesMgr.log").c_str())
   //, SignonDB_    (sdb)
   //, SesAppia_    (NULL)
   , RsReqResuGoh_(true)
{
   Reg(Kway::TMesRegs::GetDefaultMesRegs());
   Reg(SesRegs, SesRegs+numofele(SesRegs));

   OrdVA_.Add(std::string("1.連線狀態"), GetStatusVACreater ());
   OrdVA_.Add(std::string("2.連線設定"), GetSettingVACreater());

   Timer_ = new TOTimer(this);
   // cooper
   //Timer_->Start(1000*10, &this_type::OnTimer);  // 每10秒檢查一次
   Timer_->Start(1000, &this_type::OnTimer);  // 每10秒檢查一次
}
//---------------------------------------------------------------------------

K_dtor TCtrlSesMgr::~TCtrlSesMgr ()
{
}
//---------------------------------------------------------------------------

K_mf(void) TCtrlSesMgr::OnTimer(TTimerBase*)
{
   Timer_->Stop();
   
   TStrings grps,sesids;
   TKIniFile IniFileM(std::string(GetConfPath()+"/Main.ini").c_str());
   int cnt = IniFileM.ReadStrings("Main","AppiaGroup",grps);
   for(int L = 0; L < cnt; L++){
      int qty = IniFileM.ReadStrings("SESSIONID-APPIAGROUP",grps[L],sesids);
      for(int L1 = 0; L1 < qty; L1++)
         SesMap_.insert(std::make_pair(sesids[L1],grps[L]));
   }

   //ReloadSetting(std::string(GetProcConfPath()+"AppiaSes.ini"),true);
   const TTwseMainIni::SCont& iniMainCont = TTwseMainIni::GetContC();
         TCtrlIni             iniCtrl;
         char                 mesSett[512], sesSett[128], hostName[128];

   //==============================================
   // XCtrl Appia連線

   // 使用 [程式名稱_HostName] 當作 MiddlewareID
   if (gethostname(hostName, sizeof(hostName)) != 0)
      hostName[0] = '\0';

   sprintf(sesSett, "%s_%s", GetProcName().c_str(), hostName);

   // 10.204.101.51:19105R5
/*
   sprintf(mesSett, "%s:%sR%d", iniMainCont.Main_.AppiaSvrIP_.c_str(),
                                iniCtrl.GetXCtrlAppiaMwPort().c_str(),
                                iniMainCont.Main_.SocketRetryInterval_);

   // SesName, SesSetting, MesName, MesSetting
   inherited::AddSetting(std::string("XCtrl Appia連線"),
                         std::string(sesSett),
                         std::string("EvDevTcpCli"),
                         std::string(mesSett));
*/

   //==============================================
   // XCtrl Tandem連線
   // 17800
   std::string  apaMwName;
   std::string  sesSetting;
   std::string  mesSetting;
   std::string  sesName  ("XCtrl Appia連線");
   std::string  mesName  ("EvDevTcpCli");
   TKIniFile IniFile(std::string(GetConfPath()+"/XCtrl.ini").c_str());
   cnt = IniFile.ReadStrings("Summary","AppiaGroup",AppiaGrps_);
   char buf[128];
   for(int L = 0; L < cnt; L++){
      mesSetting=IniFile.ReadString(AppiaGrps_[L],"AppiaSvrAddr","127.0.0.1:9108");
      sprintf(buf, "%s_%s_%s", AppiaGrps_[L].c_str(),GetProcName().c_str(), hostName);
      sesSetting.assign(buf);
      inherited::AddSetting(sesName, sesSetting, mesName, mesSetting);
   }
   //==============================================
   sprintf(mesSett, "%s", iniCtrl.GetTandemCommandPort().c_str());

   inherited::AddSetting(std::string("XCtrl Tandem連線"),
                         std::string(),
                         std::string("EvDevTcpSvr"),
                         std::string(mesSett));

   delete Timer_;
}
//---------------------------------------------------------------------------
K_mf(void) TCtrlSesMgr::SetAppiaSes (std::string appgrp,TCtrlSesAppia* p)
{
	if(p == 0){
		TAppGrpMapi iter = AppGrpMap_.find(appgrp);
		if(iter != AppGrpMap_.end())
		   AppGrpMap_.erase(iter);
		return;
   }
	AppGrpMap_.insert(std::make_pair(appgrp,p));
}
//---------------------------------------------------------------------------
K_mf(TCtrlSesAppia*) TCtrlSesMgr::GetAppiaSes (std::string appgrp)
{
	TAppGrpMapi iter = AppGrpMap_.find(appgrp);
	if(iter != AppGrpMap_.end())
	   return iter->second;
	return 0;
}
//---------------------------------------------------------------------------
K_mf(TReqID) TCtrlSesMgr::FindNewReq(std::string appgrp)
{
	if(ReqID_.is_invalid()){
      if (RsReqResuGoh_->FindNewReq(ReqID_) != false){
	      AppGrpProc_.insert(std::make_pair(appgrp,TProcResu(epr_Process)));
         return ReqID_;
      }
	}else{
	   TAppGrpProci iter = AppGrpProc_.find(appgrp);
	   if(iter == AppGrpProc_.end()){
	      AppGrpProc_.insert(std::make_pair(appgrp,TProcResu(epr_Process)));
	      return ReqID_;
	   }
	}
	return TReqID();
}
//---------------------------------------------------------------------------
K_mf(void) TCtrlSesMgr::SetAppGrpState(std::string appgrp,TProcResu st)
{
	TAppGrpProci iter = AppGrpProc_.find(appgrp);
	if(iter != AppGrpProc_.end() && st > iter->second)
	   iter->second = st;
	for(iter = AppGrpProc_.begin(); iter != AppGrpProc_.end(); iter++){
	   if(iter->second.get_orig() < epr_Finish)
	      return;
	}
   // 能傳送的要求都傳送且已得到回報, 開始整理結果
	if(AppGrpProc_.size() == AppGrpMap_.size() && (!ReqID_.is_invalid())){
	   AppGrpProc_.clear();

      TRsReqResuHnd      hnd(RsReqResuGoh_.GetPtr(), ReqID_);
      const SRsReqResu1* cpDat1   = hnd.FindDat1();
      if (!cpDat1->AllMarketFlag_.is_true())
         if(hnd.MoveFirst2())
            do {
               const SRsReqResu2* cpDat2 = hnd.FindDat2();
               //20130220 eric 未填入Appia Group 的作業項目表示未傳送,
               // 原因可能有1. SessionID 錯誤  2.Appia MW 連線失敗 3. 未設定相對的Appia 連線
               if (cpDat2->AppiaGrp_.empty()) {
                  SRsReqResu2& udDat2 = hnd.BeginUpdate2();
                  TSesMapi iter = SesMap_.find(hnd.GetKey2().as_full());
                  if(iter == SesMap_.end()){
                     udDat2.StatusCode1_.assignT(esc_FixSesIDError);
                  }else{
                     for(int i = 0; i < AppiaGrps_.size(); i++){
                        if(AppiaGrps_[i] == iter->second){
                           udDat2.StatusCode1_.assignT(esc_AppiaNotConn);
                           break;
                        }
                     }
                  udDat2.StatusCode1_.assignT(esc_AppiaNotConn/*esc_NoAppiaConn*/);
                  }
               }
            } while (hnd.MoveNext2());
      SRsReqResu1& udDat1 = hnd.BeginUpdate1();
      udDat1.ProcResu_.assignT(epr_Finish);
      ReqID_.clear();
   }
}
//---------------------------------------------------------------------------
K_mf(TProcResu) TCtrlSesMgr::GetAppGrpState(std::string appgrp)
{
	TAppGrpProci iter = AppGrpProc_.find(appgrp);
	if(iter != AppGrpProc_.end())
	   return iter->second;
   return TProcResu();
}
//---------------------------------------------------------------------------
