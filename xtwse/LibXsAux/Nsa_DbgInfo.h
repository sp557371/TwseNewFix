/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_DbgInfo_h_)
#define _Nsa_DbgInfo_h_
//---------------------------------------------------------------------------
#include "char_ary.h"
#include "Nsa_Time.h"
#include "Nsa_Ver.h"
#include "tplns/Nsa_EvMap_2k.h"
#include "tplns/Nsa_EvMap_2k_Hnd.h"
#include "libnsAux.h"

/////////////////////////////////////////////////////////////////////////////
#if defined(_NsaVer_DbgInfo_)

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;
   class TVACreater;

namespace Nsa  {
   class TDbgInfoMgr;

/////////////////////////////////////////////////////////////////////////////
// base type
/////////////////////////////////////////////////////////////////////////////
typedef std::string        TDiName;
typedef fixed_num<size_t>  TDiSeqNo;
typedef fixed_num<int>     TDiTimeGap;

/////////////////////////////////////////////////////////////////////////////
// class TDbgInfoID
/////////////////////////////////////////////////////////////////////////////
// 這邊有異動時,請同步調整 TDbgInfoID::as_string
enum EDbgInfoID
{
   edi_Unknown,
      edi_TwseFix_TFsSysOrds_updater_dtor,
      edi_TwseFix_TFsOrdFlow_DoReq,
      edi_TwseFix_TApaSesSmpBase_OnMesRecvReady,
      edi_TwseFix_TApaSesSmpBase_TApaSesSmpPkt_GetMsg,
      edi_TwseFix_TApaSesSmp_MsgUnpack,
      edi_TwseFix_TSmpMsgMiddlewareEvent_OnSetMsgData,
      edi_TwseFix_TTwseFsOrdFlow_TRsOrdID_Insert,
      edi_TwseFix_TTwseFsOrdFlow_TRsOrdID_FindRsFixSoid,
      edi_TwseFix_TTwseFstAdpt_TOrdImpl_DoReqNew,
      
      edi_Mds_TCsNyseDict_StkProc,
   edi_Total
};
//---------------------------------------------------------------------------
LIBNSAUX_class TDbgInfoID : public EnumT<EDbgInfoID>
{
   typedef EnumT<EDbgInfoID>  inherited;

public:
   inline K_ctor TDbgInfoID (EDbgInfoID e = edi_Unknown) : inherited(e) {}
   
   K_mf(void) assign (const char*);
   
   K_mf(std::string) as_string () const;
      
   inline K_mf(void) assign (const std::string& str)  { assign(str.c_str()); }
      
   inline K_mf(EDbgInfoID) get_orig () const  { return inherited::Data_; }
};


/////////////////////////////////////////////////////////////////////////////
// class TDbgInfoHnd
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TDbgInfoHnd
{
   friend class TDbgInfoMgr;

   TDbgInfoID   DiID_;
   const void*  Owner_;
   TTimeUs      BegTime_;
   TTimeUs      EndTime_;
   int          CostTime_;
   std::string  DbgParam_;
   bool         KeepAll_;

public:
   K_ctor  TDbgInfoHnd (EDbgInfoID, const void*, bool keepAll = false, const std::string& param = std::string());
   K_dtor ~TDbgInfoHnd ();

   inline K_mf(void) SetDbgParam (const char*        str)  { DbgParam_.assign(str); }
   inline K_mf(void) SetDbgParam (const std::string& str)  { DbgParam_ = str;       }
};

/////////////////////////////////////////////////////////////////////////////
// struct SDbgInfo1
/////////////////////////////////////////////////////////////////////////////
struct SDbgInfo1
{
   typedef fixed_num<uint64>  TCount;

   TCount    ExecCount_;
   TTimeUs   ExecTime_;
   TDiSeqNo  MaxTimeGap_;

   inline K_ctor SDbgInfo1 ()
      : ExecTime_(0, 0, 0, 0)
   {}
};
/////////////////////////////////////////////////////////////////////////////
// struct SDbgInfo2
/////////////////////////////////////////////////////////////////////////////
struct SDbgInfo2
{
   TDiTimeGap   TimeGap_;
   std::string  DbgParam_;
   TTimeUs      BegTime_;
   TTimeUs      EndTime_;
};

/////////////////////////////////////////////////////////////////////////////
// class TDbgInfoMgr
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TDbgInfoMgr
{
public:
   typedef TEvMap2k<TDiName, TDiSeqNo, SDbgInfo2, SDbgInfo1>  TRecsType;
   typedef TRecsType::TMap1   TRsName;
   typedef TRecsType::TMap2   TRsSeqNo;
   typedef TRecsType::TMapI2  TRsSeqNoI;

   //==============================================
   // class TRecsHnd
   class TRecsHnd : public TEvMap2kHnd<TRsName>
   {
      typedef TEvMap2kHnd<TRsName>  inherited;

   public:
      inline K_ctor TRecsHnd (TDbgInfoMgr* p, const TDiName& key1)
         : inherited(&(p->RsName_), key1)
      {}
   };
   friend class TRecsHnd;
   
   //==============================================
   typedef std::bitset<edi_Total>  TDbgInfoIDSet;

protected:
   class TRecsVac;
   
   TRsName        RsName_;
   TRecsVac*      RecsVac_;
   TDbgInfoIDSet  DbgInfoIDSet_;
   bool           ShowAll_;

public:
   K_ctor  TDbgInfoMgr ();
   K_dtor ~TDbgInfoMgr ();

   K_mf(void)        AddInfo (TDbgInfoHnd*);
   K_mf(TVACreater&) GetVac  ();

   static K_mf(TDbgInfoMgr&) GetInst ();
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // defined(_NsaVer_DbgInfo_)

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_DbgInfo_h_)
