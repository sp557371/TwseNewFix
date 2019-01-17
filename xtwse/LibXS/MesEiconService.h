//---------------------------------------------------------------------------
#ifndef MesEiconServiceH
#define MesEiconServiceH
//---------------------------------------------------------------------------
#include "Mes.h"
#include "MemPool.h"
#include "TimerThread.h"
#include "MesEicon.h"
#include <cc++/thread.h>
//---------------------------------------------------------------------------
#define EC_WIN32
#define EC_REF
#include "EiconX25\x25.h"
#include "EiconX25\neterr.h"
//---------------------------------------------------------------------------
using namespace Kway;
//---------------------------------------------------------------------------
class TMesEicon;
/////////////////////////////////////////////////////////////////////////////
class TMesEiconService : public ost::Thread
{
   enum {
      eIdle,
      eConnecting,
      eConnected,
      eLinkReady,
      eLinkFail
   } State_;

   char*       TxBuf_;
   char*       RxBuf_;
   size_t      TxLen_;

   int         PortNo_;
   int         PvcNo_;
   int         ConnID_;
   int         iInfo_;
   int         ErrNo_;
   int         Diagcode_;
   bool        BreakService_;
   std::string ErrMsg_;
   TMemQueue   InQ_;
   TMemQueue   OutQ_;
   TMesEicon*  Mes_;

   void __fastcall SetErrMsg   (int no = 0, const char* = 0);
   static void            DoX25Post   (struct x25doneinfo*);
   bool __fastcall ChkPostInfo (struct x25doneinfo*);
   void __fastcall StartRecv   ();
   bool __fastcall OpenPVC     ();
   void __fastcall CloseFD     ();
   void __fastcall FreeBuf     (char*);
   
   void run         ();

public:
    TMesEiconService  (TMesEicon*, int lineNo, int pvcNo);
   ~TMesEiconService  ();

   bool Start    ();
   void Free     ();
   int  Send     (const void* buffer, size_t size);
   int  Recv     (      void* buffer, size_t size);

   int  ConnID   ()  { return ConnID_; }

   void __fastcall PostDoneInfo(struct x25doneinfo*);
};
//---------------------------------------------------------------------------
#endif
