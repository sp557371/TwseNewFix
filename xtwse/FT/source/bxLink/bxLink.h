//---------------------------------------------------------------------------
#ifndef bxLinkH
#define bxLinkH
//---------------------------------------------------------------------------
#include <map>
#include "bxHeadDef.h"
#include "char_ary.h"
/////////////////////////////////////////////////////////////////////////////
#pragma pack(1)
namespace Kway {
namespace Tw   {
namespace Bx   {
//---------------------------------------------------------------------------
typedef Kstl::char_ary <char, 3, 0>  TAppendNo;
typedef Kstl::char_ary <char, 4, 0>  TBrkID;
typedef Kstl::char_ary1<char,    0>  TApCode;
typedef Kstl::char_ary <char, 2, 0>  TKeyValue;
typedef Kstl::char_ary <char, 2, 0>  TPvcID;

static const TSubSys  snTSE     = 10;
static const TSubSys  snOTC     = 91;
static const TFunCode ckWakeup  = 10;   // �}���@�~
static const TFunCode ckLogon   = 20;   // �n���@�~
static const TFunCode ckLogout  = 30;   // ���u�@�~

static const TBxMsg mgL010 = 00;    // L010
static const TBxMsg mgL020 = 01;    // L020
static const TBxMsg mgL030 = 02;    // L030
static const TBxMsg mgL040 = 03;    // L040
static const TBxMsg mgL050 = 04;    // L050
static const TBxMsg mgL060 = 05;    // L060
static const TBxMsg mgL070 = 06;    // L070
static const TBxMsg mgL080 = 07;    // L080

typedef Kway::byte  TApKey; // AP-CODE
static const TApKey akStk= '0'; // ���q�ѥ���l�t��
static const TApKey akFT = '1'; // �浧�T���P�ɮ׶ǿ�l�t��
static const TApKey akOdd= '2'; // �s�ѥ���l�t��
static const TApKey akRpt= '3'; // ����^���l�t��
static const TApKey akBor= '4'; // �Эɥ���l�t��
static const TApKey akSae= '5'; // ������l�t��
static const TApKey akMrk= '6'; // ���ʥ���l�t��
static const TApKey akFix= '7'; // �L��w������l�t��
static const TApKey akEFT= '9'; // TCP/IP �X�R��-�浧�T���P�ɮ׶ǿ�l�t��
static const TApKey akERpt='A'; // TCP/IP �X�R��-����^���l�t��
//---------------------------------------------------------------------------
struct TL030 : public TControlHead
{
    typedef TControlHead inherited;
    typedef TL030        this_type;
public:
    TAppendNo    AppendNo_;
};
//---------------------------------------------------------------------------
struct TL040 : public TControlHead
{
    typedef TControlHead    inherited;
public:
    TAppendNo    AppendNo_;
    TBrkID       BrokerID_;
    TApCode      ApCode_;
    TKeyValue    KeyValue_;
};
//---------------------------------------------------------------------------
enum TLinkErrorCode{
   lec_Success      = 0,
   lec_AppNoErr     = 1,
   lec_BrkIDErr     = 2,
   lec_ApCodeErr    = 3,
   lec_KeyValueErr  = 4,
   lec_SysNotReady  = 5,
   lec_TimeErr      = 6,
   lec_SubSysErr    = 81,
   lec_FunCodeErr   = 82,
   lec_MsgTypeErr   = 83,
   lec_MsgTimeErr   = 84,
   lec_StatusErr    = 85,
   lec_TradeSuspend = 86,
   lec_MsgTimeOut   = 91,
   lec_MsgLenErr    = 92,
   lec_MsgFmtErr    = 93,
   lec_RSMsgErr     = 94,
   lec_UnknowMsg    = 95,
   lec_CallTse      = 99,
};
//---------------------------------------------------------------------------
/*
typedef std::map<TLinkErrorCode, std::string> TLinkErrInfo;
*/
//---------------------------------------------------------------------------
#pragma pack()
};  // Bx
};  // Tw
};  // Kway
//---------------------------------------------------------------------------
#endif
