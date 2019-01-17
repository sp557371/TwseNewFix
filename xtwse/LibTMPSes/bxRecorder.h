//---------------------------------------------------------------------------
#ifndef bxRecorderH
#define bxRecorderH
//---------------------------------------------------------------------------
#include <stdio.h>
#include <vector>
#include "char_ary.h"
#include "comp6.h"
#include "bxDef.h"
#include "bxOrder.h"
#include "bxRpt.h"
#include "KFile.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace bx {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
struct TBSOK
{
   Kstl::char_ary<char,2,' '>   PVC_;
   Kstl::char_ary<char,2,' '>   LineNo_;
   Kstl::char_ary<char,2,' '>   SubSys_;
   Kstl::char_ary<char,2,' '>   FnCode_;
   Kstl::char_ary<char,2,' '>   MgType_;
   Kstl::char_ary<char,6,' '>   MgTime_;
   Kstl::char_ary<char,2,' '>   StCode_;
   Kstl::char_ary<char,4,' '>   BrokerID_;
   Kstl::char_ary<char,2,' '>   LPVCID_;
   Kstl::char_ary<char,5,' '>   OrderNo_;
   Kstl::char_ary<char,7,' '>   CustNo_;
   Kstl::char_ary<char,1,' '>   CustFlag_;
   Kstl::char_ary<char,6,' '>   StkNo_;
   Kstl::char_ary<char,6,' '>   Price_;
   Kstl::char_ary<char,8,' '>   Qty_;
   Kstl::char_ary<char,1,' '>   BSCode_;
   Kstl::char_ary<char,1,' '>   ExchgCode_;
   Kstl::char_ary<char,1,' '>   OType_;
   Kstl::char_ary<char,4,' '>   OBrokerID_;
   Kstl::char_ary<char,6,' '>   TDate_;
   Kstl::char_ary<char,6,' '>   TTime_;
   Kstl::char_ary<char,8,' '>   BeforeQty_;
   Kstl::char_ary<char,8,' '>   AfterQty_;
   Kstl::char_ary<char,1,' '>   TFlag_;
   Kstl::char_ary<char,3,' '>   TseSaleNo_;
   Kstl::char_ary<char,2,' '>   Filler1_;
   Kstl::char_ary<char,1,' '>   ActOType_;
   Kstl::char_ary<char,1,' '>   Market_;
};
//---------------------------------------------------------------------------
struct TBSOK96
{
   Kstl::char_ary<char,2,' '>   LineNo_;
   Kstl::char_ary<char,2,' '>   SubSys_;
   Kstl::char_ary<char,2,' '>   FnCode_;
   Kstl::char_ary<char,2,' '>   MgType_;
   Kstl::TComp6<6>              MgTime_;
   Kstl::char_ary<char,2,' '>   StCode_;
   Kstl::char_ary<char,4,' '>   BrokerID_;
   Kstl::char_ary<char,2,' '>   LPVCID_;
   Kstl::char_ary<char,1,' '>   TermID_;
   Kstl::TComp6<4>              OrderNo_;
   Kstl::TComp6<6>              CustNo_;
   Kstl::char_ary<char,1,' '>   CustChk_;
   Kstl::char_ary<char,1,' '>   CustFlag_;
   Kstl::char_ary<char,6,' '>   StkNo_;
   Kstl::TComp6<6>              Price_;
   Kstl::TComp6<6>              Qty_;
   Kstl::char_ary<char,1,' '>   BSCode_;
   Kstl::char_ary<char,1,' '>   ExchgCode_;
   Kstl::char_ary<char,1,' '>   OType_;
   Kstl::TComp6<6>              YPrice_;
   Kstl::TComp6<2>              YQty_;
   Kstl::TComp6<8>              TDate_;
   Kstl::TComp6<6>              TTime_;
   Kstl::TComp6<6>              BeforeQty_;
   Kstl::TComp6<6>              AfterQty_;
   Kstl::char_ary<char,1,' '>   TFlag_;
   Kstl::char_ary<char,2,' '>   SLine_;
   Kstl::TComp6<4>              Htsq_;
   Kstl::TComp6<6>              Insq_;
   Kstl::TComp6<6>              Mgsq_;
   Kstl::TComp6<6>              KTime_;
   Kstl::char_ary<char,1,' '>   SCode_;
   Kstl::char_ary<char,1,' '>   OCode_;
   Kstl::char_ary<char,6,' '>   TLNo_;
   Kstl::char_ary<char,3,' '>   SalesNo_;
   Kstl::char_ary<char,12,' '>  Filler1_;
};
//---------------------------------------------------------------------------
struct TLDRKS
{
   Kstl::char_ary<char,4,' '>   SeqNo_;
   Kstl::char_ary<char,1,' '>   MidCode_;
   Kstl::char_ary<char,1,' '>   BranchNo_;
   Kstl::char_ary<char,1,' '>   Filler1_;
   Kstl::char_ary<char,5,' '>   OrderNo_;
   Kstl::char_ary<char,1,' '>   Filler2_;
   Kstl::char_ary<char,7,' '>   IvacNo_;
   Kstl::char_ary<char,1,' '>   Filler3_;
   Kstl::char_ary<char,1,' '>   OrderType_;
   Kstl::char_ary<char,1,' '>   Filler4_;
   Kstl::char_ary<char,6,' '>   StkNo_;
   Kstl::char_ary<char,1,' '>   ExChangeCode_;
   Kstl::char_ary<char,1,' '>   Filler5_;
   Kstl::char_ary<char,7,' '>   Qty_;
   Kstl::char_ary<char,1,' '>   Filler6_;
   Kstl::char_ary<char,8,' '>   Pri_;
   Kstl::char_ary<char,1,' '>   Filler7_;
   Kstl::char_ary<char,1,' '>   BSCode_;
   Kstl::char_ary<char,1,' '>   Amt_;
   Kstl::char_ary<char,1,' '>   Filler9_;
   Kstl::char_ary<char,2,' '>   Time_HH_;
   Kstl::char_ary<char,1,' '>   FillerHH_;
   Kstl::char_ary<char,2,' '>   Time_MM_;
   Kstl::char_ary<char,1,' '>   FillerMM_;
   Kstl::char_ary<char,2,' '>   Time_SS_;
   Kstl::char_ary<char,1,' '>   FillerSS_;
   Kstl::char_ary<char,7,' '>   RecNo_;
};
//---------------------------------------------------------------------------
K_class TbxRecorder
{
private:
   typedef TbxRecorder  this_type;
   typedef Kway::Tw::Stk::TStkOrder             TStkOrder;
   TFile                BFSDLog_;
   TFile                AFSDLog_;
   TFile                BSOKLog_;
   Kway::word           BSOKFmt_;

public:
   K_ctor            TbxRecorder(Kway::word);
   K_dtor            ~TbxRecorder();
   K_mf(bool)        WriteBFSD(TSesID&, const void*, size_t);
   K_mf(bool)        WriteAFSD(TSesID&, const void*, size_t);
   K_mf(bool)        WriteLog(TSesID&, const char*);
   K_mf(bool)        WriteBSOK(TT020* t020,TStkOrder& StkOrder);
   K_mf(bool)        WriteBSOK(TO020* t020,TStkOrder& StkOrder);
   K_mf(bool)        WriteBSOK(TA020* a020,TStkOrder& StkOrder);
   K_mf(bool)        WriteBSOK(TV020* v020,TStkOrder& StkOrder);
   K_mf(bool)        WriteBSOK(TE020* e020,TStkOrder& StkOrder);
   K_mf(bool)        WriteBSOK(TTseControlHead* t030,TStkOrder& StkOrder,TBrkID BrkID,TPvcID PvcID, bool broken=false);
   // 新單斷線或timeout 時寫入, TFlag = 'T', 無AfterQty_
};
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
   #pragma pack(pop)
#else
   #pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
} // namespace bx
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif
