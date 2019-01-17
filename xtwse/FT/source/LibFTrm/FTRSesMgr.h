//---------------------------------------------------------------------------
#ifndef FTRSesMgrH
#define FTRSesMgrH
//---------------------------------------------------------------------------
#include "SesMgrR.h"
#include "TimerThread.h"
#include "KFile.h"
#include "stdSettingP.h"
#include "FTRMDef.h"
//---------------------------------------------------------------------------
namespace Kway{
namespace Tw{
namespace Bx{
namespace FT{
/////////////////////////////////////////////////////////////////////////////
K_class TRSesMgr:public Kway::TSesMgrR,
                 public TFTRBase
{
   typedef TSesMgrR               inherited;
   typedef TFTRBase               inherited_FT;
   typedef std::vector<TSesBase*> TSesList;
   typedef TRSesMgr               this_type;
   typedef Kstl::ev_map<TFileCode, Kway::byte> TFCodes;
   struct THostRec{
	   eLState        State_; 
	   Kway::word     QJobs_;
	   TFCodes        FCodes_;
	   K_ctor THostRec():QJobs_(0),State_(elsNone){};
   };
   typedef Kstl::ev_map<Kway::byte, THostRec>  TRecs;
   typedef Kstl::ev_map<TBrkMkt, TRecs>        THostRecs;
   typedef Kstl::ev_map<TBrkMkt, TFCodes>      TProcRecs;
private:
   TStates                 LocalState_;
   TSettingPage*           RSetting_;
   bool                    UseRM_;
   bool                    UseRD_;
   Kway::TViewAreaMgr      VAMgr_;
   TSesList                SesList_;
   TSesList                CliList_;
   TFile                   ORRecorder_;
   THostRecs*              HostRecs_;        // 所有可繞單主機的線路狀況加總
   TProcRecs               ProcRecs_;        // 所有己送進來的工作清單, 己開始傳送後移除
   Kway::byte              LocalHID_;
public:
   K_ctor TRSesMgr       ();
   K_dtor ~TRSesMgr      ();
   K_mf(bool) SendReq    (const TFTJob* , int hid = -1);
   K_mf(void) SesReg(TSesBase*);
   K_mf(void) SesDel(TSesBase*);
   K_mf(void) RegCli(TSesBase*);
   K_mf(void) DelCli(TSesBase*);
   K_mf(void) WriteLog(char*);
   K_mf(void) SendState (TState ls);
   K_mf(bool) IsUseORD  (){return UseRD_;}
   K_mf(bool) IsUseORM  (){return UseRM_;}
   K_mf(Kway::byte) GetLocalHID(){return LocalHID_;}
   K_mf(eLState)    GetState     (TBrkMkt m, int& hid);      // 取得所有繞單機制所維護的各主機狀態
   K_mf(int)        TestHandle   (TBrkMkt m,TFileCode fcode);// 判斷某項作業是否在其他部主機己處理
   K_mf(void)       ManageState  (TState e, Kway::byte hid); // 管理其他台主機的狀態
   K_mf(void)       ResetState   (Kway::byte hid);           // 斷線時要重置其他台主機的狀態
   K_mf(int)        Dispatchable (TBrkMkt m,int,TFileCode);  // 判斷是否到達分流的條件
   K_mf(void)       AddNewFCode  (TBrkMkt, TFileCode);
};
//---------------------------------------------------------------------------
};
};
};
};
//---------------------------------------------------------------------------
#endif
