//
// TwStk 的 [成交明細]Record 及 [資料表] 定義
//
// 版本: $Id: TwStkDeal.h,v 1.2 2004/09/14 02:53:42 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkDealH
#define TwStkDealH
//---------------------------------------------------------------------------
#include "ev_map.h"
#include "KTime.h"
//---------------------------------------------------------------------------
#include "TwStk.h"
#include "TwStkTseOrdKey.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace Stk {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
struct TStkDeal//成交回報明細
{
   typedef Kstl::fixed_num<dword>   TMSeqNo; //市場成交序號
   typedef Kstl::fixed_num<dword>   TBSeqNo; //券商回報序號
   typedef TBSeqNo                  TKey;

   TKTime   Time_;   //交易所撮合時間
   TStkPri  Pri_;    //成交價
   TStkQty  Qty_;    //成交股數(最小交易單位)
   TKey     BSeqNo_; //券商成交回報序號
// TMSeqNo  MSeqNo_; //市場成交序號

#ifdef _MSC_VER
   K_mf(bool) operator==(const TStkDeal& r) const {
	   return memcmp(this, &r, sizeof(r)) == 0;
   }
#endif
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
//---------------------------------------------------------------------------
//成交回報明細表,每筆委託一個    ↓是否可以改用 ev_vector<>
TWSTK_class TStkDeals : public Kstl::ev_map<TStkDeal::TKey,TStkDeal>
{
   TTseOrdKey  TseOrdKey_;
public:
   inline K_mf(TTseOrdKey)  get_master_key  () const                 { return TseOrdKey_; }
   inline K_mf(void)        set_master_key  (const TTseOrdKey& k)    { TseOrdKey_ = k;    }

   //傳回true表示找到result指向指定的資料, 傳回false表示沒找到result指向可安插的位置
// K_mf(bool)  FindBSeqNo  (TStkDeal::TBSeqNo, const_iterator& result) const; //券商序號
   K_mf(bool)  FindMSeqNo  (TStkDeal::TMSeqNo, const_iterator& result) const; //市場序號
   //傳回true表示新增result指向新資料, 傳回false表示重複result指向舊資料
   K_mf(bool)  AddDeal     (TStkDeal::TMSeqNo,  //市場序號
                            TStkDeal::TBSeqNo,  //券商序號
                            TKTime,             //撮合時間
                            TStkPri,            //成交價
                            TStkQty,            //成交股數(最小交易單位)
                            const_iterator& result);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif
