#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "EvSesMgr.h"
#include "SesServer.h"
#include "ExtDef.h"
#include "KIniFile.h"
#include "SysMgrObjs.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw::Stk;
//---------------------------------------------------------------------------
K_ctor TEvSes_Base::TEvSes_Base(TSesMgr* sm)
   :Timer_(this)
{
   SesMgr_   = (TEvSesMgr*)sm;
   Ready_    = false;
}
//---------------------------------------------------------------------------
K_mf(int) TEvSes_Base::GetPacketSize(TEvPkt* pkt,bool f)
{
   return pkt->Len_.as_int();
}
//---------------------------------------------------------------------------
K_mf(void) TEvSes_Base::ResetTimer(size_t secs)
{
   Timer_.Stop();
   Timer_.Start(secs*1000, &this_type::OnTimer);
}
//---------------------------------------------------------------------------
K_mf(void) TEvSes_Base::OnMesLinkBroken(TMesBase* m, const std::string& str)
{
   Timer_.Stop();
   Ready_ = false;
   OnStateChanged("");
   char buf[512];
   sprintf(buf,"[%s] Link broken!", (ProcName_.empty()?IP_:ProcName_).c_str());
   SesMgr_->WriteLog(buf);
}
//---------------------------------------------------------------------------
K_mf(void) TEvSes_Base::SendCmdReq(TCmdReq& cmdreq)
{
   TEvPkt Pkt(ef_CmdReq);
   memcpy(&Pkt.CmdReq_, &cmdreq, sizeof(TCmdReq));
   SendP(Pkt);
}
//---------------------------------------------------------------------------
K_mf(void) TEvSes_Base::SendP(TEvPkt& pkt)
{
   char buf[512];
   ((char*)(&pkt))[pkt.Len_.as_int()] = 0;
   sprintf(buf," ==> [%s] %s", (ProcName_.empty()?IP_:ProcName_).c_str(),(char*)(&pkt));
   SesMgr_->WriteLog(buf);
   Send(pkt);
}
/////////////////////////////////////////////////////////////////////////////
K_ctor TEvCliSes::TEvCliSes(TSesMgr* sm)
   :inherited(sm)
{
}
//---------------------------------------------------------------------------
K_dtor TEvCliSes::~TEvCliSes()
{
   OnStateChanged("");
   SesMgr_->DelEvCliSes(this);
}
//---------------------------------------------------------------------------
K_mf(void) TEvCliSes::OnMesLinkReady(TMesBase*)
{
   IP_ = GetMes()->GetUID();
   char buf[512];
   sprintf(buf,"[%s] Connected!", IP_.c_str());
   SesMgr_->WriteLog(buf);
   TEvPkt Pkt(ef_Login);
   Pkt.ProgProc_.ProgName_.assign(GetProgName());
   Pkt.ProgProc_.ProcName_.assign(GetProcName());
   ResetTimer(HBTime);
   SendP(Pkt);
}
//---------------------------------------------------------------------------
K_mf(void) TEvCliSes::OnPktRecv(TEvPkt& pkt)
{
   char buf[512];
   sprintf(buf," <== [%s] %s", (ProcName_.empty()?IP_:ProcName_).c_str(),(char*)(&pkt));
   SesMgr_->WriteLog(buf);
   switch(pkt.Func_.as_int()){
      case ef_LoginEcho: 
         ProcName_ = pkt.ProgProc_.ProcName_.as_string();
         OnStateChanged("To: "+ProcName_);
         Ready_    = true;
         SesMgr_->RegEvCliSes(this);
         SesMgr_->DoRecover(this);
         break;
      case ef_CmdReq:
         GetExcMgr()->DoCmd(pkt.CmdReq_.ObjName_.as_string(), 
                            pkt.CmdReq_.CmdID_.as_int(),
                            pkt.CmdReq_.Msg_.as_string());
         break;
      case ef_EvEcho:
         break;
      case ef_HBEcho:
         break;
      default:
         break;
   }
}
//---------------------------------------------------------------------------
K_mf(void) TEvCliSes::OnTimer(TTimerBase*)
{
   TEvPkt Pkt(ef_HB);
   ResetTimer(HBTime);
   SendP(Pkt);
}
//---------------------------------------------------------------------------
K_mf(void) TEvCliSes::SendAlert(TEvAlert& evd, bool recover)
{
   TEvPkt Pkt(recover?ef_Recover:ef_EvAlert);
   memcpy(&Pkt.EvAlert_, &evd, sizeof(TEvAlert));
   ResetTimer(HBTime);
   SendP(Pkt);
}
/////////////////////////////////////////////////////////////////////////////
K_ctor TEvSvrSes::TEvSvrSes(TSesMgr* sm)
   :inherited(sm)
{
}
//---------------------------------------------------------------------------
K_dtor TEvSvrSes::~TEvSvrSes()
{
   SesMgr_->DelEvSvrSes(this);
}
//---------------------------------------------------------------------------
K_mf(void) TEvSvrSes::OnMesLinkReady(TMesBase*)
{
   SesMgr_->RegEvSvrSes(this);
   IP_ = GetMes()->GetUID();
   char buf[512];
   sprintf(buf,"[%s] Connected!", IP_.c_str());
   SesMgr_->WriteLog(buf);
}
//---------------------------------------------------------------------------
K_mf(void) TEvSvrSes::OnMesLinkBroken(TMesBase* m, const std::string& str)
{
   inherited::OnMesLinkBroken(m, str);
   TProcName gname, cname;
   gname.assign(ProgName_);
   cname.assign(ProcName_);
   SesMgr_->LinkStateChg(gname,cname,false);
}
//---------------------------------------------------------------------------
K_mf(void) TEvSvrSes::OnPktRecv(TEvPkt& pkt)
{
   ResetTimer(HBTime*2);
   char buf[512];
   sprintf(buf," <== [%s] %s", (ProcName_.empty()?IP_:ProcName_).c_str(),(char*)(&pkt));
   SesMgr_->WriteLog(buf);
   switch(pkt.Func_.as_int()){
      case ef_Login: {
         ProgName_ = pkt.ProgProc_.ProgName_.as_string();
         ProcName_ = pkt.ProgProc_.ProcName_.as_string();
         OnStateChanged("From:"+ProcName_);
         TEvPkt aPkt(ef_LoginEcho);
         aPkt.ProgProc_.ProcName_.assign(GetProcName());
         SendP(aPkt);
         SesMgr_->LinkStateChg(pkt.ProgProc_.ProgName_,pkt.ProgProc_.ProcName_,true);
         break;
      }
      case ef_Recover:
      case ef_EvAlert:{
         SesMgr_->CollectEv(pkt.EvAlert_,(pkt.Func_.as_int() == ef_Recover));
         TEvPkt aPkt(ef_EvEcho);
         SendP(aPkt);
         break;
      }
      case ef_HB:{
         TEvPkt aPkt(ef_HBEcho);
         SendP(aPkt);
         break;
      }
      case ef_CmdReq:
         GetExcMgr()->DoCmd(pkt.CmdReq_.ObjName_.as_string(), 
                            pkt.CmdReq_.CmdID_.as_int(),
                            pkt.CmdReq_.Msg_.as_string());
         break;
      default:
         break;
   }
}
//---------------------------------------------------------------------------
K_mf(void) TEvSvrSes::OnTimer(TTimerBase*)
{
   char buf[512];
   sprintf(buf,"[%s] Heartbeat timeout!", (ProcName_.empty()?IP_:ProcName_).c_str());
   SesMgr_->WriteLog(buf);
   Close();
}
/////////////////////////////////////////////////////////////////////////////
K_ctor TMsgSes::TMsgSes(TSesMgr* sm)
   :inherited(1024,(char*)std::string("\n").c_str())
{
   SesMgr_   = (TEvSesMgr*)sm;
}
//---------------------------------------------------------------------------
K_mf(void) TMsgSes::OnPktRecv(void* vbuf, int sz)
{
   if(!ByTelnet_){
      CurSeqNo_++;
      if(CurSeqNo_ <= LastSeqNo_)
         return;
   }else{
     MsgTimer_ = new TMsgTimer(this);
     MsgTimer_->Start(100, &this_type::OnTimer);
   }
   char buf[512]={0};
   memcpy(buf, vbuf, sz);
   TStrings fields;
   char*    Pos;
   char*    StartPos;
   if(sz <= 0)
      return;
   StartPos = buf;
   while((Pos = strchr(StartPos, ';'))){
     *Pos = 0;
     fields.push_back(std::string(StartPos));
     StartPos = Pos+1;
   }
   if(!ByTelnet_){
      LastSeqNo_ = CurSeqNo_;
      SesMgr_->SetMsgCnt(LastSeqNo_, LineNo_);
   }
   if(fields.size() < (emf_end-2))
      return;
   fields.push_back(std::string(StartPos));
   TEvAlert EvAlert;
   EvAlert.Market_.assign(fields[emf_Market]);
   EvAlert.Host_.assign(fields[emf_Host]);
   EvAlert.BrkID_.assign("----");
   EvAlert.ProgName_.assign(fields[emf_SubSys]);
   EvAlert.ProcName_.assign(fields[emf_Process]);
   EvAlert.Object_.assign(fields[emf_Object]);
   EvAlert.EvCode_.assign(fields[emf_EvCode]);
   if(fields.size() > (emf_end-1))
      EvAlert.Remark_.assign(fields[emf_Remark]);
   sprintf(buf,"%s %s",TKDate::Now().as_string(TKDate::sf_ELong).c_str(), fields[emf_Time].c_str());
   EvAlert.DateTime_.assign(buf);
   SesMgr_->CollectEv(EvAlert);
}
//---------------------------------------------------------------------------
K_mf(void) TMsgSes::OnTimer(TTimerBase*)
{
   MsgTimer_->Stop();
   delete MsgTimer_;
   Close();
}
//---------------------------------------------------------------------------
K_mf(bool) TMsgSes::SettingApply(const std::string& str)
{
   if(str[0]=='T'){
   	ByTelnet_ = true;
   	return true;
   }else
      ByTelnet_ = false;
   if(str.length() > 0){
      char_ary<char,16,' '> lineno(str);
      LineNo_    = lineno.as_int();
   }else
      LineNo_    = 0;
   LastSeqNo_ = SesMgr_->GetMsgCnt(LineNo_);
   CurSeqNo_  = 0;
   return true;
}
/////////////////////////////////////////////////////////////////////////////
static TSesBase* EvCliSes_Creater(const TSesReg&, TSesMgr& SesMgr)
{
   TEvCliSes *Ses=new Kway::TEvCliSes(&SesMgr);
   return (TSesBase*)Ses;
}
//---------------------------------------------------------------------------
static const TSesReg CliSesRegs[]=
{
   {"事件傳送連線",EvCliSes_Creater},
};
//---------------------------------------------------------------------------
static TSesBase* EvSvrSes_Creater(const TSesReg&, TSesMgr& SesMgr)
{
   TEvSvrSes *Ses=new Kway::TEvSvrSes(&SesMgr);
   return (TSesBase*)Ses;
}
//---------------------------------------------------------------------------
static TSesBase* EvMsgSes_Creater(const TSesReg&, TSesMgr& SesMgr)
{
   TMsgSes *Ses=new Kway::TMsgSes(&SesMgr);
   return (TSesBase*)Ses;
}
//---------------------------------------------------------------------------
static const TSesReg SvrSesRegs[]=
{
   {"事件接收連線",EvSvrSes_Creater},
   {"事件接收讀檔",EvMsgSes_Creater},
};
/////////////////////////////////////////////////////////////////////////////
K_ctor TEvSesMgr::TEvSesMgr(TEvCenter* ec, bool isagent)
   :TSesMgrR(std::string(GetProcLogPath()+"ModifyLog/EvSesMgr.log").c_str())
{
   Reg(Kway::TMesRegs::GetDefaultMesRegs());
   EvCenter_ = ec;
   MsgCntRec_= 0;
   Log_.Open(std::string(GetProcLogPath()+"EvSes.log").c_str(), TFileMode(fmAppend|fmCreatePath|fmOpenAlways));
   switch(GetProcMkt()){
      case m_Tse: EvAlert_.Market_.assign("T"); break;
      case m_Otc: EvAlert_.Market_.assign("O"); break;
      default   : EvAlert_.Market_.assign(" "); break;
   }
   EvAlert_.ProgName_.assign(GetProgName());
   EvAlert_.ProcName_.assign(GetProcName());
   TKIniFile IniFile(std::string(GetConfPath()+"/Main.ini").c_str());
   EvAlert_.Host_.clear();
   EvAlert_.Host_.assign(IniFile.ReadString("OpenMsg", "HostName",""));
   TStrings vacports;
   int cnt = IniFile.ReadStrings("Main", "EvSvrAddr",vacports);
   if(isagent){
      Reg(CliSesRegs, CliSesRegs+numofele(CliSesRegs));
      for(int L = 0; L < cnt; L++)
         if(!vacports[L].empty())
            AddSetting("事件傳送連線","","EvDevTcpCli",vacports[L]);
   }else{
      Reg(SvrSesRegs, SvrSesRegs+numofele(SvrSesRegs));
      for(int L = 0; L < cnt; L++)
         if(!vacports[L].empty()){
            char* pos = strchr(vacports[L].c_str(),':');
            if(pos)
               AddSetting("事件接收連線","","EvDevTcpSvr",pos+1);
         }
      std::string filepath = IniFile.ReadString("Main", "EvFilePath","./Logs/%4y%2m%2d/EvFile.log");
      AddSetting("事件接收讀檔","","File",filepath);
      std::string listenport = IniFile.ReadString("Main", "CronEvSvrAddr","7006");
      AddSetting("事件接收讀檔","T","EvDevTcpSvr",listenport);
   }
}
//---------------------------------------------------------------------------
K_dtor TEvSesMgr::~TEvSesMgr()
{
   Log_.Close();
   if(MsgCntRec_)
      delete MsgCntRec_;
}
//---------------------------------------------------------------------------
K_mf(bool) TEvSesMgr::DoAlert(TErrLevel sysstate, TErrCode ercode, TErrLevel erlvl, 
                              std::string& msg, std::string& obj, std::string& extmsg)
{
   if(ercode.as_string() == "000000")
      return true;
   EvAlert_.DateTime_.assign(TKDateTimeMS::Now().as_string());
   EvAlert_.EvCode_.assign(ercode.as_string());
   EvAlert_.Object_.assign(obj);
   EvAlert_.Remark_.assign(extmsg);
   EvAlert_.BrkID_.assign(GetProcBrkID()==TBrokerID()?"----":GetProcBrkID().as_string());
   if(EvSess_.size() > 0){
      for(TEvSess::iterator L = EvSess_.begin(); L != EvSess_.end(); L++)
         if((*L) && (*L)->IsReady())
            (*L)->SendAlert(EvAlert_);
   }else
      Evs_.push_back(EvAlert_);
   TCurrStat::iterator iter = CurrStat_.fetch(obj);
   iter->second = EvAlert_;
   return true;
}
//---------------------------------------------------------------------------
K_mf(void) TEvSesMgr::SendCmdReq(std::string& procname, std::string& objname, Kway::word cmdid, std::string msg)
{
   char buf[16];
   CmdReq_.ProcName_.assign(procname);
   CmdReq_.ObjName_.assign(objname);
   sprintf(buf,"%06d",cmdid);
   CmdReq_.CmdID_.assign(buf);
   CmdReq_.Msg_.assign(msg);
   if(CmdSess_.size() > 0){
      for(TCmdSess::iterator L = CmdSess_.begin(); L != CmdSess_.end(); L++){
         if((*L) && (*L)->GetSesProcName().find(procname) != std::string::npos)
            (*L)->SendCmdReq(CmdReq_);
      }
   }else if(EvSess_.size() > 0){
      for(TEvSess::iterator L = EvSess_.begin(); L != EvSess_.end(); L++)
         if((*L) && (*L)->IsReady())
            (*L)->SendCmdReq(CmdReq_);
   }
}
//---------------------------------------------------------------------------
K_mf(void) TEvSesMgr::WriteLog(const char* msg)
{
   char buf[512];
   if(Log_.IsOpened()){
      Log_.Seek(0,fsEnd);
      sprintf(buf,"%s %s\n",TKDateTimeMS::Now().as_string().c_str(), msg);
      Log_.Write(buf);
   }
}
//---------------------------------------------------------------------------
K_mf(void) TEvSesMgr::CollectEv(TEvAlert& EvAlert, bool recover)
{
   EvCenter_->AddEv(EvAlert, recover);
}
//---------------------------------------------------------------------------
K_mf(void) TEvSesMgr::RegEvCliSes(TEvCliSes* obj)
{
   if(EvSess_.size() > 0)
      for(TEvSess::iterator L = EvSess_.begin(); L != EvSess_.end(); L++){
         if((*L) == obj)
            return;
      }
   EvSess_.push_back(obj);
}
//---------------------------------------------------------------------------
K_mf(void) TEvSesMgr::DelEvCliSes(TEvCliSes* obj)
{
   if(EvSess_.size() > 0)
      for(TEvSess::iterator L = EvSess_.begin(); L != EvSess_.end(); L++){
         if((*L) == obj){
            EvSess_.erase(L);
            return;
         }
      }
}
//---------------------------------------------------------------------------
K_mf(void) TEvSesMgr::RegEvSvrSes(TEvSvrSes* obj)
{
   if(CmdSess_.size() > 0)
      for(TCmdSess::iterator L = CmdSess_.begin(); L != CmdSess_.end(); L++){
         if((*L) == obj)
            return;
      }
   CmdSess_.push_back(obj);
}
//---------------------------------------------------------------------------
K_mf(void) TEvSesMgr::DelEvSvrSes(TEvSvrSes* obj)
{
   if(CmdSess_.size() > 0)
      for(TCmdSess::iterator L = CmdSess_.begin(); L != CmdSess_.end(); L++){
         if((*L) == obj){
            CmdSess_.erase(L);
            return;
         }
      }
}
//---------------------------------------------------------------------------
K_mf(void) TEvSesMgr::DoRecover(TEvCliSes* obj)
{
   if(obj->IsReady()){
      for(TCurrStat::iterator iter = CurrStat_.begin(); iter != CurrStat_.end(); iter++)
         obj->SendAlert(iter->second, true);
      while(Evs_.size()){
         obj->SendAlert((*Evs_.begin()));
         Evs_.erase(Evs_.begin());
      }
   }
}
//---------------------------------------------------------------------------
K_mf(bool) TEvSesMgr::SetMsgCnt(int cnt, int lineno)
{
   //務必記得在initial 時必須先做GetMsgCnt 才會create recorder;
   TMsgCnt::iterator iter = MsgCnt_.fetch(lineno);
   TMsgCnt::updater  u    = MsgCnt_.begin_update(iter);
   (*u) = cnt;
   return true;
}
//---------------------------------------------------------------------------
K_mf(int) TEvSesMgr::GetMsgCnt(int lineno)
{
   if(MsgCntRec_ == 0)
      MsgCntRec_ = new TMsgCntRec(MsgCnt_, std::string(GetProcLogPath()+"MsgCnt.rec").c_str());
   TMsgCnt::iterator iter = MsgCnt_.find(lineno);
   if(iter == MsgCnt_.end()){
      iter = MsgCnt_.fetch(lineno);
      TMsgCnt::updater u = MsgCnt_.begin_update(iter);
      (*u) = 0;
      return 0;
   }
   return iter->second;
}
//---------------------------------------------------------------------------
K_mf(void) TEvSesMgr::LinkStateChg(TProcName gname, TProcName cname, bool ready)
{
   EvCenter_->DoLinkStateChg(gname,cname, ready);
}
/////////////////////////////////////////////////////////////////////////////
K_mf(const Kway::TFieldDesp&) TEvDataFieldMaker::GetFieldDesp(size_t n) const
{
   static Kway::TFieldDesp FdDsp[]=
   {
     { "序號"        , 6,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Fixed , },
     { "發生時間"    ,23,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "Program"     ,10,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "Process"     ,10,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
//     { "市場別"      , 6,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "Object"      , 6,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "群組別"      , 5,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "事件代碼"    , 5,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "事件等級"    , 5,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "事件說明"    ,30,  Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  , Kway::tfs_Null  , },
     { "備註"        ,20,  Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  , Kway::tfs_Null  , },
     { "採取措施"    ,30,  Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  , Kway::tfs_Null  , },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TEvDataFieldMaker::Get(const TEvDatai& i, size_t fieldNo) const
{
   switch(fieldNo)
   {
      case  0: {char buf[32];
                sprintf(buf,"%d",i->first);
                return std::string(buf);
               }
      case  1:  return i->second.Time_.GetTime().as_string()+"."+i->second.Time_.GetMS().as_string();
      case  2:  return i->second.ProgName_.as_string();
      case  3:  return i->second.ProcName_.as_string();
//      case  4:  return i->second.Market_.as_string();
      case  4:  return i->second.Object_.as_string();
      case  5:  return i->second.GrpID_.as_string();
      case  6:  return i->second.EvCode_.as_string();
      case  7:  return i->second.EvLevel_.as_string();
      case  8:  return i->second.EvMsg_.as_string();
      case  9:  return i->second.ExtMsg_.as_string();
      case 10:  return i->second.Action_.as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&) TEvDataisFieldMaker::GetFieldDesp(size_t n) const
{
   static Kway::TFieldDesp FdDsp[]=
   {
     { "事件歷程"    , 8,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Fixed , },
     { "發生時間"    ,23,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "事件代碼"    , 5,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "事件等級"    , 5,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "事件說明"    ,30,  Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  , Kway::tfs_Null  , },
     { "備註"        ,20,  Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  , Kway::tfs_Null  , },
     { "採取措施"    ,30,  Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  , Kway::tfs_Null  , },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TEvDataisFieldMaker::Get(const TEvDataisi& i, size_t fieldNo) const
{
   switch(fieldNo)
   {
      case  0: {char buf[32];
                sprintf(buf,"%d",i->first);
                return std::string(buf);
               }
      case  1:  return i->second->second.Time_.GetTime().as_string()+"."+i->second->second.Time_.GetMS().as_string();
      case  2:  return i->second->second.EvCode_.as_string();
      case  3:  return i->second->second.EvLevel_.as_string();
      case  4:  return i->second->second.EvMsg_.as_string();
      case  5:  return i->second->second.ExtMsg_.as_string();
      case  6:  return i->second->second.Action_.as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&) TObjEvFieldMaker::GetFieldDesp(size_t n) const
{
   static Kway::TFieldDesp FdDsp[]=
   {
     { "事件來源"    ,20,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null , },
     { "更新時間"    ,23,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "狀態"        , 5,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "事件代碼"    , 5,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "事件說明"    ,30,  Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  , Kway::tfs_Null  , },
     { "備註"        ,20,  Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  , Kway::tfs_Null  , },
     { "採取措施"    ,30,  Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  , Kway::tfs_Null  , },
/*
     { "Program"     ,10,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "Process"     ,10,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "市場別"      , 6,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "群組別"      , 5,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
*/
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TObjEvFieldMaker::Get(const TObjEvMapi& i, size_t fieldNo) const
{
   switch(fieldNo)
   {
      case  0:  return i->first.as_string();
      case  1:  return i->second.Time_.GetTime().as_string()+"."+i->second.Time_.GetMS().as_string();
      case  2:  return i->second.EvLevel_.as_string();
      case  3:  return i->second.EvCode_.as_string();
      case  4:  return i->second.EvMsg_.as_string();
      case  5:  return i->second.ExtMsg_.as_string();
      case  6:  return i->second.Action_.as_string();
/*
      case  2:  return i->second.ProgName_.as_string();
      case  3:  return i->second.ProcName_.as_string();
      case  4:  return i->second.Market_.as_string();
      case  5:  return i->second.GrpID_.as_string();
*/
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&) TGrpEvFieldMaker::GetFieldDesp(size_t n) const
{
   static Kway::TFieldDesp FdDsp[]=
   {
     { "群組代號"      , 6,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Fixed , },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TGrpEvFieldMaker::Get(const TGrpEvMapi& i, size_t fieldNo) const
{
   switch(fieldNo)
   {
      case  0:  return i->first.as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&) TProcEvFieldMaker::GetFieldDesp(size_t n) const
{
   static Kway::TFieldDesp FdDsp[]=
   {
     { "連線代號"  , 6,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null , },
     { "程序狀態"  , 6,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null , },
     { "更新時間"  , 6,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null , },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TProcEvFieldMaker::Get(const TProcEvMapi& i, size_t fieldNo) const
{
   switch(fieldNo)
   {
      case  0:  return i->first.as_string();
      case  1:  return i->second.Ready_?"正常":"未連線";
      case  2:  return i->second.Time_.as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&) TBrkEvFieldMaker::GetFieldDesp(size_t n) const
{
   static Kway::TFieldDesp FdDsp[]=
   {
     { "券商代號"      , 6,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Fixed , },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TBrkEvFieldMaker::Get(const TBrkEvMapi& i, size_t fieldNo) const
{
   switch(fieldNo)
   {
      case  0:  return i->first.as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(const Kway::TFieldDesp&) TProgEvFieldMaker::GetFieldDesp(size_t n) const
{
   static Kway::TFieldDesp FdDsp[]=
   {
     { "子系統"      , 6,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Fixed , },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TProgEvFieldMaker::Get(const TProgEvMapi& i, size_t fieldNo) const
{
   switch(fieldNo)
   {
      case  0:  return i->first.as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
K_mf(size_t) TObjEvVACr::GetDetails(const char**& a)
{
   static const char* desp[] = {"群組代號"};
   a = desp;
   return numofele(desp);
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*)TObjEvVACr::GetDetailVACreater(size_t detailID)
{
   switch(detailID){
     case 0:return DetailCreater_;
     }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*) TObjEvVACr::OnMasterViewRowMoved
   (Kway::TViewArea& masterVA,Kway::TViewArea*& detailVA, size_t detailID)
{
   TObjEvViewer& va=static_cast<TObjEvViewer&>(masterVA);
   const TObjEvViewer::cursor_type *cur=va.GetCurRow();
   TEvDataisViewer *DetailView =static_cast<TEvDataisViewer*>(detailVA);
   if(cur == 0 || cur->IsInvalid()){
      DetailView->SetContainer(0);
   }else{
      DetailView->SetContainer((cur->GetIterator()->second.EvDatais_));
   }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(size_t) TGrpEvVACr::GetDetails(const char**& a)
{
   static const char* desp[] = {"群組代號"};
   a = desp;
   return numofele(desp);
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*)TGrpEvVACr::GetDetailVACreater(size_t detailID)
{
   switch(detailID){
     case 0:return DetailCreater_;
     }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*) TGrpEvVACr::OnMasterViewRowMoved
   (Kway::TViewArea& masterVA,Kway::TViewArea*& detailVA, size_t detailID)
{
   TGrpEvViewer& va=static_cast<TGrpEvViewer&>(masterVA);
   const TGrpEvViewer::cursor_type *cur=va.GetCurRow();
   TObjEvViewer *DetailView =static_cast<TObjEvViewer*>(detailVA);
   if(cur == 0 || cur->IsInvalid())
      DetailView->SetContainer(0);
   else
      DetailView->SetContainer(&(cur->GetIterator()->second));
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(size_t) TProcEvVACr::GetDetails(const char**& a)
{
   static const char* desp[] = {"連線代號"};
   a = desp;
   return numofele(desp);
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*)TProcEvVACr::GetDetailVACreater(size_t detailID)
{
   switch(detailID){
     case 0:return DetailCreater_;
     }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*) TProcEvVACr::OnMasterViewRowMoved
   (Kway::TViewArea& masterVA,Kway::TViewArea*& detailVA, size_t detailID)
{
   TProcEvViewer& va=static_cast<TProcEvViewer&>(masterVA);
   const TProcEvViewer::cursor_type *cur=va.GetCurRow();
   TGrpEvViewer *DetailView =static_cast<TGrpEvViewer*>(detailVA);
   if(cur == 0 || cur->IsInvalid()){
      DetailView->SetContainer(0);
   }else{
      DetailView->SetContainer(&(cur->GetIterator()->second));
   }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(size_t) TBrkEvVACr::GetDetails(const char**& a)
{
   static const char* desp[] = {"券商代號"};
   a = desp;
   return numofele(desp);
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*)TBrkEvVACr::GetDetailVACreater(size_t detailID)
{
   switch(detailID){
     case 0:return DetailCreater_;
     }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(Kway::TaViewArea) TBrkEvVACr::CreateNewVA(){
   TViewControls   vcs;
   if(BrkEv_ == 0)
      return Kway::TaViewArea(new TBrkEvViewer(0));
   else
      return Kway::TaViewArea(new TBrkEvViewer(BrkEv_, TBrkEvFieldMaker(), vcs));
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*) TBrkEvVACr::OnMasterViewRowMoved
   (Kway::TViewArea& masterVA,Kway::TViewArea*& detailVA, size_t detailID)
{
   TBrkEvViewer& va=static_cast<TBrkEvViewer&>(masterVA);
   const TBrkEvViewer::cursor_type *cur=va.GetCurRow();
   TProcEvViewer *DetailView =static_cast<TProcEvViewer*>(detailVA);
   if(cur == 0 || cur->IsInvalid())
      DetailView->SetContainer(0);
   else
      DetailView->SetContainer(&(cur->GetIterator()->second));
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(size_t) TProgEvVACr::GetDetails(const char**& a)
{
   static const char* desp[] = {"子系統"};
   a = desp;
   return numofele(desp);
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*)TProgEvVACr::GetDetailVACreater(size_t detailID)
{
   switch(detailID){
     case 0:return DetailCreater_;
     }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_mf(Kway::TVACreater*) TProgEvVACr::OnMasterViewRowMoved
   (Kway::TViewArea& masterVA,Kway::TViewArea*& detailVA, size_t detailID)
{
   TProgEvViewer& va=static_cast<TProgEvViewer&>(masterVA);
   const TProgEvViewer::cursor_type *cur=va.GetCurRow();
   TBrkEvViewer *DetailView =static_cast<TBrkEvViewer*>(detailVA);
   if(cur == 0 || cur->IsInvalid()){
      DetailView->SetContainer(0);
      DetailCreater_->SetContainer(0);
   }else{
      DetailView->SetContainer(&(cur->GetIterator()->second));
      DetailCreater_->SetContainer(&(cur->GetIterator()->second));
   }
   return inherited::DetailVACreaterNoChange;
}
//---------------------------------------------------------------------------
K_ctor TEvCenter::TEvCenter()
{
   EvDatas_    = new TEvDatas();
   EvDataRec_  = new TEvDataRec(*EvDatas_, (char*)std::string(GetProcLogPath()+"EvDatas.rec").c_str());
   EvDataVACr_ = new TEvDataVACr(EvDatas_);
   OpenSesMgr_ = new TOpenSesMgr();
   EvIndex_    = new THProgEvMap();

   for(TEvDatai edi = EvDatas_->begin(); edi != EvDatas_->end(); edi++)
      AddEvIndex(edi);

   EvDataVA_.Add("0.總合狀態", EvVa_);
   EvDataVA_.Add("1.系統訊息記錄", *EvDataVACr_);
   EvDataVA_.Add("2.訊息平台連線管理",SesMgrVA_);
   EvDataVA_.Add("3.訊息平台傳送記錄",*(OpenSesMgr_->OpenMsgVACr_));
   SesMgrVA_.Add("1.連線狀態",OpenSesMgr_->GetStatusVACreater());
   SesMgrVA_.Add("2.連線設定",OpenSesMgr_->GetSettingVACreater());
   const TSysMgrObjs&  sys = GetSysMgrObjs();
   sys.MainVAMgr_->Add("4.系統訊息狀態",EvDataVA_);
}
//---------------------------------------------------------------------------
K_dtor TEvCenter::~TEvCenter()
{
   const TSysMgrObjs&  sys=GetSysMgrObjs();
   sys.MainVAMgr_->Del(EvDataVA_);
//   EvDataVA_.Del(*ProgEvVACr_);
   EvDataVA_.Del(SesMgrVA_);
   EvDataVA_.Del(*(OpenSesMgr_->OpenMsgVACr_));
   EvDataVA_.Del(*EvDataVACr_);
   EvDataVA_.Del(EvVa_);
   SesMgrVA_.Del(OpenSesMgr_->GetStatusVACreater());
   SesMgrVA_.Del(OpenSesMgr_->GetSettingVACreater());
   delete EvIndex_;
   delete OpenSesMgr_;
   delete EvDataVACr_;
   delete EvDataRec_;
   delete EvDatas_;
//   delete ProgEvVACr_;
//   delete ProgEvMap_;
   for(TXTwseList::iterator iter = XTwseList_.begin(); iter != XTwseList_.end(); iter++)
      EvVa_.Del(iter->second.BrkVACr_);
   for(TProgList::iterator iter = ProgList_.begin(); iter != ProgList_.end(); iter++)
      EvVa_.Del(iter->second.ProcVACr_);
}
//---------------------------------------------------------------------------
K_mf(void) TEvCenter::AddEv(TEvAlert& EvAlert, bool recover)
{
   TErrLevel erlevel;
   TObjID    grpid;
   bool      alert = false;
   TSendData data;
   std::string ermsg, action;
   TEvData  evdata;
   evdata.Market_.assign(EvAlert.Market_.as_string());
   evdata.ProgName_.assign(EvAlert.ProgName_.as_string());
   evdata.ProcName_.assign(EvAlert.ProcName_.as_string());
   evdata.Host_.assign(EvAlert.Host_.as_string());
   evdata.Time_.assign(EvAlert.DateTime_.as_string());
   evdata.EvCode_.assign(EvAlert.EvCode_.as_string().c_str());
   if(GetExcMgr() == 0 || (!GetExcMgr()->GetErrDef(evdata.EvCode_, erlevel, grpid, ermsg, action, alert))){
      erlevel = emlNotDef;
      ermsg = std::string("未定義的事件代碼!");
   }
   evdata.GrpID_   = grpid.empty()?TObjID("Not-Defined"):grpid;
   evdata.EvLevel_ = erlevel;
   if(EvAlert.BrkID_.empty())
      evdata.BrkID_.assign("----");
   else
      evdata.BrkID_   = EvAlert.BrkID_;
   evdata.EvMsg_.assign(ermsg);
   evdata.Object_.assign(EvAlert.Object_.as_string());
   evdata.ExtMsg_.assign(EvAlert.Remark_.as_string());
   evdata.Action_.assign(action);

   TProcEvMapi Procit1;
   TProcEvMap::updater Procu;
   if(EvAlert.ProgName_.as_string().substr(0,5) == std::string("XTwse")){
      TXTwseList::iterator iter = XTwseList_.find(EvAlert.ProgName_);
      if(iter == XTwseList_.end()){
         iter = XTwseList_.fetch(EvAlert.ProgName_);
         iter->second.BrkVACr_.SetContainer(&iter->second.BrkEvMap_);
         EvVa_.Add(EvAlert.ProgName_.as_string(), (iter->second.BrkVACr_));
      }
      TBrkEvMapi BrkEvi = iter->second.BrkEvMap_.fetch(EvAlert.BrkID_);
      Procit1 = BrkEvi->second.fetch(EvAlert.ProcName_);
      Procu   = BrkEvi->second.begin_update(Procit1);
      // 20101006 因應訊息平台新規範, 將ProcName 放在Msg 中
      data.Msg_.assign("[BROKER]="+EvAlert.BrkID_.as_string()+"[SESSION ID]="+EvAlert.ProcName_.as_string()+
                     "[OBJECT]="+(evdata.Object_.empty()?"":evdata.Object_.as_string())+
                     "[INFO]="+ermsg+evdata.ExtMsg_.as_string()+
                     (action.empty()?"":("[ACTION]="+action)));
   }else{
      TProgList::iterator iter = ProgList_.find(EvAlert.ProgName_);
      if(iter == ProgList_.end()){
         iter = ProgList_.fetch(EvAlert.ProgName_);
         iter->second.ProcVACr_.SetContainer(&iter->second.ProcEvMap_);
         EvVa_.Add(EvAlert.ProgName_.as_string(), (iter->second.ProcVACr_));
      }
      Procit1 = iter->second.ProcEvMap_.fetch(EvAlert.ProcName_);
      Procu   = iter->second.ProcEvMap_.begin_update(Procit1);
      // 20100804 電作四組要求群組作業相關訊息不加其他標籤, 須配合將事件代碼對應的群組別改為CronGroup
      if(grpid == TObjID("CronGroup")) // 20100812 加上ObjectName
         data.Msg_.assign((evdata.Object_.empty()?"":(evdata.Object_.as_string()+"-"))+ermsg);
      else
         data.Msg_.assign("[OBJECT]="+(evdata.Object_.empty()?"":evdata.Object_.as_string())+
                           "[INFO]="+ermsg+evdata.ExtMsg_.as_string()+(action.empty()?"":("[ACTION]="+action)));
   }
   Procu->Ready_ = true;
   Procu->Time_  = TKTime::Now();
   bool   newlink=false;
   TGrpEvMapi  Grpiter = Procit1->second.fetch(grpid);
//   TObjEvMapi  Objiter = Grpiter->second.fetch(EvAlert.Object_);
   TObjEvMapi  Objiter = Grpiter->second.find(EvAlert.Object_);
   if(Objiter == Grpiter->second.end()){
      newlink = true;
      Objiter = Grpiter->second.fetch(EvAlert.Object_);
   }
   TObjEvMap::updater Obju = Grpiter->second.begin_update(Objiter);
   *Obju = evdata;
   if(newlink){
      THBrkEvMapi brki = EvIndex_->fetch(EvAlert.ProgName_)->second.fetch(EvAlert.BrkID_);
      THObjEvMapi obji = brki->second.fetch(EvAlert.ProcName_)->second.fetch(EvAlert.Object_);
      Obju->EvDatais_ = &(obji->second);
   }
   if(recover)
      return;
   if(EvDatas_){
      Kway::dword sz = EvDatas_->size()+1;
      EvDatas_->insert(std::make_pair(sz,evdata));
      TEvDatai edi = EvDatas_->fetch(sz);
      AddEvIndex(edi);
   }
   if(EvAlert.EvCode_.as_string() == "XC9999"){  // 20101006 發動訊息平台離線通知
      OpenSesMgr_->Disconnect();
   }
   if(!alert)
      return;
   char buf[16];
   data.Date_.assign(evdata.Time_.GetDate().as_string(TKDate::sf_ELong,0));
   data.Time_.assign(evdata.Time_.GetTime().as_string(false));
   data.Dot_.assign(".");
   data.MS_.assign(evdata.Time_.GetMS().as_string(3));
   data.Market_.assign(EvAlert.Market_[0]=='T'?"0":(EvAlert.Market_[0]=='O'?"1":"2"));
   data.HostName_.assign(EvAlert.Host_.as_string());
   data.SubSys_.assign(EvAlert.ProgName_.as_string());
   data.ProgName_.assign(EvAlert.ProgName_.as_string()); // 20101006 因應訊息平台新規範, 將ProcName --> ProgName
   sprintf(buf,"%c   ",erlevel.as_string()[0]);
   data.MsgType_.assign(buf);
   data.EvCode_.assign(evdata.EvCode_.as_string());
   OpenSesMgr_->NewMsg(data);
}
//---------------------------------------------------------------------------
K_mf(void) TEvCenter::DoLinkStateChg(TProcName progname, TProcName procname, bool ready)
{
   TProcEvMapi Procit1;
   TProcEvMap::updater Procu;
   if(progname.as_string().substr(0,5) == std::string("XTwse")){
      TBrkID brkid;
      brkid.assign(procname.as_string().substr(1,4));
      TXTwseList::iterator iter = XTwseList_.find(progname);
      if(iter == XTwseList_.end())
         return;
      TBrkEvMapi brki = iter->second.BrkEvMap_.fetch(brkid);
      Procit1 = brki->second.fetch(procname);
      Procu   = brki->second.begin_update(Procit1);
   }else{
      TProgList::iterator iter = ProgList_.find(progname);
      if(iter == ProgList_.end())
         return;
      Procit1 = iter->second.ProcEvMap_.fetch(procname);
      Procu   = iter->second.ProcEvMap_.begin_update(Procit1);
   }
   Procu->Ready_ = ready;
   Procu->Time_  = TKTime::Now();
/*
   if(!ready){
      while(Procit1->second.size()){
         TGrpEvMapi L1 = Procit1->second.begin();
         while(L1->second.size()){
            TObjEvMapi L2 = L1->second.begin();
            L2->second.EvDatais_ = 0;
            L1->second.erase(L2);
         }
//         Procit1->second.erase(L1);
      }
//      Procu->clear();
   }
*/
}
//---------------------------------------------------------------------------
K_mf(void) TEvCenter::AddEvIndex(TEvDatai& edi)
{
   THBrkEvMapi brki = EvIndex_->fetch(edi->second.ProgName_)->second.fetch(edi->second.BrkID_);
   THObjEvMapi obji = brki->second.fetch(edi->second.ProcName_)->second.fetch(edi->second.Object_);
   obji->second.insert(std::make_pair(obji->second.size()+1, edi));
}
//---------------------------------------------------------------------------
