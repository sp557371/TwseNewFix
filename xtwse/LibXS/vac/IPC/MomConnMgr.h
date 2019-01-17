//---------------------------------------------------------------------------
#ifndef MomConnMgrH
#define MomConnMgrH

#include "MomMgr.h"
#include "ExtFuncs.h"

using namespace Kway::Ms;
//---------------------------------------------------------------------------
class TMomConnMgr : public Kway::Ms::TMomMgr
{
   typedef Kway::Ms::TMomMgr  inherited;
public:
   K_ctor TMomConnMgr(const std::string& user, const std::string& pass)
      : inherited("VaRM-IPCMgr", Kway::Pk::TVer(1,0,0,0), user, pass)
      {
      }
};
//---------------------------------------------------------------------------
#endif
