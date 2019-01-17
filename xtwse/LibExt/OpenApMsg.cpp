#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "OpenApMsg.h"
#include "KIniFile.h"
#include "SysMgrObjs.h"
#include "ExtDef.h"
//---------------------------------------------------------------------------
using namespace Kway;
//using namespace Kway::Tw::Stk;
//---------------------------------------------------------------------------
K_ctor TOpenCliSes::TOpenCliSes(TSesMgr* sm)
   :HBTimer_(this),
    AckTimer_(this)
{
   SesMgr_   = (TOpenSesMgr*)sm;
   Ready_    = false;
}
//---------------------------------------------------------------------------
K_dtor TOpenCliSes::~TOpenCliSes()
{
   HBTimer_.Stop();
   AckTimer_.Stop();
   OnStateChanged("");
   SesMgr_->DelOpenCliSes(this);
   char buf[128];
   sprintf(buf,"[%s] Closed!", IP_.c_str());
   SesMgr_->WriteLog(buf);
}
//---------------------------------------------------------------------------
K_mf(int) TOpenCliSes::GetPacketSize(TOpenPkt* pkt,bool issend)
{
   if(issend){
      switch(pkt->Func_.as_int()){
         case efs_Init    : return Init_sz;
         case efs_SendData: return SendData_sz;
         case efs_Finish  :
         case efs_RstCnt  : return HdSize;
         case efs_HB      : return HB_sz;
      }
   }else{
      switch(pkt->Func_.as_int()){
         case efr_InitAck    : return InitAck_sz;
         case efr_SendDataAck: 
         case efr_RstCntAck  : 
         case efr_FinishAck  :
         case efr_HBAck      : return Ack_sz;
      }
   }
   char buf[128];
   sprintf(buf,"[%s] Unknown function code!", IP_.c_str());
   SesMgr_->WriteLog(buf);
   Close();
   Open();
   return 0;
}
//---------------------------------------------------------------------------
K_mf(void) TOpenCliSes::ResetHBTimer()
{
   HBTimer_.Stop();
   HBTimer_.Start(OpenHBTime*1000, &this_type::OnHBTimer);
}
//---------------------------------------------------------------------------
K_mf(void) TOpenCliSes::OnMesLinkBroken(TMesBase* m, const std::string& str)
{
   HBTimer_.Stop();
   AckTimer_.Stop();
   Ready_ = false;
   SesMgr_->DelOpenCliSes(this);
   OnStateChanged("");
   char buf[512];
   sprintf(buf,"[%s] Link broken!", IP_.c_str());
   SesMgr_->WriteLog(buf);
}
//---------------------------------------------------------------------------
K_mf(void) TOpenCliSes::SendP(TOpenPkt& pkt)
{
   char buf[512];
   ((char*)(&pkt))[GetPacketSize(&pkt,true)] = 0;
   sprintf(buf,"%5s ==> %s", SrcPort_,((char*)(&pkt))/*+2*/);
   SesMgr_->WriteLog(buf);
   Ready_ = false;
   Send(pkt);
   ResetHBTimer();
   AckTimer_.Start(OpenAckTime*1000, &this_type::OnAckTimer);
}
//---------------------------------------------------------------------------
K_mf(void) TOpenCliSes::OnMesLinkReady(TMesBase*)
{
   IP_ = GetMes()->GetUID();
   char* tmp = (char*)IP_.c_str();
   while(tmp){
      SrcPort_ = ++tmp;
      tmp = strchr(SrcPort_,',');
      if(tmp == 0)
         tmp = strchr(SrcPort_,':');
   }
   char buf[512];
   sprintf(buf,"[%s] Connected!", IP_.c_str());
   SesMgr_->WriteLog(buf);
   TOpenPkt Pkt(efs_Init);
   Pkt.Init_.Msg_Record_Type.assign("12"); // 此內容須視四組定義20100209 改13, 20100303 改 12
   sprintf(buf,"%03d",sizeof(TDataBody));
   Pkt.Init_.Msg_Record_Length.assign(buf);
   SendP(Pkt);
}
//---------------------------------------------------------------------------
K_mf(void) TOpenCliSes::OnPktRecv(TOpenPkt& pkt)
{
   char buf[512];
   ((char*)(&pkt))[GetPacketSize(&pkt,false)] = 0;
   sprintf(buf,"%5s <== %s", SrcPort_,((char*)(&pkt))/*+2*/);
   SesMgr_->WriteLog(buf);
   AckTimer_.Stop();
   int st = (pkt.Func_.as_int()==efr_InitAck?pkt.InitAck_.Status.as_int():pkt.Status.as_int());
   if(st > 0){
      std::string str;
      switch(st){
         case 1: str = std::string("封包長度錯誤"); break;
         case 2: str = std::string("Packet_Type 錯誤"); break;
         case 3: str = std::string("訊息長度欄位錯誤"); break;
         case 4: str = std::string("資料來源欄位定義錯誤"); break;
         case 5: str = std::string("訊息不同步"); break;
         case 9999: str = std::string("系統錯誤"); break;
         default: str = std::string("其他不明錯誤"); break;
      }
      sprintf(buf,"%04d-%s", st, str.c_str());
      SesMgr_->WriteLog(buf);
      AckTimer_.Start(1*1000, &this_type::OnAckTimer);
      OnStateChanged(buf);
      return;
   }
   Ready_ = true;
   switch(pkt.Func_.as_int()){
      case efr_InitAck    : 
         SesMgr_->RegOpenCliSes(this, pkt.InitAck_.Record_Number.as_int());
         break;
      case efr_SendDataAck: 
      case efr_RstCntAck  : 
         SesMgr_->SendNext();
         break;
      case efr_HBAck      :
         break;
      default:
         break;
   }
}
//---------------------------------------------------------------------------
K_mf(void) TOpenCliSes::OnHBTimer(TTimerBase*)
{
   TKDateTimeMS dtm = TKDateTimeMS::Now();
   TOpenPkt Pkt(efs_HB);
   Pkt.HB_.Date_.assign(dtm.GetDate().as_string(TKDate::sf_ELong,0));
   Pkt.HB_.Dash_.assign("-");
   Pkt.HB_.Time_.assign(dtm.GetTime().as_string(false));
   Pkt.HB_.Dot_.assign(".");
   Pkt.HB_.MS_.assign(dtm.GetMS().as_string(2));
   SendP(Pkt);
}
//---------------------------------------------------------------------------
K_mf(void) TOpenCliSes::OnAckTimer(TTimerBase*)
{
   Close();
   Open();
}
//---------------------------------------------------------------------------
K_mf(void) TOpenCliSes::SendAlert(TSendData& data)
{
   TOpenPkt Pkt(efs_SendData);
   memcpy(&Pkt.SendData_, &data, sizeof(TSendData));
   SendP(Pkt);
}
//---------------------------------------------------------------------------
K_mf(void) TOpenCliSes::Disconnect()
{
   TOpenPkt Pkt(efs_Finish);
   SendP(Pkt);
}
//---------------------------------------------------------------------------
K_mf(void) TOpenCliSes::SendReset()
{
   TOpenPkt Pkt(efs_RstCnt);
   SendP(Pkt);
}
/////////////////////////////////////////////////////////////////////////////
K_mf(const Kway::TFieldDesp&) TOpenMsgFieldMaker::GetFieldDesp(size_t n) const
{
   static Kway::TFieldDesp FdDsp[]=
   {
     { "序號"        , 6,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Fixed , },
     { "己傳送"      , 6,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "發生時間"    , 6,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "市場別"      , 6,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "主機名稱"    , 8,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
//     { "系統別"      , 8,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "子系統"      ,10,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "程序名稱"    ,10,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "訊息型別"    , 5,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "錯誤碼"      , 5,  Kway::ta_xCenter_yCenter,  Kway::ta_xCenter_yCenter, Kway::tfs_Null  , },
     { "訊息內容"    ,60,  Kway::ta_xCenter_yCenter,  Kway::ta_xLeft_yCenter  , Kway::tfs_Null  , },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------
K_mf(std::string) TOpenMsgFieldMaker::Get(const TDataReci& i, size_t fieldNo) const
{
   switch(fieldNo)
   {
      case  0:  return i->first.as_string();
      case  1:  return i->second.SendAlready_?"Y":"";
      case  2:  return i->second.Data_.Time_.as_string()+"."+i->second.Data_.MS_.as_string();
      case  3:  return i->second.Data_.Market_.as_string();
      case  4:  return i->second.Data_.HostName_.as_string();
      case  5:  return i->second.Data_.SubSys_.as_string();
      case  6:  return i->second.Data_.ProgName_.as_string();
      case  7:  return i->second.Data_.MsgType_.as_string();
      case  8:  return i->second.Data_.EvCode_.as_string();
      case  9:  return i->second.Data_.Msg_.as_string();
   }
   return std::string();
}
/////////////////////////////////////////////////////////////////////////////
static TSesBase* OpenCliSes_Creater(const TSesReg&, TSesMgr& SesMgr)
{
   TOpenCliSes *Ses=new Kway::TOpenCliSes(&SesMgr);
   return (TSesBase*)Ses;
}
//---------------------------------------------------------------------------
static const TSesReg CliSesRegs[]=
{
   {"OPEN平台連線",OpenCliSes_Creater},
};
/////////////////////////////////////////////////////////////////////////////
K_ctor TOpenSesMgr::TOpenSesMgr()
   :TSesMgrR(std::string(GetProcLogPath()+"ModifyLog/OpenSesMgr.log").c_str())
{
   Reg(Kway::TMesRegs::GetDefaultMesRegs());
   Log_.Open(std::string(GetProcLogPath()+"OpenSes.log").c_str(), TFileMode(fmAppend|fmCreatePath|fmOpenAlways));
   Reg(CliSesRegs, CliSesRegs+numofele(CliSesRegs));

   DataRecs_     = new TDataRecs();
   DataRecorder_ = new TDataRecorder(*DataRecs_,(char*)std::string(GetProcLogPath()+"ApMsg2Open.rec").c_str());
   TKIniFile IniFile(std::string(GetConfPath()+"/Main.ini").c_str());
   SystemName_ = IniFile.ReadString("OpenMsg", "SystemName","FIX_Gateway");
   HostName_   = IniFile.ReadString("OpenMsg", "HostName","KWAY_TMP");
   std::string svraddr = IniFile.ReadString("OpenMsg", "OpenSvrAddr","127.0.0.1:1999");
   TStrings vacports;
   int cnt = IniFile.ReadStrings("OpenMsg", "SourcePort",vacports);
   for(int L = 0; L < cnt; L++)
      if(!vacports[L].empty())
         AddSetting("OPEN平台連線","","EvDevTcpCli",svraddr+","+vacports[L]);
   OpenMsgVACr_ = new TOpenMsgVACr(DataRecs_);
}
//---------------------------------------------------------------------------
K_dtor TOpenSesMgr::~TOpenSesMgr()
{
   Log_.Close();
   delete OpenMsgVACr_;
   delete DataRecorder_;
   delete DataRecs_;
}
//---------------------------------------------------------------------------
K_mf(bool) TOpenSesMgr::SendOpenMsg(TSendData& sdata)
{
   if(OpenSess_.size() > 0){
      for(TOpenSess::iterator L = OpenSess_.begin(); L != OpenSess_.end(); L++)
         if((*L) && (*L)->IsReady()){
            (*L)->SendAlert(sdata);
            return true;
         }
   }
   return false;
}
//---------------------------------------------------------------------------
K_mf(void) TOpenSesMgr::WriteLog(const char* msg)
{
   char buf[1024];
   if(Log_.IsOpened()){
      Log_.Seek(0,fsEnd);
      sprintf(buf,"%s %s\n",TKDateTimeMS::Now().as_string().c_str(), msg);
      Log_.Write(buf);
   }
}
//---------------------------------------------------------------------------
K_mf(void) TOpenSesMgr::RegOpenCliSes(TOpenCliSes* obj, Kway::dword mcount)
{
   if(OpenSess_.size() > 0)
      for(TOpenSess::iterator L = OpenSess_.begin(); L != OpenSess_.end(); L++){
         if((*L) == obj)
            return;
      }
   OpenSess_.push_back(obj);
   int nextseq = 1;
   for(TDataReci iter = DataRecs_->begin(); iter != DataRecs_->end(); iter++){
      nextseq = iter->first;
      if(!iter->second.SendAlready_)
      	break;
   }
   if(nextseq == DataRecs_->size()) // 20100817 已全部傳送完成, 下一筆序號應加1
      nextseq += 1;
   char buf[256];
   if(DataRecs_->size() == 0 || mcount > nextseq){
      sprintf(buf,"Next seqno %d, logon ack seqno %d, reset seqno!\n",nextseq, mcount);
      WriteLog(buf);
      MsgCount_ = 1;
      obj->SendReset();
   }else{
      if(mcount < 1)
         MsgCount_ = 1;
      else
         MsgCount_ = mcount;
      sprintf(buf,"Next seqno %d, logon ack seqno %d, will send from %d!\n",nextseq, mcount,MsgCount_);
      WriteLog(buf);
      SendNext();
   }
}
//---------------------------------------------------------------------------
K_mf(void) TOpenSesMgr::DelOpenCliSes(TOpenCliSes* obj)
{
   if(OpenSess_.size() > 0)
      for(TOpenSess::iterator L = OpenSess_.begin(); L != OpenSess_.end(); L++){
         if((*L) == obj){
            OpenSess_.erase(L);
            return;
         }
      }
}
//---------------------------------------------------------------------------
K_mf(void) TOpenSesMgr::SendNext()
{
   if(MsgCount_ > DataRecs_->size())
      return;
   TDataRecs::iterator L = DataRecs_->find(MsgCount_);
   if(L == DataRecs_->end())
      return;
   if(SendOpenMsg(L->second.Data_)){
      MsgCount_++;
      TDataRecs::updater u = DataRecs_->begin_update(L);
      u->SendAlready_ = true;
   }
}
//---------------------------------------------------------------------------
K_mf(void) TOpenSesMgr::NewMsg(TSendData& sdata)
{
   TDataRec drec;
   char buf[16];
   sdata.SysName_.assign(SystemName_);
   if(sdata.HostName_.empty())
      sdata.HostName_.assign(HostName_);
   sprintf(buf,"%05d",sizeof(TOpenHead)+sizeof(TSendData));
   sdata.Packet_Total_Length.assign(buf);
   sdata.Batch_Record_Count.assign("0001");
   drec.Data_ = sdata;
   DataRecs_->insert(std::make_pair(DataRecs_->size()+1,drec));
   SendNext();
}
//---------------------------------------------------------------------------
K_mf(void) TOpenSesMgr::Disconnect()
{
   if(OpenSess_.size() > 0)
      for(TOpenSess::iterator L = OpenSess_.begin(); L != OpenSess_.end(); L++)
         (*L)->Disconnect();
}
//---------------------------------------------------------------------------
