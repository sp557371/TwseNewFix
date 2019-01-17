//---------------------------------------------------------------------------
#ifndef __MesTSEClientH__
#define __MesTSEClientH__
//---------------------------------------------------------------------------
#ifdef __BORLANDC__
// cooper 這邊會造成重覆宣告 (E2238 Multiple declaration for 'fd_set')
//#include "winsock.h"
#else
#include <netinet/in.h>
#endif
#include "MesTCPClient.h"
#include "TimerThread.h"
#include "char_ary.h"
#include "KFile.h"
//---------------------------------------------------------------------------
namespace Kway {//---------------------------------------------------------------------------
#ifdef  CCXX_NAMESPACES
using namespace std;
using namespace ost;
#endif
/////////////////////////////////////////////////////////////////////////////
static const word Head = 0xFEFE;
static const word Trailer = 0xEFEF;
typedef Kstl::char_ary<char,2,0>     TCtrlCode;
////////////////////////////////////////////////////////////////////////////
K_class TTseHead
{
   typedef TTseHead   this_type;
   word                         Header_;
   TCtrlCode                    CtrlCode_;
   word                         PktSize_;
public:
   K_ctor       TTseHead()      {Header_ = Head;}
   inline K_mf(word)            GetHeader() const       { return Header_;}
   inline K_mf(TCtrlCode)       GetCtrlCode() const     { return CtrlCode_;}
   inline K_mf(void)            SetCtrlCode(char* code) { CtrlCode_.assign(code);}
   K_mf(word)                   GetPkSize () const      { return ntohs(PktSize_);}
   K_mf(void)                   SetPkSize(word size)    { PktSize_ = htons(size);}
};
//---------------------------------------------------------------------------
static const size_t HeadSize = sizeof(TTseHead);
//---------------------------------------------------------------------------
K_class TTsePK : public TTseHead
{
   char  Dat_[0x1000-sizeof(TTseHead)];  //每筆資料最多 4k bytes
public:
   K_mf(bool) CheckPacket();
};
//---------------------------------------------------------------------------
K_class TTseTrailer
{
   word Trailer_;
public:
   K_ctor TTseTrailer() { Trailer_ = Trailer;}
};
//---------------------------------------------------------------------------
static const size_t TailSize = sizeof(TTseTrailer);
//---------------------------------------------------------------------------
K_class THeartBeat : public TTseHead
{
   TTseTrailer  Trailer_;
public:
   K_ctor THeartBeat()
   {
        SetPkSize(0);
        SetCtrlCode("11");
   }
};
//---------------------------------------------------------------------------
enum TChkResult
{
   chk_HeadOK,    //此訊息OK,將訊息收完後,用 OnSocketRecv() 通知,不需要再呼叫 OnSocketCheck()
   chk_Continue,  //此訊息目前無法判斷是否OK,當有收到新資料時再呼叫 OnSocketCheck() 檢查一次
   chk_Error,     //此訊息有問題,將已收到的資料清除
};
//---------------------------------------------------------------------------
K_class TMesTSEClient : public TMesTCPClient
{
   typedef TMesTCPClient        inherited;
   typedef TMesTSEClient        this_type;

   TTsePK                       RecvPk_[2];             //RecvPk_[1]作保護用!
   char                         TMPBuf_[4096];          //儲存TMP訊息
   char                         bufforlog[128];
   size_t                       BufPos_;
   size_t                       RecvPos_;               //已收到的資料大小
   size_t                       PacketSize_;
   TChkResult                   PkResult_;
   size_t                       SkipBytes_;
   TTimer<this_type>            Timer_;
   TTimer<this_type>            HBTimer_;
   word                         RecvTimeOut_; 
   word                         TseTimeOut_;
   bool                         Conneted_;
   bool                         LinkReady_;
   bool                         LinkCfm_;
   TFile                        Logs_;
   int                          SPort_;
public:
   K_ctor       TMesTSEClient();
   K_dtor       ~TMesTSEClient();
   K_mf(bool)         Open       ();
   K_mf(bool)         Close      ();
   K_mf(bool)         SettingApply(const std::string&);
private:
   K_mf(void)   EraseTMPBuf      (size_t len);
   K_mf(int)    Send             (const void* buffer, size_t size);
   K_mf(int)    Recv             (void* buffer, size_t size);
   K_mf(void)   StartTimer       (size_t ms);
   K_mf(void)   StopTimer        ();
   K_mf(void)   ResetHBTimer     ();
   K_mf(void)   OnTimer          (TTimerBase*);
   K_mf(void)   OnHBTimer        (TTimerBase*);
   K_mf(void)   OnSocketDataIn   (size_t);
   K_mf(void)   OnSocketLinkReady();
   K_mf(void)   OnSocketLinkError(const std::string&);
   K_mf(TChkResult)   OnPkCheck  (const TTseHead& pk, size_t recvSize,size_t* PacketSize);
   K_mf(void)   OnPkRecv         (TTseHead& pk);
   K_mf(void)   WriteLog         (char*);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif //__MesTSEClientH__
