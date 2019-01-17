//---------------------------------------------------------------------------
#ifndef bxRptH
#define bxRptH
//---------------------------------------------------------------------------
#include <map>
#include "char_ary.h"
#include "bxDef.h"
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
K_class TR1 : public TTseControlHead            //成交回報起始作業訊息
{
   typedef TTseControlHead      inherited;
   typedef TR1                  this_type;
public:
   Kstl::char_ary<char,4,' '>   BrokerID_;
   Kstl::char_ary<char,6,'0'>   StartSeq_;
   K_ctor TR1(TSubSysName);
};
//---------------------------------------------------------------------------
K_class TR2 : public TTseControlHead            //成交回報起始作業回覆訊息
{
   typedef TTseControlHead      inherited;
   typedef TR2                  this_type;
public:
   Kstl::char_ary<char,4,' '>   BrokerID_;
   Kstl::char_ary<char,6,'0'>   StartSeq_;
};
//---------------------------------------------------------------------------
struct TRptBody
{
   Kstl::char_ary<char,6,' '>   StkNo_;
   Kstl::char_ary<char,8,'0'>   MthQty_;
   Kstl::char_ary<char,6,'0'>   MthPri_;
   Kstl::char_ary<char,6,' '>   MthTime_;
   Kstl::char_ary<char,2,' '>   MthTimems_;
   Kstl::char_ary<char,1,' '>   ExchageCode_;
   Kstl::char_ary<char,1,' '>   BSCode_;
   Kstl::char_ary<char,5,' '>   OrderNo_;
   Kstl::char_ary<char,7,' '>   IvacNo_;
   Kstl::char_ary<char,1,' '>   OrderType_;
   Kstl::char_ary<char,6,' '>   SeqNo_;
   Kstl::char_ary<char,4,' '>   BrokerID_;
   Kstl::char_ary<char,8,' '>   RecNo_;                 //成交總檔編號
   Kstl::char_ary<char,1,' '>   MarkS_;                 //補送註記,若為補送為"*"
};
//---------------------------------------------------------------------------
K_class TR3 : public TTseControlHead                    //成交回報資料
{
   typedef TTseControlHead      inherited;
   typedef TR3                  this_type;
public:
   Kstl::char_ary<char,4,'0'>   BodyLength_;
   Kstl::char_ary<char,2,'0'>   BodyCount_;
   TRptBody                     Body_[48];               //最多48筆預留固定空間
};
//---------------------------------------------------------------------------
K_class TR5 : public TTseControlHead    //成交回報確定連線作業回覆訊息
{
   typedef TTseControlHead      inherited;
   typedef TR5                  this_type;
public:
   K_ctor TR5(TSubSysName);
};
//---------------------------------------------------------------------------
K_class TR6 : public TTseControlHead    //結束成交回報資料傳送訊息
{
   typedef TTseControlHead      inherited;
   typedef TR6                  this_type;
public:
   Kstl::char_ary<char,6,'0'>   TotalRec_;              //成交資料總筆數
};
//---------------------------------------------------------------------------
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
