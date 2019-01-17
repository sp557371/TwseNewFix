/**
 通訊classes基礎: Session-Messager
    class TMesTCPClient;

 版本: $Id: MesTCPClient.h,v 1.7 2006/04/03 14:38:16 fonwin Exp $

 921023:
   增加 virtual K_mf(void)  TMesTCPClient::OnSocketDataIn (size_t); 直接轉呼叫底層的 OnRecvReady()
 921030:
   1.將 class MesTCPClient 移至 MesTCPClient.cpp, 並更名為 class TMesTCPClient::TImpl;
   2.增加 virtual K_mf(void)  TMesTCPClient::OnSocketLinkReady (); 直接轉呼叫底層的 SetLinkReady()
          virtual K_mf(void)  TMesTCPClient::OnSocketLinkError (); 直接轉呼叫底層的 SetLinkError()
*/
#ifndef MesTCPClientH
#define MesTCPClientH
/////////////////////////////////////////////////////////////////////////////
#include "Mes.h"
#include "MesTcpCliSetting.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMesTCPClient : public TMesNoThread
{
   typedef TMesNoThread    inherited;
public:
   K_ctor  TMesTCPClient  ();
   K_dtor ~TMesTCPClient  ();

   // "serverIP:serverPort[,[N]localPort][,M<size>]"
   // 在 localPort 之前加上 "N" 表示[不] SO_REUSEADDR
   // size = 資料最多合併bytes傳送(default=4096)
   K_mf(bool)         SettingApply  (const std::string&);
   K_mf(std::string)  GetSetting    ();
   inline K_mf(const TMesTcpCliSetting&) GetTcpSetting () const {
      return Setting_;
   }

   K_mf(std::string)  GetUID ();
   K_mf(int)          Send   (const void* buffer, size_t size);
   K_mf(int)          Recv   (      void* buffer, size_t size);

protected:
  virtual K_mf(void)  OnSocketRecreate  (void* impl);
          K_mf(bool)  OpenImpl          ();
          K_mf(bool)  CloseImpl         ();

private:
  virtual K_mf(void)  OnSocketDataIn    (size_t);
  virtual K_mf(void)  OnSocketLinkReady ();
  virtual K_mf(void)  OnSocketLinkError (const std::string&);

  class TImpl;
  friend class TImpl;
  TImpl*             Impl_;
  TMesTcpCliSetting  Setting_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
