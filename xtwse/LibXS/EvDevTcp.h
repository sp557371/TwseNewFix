/** @file EvDevTcp.h

@brief TcpClient,TcpServer.

@author ­·¤H fonwin@ms21.hinet.net
*/
#ifndef EvDevTcpH
#define EvDevTcpH
/////////////////////////////////////////////////////////////////////////////
#include "EvDevSocket.h"
#include "Mes.h"
#include "TimerThread.h"
#include <set>
//---------------------------------------------------------------------------
namespace Kway {
LIBNS_class TMesTcpServer;
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMesEvDevTcpCli : public    TMesNoThread
                            , protected TSocketFD
{
   typedef TMesNoThread    inheritedMes;
   typedef TSocketFD       inheritedFD;

protected:
   K_mf(EEvResult) OnLinkReady ();
   K_mf(EEvResult) OnLinkError ();
   K_mf(EEvResult) OnRecvReady ();
   K_mf(EEvResult) OnSendEmpty ();

   K_mf(bool)  CloseImpl  ();

public:
   K_ctor  TMesEvDevTcpCli (TFD, TMesEvHandler&);
   K_ctor  TMesEvDevTcpCli ();
   K_dtor ~TMesEvDevTcpCli ();

   K_mf(int)         Recv   (void* buffer, size_t sz);
   K_mf(int)         Send   (const void* buffer, size_t sz);
   K_mf(std::string) GetUID ();
};
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMesTcpClient : public    TMesEvDevTcpCli
                          , protected TTimerBase
                          , protected TMesTcpConnecting
{
   typedef TMesEvDevTcpCli    inherited;

   K_mf(void) OnIPConnecting (TMesTcpCliSetting*, int eno);
   K_mf(bool) CheckConnectResult (const std::string&);

   K_mf(EEvResult) OnLinkReady ();
   K_mf(EEvResult) OnLinkError ();

   K_mf(bool)        SettingApply (const std::string&);
   K_mf(std::string) GetSetting   ();
   K_mf(bool)        OpenImpl     ();

   K_mf(void)  OnTimer  ();

   /*
   std::string    IP_;
   port_t         Port_;
   port_t         LocPort_;
   bool           LocPortReuse_;
   size_t         MergeSize_;
   */
   TMesTcpCliSetting Setting_;
public:
   K_ctor  TMesTcpClient ();
   K_dtor ~TMesTcpClient ();
};
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMesTcpServer : public    TMesNoThread
                          , protected TSocketFD
                          , protected TTimerBase
{
   typedef TSocketFD    inheritedFD;
   typedef TMesBase     inheritedMes;

   K_mf(EEvResult)  OnEvRead  (TFD, TEventsDev*);
   K_mf(EEvResult)  OnEvWrite (TFD, TEventsDev*);

   // override TMesBase::Recv(), TMesBase::Send()
   K_mf(int)  Recv  (void* buffer, size_t sz);
   K_mf(int)  Send  (const void* buffer, size_t sz);

   K_mf(bool)        SettingApply (const std::string&);
   K_mf(std::string) GetSetting   ();

   K_mf(bool) OpenImpl  ();
   K_mf(bool) CloseImpl ();

   K_mf(void) OnTimer   ();

   port_t   Port_;

   class TCli;
   typedef std::set<TCli*> TClis;
   TClis Clis_;
   K_mf(void) UpdateCliCountState ();
public:
  K_ctor  TMesTcpServer  ();
  K_dtor ~TMesTcpServer  ();

  K_mf(void) OnCliClosed (TCli*);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

