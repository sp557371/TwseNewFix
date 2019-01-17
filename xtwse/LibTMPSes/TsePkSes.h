//---------------------------------------------------------------------------
#ifndef TseTcpSesH
#define TseTcpSesH
#include "bxSes.h"
#include "bxDef.h"
#include "MesTSEClient.h"
#include "bxOrder.h"
#include "bxLink.h"
#include "bxRpt.h"
#ifdef __BORLANDC__
#include "winsock.h"
#else
#include <netinet/in.h>
#endif
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw::Stk;
using namespace Kway::Tw::bx;
//---------------------------------------------------------------------------
class TTsePkSes : public TbxSes
{
   typedef TbxSes inherited;
//   typedef Kway::TMesBase TMesBase;
   typedef TTsePkSes      this_type;
private:
   Kway::byte              buf_[4096];      // 如果一個封包會大於4k時,就要重新考慮了
   TbxRec*                 Pkt_;
   Kway::word              Pos_;
   K_mf(void)              OnMesRecvReady   (TMesBase* M_, size_t size)//當有資料
   {
      int PacketSz = 0;
      int BufSz = size;
      while(1){
         int rdsz = M_->Recv(buf_ + Pos_, sizeof(TTsePK));
         if (rdsz>0) {
            Pos_  += rdsz;
         } else
            break;
         while (Pos_ >= CtlHeadSize) {
            PacketSz = GetPacketSize(Pkt_);
            if (PacketSz > 0 && Pos_ >= PacketSz) {
               TbxRec pk;
               memcpy(&pk, buf_, PacketSz);
               OnPktRecv(pk);
               Pos_ -= PacketSz;
               memcpy(buf_, buf_+ PacketSz, Pos_);
            }
         }
      } // while(1){
   }
protected:
   virtual K_mf(void)      OnMesLinkReady   (TMesBase*)
   {
      Pos_ = 0;
   }
   virtual K_mf(void)      OnMesLinkBroken  (TMesBase*, const std::string&)
   {
   }
   virtual K_mf(void)      ShowState(EState){};

   virtual K_mf(int)       GetPacketSize    (TbxRec*);
   virtual K_mf(void)      OnPktRecv        (TbxRec&)= 0;
           K_mf(bool)      Send             (TbxRec& pkt)
           {
              if(GetMes() == 0  ||  !GetMes()->IsLinkReady())
                 return false;
              int sz = GetPacketSize(&pkt);
              if (sz){
                 return (GetMes()->Send(&pkt, sz) == sz);
              }
              return false;
           }
public:
           K_ctor TTsePkSes(TbxSesMgr* mgr):inherited(mgr)
           {
              Pos_ = 0;
              Pkt_ = reinterpret_cast<TbxRec*>(buf_);
           }
           K_dtor ~TTsePkSes(){};
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
//---------------------------------------------------------------------------
#endif
