//
// 使用權/零股使用權
//    class TUPTwStkRights;
//
// 版本: $Id: UPTwStkRights.h,v 1.3 2003/12/03 02:58:59 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef UPTwStkRightsH
#define UPTwStkRightsH
//---------------------------------------------------------------------------
//
// 政策名稱 = TUPTwStkRights::PolicyName
//            TUPTwStkRights::OddPolicyName
// 政策內容 = TUPTwStkRights
//
#include "libnsTwStk.h"
#include "Signon/Policy/PolicyBase.h"
#include <bitset>
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
namespace Policy {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   TUserTwStkRightsItem;
   enum TUserTwStkRightsItem_t
#else
   enum TUserTwStkRightsItem
#endif
{  //與 ProgramTrade\LogonMgr\ExLogon.h 中的定義完全相同
         trLnMgr,       // 登入資料庫管理員
         trLnMgrMCust,  // 允許維護代管帳號
         trLnMgrMSale,  //         代管營業員

         trOrder,       // 允許委託買進賣出
         trDelete,      // 允許取消
         trChange,      // 允許改量
         trQuery,       // 允許查詢
         trCustNo,      // 允許輸入客戶帳號
         trSaleNo,      // 允許輸入營業員代號
         trOrderNo,     // 允許輸入委託書號,直接改量取消
         trAutoQty,     // 允許自動計算一套張數
         trSysMon,      // 系統監看
         trChkMCust,    // 委託時,檢查代管帳號,非代管帳號不可委託
         trChkMSale,    // 委託時,檢查代管營業員,非代管營業員不可委託
         trQryBal,      // 允許庫存查詢
         trQryCust,     // 允許客戶資料查詢
         trRptMSale,    // 需要回報代管營業員的委託書?
         trRevOrder,    // 允許反沖
         trTrialMul,    // 允許 * 試算
         trTrialDiv,    // 允許 / 試算
         trNoSc,        // 安控失敗仍允許委託
         trErReport,    // 允許錯帳申報
         trReForce,     // 允許強迫其他使用者的安控委託書
         trMaExTabs,    // 允許使用者維護 Ex Tables, 風控相關資料表
         trMarkDeal,    // 允許使用者勾單
         trNoRptTermID, // 定櫃使用者,不依照櫃號回報委託
         trChkRevCrDb,  // 資買限資.券賣限券,檢查是否為當沖操作
         trRptCanForce, // 僅回報有[強迫權限]的[安控委託書]
         trChkCfmForce, // 委託安控檢查時,檢查 CfmTForce 是否已經強迫過
         trChkRvCustTotBSAmt, // 試算時,檢查反沖時,是否會超過投資上限
         trScChkDeal,         // 委託額度檢查前,先檢查已成交額度
         trCrossTerm,   //可跨櫃查詢刪單改量
         trCrossGroup,  //可跨群組系統監看

         trEnd,
} K_EnumPacked;
//---------------------------------------------------------------------------
struct TUserTwStkRights
{
   typedef std::bitset<trEnd>    TRightsFlag;
   TRightsFlag    Rights_;
   /*
   #ifdef __BORLANDC__
      #if sizeof(TRightsFlag) > ((trEnd+7)/8)
      #  error "不可將bitset<>直接存檔!!"
      #endif
   #endif
   */
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TUPTwStkRights : public TTRecPolicy<TUserTwStkRights>
{
   typedef TTRecPolicy<TUserTwStkRights>  inherited;
public:
   inline K_ctor      TUPTwStkRights  (const TPolicyBaseRef& ref)  : inherited(ref) {}
   static const char  PolicyName[];
   static const char  OddPolicyName[];//零股使用權
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Policy
} // namespace Kway
} // namespace Signon
//---------------------------------------------------------------------------
#endif

