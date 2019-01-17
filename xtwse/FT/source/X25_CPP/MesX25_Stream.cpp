//
// 通訊classes基礎: Session-Messager
//    class MesX25Service; // for Stream library
//
//    SunLink 參考網址: http://docs.sun.com/db/doc/802-3313/6i77g2pi9?a=view
//
// 版本: $Id: MesX25_Stream.cpp,v 1.3 2004/04/02 06:43:30 fonwin Exp $
//
// Setting format:
//    LineNo:PvcNo
//
//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
/////////////////////////////////////////////////////////////////////////////
#include "MesX25_Stream.h"
#include "TimerThread.h"
#include "ExtFuncs.h"   //ErrLog()
//---------------------------------------------------------------------------
#include <fcntl.h>
#include <errno.h>
#include <sys/stream.h>
#include <stropts.h>
//#include <sys/stropts.h>
namespace NetX25 {
#include <netx25/uint.h>
#include <netx25/x25_proto.h>
} // NetX25
//---------------------------------------------------------------------------
#include "Mes_Aux.h"
//---------------------------------------------------------------------------
//using namespace Kway;
/////////////////////////////////////////////////////////////////////////////
MesX25Service::MesX25Service (Kway::TMesX25* mes, int lineNo, int pvcNo)
   : LineNo_(lineNo)
   , PvcNo_(pvcNo)
   , FD_(-1)
   , ErrNo_(0)
   , Diagcode_(0)
   , BreakService_(false)
   , ErrMsg_()
   , InQ_(4096)
   , OutQ_(0)
   , Mes_(mes)
{
   char buf[16];
   sprintf(buf, "%03d-%03d", LineNo_, PvcNo_);
   std::string filename = std::string("/X25Stream")+ std::string(buf);
   appLog = new MesLog(filename.c_str());
   appLog->log("Create MesX25Service"); 
   OpenFD();
}
//---------------------------------------------------------------------------
MesX25Service::~MesX25Service ()
{
   appLog->log("Destory MesX25Service"); 
   CloseFD();
   if(appLog)
     delete appLog;
}
//---------------------------------------------------------------------------
bool  MesX25Service::Start  ()
{
   appLog->log("MesX25Service-> Start() begin"); 
   if(IsReady()) {
      start();
      appLog->log("MesX25Service-> Start() return true"); 
      return true;
   }
   appLog->log("MesX25Service-> Start() return false"); 
   return false;
}
//---------------------------------------------------------------------------
void  MesX25Service::Free  ()
{
//   appLog->log("MesX25Service-> Free() begin"); 
   if(this==0) {
     MesLog log("/AppTrace.log");
     log.log("MesX25Service-> if(this==0) return"); 
      return;
   }
   
   appLog->log("MesX25Service-> Free"); 
   BreakService_ = true;
   Mes_ = NULL;
   CloseFD();
   DelayFreeObjChecker(this, TThreadFinishChecker());
   appLog->log("MesX25Service-> Free return"); 
}
//---------------------------------------------------------------------------
void  MesX25Service::SetErrMsg  (int no, const char* msg)
{
   ErrNo_  = (no  ? no  : errno);
   ErrMsg_ = (msg ? msg : strerror(ErrNo_));
   if(IsReady()) {
      TMesSync_MF_2<Kway::TMesX25, TMesStateLevel, const std::string&>  a2(&Mes_, mslError, ErrMsg_);
      //Mes_->OnStateChanged(mslError, ErrMsg_);
      a2(&Kway::TMesX25::OnStateChanged);
   }
}
//---------------------------------------------------------------------------
bool  MesX25Service::ChkError  (bool needSleep)
{
//   appLog->log("MesX25Service-> ChkError begin"); 
   if(errno==EAGAIN  ||  errno==0) {
      if(needSleep) 
         sleep(1);   // 1ms
 
//      appLog->log("MesX25Service-> ChkError return false"); 
      return false;
   }
   SetErrMsg();
//   appLog->log("MesX25Service-> ChkError return true"); 
   return true;
}
//---------------------------------------------------------------------------
bool  MesX25Service::OpenFD  ()
{
   appLog->log("MesX25Service-> OpenFD() begin"); 
   CloseFD();
   if(BreakService_) {
      appLog->log("MesX25Service-> BreakService_ return false"); 
      return false;
   }
   FD_ = open("/dev/x25", O_RDWR|O_NDELAY);
   if(FD_ >= 0) {
      appLog->log("MesX25Service-> OpenFD() return true"); 
      return true;
   }
   SetErrMsg();
   appLog->log("MesX25Service-> OpenFD() return false"); 
   return false;
}
//---------------------------------------------------------------------------
void  MesX25Service::CloseFD  ()
{
   appLog->log("MesX25Service-> CloseFD()"); 
   if(FD_ < 0)  
      return;
   
   ::close(FD_);
   FD_ = -1;
   appLog->log("MesX25Service-> CloseFD() return"); 
}
//---------------------------------------------------------------------------
int  MesX25Service::Send  (const void* buffer, size_t size)
{
   if(0){}//可考慮在此先 putmsg() 將資料送出, 若有 EAGAIN, 才放到 OutQ_
   OutQ_.PutMemBlock(buffer, size);
   return size;
}
//---------------------------------------------------------------------------
int  MesX25Service::Recv  (void* buffer, size_t size)
{
   const void* mem;
   size_t      wsz = InQ_.GetMemBlock(mem);
   if(wsz) {
      memcpy(buffer,mem,wsz);
      InQ_.SetMemUsed(wsz);
   }
   return wsz;
}
//---------------------------------------------------------------------------
bool  MesX25Service::OpenPVC  ()
{
   appLog->log("MesX25Service-> OpenPVC() begin"); 
   TMesSync_MF_2<Kway::TMesX25, TMesStateLevel, const std::string&>  a2(&Mes_, mslNormal, "PVC-Opening...");
   //Mes_->OnStateChanged(mslNormal, "PVC-Opening...");
   a2(&Kway::TMesX25::OnStateChanged);

   struct NetX25::pvcattf  attach;
   memset(&attach, 0, sizeof(attach));
   attach.xl_type    = XL_CTL;
   attach.xl_command = N_PVC_ATTACH;
   attach.lci        = PvcNo_;
   attach.link_id    = LineNo_;

   struct strbuf   ctlblk;
   ctlblk.len    = sizeof(attach);
   ctlblk.maxlen = sizeof(attach);
   ctlblk.buf    = (char*)&attach;
   // send attach message to stream
   appLog->log("MesX25Service-> OpenPVC() -> struct pvcattf & strbuf"); 
   while(IsReady()) {
      appLog->log("MesX25Service-> OpenPVC() -> IsReady()"); 
      if(putmsg(FD_, &ctlblk, 0, 0)==0) {
         appLog->log("MesX25Service-> OpenPVC() -> if(putmsg(FD_, &ctlblk, 0, 0)==0)"); 
         goto __GET_ATTACH_RESULT;
      }
      appLog->log("MesX25Service-> OpenPVC() -> putmsg(FD_, &ctlblk, 0, 0) > 0"); 
      if(ChkError(true)) {
         appLog->log("MesX25Service-> OpenPVC() -> if(ChkError(true)) return"); 
         return false;
      }
   }
   appLog->log("MesX25Service-> OpenPVC() -> nothing return"); 
   return false;

__GET_ATTACH_RESULT:
   // get respsonse message for the attach from the streams driver
   appLog->log("MesX25Service-> OpenPVC() -> __GET_ATTACH_RESULT:"); 
   while(IsReady()) {
      appLog->log("MesX25Service-> OpenPVC() -> while(IsReady())"); 
      int flags = 0;
      if(getmsg(FD_, &ctlblk, 0, &flags)==0) {
         appLog->log("MesX25Service-> OpenPVC() -> if(getmsg(FD_, &ctlblk, 0, &flags)==0)"); 
         if(attach.xl_type    == XL_CTL
         && attach.xl_command == N_PVC_ATTACH) {
            appLog->log("MesX25Service-> OpenPVC() -> if(XL_CTL && N_PVC_ATTACH)"); 
            switch(Diagcode_ = attach.result_code) {
            case PVC_SUCCESS:       return true;
            case PVC_NOSUCHSUBNET:  SetErrMsg(-8, "No such subnet"); break;
            case PVC_CFGERROR:      SetErrMsg(-9, "Cfg error");      break;
            case PVC_PARERROR:      SetErrMsg(-10,"Param error");    break;
            case PVC_BUSY:          SetErrMsg(-11,"PVC Busy");       break;
            default:                SetErrMsg(-12,"unknow error");   break;
            } // switch(attach.result_code...
            return false;
         } // if(N_PVC_ATTACH
      } else
      if(ChkError(true)) {
         appLog->log("MesX25Service-> OpenPVC() -> getmsg(FD_, &ctlblk, 0, &flags) > 0"); 
         return false;
      }
   }
   appLog->log("MesX25Service-> OpenPVC() end"); 
   return false;
}
//---------------------------------------------------------------------------
void  MesX25Service::run  ()
{
   // waitting PVC attach
   while(IsReady()) {
      if(OpenPVC())  
         goto __CHECK_RxTx;
      
__REOPEN_FD:
      if(!IsReady())
         break;
      TMesSync_MF_1<Kway::TMesX25, const std::string&>   a1(&Mes_,ErrMsg_);
      //Mes_->SetLinkError(ErrMsg_);
      a1(&Kway::TMesX25::SetLinkError);
      if(IsReady())
         if(OpenFD()) {//重新開啟fd
            sleep(5000);
            continue;
         }
      return;
   }
   return;

__CHECK_RxTx:
   // check Send/Recv
   InQ_.Clear();
   OutQ_.Clear();

   struct strbuf  ct;
   char           cblk[1024];
   bzero(cblk, sizeof(cblk));
   ct.buf    = cblk;
   ct.maxlen = sizeof(cblk);

   struct strbuf  dt;
   char           cCurrentData[256];
   dt.buf    = cCurrentData;
   dt.maxlen = sizeof(cCurrentData);

   TMesSync_MF_0<Kway::TMesX25>  a(&Mes_);
   //Mes_->OnStateChanged(mslNormal, "PVC-Ready!");
   //Mes_->SetLinkReady();
   a(&Kway::TMesX25::SetLinkReady);
   
   while(IsReady()) {
      int   flag = 0;
      if(getmsg(FD_, &ct, &dt, &flag) != 0) {
         if(ChkError(false)) 
            goto __REOPEN_FD;
         
         //----- check send -----
         const void* mem;
         size_t      wsz = OutQ_.GetMemBlock(mem);
         if(wsz) {
            struct strbuf  ctl;
            struct strbuf  dat;
            struct NetX25::xdataf  data;
            bzero((char*)&data, sizeof(data));
            data.xl_type    = XL_DAT;
            data.xl_command = N_Data;
            ctl.len = sizeof(data);
            ctl.buf = (char*)&data;
            dat.buf = (char*)mem;
            dat.len = wsz;
            data.More = data.setQbit = data.setDbit = false;
            if(putmsg(FD_, &ctl, &dat,0) == 0) 
               OutQ_.SetMemUsed(wsz);
         } // if(wsz...
         sleep(1);
      } else {
         NetX25::S_X25_HDR*  result = (NetX25::S_X25_HDR*)cblk;
         switch(result->xl_type) {
         case XL_CTL:
            switch(result->xl_command) {
            case N_Abort:
                     SetErrMsg(errno, "Connection Aborted!");
                     goto __REOPEN_FD;
            case N_DI:  // DISC Indicate
                     SetErrMsg(errno, "Disconnect!");
                     goto __REOPEN_FD;
            case N_PVC_DETACH://15
                     SetErrMsg(errno, "PVC-Detach!");
                     goto __REOPEN_FD;
            case N_RI:  // RESET Indicate
                     // Send a RESET Confirm packet
                     struct NetX25::xrscf   rset;
                     bzero((char*)&rset, sizeof(rset));
                     rset.xl_type    = XL_CTL;
                     rset.xl_command = N_RC;
                     ct.len = sizeof(rset);
                     ct.buf = (char*)&rset;
                     while(IsReady()) {
                        if(putmsg(FD_, &ct, 0, 0)==0) 
                           break;
                        if(ChkError(true)) 
                           goto __REOPEN_FD;
                     }
                     SetErrMsg(errno, "RESET-Disconnect!");
                     goto __REOPEN_FD;
            default:
                     ErrLog("X.25 unknow ctrl command:%d", result->xl_command);
                     break;
            } //switch(xl_command...
            break;
         case XL_DAT:
            if(result->xl_command == N_Data) {
               InQ_.PutMemBlock(dt.buf, dt.len);
               //if((struct xdataf*)result->More)
               //   ...
               TMesSync_MF_1<Kway::TMesX25, size_t>   a1(&Mes_,0);
               //Mes_->OnRecvReady(0);
               a1(&Kway::TMesX25::OnRecvReady);
            } else
               ErrLog("X.25 unknow dat command:%d", result->xl_command);
            break; 
         } //switch(xl_type...
      } //if(getmsg()...
   } //while(IsReady()...
} // MesX25Service::run()
/////////////////////////////////////////////////////////////////////////////

