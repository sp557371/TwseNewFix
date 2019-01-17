//---------------------------------------------------------------------------
#ifndef FTJobDefH
#define FTJobDefH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include "bxLink.h"
#include "bxFT.h"
#include "fixed_num.h"
#include "TwStk.h"
#include <queue>
#include <set>
#include <vector>
#include "inn_file.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
//---------------------------------------------------------------------------
typedef Kstl::fixed_num<Kway::int32>    TSeqNo;
typedef KStk::TMarket                   TMarket;
typedef Kstl::char_ary<char, 50, 0>     TLastMsg;

static TBrkID   TSEID_("0000");
//---------------------------------------------------------------------------
struct TBrkMkt{
   TBrkID       BrokerID_;	
   TMarket      Market_;
   TBrkMkt() {};
   TBrkMkt(const TBrkID& a, const TMarket& b): BrokerID_(a), Market_(b) {}
   K_mf(bool) operator == (const TBrkMkt& a) const{
      return BrokerID_ == a.BrokerID_ && Market_ == a.Market_;}
   K_mf(bool) operator < (const TBrkMkt& a) const
      { return BrokerID_==a.BrokerID_ ? a.Market_<Market_ : a.BrokerID_<BrokerID_; }
   K_mf(std::string) as_string()const {return std::string(BrokerID_.as_string()+"-"+Market_.as_string());}
   K_mf(void)        assign(const char*){};
};
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   EFTFlowID;
   enum EFTFlowID_t
#else
   enum EFTFlowID
#endif
{
   fft_P_ing,   //處理中
   fft_Q_ing,   //排隊中
   fft_S_ing,   //傳送中
   fft_R_ing,   //接收中
   fft_W_ing,   //等候回報, 若是券商申報資料, 則有可能需要等候交易所回傳申報回覆資料才算OK
   fft_Routed,  //己繞送至其他台主機處理, 
   fft_Finish,  //結束
   fft_Timeout, //作業逾時, 
   fft_Abort,   //委託中斷! (發生異常)
} K_EnumPacked;
K_class TFTFlowID : public Kstl::EnumT<EFTFlowID>
{
   typedef Kstl::EnumT<EFTFlowID>   inherited;
public:
   inline K_ctor TFTFlowID(EFTFlowID a = fft_Finish) : inherited(a) {}
   K_mf(std::string)  as_string  () const;
   K_mf(void)         assign     (const std::string&);
};
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   EFTMode;
   enum EFTMode_t
#else
   enum EFTMode
#endif
{
   ftm_Daily,   // 交易所每日主動下載
   ftm_Report,  // 券商申報(含回覆)
   ftm_ByReq,   // 券商要求下載
   ftm_Schedule,// 排程下載
   ftm_BrkMsg,  // 券商送出訊息
   ftm_TSEMsg,  // 交易所送出訊息
   ftm_SysReq,  // 系統要求
   ftm_End,     // 定義結束
} K_EnumPacked;
K_class TEFTMode : public Kstl::EnumT<EFTMode>
{
   typedef Kstl::EnumT<EFTMode>   inherited;
public:
   inline K_ctor      TEFTMode(EFTMode a = ftm_Daily)  : inherited(a) {}
   K_mf(std::string)  as_string  () const;
   K_mf(void)         assign     (const std::string&);
};
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   EFTType;
   enum EFTType_t
#else
   enum EFTType
#endif
{
   ftt_File,    // 檔案
   ftt_Msg,     // 訊息
   ftt_End,     // 定義結束
} K_EnumPacked;
K_class TEFTType : public Kstl::EnumT<EFTType>
{
   typedef Kstl::EnumT<EFTType>   inherited;
public:
   inline K_ctor      TEFTType(EFTType a = ftt_File): inherited(a) {}
   K_mf(std::string)  as_string  () const;
   K_mf(void)         assign     (const std::string&);
};
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   eEnvMode;
   enum eEnvMode_t
#else
   enum eEnvMode
#endif
{
   evm_2T,    //兩層式要求
   evm_3T,    //三層式要求
   evm_End,   //定義結束
} K_EnumPacked;
K_class TEnvMode : public Kstl::EnumT<eEnvMode>
{
   typedef Kstl::EnumT<eEnvMode>   inherited;
public:
   inline K_ctor      TEnvMode(EFTType a = evm_2T): inherited(a) {};
};

//---------------------------------------------------------------------------
struct TFTJobBase
{
    TSeqNo  SeqNo_;     // seqno 作業序號
    TBrkID  SourceID_;  // source id
    TBrkID  DestID_;    // dest id
    TMarket Market_;    // TSE/OTC
};
//---------------------------------------------------------------------------
struct TFTNewJob : public TFTJobBase
{
    TFileCode   FileCode_;
    TEFTType    Type_;      // File or Message
    TEFTMode    Mode_;      // Mode
    TSeqNo      SrcSeqNo_;  // 傳送要求來源的key
    TMsgData    SrcData_;   // upload msg or filename
    TMsgData    DstData_;   // download msg or filename
    TSeqNo      RecLen_;    // Record len
    TKTime      CreateTime_;// 建立的時間
    TEnvMode    EnvMode_;   // 此上傳要求來自二層還是三層架構20090212 for ubs
    bool        Done3T_;    // 三層式下載作業(TSE->Broker)檔案複製己完成
    K_ctor TFTNewJob():Done3T_(false){};
};
//---------------------------------------------------------------------------
typedef Kstl::rr_inn_file<Kway::uint64, Kway::dword> TCenterInn;
//---------------------------------------------------------------------------
struct TFTJob : public TFTNewJob
{
    typedef TFTNewJob inherited;
    TFTFlowID   Flow_;      // 狀態
    TKTime      SendTime_;  // 傳送的時間
    TStatusCode LastCode_;  // 最後交易所的狀態碼
    TKTime      ReplyTime_; // 回覆的時間
    TLastMsg    LastMsg_;   // 最後訊息

    TCenterInn::rr_key mutable RoomKey_;
    K_ctor TFTJob():inherited()        { memset(this, 0, sizeof(TFTJob)); }
    K_ctor TFTJob(const TFTNewJob& job){ memcpy(this, &job, sizeof(TFTJob)); }
};
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif
