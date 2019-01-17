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
BCB: �Y�ϥ� console mode, �аO�o�[�W -b- �ﶵ.
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
   msf_MessageAck,// �T���T�{�^��.
   msf_RecoverEnd,// �T���^�ɵ���.
   msf_SupportEncryption,//fonwin: 20090604: Server�䴩�T���[�K
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
   /** for MS Server: ���U/�������U Pub/Sub �����G, �Y���UPub���\, �h�����P�ɳ]�wmsgID
   */
   struct TRegAck {  //���U/���� �n�D.
      TReqAckID   CliReqID_;///< ��Cli�n�D�ɳ]�w,Svr�ݤ����ܦ���.(Svr�ݰe�T����,�ϥΦ�ID�i���O����Destin���T��).
      TReqAckID   SvrAckID_;///< Svr�^�е��U�����G,Cli[�������U]�B[�o��T��] �ɨϥΦ�ID.
      byte        DestType_;///< dt_Topic or dt_Queue
      dword       ResultCode_;///< �Ѧ� MSSesCli.h: enum EMSCliResult;
   };
   struct TRegAckPubReady : public TRegAck {
      Kway::Ms::TMessageID::TOnlineID  MessageID_OnlineID_;///< �T���Ǹ��q�o��}�l�s��. Producer���U���\(ResultCode_==mcr_PubReady),�~������.
      Kway::Ms::TMessageID::TSeqNo     MessageID_SeqNo_;   ///< �T���Ǹ��q�o��}�l�s��. Producer���U���\(ResultCode_==mcr_PubReady),�~������.
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
   mst_PubDirect,///< �|�����U����Publisher, �����o��T��.
   mst_MessageRejectAndCliID,//�ڵ��� Message �� CliID
   mst_MessageAck,
   mst_MessageRejectBySvrID, //20071023: �ڵ����w�� Message, �����O���q�\.
   mst_SupportEncryption,    //fonwin: 20090604: Client�䴩�T���[�K(�^�� msf_SupportEncryption)
   mst_BrokenAutoPub,        //fonwin: 20090609: ��Client�_�u��,�۰ʵo��T��
   mst_BrokenAutoPubDirect,  //fonwin: 20090609: ��Client�_�u��,�۰ʵo��T��
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
   /** ���U/�������U.
       reqName = TopicName or QueueName, �C���u��n�D�@��Topic/Queue.
       �p�G reqType == reqt_RegConsumer, �h reqName �i�H�]�t '\t' + durableName
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
   struct TRegReq {  //���U/���� �n�D.
      byte           ReqType_; ///< Reg:Producer/Consumer �� Unreg.
      byte           DestType_;///< Topic or Queue.
      union {
         TReqAckID   CliReqID_;///< ���U�n�D: Svr�ݦb:[�^�е��U���G]�B[�T���q��],�ɷ|�ϥΦ�ID.
         TReqAckID   SvrAckID_;///< �����n�D: �a�JSvr�ݦ^�е��U�n�D�ɪ�AckID.
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

