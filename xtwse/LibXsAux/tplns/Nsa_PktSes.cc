
/////////////////////////////////////////////////////////////////////////////
// class TPacketSes impl
/////////////////////////////////////////////////////////////////////////////
#define Nsa_TPacketSes_Impl(return_type)                       \
   template <class Packet, int HeadSize, class SesBase>        \
   K_mf(return_type) TPacketSes<Packet, HeadSize, SesBase>::   \
//---------------------------------------------------------------------------

template <class Packet, int HeadSize, class SesBase>
K_ctor TPacketSes<Packet, HeadSize, SesBase>::TPacketSes ()
{
   CurrPos_ = 0;
   Packet_  = reinterpret_cast<TPacket*>(Buffer_);
}
//---------------------------------------------------------------------------

template <class Packet, int HeadSize, class SesBase>
K_dtor TPacketSes<Packet, HeadSize, SesBase>::~TPacketSes ()
{
}
//---------------------------------------------------------------------------

Nsa_TPacketSes_Impl(void) OnMesLinkReady (TMesBase* MesBase)
{
   inherited::OnMesLinkReady(MesBase);

   CurrPos_ = 0;
}
//---------------------------------------------------------------------------

Nsa_TPacketSes_Impl(void) OnMesRecvReady (TMesBase* Mes, size_t size)
{
   int PacketSz = 0;
   int rdsz;
   
   do {
      rdsz = Mes->Recv(Buffer_ + CurrPos_, sizeof(TPacket));
      if (rdsz > 0) {
         OnMesDataIn(Buffer_+CurrPos_, (size_t)rdsz);
         CurrPos_ += rdsz;
      }
      else {
         break;
      }

      while (CurrPos_ >= HeadSize) {
         PacketSz = GetPacketSize(Packet_, false);
         if (CurrPos_ >= PacketSz) {
            TPacket Pkt;

            //TUtilComm::BugOut("1: CurrPos[%d], PacketSz[%d]", CurrPos_, PacketSz);

            memcpy(&Pkt, Buffer_, PacketSz);
            OnPktRecv(Pkt, PacketSz);

            CurrPos_ -= PacketSz;

            //TUtilComm::BugOut("2: CurrPos[%d], PacketSz[%d]", CurrPos_, PacketSz);

            memcpy(Buffer_, Buffer_+ PacketSz, CurrPos_);
         }
         else {
            #ifdef __sun
            GetPacketSize(Packet_, false); //要將header轉置回來
            #endif
            break;
         }
      }
   } while (1);
}
//---------------------------------------------------------------------------

Nsa_TPacketSes_Impl(bool) SendPkt (TPacket& pkt)
{
   int sz = GetPacketSize(&pkt ,true);

   if (sz) {
      OnSendPkt(pkt, sz);
      return (inherited::Send(&pkt, sz) == sz);
   }

   return false;
}


/////////////////////////////////////////////////////////////////////////////
// class TLineSes impl
/////////////////////////////////////////////////////////////////////////////
#define Nsa_TLineSes_Impl(return_type)                                     \
   template <class SesT>                                                   \
   K_mf(return_type) TLineSes<SesT>::                                      \
//---------------------------------------------------------------------------

template <class SesT>
K_ctor TLineSes<SesT>::TLineSes (char splitChar)
   : inherited ()
   , SplitChar_(splitChar)
   , CurrPos_  (0)
{
}
//---------------------------------------------------------------------------

Nsa_TLineSes_Impl(void) OnMesLinkReady (TMesBase* mes)
{
   inherited::OnMesLinkReady(mes);
   CurrPos_ = 0;
   ScanPos_ = 0;
}
//---------------------------------------------------------------------------

Nsa_TLineSes_Impl(void) OnMesRecvReady (TMesBase* mes, size_t size)
{
   int    RdSz;
   char*  mem;
   size_t PktSz;

   while (true) {
      RdSz = mes->Recv(RecvBuf_+CurrPos_, (Buf_Size-CurrPos_));
      if (RdSz <= 0) {
         break;
      }
      OnMesDataIn(RecvBuf_+CurrPos_, RdSz);
         
      CurrPos_ += RdSz;

      do {
         mem = (char*)memchr(RecvBuf_+ScanPos_, (int)SplitChar_, (CurrPos_ - ScanPos_));
         if (mem) {

            PktSz = (mem - RecvBuf_ + 1);

            OnPktRecv(RecvBuf_, PktSz);
            // 將資料往前搬
            CurrPos_ -= PktSz;
            memcpy(RecvBuf_, RecvBuf_+PktSz, CurrPos_);
            //memset(RecvBuf_+CurrPos_, 0, 2);
            ScanPos_ = 0;
         }
         else {
            ScanPos_ = CurrPos_;
         }
      } while (mem && CurrPos_ > 0);
   }
}


/////////////////////////////////////////////////////////////////////////////
// class TSplitSes impl
/////////////////////////////////////////////////////////////////////////////
#define Nsa_TSplitSes_Impl(return_type)                                    \
   template <class SesT>                                                   \
   K_mf(return_type) TSplitSes<SesT>::                                     \
//---------------------------------------------------------------------------

template <class SesT>
K_ctor TSplitSes<SesT>::TSplitSes (const char* splitChar, size_t splitNum)
   : inherited()
   , SplitNum_(splitNum)
{
   memcpy(SplitChar_, splitChar, splitNum);
}
//---------------------------------------------------------------------------

Nsa_TSplitSes_Impl(void) OnMesLinkReady (TMesBase* mes)
{
   inherited::OnMesLinkReady(mes);
   CurrPos_ = 0;
   ScanPos_ = 0;
}
//---------------------------------------------------------------------------

Nsa_TSplitSes_Impl(void) OnMesRecvReady (TMesBase* mes, size_t size)
{
   int    RdSz;
   char*  mem;
   size_t PktSz;
   size_t BufSz;

   while (true) {
      BufSz = sizeof(RecvBuf_) - CurrPos_;
      RdSz = mes->Recv(RecvBuf_ + CurrPos_, BufSz);
      if (RdSz <= 0) {
         break;
      }
      OnMesDataIn(RecvBuf_+CurrPos_, RdSz);

      CurrPos_ += RdSz;

      do {
         //mem = (char*)memchr(RecvBuf_ + ScanPos_, (int)SplitChar_[0], (CurrPos_ - ScanPos_ - 1));
         mem = (char*)memchr(RecvBuf_ + ScanPos_, (int)SplitChar_[0], (CurrPos_ - ScanPos_));
         if (mem) {
            // 要判斷 mem ~ CurrPos 長度是否小於 SplitNum_
            if (RecvBuf_ + CurrPos_ - mem >= (int)SplitNum_) {
               size_t i;

               for (i=1; i<SplitNum_; i++) {
                  if (mem[i] != SplitChar_[i])
                     break;
               }

               if (i == SplitNum_) {
                  // 找到完全吻合的
                  PktSz = (mem + i) - RecvBuf_;
                  OnPktRecv(RecvBuf_, PktSz);
                  CurrPos_ -= PktSz;

                  // 將資料往前搬
                  memcpy(RecvBuf_, RecvBuf_+PktSz, CurrPos_);
                  ScanPos_ = 0;
               }
               else {
                  ScanPos_ = (mem - RecvBuf_ + 1);
               }
            }
            else {
               // 這邊代表有掃到第一個字元,但是因為長度不夠而無法處理
               // 所以 ScanPos 要停留在第一個字元,讓下次有資料進來時
               // 重新確認這個資料是否有效
               //ScanPos_ = CurrPos_;
               ScanPos_ = (mem - RecvBuf_);

               // 資料已經掃到最後了，可以離開了
               break;
            }
         }
         else {
            ScanPos_ = CurrPos_;
         }
      } while (mem && CurrPos_ > 0);
   }
}
//---------------------------------------------------------------------------
