#ifndef bxHeadDefH
#define bxHeadDefH
//---------------------------------------------------------------------------
#include <stdio.h>
#include "KTime.h"
#include "char_ary.h"
#include "TwStk.h"
//---------------------------------------------------------------------------
#pragma pack(1)
//---------------------------------------------------------------------------
namespace KStk = Kway::Tw::Stk;
namespace Kway {
namespace Tw   {
namespace Bx   {
//---------------------------------------------------------------------------
// 交易所連線 Header
typedef Kstl::char_ary<char, 2, ' '>  TSubSysName;
typedef Kstl::char_ary<char, 2, '0'>  TFunctionCode;
typedef Kstl::char_ary<char, 2, '0'>  TMessageType;
typedef Kstl::char_ary<char, 6, '0'>  TMessageTime;
typedef Kstl::char_ary<char, 2, '0'>  TStatusCode;
#define szMsgKey    sizeof(TSubSysName)+sizeof(TFunctionCode)+sizeof(TMessageType)

typedef Kway::dword TWorkKey;       // as Sub-System Code *100 + Function Code
typedef Kway::word  TBxSize;        // 訊息長度
typedef Kway::word  TSubSys;        // Sub-System Code
typedef Kway::word  TFunCode;       // Function Code
typedef Kway::word  TBxMsg;         // Message Type
typedef Kway::word  TStCode;        // Status Code
//---------------------------------------------------------------------------
struct TControlHead
{
protected:
   TSubSysName          SubSysName_;
   TFunctionCode        FunctionCode_;
   TMessageType         MessageType_;
   TMessageTime         MessageTime_;
   TStatusCode          StatusCode_;
public:
   inline K_mf(TWorkKey) GetWorkKey() const
   {
     return SubSysName_.as_int()*100 + FunctionCode_.as_int();
   }
   inline K_mf(TSubSysName) GetSubSys() const     {return SubSysName_;}
   inline K_mf(TStatusCode) GetStCode()     {return StatusCode_;}
   inline K_mf(TFunCode)GetFun() const      { return (TFunCode)FunctionCode_.as_int();  }
   inline K_mf(TBxMsg)  GetMsg() const      { return (TBxMsg)MessageType_.as_int();     }
   inline K_mf(TStCode) GetCode()const      { return (TStCode)StatusCode_.as_int();     }
   inline K_mf(void)    SetHead(TWorkKey aKey, TBxMsg aMsg, TStCode aCode)
   {
     char buf[4];
     sprintf(buf, "%02d", aKey/100);
     SubSysName_  .assign(buf);
     sprintf(buf, "%02d", aKey%100);
     FunctionCode_.assign(buf);
     sprintf(buf, "%02d", aMsg);
     MessageType_.assign(buf);
     MessageTime_.assign(Kway::TKTime::Now().as_string(false));
     sprintf(buf, "%02d", aCode);
     StatusCode_.assign(buf);
   }
};
#define szTControlHead  (sizeof(TControlHead))
//---------------------------------------------------------------------------
struct TbxData : public TControlHead
{
protected:
   char Body_[4096];    //每個封包Body 最大4096 bytes 因TCP/IP版最大為4088
};
//---------------------------------------------------------------------------
#pragma pack()
};  // Bx
};  // Tw
};  // Kway
/////////////////////////////////////////////////////////////////////////////
#endif












