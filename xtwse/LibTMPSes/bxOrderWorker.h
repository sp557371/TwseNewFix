//---------------------------------------------------------------------------
#ifndef bxOrderWorkerH
#define bxOrderWorkerH
//---------------------------------------------------------------------------
#include "bxDef.h"
#include "bxSes.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace bx {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
   #pragma pack(push,1)
#else
   #pragma pack(1)
#endif
//一般交易子系統
//---------------------------------------------------------------------------
typedef Kway::Tw::Stk::TOrdCenter::TOrdHnd      TOrdHnd;
typedef Kway::Tw::Stk::TOrdFlow::TReqChgQtyBase TReqChgQtyBase;
//---------------------------------------------------------------------------
K_class TWkr_T3000 : public TWorker             //連線訊息(T040,T050,T060)
{
   typedef TWorker      inherited;
   typedef TWkr_T3000   this_type;
public:
   K_mf(TCheckResult)  OnPkCheck  (TbxSes& Ses,const THead& pk, TSize recvSize,TSize* PacketSize);
   K_mf(void)          OnPkRecv   (TbxSes& Ses,const THead& pk, TReqPtr& ReqPtr);
   K_mf(bool)          SendReq    (TbxSes& Ses,TPacketCode code,TReqPtr& ReqPtr);
   K_mf(bool)          SendReq   (TbxSes& Ses,TPacketCode code,TSize Num){return false;}
};
//---------------------------------------------------------------------------
K_class TWkr_T4000 : public TWorker             //連線訊息(T040,T050,T060)
{
   typedef TWorker      inherited;
   typedef TWkr_T4000   this_type;
public:
   K_mf(TCheckResult)  OnPkCheck  (TbxSes& Ses,const THead& pk, TSize recvSize,TSize* PacketSize);
   K_mf(void)          OnPkRecv   (TbxSes& Ses,const THead& pk, TReqPtr& ReqPtr);
   K_mf(bool)          SendReq    (TbxSes& Ses,TPacketCode code,TReqPtr& ReqPtr);
   K_mf(bool)          SendReq   (TbxSes& Ses,TPacketCode code,TSize Num){return false;}
};
//---------------------------------------------------------------------------
K_class TWkr_TOrder : public TWorker             //連線訊息(T010,T020,T030)
{
   typedef TWorker                                 inherited;
   typedef TWkr_TOrder                             this_type;
public:
   K_mf(TCheckResult)  OnPkCheck  (TbxSes& Ses,const THead& pk, TSize recvSize,TSize* PacketSize);
   K_mf(void)          OnPkRecv   (TbxSes& Ses,const THead& pk, TReqPtr& ReqPtr);
   K_mf(bool)          SendReq    (TbxSes& Ses,TPacketCode code,TReqPtr& ReqPtr);
   K_mf(bool)          SendReq    (TbxSes& Ses,TPacketCode code,TSize Num){return false;}
};
//---------------------------------------------------------------------------
K_class TWkr_OOrder : public TWorker             //連線訊息(T010,T020,T030)
{
   typedef TWorker                                 inherited;
   typedef TWkr_OOrder                             this_type;
public:
   K_mf(TCheckResult)  OnPkCheck  (TbxSes& Ses,const THead& pk, TSize recvSize,TSize* PacketSize);
   K_mf(void)          OnPkRecv   (TbxSes& Ses,const THead& pk, TReqPtr& ReqPtr);
   K_mf(bool)          SendReq    (TbxSes& Ses,TPacketCode code,TReqPtr& ReqPtr);
   K_mf(bool)          SendReq    (TbxSes& Ses,TPacketCode code,TSize Num){return false;}
};
//---------------------------------------------------------------------------
K_class TWkr_Auction : public TWorker             //(A010~A050)
{
   typedef TWorker                                 inherited;
   typedef TWkr_Auction                            this_type;
public:
   K_mf(TCheckResult)  OnPkCheck  (TbxSes& Ses,const THead& pk, TSize recvSize,TSize* PacketSize);
   K_mf(void)          OnPkRecv   (TbxSes& Ses,const THead& pk, TReqPtr& ReqPtr);
   K_mf(bool)          SendReq    (TbxSes& Ses,TPacketCode code,TReqPtr& ReqPtr);
};
//---------------------------------------------------------------------------
K_class TWkr_Lend : public TWorker             //(V010~V050)
{
   typedef TWorker                                 inherited;
   typedef TWkr_Lend                               this_type;
public:
   K_mf(TCheckResult)  OnPkCheck  (TbxSes& Ses,const THead& pk, TSize recvSize,TSize* PacketSize);
   K_mf(void)          OnPkRecv   (TbxSes& Ses,const THead& pk, TReqPtr& ReqPtr);
   K_mf(bool)          SendReq    (TbxSes& Ses,TPacketCode code,TReqPtr& ReqPtr);
};
//---------------------------------------------------------------------------
K_class TWkr_Tend : public TWorker             //(E010~E050)
{
   typedef TWorker                                 inherited;
   typedef TWkr_Tend                               this_type;
public:
   K_mf(TCheckResult)  OnPkCheck  (TbxSes& Ses,const THead& pk, TSize recvSize,TSize* PacketSize);
   K_mf(void)          OnPkRecv   (TbxSes& Ses,const THead& pk, TReqPtr& ReqPtr);
   K_mf(bool)          SendReq    (TbxSes& Ses,TPacketCode code,TReqPtr& ReqPtr);
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
   #pragma pack(pop)
#else
   #pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
} // namespace bx
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif
