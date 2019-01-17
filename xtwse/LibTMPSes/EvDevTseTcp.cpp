//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
/////////////////////////////////////////////////////////////////////////////
#include "EvDevTseTcp.h"
#include <errno.h>
#include "ExtDef.h"
//---------------------------------------------------------------------------
using namespace Kway;
/////////////////////////////////////////////////////////////////////////////
K_ctor  TMesEvDevTseTcpCli::TMesEvDevTseTcpCli ()
   : RecvPos_(0),
     PkResult_(chk_Continue),
     SkipBytes_(0),
     Timer_(this),
     HBTimer_(this),
     BufPos_(0),
     Conneted_(false),
     LinkReady_(false)
{
   SetMemQueue(1024*4);
   TseTimeOut_ = 10;
   RecvTimeOut_ = 15;
}
//---------------------------------------------------------------------------
K_ctor  TMesEvDevTseTcpCli::TMesEvDevTseTcpCli (TFD fd, TMesEvHandler& cliEvH)
   : inheritedFD(fd),
     RecvPos_(0),
     PkResult_(chk_Continue),
     SkipBytes_(0),
     Timer_(this),
     HBTimer_(this),
     BufPos_(0),
     Conneted_(false),
     LinkReady_(false)
{
   SetEvHandler(&cliEvH);
   SetMemQueue(1024*4);
   TseTimeOut_ = 10;
   RecvTimeOut_ = 15;
}
//---------------------------------------------------------------------------
K_dtor  TMesEvDevTseTcpCli::~TMesEvDevTseTcpCli ()
{
}
//---------------------------------------------------------------------------
K_mf(EEvResult) TMesEvDevTseTcpCli::OnLinkReady ()
{
//   inheritedMes::SetLinkReady();
   Conneted_ = true;
   OnStateChanged(mslNormal, "Connected, Waiting confirm!");
   WriteLog("Connected, Waiting confirm!");
   ResetHBTimer();
   return er_Continue;
}
//---------------------------------------------------------------------------
K_mf(EEvResult) TMesEvDevTseTcpCli::OnLinkError ()
{
   StopTimer();
   HBTimer_.Stop();
   WriteLog((char*)(MakeErrStr("", errno).c_str()));
   SetLinkError(MakeErrStr("", errno));
   return er_Remove;
}
//---------------------------------------------------------------------------
K_mf(void)   TMesEvDevTseTcpCli::EraseTMPBuf(size_t len)
{
   if(len > BufPos_)
        len = BufPos_;
   memcpy(TMPBuf_,TMPBuf_ + len,BufPos_ - len);
   BufPos_ -= len;
}
//---------------------------------------------------------------------------
K_mf(EEvResult) TMesEvDevTseTcpCli::OnRecvReady ()
{
//   inheritedMes::OnRecvReady(0);
//   return er_Continue;
   char* RecvBuf = reinterpret_cast<char*>(RecvPk_);
   for(;;) {
      int   reqsz = sizeof(*RecvPk_) - RecvPos_;
      int   rdsz  = inheritedFD::Recv(RecvBuf + RecvPos_, reqsz);
      if(rdsz <= 0)//已無資料
         break;
      if((RecvPos_ += rdsz) < HeadSize)
         //Header尚未收完
         break;
__CHECK_NEXT_PK:
      //檢查 PacketKey 是否合理
      bool pkcheck = RecvPk_->CheckPacket();
      size_t   Skips = 0;
      while(!pkcheck) {//不合理的SubSysName 或 沒有此訊息的處理程序
        //尋找下一個合理的 PacketKey
__SKIP_BYTES:
         ++Skips;
         if(--RecvPos_ < HeadSize)
            break;
         pkcheck = reinterpret_cast<TTsePK*>(RecvBuf + Skips)->CheckPacket();
      } //while
      if(Skips) {
         SkipBytes_ += Skips;
         memcpy(RecvBuf, RecvBuf+Skips, RecvPos_);
         if(RecvPos_ < HeadSize)
            continue;
      }
      if(pkcheck) {
//         PacketSize_ = 0;
         PacketSize_ = RecvPk_->GetPkSize()+ HeadSize+ TailSize;
         if(PkResult_ == chk_HeadOK) {
__CHECK_RECV_FINISH:
            if(static_cast<size_t>(RecvPos_) >= PacketSize_) {
               //訊息已收完
               OnPkRecv(*RecvPk_);
               PkResult_ = chk_Continue;
               Skips = PacketSize_;
               if(Skips < RecvPos_) {
                  memcpy(RecvBuf, RecvBuf+Skips, RecvPos_ -= Skips);
                  if(RecvPos_ >= HeadSize)
                     goto __CHECK_NEXT_PK;
               } else
                  RecvPos_ = 0;
            } // if(RecvPos_ >= RecvPk->GetPkSize()...
         } else {
            switch(PkResult_ = OnPkCheck(*RecvPk_, static_cast<size_t>(RecvPos_),&PacketSize_)) {
            case chk_HeadOK:
            case chk_Continue:
                        goto __CHECK_RECV_FINISH;
            case chk_Error:
                        Skips = 0;
                        goto __SKIP_BYTES;
            } // switch(worker->OnPkCheck()
         } // if(PkResult_ == cpr_HeadOK..else...
      } // if(pkcheck...
   } // for(;;...
   return er_Continue;
}
//---------------------------------------------------------------------------
K_mf(void)  TMesEvDevTseTcpCli::StartTimer  (size_t ms)
{
   Timer_.Start(ms*1000, &this_type::OnTimer);
}
//---------------------------------------------------------------------------
K_mf(void)  TMesEvDevTseTcpCli::StopTimer  ()
{
   Timer_.Stop();
}
//---------------------------------------------------------------------------
K_mf(void)  TMesEvDevTseTcpCli::ResetHBTimer  ()
{
   HBTimer_.Stop();
   HBTimer_.Start(RecvTimeOut_*1000, &this_type::OnHBTimer);//沒有交易資料時30秒判斷一次
   if(DebugLog_)
      WriteLog("Reset TSE heartbeat timer.", true);
}
//---------------------------------------------------------------------------
K_mf(void)  TMesEvDevTseTcpCli::OnHBTimer  (TTimerBase*)
{
   HBTimer_.Stop();
   StopTimer();
   if(DebugLog_)
      WriteLog("TSE heartbeat timeout, close socket & restart!");
   inheritedFD::OnLinkError();
   LinkReady_ = false;
   Conneted_  = false;
   DoHBTimer();
//   inheritedFD::Close();
//   Open();
}
//---------------------------------------------------------------------------
K_mf(void)  TMesEvDevTseTcpCli::OnTimer  (TTimerBase*)
{
   THeartBeat HeartBeat;
   inheritedFD::Send(&HeartBeat,sizeof(HeartBeat));
   if(DebugLog_)
         WriteLog("Send heartbeat to TSE!");
}
//---------------------------------------------------------------------------
K_mf(TChkResult)  TMesEvDevTseTcpCli::OnPkCheck(const TTseHead& pk, size_t recvSize,size_t* PacketSize)
{
   TCtrlCode code = pk.GetCtrlCode();
   if(code.as_int() == 0)
      *PacketSize = HeadSize + pk.GetPkSize() + sizeof(TTseTrailer);
   else
      *PacketSize = HeadSize + sizeof(TTseTrailer);
   return chk_Continue;
}
//---------------------------------------------------------------------------
K_mf(void)  TMesEvDevTseTcpCli::OnPkRecv(TTseHead& pk)
{
   if(DebugLog_){
      sprintf(bufforlog,"Recv:CtrlCode = %s, Size = %d", pk.GetCtrlCode().as_string().c_str(),pk.GetPkSize()); 
      WriteLog(bufforlog);
   }
   TCtrlCode code = pk.GetCtrlCode();
   ResetHBTimer();
   if(code.as_int() == 0){
      size_t bodysize = pk.GetPkSize();
      memcpy(TMPBuf_ + BufPos_,reinterpret_cast<char*>(&pk) + HeadSize,bodysize);
      BufPos_ += bodysize;
      inheritedMes::OnRecvReady(0);
   }else if(code.as_int() == 10){
      LinkReady_ = true;
//      StartTimer(TseTimeOut_);
      WriteLog("TSE: connection granted! (SLM010)");
      inheritedMes::SetLinkReady();
      OnStateChanged(mslNormal, "LinkReady");
   }else if(code.as_int() == 11){         //HeartBeat
   }else{
        switch(code.as_int()){
                case 21:
                        SetLinkError("21 Frameing Error");
                        WriteLog("TSE: 21 Frameing Error.");
                        break;
                case 22:
                        SetLinkError("22 非授權Source Port");
                        WriteLog("TSE: 22 非授權Source Port.");
                        break;
                case 23:
                        SetLinkError("23 Heartbeat Timeout");
                        WriteLog("TSE: 23 Heartbeat Timeout.");
                        break;
                case 24:
                        SetLinkError("24 Suspended by TSE");
                        WriteLog("TSE: 24 Suspended by TSE.");
                        break;
                case 25:
                        SetLinkError("25 Invalid Frame");
                        WriteLog("TSE: 25 Invalid Frame.");
                        break;
                case 29:
                        SetLinkError("29 System Not Ready");
                        WriteLog("TSE: 29 System Not Ready.");
                        break;
        }
//        StopTimer();
//      inheritedFD::Close();
      inheritedFD::OnLinkError();
   }
}
//---------------------------------------------------------------------------
K_mf(EEvResult) TMesEvDevTseTcpCli::OnSendEmpty ()
{
   inheritedMes::OnSendEmpty();
   return er_Continue;
}
//---------------------------------------------------------------------------
K_mf(bool)  TMesEvDevTseTcpCli::CloseImpl  ()
{
   return inheritedFD::Close();
}
//---------------------------------------------------------------------------
K_mf(int)  TMesEvDevTseTcpCli::Recv  (void* buffer, size_t sz)
{
//   return inheritedFD::Recv(buffer,sz);
   if(sz > BufPos_)
        sz = BufPos_;
   if(sz > 0)
   {
        memcpy(buffer,TMPBuf_,sz);
        EraseTMPBuf(sz);
        return sz;
   }
   else
        return -1;
}
//---------------------------------------------------------------------------
K_mf(int)  TMesEvDevTseTcpCli::Send  (const void* buffer, size_t size)
{
   StopTimer();
   StartTimer(TseTimeOut_);
   char buf[4096];      //930513 因應檔案傳輸需求由256更改為4096;
   const char* TMPMsg;
   if(DebugLog_){
      sprintf(bufforlog,"Send:CtrlCode = 00, Size = %d", size);
      WriteLog(bufforlog);
   }
   TTseHead     Header;
   Header.SetCtrlCode("00");
   TMPMsg = static_cast<const char*>(buffer);
   Header.SetPkSize(size);
   memcpy(buf,&Header,HeadSize);
   memcpy(buf + HeadSize,TMPMsg,size);
   memcpy(buf + HeadSize + size,&Trailer,sizeof(Trailer));
   return inheritedFD::Send(buf,size + sizeof(Header) + sizeof(Trailer));
}
//---------------------------------------------------------------------------
K_mf(std::string) TMesEvDevTseTcpCli::GetUID ()
{
   std::string localUID = GetLocalUID();
   if(localUID.empty())
      return GetRemoteUID();
   return GetRemoteUID() + ",Local:" + localUID;
}
/////////////////////////////////////////////////////////////////////////////
K_ctor  TMesTseTcpClient::TMesTseTcpClient (bool debug)
   : TTimerBase(ots_MainThread)
{
   DebugLog_ = debug;
   Logs_.Open(std::string(GetProcLogPath()+"EvTseClient.log").c_str(), (TFileMode)(fmRandomAccess|fmOpenAlways|fmCreatePath));
}
//---------------------------------------------------------------------------
K_dtor  TMesTseTcpClient::~TMesTseTcpClient ()
{
    WriteLog((char*)GetSetting().c_str());
    WriteLog("TSEClient socket close!");
    Logs_.Close();
}
//---------------------------------------------------------------------------
K_mf(bool)  TMesTseTcpClient::SettingApply  (const std::string& str)
{
//   return Setting_.SettingApply(setting);
   Setting_.SettingApply(str, true);
   std::string confstr = str;
   char* S;
   char* pos; 
   int R_;
   SPort_ = 0;
   S = (char*)confstr.c_str();
   while(S){
      pos = ++S;
      S = strchr(pos,',');
      if(S == 0)
         S = strchr(pos,':');
   }
   SPort_ = atoi(pos);
   
/*
   S = strchr((char*)str.c_str(),',');
   if(S){
      pos = S++;
      if(strchr(pos,'B')){
         if((S=strchr(pos,':')))
            SPort_ = atoi(++S);
         else
            SPort_ = 0;
      }else
         SPort_ = atoi(pos);
   }
*/
   WriteLog("TSEClient socket created!");
   WriteLog((char*)str.c_str());
/*
   S = strchr((char*)str.c_str(),'-');
   if(!S)
      return true; 
   do{
      pos = strchr(S,','); 
      ++S; 
      char tmp;
      tmp = *S;
      if(pos){
         *pos = 0;
         R_ = atoi(++S);
         *pos = ',';
         S = pos; 
      } else
         R_ = atoi(++S); 
      if(R_<=0)
         continue; 
      if(tmp == 'R')
         RecvTimeOut_ = R_; 
      else if(tmp == 'S')
         TseTimeOut_ = R_;
   }while(S = strchr(S,'-'));
*/
   return true;

}
//---------------------------------------------------------------------------
K_mf(std::string)  TMesTseTcpClient::GetSetting  ()
{
   return Setting_.GetSetting();
}
//---------------------------------------------------------------------------
K_mf(EEvResult) TMesTseTcpClient::OnLinkReady ()
{
   Stop();//Stop timer.
   return inherited::OnLinkReady();
}
//---------------------------------------------------------------------------
K_mf(EEvResult) TMesTseTcpClient::OnLinkError ()
{
   Start(1000 * Setting_.RetrySeconds_);//Server斷線,或連線失敗,等 n 秒後重新連線.
   TSocketFD::Close();
   return inherited::OnLinkError();
}
//---------------------------------------------------------------------------
K_mf(bool)  TMesTseTcpClient::OpenImpl ()
{
   if(Setting_.GetSetting().empty())//等有正確設定後才開啟.
      return true;

   if(!IsReadyFD())
      if(!SetFD(socket(AF_INET, SOCK_STREAM, 0))) {
         Start(1000 * Setting_.RetrySeconds_);//等 n 秒,重新連線.
         SetLinkError("cannot create client socket.");
         return false;
      }
 //std::string result = Connect(GetGlobalEvDev(), IP_.c_str(), Port_, LocPort_, LocPortReuse_, MergeSize_);
/*
   std::string result = Connect(GetGlobalEvDev(), Setting_);
   if(result.empty()) {
      Start(1000 * 30);//等 n 秒,若未連線成功則重新連線.
      return true;
   }
   CloseFD();
   Start(1000 * Setting_.RetrySeconds_);//等 n 秒,重新連線.
   SetLinkError(result);
   return false;
*/
   return CheckConnectResult(Connect(Setting_, this));
}
//---------------------------------------------------------------------------
K_mf(void) TMesTseTcpClient::OnIPConnecting (TMesTcpCliSetting*, int eno)
{
   CheckConnectResult(ConnectingHandler(eno, GetGlobalEvDev()));
}
//---------------------------------------------------------------------------
K_mf(bool) TMesTseTcpClient::CheckConnectResult (const std::string& result)
{
   if(result.empty()) {
      Start(1000 * 30);//. n .,............
      return true;
   }
   CloseFD();
   Start(1000 * Setting_.RetrySeconds_);//. n .,.....
   SetLinkError(result);
   return false;
}
//---------------------------------------------------------------------------
K_mf(void) TMesTseTcpClient::WriteLog(char* buf, bool prevtime)
{
   char tmp[128];
   Logs_.Seek(0, fsEnd);
   if(!prevtime)
      TimeStamp_ = TKDateTimeMS::Now();
   sprintf(tmp,"%s %5d %s",TimeStamp_.as_string().c_str(),SPort_, buf);
   Logs_.WriteLN(tmp);
}
//---------------------------------------------------------------------------
K_mf(void)  TMesTseTcpClient::DoHBTimer  ()
{
//   TSocketFD::Close();
   CloseFD();
   Start(1000/* * Setting_.RetrySeconds_*/);//等 n 秒,重新連線. 20100720 改為1秒
   SetLinkError(std::string("HB Timeout!"));
//   Open();
}
//---------------------------------------------------------------------------
K_mf(void)  TMesTseTcpClient::OnTimer  ()
{
   if(GetLinkState()==mls_Opening) {
      TLocker  locker(this);
      CloseReason("connect timeout");
      Start(1000 * Setting_.RetrySeconds_);//等 n 秒,重新連線.
   } else {
      Stop();
      Open();
   }
}
/////////////////////////////////////////////////////////////////////////////
class TMesTseTcpServer::TCli : public TMesEvDevTseTcpCli
{
   typedef TMesEvDevTseTcpCli    inherited;
   TMesTseTcpServer* Server_;

   K_mf(bool)  OpenImpl  ()
      { return true; }
public:
   K_ctor  TCli  (TFD fd, TMesTseTcpServer& svr, TMesEvHandler& cliEvH)
      : inherited(fd, cliEvH)
      , Server_(&svr)
      {
      }
   K_dtor ~TCli  ()
      { Server_->OnCliClosed(this);
      }
   using inherited::Attach;
};
/////////////////////////////////////////////////////////////////////////////
K_ctor  TMesTseTcpServer::TMesTseTcpServer  ()
   : Port_(0)
   , TTimerBase(ots_MainThread)
{
}
//---------------------------------------------------------------------------
K_dtor  TMesTseTcpServer::~TMesTseTcpServer  ()
{
   TClis clis;
   clis.swap(Clis_);
   TClis::iterator   iend = clis.end();
   for(TClis::iterator  i = clis.begin();  i != iend;  ++i)
      delete *i;
}
//---------------------------------------------------------------------------
K_mf(void) TMesTseTcpServer::UpdateCliCountState ()
{
   char  buf[1024];
   OnStateChanged(mslNormal, std::string(buf, sprintf(buf,"SvrListen...(%d)", Clis_.size())));
}
//---------------------------------------------------------------------------
K_mf(void) TMesTseTcpServer::OnCliClosed (TCli* cli)
{
   if(Clis_.empty())
      return;
   TClis::iterator   i = Clis_.find(cli);
   if(i == Clis_.end())
      return;
   Clis_.erase(i);
   UpdateCliCountState();
}
//---------------------------------------------------------------------------
K_mf(EEvResult)  TMesTseTcpServer::OnEvRead  (TFD fd, TEventsDev* dev)
{
   size_t   count = 0;
   for(;;) {
      int fdCli = accept(fd, 0, 0);
      if(fdCli < 0) {
         if(count)
            UpdateCliCountState();
         return er_Continue;
      }

      TMesEvHandler* cliEvH = GetClientEvHandler();
      if(cliEvH == 0) {
         ::close(fdCli);
         continue;
      }
      TCli* cliMes = new TCli(fdCli, *this, *cliEvH);
      if(cliMes->Attach(*dev, ev_Write)) {
         if(OnClientConnect(cliMes)) {
            Clis_.insert(cliMes);
            ++count;
         } else {
            //cliMes has been deleted.
         }
      } else
         delete cliMes;
   }
}
//---------------------------------------------------------------------------
K_mf(EEvResult)  TMesTseTcpServer::OnEvWrite  (TFD, TEventsDev*)
{  //應該不會有此事件!!
   //如果有,表示有異常,則關閉此連線.
   inheritedMes::CloseReason(MakeErrStr("Listen fail",errno));
   return er_Remove;
}
//---------------------------------------------------------------------------
K_mf(bool) TMesTseTcpServer::SettingApply (const std::string& setting)
{
   Port_ = atoi(setting.c_str());
   return true;
}
//---------------------------------------------------------------------------
K_mf(std::string) TMesTseTcpServer::GetSetting ()
{
   char  buf[128];
   return std::string(buf, sprintf(buf, "%d", Port_));
}
//---------------------------------------------------------------------------
K_mf(bool) TMesTseTcpServer::OpenImpl ()
{
   std::string result = Listen(GetGlobalEvDev(), Port_, true);
   if(result.empty())
      return true;
   Start(1000 * 10); //等n秒,重新開啟listen.
   return false;
}
//---------------------------------------------------------------------------
K_mf(bool) TMesTseTcpServer::CloseImpl ()
{
   return inheritedFD::Close();
}
//---------------------------------------------------------------------------
K_mf(int)  TMesTseTcpServer::Recv  (void*, size_t)
{
   return 0;
}
//---------------------------------------------------------------------------
K_mf(int)  TMesTseTcpServer::Send  (const void*, size_t)
{
   return 0;
}
//---------------------------------------------------------------------------
K_mf(void)  TMesTseTcpServer::OnTimer  ()
{
   if(OpenImpl())
      Stop();
}
/////////////////////////////////////////////////////////////////////////////

