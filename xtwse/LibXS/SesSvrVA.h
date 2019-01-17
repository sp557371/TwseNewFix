//
// �q�Tclasses��¦: Session-Messager
//    class TSesSvrVA; �D���ݪ� ViewArea Session
//
// ����: $Id: SesSvrVA.h,v 1.3 2005/04/26 07:21:10 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SesSvrVAH
#define SesSvrVAH
//---------------------------------------------------------------------------
#include "SesServer.h"
#include "Pk/PkSesVA.h"
#include "ViewAreaMgr.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Pk {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSesSvrVA : public TSesServer
{
   typedef TSesServer   inherited;

   class TWorkerVA : public TWorker
   {
      K_mf(TCheckResult)  OnPkCheck  (TSesPk&, const THead&, TSize recvSize);
      K_mf(void)          OnPkRecv   (TSesPk&, const THead&);
   };
   static TWorkerVA     VAWorker_;

   class   TImpl;
   TImpl*  Impl_;

   K_mf(void)  OnStateChanged  (const std::string& reason);
public:
   K_ctor  TSesSvrVA  (TWorkersBase&);
   K_dtor ~TSesSvrVA  ();

   K_mf(std::string) SetUserReady (TSignonSvr*, const std::string& user, TOnlineID);

   K_mf(void)  ReqVA  (const TSesVAReq&);

// 2005.2.22.��� SysMgrObjs.h �� GetSysMgrObjs().MainVAMgr_
// K_mf(void)  AddVAMgr  (const std::string& vaMgrName, Kway::TViewAreaMgr&);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Pk
} // namespace Kway
//---------------------------------------------------------------------------
#endif

