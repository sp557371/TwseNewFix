//---------------------------------------------------------------------------
#ifndef FTRMDefH
#define FTRMDefH
#include "PacketSes.h"
#include "TwStkOrder.h"
#include "FTJobDef.h"
#include "ExtDef.h"
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
namespace Kway{
namespace Tw{
namespace Bx{
namespace FT{
/////////////////////////////////////////////////////////////////////////////
#define RTime 60
enum  eCode{
   orHB       = 001,
   orHBEcho   = 002,
   orReq      = 101,
   orReqE     = 102,
   orState    = 201,
};
enum eLState{
	elsNone    = 0x00,
	elsRecv    = 0x01,
	elsSend    = 0x02,
	elsAll     = 0x03,
};
//---------------------------------------------------------------------------
struct TStateBase{
   TBrkMkt                     BrkMkt_;
   bool                        Send_;
   bool                        Recv_;
   Kway::word                  QJobs_;
   Kway::word                  Count_;
   char_ary<char,256,' '>      LocaltmpPath_; // 3T�[�c"�����Ȧs���|"���]�w
};
struct TState : public TStateBase{
   TFileCode                   FCode_[256];
   K_ctor TState(){Send_ = false; Recv_ = false; Count_ = 0; QJobs_ = 0;}
   K_ctor TState(TBrkID a, TMarket b){BrkMkt_ = TBrkMkt(a,b);Send_ = false; Recv_ = false; Count_ = 0; QJobs_ = 0;}
   K_ctor TState(TBrkMkt a){BrkMkt_ = a;Send_ = false; Recv_ = false; Count_ = 0; QJobs_ = 0;}
   K_mf(Kway::word) GetSize(){return sizeof(TStateBase)+Count_*sizeof(TFileCode);}
};
//---------------------------------------------------------------------------
#define HSize  5
struct SFTrm{
   Kway::word  Code_;
   Kway::word  Size_;
   Kway::byte  HostID_;
   Kway::byte  buf_[4096-HSize];
};
//---------------------------------------------------------------------------
typedef Kstl::ev_map<TBrkMkt, TState>       TStates;
//---------------------------------------------------------------------------
typedef TPacketSes<SFTrm,HSize,sizeof(SFTrm)> TFTrm_Base;
//---------------------------------------------------------------------------
class TFTRBase 
{
private:
protected:	// User declarations
public:		
           K_ctor          TFTRBase     (){};
           K_dtor          ~TFTRBase    (){}
   virtual K_mf(bool)      SendReq      (const TFTJob*, int hid = -1)        // �s�e�U��F�᪺�@�~,�i���w�ǰe�D��
                                                            {return false;}
   virtual K_mf(void)      SendState    (TState){};                          // �������u���Τu�@���A���ܮɩҵo�X���q��
   virtual K_mf(eLState)   GetState     (TBrkMkt m,int& hid){return elsNone;}// ���o�Ҧ�¶�����Һ��@���U�D�����A
   virtual K_mf(bool)      IsUseORD     (){return false;}                    // �O�_�i�����@�~
   virtual K_mf(bool)      IsUseORM     (){return false;}                    // �O�_�i�i���_�u¶��@�~
   virtual K_mf(int)       TestHandle   (TBrkMkt m,TFileCode fcode){return -1;}// �P�_�Y���@�~�O�_�b��L���D���v�B�z
   virtual K_mf(void)      ManageState  (TState e, Kway::byte hid){};          // �޲z��L�x�D�������A
   virtual K_mf(void)      ResetState   (Kway::byte hid){};                    // �_�u�ɭn���m��L�x�D�������A
   virtual K_mf(int)       Dispatchable (TBrkMkt m,int,TFileCode){return -1;}  // �P�_�O�_��F���檺����,�^�ǥD���N��
   // �b�٨S����ǽT���P�_�覡���e, ���檺�����¥H�B�z�����u�@�ƶq�ӧP�_,�^�� -1 ��ܤ�����
};
//---------------------------------------------------------------------------
};
};
};
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
   #pragma pack(pop)
#else
   #pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
#endif
