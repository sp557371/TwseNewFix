//---------------------------------------------------------------------------
#ifndef bxDefH
#define bxDefH
//---------------------------------------------------------------------------
#include <stdio.h>
#include "char_ary.h"
#include "TwStkBroker.h"
#include "TwStkOrdFlow.h"
#include "TwStkOrdCenter.h"
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
/////////////////////////////////////////////////////////////////////////////
typedef Kstl::char_ary<char,2,' '>  TSubSysName;
typedef Kstl::char_ary<char,2,' '>  TFunctionCode;
typedef Kstl::char_ary<char,2,' '>  TMessageType;
typedef Kstl::char_ary<char,6,' '>  TMessageTime;
typedef Kstl::char_ary<char,2,' '>  TStatusCode;
typedef Kstl::char_ary<char,3,' '>  TAppendNo;
typedef Kstl::char_ary<char,1,' '>  TApCode;
typedef Kstl::char_ary<char,2,' '>  TKeyValue;
typedef Kstl::char_ary<char,6,' '>  TbxTime;
typedef Kstl::char_ary<char,4,' '>  TBrkID;
typedef Kstl::char_ary<char,2,' '>  TPvcID;
typedef Kstl::char_ary<char,2,' '>  TCmp;
typedef Kstl::char_ary<char,7,' '>  TSesID;
//---------------------------------------------------------------------------
typedef Kstl::char_ary<char,4,' '>  TPacketKey;
typedef word   TSize;   //包含 THead 的訊息長度
typedef Kway::Tw::Stk::TOrdFlow::TReqPtr     TReqPtr;
//typedef Kway::Tw::Stk::TOrdCenter            TOrdCenter;
//---------------------------------------------------------------------------
K_class TTseControlHead
{
public:
   TSubSysName          SubSysName_;
   TFunctionCode        FunctionCode_;
   TMessageType         MessageType_;
   TMessageTime         MessageTime_;
   TStatusCode          StatusCode_;
   inline K_mf(TPacketKey)     GetPacketKey() const
   {
        TPacketKey Key(SubSysName_.as_string() + FunctionCode_.as_string());
        return Key;
   }

};
/////////////////////////////////////////////////////////////////////////////
static const TSize      CtlHeadSize = sizeof(TTseControlHead);
typedef TTseControlHead         THead;
/////////////////////////////////////////////////////////////////////////////
struct TbxRec : public THead
{
   char  Body_[4096];    //每個封包Body 最大4096 bytes
};
//---------------------------------------------------------------------------
typedef Kstl::char_ary<char,4,' '>  TSourceID;
typedef Kstl::char_ary<char,4,' '>  TObjectID;
typedef Kstl::char_ary<char,4,' '>  TBodyLength;
//---------------------------------------------------------------------------
struct TTseFTHead
{
   TSourceID            SourceID_;
   TObjectID            ObjectID_;
   TBodyLength          BodyLength_;
};
/////////////////////////////////////////////////////////////////////////////
static const TSize      FTHeadSize = sizeof(TTseFTHead);
/////////////////////////////////////////////////////////////////////////////
enum TPacketCode{
   pc_L010,
   pc_L020,
   pc_L030,
   pc_L040,
   pc_L050,
   pc_L060,
   pc_L070,
   pc_L080,
   pc_T010,
   pc_T020,
   pc_T030,
   pc_T040,
   pc_T050,
   pc_T060,
   pc_T070E,
   pc_O010,
   pc_O020,
   pc_O030,
   pc_A010,
   pc_A020,
   pc_A030,
   pc_A040,
   pc_A050,
   pc_V010,
   pc_V020,
   pc_V030,
   pc_V040,
   pc_V050,
   pc_E010,
   pc_E020,
   pc_E030,
   pc_E040,
   pc_E050,
   pc_R1,
   pc_R2,
   pc_R3,
   pc_R4,
   pc_R5,
   pc_R6,
};
/*
enum EMarketPh{
   mp_Normal,
   mp_Fixed,
   mp_Odd,
   mp_None,
};
*/
/////////////////////////////////////////////////////////////////////////////
K_class TSesMsg{
public:
  Kstl::char_ary<char, 4,' '> MP_;
  Kstl::char_ary<char,32,' '> msg_;
  K_mf(std::string) as_string(){return std::string(MP_.as_string()+"-"+msg_.as_string());};
};

/////////////////////////////////////////////////////////////////////////////
typedef Stk::EMarket EMarket;
typedef Stk::TMarket TMarket;
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

