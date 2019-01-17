//
// PkSignonWorkers - 簽入訊息處理者
//
// 版本: $Id: PkSignonWorkers.h,v 1.3 2005/08/26 01:36:15 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef PkSignonWorkersH
#define PkSignonWorkersH
//---------------------------------------------------------------------------
#include "SesPk.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
   LIBNS_class TSignonSvr;
}
namespace Pk {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSignonWorkers
{
   class    TImpl;
   //std::auto_ptr<TImpl>    Impl_;
   TImpl*   Impl_;
public:
   K_ctor  TSignonWorkers  (const TVer& serverVer, const std::string& serverName, Kway::Signon::TSignonSvr&);
   K_ctor  TSignonWorkers  (const TVer& serverVer, const std::string& serverName);//使用預設值: SignonSvr.cpp: GetGlobalSignonSvr()
   K_dtor ~TSignonWorkers  ();

   K_mf(TWorkersBase&)  GetMsgWorkers  ();

   static std::string   ivasUFrom;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Pk
} // namespace Kway
//---------------------------------------------------------------------------
#endif
