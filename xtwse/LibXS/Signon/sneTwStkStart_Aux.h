//---------------------------------------------------------------------------
// $Id: sneTwStkStart_Aux.h,v 1.1 2004/07/14 05:31:17 fonwin Exp $
// 因為 alpha DEC UNIX 上, 若在 sneTwStkStart.cpp 產生
//    IvacMasterLog_, IvacDetailLog_, SaleMasterLog_, SaleDetailLog_
// 會造成 g++ 記憶體不足的錯誤, 所以將底下的物件移到這兒產生
// 如果還是有「virtual memory exhausted: Not enough space」的錯誤,
// 試著將 Makefile 裡面的 -g -O2 拿掉試試看,
// 如果還是有錯誤, 再將此處的 Ivac 及 Sale 分開.
//---------------------------------------------------------------------------
#ifndef sneTwStkStart_AuxH
#define sneTwStkStart_AuxH
//---------------------------------------------------------------------------
#include "UserTwStkIvacNos.h"    //代管帳號
#include "UserTwStkSaleNos.h"    //代管營業員
#include "SesExtab.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Signon;
//---------------------------------------------------------------------------
class TUserIvacLogMgr {
   std::auto_ptr<TTableLog>            MasterLog_;
   std::auto_ptr<TTableLog2d>          DetailLog_;
   std::auto_ptr<TUserTwStkIvacMgr>    Recs_;
public:
   K_ctor  TUserIvacLogMgr  (::Kway::Extab::TExtabUpdaterMgr&,
                             const std::string& mGroupName,
                             const std::string& mTableName,
                             const std::string& dGroupName,
                             const std::string& dTableName,
                             const std::string& dbPath);
   inline K_mf(TUserTwStkIvacMgr::TMasterRecs&)  GetRecs  ()   { return *(Recs_->get_recs()); }
   inline K_mf(TPmBaseVA&)                       GetPmBase()   { return *Recs_;               }
};
//---------------------------------------------------------------------------
class TUserSaleLogMgr {
   std::auto_ptr<TTableLog>            MasterLog_;
   std::auto_ptr<TTableLog2d>          DetailLog_;
   std::auto_ptr<TUserTwStkSaleMgr>    Recs_;
public:
   K_ctor  TUserSaleLogMgr  (::Kway::Extab::TExtabUpdaterMgr&,
                             const std::string& mGroupName,
                             const std::string& mTableName,
                             const std::string& dGroupName,
                             const std::string& dTableName,
                             const std::string& dbPath);
   inline K_mf(TUserTwStkSaleMgr::TMasterRecs&)  GetRecs  ()   { return *(Recs_->get_recs()); }
   inline K_mf(TPmBaseVA&)                       GetPmBase()   { return *Recs_;               }
};
//---------------------------------------------------------------------------
#endif
