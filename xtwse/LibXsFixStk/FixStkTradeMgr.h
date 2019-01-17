/////////////////////////////////////////////////////////////////////////////
#if !defined(_FixStkOrdMgr_h_)
#define _FixStkOrdMgr_h_
//---------------------------------------------------------------------------
#include "wise_ptr.h"
#include "ViewAreaMgr.h"
#include "FixStkIntf.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;
namespace FIX  {
namespace Stk  {
//---------------------------------------------------------------------------
class TCoids;
class TFsOrdCenter;
class TRptCenter;
class TFsOrdFlow;
class TRptFlow;
class TOrdTrack;
//*************************************************
#if defined(_FixStkVer_RptTrack_)
class TRptTrack;
#endif
//*************************************************
class TTradeAdpt;

/////////////////////////////////////////////////////////////////////////////
// class TTradeUnit
/////////////////////////////////////////////////////////////////////////////
K_class TTradeUnit : public ref_counter<>
{
protected:
   std::string  SesID_;
   TTradeInfo*  TradeInfo_;
   TTradeFact*  TradeFact_;

public:
   //==============================================
   // TTradeUnit virtual function
   virtual K_mf(TTradeFact*) CreateTradeFact () = 0;

   //==============================================
   // TTradeUnit function
   inline K_mf(TTradeInfo*)        GetFixTradeInfo ()        { return TradeInfo_;                    }
   inline K_mf(TFsOrdFlow*)        GetFixOrdFlow   ()        { return TradeInfo_->GetFixOrdFlow  (); }
   inline K_mf(TRptCenter*)        GetFixRptCenter ()        { return TradeInfo_->GetFixRptCenter(); }
   inline K_mf(TTradeAdpt*)        GetFixTradeAdpt ()        { return TradeInfo_->GetFixTradeAdpt(); }
   inline K_mf(TViewAreaMgr&)      GetVaMgr        ()        { return TradeInfo_->GetVaMgr       (); }
   inline K_mf(const std::string&) GetSesID        () const  { return SesID_;                        }

protected:
           K_ctor  TTradeUnit (const std::string& sesID);
   virtual K_dtor ~TTradeUnit ();

   K_mf(void) Init ();
};
//---------------------------------------------------------------------------
typedef wise_ref<TTradeUnit>  TTradeUnitPtr;


/////////////////////////////////////////////////////////////////////////////
// class TTradeUnitReg
/////////////////////////////////////////////////////////////////////////////
typedef TTradeUnit*  (*TTradeUnitCreater)  (const std::string& sesID, void*);
//---------------------------------------------------------------------------
struct TTradeUnitReg
{
   const char*        PoliName_;      
   TTradeUnitCreater  TradeUnitCreater_;
   void*              Param_;
};


/////////////////////////////////////////////////////////////////////////////
// class TTradeMgr
/////////////////////////////////////////////////////////////////////////////
K_class TTradeMgr
{
public:
   struct SRecBase
   {
      std::string  PoliName_;
   };

   struct SRec : SRecBase
   {
      TTradeUnitPtr  TradeUnitPtr_;
   
      inline K_ctor SRec ()                                       {}
      inline K_ctor SRec (const SRecBase& base) : SRecBase(base)  {}
   };

   typedef ev_map<std::string, SRec>  TRecs;
   typedef TRecs::iterator            TRecsI;
   typedef TRecs::updater             TRecsU;

   typedef pod_vector<const TTradeUnitReg*>  TTradeUnitRegs;
   typedef TTradeUnitRegs::iterator          TTradeUnitRegsI;

private:
   class TRecsRecorder;

   TRecs           Recs_;
   TRecsRecorder*  RecsRecorder_;
   TViewAreaMgr    VAMgr_;
   TTradeUnitReg   DefReg_;
   TTradeUnitRegs  TradeUnitRegs_;

public:
   K_ctor  TTradeMgr ();
   K_dtor ~TTradeMgr ();

   K_mf(void)          Reload       (const char*);
   K_mf(TTradeUnitPtr) QueryUnit    (const std::string& sesID, const std::string& poliName = std::string());
   K_mf(TTradeUnitPtr) GetFirstUnit ();
   K_mf(void)          Cleanup      ();

   inline K_mf(void) Reg (TTradeUnitCreater tuCreater, void* param)
      {
         DefReg_.TradeUnitCreater_ = tuCreater;
         DefReg_.Param_            = param;
      }
   
   //inline K_mf(void) Reg (const TTradeUnitReg* begin, const TTradeUnitReg* end)  { TradeUnitRegs_.add(begin, end); }
   //inline K_mf(void) Del (const TTradeUnitReg* begin, const TTradeUnitReg* end)  { TradeUnitRegs_.del(begin, end); }

   inline K_mf(TViewAreaMgr&) GetVAC ()  { return VAMgr_; }
   
   inline K_mf(size_t) GetUnitSize () const  { return Recs_.size(); }

   static K_mf(TTradeMgr&) GetInst ();

protected:
   K_mf(TTradeUnitPtr) CreateTradeUnit (TRecsI&);
   K_mf(TTradeUnit*)   CreateTradeUnit (const std::string& sesID, const std::string& poliName = std::string());
   K_mf(void)          Cleanup         (TRecsI&, bool autoErase);

//public:
//   K_mf(void)          InitUnit  (TTradeUnitPtr);
};


/////////////////////////////////////////////////////////////////////////////
}; // namespace Stk
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_FixStkOrdMgr_h_)
