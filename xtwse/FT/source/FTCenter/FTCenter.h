//---------------------------------------------------------------------------
#ifndef FtCenterH
#define FtCenterH
//---------------------------------------------------------------------------
#include "inn_file.h"
#include "SimpleRecorder.h"
#include "ViewArea.h"
#include "ViewAreaMgr.h"
#include "FTJobMap2d.h"
#include "FTFlowDef.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
//---------------------------------------------------------------------------
enum TFTCenterFieldNo {
    ftcfn_Seqno,
    ftcfn_WorkSeqno,
    ftcfn_BrkID,
//    ftcfn_Market,
    ftcfn_FileCode,
    ftcfn_Type,
    ftcfn_SrcData,
    ftcfn_DstData,
    ftcfn_RecLen,
    ftcfn_CreateTime,
    ftcfn_SendTime,
    ftcfn_ReplyTime,
    ftcfn_Memo,
    ftcfn_End,
};
//*****券商-檔案工作的Master-Detail***********************************************
K_class TJobsVACr : public Kway::TVACreater //Detail
{
   typedef Kway::TVACreater inherited;
   typedef TJobsVACr        this_type;
   K_class TDJobsFieldMaker
   {
      typedef TFtJobs                   TEvContainer;
      typedef TEvContainer::iterator    iterator;
      typedef size_t                    field_n_type;
   public:
      inline K_mf(field_n_type)     GetFieldCount ()                const { return ftcfn_End; }
      K_mf(const Kway::TFieldDesp&) GetFieldDesp  (field_n_type n)  const;
      K_mf(std::string)             Get (const iterator& i, field_n_type fieldNo) const;
      K_mf(void)                    Put (TEvContainer& c,iterator& iter, field_n_type fieldNo, const std::string& str);
   };
public:
   typedef Kway::TTViewArea<TFtJobs, TDJobsFieldMaker>  TDJobsViewer;
   K_mf(Kway::TaViewArea)  CreateNewVA  ()
    { return Kway::TaViewArea(new TDJobsViewer(0, TDJobsFieldMaker(), TViewControls())); }
};
//---------------------------------------------------------------------------
K_class TJobsVAC : public Kway::TVACreater         //Master
{
   typedef Kway::TVACreater inherited;
   typedef TJobsVAC         this_type;
   TJobsVACr                DetailCreater_;
   K_class TMJobsFieldMaker
   {
      typedef TBrkJobs                  TEvContainer;
      typedef TEvContainer::iterator    iterator;
      typedef size_t                    field_n_type;
   public:
      inline K_mf(field_n_type)     GetFieldCount () const    { return 1; }
      K_mf(const Kway::TFieldDesp&) GetFieldDesp  (field_n_type n) const;
      K_mf(std::string)             Get  (const iterator& i, field_n_type fieldNo) const;
      K_mf(void)                    Put  (TEvContainer& c, iterator& iter, field_n_type fieldNo, const std::string& str);
   };
   TBrkJobs*   Recs_;  // Master 的container
public:
   K_ctor TJobsVAC(TBrkJobs& recs) : Recs_(&recs) {}
   typedef Kway::TTViewArea<TBrkJobs, TMJobsFieldMaker>  TMJobsViewer;
   K_mf(Kway::TaViewArea)  CreateNewVA          () { return Kway::TaViewArea(new TMJobsViewer(Recs_,
                                                    TMJobsFieldMaker(), TViewControls())); }
   K_mf(size_t)            GetDetails           (const char**& a);
   K_mf(Kway::TVACreater*) GetDetailVACreater   (size_t detailID);
   K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);
};
//---------------------------------------------------------------------------
typedef Kway::TSimpleRecorder_Detail<TFtJobMapMaker, TCenterInn, sizeof(TFTJob)> TJob2d;
//---------------------------------------------------------------------------
K_class TFTCenter : public TFTFlowHandler
{
    typedef std::map<TBrkMkt, TSeqNo>   BrkSeqMap;
    typedef BrkSeqMap::iterator         BrkSeqMapI;
    typedef Tw::Stk::TMarket            TMarket;
    BrkSeqMap           BrkSeq_;
    TJob2d*             Job2d_;
    TBrkJobs            AllJobs_;
    TJobsVAC*           VAC_;
    TViewAreaMgr        VAMgr_;
    K_mf(bool)  GetJobHnd (const TBrkID&, const TMarket&, TSysID, TFtJobs::updater&);
    K_mf(void)  CreateVAC (void); //建立遠端VAC
    K_mf(void)  DestoryVAC(void);
    K_mf(void)  OnRecover (void);   
public:
    K_ctor TFTCenter    ();
    K_dtor ~TFTCenter   ();
    K_mf(const TViewAreaMgr*) GetViewAreaMgr() const { return &VAMgr_; }
    K_mf(TSysID) DoReq (const TFTNewJob&);
//    K_mf(TSysID) DoReq (const TBrkID&, const TMarket&, const TFileCode&, const char* msg);
    K_mf(TSysID) DoS10Msg(const TMarket&, const TF050&);
    K_mf(bool)   GetHnd(const TBrkID&, const TMarket&, const TSysID&, const TFTJob**);
    K_mf(TSeqNo) FetchSeqNo     (const TBrkID&, const TMarket&);
    K_mf(TSysID) FetchNextQing  (const TBrkID&, const TMarket&, TFileCode code = TFileCode());
    K_mf(void)   UpdateFTStatus (const TBrkID&, const TMarket&, TSysID, EFTFlowID, TStCode, const char*);
    K_mf(void)   UpdateFTFlow   (const TBrkID&, const TMarket&, TSysID, EFTFlowID, const std::string&);
    K_mf(void)   UpdateDstData  (const TBrkID&, const TMarket&, TSysID, const std::string&);
    K_mf(bool)   UpdateFTCenter (TBrkMkt&, TFileCode&);
    K_mf(Kway::word) GetQJobs   (TBrkMkt, TFileCode fc = TFileCode());
    K_mf(TSysID) FindFirstWaiting(const TBrkID&, const TMarket&, TFileCode code);
};
//---------------------------------------------------------------------------
K_fn(const TFTCenter&) GetCenter();
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif
