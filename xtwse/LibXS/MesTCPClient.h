/**
 �q�Tclasses��¦: Session-Messager
    class TMesTCPClient;

 ����: $Id: MesTCPClient.h,v 1.7 2006/04/03 14:38:16 fonwin Exp $

 921023:
   �W�[ virtual K_mf(void)  TMesTCPClient::OnSocketDataIn (size_t); ������I�s���h�� OnRecvReady()
 921030:
   1.�N class MesTCPClient ���� MesTCPClient.cpp, �ç�W�� class TMesTCPClient::TImpl;
   2.�W�[ virtual K_mf(void)  TMesTCPClient::OnSocketLinkReady (); ������I�s���h�� SetLinkReady()
          virtual K_mf(void)  TMesTCPClient::OnSocketLinkError (); ������I�s���h�� SetLinkError()
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
   // �b localPort ���e�[�W "N" ���[��] SO_REUSEADDR
   // size = ��Ƴ̦h�X��bytes�ǰe(default=4096)
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
