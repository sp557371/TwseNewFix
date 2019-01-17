//
// 通訊classes基礎: Session-Messager
//    class TMesTCPClientTSE;
//    與交易所連線用的 TCPClient
//
// 版本: $Id: MesTCPClientTSE.h,v 1.2 2005/12/28 08:22:18 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef MesTCPClientTSEH
#define MesTCPClientTSEH
//---------------------------------------------------------------------------
#include "MesTCPClient.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMesTCPClientTSE : public TMesTCPClient
{
   typedef TMesTCPClient   inherited;
public:
   K_ctor  TMesTCPClientTSE  ();
   K_dtor ~TMesTCPClientTSE  ();

   K_mf(int)  Send  (const void* buffer, size_t size);
   K_mf(int)  Recv  (      void* buffer, size_t size);

private:
   inline K_mf(int)  PhySend  (const void* buffer, size_t size)  { return inherited::Send(buffer,size); }
   inline K_mf(int)  PhyRecv  (      void* buffer, size_t size)  { return inherited::Recv(buffer,size); }
   K_mf(void)  OnSocketDataIn    (size_t);
   K_mf(void)  OnSocketLinkReady ();
   K_mf(void)  OnSocketLinkError (const std::string&);

   class  TImpl;
   TImpl* Impl_;
   friend class TImpl;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
