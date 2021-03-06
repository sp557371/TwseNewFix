//---------------------------------------------------------------------------
#ifndef PacketSesH
#define PacketSesH
//---------------------------------------------------------------------------
#include "Ses.h"
//---------------------------------------------------------------------------
template<class TPacket, int HeadSize = 0, int PktSz = 1>
class TPacketSes : public Kway::TSesBase
{
public:
   typedef Kway::TSesBase inherited;
   typedef Kway::TMesBase TMesBase;

private:
   Kway::byte              buf_[(PktSz>sizeof(TPacket) ? PktSz : sizeof(TPacket)) * 2];
   TPacket                *Pkt_;
   Kway::word              Pos_;

   K_mf(void)              OnMesRecvReady(TMesBase* M_, size_t size)//當有資料
   {
      int PacketSz = 0;
      int rdsz;
      do {
         rdsz = M_->Recv(buf_ + Pos_, sizeof(TPacket));
         if (rdsz>0) {
            Pos_  += rdsz;
         } else
            break;
         while (Pos_ >= HeadSize) {
// --> 20140801 modify by k288 for 接收異常封包資料, 解出的資料長度為 0 時,會變成無窮迴圈
            if((PacketSz = GetPacketSize(Pkt_, false)) <= 0 || PacketSz > sizeof(TPacket)){
            //if((PacketSz = GetPacketSize(Pkt_, false)) < 0){
// <-- 20140801 modify by k288 for 接收異常封包資料, 解出的資料長度為 0 時,會變成無窮迴圈
               Pos_ = 0;
               break;
            } 
            if (Pos_ >= PacketSz) {
               TPacket pk;
               memcpy(&pk, buf_, PacketSz);
               OnPktRecv(pk);
               Pos_ -= PacketSz;
               memcpy(buf_, buf_+ PacketSz, Pos_);
               buf_[Pos_] = 0;
            } else{
#ifdef __sun
               GetPacketSize(Pkt_, false); //要將header轉置回來
#endif
               break;
            }
         }
      }   while(1);// (BufSz > 0);// while (BufSz)
   }

protected:
   virtual K_mf(void) OnMesLinkReady(TMesBase*)
   {
      Pos_ = 0;
      memset(buf_, 0, sizeof(buf_));
   }
   virtual K_mf(void)      OnMesLinkBroken  (TMesBase*, const std::string&) = 0;
   virtual K_mf(void)      OnMesLinkFail    (TMesBase*, const std::string&) = 0;
   virtual K_mf(int)       GetPacketSize    (TPacket* ,bool IsSend=false) {if (HeadSize == 0) return PktSz;}
   virtual K_mf(void)      OnPktRecv        (TPacket&) = 0;
           K_mf(bool)      Send             (TPacket& pkt)
           {
              if(GetMes() == 0  ||  !GetMes()->IsLinkReady())
                 return false;
              int sz = GetPacketSize(&pkt ,true);
              if (sz)
                 return GetMes()->Send(&pkt, sz) == sz;
              return false;
           }
public:
           K_ctor TPacketSes()
           {
              Pos_ = 0;
              Pkt_ = reinterpret_cast<TPacket*>(buf_);
           }
           K_dtor ~TPacketSes(){};
};
//---------------------------------------------------------------------------
class TLineSes : public Kway::TSesBase
{
public:
   typedef Kway::TSesBase inherited;
   typedef Kway::TMesBase TMesBase;
private:
   char                    buf_[4096*2];
   Kway::word              Pos_;
   Kway::word              MaxLen_;
   Kway::word              SepLen_;
   char                    Seperator_;
   char*                   S;
   bool                    Skip_;
   K_mf(void)              OnMesRecvReady(TMesBase* M_, size_t size)//當有資料
   {
//      do{
         if(Pos_ >= MaxLen_){            // 950331 防止大量的垃圾資料影響運作
            Pos_ = 0;
            memset(buf_, 0, sizeof(buf_));
         }
// --> 20140801 modify by k288 for 接收異常封包資料造成 core dump 的情況
         int rdsz = M_->Recv(buf_ + Pos_, 4096*2-Pos_ - 1);		// -1 是為了 buf_[Pos_] = 0;
         //int rdsz = M_->Recv(buf_ + Pos_, 4096*2-Pos_);
// <-- 20140801 modify by k288 for 接收異常封包資料造成 core dump 的情況
         if (rdsz>0) {
            Pos_  += rdsz;
            buf_[Pos_] = 0;
         } else
            return;
         while ((S = strchr(buf_, Seperator_))) {
            int len = S - buf_;
            *S      = 0;
            OnPktRecv(buf_, len);
            Pos_   -= len +SepLen_;
            if(Pos_ > 0){
               memcpy(buf_, buf_+ len + SepLen_, Pos_);
               buf_[Pos_] = 0;
            }else{
               Pos_ = 0; 
               memset(buf_, 0, sizeof(buf_));
               return;
            }
         }
//      }  while (1);
   }

protected:
   virtual K_mf(void)      OnMesLinkReady   (TMesBase*)
   {
      Pos_ = 0;
      memset(buf_, 0, sizeof(buf_));
      Skip_ = false;
   }
   virtual K_mf(void)      OnMesLinkBroken  (TMesBase*, const std::string&) = 0;
   virtual K_mf(void)      OnMesLinkFail    (TMesBase*, const std::string&) = 0;
   virtual K_mf(void)      OnPktRecv        (void*, int) = 0;
           K_mf(bool)      Send             (void* buf, int sz)
           {
              if(GetMes() == 0  ||  !GetMes()->IsLinkReady())
                 return false;
              if (sz)
                 return GetMes()->Send(buf, sz) == sz;
              return false;
           }
public:
           K_ctor TLineSes(int MaxLen, char* Sep)
           {
              MaxLen_ = MaxLen;
              Seperator_ = *Sep;
              SepLen_ = std::string(Sep).length();
              Pos_ = 0;
           }
           K_dtor ~TLineSes(){};
};
//---------------------------------------------------------------------------
template<class TPacket, int num = 1>
class TPktArraySes : public Kway::TSesBase
{
public:
   typedef Kway::TSesBase inherited;
   typedef Kway::TMesBase TMesBase;

protected:
   TPacket                 Pkt_[num+1];
private:
   char*                    buf_;
   int                     PacketSz;
   Kway::word              Pos_;

   K_mf(void)              OnMesRecvReady(TMesBase* M_, size_t size)//當有資料
   {
      int rdsz;
      do {
         rdsz = M_->Recv(buf_ + Pos_, PacketSz*num);
         if (rdsz>0) {
            Pos_  += rdsz;
         } else
            break;
         if(Pos_ >= PacketSz) {
            Kway::word count = Pos_ / PacketSz;
            Kway::word skip  = Pos_ % PacketSz;
            OnPktRecv(count);
            if(skip > 0){
               memcpy(buf_, buf_+ count*PacketSz, skip);
               Pos_ = skip;
            }else
               Pos_ = 0;
         }
      }   while(1);// (BufSz > 0);// while (BufSz)
   }

protected:
   virtual K_mf(void) OnMesLinkReady(TMesBase*)
   {
      Pos_ = 0;
   }
   virtual K_mf(void)      OnMesLinkBroken  (TMesBase*, const std::string&) = 0;
   virtual K_mf(void)      OnMesLinkFail    (TMesBase*, const std::string&) = 0;
   virtual K_mf(void)      OnPktRecv        (Kway::word) = 0;
           K_mf(bool)      Send             (TPacket& pkt)
           {
              if(GetMes() == 0  ||  !GetMes()->IsLinkReady())
                 return false;
              return GetMes()->Send(&pkt, sizeof(TPacket)) == sizeof(TPacket);
           }
public:
           K_ctor TPktArraySes()
           {
              Pos_ = 0;
              PacketSz = sizeof (TPacket);
              buf_ = (char*)Pkt_;
           }
           K_dtor ~TPktArraySes(){};
};
//---------------------------------------------------------------------------
#endif
