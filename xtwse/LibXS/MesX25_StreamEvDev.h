//---------------------------------------------------------------------------
//
// 通訊classes基礎: Session-Messager
//    class TMesX25EvDevService; // for Stream library + EvDev(devpoll)
//
// 版本: $Id$
//
//
//---------------------------------------------------------------------------
#ifndef MesX25_StreamEvDevH
#define MesX25_StreamEvDevH
//---------------------------------------------------------------------------
#include "Mes.h"
#include "MemPool.h"
#include "EvDev.h"
#include "TimerThread.h"
//---------------------------------------------------------------------------
#include <fcntl.h>
#include <errno.h>
#include <sys/stream.h>
#include <stropts.h>
#include <netx25/uint.h>
#include <netx25/x25_proto.h>
//---------------------------------------------------------------------------
namespace Kway {
//---------------------------------------------------------------------------
class TMesX25EvDev;
/////////////////////////////////////////////////////////////////////////////
class MesX25EvDevService : public  TAutoEvFD
                         , private TTimerBase
{
   int            State_;
   int            LineNo_;
   int            PvcNo_;
   int            ErrNo_;
   int            Diagcode_;
   std::string    ErrMsg_;
   TMemQueue      OutQ_;
   TMesX25EvDev*  Mes_;
   struct pvcattf attach;
   struct strbuf  ctlblk;
   char           cblk[1024*16];
   struct strbuf  datblk;
   char           dblk[1024*16];
   int            ChkCount_;

   K_mf(void) Reopen           ();
   K_mf(bool) OpenFD           ();
   K_mf(void) CheckState       ();
   K_mf(bool) AttachToPvc      ();
   K_mf(bool) CheckPvcAttached ();

   K_mf(void)   OnTimer    ();
   K_mf(bool)   ChkError   (const char* prompt);
   K_mf(void)   SetErrMsg  (const char* prompt, int no = 0, const char* = 0);
   K_mf(size_t) PutMsg     (const void*, size_t);
   K_mf(void)   CheckSend  ();

   K_mf(EEvResult)  OnEvRead  (TFD, TEventsDev*);
   K_mf(EEvResult)  OnEvWrite (TFD, TEventsDev*);

   inline K_mf(bool) IsReady ()   { return(IsReadyFD()  &&  Mes_); }
public:
   K_ctor  MesX25EvDevService  (TMesX25EvDev*, int lineNo, int pvcNo);
   K_dtor ~MesX25EvDevService  ();

   K_mf(bool) Start ();
   K_mf(void) Free  ();
   K_mf(int)  Send  (const void* buffer, size_t size);
   K_mf(int)  Recv  (      void* buffer, size_t size);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
