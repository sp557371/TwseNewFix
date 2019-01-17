#ifndef bxFTH
#define bxFTH
//---------------------------------------------------------------------------
#include "bxHeadDef.h"
#include "char_ary.h"
#include <map>
/////////////////////////////////////////////////////////////////////////////
#pragma pack(1)
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
//---------------------------------------------------------------------------
static const TSubSys  snFTTse   = 20;   // TSE 檔案傳輸 Sub-System Code
static const TSubSys  snFTOtc   = 92;   // OTC 檔案傳輸 Sub-System Code
static const TFunCode ckFTStart = 00;  // 檔案傳輸起始作業
static const TFunCode ckFTCont  = 01;  // 檔案傳輸中繼作業
static const TFunCode ckFTMsg   = 02;  // 單筆訊息傳輸作業
static const TFunCode ckFTEnd   = 03;  // 結束傳輸作業
static const TFunCode ckFTLink  = 04;  // 確定連線作業

static const TBxMsg mgF010 = 00;    // 起始訊息
static const TBxMsg mgF020 = 01;    // 回覆起始訊息
static const TBxMsg mgF030 = 02;    // 傳送資料訊息
static const TBxMsg mgF040 = 03;    // 回覆資料訊息
static const TBxMsg mgF050 = 04;    // 傳送單筆訊息
static const TBxMsg mgF060 = 05;    // 回覆單筆訊息
static const TBxMsg mgF070 = 06;    // 結束訊息
static const TBxMsg mgF080 = 07;    // 回覆結束訊息
// 以下為TCP/IP 擴充版
static const TBxMsg mgF170 = 0x08;  // 確定連線訊息(08會認為是不合法的)
static const TBxMsg mgF180 = 0x09;  // 回覆確定連線訊息
static const TBxMsg mgF210 = 12;    // 傳送資料訊息3976 Bytes
static const TBxMsg mgF220 = 13;    // 回覆資料訊息3976 Bytes
// 以上

typedef Kstl::char_ary<char, 4, 0>  TIdentID;
typedef Kstl::char_ary<char, 4, 0>  TBodyLength;

typedef Kstl::char_ary<char, 3, 0>  TFileCode;  // 檔案代碼
typedef Kstl::char_ary<char, 8, 0>  TFileSize;  // 資料檔案大小(以Byte為單位)
typedef Kstl::char_ary1<char, 0>    TEof;       // 若為檔案最後一筆資料填1,否則為0
typedef Kstl::char_ary<char, 994, 0>TData;      // 資料檔案內容
typedef Kstl::char_ary<char, 995, 0>TMsgData;   // 單筆訊息內容
// 以下為TCP/IP 擴充版
typedef Kstl::char_ary<char, 3976,0>TLargeData; // 20060725 修正為3976 bytes 資料檔案內容
//typedef Kstl::char_ary<char, 4058,0>TLargeData; // 4058 bytes 資料檔案內容
// 以上
//---------------------------------------------------------------------------
struct TFTHead : public TControlHead
{
public:
    TIdentID    SourceID_;
    TIdentID    DestID_;
    TBodyLength BodyLen_;
    inline K_mf(void)  SetBodyLen(size_t aLen)
    {
      char buf[5];
      sprintf(buf, "%04d", aLen);
      BodyLen_.assign(buf);
    }
    inline K_mf(size_t)GetBodyLen() const { return BodyLen_.as_int(); }
};
//---------------------------------------------------------------------------
struct TF010 : public TFTHead
{
public:
    TFileCode   FileCode_;
    TFileSize   FileSize_;
};
#define szCODE_SIZE (sizeof(TF010)-sizeof(TFTHead))
//---------------------------------------------------------------------------
struct TF040 : public TFTHead
{
public:
    TFileCode   FileCode_;
    TEof        Eof_;
    K_mf(bool)  IsEof() const  { return (Eof_ == '1'); }
    K_mf(void)  SetEof(bool e) { Eof_ = e ? '1':'0';   }
};
#define szCODE_EOF  (sizeof(TF040)-sizeof(TFTHead))
//---------------------------------------------------------------------------
struct TF030 : public TF040
{
public:
    TData       Data_;
};
//---------------------------------------------------------------------------
struct TF050 : public TFTHead
{
public:
    TFileCode   FileCode_;
    TMsgData    Msg_;
};
#define szCode  (sizeof(TFileCode))
//---------------------------------------------------------------------------
struct TF210 : public TF040
{
public:
    TLargeData  LargeData_;
};
//---------------------------------------------------------------------------
enum TFTErrorCode
{
  fec_Success     = 0,
  fec_ErrFileCode = 10,
  fec_ErrEOF      = 11,
  fec_ErrFileSize = 12,
  fec_TimingError = 13,
  fec_FileNotReady= 14,
  fec_FileEmpty   = 17,
  fec_Abort       = 19,
  fec_DupRequest  = 79,
  fec_ErrSubName  = 81,
  fec_ErrFunCode  = 82,
  fec_ErrMsgType  = 83,
  fec_ErrMsgTime  = 84,
  fec_ErrStatCode = 85,
  fec_ErrSourceID = 86,
  fec_ErrObjectID = 87,
  fec_ErrBodyLen  = 88,
  fec_InternalErr = 89,
  fec_FTCountErr  = 92,
  fec_ReqCountErr = 93,
  fec_CallTSE     = 99,
};
//---------------------------------------------------------------------------
//typedef std::map<TFTErrorCode, std::string> TFTErrInfo;
//---------------------------------------------------------------------------
#pragma pack()
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif




