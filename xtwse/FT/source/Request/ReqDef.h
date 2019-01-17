//---------------------------------------------------------------------------
#ifndef ReqDefH
#define ReqDefH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include "SesPacket.h"
#include "char_ary.h"
#include "FTFlowDef.h"
#include "ReqMgr.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
#define HEAD_CODE   0x1B
#define END_CODE    0xFE
//---------------------------------------------------------------------------
struct StXFile
{
  typedef Kstl::char_ary1<char, HEAD_CODE>THeadCode;
  typedef Kstl::char_ary1<char, ' ' >     TType;
  typedef Kstl::char_ary1<char, END_CODE> TEndCode;
  typedef Kstl::char_ary1<char, '0' >     TErrCode;
  typedef Kstl::char_ary <char, 4, '0'>   TLength;
  typedef Kstl::char_ary <char, 4, '0'>   TSeqNo;
  typedef Kstl::char_ary1<char, ' '>      TSecCode;
  typedef Kstl::char_ary <char, 6, '-'>   TBroker;
  typedef Kstl::char_ary <char, 6, '-'>   TFileCode;
  typedef Kstl::char_ary <char, 5, '0'>   TFNameLen;

  struct SHead
  {
    THeadCode   Code_;
    TLength     Len_;
  };
  struct SRequestBase
  {
    TSeqNo      SeqNo_;
    TSecCode    SecCode_;
    TBroker     Broker_;
  };
  struct SFileRequest : public SRequestBase
  {
    TFileCode   FileCode_;
    TLength     RecLen_;
    TFNameLen   NameLen_;
    char        NameData_[994];
  };
  struct SMsgRequest : public SRequestBase
  {
    TFileCode   FileCode_;
    TFNameLen   MsgLen_;
    char        MsgData_[994];
  };
  struct SPkt
  {
    SHead       Head_;
    TType       Type_;
    char        Body_[1024];
  };
};
//---------------------------------------------------------------------------
typedef TTSesPacket<StXFile::SPkt, sizeof(StXFile::SHead)>   TAPISesBase;
//---------------------------------------------------------------------------
enum eRetCode
{
  ercOK=0,          // success
  ercConnFail,      // connect fail
  ercSeqnoDup,      // Seqno dup
  ercDataErr,       // Data error
  ercFileNotExist,  // File Not Exist
  ercBrokerErr,     // Broker error
  ercSeqnoErr,      // Seqno error
};
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
//---------------------------------------------------------------------------
#endif

