/** @file MesTcpCliSetting.h

@brief Mes: TCP Client �]�w

@author ���H fonwin@ms21.hinet.net
*/
#ifndef MesTcpCliSettingH
#define MesTcpCliSettingH
/////////////////////////////////////////////////////////////////////////////
#include "KwayBase.h"
#include <cc++/socket.h>

#ifdef K_WINDOWS
#include <ws2tcpip.h>
#endif
//---------------------------------------------------------------------------
namespace Kway {
/*/////////////////////////////////////////////////////////////////////////*/
class TDomainResolutionThr;

LIBNS_class TSockAddr
{
   union {
      struct sockaddr_in   IPv4_;
      struct sockaddr_in6  IPv6_;
   };
   int                     Family_;
   unsigned                Port_;
   bool                    IsAddrReady_;
public:
   inline K_ctor TSockAddr () : Family_(AF_INET), Port_(0), IsAddrReady_(false) {
   }

   // family = AF_INET, AF_INET6
   K_mf(void) Reset (int family);

   inline K_mf(int) GetFamily () const {
      return Family_;
   }
   inline K_mf(bool) IsIPv6() const {
      return Family_ == AF_INET6;
   }
   inline K_mf(int) GetPort () const {
      return Port_;
   }
   inline K_mf(bool) IsAddrReady () const {
      return IsAddrReady_;
   }

   inline K_mf(bool) IsAddrAny() const {
      #ifdef LIBNS_NO_IP6
         return(IPv4_.sin_addr.s_addr==INADDR_ANY);
      #else
         return(IsIPv6() ? memcmp(&IPv6_.sin6_addr, &in6addr_any, sizeof(in6addr_any))==0 : (IPv4_.sin_addr.s_addr==INADDR_ANY));
      #endif
   }
   inline K_mf(const struct sockaddr*) GetSockAddr () const {
      return (const struct sockaddr*)&IPv4_;
   }
   inline K_mf(int) GetSockAddrLen () const {
      return IsIPv6() ? sizeof(IPv6_) : sizeof(IPv4_);
   }

   K_mf(void) SetPort (unsigned port);
   K_mf(bool) SetIPAddress (const char* addr);
   K_mf(void) SetIPv6 (const struct in6_addr& ip6addr);
   K_mf(void) SetIPv4 (const struct in_addr& ip4addr);
};

LIBNS_class TDomainAddress //: public ost::InetHostAddress
{
   std::string DomainAddress_; // domain name or ip address
   TSockAddr   SockAddr_;

   friend class TDomainResolutionThr;
   virtual K_mf(void) IPAddressReady() = 0;
protected:
   ~TDomainAddress ();

   inline K_mf(void) SetDomainAddress (const std::string& domainAddress, int family) {
      DomainAddress_ = domainAddress;
      SockAddr_.Reset(family);
   }
   inline K_mf(void) SetPort (int port) {
      SockAddr_.SetPort(port);
   }

   K_mf(void) StartAddressResolution ();
   K_mf(void) StopAddressResolution ();

public:
   K_mf(const TSockAddr&) GetDomainSockAddr() const {
      return SockAddr_;
   }
};
/*/////////////////////////////////////////////////////////////////////////*/
LIBNS_class TMesTcpCliSetting;

LIBNS_class TMesTcpConnecting
{
   friend class TMesTcpCliSetting; 

   virtual K_mf(void) OnIPConnecting (TMesTcpCliSetting*, int eno) = 0;
};
/////////////////////////////////////////////////////////////////////////////
// �B�z Mes TCP Client �]�w, �榡�p�U:
// RemoteIP:RemotePort[,[<B>bindAds:][N]localPort [<M>mergeSize]] [A[ms]] [<S>szSndBuf] [n] [<R>RetrySeconds]
//   A = Keep Alive.
//   N = Not Reuse localPort(�S�['N',�h�w�]��true)
//   M = 0:���X��, >0:�h��Send()�i�X�֦��@��(TCP�ʥ])�ǰe, �w�]=1024*8
//   S = Size of send buffer(TCP option).
//   n = TCP_NODELAY = true, �q�`�ݷf�t M0
//  nn = TCP_NODELAY = false
//       (�S�[n�h�� OS�w�]=�q�`��false)
//  RetrySeconds �w�]�� 10
// �Ҧp:
//    127.0.0.1:1234,16996
//       => �s�� 127.0.0.1:1234, LocalPort=16996
//    192.168.3.1:1234,B192.168.3.6:N16996
//       => �s�u�� 192.168.3.1:1234, �ϥ�192.168.3.6������, LocalPort=16996, �Y16996�Q�e��, �h�|�s�u����
//    192.168.3.1:1234,B192.168.3.6:
//       => �s�u�� 192.168.3.1:1234, �ϥ�192.168.3.6������, �����w LocalPort
LIBNS_class TMesTcpCliSetting : public TDomainAddress
{
   mutable TMesTcpConnecting* ConnectingHandler_;
   mutable SOCKET             ConnectSo_;
   TSockAddr                  Remote_;

   K_mf(void) IPAddressReady();

public:
   #ifdef K_WINDOWS
      typedef BOOL   sckopt_bool_t;
   #else
      typedef int    sckopt_bool_t;
   #endif

   TSockAddr            Local_;
   size_t               MergeSize_;
   sckopt_bool_t        LocPortReuse_;
   sckopt_bool_t        TcpNoDelay_;
   int                  SizeSendBuf_;
   sckopt_bool_t        KeepAlive_;
   unsigned long        KeepAliveTime_;
   std::string          Setting_;
   int                  RetrySeconds_;
   /// >=0: l_onoff=1, l_linger=LingerSecs_;
   /// =-1: l_onoff=0, l_linger=0;
   /// else: dont change linger options.
   int                  LingerSecs_;

   K_ctor TMesTcpCliSetting() {
      SetDefault();
   }
   virtual ~TMesTcpCliSetting() {
      Close();
   }
   K_mf(std::string) SetSocketOpt (SOCKET so) const;
   K_mf(bool)        SettingApply (const std::string& setting, bool hasRemote);
   K_mf(std::string) GetSetting   () const;
   K_mf(void)        SetDefault   ();

   //inline K_mf(const struct sockaddr_in&) GetRemoteAddress() const {
   //   return Remote_;
   //}

   K_mf(void) Connect (SOCKET so, TMesTcpConnecting* handler) const;
   K_mf(void) Close   () const;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

