//---------------------------------------------------------------------------
#ifndef bxRptH
#define bxRptH
//---------------------------------------------------------------------------
#include <map>
#include "bxHeadDef.h"
#include "bxLink.h"
//---------------------------------------------------------------------------
namespace Kway  {
namespace Tw    {
namespace Bx    {
namespace Rpt   {
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

typedef Kstl::char_ary<char,6,'0'>  TSeqNo;
typedef Kstl::char_ary<char,4,'0'>  TLength;
typedef Kstl::char_ary<char,2,'0'>  TCount;
typedef Kstl::char_ary<char,6,'0'>  TRecs;
//---------------------------------------------------------------------------
struct TRptBody
{
   typedef Kstl::char_ary<char,7,'0'>   TIvacNo;
   typedef Kstl::char_ary<char,6,' '>   TStkNo;
   typedef Kstl::char_ary<char,6,'0'>   TPri;
   typedef Kstl::char_ary<char,1,' '>   TBSCode;
   typedef Kstl::char_ary<char,5,'0'>   TOrdNo;
   typedef Kstl::char_ary<char,8,'0'>   TRecNo;
   typedef Kstl::char_ary<char,1,' '>   TMark;
   typedef Kstl::char_ary<char,1,' '>   TOrdType;
   typedef Kstl::char_ary<char,1,' '>   TExcCode;
   typedef Kstl::char_ary<char,6,'0'>   TTime;
   typedef Kstl::char_ary<char,2,'0'>   TTimems;
   typedef Kstl::char_ary<char,8,'0'>   TQty;


   TStkNo   StkNo_;     // [證券代號]，六位文數字
   TQty     MthQty_;    // [成交數量]，八位文數字，表示成交張數(普通股/外國股票)或成交股數(零股)
   TPri     MthPri_;    // [成交價格]，四位整數，二位小數，單位為元。
   TTime    MthTime_;   // [成交時間]，八位數字，表時，分，秒，微秒。
   TTimems  MthTimems_;
   TExcCode ExcCode_;   // [交易種類]，一位數字，表示交易種類 0︰普通股交易及外國股票交易 1︰鉅額交易 2︰零股交易
   TBSCode  BSCode_;    // [買賣別]，一位文數字 B︰買，S︰賣
   TOrdNo   OrdNo_;     // [委託書編號]，含下列二欄位內容：終端機代號：一位文數字   委託書編號：四位數字
   TIvacNo  IvacNo_;    // [投資人帳號]，七位數字。
   TOrdType OrdType_;   // [委託種類]，一位數字。
   TSeqNo   SeqNo_;     // [流水號]，六位數字，表示成交回報之順序。
   TBrkID   BrkID_;     // [證券商代號]，四位文數字。
   TRecNo   RecNo_;     // [成交總檔編號]，八位數字，表示此筆成交資料在成交總檔之序號。
   TMark    MarkS_;     // [補送註記]，一位文字，補送之成交資料，此欄位內容為"*"。
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
   TRptBody Body_[4];   //最多四筆預留固定空間
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
#pragma pack()
}; // Rpt
}; // Bx
}; // Tw
}; // Kway
//---------------------------------------------------------------------------
#endif
