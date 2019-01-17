/////////////////////////////////////////////////////////////////////////////
#if !defined(_TserK02_h_)
#define _TserK02_h_
//---------------------------------------------------------------------------
#include "char_ary.h"
#include "KFile.h"
#include "ApaSmpType.h"
#include "TwseRes.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;

   class TTserIni;
   struct SSmpMsgMiddlewareEvent;

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// struct STserK02Data2
/////////////////////////////////////////////////////////////////////////////
/*
struct STserK02Data2
{
   char_ary<char,  8, ' '>  BkTestNo_;       // 券商端之測試線路
   char_ary<char,  5, '0'>  BkPvcNo_;        // 證券端之PVC NO 或 SOCKET PORT NO
   char_ary<char,  1, ' '>  MarketKind_;     // 市場別(T:集中市場, O:櫃檯買賣中心)
   char_ary<char,  1, ' '>  IpX25Kind_;      // IP 或 X.25連線別
   char_ary<char,  8, ' '>  ApplyDate_;      // 申請測試日期
   char_ary<char, 10, ' '>  BrokerNo_;       // 證券商代號
   char_ary<char,  4, '0'>  ApplySeqNo_;     // 申請收文序號
   char_ary<char,  4, '0'>  Password_;       // 密碼
   char_ary<char, 26, ' '>  PortName_;       // TSE PORT NAME 或 GATEWAY
   char_ary<char,  8, '0'>  CompleteDate_;   // 測試完成日期
   char_ary<char,  8, '0'>  CompleteTime_;   // 測試完成時間
   char_ary<char,  3, '0'>  Result_;         // 測試結果
   char_ary<char, 15, ' '>  IpAddress_;      // 證商IP ADDRESS
   char_ary<char,  1, ' '>  Type_;           // 線路設定別(F:FIX線路設定, K:TMP線路設定)
   char_ary<char,  2, ' '>  FixSkid_;        // TMP所對應之FIX SOCKET ID
   char_ary<char,  1, ' '>  FixVersion_;     // FIX 版本別('1':4.2, '2':4.4)
   char_ary<char, 15, ' '>  Filler_;
};
*/
struct STserK02Data2
{
   char_ary<char,  8, ' '>  BkTestNo_;       // 券商端之測試線路
   char_ary<char,  5, '0'>  BkPvcNo_;        // 證券端之PVC NO 或 SOCKET PORT NO
   char_ary<char,  1, ' '>  MarketKind_;     // 市場別(T:集中市場, O:櫃檯買賣中心)
   char_ary<char,  4, ' '>  BrokerNo_;       // 證券商代號
   char_ary<char,  2, ' '>  PVCID_;          // PVCID
   char_ary<char,  1, ' '>  APCode_;         // 作業別 0: 委託下單 3: 成交回報
   char_ary<char,  4, '0'>  Password_;       // 密碼
   char_ary<char,  1, 'N'>  Type_;           // 線路設定別(N:新FIX線路設定)
   char_ary<char,  1, ' '>  FixVersion_;     // FIX 版本別('1':4.2, '2':4.4)
   char_ary<char, 15, ' '>  IP1_;            // 證商第一組IP ADDRESS
   char_ary<char, 15, ' '>  IP2_;            // 證商第二組IP ADDRESS
   char_ary<char,  5, ' '>  FwdGrp_;         // Forwarder 群組
   char_ary<char,  5, ' '>  AppGrp_;         // Appia 群組
   char_ary<char, 15, ' '>  DestIP_;         // DESTINATION IP
   char_ary<char,  5, ' '>  ListPort1_;      // LISTEN PORT 1
   char_ary<char,  5, ' '>  ListPort2_;      // LISTEN PORT 2
   char_ary<char,  6, ' '>  ApplySeqNo_;     // 申請收文序號
   char_ary<char,  8, ' '>  ApplyDate_;      // 申請測試日期
   char_ary<char,  8, '0'>  CompleteDate_;   // 測試完成日期
   char_ary<char,  8, '0'>  CompleteTime_;   // 測試完成時間
   char_ary<char,  3, '0'>  Result_;         // 測試結果
   char_ary<char, 35, ' '>  Filler_;
};

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif

/////////////////////////////////////////////////////////////////////////////
// class TTserK02
/////////////////////////////////////////////////////////////////////////////
class TTserK02
{
   TFile  File_;

public:
   K_ctor TTserK02 ();
   
   K_mf(void) AddRec (TTserIni&,
                      const TSmpString& strTandSesID,
                      ETwseFixRejectCode,
                      int pwd,
                      const SSmpMsgMiddlewareEvent&,
                      int fixVer);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TserK02_h_)
