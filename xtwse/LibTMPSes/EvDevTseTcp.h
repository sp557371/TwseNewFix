/** @file EvDevTcp.h

@brief TcpClient,TcpServer.

@author 風人 fonwin@ms21.hinet.net
*/
#ifndef EvDevTseTcpH
#define EvDevTseTcpH
/////////////////////////////////////////////////////////////////////////////
#include "EvDevSocket.h"
#include "Mes.h"
#include "TimerThread.h"
#include "MesTSEClient.h"
#include <set>
//---------------------------------------------------------------------------
namespace Kway {
K_class TMesTseTcpServer;
/////////////////////////////////////////////////////////////////////////////
K_class TMesEvDevTseTcpCli : public    TMesNoThread
                            , protected TSocketFD
{
   typedef TMesNoThread    inheritedMes;
   typedef TSocketFD       inheritedFD;
   typedef TMesEvDevTseTcpCli this_type;

private:
   TTsePK             RecvPk_[2];             //RecvPk_[1]作保護用!
   char               TMPBuf_[4096];          //儲存TMP訊息
   char               bufforlog[128];
   size_t             BufPos_;
   size_t             RecvPos_;               //已收到的資料大小
   size_t             PacketSize_;
   TChkResult         PkResult_;
   size_t             SkipBytes_;
   TTimer<this_type>  Timer_;
   TTimer<this_type>  HBTimer_;
   word               RecvTimeOut_; 
   word               TseTimeOut_;
   bool               Conneted_;
   bool               LinkReady_;
   bool               LinkCfm_;
//   TFile              Logs_;
   int                SPort_;
   K_mf(void)         EraseTMPBuf      (size_t len);
   K_mf(void)         StartTimer       (size_t ms);
   K_mf(void)         StopTimer        ();
   K_mf(void)         ResetHBTimer     ();
   K_mf(void)         OnTimer          (TTimerBase*);
   K_mf(TChkResult)   OnPkCheck  (const TTseHead& pk, size_t recvSize,size_t* PacketSize);
   K_mf(void)         OnPkRecv         (TTseHead& pk);

protected:
   bool               DebugLog_;
   K_mf(EEvResult)    OnLinkReady ();
   K_mf(EEvResult)    OnLinkError ();
   K_mf(EEvResult)    OnRecvReady ();
   K_mf(EEvResult)    OnSendEmpty ();

   K_mf(bool)         CloseImpl  ();
   virtual K_mf(void) WriteLog         (char*,bool prevtime = false){};
   K_mf(void)         OnHBTimer        (TTimerBase*);
   virtual K_mf(void) DoHBTimer  () {};

public:
   K_ctor  TMesEvDevTseTcpCli (TFD, TMesEvHandler&);
   K_ctor  TMesEvDevTseTcpCli ();
   K_dtor ~TMesEvDevTseTcpCli ();

   K_mf(int)         Recv   (void* buffer, size_t sz);
   K_mf(int)         Send   (const void* buffer, size_t size);
   K_mf(std::string) GetUID ();
};
/////////////////////////////////////////////////////////////////////////////
K_class TMesTseTcpClient : public    TMesEvDevTseTcpCli
                          , protected TTimerBase
                          , protected TMesTcpConnecting
{
   typedef TMesEvDevTseTcpCli    inherited;

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
   word               RecvTimeOut_; 
   word               TseTimeOut_;
   TFile              Logs_;
   int                SPort_;
   TKDateTimeMS       TimeStamp_;

   TMesTcpCliSetting Setting_;
   K_mf(void) OnIPConnecting (TMesTcpCliSetting*, int eno);
   K_mf(bool) CheckConnectResult (const std::string&);

public:
   K_ctor  TMesTseTcpClient (bool debug = false);
   K_dtor ~TMesTseTcpClient ();
   K_mf(void) WriteLog         (char*,bool prevtime = false);
   K_mf(void) DoHBTimer     ();
};
/////////////////////////////////////////////////////////////////////////////
K_class TMesTseTcpServer : public    TMesNoThread
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
  K_ctor  TMesTseTcpServer  ();
  K_dtor ~TMesTseTcpServer  ();

  K_mf(void) OnCliClosed (TCli*);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

