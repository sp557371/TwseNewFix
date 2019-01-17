//---------------------------------------------------------------------------
#ifndef PkMSH
#define PkMSH
//---------------------------------------------------------------------------
#include "SesPk.h"
#include "PkData.h"
#include "MSTypes.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Pk {
/**
BCB: 若使用 console mode, 請記得加上 -b- 選項.
*/
//---------------------------------------------------------------------------
using Kway::Ms::TReqAckID;
using Kway::Ms::EDestinationType;
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
static const TMsgCode   mcMS = 0x060;
/////////////////////////////////////////////////////////////////////////////
#ifdef K_EnumPacked_Bug
   typedef byte   TPkFrMSFunc;
   enum TPkFrMSFunc_t
#else
   enum TPkFrMSFunc
#endif
{
   msf_RegAck,    // Req(TopicSubscribe/TopicPublish/QueueSend/QueueReceive) Result.
   msf_Message,   // Topic/Queue Message.
   msf_MessageAck,// 訊息確認回覆.
   msf_RecoverEnd,// 訊息回補結束.
   msf_SupportEncryption,//fonwin: 20090604: Server支援訊息加密
} K_EnumPacked;
//---------------------------------------------------------------------------
struct TPkFrMSHead : public THead
{
   TSize    DatLen_;
};
//---------------------------------------------------------------------------
/** Package from MS Server.
*/
class TPkFrMS : public TPkFrMSHead
{
   typedef TPkFrMS   this_type;

   enum {   DatSize = sizeof(TPkRec)-sizeof(TPkFrMSHead)   };
   char     Dat_[DatSize];

   inline K_mf(TSize) GetExpectSize () const    { return static_cast<TSize>(GetMinSize() + DatLen_); }

public:
   /** for MS Server: 註冊/取消註冊 Pub/Sub 的結果, 若註冊Pub成功, 則必須同時設定msgID
   */
   struct TRegAck {  //註冊/取消 要求.
      TReqAckID   CliReqID_;///< 由Cli要求時設定,Svr端不改變此值.(Svr端送訊息時,使用此ID告知是哪個Destin的訊息).
      TReqAckID   SvrAckID_;///< Svr回覆註冊的結果,Cli[取消註冊]、[發行訊息] 時使用此ID.
      byte        DestType_;///< dt_Topic or dt_Queue
      dword       ResultCode_;///< 參考 MSSesCli.h: enum EMSCliResult;
   };
   struct TRegAckPubReady : public TRegAck {
      Kway::Ms::TMessageID::TOnlineID  MessageID_OnlineID_;///< 訊息序號從這兒開始編號. Producer註冊成功(ResultCode_==mcr_PubReady),才有此值.
      Kway::Ms::TMessageID::TSeqNo     MessageID_SeqNo_;   ///< 訊息序號從這兒開始編號. Producer註冊成功(ResultCode_==mcr_PubReady),才有此值.
      Kway::Ms::TMaxMsgLmt             MaxMsgLmt_;
      K_ctor TRegAckPubReady() : MessageID_OnlineID_(), MessageID_SeqNo_(), MaxMsgLmt_(0) { }
   };
   inline K_ctor TPkFrMS (const TRegAck& ack, const Kway::Ms::TMessageID* msgID, Kway::Ms::TMaxMsgLmt maxMsgLmt)
      {  SetMsgCode(TMsgCode(mcMS + msf_RegAck));
         *reinterpret_cast<TRegAck*>(Dat_) = ack;
         if(msgID  &&  ack.ResultCode_==Kway::Ms::mcr_PubReady) {
            reinterpret_cast<TRegAckPubReady*>(Dat_)->MessageID_OnlineID_ = msgID->GetOnlineID();
            reinterpret_cast<TRegAckPubReady*>(Dat_)->MessageID_SeqNo_    = msgID->GetSeqNo();
            reinterpret_cast<TRegAckPubReady*>(Dat_)->MaxMsgLmt_          = maxMsgLmt;
            SetDatLen(sizeof(TRegAckPubReady));
         } else
            SetDatLen(sizeof(ack));
      }

   /** for MS Server: TPkData<TPkFrMS>.
   */
   inline K_ctor             TPkFrMS       ()                           { }
   inline K_mf(void)         SetDatLen     (TSize sz)                   { DatLen_ = sz;     SetPkSize(GetExpectSize()); }
   inline K_mf(void)         SetDatLen     (TSize sz, THiLoTransRef)    { SetDatLen(sz);    }
   inline static K_mf(TSize) GetMaxDatSize ()                           { return DatSize-1; }

   //----- for Client -----
   static inline K_mf(TSize) GetMinSize    ()                        { return sizeof(this_type) - DatSize; }
   inline K_mf(const void*)  GetDat        () const                  { return Dat_; }
   inline K_mf(void*)        GetDat        ()                        { return Dat_; }
   inline K_mf(TSize)        GetDatLen     (THiLoTransRef r) const   { return HiLoTransT(DatLen_,r); }
   inline K_mf(TSize)        GetExpectSize (THiLoTransRef r) const   { return static_cast<TSize>(GetMinSize() + HiLoTransT(DatLen_,r)); }
};
/////////////////////////////////////////////////////////////////////////////
#ifdef K_EnumPacked_Bug
   typedef byte   TPkToMSFunc;
   enum TPkToMSFunc_t
#else
   enum TPkToMSFunc
#endif
{
   mst_Reg, ///< Register: Add/Del TopicPublish/TopicSubscribe/QueueSend/QueueReceive
   mst_Pub, ///< Publish message.
   mst_PubDirect,///< 尚未註冊成為Publisher, 直接發行訊息.
   mst_MessageRejectAndCliID,//拒絕此 Message 及 CliID
   mst_MessageAck,
   mst_MessageRejectBySvrID, //20071023: 拒絕指定的 Message, 但仍保持訂閱.
   mst_SupportEncryption,    //fonwin: 20090604: Client支援訊息加密(回覆 msf_SupportEncryption)
   mst_BrokenAutoPub,        //fonwin: 20090609: 當Client斷線時,自動發行訊息
   mst_BrokenAutoPubDirect,  //fonwin: 20090609: 當Client斷線時,自動發行訊息
} K_EnumPacked;

#ifdef K_EnumPacked_Bug
   typedef byte   TReqType;
   enum TReqType_t
#else
   enum TReqType
#endif
{
   reqt_RegProducer,
   reqt_RegConsumer,
   reqt_UnReg,          ///< by SvrID
   reqt_UnRegPubByCliID,
   reqt_UnRegSubByCliID,
} K_EnumPacked;
//---------------------------------------------------------------------------
struct TPkToMSHead : public THead
{
   TSize    DatLen_;
};

/** Package to MS Server.
*/
class TPkToMS : public TPkToMSHead
{
   typedef TPkToMS  this_type;
   enum {   DatSize = sizeof(TPkRec)-sizeof(TPkToMSHead)   };
   char     Dat_[DatSize];

public:
   /** 註冊/取消註冊.
       reqName = TopicName or QueueName, 每次只能要求一個Topic/Queue.
       如果 reqType == reqt_RegConsumer, 則 reqName 可以包含 '\t' + durableName
            destType = EDestinationType = dt_Dontcare,dt_Topic,dt_Queue
   */
   static K_mf(void) SendReg ( TSesPk&            ses
                             , TReqAckID          ackID
                             , TReqType           reqt
                             , EDestinationType   destType
                             , const std::string& reqName
                             , THiLoTransRef      r )
   {
      TPkData<TPkToMS> pk(TMsgCode(mcMS + mst_Reg), ses, r);
      Kstl::psave(pk, (Kway::byte)reqt);
      Kstl::psave(pk, (Kway::byte)destType);
      Kstl::psave(pk, ackID);
      Kstl::psave(pk, reqName.c_str(), unsigned(reqName.size()+1));
   }
   struct TRegReq {  //註冊/取消 要求.
      byte           ReqType_; ///< Reg:Producer/Consumer 或 Unreg.
      byte           DestType_;///< Topic or Queue.
      union {
         TReqAckID   CliReqID_;///< 註冊要求: Svr端在:[回覆註冊結果]、[訊息通知],時會使用此ID.
         TReqAckID   SvrAckID_;///< 取消要求: 帶入Svr端回覆註冊要求時的AckID.
      };
      //char         DestName_[];
   };

   /** for MS Client: TPkData<TPkToMS>.
   */
   inline K_ctor TPkToMS ()
            {  }
   inline K_mf(void)  SetDatLen  (TSize sz, THiLoTransRef r)
            {  DatLen_ = sz;
               SetPkSize(GetExpectSize());
               DatLen_ = HiLoTransT(DatLen_,r);
            }
   inline static K_mf(TSize) GetMaxDatSize ()
            { return DatSize-1;
            }

   //----- for server -----
   static inline K_mf(TSize) GetMinSize    ()         { return sizeof(this_type) - DatSize; }
   inline K_mf(const void*)  GetDat        () const   { return Dat_;    }
   inline K_mf(void*)        GetDat        ()         { return Dat_;    }
   inline K_mf(TSize)        GetDatLen     () const   { return DatLen_; }
   inline K_mf(TSize)        GetExpectSize () const   { return static_cast<TSize>(GetMinSize() + DatLen_); }
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
} // namespace Pk
} // namespace Kway
//---------------------------------------------------------------------------
#endif

