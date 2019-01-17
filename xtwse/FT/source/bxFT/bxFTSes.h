//---------------------------------------------------------------------------
#ifndef bxFTSesH
#define bxFTSesH
//---------------------------------------------------------------------------
#include "bxLinkSes.h"
#include "bxFT.h"
#include "bxFTWorker.h"
#include "FTJob.h"
#include "bxSesMgr.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
//---------------------------------------------------------------------------
K_class TbxFTSes : public TbxLinkSes
{
    typedef TbxLinkSes          inherited;
    typedef TbxFTSes            this_type;
    typedef TTimer<this_type>   Timer;
protected:
    TbxFTSesMgr& SesMgr_;
    Timer       FTTimer_;
    size_t      ReplyTimeOut_;
    size_t      HBSendTimeOut_;
    size_t      HBWaitTimeOut_;
    TWork_F2000 F2000_;
    TWork_F2001 F2001_;
    TWork_F2002 F2002_;
    TWork_F2003 F2003_;
    TWork_F2004 F2004_;
    size_t      RetryCount_;

    size_t      FileBlockSize_;
//    TFTErrInfo  FTErrInfo_;
    bool        IsX25_;
    TFileCode   CurrentFC_;
    size_t      CurrentSz_;
    std::string ID_;

    TFTReqBase* Transaction_;

    enum eFTDirect
    {
        eftdSend,   // FT Send
        eftdRecv,   // FT Recv
    };
    eFTDirect   FTDirect_;
    TSysID      WorkSysID_;
    K_mf(void)  AddLinkWorkers ();
//    K_mf(void)  SetFTErrorInfo ();
    K_mf(bool)  SettingApply   (const std::string& setting);
    K_mf(void)  OnMesLinkReady   (TMesBase*);
    K_mf(void)  OnMesLinkBroken  (TMesBase*, const std::string&);//清除接收緩衝區
    K_mf(void)  OnMesLinkFail    (TMesBase*, const std::string&);//SetState(sp_Error);
public:
    enum eFTState
    {
        efsError=9000,  // 由9000開始計算
        efsStart,
        efsFT,
        efsMsg,
        efsStop,
        efsHeartbeat,
        efsWaitNextJob
    };
virtual K_mf(void)  OnSubSysReady  (TApKey);
virtual K_mf(void)  OnSubSysBroken (TSubSysName,TStCode);
virtual K_mf(void)  DoTimeOut      (TTimerBase*);
virtual K_mf(void)  HBTimeOut      (TTimerBase*);
        K_mf(TWorkKey) GetWorkKey(TFunCode);
    K_ctor TbxFTSes(TbxFTSesMgr*, KStk::TMarket, bool send=true);
    K_dtor ~TbxFTSes(); 
    K_mf(void)        SetState   (eFTState, const std::string&);
    K_mf(void)        SetJob     (TFTReqBase*);
    K_mf(TFTReqBase*) GetJob     ()     { return Transaction_; }
    K_mf(void)        JobFinish  (TStCode code);
    K_mf(TFTFileReq*) AllocNewFT (const TF010&);
    K_mf(TFTFileReq*) AllocNewFT (const TSysID&, const TFTJob&);
    K_mf(TFTMsgReq*)  AllocNewMsg(const TF050&);
    K_mf(TFTMsgReq*)  AllocNewMsg(const TSysID&, const TFTJob&);
    K_mf(void)      StartFTTimer(size_t tm) { FTTimer_.Start(tm*1000, &this_type::HBTimeOut); }
    K_mf(void)      StopFTTimer ()          { FTTimer_.Stop();                    }
    K_mf(void)      StartHB();
    K_mf(void)      StopHB();
    K_mf(void)      StartWaitReplyTimer()   { StartTimer(ReplyTimeOut_);      }
    K_mf(void)      StopWaitReplyTimer()    { StopTimer();                    }
    K_mf(void)      StartSendHBTimer()      { StartFTTimer(HBSendTimeOut_);   }
    K_mf(void)      StartWaitHBTimer()      { StartFTTimer(HBWaitTimeOut_);   }
    K_mf(bool)      SendPkt  (const TFTHead&);
    K_mf(bool)      CheckStatusCode(TSubSysName, const TStCode);
    K_mf(bool)      IsFree()  { return GetSesState() ==efsStop;             }
    K_mf(bool)      IsSend()  { return FTDirect_     ==eftdSend;            }
    K_mf(bool)      IsExtend(){ return (!IsX25_);                           }
    K_mf(void)      UpdateFTStatus(EFTFlowID, TStCode, const char* msg=NULL);
    virtual K_mf(const std::string) SesName()
    { return std::string("檔案傳輸" + (FTDirect_==eftdSend ? std::string("(送)"):std::string("(收)"))); }
    K_mf(bool)      IsCompleteLog(){return SesMgr_.IsCompleteLog();}
    K_mf(void)      FetchNextJob(){SesMgr_.FetchNextJob(GetBrokerID(), GetMarket());}
    K_mf(std::string) GetID(){return ID_;}
};
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif
