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
// �ʥ]����Session,����void*�ӳB�z,�M���template TTSesPacket �w�q��ڪ��ʥ]
//---------------------------------------------------------------------------
K_class TSesPacket : public TSesLibBase
{
   typedef TSesLibBase  inherited;

protected:
   int   HeadSize_;
   int   WaitSkipBytes_;
   int   MaxPacketSize_;   //�̤j�ʥ]���j�p
   int   CurrPacketSize_;  //�{�b���b�������ʥ]���j�p
   int   RPos_;
   char* Buf_;

   //����[������],����Y��Client�ǨӪ��Y�ӫʥ]��,�Ω��U��method�ӧ@���򪺳B�z
   //  �Ǧ^false,��ܤ��n�A�~��B�z���򪺧@�~�F
        K_mf(bool) ChangeCurrSize(int aNewSz);
virtual K_mf(int)  vBeforeAPacket (void*); //�Ǧ^ <= _CurrPacketSize,��ܦ����ʥ]�w�g����,�i�i�JAPacket
                                          //�_�h�Ǧ^�s�� CurrPacketSize,�M���~�򱵦�����z�Ǧ^���j�p
virtual K_mf(bool) vAPacket       (void*) = 0;
virtual K_mf(void) vAfterAPacket  (void*);
virtual K_mf(void) DoTimeOut      (TTimerBase*) {};

public:
        K_ctor TSesPacket(TSesMgr* aOwner, int aMaxPacketSize, int aHeadSize, void* aBuf);
   //���Y�������ƮɡA�ǥѥs�� CheckRecv() �ӱ������
   //  �Y�w���F�@�ӧ��㪺�ʥ]�A�h�|�s�� APacket(Buf)
virtual K_mf(void) CheckRecv     ();
virtual K_mf(char*)GetState      () { return ""; }
virtual K_mf(int)  vGetPacketSize (void*) = 0;
};
//---------------------------------------------------------------------------
// �ʥ]����Session,����@��TPacket�ʥ]�ɶi��B�z
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
   //����[������],����Y��Client�ǨӪ��Y�ӫʥ]��,�Ω��U��method�ӧ@���򪺳B�z
   //  �Ǧ^ > _CurrPacketSize,��ܫʥ]����٨S����,�Ǧ^�Ȭ������T���ʥ]�j�p
virtual K_mf(int)  BeforeAPacket (TPacket&);
   //��APacket �Ǧ^ true, �h�|�i�� AfterAPacket(),�_�h�~��B�z�U�@�� packet
virtual K_mf(bool) APacket       (TPacket&) = 0;
virtual K_mf(void) AfterAPacket  (TPacket&);

public:
        K_ctor TTSesPacket (TSesMgr* aOwner)
               : inherited(aOwner, aaPktSz>sizeof(TPacket) ? aaPktSz : sizeof(TPacket),
                           aaHeadSize, &RecvPkt_)
               { }

   //���ǰe�@�ӥ]�˵�Client, �Y return false ��ܶǰe����
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
