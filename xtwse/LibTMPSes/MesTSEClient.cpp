//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#ifndef __MesTSEClientCPP__
#define __MesTSEClientCPP__
/////////////////////////////////////////////////////////////////////////////
#include "MesTSEClient.h"
#include "SyncEv.h"
#include "ExtDef.h"
#include <stdio.h>
//---------------------------------------------------------------------------
using namespace Kway;
/////////////////////////////////////////////////////////////////////////////
K_mf(bool) TTsePK::CheckPacket()
{
   if(GetHeader() == Head)          //找到0xFEFE
        return true;
   else
        return false;
}
/////////////////////////////////////////////////////////////////////////////
K_ctor      TMesTSEClient::TMesTSEClient()
            : RecvPos_(0),
              PkResult_(chk_Continue),
              SkipBytes_(0),
              Timer_(this),
              HBTimer_(this),
              BufPos_(0),
              Conneted_(false),
              LinkReady_(false),
              inherited()
{
   TseTimeOut_ = 50;
   RecvTimeOut_ = 30;
//   LinkCfm_   = false;
   Logs_.Open(std::string(GetProcLogPath()+"TseClient.log").c_str(), (TFileMode)(fmRandomAccess|fmOpenAlways|fmCreatePath));
}
//---------------------------------------------------------------------------
K_dtor      TMesTSEClient::~TMesTSEClient()
{
   Logs_.Close();
//   inherited::~TMesTCPClient();
}
//---------------------------------------------------------------------------
K_mf(bool)    TMesTSEClient::Open   ()
{
   LinkReady_ = false;
   RecvPos_ = 0;
   BufPos_ = 0;
   WriteLog("Socket Opened!");
//   StartTimer(TseTimeOut_);
   return inherited::Open();
}
//---------------------------------------------------------------------------
K_mf(bool)    TMesTSEClient::Close  ()
{
   Conneted_ = false;
   LinkReady_ = false;
   RecvPos_ = 0;
   BufPos_ = 0;
   WriteLog("Socket Closed!");
   return inherited::Close();
}
//---------------------------------------------------------------------------
K_mf(void)   TMesTSEClient::EraseTMPBuf(size_t len)
{
   if(len > BufPos_)
        len = BufPos_;
   memcpy(TMPBuf_,TMPBuf_ + len,BufPos_ - len);
   BufPos_ -= len;
}
//---------------------------------------------------------------------------
K_mf(int)  TMesTSEClient::Send  (const void* buffer, size_t size)
{
   StopTimer();
   StartTimer(TseTimeOut_);
   char buf[4096];      //930513 因應檔案傳輸需求由256更改為4096;
   const char* TMPMsg;
   sprintf(bufforlog,"Send:CtrlCode = 00, Size = %d", size);
   WriteLog(bufforlog);
   TTseHead     Header;
   Header.SetCtrlCode("00");
   TMPMsg = static_cast<const char*>(buffer);
   Header.SetPkSize(size);
   memcpy(buf,&Header,HeadSize);
   memcpy(buf + HeadSize,TMPMsg,size);
   memcpy(buf + HeadSize + size,&Trailer,sizeof(Trailer));
   return inherited::Send(buf,size + sizeof(Header) + sizeof(Trailer));
}
//---------------------------------------------------------------------------
K_mf(int)  TMesTSEClient::Recv  (void* buffer, size_t size)
{
   if(size > BufPos_)
        size = BufPos_;
   if(size > 0)
   {
        memcpy(buffer,TMPBuf_,size);
        EraseTMPBuf(size);
        return size;
   }
   else
        return -1;
}
//---------------------------------------------------------------------------
K_mf(void)  TMesTSEClient::StartTimer  (size_t ms)
{
   Timer_.Start(ms*1000, &this_type::OnTimer);
}
//---------------------------------------------------------------------------
K_mf(void)  TMesTSEClient::StopTimer  ()
{
   Timer_.Stop();
}
//---------------------------------------------------------------------------
K_mf(void)  TMesTSEClient::ResetHBTimer  ()
{
   HBTimer_.Stop();
   HBTimer_.Start(RecvTimeOut_*1000, &this_type::OnHBTimer);//沒有交易資料時30秒判斷一次
   WriteLog("Reset TSE heartbeat timer.");
}
//---------------------------------------------------------------------------
K_mf(void)  TMesTSEClient::OnHBTimer  (TTimerBase*)
{
   HBTimer_.Stop();
   StopTimer();
   WriteLog("TSE heartbeat timeout, close socket & restart!");
   Close();
   Open();
}
//---------------------------------------------------------------------------
K_mf(void)  TMesTSEClient::OnTimer  (TTimerBase*)
{
   THeartBeat HeartBeat;
   inherited::Send(&HeartBeat,sizeof(HeartBeat));
   WriteLog("Send heartbeat to TSE!");
}
//---------------------------------------------------------------------------
K_mf(TChkResult)  TMesTSEClient::OnPkCheck(const TTseHead& pk, size_t recvSize,size_t* PacketSize)
{
   TCtrlCode code = pk.GetCtrlCode();
   if(code.as_int() == 0)
      *PacketSize = HeadSize + pk.GetPkSize() + sizeof(TTseTrailer);
   else
      *PacketSize = HeadSize + sizeof(TTseTrailer);
   return chk_Continue;
}
//---------------------------------------------------------------------------
K_mf(void)  TMesTSEClient::OnPkRecv(TTseHead& pk)
{
   sprintf(bufforlog,"Recv:CtrlCode = %s, Size = %d", pk.GetCtrlCode().as_string().c_str(),pk.GetPkSize()); 
   WriteLog(bufforlog);
   TCtrlCode code = pk.GetCtrlCode();
   ResetHBTimer();
   if(code.as_int() == 0){
      size_t bodysize = pk.GetPkSize();
      memcpy(TMPBuf_ + BufPos_,reinterpret_cast<char*>(&pk) + HeadSize,bodysize);
      BufPos_ += bodysize;
      OnRecvReady(0);
   }else if(code.as_int() == 10){
      LinkReady_ = true;
      StartTimer(TseTimeOut_);
      WriteLog("TSE: connection granted! (SLM010)");
      SetLinkReady();
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
      Close();
      Open();
   }
}
//---------------------------------------------------------------------------
K_mf(void)  TMesTSEClient::OnSocketDataIn  (size_t sz)
{
//   StopTimer();
   char* RecvBuf = reinterpret_cast<char*>(RecvPk_);
   for(;;) {
      int   reqsz = sizeof(*RecvPk_) - RecvPos_;
      int   rdsz  = inherited::Recv(RecvBuf + RecvPos_, reqsz);
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
//      else
//         StartTimer(TseTimeOut_);
//      if(rdsz < reqsz)//已將Mes緩衝區的資料收完了
//         break;
   } // for(;;...
}
//---------------------------------------------------------------------------
K_mf(void)   TMesTSEClient::OnSocketLinkReady()
{
   Conneted_ = true;
//   if(LinkReady_)
//        SetLinkReady();
   OnStateChanged(mslNormal, "Connected, Waiting confirm!");
   WriteLog("Connected, Waiting confirm!");
   ResetHBTimer();
}
//---------------------------------------------------------------------------
K_mf(void)   TMesTSEClient::OnSocketLinkError (const std::string& reason)
{
   StopTimer();
   HBTimer_.Stop();
   SetLinkError(reason);
   WriteLog((char*)(reason.c_str()));
}
//---------------------------------------------------------------------------
K_mf(bool)   TMesTSEClient::SettingApply(const std::string& str)
{
   inherited::SettingApply(str);
   char* S;
   char* pos; 
   int R_;
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
   WriteLog("TSEClient socket created!");
   WriteLog((char*)str.c_str());
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
   return true;
}
//---------------------------------------------------------------------------
K_mf(void)   TMesTSEClient::WriteLog(char* buf)
{
   char tmp[128];
   Logs_.Seek(0, fsEnd);
   sprintf(tmp,"%s.%s %5d %s",TKTime::Now().as_string().c_str(),TKTimeMS::Now().as_string().c_str(),SPort_, buf);
   Logs_.WriteLN(tmp);
}
/////////////////////////////////////////////////////////////////////////////
#endif //__MesTCPClientCPP__
