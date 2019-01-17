//---------------------------------------------------------------------------
#ifndef bxRptWorkerH
#define bxRptWorkerH
//---------------------------------------------------------------------------
#include "bxDef.h"
#include "bxSes.h"
#include "TwStkOrdCenter.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace bx {
/////////////////////////////////////////////////////////////////////////////
/*
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
   #pragma pack(push,1)
#else
   #pragma pack(1)
#endif
*/
//����^���l�t��
K_class TWkr_T5000 : public TWorker             //�s�u�T��(R1,R2,R4,R5)
{
   typedef TWorker      inherited;
   typedef TWkr_T5000   this_type;
public:
   K_mf(TCheckResult)  OnPkCheck  (TbxSes& Ses,const THead& pk, TSize recvSize,TSize* PacketSize);
   K_mf(void)          OnPkRecv   (TbxSes& Ses,const THead& pk, TReqPtr& ReqPtr);
   K_mf(bool)          SendReq    (TbxSes& Ses,TPacketCode code,TReqPtr& ReqPtr){return false;}
   K_mf(bool)          SendReq    (TbxSes& Ses,TPacketCode code,TSize Num);
};
/////////////////////////////////////////////////////////////////////////////
K_class TWkr_T5010 : public TWorker             //����^�����(R3)
{
   typedef TWorker      inherited;
   typedef TWkr_T5010   this_type;
public:
   K_mf(TCheckResult)  OnPkCheck  (TbxSes& Ses,const THead& pk, TSize recvSize,TSize* PacketSize);
   K_mf(void)          OnPkRecv   (TbxSes& Ses,const THead& pk, TReqPtr& ReqPtr);
   //������^����ƥu����V����,����@SendReq
   //K_mf(bool)          SendReq    (TbxSes& Ses,TPacketCode code,TSize Num){return false;}
};
/////////////////////////////////////////////////////////////////////////////
K_class TWkr_T5020 : public TWorker             //����^�������T��(R6)
{
   typedef TWorker      inherited;
   typedef TWkr_T5020   this_type;
public:
   K_mf(TCheckResult)  OnPkCheck  (TbxSes& Ses,const THead& pk, TSize recvSize,TSize* PacketSize);
   K_mf(void)          OnPkRecv   (TbxSes& Ses,const THead& pk, TReqPtr& ReqPtr);
   //������^�������T���u����V����,����@SendReq
   //K_mf(bool)          SendReq    (TbxSes& Ses,TPacketCode code,TSize Num){return false;}
};
//---------------------------------------------------------------------------
/*
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
   #pragma pack(pop)
#else
   #pragma pack()
#endif
*/
/////////////////////////////////////////////////////////////////////////////
} // namespace bx
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif
