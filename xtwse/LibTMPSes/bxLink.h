//---------------------------------------------------------------------------
#ifndef bxLinkH
#define bxLinkH
//---------------------------------------------------------------------------
#include <map>
#include "bxDef.h"
#include "char_ary.h"
#include "TwStkBroker.h"
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
K_class TL030 : public TTseControlHead
{
   typedef TTseControlHead      inherited;
   typedef TL030                this_type;
public:
   TAppendNo    AppendNo_;
};
//---------------------------------------------------------------------------
K_class TL040 : public TTseControlHead
{
public:
   TAppendNo    AppendNo_;
   TBrkID       BrokerID_;
   TApCode      ApCode_;
   TKeyValue    KeyValue_;
};
//---------------------------------------------------------------------------
enum TLinkErrorCode{
   lec_Success = 0,
   lec_AppNoErr = 1,
   lec_BrkIDErr = 2,
   lec_ApCodeErr = 3,
   lec_KeyValueErr = 4,
   lec_SysNotReady = 5,
   lec_TimeErr     = 6,
   lec_SubSysErr   = 0x51,
   lec_FunCodeErr  = 0x52,
   lec_MsgTypeErr  = 0x53,
   lec_MsgTimeErr  = 0x54,
   lec_StatusErr   = 0x55,
   lec_TradeSuspend = 0x56,
   lec_MsgTimeOut = 0x5b,
   lec_MsgLenErr  = 0x5c,
   lec_MsgFmtErr  = 0x5d,
   lec_RSMsgErr   = 0x5e,
   lec_UnknowMsg  = 0x5f,
   lec_CallTse    = 0x63,
};

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
