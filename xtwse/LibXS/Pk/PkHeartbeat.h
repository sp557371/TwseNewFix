//---------------------------------------------------------------------------
#ifndef PkHeartbeatH
#define PkHeartbeatH
//---------------------------------------------------------------------------
#include "SesPk.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Pk {
/////////////////////////////////////////////////////////////////////////////
//��Heartbeat:�� Client�ݵo�_(��S�������ƶǰe��,�C�jn��e�@��)
//               Server�ݦ����, �ߧY�^�� Heartbeat(�����N���쪺�T���^��)
//---------------------------------------------------------------------------
static const TMsgCode   mcHeartbeat           = 0x0001;  // Size = HeadSize
static const int        HeartbeatIntervalSecs = 30;      // �C�j n��,���q�@��Heartbeat
                                                         // �Y2�������S�������ʥ],�h���P�_�u!
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

