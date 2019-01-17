//
// 通訊classes基礎: Session-Messager
//    class MesSerialImpl;    // RS232
//
// 版本: $Id: MesSerialImpl.h,v 1.1 2004/10/13 15:33:00 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef __MesSerialImpl_H__
#define __MesSerialImpl_H__
//---------------------------------------------------------------------------
#include <cc++/serial.h>
#include "MesSerial.h"
#include "MemPool.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
class MesSerialImpl : public ost::Serial
                    , public ost::Thread
                    , public TMemQueue
{
public:
   K_ctor  MesSerialImpl (TMesSerial*);
          ~MesSerialImpl ();  //因為繼承 ots:: 的 class, 所以不能加上 K_dtor

   virtual K_mf(void)  Start  ();
   virtual K_mf(void)  Free   ();

   K_mf(int)  Send  (const void *buf, size_t len);
   K_mf(int)  Recv  (      void *buf, size_t len);

   K_mf(bool) IsRunning () const  { return State_==es_Running; }
protected:
   virtual K_mf(void)  MainLoop     ();
   virtual K_mf(bool)  UpdateState  (char* msgbuf);

   inline K_mf(TMesSerial*)  GetMes         ()  { return Mes_.get(); }
   inline K_mf(void)         OnStateChanged (TMesStateLevel msl, const char* msg)
                                                { return Mes_->OnStateChanged(msl,msg); }
   inline K_mf(void)         SetLinkError   (const char* msg)
                                                { return Mes_->SetLinkError(msg); }
   #ifdef WIN32
      int  aWrite  (const char*, const int);//override ost::Serial::aWrite()
   #endif
   K_mf(void)  SetErrMsg  (const char*);

private:
        void   run        (); // override ost::Thread::run()
   K_mf(bool)  SetErrMsg  (ost::Serial::Error, const char*, int);

   K_mf(bool)  isPendingInput   ();
   K_mf(bool)  isPendingOutput  ();

   typedef Kstl::wise_ref<TMesSerial>  TMesPtr;
   TMesPtr  Mes_;
   enum EState {
      es_Initing,
      es_Inited,
      es_Error,
      es_Running,
      es_Terminate,
   }        State_;
   #ifdef WIN32
      OVERLAPPED  WrOverlapped_;
      size_t      WrWaittingOverlapped_;
      COMSTAT	   CS_;
   #endif
};
//---------------------------------------------------------------------------
inline K_mf(bool)  MesSerialImpl::isPendingInput  ()
{
   #ifdef WIN32
      return CS_.cbInQue != 0;
   #else
      return ost::Serial::isPending(ost::Serial::pendingInput,0);
   #endif
}
//---------------------------------------------------------------------------
inline K_mf(int)  MesSerialImpl::Send  (const void* buffer, size_t size)
{
   PutMemBlock(buffer,size);
   return (int)size;
}
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

