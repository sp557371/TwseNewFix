//---------------------------------------------------------------------------
#ifndef KwayNotifyH
#define KwayNotifyH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include "SesPacket.h"
#include "char_ary.h"
#include "KwayReq.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
#define HEAD_CODE   0x1B
#define END_CODE    0xFE
//---------------------------------------------------------------------------
struct StXShell
{
  typedef Kstl::char_ary1<char, HEAD_CODE>THeadCode;
  typedef Kstl::char_ary <char, 4, '0'>   TLength;
  typedef Kstl::char_ary1<char, END_CODE> TEndCode;
  typedef Kstl::char_ary <char, 6, ' '>   TBroker;
  typedef Kstl::char_ary <char, 6, ' '>   TFileCode;
  typedef Kstl::char_ary <char, 4, '0'>   TSeqNo;
  typedef Kstl::char_ary1<char, ' '>      TSecCode;
  typedef Kstl::char_ary1<char, ' ' >     TFlag;
  typedef Kstl::char_ary <char, 2, '0'>   TStatus;

  struct SHead
  {
    THeadCode   Code_;
    TLength     Len_;
  };
  struct SFeedBackBase
  {
    TBroker     Broker_;
    TFileCode   FileCode_;
    TSeqNo      SeqNo_;
    TSecCode    SecCode_;
    TFlag       Process_;
    TStatus     Status_;
  };
  struct SFeedBack : public SFeedBackBase
  {
    TLength     FileNameLen_;
    char        FileName_[512];
  };
  #define szFeedBackHead    (sizeof(StXShell::SFeedBackBase) + sizeof(StXShell::TLength))
  struct SShellReply
  {
    TFlag       ReplyCode_;
  };
  struct SPkt
  {
    SHead       Head_;
    char        Body_[128];
  };
};
//---------------------------------------------------------------------------
typedef TTSesPacket<StXShell::SPkt, sizeof(StXShell::SHead)>   TShellSesBase;
//---------------------------------------------------------------------------
#include "FTJobDef.h"
#include "FTFlowDef.h"
//---------------------------------------------------------------------------
K_class TSenderMgr;
K_class TShellSes : public TShellSesBase,
                    protected TFTNotifyHandler
{
    typedef TShellSesBase       inherited;
    typedef TFTNotifyHandler    inherited_Handler;
    typedef TShellSes           this_type;
private:
virtual K_mf(void) OnMesLinkReady   (Kway::TMesBase*);
virtual K_mf(void) OnMesLinkBroken  (Kway::TMesBase*, const std::string& reason);
    TSenderMgr*     SenderMgr_;
protected:
    // inherited by SesPacket
virtual K_mf(int)  GetPacketSize (StXShell::SPkt&);
virtual K_mf(int)  BeforeAPacket (StXShell::SPkt&);
virtual K_mf(bool) APacket       (StXShell::SPkt&);
public:
        K_ctor     TShellSes (TSenderMgr* aOwner);
        K_dtor    ~TShellSes();
        K_mf(bool) SendFeedBack(StXFile::SFileRequest&, char aProcFlag, char* aReplyCode, int aSeqNo=-1);
virtual K_mf(bool) DoNotify(const TFTJob&);
        K_mf(void) SendFileCmd   (bool, int, std::string&){};
};
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif
