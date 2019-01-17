//
// Singon Server 端的處理 - 使用 SignonDB / 並提供Session的註冊
//
// 版本: $Id: SignonSvrDB.h,v 1.6 2005/04/26 07:43:39 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SignonSvrDBH
#define SignonSvrDBH
//---------------------------------------------------------------------------
#include "Signon/sneLoader.h"
#include "Signon/SignonSvr_DC.h"
#include "Signon/UserRemoteMgrPolicy.h"
#include "PkSignonWorkers.h"
#include "SesMgr.h"
//---------------------------------------------------------------------------
//#include "SesExtab.h"
namespace Kway {
namespace Extab {
   class TExtabUpdaterMgr;
}
}
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
/////////////////////////////////////////////////////////////////////////////
SIGDB_gd(::Kway::Extab::TExtabUpdaterMgr*)    SignonDB_ExtabMgr;
/////////////////////////////////////////////////////////////////////////////
class TSignonSvrDB_Base : public TSesReg
                        , public TSignonSvr_DataCenter
{
   typedef TSignonSvrDB_Base   this_type;

   static TSesBase*  SesCreater  (const TSesReg&, TSesMgr&);
   Pk::TSignonWorkers         PkWorkers_;
   TUserRemoteMgrPolicy_Mgr   UserRemoteMgrPolicy_;
   TViewAreaMgr               SignonVAMgr_;
   TViewAreaMgr               SignonVAMgrA_;
   TsneMgr                    sneModules_;
public:
   K_ctor  TSignonSvrDB_Base  ();
   K_ctor ~TSignonSvrDB_Base  ();

   inline K_mf(TsneMgr&)       GetSneModuleMgr ()  { return sneModules_;   }
   virtual K_mf(TViewAreaMgr*) GetSignonVAMgr  ()  { return &SignonVAMgr_; }
};
//---------------------------------------------------------------------------
typedef Kstl::wise_ref_obj<TSignonSvrDB_Base,unsigned>   TSignonSvrDB;
/////////////////////////////////////////////////////////////////////////////
//K_exportC(TSignonSvr*)  SignonSvrStart   ();
//K_exportC(bool)         SignonSvrFinish  (TSignonSvr*);
/////////////////////////////////////////////////////////////////////////////
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
SIGDB_exportC(const char)                SignonDefaultLogFN[];// = "./Logs/%4y%2m%2d/Signon.log"
///> 範例: ExtSignonSvrStart("OMS使用者管理", "./SignonDB/OMS/", SignonDefaultLogFN, NULL);
SIGDB_exportC(Kway::Signon::TSignonSvr*) ExtSignonSvrStart  (const char* vaName, const char* dbPath, const char* logFN, Kway::Signon::TSignonSvr* svr = NULL);
SIGDB_exportC(Kway::Signon::TSignonSvr*) ExtSignonSvrStart2 (const char* vaName, const char* dbPath);
SIGDB_exportC(bool)                      ExtSignonSvrFinish (Kway::Signon::TSignonSvr*);
//---------------------------------------------------------------------------
#endif

