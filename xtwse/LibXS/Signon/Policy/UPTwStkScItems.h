//
// 風控項目/零股風控項目
//
// 版本: $Id: UPTwStkScItems.h,v 1.3 2005/04/26 03:23:06 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef UPTwStkScItemsH
#define UPTwStkScItemsH
//---------------------------------------------------------------------------
//
// 政策名稱 = TUPTwStkScItems::PolicyName
//            TUPTwStkScItems::OddPolicyName
// 政策內容 = TUPTwStkScItems
//
#include "libnsTwStk.h"
#include "Signon/Policy/PolicyBase.h"
#include <string.h>
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
   typedef char   TScLevel;
   enum TScLevel_t
#else
   enum TScLevel
#endif
{
   slCanForce     = '0',   //可強迫
   slCannotForce  = '1',   //不能強迫
   slDontCheck    = '2',   //不檢查
   slSend2Mgr     = '3',   //送主管
   slMgrPass      = '4',   //放行密碼(Keyin端輸入放行密碼後強迫)
} K_EnumPacked;//因為 gcc3.2-3.3的 bug,加上 __attribute__((packed)); 會有compile的錯誤
//---------------------------------------------------------------------------
struct TUserTwStkScItems
{
   inline K_ctor  TUserTwStkScItems  ()  { memset(this, slCannotForce, sizeof(*this)); }

   union {
      TScLevel    ScItems_[80];
      #ifdef __BORLANDC__
         #if sizeof(TScLevel) != 1
         #  error "請重新定義 ScItems_[] 也許你必須在專案檔(.bpr)增加 -b- 的選項"
         #endif
      #endif
      struct {
         TScLevel    Fc0;
         TScLevel    FcNoT30;       // T30無資料
         TScLevel    FcPriceLimit;  // 超過漲跌停
         TScLevel    FcFullT;       // 全額交割
         TScLevel    FcNoPriLmt;    // 無漲跌幅股票之市價委託控管
         TScLevel    Fc5;
         TScLevel    Fc6;
         TScLevel    Fc7;
         TScLevel    Fc8;
         TScLevel    Fc9;

         TScLevel    FcStkDeposit;  //交易所警示股(預收款券)
         TScLevel    FcStkSC;       //券商內控股(帳號+股票)
         TScLevel    FcStkCpSC;     //(單一營業據點)+股票.安控
         TScLevel    FcIvacStkSC;   //個人個股安控(特定帳號+股票)
         TScLevel    Fc14;
         TScLevel    Fc15;
         TScLevel    Fc16;
         TScLevel    Fc17;
         TScLevel    Fc18;
         TScLevel    Fc19;

         TScLevel    FcCrStop;      // 停止融資
         TScLevel    FcDbStop;      // 停止融券
         TScLevel    FcCrQty;       // 融資配額不足
         TScLevel    FcDbQty;       // 融券配額不足
         TScLevel    FcToBal;       // 配額為0或停資券,但允許當沖
         TScLevel    FcNoFinaStk;   // 證金配額未建檔
         TScLevel    FcCrCQty;      // 資配額不足(但交易類別使用 ot_CrC)
         TScLevel    FcDbCQty;      // 券配額不足(但交易類別使用 ot_DbC)
         TScLevel    Fc28;
         TScLevel    FcBalSBLQty;   // 20090324 因應信用交易將sbl 庫存控管由原FcBalDbQty獨立出來

         TScLevel    FcBalGnQty; //庫存.投資上限.級數
         TScLevel    FcBalCrQty;
         TScLevel    FcBalDbQty;
         TScLevel    FcTotBSAmt; //買賣合計投資上限
         TScLevel    FcBalCrAmt;
         TScLevel    FcBalDbAmt;
         TScLevel    FcCr1StkAmt;//單股融資.融券超過
         TScLevel    FcDb1StkAmt;
         TScLevel    FcWarnAmt;  //風險戶單筆警示
         TScLevel    FcUpLmtAmt; //風險戶單筆最高

         TScLevel    FcCtrlBS; //整戶管制
         TScLevel    FcCtrlCD; //信用管制
         TScLevel    FcRisk;   //風險戶管制
         TScLevel    FcAmtLevel1;   //帳號委託金額分層管制第一級
         TScLevel    FcAmtLevel2;   //帳號委託金額分層管制第一級
         TScLevel    FcAmtLevel3;   //帳號委託金額分層管制第一級
         TScLevel    FcDormant;     //靜止戶(第一筆委託強迫後, 即解除靜止戶狀態)
         TScLevel    FcGroupAmt;    // 20070820群組投資額度
         TScLevel    FcNoKYC;       // 20080220無KYC
         TScLevel    FcExceedBound; // 20090617 外國股票超過上下界
      };
   };
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TUPTwStkScItems : public TTRecPolicy<TUserTwStkScItems>
{
   typedef TTRecPolicy<TUserTwStkScItems>  inherited;
public:
   inline K_ctor      TUPTwStkScItems  (const TPolicyBaseRef& ref)  : inherited(ref) {}
   static const char  PolicyName[];
   static const char  OddPolicyName[];//零股風控項目
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Policy
} // namespace Kway
} // namespace Signon
//---------------------------------------------------------------------------
#endif
