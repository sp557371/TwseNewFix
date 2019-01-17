//
// 採用 Socket 的 Messager 的基底類別: 如: IPX,UDP,Multicast
//
// 版本: $Id: MesBaseSocket.h,v 1.4 2005/04/28 05:51:23 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef MesBaseSocketH
#define MesBaseSocketH
//---------------------------------------------------------------------------
#include <cc++/socket.h>
#include "Mes.h"
//---------------------------------------------------------------------------
#ifndef K_WINDOWS
typedef int sk_size_t;
#endif
//---------------------------------------------------------------------------
namespace Kway {
using namespace ost;
/////////////////////////////////////////////////////////////////////////////
class TMesBaseService : public Thread
{
public:
            TMesBaseService  (int af, int type, int protocol,
                              struct sockaddr* remoteAddr, socklen_t remoteAddrLen,
                              TMesBase*);
   virtual ~TMesBaseService  ();

   inline bool  IsSocketReady  ()  { return so != INVALID_SOCKET; }
          void  StartService   ();
          int   recv           (void *buf, sk_size_t len);
          int   send           (const void *buf, sk_size_t len);
         void   Free           ();

   static std::string RecvFrom (const struct sockaddr_in*);

protected:
   virtual void  endSocket    ();
   virtual void  StopService  ();
           void  run          ();   //override Thread::run()
           bool  Bind         (const struct sockaddr* localName, int nameLen);

   SOCKET   so;//與 Common C++ 的命名相同, 保留以後修改的相同性
   /* 20090930 fonwin 改成直接通知有資料,不用 RecvBuffer_
   char     RecvBuffer_[1024 * 32];
   int      RecvPos_;
   int      FetchPos_;
   */

   struct sockaddr*  RemoteAddr_;
   socklen_t         RemoteAddrLen_;
   TMesPtr           Mes_;

private:
   virtual void  LinkError  (const std::string&);//do nothing
   virtual void  RecvReady  (int, const struct sockaddr_in*);//do nothing
};
////////////////////////////////////////////////////////////////////////////
class TMesBaseSocket : public TMesBase
{
   typedef TMesBase  inherited;
   TMesBaseService*  Service_;
   virtual K_mf(TMesBaseService*)  CreateService  () = 0;

protected:
   K_mf(bool)  OpenImpl  ();
   K_mf(bool)  CloseImpl ();

public:
   K_ctor  TMesBaseSocket  ();
   K_dtor ~TMesBaseSocket  ();

   K_mf(int)   Send  (const void* buffer, size_t size);
   K_mf(int)   Recv  (      void* buffer, size_t size);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

