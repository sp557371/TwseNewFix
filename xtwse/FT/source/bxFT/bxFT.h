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
static const TSubSys  snFTTse   = 20;   // TSE �ɮ׶ǿ� Sub-System Code
static const TSubSys  snFTOtc   = 92;   // OTC �ɮ׶ǿ� Sub-System Code
static const TFunCode ckFTStart = 00;  // �ɮ׶ǿ�_�l�@�~
static const TFunCode ckFTCont  = 01;  // �ɮ׶ǿ餤�~�@�~
static const TFunCode ckFTMsg   = 02;  // �浧�T���ǿ�@�~
static const TFunCode ckFTEnd   = 03;  // �����ǿ�@�~
static const TFunCode ckFTLink  = 04;  // �T�w�s�u�@�~

static const TBxMsg mgF010 = 00;    // �_�l�T��
static const TBxMsg mgF020 = 01;    // �^�а_�l�T��
static const TBxMsg mgF030 = 02;    // �ǰe��ưT��
static const TBxMsg mgF040 = 03;    // �^�и�ưT��
static const TBxMsg mgF050 = 04;    // �ǰe�浧�T��
static const TBxMsg mgF060 = 05;    // �^�г浧�T��
static const TBxMsg mgF070 = 06;    // �����T��
static const TBxMsg mgF080 = 07;    // �^�е����T��
// �H�U��TCP/IP �X�R��
static const TBxMsg mgF170 = 0x08;  // �T�w�s�u�T��(08�|�{���O���X�k��)
static const TBxMsg mgF180 = 0x09;  // �^�нT�w�s�u�T��
static const TBxMsg mgF210 = 12;    // �ǰe��ưT��3976 Bytes
static const TBxMsg mgF220 = 13;    // �^�и�ưT��3976 Bytes
// �H�W

typedef Kstl::char_ary<char, 4, 0>  TIdentID;
typedef Kstl::char_ary<char, 4, 0>  TBodyLength;

typedef Kstl::char_ary<char, 3, 0>  TFileCode;  // �ɮץN�X
typedef Kstl::char_ary<char, 8, 0>  TFileSize;  // ����ɮפj�p(�HByte�����)
typedef Kstl::char_ary1<char, 0>    TEof;       // �Y���ɮ׳̫�@����ƶ�1,�_�h��0
typedef Kstl::char_ary<char, 994, 0>TData;      // ����ɮפ��e
typedef Kstl::char_ary<char, 995, 0>TMsgData;   // �浧�T�����e
// �H�U��TCP/IP �X�R��
typedef Kstl::char_ary<char, 3976,0>TLargeData; // 20060725 �ץ���3976 bytes ����ɮפ��e
//typedef Kstl::char_ary<char, 4058,0>TLargeData; // 4058 bytes ����ɮפ��e
// �H�W
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




