//
// TwStk 的 [交易權限]設定, 及 提供[委託流程]回覆委託狀態的基本class:
//    class TTrProfile;
//
// 版本: $Id: TwStkTrProfile.h,v 1.3 2005/12/28 09:15:47 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkTrProfileH
#define TwStkTrProfileH
/////////////////////////////////////////////////////////////////////////////
#include "TwStkOrdFlow.h"
//---------------------------------------------------------------------------
#include "Signon/Policy/UPTwStkIvacNos.h"
#include "Signon/Policy/UPTwStkSaleNos.h"
#include "Signon/Policy/UPTwStkRights.h"
#include "Signon/Policy/UPTwStkScItems.h"
#include "Signon/Policy/UFTwStkRes.h"
//---------------------------------------------------------------------------
#include <set>
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
   LIBNS_class TSignonSvr;
}
namespace Tw {
namespace Stk {
/////////////////////////////////////////////////////////////////////////////
typedef std::set<Kway::Signon::Policy::TUserTwStkIvacRec>   TTrIvacNos;
typedef std::set<Kway::Signon::Policy::TUserTwStkSaleRec>   TTrSaleNos;
typedef Kway::Signon::Policy::TUserTwStkRights              TTrRights;
typedef Kway::Signon::Policy::TUserTwStkScItems             TTrScItems;
typedef Kway::Signon::Policy::TUserTwStkRes                 TTrRes;
typedef Kstl::char_ary<char,12,0>                           TTrUserID;
typedef Kstl::char_ary1<char,' '>                           TTrTerm;
//---------------------------------------------------------------------------
TWSTK_class  TTrProfile {
protected:
   TTrRights      TrRights_;     //使用權限
   TTrScItems     TrScItems_;    //風控項目設定
   TTrIvacNos     TrIvacNos_;    //代管帳號
   TTrSaleNos     TrSaleNos_;    //代管營業員
   TTrRes         TrRes_;        //交易資源
   TTrUserID      TrUserID_;     //使用者代號
   TTrTerm        TrTerm_;       //委託櫃號(定櫃)

public:
           K_ctor  TTrProfile  ();
   virtual K_dtor ~TTrProfile  ();

   //↓若 req.IsFinish() 則將 Reqs_ 的資料移除, 並傳回 false, 表示委託流程已結束
   //  否則傳回 true, 表示委託流程尚未結束
   virtual K_mf(bool) OnOrdReqUpdate  (TOrdFlow::TReqPtr);

   inline K_mf(const TTrRights&)   GetTrRights  () const { return TrRights_;  }  //使用權限
   inline K_mf(const TTrScItems&)  GetTrScItems () const { return TrScItems_; }  //風控項目設定
   inline K_mf(const TTrIvacNos&)  GetTrIvacNos () const { return TrIvacNos_; }  //代管帳號
   inline K_mf(const TTrSaleNos&)  GetTrSaleNos () const { return TrSaleNos_; }  //代管營業員
   inline K_mf(const TTrRes&)      GetTrRes     () const { return TrRes_;     }  //交易資源
   inline K_mf(const TTrUserID&)   GetTrUserID  () const { return TrUserID_;  }  //使用者代號
   inline K_mf(const TTrTerm)      GetTrTerm    () const { return TrTerm_;    }  //委託櫃號(定櫃)

   /** 載入相關的 user policys.
   */
   K_mf(void)  GetUserPolicys  (const std::string& user, const Kway::Signon::TSignonSvr*);
private:
   //Reqs_ 由 TOrdFlow::DoReq() 與 TTrProfile::~TTrProfile() 共同維護
   // 當req進入委託流程時, TOrdFlow::DoReq() 有責任將req物件加入 TrProfile->Reqs_
   // 當 TTrProfile解構時, 會將尚未結束的Reqs_ 的 TrProfile_ 設為預設值: 全部的 TrRights_... 皆為 NULL
   //                      如此一來,尚未結束的Reqs_將無法進行相關的風控檢查  
   friend class TOrdFlow;
   typedef TOrdFlow::TReqPtrs    TReqs;
   typedef TReqs::iterator       TReqi;
   TReqs    Reqs_;         // 透過此profile正在進行委託要求的物件
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif

