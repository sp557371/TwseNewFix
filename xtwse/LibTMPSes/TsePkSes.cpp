//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TsePkSes.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw::Stk;
using namespace Kway::Tw::bx;
//---------------------------------------------------------------------------
K_mf(int) TTsePkSes::GetPacketSize    (TbxRec* pkt)
{
   char_ary<char,6,' '> Code_;
   memcpy(Code_.begin(), pkt, sizeof(Code_));
   switch(Code_.as_int()){
      case 102002:
      case 912002:
                   return CtlHeadSize+sizeof(TAppendNo);
      case 102003:
      case 912003:
                   return sizeof(TL040);
      case 300100:   /* 上市櫃的整股及盤後委託封包,下刪改查*/
      case 300200:
      case 300300:
      case 300400:
      case 300500:
      case 300600:
      case 300700:
      case 930100:
      case 930200:
      case 930300:
      case 930400:
      case 930500:
      case 930600:
      case 930700:
      case 320100:
      case 320200:
      case 320300:
      case 320400:
      case 320500:
      case 320600:
      case 320700:
      case 980100:
      case 980200:
      case 980300:
      case 980400:
      case 980500:
      case 980600:
      case 980700:
                   return sizeof(TT010);
      case 300101:   /* 上市櫃的整股及盤後委託回報,下刪改查*/
      case 300201:
      case 300301:
      case 300401:
      case 300501:
      case 300601:
      case 300701:
      case 930101:
      case 930201:
      case 930301:
      case 930401:
      case 930501:
      case 930601:
      case 930701:
      case 320101:
      case 320201:
      case 320301:
      case 320401:
      case 320501:
      case 320601:
      case 320701:
      case 980101:
      case 980201:
      case 980301:
      case 980401:
      case 980501:
      case 980601:
      case 980701:
                   return sizeof(TT020);
      case 400100:    /* 上市櫃的零股委託,下刪改查*/
      case 400200:
      case 400300:
      case 400400:
      case 400500:
      case 940100:
      case 940200:
      case 940300:
      case 940400:
      case 940500:
                   return sizeof(TO010);
      case 400101:   /* 上市櫃的零股委託回報,下刪改查*/
      case 400201:
      case 400301:
      case 400401:
      case 400501:
      case 940101:
      case 940201:
      case 940301:
      case 940401:
      case 940501:
                   return sizeof(TO020);
      case 500000:
      case 950000:
                   return sizeof(TR1);
      case 500001:
      case 950001:
                   return sizeof(TR2);
      case 501000:
      case 951000: {
                   TR3* R3_;
                   R3_=(TR3*)pkt;
                   return CtlHeadSize+6+R3_->BodyLength_.as_int();
      }
      case 502000:
      case 952000:
                   return sizeof(TR6);
      case 700100:
      case 700200:
      case 700300:
      case 700400:
                   return sizeof(TA010);
      case 700101:
      case 700201:
      case 700301:
      case 700401:
                   return sizeof(TA020);
      case 310100:
      case 310200:
      case 310300:
      case 310400:
      case 900100:
      case 900200:
      case 900300:
      case 900400:
                   return sizeof(TV010);
      case 310101:
      case 310201:
      case 310301:
      case 310401:
      case 900101:
      case 900201:
      case 900301:
      case 900401:
                   return sizeof(TV020);
      case 410100:
      case 410200:
      case 410300:
      case 410400:
      case 970100:
      case 970200:
      case 970300:
      case 970400:
                   return sizeof(TE010);
      case 410101:
      case 410201:
      case 410301:
      case 410401:
      case 970101:
      case 970201:
      case 970301:
      case 970401:
                   return sizeof(TE020);
   }
   return CtlHeadSize;      // 預設值
}
//---------------------------------------------------------------------------

