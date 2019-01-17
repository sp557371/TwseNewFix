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
   char_ary<char,256,' '>      LocaltmpPath_; // 3T架構"本機暫存路徑"的設定
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
   virtual K_mf(bool)      SendReq      (const TFTJob*, int hid = -1)        // 新委託到達後的作業,可指定傳送主機
                                                            {return false;}
   virtual K_mf(void)      SendState    (TState){};                          // 當本機的線路及工作狀態改變時所發出的通知
   virtual K_mf(eLState)   GetState     (TBrkMkt m,int& hid){return elsNone;}// 取得所有繞單機制所維護的各主機狀態
   virtual K_mf(bool)      IsUseORD     (){return false;}                    // 是否進行分單作業
   virtual K_mf(bool)      IsUseORM     (){return false;}                    // 是否可進行斷線繞單作業
   virtual K_mf(int)       TestHandle   (TBrkMkt m,TFileCode fcode){return -1;}// 判斷某項作業是否在其他部主機己處理
   virtual K_mf(void)      ManageState  (TState e, Kway::byte hid){};          // 管理其他台主機的狀態
   virtual K_mf(void)      ResetState   (Kway::byte hid){};                    // 斷線時要重置其他台主機的狀態
   virtual K_mf(int)       Dispatchable (TBrkMkt m,int,TFileCode){return -1;}  // 判斷是否到達分單的條件,回傳主機代號
   // 在還沒有更準確的判斷方式之前, 分單的條件單純以處理中的工作數量來判斷,回傳 -1 表示不分單
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
