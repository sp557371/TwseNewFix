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
   { "XCtrl Tandem�s�u", SesTandemCreater },
   { "XCtrl Appia�s�u",  SesAppiaCreater  },
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

   OrdVA_.Add(std::string("1.�s�u���A"), GetStatusVACreater ());
   OrdVA_.Add(std::string("2.�s�u�]�w"), GetSettingVACreater());

   Timer_ = new TOTimer(this);
   // cooper
   //Timer_->Start(1000*10, &this_type::OnTimer);  // �C10���ˬd�@��
   Timer_->Start(1000, &this_type::OnTimer);  // �C10���ˬd�@��
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
   // XCtrl Appia�s�u

   // �ϥ� [�{���W��_HostName] ��@ MiddlewareID
   if (gethostname(hostName, sizeof(hostName)) != 0)
      hostName[0] = '\0';

   sprintf(sesSett, "%s_%s", GetProcName().c_str(), hostName);

   // 10.204.101.51:19105R5
/*
   sprintf(mesSett, "%s:%sR%d", iniMainCont.Main_.AppiaSvrIP_.c_str(),
                                iniCtrl.GetXCtrlAppiaMwPort().c_str(),
                                iniMainCont.Main_.SocketRetryInterval_);

   // SesName, SesSetting, MesName, MesSetting
   inherited::AddSetting(std::string("XCtrl Appia�s�u"),
                         std::string(sesSett),
                         std::string("EvDevTcpCli"),
                         std::string(mesSett));
*/

   //==============================================
   // XCtrl Tandem�s�u
   // 17800
   std::string  apaMwName;
   std::string  sesSetting;
   std::string  mesSetting;
   std::string  sesName  ("XCtrl Appia�s�u");
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

   inherited::AddSetting(std::string("XCtrl Tandem�s�u"),
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
   // ��ǰe���n�D���ǰe�B�w�o��^��, �}�l��z���G
	if(AppGrpProc_.size() == AppGrpMap_.size() && (!ReqID_.is_invalid())){
	   AppGrpProc_.clear();

      TRsReqResuHnd      hnd(RsReqResuGoh_.GetPtr(), ReqID_);
      const SRsReqResu1* cpDat1   = hnd.FindDat1();
      if (!cpDat1->AllMarketFlag_.is_true())
         if(hnd.MoveFirst2())
            do {
               const SRsReqResu2* cpDat2 = hnd.FindDat2();
               //20130220 eric ����JAppia Group ���@�~���ت�ܥ��ǰe,
               // ��]�i�঳1. SessionID ���~  2.Appia MW �s�u���� 3. ���]�w�۹諸Appia �s�u
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
