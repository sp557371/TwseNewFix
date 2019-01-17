//---------------------------------------------------------------------------
//
// 通訊classes基礎: Session-Messager
//    class TMesX25; // for Stream library
//
// 版本: $Id: MesX25_Stream.h,v 1.2 2004/02/27 08:28:56 fonwin Exp $
//
//
//---------------------------------------------------------------------------
#ifndef MesX25_StreamH
#define MesX25_StreamH
//---------------------------------------------------------------------------
#include "Mes.h"
#include "MemPool.h"
#include <cc++/thread.h>
#include "FileLog.h" 
//---------------------------------------------------------------------------
namespace Kway {
//---------------------------------------------------------------------------
class TMesX25;
/////////////////////////////////////////////////////////////////////////////
class MesX25Service : public ost::Thread
{
   int         LineNo_;
   int         PvcNo_;
   int         FD_;
   int         ErrNo_;
   int         Diagcode_;
   bool        BreakService_;
   std::string ErrMsg_;
   TMemQueue   InQ_;
   TMemQueue   OutQ_;
   TMesX25*    Mes_;
   MesLog*     appLog;

   bool  ChkError   (bool needSleep);
   void  SetErrMsg  (int no = 0, const char* = 0);
   bool  OpenFD     ();
   void  CloseFD    ();
   bool  OpenPVC    ();
   void  run        ();

   inline bool  IsReady  ()   { return(FD_ >= 0  &&  !BreakService_  &&  Mes_); }
public:
    MesX25Service  (TMesX25*, int lineNo, int pvcNo);
   ~MesX25Service  ();

   bool  Start    ();
   void  Free     ();
   int   Send     (const void* buffer, size_t size);
   int   Recv     (      void* buffer, size_t size);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
