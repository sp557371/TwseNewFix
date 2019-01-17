//---------------------------------------------------------------------------
#ifndef bxLinkWorkerH
#define bxLinkWorkerH
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
//連線子系統
K_class TWkr_L1010 : public TWorker             //開機通知作業(L010,L020)
{
   typedef TWorker      inherited;
   typedef TWkr_L1010   this_type;
public:
   K_mf(TCheckResult)  OnPkCheck  (TbxSes& Ses,const THead& pk, TSize recvSize,TSize* PacketSize);
   K_mf(void)          OnPkRecv   (TbxSes& Ses,const THead& pk, TReqPtr& ReqPtr);
   K_mf(bool)          LinkReq    (TbxSes& Ses,TPacketCode code,TApCode apcode);
};
//---------------------------------------------------------------------------
K_class TWkr_L1020 : public TWorker             //登入作業(L030,L040,L050,L060)
{
   typedef TWorker      inherited;
   typedef TWkr_L1020   this_type;
public:
   K_mf(TCheckResult)  OnPkCheck  (TbxSes& Ses,const THead& pk, TSize recvSize,TSize* PacketSize);
   K_mf(void)          OnPkRecv   (TbxSes& Ses,const THead& pk, TReqPtr& ReqPtr);
   K_mf(bool)          LinkReq    (TbxSes& Ses,TPacketCode code,TApCode apcode);
};
//---------------------------------------------------------------------------
K_class TWkr_L1030 : public TWorker             //離線作業(L070,l080)
{
   typedef TWorker      inherited;
   typedef TWkr_L1030   this_type;
public:
   K_mf(TCheckResult)  OnPkCheck  (TbxSes& Ses,const THead& pk, TSize recvSize,TSize* PacketSize);
   K_mf(void)          OnPkRecv   (TbxSes& Ses,const THead& pk, TReqPtr& ReqPtr);
   K_mf(bool)          LinkReq    (TbxSes& Ses,TPacketCode code,TApCode apcode);
};

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

