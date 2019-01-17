//---------------------------------------------------------------------------
#ifndef NotifyDefH
#define NotifyDefH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include "SesPacket.h"
#include "char_ary.h"
#include "ReqDef.h"
#include "FTJobDef.h"
#include "FTFlowDef.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
#define HEAD_CODE   0x1B
#define END_CODE    0xFE
#define HBInterval  120
//---------------------------------------------------------------------------
struct SNotifyPkt
{
  typedef Kstl::char_ary1<char, HEAD_CODE>THeadCode;
  typedef Kstl::char_ary <char, 4, '0'>   TLength;
  typedef Kstl::char_ary1<char, END_CODE> TEndCode;
  typedef Kstl::char_ary <char, 6, ' '>   TBroker;
  typedef Kstl::char_ary <char, 6, ' '>   TFileCode;
  typedef Kstl::char_ary <char, 4, '0'>   TSeqNo;
  typedef Kstl::char_ary1<char, ' '>      TSecCode;
  typedef Kstl::char_ary1<char, ' ' >     TFlag;
  typedef Kstl::char_ary1<char, ' ' >     TFunction;
  typedef Kstl::char_ary <char, 2,'0'>   TStatus;
  typedef Kstl::char_ary <char, 8,' '>   TFileSeq;
  typedef Kstl::char_ary <char,50,' '>   TMsg;

  struct SHead
  {
    THeadCode   Code_;
    TLength     Len_;
    TFunction   Fun_;      // '0':通知作業 '1':檔案上傳命令 '2':檔案下載作業 '3':HeartBeat 作業
  };
  struct SFeedBackBase
  {
    TBroker     Broker_;
    TFileCode   FileCode_;
    TSeqNo      SeqNo_;
    TSecCode    SecCode_;
    TFlag       Process_;
    TStatus     Status_;
    TMsg        ErrMsg_;      // 20070626 狀態為'E':錯誤的時侯要加送錯誤原因
  };
  struct SFeedBack : public SFeedBackBase
  {
    TLength     FileNameLen_;
    char        FileName_[64];
  };
  #define szFeedBackH    (sizeof(SNotifyPkt::SFeedBackBase) + sizeof(SNotifyPkt::TLength))
  struct SShellReply
  {
    TFlag       ReplyCode_;
  };
  struct SFileCmdReq{
     TFileSeq         SeqNo_;
     char             Cmd_[512];
  };
  struct SFileCmdEcho{
     TFileSeq         SeqNo_;
     TFlag            Result_; // '0':fail '1':success
  };
  struct SPkt
  {
    SHead       Head_;
    char        Body_[1024];
  };
};
//---------------------------------------------------------------------------
typedef TTSesPacket<SNotifyPkt::SPkt, sizeof(SNotifyPkt::SHead)>   TNotify3TBase;
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif
