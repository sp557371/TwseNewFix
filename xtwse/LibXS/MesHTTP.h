//
// 通訊classes基礎: Session-Messager
//    class TMesHTTP;
//
// 版本: $Id$
//
//---------------------------------------------------------------------------
#ifndef MesHTTP_H
#define MesHTTP_H
//---------------------------------------------------------------------------
#include "MesTCPClient.h"
//---------------------------------------------------------------------------
namespace Kway {
struct TMesReg;// MesRegs.h
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMesHTTP : public TMesTCPClient
{
   typedef TMesTCPClient   inherited;
public:
   K_ctor  TMesHTTP  ();
   K_dtor ~TMesHTTP  ();

   K_mf(int)  Send  (const void* buffer, size_t size);
   K_mf(int)  Recv  (      void* buffer, size_t size);

   static K_mf(TMesReg&)  GetReg  ();

   // TCPClient-Setting;HttpSetting
   // HttpSetting = RecvHead(Y/N),LinkReadySendFile,SendHttpHeadFile
   //    RecvHead = Y:當呼叫Recv()時會連同 Http head 一起傳回.
   //    LinkReadySendFile = 當連線成功後,立即送出此檔內容.
   //    SendHttpHeadFile  = 當呼叫Send()時,要送出的 Http head 資料.
   //                        此檔<必須>包含 "Content-Length:" Send()的size會填入此處
   K_mf(bool) SettingApply (const std::string&);
   K_mf(std::string) GetSetting ();

protected:
   K_mf(bool) OpenImpl ();

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
