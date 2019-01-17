/**
   讀取 serverListIni:
      hostid = ip  services(提供的服務)
   例如:
      1 = 192.168.1.1  FS,TS,FR,TR
      2 = 192.168.2.1  FS,TS
*/
//---------------------------------------------------------------------------
#ifndef ServerListH
#define ServerListH
//---------------------------------------------------------------------------
#include <map>
#include <string>
//---------------------------------------------------------------------------
#ifdef KWAY_BUILD_API
   #define LIBNS_class     class
   #define K_ctor
   #define K_dtor
   #define K_mf(rtn)       rtn
   #define TServerDef      TKwayServerDef
   #define TServerList     TKwayServerList
   #define TExcludeHostid  TKwayExcludeHostid
#else
   #include "KwayBase.h"
   namespace Kway {
#endif
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TServerDef
{
   std::string    IP_;
   std::string    Services_;
public:
#ifdef _MSC_VER
   K_ctor TServerDef() {}//(for avoid stl error)
#endif
   K_ctor             TServerDef (const std::string& ip, const std::string& srvs);
   K_mf(std::string)  GetIpPort  (size_t portNo) const;
   K_mf(bool)         HasService (const char*)   const;

   K_mf(const std::string&) GetIp      () const    { return IP_;       }
   K_mf(const std::string&) GetServices() const    { return Services_; }
};
//---------------------------------------------------------------------------
LIBNS_class TServerList : public std::map<size_t, TServerDef>
{
public:
   K_ctor      TServerList  (const char* serverListIni);
   K_ctor      TServerList  ();
   K_mf(void)  AddFromIni   (const char* serverListIni);
   K_mf(void)  ExcludeIni   (const char* excludeIniName);
   K_mf(void)  ExcludeHost  (size_t hostID);
};
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TExcludeHostid
{
   int   HostID_;
public:
   K_ctor  TExcludeHostid  (const char* exclude);

   K_mf(int)  GetHostID  ()  const  { return HostID_; }
};
/////////////////////////////////////////////////////////////////////////////
#ifndef KWAY_BUILD_API
   } // namespace Kway
#endif
//---------------------------------------------------------------------------
#endif

