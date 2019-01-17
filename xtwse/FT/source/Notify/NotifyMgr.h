//---------------------------------------------------------------------------
#ifndef NotifyMgrH
#define NotifyMgrH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include "SesMgr.h"
#include "Ses.h"
#include "FTJob.h"
#include "TimerThread.h"
#include "stdSettingP.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
K_class TFTNotifyHandler;
//---------------------------------------------------------------------------
K_class TSenderMgr : public TSesMgr
{
    typedef TSesMgr         inherited;
    typedef TSenderMgr      this_type;
    typedef std::vector<TFTNotifyHandler*>  TSesVec;
    typedef TSesVec::iterator               TSesVecI;
    typedef std::queue<TFTJob>              TJobQue;
    struct SJobTime{
       Kway::TKTime   Time_;
       Kway::word     Interval_;
       TFTJob         Job_;
       std::string    Cmd_;
    };
    struct SMarketJob{
       TMarket    Market_;
       TFileCode  FileCode_;
       K_ctor SMarketJob(){};
       K_ctor SMarketJob(TMarket m, TFileCode fc): Market_(m), FileCode_(fc){};
          K_mf(bool) operator < (const SMarketJob& a) const
             { return Market_==a.Market_ ? FileCode_<a.FileCode_ : Market_<a.Market_; }
    };
    typedef std::map<SMarketJob, SJobTime>  TJobMap;
    typedef std::map<int, TFTNewJob>        TJobMap2;
    typedef TJobMap::iterator               TJobMapi;

    TSesVec         SesVec_;
    TViewAreaMgr    VAMgr_;
    TJobQue         Queue_;
    TFile           Log_;
    int             SeqNo_;
    TJobMap         JobMap_;
    TJobMap2        JobMap2_;
    TSettingPage*   Setting_;
    bool            NotifyFirst_;
    Kway::TTimer<this_type> Timer_;
    K_mf(void) OnTimer(TTimerBase*);
    K_mf(void) UnlinkAll ();
    K_mf(void) PushNotify();
public:
    K_ctor TSenderMgr();
    K_dtor ~TSenderMgr();
    K_mf(void) RegSes(TFTNotifyHandler*);
    K_mf(void) DelSes(TFTNotifyHandler*);
    K_mf(void) SendNotify(const TFTJob& job, bool t = false); 
    K_mf(const TViewAreaMgr*) GetViewAreaMgr() const { return &VAMgr_; }
    K_mf(bool) WriteLog  (const char*, size_t);
    K_mf(void) DoFileCmd(int, std::string&);
    K_mf(void) FileDLEcho(int, bool);
};
//---------------------------------------------------------------------------
K_fn(const TSenderMgr&) GetSender();
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif
