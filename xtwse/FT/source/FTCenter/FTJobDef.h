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
   fft_P_ing,   //�B�z��
   fft_Q_ing,   //�ƶ���
   fft_S_ing,   //�ǰe��
   fft_R_ing,   //������
   fft_W_ing,   //���Ԧ^��, �Y�O��ӥӳ����, �h���i��ݭn���ԥ���Ҧ^�ǥӳ��^�и�Ƥ~��OK
   fft_Routed,  //�v¶�e�ܨ�L�x�D���B�z, 
   fft_Finish,  //����
   fft_Timeout, //�@�~�O��, 
   fft_Abort,   //�e�U���_! (�o�Ͳ��`)
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
   ftm_Daily,   // ����ҨC��D�ʤU��
   ftm_Report,  // ��ӥӳ�(�t�^��)
   ftm_ByReq,   // ��ӭn�D�U��
   ftm_Schedule,// �Ƶ{�U��
   ftm_BrkMsg,  // ��Ӱe�X�T��
   ftm_TSEMsg,  // ����Ұe�X�T��
   ftm_SysReq,  // �t�έn�D
   ftm_End,     // �w�q����
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
   ftt_File,    // �ɮ�
   ftt_Msg,     // �T��
   ftt_End,     // �w�q����
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
   evm_2T,    //��h���n�D
   evm_3T,    //�T�h���n�D
   evm_End,   //�w�q����
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
    TSeqNo  SeqNo_;     // seqno �@�~�Ǹ�
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
    TSeqNo      SrcSeqNo_;  // �ǰe�n�D�ӷ���key
    TMsgData    SrcData_;   // upload msg or filename
    TMsgData    DstData_;   // download msg or filename
    TSeqNo      RecLen_;    // Record len
    TKTime      CreateTime_;// �إߪ��ɶ�
    TEnvMode    EnvMode_;   // ���W�ǭn�D�ӦۤG�h�٬O�T�h�[�c20090212 for ubs
    bool        Done3T_;    // �T�h���U���@�~(TSE->Broker)�ɮ׽ƻs�v����
    K_ctor TFTNewJob():Done3T_(false){};
};
//---------------------------------------------------------------------------
typedef Kstl::rr_inn_file<Kway::uint64, Kway::dword> TCenterInn;
//---------------------------------------------------------------------------
struct TFTJob : public TFTNewJob
{
    typedef TFTNewJob inherited;
    TFTFlowID   Flow_;      // ���A
    TKTime      SendTime_;  // �ǰe���ɶ�
    TStatusCode LastCode_;  // �̫����Ҫ����A�X
    TKTime      ReplyTime_; // �^�Ъ��ɶ�
    TLastMsg    LastMsg_;   // �̫�T��

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
