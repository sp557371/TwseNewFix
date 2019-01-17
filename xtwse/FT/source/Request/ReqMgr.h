//---------------------------------------------------------------------------
#ifndef ReqMgrH
#define ReqMgrH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include "SesMgr.h"
#include "FTCenter.h"
#include "bxSesMgr.h"
#include "KFile.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
//---------------------------------------------------------------------------
K_class TAPIMgr : public Kway::TSesMgr
{
    typedef Kway::TSesMgr   inherited;
    typedef TAPIMgr         this_type;
    typedef Kstl::ev_map<TBrokerID, Kway::dword>  TBrkSeqs;
    typedef TStdRecorder<TBrkSeqs, Kway::dword>   TBrkSeqsRec;
    typedef Kstl::ev_map<TBrkMkt, Kway::dword>    TBrkF06Seqs;
    typedef TStdRecorder<TBrkF06Seqs,Kway::dword> TBrkF06SeqsRec;

    TBrkSeqs        BrkSeqs_;
    TBrkSeqsRec*    BrkSeqsRec_;
    TBrkF06Seqs     BrkF06Seqs_;
    TBrkF06SeqsRec* BrkF06SeqsRec_;
    TViewAreaMgr    VAMgr_;
    TFile           Log_;
    K_mf(bool) WriteLog(const char*, bool input=true);
public:
    K_ctor TAPIMgr();
    K_dtor ~TAPIMgr();
    K_mf(const TViewAreaMgr*) GetViewAreaMgr() const { return &VAMgr_; }
    K_mf(bool) WriteRecvLog(const char* data) { return WriteLog(data); }
    K_mf(bool) WriteSendLog(const char* data) { return WriteLog(data, false); }
    K_mf(void) WriteSeqNo(TBrokerID, Kway::dword);
    K_mf(Kway::dword) GetSeqNo(TBrokerID);
    K_mf(void) WriteF06SeqNo(TBrokerID, TMarket, Kway::dword);
    K_mf(Kway::dword) GetF06SeqNo(TBrokerID, TMarket);
};
//---------------------------------------------------------------------------
K_fn(const TAPIMgr&) GetAPI();
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif
