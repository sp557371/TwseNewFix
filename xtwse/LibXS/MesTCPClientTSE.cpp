#include "stdafx.h"
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
/////////////////////////////////////////////////////////////////////////////
#include "MesTCPClientTSE.h"
#include "TimerThread.h"
#include "KTime.h"
#include <stdio.h>
//---------------------------------------------------------------------------
using namespace Kway;
//---------------------------------------------------------------------------
static const char slm_heartbeat[] = { (char)0xfe, (char)0xfe, '1', '1', 0, 0, (char)0xef, (char)0xef };
static const char slm_tail[]      = { (char)0xef, (char)0xef };
static const int  slm_min_size    = sizeof(slm_heartbeat);
/////////////////////////////////////////////////////////////////////////////
class TMesTCPClientTSE::TImpl : public TTimerBase
{
   TMesTCPClientTSE*    Mes_;
   enum EState {
      eWaittingSLM010,
      eWaittingRecv,
      eDataRecved,
      eWaitReopen,
   }  State_;
   std::string    Buffer_;
   bool           IsDataSent_;

   K_mf(void)  SetWaitReopen (const char* message)
      {  Start(10*1000);// n secs 之後重新開啟.
         Mes_->OnStateChanged(mslError, message);
         State_ = eWaitReopen;
      }
   K_mf(void)  OnTimer  ()
      {
         switch(State_) {
         case eWaittingSLM010:
                     SetWaitReopen("無法建立連線-沒收到SLM-010");
                     return;
         case eWaittingRecv:
                     SetWaitReopen("SLM-Timeout-沒收到訊息");
                     return;
         case eDataRecved:
                     State_ = eWaittingRecv;
                     break;
         case eWaitReopen:
                     TMesTCPClientTSE* mes = Mes_;
                     mes->Close();//會產生 LinkBroken()或LinkFail()事件, 造成 Impl 被刪除, 所以先將 Mes_ 取出!
                     mes->Open();
                     return;
         }
         if(IsDataSent_)
            IsDataSent_ = false;
         else//n秒內沒有送過資料,送出 heartbeat
            Mes_->PhySend(slm_heartbeat, sizeof(slm_heartbeat));
      }

   K_mf(size_t)  CheckBuffer  ()
      {
         if(Buffer_.length() < slm_min_size)
            return 0;
         const char* slm = Buffer_.data();
         if(slm[0] != '\xfe'  ||  slm[1] != '\xfe') { //check header
      __unknowSLMmessage:
            SetWaitReopen("Unknow SLM Message");
            return 0;
         }
         switch(slm[2]) {//check control code
         case '0':   if(slm[3]=='0') {// "00": SLM-020(TMP Message)
                        size_t   tmpsz = static_cast<byte>(slm[5]) + (static_cast<word>(byte(slm[4])) << 8);
                        if(tmpsz==0) {
                           SetWaitReopen("empty TMP Message");
                           return 0;
                        }
                        if(Buffer_.length() >= slm_min_size + tmpsz) {
                           State_ = eDataRecved;
                           char  buf[1024];
                           sprintf(buf, "Recv-TMP:%d", tmpsz);
                           Mes_->OnStateChanged(mslNormal, buf);
                           return tmpsz;
                        }
                        return 0;
                     }
                     goto __unknowSLMmessage;
         case '1':   switch(slm[3]) {
                     case '0':   // "10": SLM-010(建立Socket完成)
                              if(State_ == eWaittingSLM010) {
                                 State_ =  eWaittingRecv;
                                 Start(30*1000);//等 n 秒,檢查是否收到交易所的 heartbeat
                                 Mes_->SetLinkReady();
                              }
                              break;
                     case '1':   // "11": SLM-030(Heartbeat:確定Socket連線)
                              Mes_->OnStateChanged(mslNormal, "LinkCfm");
                              State_ = eDataRecved;
                              break;
                     default:
                        goto __unknowSLMmessage;
                     }
                     Buffer_.erase(0, slm_min_size);
                     break;
         default:    //錯誤訊息
                     char  errmsg[128];
                     sprintf(errmsg, "SLM-Err: %c%c", slm[2], slm[3]);
                     SetWaitReopen(errmsg);
                     break;
         } //switch(slm[2]
         return 0;
      }
public:
   K_ctor  TImpl  (TMesTCPClientTSE* mes)
      : TTimerBase(ots_MainThread)
      , Mes_(mes)
      , State_(eWaittingSLM010)
      , IsDataSent_(false)
      { Start(25*1000);//waitting SLM-010
      }

   K_mf(size_t)  OnSocketDataIn  ()
      {
         char  buf[1024*10];
         int   rsz = Mes_->PhyRecv(buf,sizeof(buf));
         if(rsz <= 0)
            return 0;
         Buffer_.insert(Buffer_.length(), buf, rsz);
         return CheckBuffer();
      }

   inline K_mf(void)  SetDataSent  ()   { IsDataSent_ = true; }

   K_mf(int)  Recv  (void* buffer, size_t size)
      {
         if(size_t tmpsz = CheckBuffer()) {
            memcpy(buffer, Buffer_.data() + (slm_min_size - sizeof(slm_tail)), tmpsz);
            Buffer_.erase(0, tmpsz + slm_min_size);
            return (int)tmpsz;
         }
         return 0;
      }
};
/////////////////////////////////////////////////////////////////////////////
K_ctor  TMesTCPClientTSE::TMesTCPClientTSE  ()
   : Impl_(0)
{
}
//---------------------------------------------------------------------------
K_dtor TMesTCPClientTSE::~TMesTCPClientTSE  ()
{
   delete Impl_;
}
//---------------------------------------------------------------------------
K_mf(int)  TMesTCPClientTSE::Send  (const void* buffer, size_t size)
{
   if(Impl_ == 0)
      return -1;
   Impl_->SetDataSent();
   char  header[] = { (char)0xfe, (char)0xfe, '0', '0',  (char)((size>>8)&0xff), (char)(size&0xff)};
   PhySend(header, sizeof(header));
   int   rtn = PhySend(buffer, size);
   PhySend(slm_tail, sizeof(slm_tail));
   return rtn;
}
//---------------------------------------------------------------------------
K_mf(int)  TMesTCPClientTSE::Recv  (void* buffer, size_t size)
{
   return Impl_ ? Impl_->Recv(buffer, size) : -1;
}
//---------------------------------------------------------------------------
K_mf(void)  TMesTCPClientTSE::OnSocketDataIn  (size_t)
{
   if(Impl_)
      if(size_t sz = Impl_->OnSocketDataIn())
         inherited::OnRecvReady(sz);
}
//---------------------------------------------------------------------------
K_mf(void)  TMesTCPClientTSE::OnSocketLinkReady ()
{
   delete Impl_;
   Impl_ = new TImpl(this);
}
//---------------------------------------------------------------------------
K_mf(void)  TMesTCPClientTSE::OnSocketLinkError (const std::string& reason)
{
   delete Impl_;
   Impl_ = 0;
   inherited::SetLinkError(reason);
}
//---------------------------------------------------------------------------

