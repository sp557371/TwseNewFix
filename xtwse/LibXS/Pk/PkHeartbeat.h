//---------------------------------------------------------------------------
#ifndef PkHeartbeatH
#define PkHeartbeatH
//---------------------------------------------------------------------------
#include "SesPk.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Pk {
/////////////////////////////////////////////////////////////////////////////
//●Heartbeat:由 Client端發起(當沒有任何資料傳送時,每隔n秒送一次)
//               Server端收到後, 立即回覆 Heartbeat(直接將收到的訊息回覆)
//---------------------------------------------------------------------------
static const TMsgCode   mcHeartbeat           = 0x0001;  // Size = HeadSize
static const int        HeartbeatIntervalSecs = 30;      // 每隔 n秒,溝通一次Heartbeat
                                                         // 若2分鐘內沒收到任何封包,則視同斷線!
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
LIBNS_class THeartbeat : public THead
{
public:
   inline K_ctor  THeartbeat  ()
            { SetMsgCode(mcHeartbeat);
              SetPkSize(sizeof(*this)); }

   inline K_mf(TWorker::TCheckResult)  PkCheck  (TSize recvSize) const
            { return(GetPkSize()==sizeof(*this) ? TWorker::cpr_HeadOK : TWorker::cpr_Error); }
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

