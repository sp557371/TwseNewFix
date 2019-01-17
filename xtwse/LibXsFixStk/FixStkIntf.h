/////////////////////////////////////////////////////////////////////////////
#if !defined(_FixStkIntf_h_)
#define _FixStkIntf_h_
//---------------------------------------------------------------------------
#include <string>
#include <bitset>
#include "Nsa_LogFile.h"
#include "FixStkVer.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   class TVACreater;
   class TViewAreaMgr;
   using namespace Nsa;

namespace FIX {
namespace Stk {
//---------------------------------------------------------------------------
class TCoids;
class TOrdTrack;
class TFsOrdFlow;
class TFsOrdCenter;
//*************************************************
#if defined(_FixStkVer_RptTrack_)
class TRptTrack;
#endif
//*************************************************
class TRptFlow;
class TRptCenter;
class TTradeUnit;
class TTradeAdpt;


/////////////////////////////////////////////////////////////////////////////
// class TTiProp
/////////////////////////////////////////////////////////////////////////////
enum ETiProp
{
   tip_Disable_RptTrack_Reco,
   tip_Total
};
typedef std::bitset<tip_Total>  TTiProp;


/////////////////////////////////////////////////////////////////////////////
// class TTradeLog
/////////////////////////////////////////////////////////////////////////////
K_class TTradeLog
{
   TLogFile*    LogFile_;
   std::string  LogPath_;

public:
   K_ctor TTradeLog (const char* logPath);

   inline K_dtor ~TTradeLog ()
      { if (LogFile_)  delete LogFile_; }

   K_mf(void) LogMem (ELogType, const void*, size_t);
};


/////////////////////////////////////////////////////////////////////////////
// class TTradeInfo
/////////////////////////////////////////////////////////////////////////////
K_class TTradeInfo
{
public:
   struct SFlushInfo
   {
      bool  Coids_;
      bool  OrdCenter_;
      bool  OrdTrack_;
      bool  RptCenter_;
      bool  ExecIDGene_;
   
      inline K_ctor SFlushInfo ()
         : Coids_(false), OrdCenter_(false), OrdTrack_(false), RptCenter_(false), ExecIDGene_(false)
      {}
   };

protected:
   std::string    SesID_;
   std::string    LogPath_;
   TViewAreaMgr*  VaMgr_;
   SFlushInfo     FlushInfo_;
   int            FixVer_; // fix ª©¥»¸¹
   // FixStkObjs
   TTiProp        FixTiProp_;
   TTradeLog*     FixTradeLog_;
   TCoids*        FixCoids_;
   TOrdTrack*     FixOrdTrack_;
   TFsOrdFlow*    FixOrdFlow_;
   TFsOrdCenter*  FixOrdCenter_;
   //**********************************************
   #if defined(_FixStkVer_RptTrack_)
   TRptTrack*     FixRptTrack_;
   #endif
   //**********************************************
   TRptFlow*      FixRptFlow_;
   TRptCenter*    FixRptCenter_;
   TTradeUnit*    FixTradeUnit_;
   TTradeAdpt*    FixTradeAdpt_;

public:
           K_ctor  TTradeInfo (const std::string& sesID);
   virtual K_dtor ~TTradeInfo ();

   K_mf(void) UseLogFile ();

   inline K_mf(void) SetFixVer       (int           n)  { FixVer_       = n; }
   inline K_mf(void) SetFixCoids     (TCoids*       p)  { FixCoids_     = p; }
   inline K_mf(void) SetFixOrdTrack  (TOrdTrack*    p)  { FixOrdTrack_  = p; }
   inline K_mf(void) SetFixOrdFlow   (TFsOrdFlow*   p)  { FixOrdFlow_   = p; }
   inline K_mf(void) SetFixOrdCenter (TFsOrdCenter* p)  { FixOrdCenter_ = p; }
   //**********************************************
   #if defined(_FixStkVer_RptTrack_)
   inline K_mf(void) SetFixRptTrack  (TRptTrack*    p)  { FixRptTrack_  = p; }
   #endif
   //**********************************************
   inline K_mf(void) SetFixRptFlow   (TRptFlow*     p)  { FixRptFlow_   = p; }
   inline K_mf(void) SetFixRptCenter (TRptCenter*   p)  { FixRptCenter_ = p; }
   inline K_mf(void) SetFixTradeUnit (TTradeUnit*   p)  { FixTradeUnit_ = p; }
   inline K_mf(void) SetFixTradeAdpt (TTradeAdpt*   p)  { FixTradeAdpt_ = p; }

   inline K_mf(TViewAreaMgr&) GetVaMgr        ()  { return *VaMgr_;       }
   inline K_mf(SFlushInfo&)   GetFlushInfo    ()  { return FlushInfo_;    }
   inline K_mf(TTradeLog*)    GetFixTradeLog  ()  { return FixTradeLog_;  }
   inline K_mf(TCoids*)       GetFixCoids     ()  { return FixCoids_;     }
   inline K_mf(TOrdTrack*)    GetFixOrdTrack  ()  { return FixOrdTrack_;  }
   inline K_mf(TFsOrdFlow*)   GetFixOrdFlow   ()  { return FixOrdFlow_;   }
   inline K_mf(TFsOrdCenter*) GetFixOrdCenter ()  { return FixOrdCenter_; }
   //**********************************************
   #if defined(_FixStkVer_RptTrack_)
   inline K_mf(TRptTrack*)    GetFixRptTrack  ()  { return FixRptTrack_;  }
   #endif
   //**********************************************
   inline K_mf(TRptFlow*)     GetFixRptFlow   ()  { return FixRptFlow_;   }
   inline K_mf(TRptCenter*)   GetFixRptCenter ()  { return FixRptCenter_; }
   inline K_mf(TTradeUnit*)   GetFixTradeUnit ()  { return FixTradeUnit_; }
   inline K_mf(TTradeAdpt*)   GetFixTradeAdpt ()  { return FixTradeAdpt_; }
   inline K_mf(TTiProp*)      GetFixTiProp    ()  { return &FixTiProp_;   }

   inline K_mf(const std::string&) GetSesID   () const  { return SesID_;   }
   inline K_mf(const std::string&) GetLogPath () const  { return LogPath_; }
   inline K_mf(const int&)         GetFixVer  () const  { return FixVer_;  }

   inline K_mf(const bool&) GetFlushCoids      () const  { return FlushInfo_.Coids_;      }
   inline K_mf(const bool&) GetFlushOrdCenter  () const  { return FlushInfo_.OrdCenter_;  }
   inline K_mf(const bool&) GetFlushOrdTrack   () const  { return FlushInfo_.OrdTrack_;   }
   inline K_mf(const bool&) GetFlushRptCenter  () const  { return FlushInfo_.RptCenter_;  }
   inline K_mf(const bool&) GetFlushExecIDGene () const  { return FlushInfo_.ExecIDGene_; }
};


/////////////////////////////////////////////////////////////////////////////
// class TTradeFact
/////////////////////////////////////////////////////////////////////////////
K_class TTradeFact
{
public:
   virtual K_mf(TFsOrdFlow*) CreateOrdFlow      (TTradeInfo*) = 0;
   virtual K_mf(TRptFlow*)   CreateRptFlow      (TTradeInfo*) = 0;
   virtual K_mf(TTradeAdpt*) CreateTradeAdpt    (TTradeInfo*) = 0;

   virtual K_mf(TTradeInfo*) CreateTradeInfo    (const std::string&);
   virtual K_mf(TVACreater*) CreateOrdCenetrVAC (TTradeInfo*);
};


/////////////////////////////////////////////////////////////////////////////
// class TTradeAdpt
/////////////////////////////////////////////////////////////////////////////
K_class TTradeAdpt
{
protected:
   TTradeInfo*  FixTradeInfo_;

public:
   inline K_ctor TTradeAdpt (TTradeInfo* ti)
      : FixTradeInfo_(ti)
   {}

   virtual K_dtor ~TTradeAdpt ()  {}

   //==============================================
   // TTradeAdpt virtual function
   virtual K_mf(void) Startup () = 0;

   //==============================================
   // TTradeAdpt function
   
   //==============================================
   // TTradeAdpt inline function
   inline K_mf(TTradeInfo*)        GetFixTradeInfo ()        { return FixTradeInfo_;                    }
   inline K_mf(TFsOrdCenter*)      GetFixOrdCenter ()        { return FixTradeInfo_->GetFixOrdCenter(); }
   inline K_mf(TOrdTrack*)         GetFixOrdTrack  ()        { return FixTradeInfo_->GetFixOrdTrack (); }
   inline K_mf(TFsOrdFlow*)        GetFixOrdFlow   ()        { return FixTradeInfo_->GetFixOrdFlow  (); }
   inline K_mf(TRptFlow*)          GetFixRptFlow   ()        { return FixTradeInfo_->GetFixRptFlow  (); }
   inline K_mf(const std::string&) GetSesID        () const  { return FixTradeInfo_->GetSesID       (); }
   inline K_mf(const std::string&) GetLogPath      () const  { return FixTradeInfo_->GetLogPath     (); }
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Stk
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_FixStkIntf_h_)
