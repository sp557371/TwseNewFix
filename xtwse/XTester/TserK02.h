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
   char_ary<char,  8, ' '>  BkTestNo_;       // ��Ӻݤ����սu��
   char_ary<char,  5, '0'>  BkPvcNo_;        // �Ҩ�ݤ�PVC NO �� SOCKET PORT NO
   char_ary<char,  1, ' '>  MarketKind_;     // �����O(T:��������, O:�d�i�R�椤��)
   char_ary<char,  1, ' '>  IpX25Kind_;      // IP �� X.25�s�u�O
   char_ary<char,  8, ' '>  ApplyDate_;      // �ӽд��դ��
   char_ary<char, 10, ' '>  BrokerNo_;       // �Ҩ�ӥN��
   char_ary<char,  4, '0'>  ApplySeqNo_;     // �ӽЦ���Ǹ�
   char_ary<char,  4, '0'>  Password_;       // �K�X
   char_ary<char, 26, ' '>  PortName_;       // TSE PORT NAME �� GATEWAY
   char_ary<char,  8, '0'>  CompleteDate_;   // ���է������
   char_ary<char,  8, '0'>  CompleteTime_;   // ���է����ɶ�
   char_ary<char,  3, '0'>  Result_;         // ���յ��G
   char_ary<char, 15, ' '>  IpAddress_;      // �Ұ�IP ADDRESS
   char_ary<char,  1, ' '>  Type_;           // �u���]�w�O(F:FIX�u���]�w, K:TMP�u���]�w)
   char_ary<char,  2, ' '>  FixSkid_;        // TMP�ҹ�����FIX SOCKET ID
   char_ary<char,  1, ' '>  FixVersion_;     // FIX �����O('1':4.2, '2':4.4)
   char_ary<char, 15, ' '>  Filler_;
};
*/
struct STserK02Data2
{
   char_ary<char,  8, ' '>  BkTestNo_;       // ��Ӻݤ����սu��
   char_ary<char,  5, '0'>  BkPvcNo_;        // �Ҩ�ݤ�PVC NO �� SOCKET PORT NO
   char_ary<char,  1, ' '>  MarketKind_;     // �����O(T:��������, O:�d�i�R�椤��)
   char_ary<char,  4, ' '>  BrokerNo_;       // �Ҩ�ӥN��
   char_ary<char,  2, ' '>  PVCID_;          // PVCID
   char_ary<char,  1, ' '>  APCode_;         // �@�~�O 0: �e�U�U�� 3: ����^��
   char_ary<char,  4, '0'>  Password_;       // �K�X
   char_ary<char,  1, 'N'>  Type_;           // �u���]�w�O(N:�sFIX�u���]�w)
   char_ary<char,  1, ' '>  FixVersion_;     // FIX �����O('1':4.2, '2':4.4)
   char_ary<char, 15, ' '>  IP1_;            // �ҰӲĤ@��IP ADDRESS
   char_ary<char, 15, ' '>  IP2_;            // �ҰӲĤG��IP ADDRESS
   char_ary<char,  5, ' '>  FwdGrp_;         // Forwarder �s��
   char_ary<char,  5, ' '>  AppGrp_;         // Appia �s��
   char_ary<char, 15, ' '>  DestIP_;         // DESTINATION IP
   char_ary<char,  5, ' '>  ListPort1_;      // LISTEN PORT 1
   char_ary<char,  5, ' '>  ListPort2_;      // LISTEN PORT 2
   char_ary<char,  6, ' '>  ApplySeqNo_;     // �ӽЦ���Ǹ�
   char_ary<char,  8, ' '>  ApplyDate_;      // �ӽд��դ��
   char_ary<char,  8, '0'>  CompleteDate_;   // ���է������
   char_ary<char,  8, '0'>  CompleteTime_;   // ���է����ɶ�
   char_ary<char,  3, '0'>  Result_;         // ���յ��G
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
