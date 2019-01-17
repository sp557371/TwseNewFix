//---------------------------------------------------------------------------
#ifndef SesPacketH
#define SesPacketH
//---------------------------------------------------------------------------
#include "SesLibBase.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
/////////////////////////////////////////////////////////////////////////////
// 封包式的Session,先用void*來處理,然後用template TTSesPacket 定義實際的封包
//---------------------------------------------------------------------------
K_class TSesPacket : public TSesLibBase
{
   typedef TSesLibBase  inherited;

protected:
   int   HeadSize_;
   int   WaitSkipBytes_;
   int   MaxPacketSize_;   //最大封包的大小
   int   CurrPacketSize_;  //現在正在接收的封包的大小
   int   RPos_;
   char* Buf_;

   //↓當[接收機],收到某個Client傳來的某個封包時,用底下的method來作後續的處理
   //  傳回false,表示不要再繼續處理後續的作業了
        K_mf(bool) ChangeCurrSize(int aNewSz);
virtual K_mf(int)  vBeforeAPacket (void*); //傳回 <= _CurrPacketSize,表示此筆封包已經收完,可進入APacket
                                          //否則傳回新的 CurrPacketSize,然後繼續接收直到您傳回的大小
virtual K_mf(bool) vAPacket       (void*) = 0;
virtual K_mf(void) vAfterAPacket  (void*);
virtual K_mf(void) DoTimeOut      (TTimerBase*) {};

public:
        K_ctor TSesPacket(TSesMgr* aOwner, int aMaxPacketSize, int aHeadSize, void* aBuf);
   //↓若有收到資料時，藉由叫用 CheckRecv() 來接收資料
   //  若已收了一個完整的封包，則會叫用 APacket(Buf)
virtual K_mf(void) CheckRecv     ();
virtual K_mf(char*)GetState      () { return ""; }
virtual K_mf(int)  vGetPacketSize (void*) = 0;
};
//---------------------------------------------------------------------------
// 封包式的Session,當收到一個TPacket封包時進行處理
//---------------------------------------------------------------------------
template <typename TPacket, int aaHeadSize, int aaPktSz=1>
K_class TTSesPacket : public TSesPacket
{
   typedef TSesPacket                        inherited;
   typedef TTSesPacket<TPacket,aaHeadSize>   TSelf;
protected:
   TPacket      RecvPkt_;

virtual K_mf(int)  vGetPacketSize (void* b) { return GetPacketSize(*(TPacket*)b); }
virtual K_mf(int)  vBeforeAPacket (void* b) { return BeforeAPacket(*(TPacket*)b); }
virtual K_mf(bool) vAPacket       (void* b) { return APacket(*(TPacket*)b);       }
virtual K_mf(void) vAfterAPacket  (void* b) {        AfterAPacket(*(TPacket*)b);  }
virtual K_mf(void) DoTimeOut      (TTimerBase* tm) { inherited::DoTimeOut(tm); }
protected:
   //↓當[接收機],收到某個Client傳來的某個封包時,用底下的method來作後續的處理
   //  傳回 > _CurrPacketSize,表示封包資料還沒收完,傳回值為成正確的封包大小
virtual K_mf(int)  BeforeAPacket (TPacket&);
   //↓APacket 傳回 true, 則會進行 AfterAPacket(),否則繼續處理下一個 packet
virtual K_mf(bool) APacket       (TPacket&) = 0;
virtual K_mf(void) AfterAPacket  (TPacket&);

public:
        K_ctor TTSesPacket (TSesMgr* aOwner)
               : inherited(aOwner, aaPktSz>sizeof(TPacket) ? aaPktSz : sizeof(TPacket),
                           aaHeadSize, &RecvPkt_)
               { }

   //↓傳送一個包裝給Client, 若 return false 表示傳送失敗
virtual K_mf(bool) Send          (TPacket&);
virtual K_mf(int)  GetPacketSize (TPacket&) = 0;
};
//===========================================================================
template <typename TPacket, int aaHeadSize, int aaPktSz>
K_mf(int) TTSesPacket<TPacket,aaHeadSize,aaPktSz>::BeforeAPacket (TPacket&)
{
  return 0;
}
//---------------------------------------------------------------------------
template <typename TPacket, int aaHeadSize, int aaPktSz>
K_mf(void) TTSesPacket<TPacket,aaHeadSize,aaPktSz>::AfterAPacket (TPacket&)
{
}
//---------------------------------------------------------------------------
template <typename TPacket, int aaHeadSize, int aaPktSz>
K_mf(bool) TTSesPacket<TPacket,aaHeadSize,aaPktSz>::Send (TPacket& pkt)
{
  if(GetMes() == NULL)
    return false;
  int sz = BeforeAPacket(pkt);
  GetMes()->Send(&pkt, sz ? sz:GetPacketSize(pkt));
  return true;
}
/////////////////////////////////////////////////////////////////////////////
};  // Bx
};  // Tw
};  // Kway
//---------------------------------------------------------------------------
#endif
