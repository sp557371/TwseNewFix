//---------------------------------------------------------------------------
#ifndef bxDRH
#define bxDRH
//---------------------------------------------------------------------------
#include <map>
#include "bxHeadDef.h"
#include "bxLink.h"
//---------------------------------------------------------------------------
namespace Kway  {
namespace Tw    {
namespace Bx    {
namespace DR    {
using namespace Kstl;
#pragma pack(1)
/////////////////////////////////////////////////////////////////////////////
static const TSubSys  snRPTTse   = 50;  // TSE 成交回報 Sub-System Code
static const TSubSys  snRPTOtc   = 95;  // OTC 成交回報 Sub-System Code
static const TFunCode ckRPTLink  = 00;  // 成交回報起始作業 R1/R2/R4/R5
static const TFunCode ckRPTData  = 10;  // 成交回報資料     R3
static const TFunCode ckRPTEnd   = 20;  // 成交回報結束作業 R6

static const TBxMsg mgR1 = 00;    // 起始訊息
static const TBxMsg mgR2 = 01;    // 起始回覆訊息
static const TBxMsg mgR3 = 00;    // 成交回報資料訊息
static const TBxMsg mgR4 = 04;    // 確定連線作業訊息
static const TBxMsg mgR5 = 05;    // 確定連線作業回覆訊息
static const TBxMsg mgR6 = 00;    // 結束成交回報訊息

typedef Kstl::char_ary<char,7,' '>  TSeqNo; // 成交回報使用時要用6位, 交易所內部使用時要用7位
typedef Kstl::char_ary<char,4,' '>  TLength;
typedef Kstl::char_ary<char,2,' '>  TCount;
typedef Kstl::char_ary<char,7,' '>  TRecs;
typedef Kstl::char_ary<char,2,' '>  TPvcID;
typedef Kstl::char_ary<char,10,' '> TFixSesID;
//---------------------------------------------------------------------------
struct TDRBody{
   char_ary<char, 4,' '>  BrokerId_;
   char_ary<char, 7,' '>  IvacNo_;
   char_ary<char, 6,' '>  StkNo_;
   char_ary<char, 5,' '>  OrderNo_;
   char_ary<char, 1,' '>  FunctionCode_;
   char_ary<char, 1,' '>  LineBrk4_;      // 連線券商第四碼
   char_ary<char, 8,' '>  AfQty_;      
   char_ary<char, 6,' '>  Pri_;      
   char_ary<char, 1,' '>  LineBrk3_;      // 連線券商第三碼
   char_ary<char, 1,' '>  OrderType_;      
   char_ary<char, 1,' '>  BSCode_;      
   char_ary<char, 1,' '>  IvacNo_Flag_;      
   char_ary<char, 8,' '>  Date_;      
   char_ary<char, 8,' '>  OrderTime_;      
   char_ary<char, 8,' '>  BfQty_;      
   TPvcID                 PvcID_;      
   char_ary<char, 4,' '>  Printer_; 
   TSeqNo                 SeqNo_;     
};
//---------------------------------------------------------------------------
K_class TR1R2 : public TControlHead     //成交回報起始作業訊息(回覆) R1/R2
{
   typedef TControlHead inherited;
   typedef TR1R2        this_type;
public:
   TBrkID   BrkID_;
   TSeqNo   StartSeq_;
};
//---------------------------------------------------------------------------
K_class TR3 : public TControlHead       //成交回報資料 R3
{
   typedef TControlHead inherited;
   typedef TR3          this_type;
public:
   TLength  BodyLength_;
   TCount   BodyCount_;
   TDRBody  Body_[50];   //最多四筆預留固定空間
   #define szTR3_Head   (sizeof(TControlHead) + sizeof(TLength) + sizeof(TCount))
};
//---------------------------------------------------------------------------
K_class TR4R5 : public TControlHead {}; //成交回報確定連線作業回覆訊息 R4/R5
//---------------------------------------------------------------------------
K_class TR6 : public TControlHead       //結束成交回報資料傳送訊息  R6
{
   typedef TControlHead inherited;
   typedef TR6          this_type;
public:
   TRecs    TotalRec_;  //成交資料總筆數
};
//---------------------------------------------------------------------------
struct TBrkMPvc{
   TBrkID       BrkID_;
   KStk::TMarket Market_;
   TPvcID       PvcID_;
   TBrkMPvc() {};
   TBrkMPvc(const TBrkID& a, const KStk::TMarket& b, const TPvcID& c): BrkID_(a),Market_(b),PvcID_(c) {};
   K_mf(bool) operator < (const TBrkMPvc& a) const
      { return  Market_==a.Market_?
               (BrkID_ ==a.BrkID_?PvcID_<a.PvcID_:BrkID_<a.BrkID_):
               Market_ < a.Market_;
      }
   K_mf(std::string) as_string()const {return std::string(BrkID_.as_string() +"-"+
                                                          Market_.as_string()+"-"+
                                                          PvcID_.as_string());}
   K_mf(void)        assign(const char*){};
};
//---------------------------------------------------------------------------
#pragma pack()
}; // DR
}; // Bx
}; // Tw
}; // Kway
//---------------------------------------------------------------------------
#endif
