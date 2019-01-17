//---------------------------------------------------------------------------
//
// 通訊classes基礎: Session-Messager
//    class TMesX25; // for Alpha DEC
//
// 版本: $Id: MesX25_AlphaDEC.h,v 1.2 2005/04/28 06:31:38 fonwin Exp $
//
//
//---------------------------------------------------------------------------
#ifndef MesX25_AlphaDECH
#define MesX25_AlphaDECH
//---------------------------------------------------------------------------
#include "Mes.h"
#include "MemPool.h"
#include <cc++/thread.h>
//---------------------------------------------------------------------------
namespace Kway {
//---------------------------------------------------------------------------
class TMesX25;
/////////////////////////////////////////////////////////////////////////////
class MesX25Service : public ost::Thread
{
   int            LineNo_;
   int            PvcNo_;
   int            FD_;
   int            ErrNo_;
   int            Diagcode_;
   bool           BreakService_;
   std::string    ErrMsg_;
   TMemQueue      InQ_;
   TMemQueue      OutQ_;
   TMesX25*       Mes_;
   unsigned char  epl[4096];

   bool  ChkError   (bool needSleep);
   bool  SetErrMsg  (int no, const char*);
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
