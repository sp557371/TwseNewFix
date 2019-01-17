//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "ReqMgr.h"
//#include "KwayReq.h"
//#include "Req3T.h"
//#include "SystexReq.h"
//#include "SystexF06.h"
#include "TwseReq.h"
#include "FTCenter.h"
#include "MesRegs.h"
#include "SysMgrObjs.h"
#include "FTSysSetting.h"
#include "FileLog.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx;
using namespace Kway::Tw::Bx::FT;
//---------------------------------------------------------------------------
K_fn(const TAPIMgr&) Kway::Tw::Bx::FT::GetAPI()
{
  static TAPIMgr apiMgr;
  return apiMgr;
}
//---------------------------------------------------------------------------
static Kway::TSesBase* TTwseReqSes_Creater(const Kway::TSesReg&, Kway::TSesMgr& aSesMgr)
{
  TFTCenter&   ft    = const_cast<TFTCenter&>  (GetCenter());
  TAPIMgr*     mgr   = static_cast<TAPIMgr*>(&aSesMgr);
  return dynamic_cast<Kway::TSesBase*>(new TTwseReqSes(mgr, &ft, GetBxMgr()));
}
//---------------------------------------------------------------------------
static const Kway::TSesReg  SesReg[] = {
    {"TWSE作業介面", TTwseReqSes_Creater },
};
//---------------------------------------------------------------------------
K_ctor TAPIMgr::TAPIMgr()
    : inherited()
{
  static bool isCreate;
  if(isCreate == false)
  {
    Reg(Kway::TMesRegs::GetDefaultMesRegs());
    Reg(SesReg, SesReg+numofele(SesReg));
    ReloadSetting(GetProcConfPath()+"/API.Ses");

     const TSysMgrObjs& sys = GetSysMgrObjs();
     VAMgr_.Add("連線狀態", GetStatusVACreater());
     VAMgr_.Add("連線設定", GetSettingVACreater());
     sys.SysVAMgr_->Add("檔案傳輸要求", VAMgr_);

     std::string fname = GetProcLogPath()+"/ApiCall.log";
     TFileMode fmode = TFileMode(fmWrite|fmAppend|fmOpenAlways|fmCreatePath);
     Log_.Open(fname.c_str(), fmode);

     isCreate = true;
  }
}
//---------------------------------------------------------------------------
K_dtor TAPIMgr::~TAPIMgr()
{
   const TSysMgrObjs& sys = GetSysMgrObjs();
   sys.SysVAMgr_->Del(VAMgr_);
   VAMgr_.Del(GetStatusVACreater());
   VAMgr_.Del(GetSettingVACreater());
   delete BrkSeqsRec_;
   delete BrkF06SeqsRec_;
}
//---------------------------------------------------------------------------
K_mf(bool) TAPIMgr::WriteLog(const char* data, bool input)
{
   if(Log_.IsOpened())
   {
     static char buf[255];
     sprintf(buf, "%s.%s %s %s\n", TKTime::Now().as_string().c_str(), 
       TKTimeMS::Now().as_string().c_str(), input ? "<====":"====>", data);
     Log_.Write(buf);
     return true;
   }
   return false;
}
//---------------------------------------------------------------------------
K_mf(Kway::dword) TAPIMgr::GetSeqNo(TBrokerID brk)
{
   if(BrkSeqsRec_ == 0)
      BrkSeqsRec_ = new TBrkSeqsRec(BrkSeqs_,std::string(GetProcLogPath()+"/BrkSeqs.rec").c_str());
   return BrkSeqs_.fetch(brk)->second;
}
//---------------------------------------------------------------------------
K_mf(void) TAPIMgr::WriteSeqNo(TBrokerID brk, Kway::dword seq)
{
   TBrkSeqs::iterator iter = BrkSeqs_.fetch(brk);
   if(iter->second >= seq)
      return;
   TBrkSeqs::updater u = BrkSeqs_.begin_update(iter);
   *u = seq;
}
//---------------------------------------------------------------------------
K_mf(Kway::dword) TAPIMgr::GetF06SeqNo(TBrokerID brk, TMarket mkt)
{
   TBrkID brkid;
   brkid.assign(brk.as_string());
   if(BrkF06SeqsRec_ == 0)
      BrkF06SeqsRec_ = new TBrkF06SeqsRec(BrkF06Seqs_,std::string(GetProcLogPath()+"/BrkF06Seqs.rec").c_str());
   return BrkF06Seqs_.fetch(TBrkMkt(brkid,mkt))->second;
}
//---------------------------------------------------------------------------
K_mf(void) TAPIMgr::WriteF06SeqNo(TBrokerID brk, TMarket mkt, Kway::dword seq)
{
   TBrkID brkid;
   brkid.assign(brk.as_string());
   TBrkF06Seqs::iterator iter = BrkF06Seqs_.fetch(TBrkMkt(brkid,mkt));
   if(iter->second >= seq)
      return;
   TBrkF06Seqs::updater u = BrkF06Seqs_.begin_update(iter);
   *u = seq;
}
//---------------------------------------------------------------------------




