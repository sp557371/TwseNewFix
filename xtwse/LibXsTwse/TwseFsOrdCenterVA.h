/////////////////////////////////////////////////////////////////////////////
#if !defined(_TwseFsOrdCenterVA_h_)
#define _TwseFsOrdCenterVA_h_
//---------------------------------------------------------------------------
#include "ViewAreaMgr.h"
#include "FixStkIntf.h"
#include "FixStkOrdTrack.h"
#include "FixStkOrdCenter.h"
#include "FixStkRptTrack.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {

/////////////////////////////////////////////////////////////////////////////
// class TTwseFsOrdCenterVA
/////////////////////////////////////////////////////////////////////////////
class TTwseFsOrdCenterVA : public TVACreater
{
   typedef TVACreater  inherited;

public:
   typedef FIX::Stk::TFixSysOrdID  TSysOrdID;
   typedef FIX::Stk::TTradeInfo    TTradeInfo;
   typedef FIX::Stk::TFsSysOrd     TSysOrd;
   typedef FIX::Stk::TFsSysOrdRef  TSysOrdRef;
   typedef FIX::Stk::TFsSysOrds    TSysOrds;
   typedef FIX::Stk::TOrdTrack     TOrdTrack;
   //**********************************************
   #if defined(_FixStkVer_RptTrack_)
   typedef FIX::Stk::TRptTrack     TRptTrack;
   #endif
   //**********************************************

private:
   //==============================================
   // class TSysOrdsFm
   //==============================================
   class TSysOrdsFm
   {
      typedef TSysOrds             container;
      typedef container::iterator  iterator;
      typedef container::updater   updater;

   public:
      K_mf(const TFieldDesp&) GetFieldDesp  (size_t n)                          const;
      K_mf(std::string)       Get           (const iterator& i, size_t fieldNo) const;
      K_mf(size_t)            GetFieldCount ()                                  const;
      K_mf(void)              Put           (container& c, iterator& i, size_t fieldNo, const std::string& str);
   };
   //==============================================
   // class TOrdTrackFm
   //==============================================
   class TOrdTrackFm
   {
      typedef TOrdTrack::TRsSeqNo  container;
      typedef container::iterator  iterator;
      typedef container::updater   updater;

   public:
      K_mf(const TFieldDesp&) GetFieldDesp  (size_t n)                          const;
      K_mf(std::string)       Get           (const iterator& i, size_t fieldNo) const;
      K_mf(size_t)            GetFieldCount ()                                  const;
      K_mf(void)              Put           (container& c, iterator& i, size_t fieldNo, const std::string& str);
   };
   
   //**********************************************
   #if defined(_FixStkVer_RptTrack_)
   //==============================================
   // class TRptTrackFm
   //==============================================
   class TRptTrackFm
   {
      typedef TRptTrack::TRsSeqNo  container;
      typedef container::iterator  iterator;

   public:
      K_mf(const TFieldDesp&) GetFieldDesp  (size_t n)                          const;
      K_mf(std::string)       Get           (const iterator& i, size_t fieldNo) const;
      K_mf(size_t)            GetFieldCount ()                                  const;

      inline K_mf(void) Put (container& c, iterator& i, size_t fieldNo, const std::string& str) {}
   };
   #endif
   //**********************************************
   
   //==============================================
   // class TSysOrdsVA
   //==============================================
   typedef Kway::TTViewArea<TSysOrds, TSysOrdsFm>  TSysOrdsVA;

   //==============================================
   // class TTrackVACr
   //==============================================
   template <class CtnT, class FmT>
   class TTrackVACr : public TVACreater
   {
   public:
      typedef CtnT                   TCtn;
      typedef FmT                    TFm;
      typedef TTViewArea<TCtn, TFm>  TViewArea;

      inline K_mf(TaViewArea) CreateNewVA ()  { return TaViewArea(new TViewArea(0)); }
   };
   
   //==============================================
   // class TVACrDetail
   //==============================================
   typedef TTrackVACr<TOrdTrack::TRsSeqNo, TOrdTrackFm>  TOrdTrackVACr;
   typedef TOrdTrackVACr::TViewArea                      TOrdTrackVA;
   //----------------------------------------------
   //**********************************************
   #if defined(_FixStkVer_RptTrack_)
   typedef TTrackVACr<TRptTrack::TRsSeqNo, TRptTrackFm>  TRptTrackVACr;
   typedef TRptTrackVACr::TViewArea                      TRptTrackVA;
   #endif
   //**********************************************

private:
   TSysOrds*      SysOrds_;
   TOrdTrack*     OrdTrack_;
   TOrdTrackVACr  OrdTrackVACr_;
   //**********************************************
   #if defined(_FixStkVer_RptTrack_)
   TRptTrack*     RptTrack_;
   TRptTrackVACr  RptTrackVACr_;
   #endif
   //**********************************************

public:
   K_ctor TTwseFsOrdCenterVA (TTradeInfo*);

protected:
   //==============================================
   // overwrite TVACreater virtual function
   K_mf(TaViewArea)  CreateNewVA          ();
   K_mf(size_t)      GetDetails           (const char**&);
   K_mf(TVACreater*) GetDetailVACreater   (size_t detailID);
   K_mf(TVACreater*) OnMasterViewRowMoved (TViewArea& masterVA, TViewArea*& detailVA, size_t detailID);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseFsOrdCenterVA_h_)
