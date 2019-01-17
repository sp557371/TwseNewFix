//---------------------------------------------------------------------------
// $Id: sneTwStkStart_Aux.h,v 1.1 2004/07/14 05:31:17 fonwin Exp $
// �]�� alpha DEC UNIX �W, �Y�b sneTwStkStart.cpp ����
//    IvacMasterLog_, IvacDetailLog_, SaleMasterLog_, SaleDetailLog_
// �|�y�� g++ �O���餣�������~, �ҥH�N���U�����󲾨�o�ಣ��
// �p�G�٬O���uvirtual memory exhausted: Not enough space�v�����~,
// �յ۱N Makefile �̭��� -g -O2 �����ոլ�,
// �p�G�٬O�����~, �A�N���B�� Ivac �� Sale ���}.
//---------------------------------------------------------------------------
#ifndef sneTwStkStart_AuxH
#define sneTwStkStart_AuxH
//---------------------------------------------------------------------------
#include "UserTwStkIvacNos.h"    //�N�ޱb��
#include "UserTwStkSaleNos.h"    //�N����~��
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
