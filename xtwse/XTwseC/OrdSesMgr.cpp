/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "MesRegs.h"
#include "ExtDef.h"
#include "ExcMgrBase.h"
#include "OrdSesMgr.h"
#include "SysMgrObjs.h"
#include "TwseType.h"
#include "TwseSesApaSmp.h"
#include "TwseExcCode.h"
#include "TwseIniFile.h"
#include "TwseFstAdpt.h"
//---------------------------------------------------------------------------
static Kway::TSesBase* TwseSesApaSmpCreater (const Kway::TSesReg&, Kway::TSesMgr& sesMgr)
{
   TOrdSesMgr&              ordSesMgr = static_cast<TOrdSesMgr&>(sesMgr);
   Kway::Tw::Stk::TOrdFlow* of        = ordSesMgr.GetOrdFlow();

   return new TTwseSesApaSmp(of);
}
//---------------------------------------------------------------------------
static const Kway::TSesReg SesRegs[] = {
   { "TWSE Appia Socket", TwseSesApaSmpCreater },
};


/////////////////////////////////////////////////////////////////////////////
// class TOrdSesMgr impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TOrdSesMgr::TOrdSesMgr (/*TSysInfoPage* sysinfo, */TOrdFlow* of, TSignonSvrDB* sdb)
   : inherited(std::string(GetProcLogPath()+"/ModifyLog/OrdSesMgr.log").c_str())
   , OrdFlow_  (of)
   , OrdCenter_(&(of->GetOrdCenter()))
   , SignonDB_ (sdb)
{
//   SysInfo_ = sysinfo;
   const TSysMgrObjs&   sys = GetSysMgrObjs();
   Reg(Kway::TMesRegs::GetDefaultMesRegs());
   Reg(SesRegs, SesRegs+numofele(SesRegs));
   OrdVA_.Add(std::string("1.MW�s�u���A"),  GetStatusVACreater());
   OrdVA_.Add(std::string("2.MW�s�u�]�w"),  GetSettingVACreater());
   sys.SysVAMgr_->Add(std::string("MW�s�u�޲z��"), OrdVA_);
   // �O�_�O DR
   TTwseDrIni iniDr;
   IsDR_ = iniDr.IsDR();

   Timer_ = new TOTimer(this);
   Timer_->Start(1000, &this_type::OnTimer);  // �C10���ˬd�@��
}
//---------------------------------------------------------------------------
// �b�Ұ� Appia �s�u���ˬd.�@������
K_mf(bool) TOrdSesMgr::PreChkNormal ()
{
   TExcMgrBase* excMgr = GetExcMgr();

   //==============================================
   // ���ݥ���@�� TmpSes �s�W�u
   if (IsTMPReady() == false) {
      // �o�e�ƥ�
      if (excMgr)
         excMgr->Alert(TTwseExcCode::GetXTwseCode(tec_AppiaMwWaitTmpSesReady), TTwseExcCode::LinkState);

      TUtilComm::ConsMsg("TMPSes �n�J�@�~�|������, ���ݤ�");
      return false;
   }

   //==============================================
   // �P�_�{�b�O�_���b���� TmpSes �����d�ߧ@�~�����������A
   FIX::Stk::TTradeMgr& tradeMgr = FIX::Stk::TTradeMgr::GetInst();

   if (tradeMgr.GetUnitSize() > 0) {
      FIX::Stk::TTradeUnitPtr unitPtr = tradeMgr.GetFirstUnit();
      FIX::Stk::TTradeUnit*   unit    = unitPtr.get();

      if (unit) {
         TTwseFstAdpt* adptTwse = static_cast<TTwseFstAdpt*>(unit->GetFixTradeAdpt());

         if (adptTwse->IsReady() == false) {
            // �o�e�ƥ�
            if (excMgr)
               excMgr->Alert(TTwseExcCode::GetXTwseCode(tec_AppiaMwWaitTmpSesReadyAll), TTwseExcCode::LinkState);

            TUtilComm::ConsMsg("TMPSes �n�J�@�~�|����������, ���ݤ�");
            return false;
         }
      }
   }

   return true;
}
//---------------------------------------------------------------------------
// �b�Ұ� Appia �s�u���ˬd.DR����
K_mf(bool) TOrdSesMgr::PreChkDR ()
{
   if (!IsDRTMPReady()) {
      TUtilComm::ConsMsg("DR �ݩe�UŪ�ɧ@�~�|������,���ݤ�!");
      return false;
   }
   return true;
}
//---------------------------------------------------------------------------
K_mf(void) TOrdSesMgr::OnTimer(TTimerBase*)
{
   if (!IsDR_) {
      if (!PreChkNormal())
         return;
   }
   else {
      if (!PreChkDR())
         return;
   }

   Timer_->Stop();

   // cooper { ��Φۦ�s�W���覡
   //ReloadSetting(std::string(GetProcConfPath()+"OrdSes.ini"),true);
   TTwseMainIni iniMain;
   std::string  sesName     ("TWSE Appia Socket");
   std::string  mesName     ("EvDevTcpCli");
   std::string  strTandSesID(GetProcName());
   std::string  sesSetting;
   char         buf[512], hostName[256];

   //==============================================
   // sesSetting, �ϥ� TandSesID_HostName ��@ MiddlewareID
   if (gethostname(hostName, sizeof(hostName)) != 0)
      hostName[0] = '\0';

   sprintf(buf, "%s_%s", strTandSesID.c_str(), hostName);
   sesSetting.assign(buf);

   //==============================================
   // mesSetting
   // M0 => ���X��
   // R? => RetrySeconds
   sprintf(buf, "%s:%s,M0R%d", iniMain.GetAppiaSvrIP         ().c_str(),
                               iniMain.GetAppiaSvrPort       ().c_str(),
                               iniMain.GetSocketRetryInterval());

   inherited::AddSetting(sesName, sesSetting, mesName, std::string(buf));
   // } cooper

   delete Timer_;

   //Kway::ErrLog("<TOrdSesMgr::OnTimer> leave");
}
//---------------------------------------------------------------------------
