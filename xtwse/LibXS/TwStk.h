//
// TwStk 的基本 classes 及 types
//
// 版本: $Id: TwStk.h,v 1.4 2005/12/28 08:57:38 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkH
#define TwStkH
//---------------------------------------------------------------------------
#include "char_ary.h"
#include "fixed_num.h"
#include "kutility.h"
#include "AlphaSeq.h"
#include "libnsTwStk.h"
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
typedef Kstl::char_ary<char,6,' '>  TStkNo;
typedef Kstl::char_ary<char,8,0>    TStkName;
typedef Kstl::fixed_dec<uint32,2>   TStkPri; //價格不可能 < 0, 所以使用 uint32
typedef Kstl::fixed_num<int32>      TStkQty; //數量[可能] < 0
typedef Kstl::fixed_dec<int64,2>    TStkAmt; //價金[可能] < 0
typedef Kstl::char_ary1<char,0>     TSfGroup;//[信用配額組別]:0-9,A-Z,a-z=組別,
                                             //Space,Null=全公司不分組, '-'=BranchNo
//---------------------------------------------------------------------------
inline TStkAmt operator*  (const TStkQty& qty, const TStkPri& pri)  { return(TStkAmt(qty,0) *= pri); }
inline TStkAmt operator*  (const TStkPri& pri, const TStkQty& qty)  { return(TStkAmt(qty,0) *= pri); }
inline TStkQty operator/  (const TStkAmt& amt, const TStkPri& pri)  { return TStkQty(static_cast<TStkQty::orig_type>(amt.get_orig()/pri.get_orig())); }
inline TStkPri operator/  (const TStkAmt& amt, const TStkQty& qty)  { return TStkPri(static_cast<TStkPri::orig_type>(amt.get_orig()/static_cast<TStkQty::orig_type>(qty)), 2); }
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef char   EMarket;
   enum EMarket_t
#else
   enum EMarket
#endif
{
      m_Tse    = 'T',  //上市
      m_Otc    = 'O',  //上櫃
      m_OtcR   = 'R',  //興櫃
      m_Unknow = '?',  //不明的市場別.
} K_EnumPacked;
TWSTK_class TMarket : public Kstl::EnumT<EMarket>
{
   typedef Kstl::EnumT<EMarket>  inherited;
public:
   inline K_ctor      TMarket    (EMarket m = m_Tse)    : inherited(m)                      { }
          K_ctor      TMarket    (const TStkNo&);     //: inherited(GetMarket(stkNo).Data_) { }
   K_mf(std::string)  as_string  (bool longStr=false) const;
 //K_mf(void)         assign     (const std::string&); 使用 EnumT<> 的預設 assign()

   static K_mf(TMarket)  DefaultGetMarket  (const TStkNo&);//預設取得 StkBase, 然後傳回 Market_
   //這樣↓定義在 gcc 有錯! 因為 TMarket 為傳回值!!  :(
   // typedef K_mf(TMarket)  (*TGetMarket)  (const TStkNo&);
   //所以只好放在 class TMarket 的外面定義
};
typedef TMarket  (K_FASTCALL *TGetMarket)  (const TStkNo&);//預設取得 StkBase, 然後傳回 Market_
TWSTK_gd(TGetMarket)               GetMarket;
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef char   EStkKind;
   enum EStkKind_t
#else
   enum EStkKind
#endif
{
      sk_Normal    = ' ',  //一般股票
      sk_Special   = 'A',  //一般特別股
      sk_StkBond   = 'B',  //轉換公司債
      sk_CBond     = 'c',  //公司債
      sk_BeneCert  = 'C',  //受益憑證(Beneficiary Certificate)
      sk_ETF       = 'E',  //指數股票型基金(ETF)
      sk_Foreign   = 'F',  //外國股票
      sk_ForeBond  = 'f',  //外國債券
      sk_GovBond   = 'G',  //中央登錄公債
      sk_ChgStk    = 'R',  //換股權利證書
      sk_TDR       = 'T',  //存託憑證(TDR)
      sk_Wrnt      = 'W',  //認購(售)權證(Warrant)
      sk_Otc2      = '2',  //二類股
      sk_GBond     = 'b',  //附認股權(G)公司債
      sk_GWrnt     = 'w',  //認股權憑證
      sk_GExecBond = 'e',  //附認股權(G)履約後之公司債
      sk_GSpecial  = 'g',  //附認股權(G)特別股
      sk_GToken    = 't',  //附認股權(G)履約後股款繳納憑證
      sk_REITs     = 'r',  //不動產證券化
} K_EnumPacked;
TWSTK_class TStkKind : public Kstl::EnumT<EStkKind>
{
   typedef Kstl::EnumT<EStkKind>    inherited;
public:
          K_mf(std::string)  as_string  (bool longStr=false) const;
   inline K_mf(void)         assign     (const std::string& s)    { Data_ = (EStkKind)(*s.c_str()); }
};
//---------------------------------------------------------------------------
//為了簡化Client查詢時的處理:將整個TStkBase直接傳送
//所以使用 #pragma pack 的選項
struct TStkBase
{
   TStkName    Name_;
   TMarket     Market_;    //交易市場:上市/上櫃/興櫃...
   TStkKind    Kind_;
   TStkPri     PriRef_;
   TStkPri     PriUpLmt_;
   TStkPri     PriDnLmt_;
   TStkPri     PriClose_;  //今日收盤價
   TStkPri     PriOdd_;    //零股參考價

   //傳回 TStkBase* 雖然不會造成 gcc 的問題, 但為了一致性(與 TMarket 相同)的寫法
   //所以還是將底下的定義放到 struct TStkBase 外面
   //static K_mf(const TStkBase*)  (*GetStkBase)  (const TStkNo&);
};
typedef const TStkBase*  (K_FASTCALL *TGetStkBase)  (const TStkNo&);
TWSTK_gd(TGetStkBase)                  GetStkBase;
typedef K_mf(const dword)    (*TGetStkUnitQty)  (const TStkNo&);
TWSTK_gd(TGetStkUnitQty)             GetStkUnitQty;
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
//---------------------------------------------------------------------------
// typedef Kstl::ev_map<TStkNo, const TStkBase>  TStkBases;
/////////////////////////////////////////////////////////////////////////////
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif

