/////////////////////////////////////////////////////////////////////////////
#if !defined(_TwseFsOrdFlowAux_h_)
#define _TwseFsOrdFlowAux_h_
//---------------------------------------------------------------------------
#include "TwseFsOrdFlow.h"
#include "tplns/Nsa_EvMap_2k.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Nsa;

/////////////////////////////////////////////////////////////////////////////
// class TTwseFsOrdFlow::TRsOrdID
/////////////////////////////////////////////////////////////////////////////
// 不使用 recorder, 在每次啟動時從 FixOrdCenter 載入即可
class TTwseFsOrdFlow::TRsOrdID
{
public:
   typedef TEvMap2k<TBrkOrdID, TFixSysOrdID>  TRecsType;
   typedef TRecsType::TMap1      TRsBrkOrdID;
   typedef TRecsType::TDat1      TRsBrkOrdIDDat;
   typedef TRecsType::TMap2      TRsFixSoid;
   typedef TRsFixSoid::iterator  TRsFixSoidI;

private:
   class TFieldMaker1;
   class TFieldMaker2;
   class TRsVac;

   TRsBrkOrdID   RsFull_;
   TRsBrkOrdID   RsOdd_;
   TRsBrkOrdID   RsFixed_;
   TViewAreaMgr  VaMgr_;
   TRsVac*       RsVacFull_;
   TRsVac*       RsVacOdd_;
   TRsVac*       RsVacFixed_;

public:
   K_ctor  TRsOrdID (const char*);
   K_dtor ~TRsOrdID ();

   // 直接新增資料
   K_mf(void) Insert (const TTrxSegment&, const TBrkOrdID&, const TFixSysOrdID&);

   // 檢查是否有重覆的[卷商代號+委託書號],如果有重覆的則直接回傳失敗
   K_mf(bool) CheckInsert (const TTrxSegment&, const TBrkOrdID&, const TFixSysOrdID&);

   K_mf(TRsFixSoid*) FindRsFixSoid (const TTrxSegment&, const TBrkOrdID&);
   
   K_mf(TVACreater&) GetVaMgr ();
};

/////////////////////////////////////////////////////////////////////////////
// class TTwseFsOrdFlow::TOrdFdChk
/////////////////////////////////////////////////////////////////////////////
// > 委託內容比對
class TTwseFsOrdFlow::TOrdFdChk
{
   struct SConf
   {
      bool  ChkPrice_;
      bool  ChkAccount_;
      
      inline K_ctor SConf ()
         : ChkPrice_  (true)
         , ChkAccount_(true)
      {}
   };

   ETrxSegment            TrxSeg_;
   const TFsStkOrder&     FixOrd_;
   const SFixOfOrdField&  FixOrdField_;
   bool   IsChg_;
   bool   IsDel_;
   bool   IsQry_;
   SConf  Conf_;
   char   ErrField_[50];
   char   ErrDesc_ [128];

public:
   K_ctor TOrdFdChk (const TTrxSegment&, EFixOrdCmdID, const TFsStkOrder&, const SFixOfOrdField&);

   // 開始檢查
   K_mf(bool) DoChk ();
   
   // 取得錯誤訊息
   K_mf(const char*) GetErrDesc (const TFixSysOrdID&, const TBrkOrdID&);

private:
   K_mf(bool) ChkField ();
};

/////////////////////////////////////////////////////////////////////////////
} // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_TwseFsOrdFlowAux_h_)
