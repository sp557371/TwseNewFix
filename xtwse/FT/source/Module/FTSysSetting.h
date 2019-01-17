//---------------------------------------------------------------------------
#ifndef SysSettingH
#define SysSettingH
//---------------------------------------------------------------------------
#include "ev_map.h"
#include "char_ary.h"
#include "fixed_num.h"
#include "TwStk.h"
#include "ViewArea.h"
#include "ViewAreaMgr.h"
#include "FTJobDef.h"
#include "StdRecorder.h"
#include "stdSettingP.h"
#include "ExtDef.h"
//---------------------------------------------------------------------------
namespace Kway  {
namespace Tw    {
namespace Bx    {
/////////////////////////////////////////////////////////////////////////////
typedef Kstl::fixed_num<int>        TKey;
typedef Kstl::char_ary<char,30,' '> TName;
typedef Kstl::char_ary<char,40,' '> TData;
typedef Kstl::char_ary<char,40,' '> TMemo;
typedef Kway::Tw::Bx::FT::TFileCode TFileCode;
//---------------------------------------------------------------------------
struct TFTSysSet
{
  TName     Name_;
  TData     Data_;
  TMemo     Memo_;
};
//---------------------------------------------------------------------------
enum TSysField
{
  sf_ID,
  sf_Name,
  sf_Data,
  sf_Memo,
  sf_EOF
};
//---------------------------------------------------------------------------
enum TSysSettingItem
{
  ssi_TempSave=1,   // Temp Save Path
  ssi_SaveTo,       // SaveTo
  ssi_RptFile,	    // Pmach filename
  ssi_LineLog,      // Line Status LogPath
  ssi_FTInterval,   // Interval between 2 FT jobs(second)
  ssi_TimeoutInt,   // Time out interval(minute)
};
//---------------------------------------------------------------------------
typedef Kstl::ev_map<TKey, TFTSysSet>   TFTSysSets;
//---------------------------------------------------------------------------
K_class TFTSysSetsVAC : public Kway::TVACreater
{
   typedef TVACreater       inherited;
   typedef TFTSysSetsVAC    this_type;
   K_class TFTSysSetsFieldMaker
   {
     typedef TFTSysSets               TEvContainer;
     typedef TEvContainer::iterator   iterator;
     typedef size_t                   field_n_type;
   public:
     K_mf(const Kway::TFieldDesp&) GetFieldDesp (field_n_type) const;
     inline K_mf(field_n_type)     GetFieldCount() const { return sf_EOF; }
     K_mf(std::string) Get (const iterator&, field_n_type) const;
     K_mf(void)        Put (TEvContainer&, iterator&, field_n_type, const std::string&);
   };
   TFTSysSets*  Recs_;  //Master 的container
   typedef TTViewArea<TFTSysSets, TFTSysSetsFieldMaker>     TFTSysSetsViewer;
public:
   K_ctor TFTSysSetsVAC(TFTSysSets& recs)
     : Recs_(&recs)
     {}
   K_mf(TaViewArea)        CreateNewVA() {return TaViewArea(new TFTSysSetsViewer(Recs_));}
};
//---------------------------------------------------------------------------
/*
typedef Kstl::rr_inn_file<Kway::uint64, Kway::word>           TFTInn;
typedef TXRecorder<TFTSysSets, Xinn_type, sizeof(TFTSysSet)>  TFTSysRec;
*/
typedef TStdRecorder<TFTSysSets, TFTSysSet>             TFTSysRec;
typedef Kstl::fixed_num<Kway::word>                     TRecLength;
//---------------------------------------------------------------------------
struct SFTMode{
   bool                          IsAppend_;
   TRecLength                    RecLen_;
   TRecLength                    DelayTime_; // 20070709 for block trade
   TMemo                         Remark_;
   bool                          Ackless_;   // 20080701 不需等待回覆即可傳送下一筆作業 ex.F06
#define FTsz (128-sizeof(bool)*2-sizeof(TRecLength)*2-sizeof(TMemo)) 
   Kstl::char_ary<char,FTsz,' '> Filler_;    // 保留給將來使用,增加新
   K_ctor SFTMode(){ IsAppend_ = false; Ackless_ = false;}
};
//---------------------------------------------------------------------------
typedef Kstl::ev_map<TFileCode, SFTMode>   TFTMode;
typedef TStdRecorder<TFTMode, SFTMode>     TFTModeRec;
//---------------------------------------------------------------------------
K_class TFTModeVAC : public Kway::TVACreater
{
   typedef TVACreater       inherited;
   typedef TFTModeVAC    this_type;
   K_class TFTModeFieldMaker
   {
     typedef TFTMode                  TEvContainer;
     typedef TEvContainer::iterator   iterator;
     typedef size_t                   field_n_type;
   public:
     K_mf(const Kway::TFieldDesp&) GetFieldDesp (field_n_type) const;
     inline K_mf(field_n_type)     GetFieldCount() const { return 6; }
     K_mf(std::string) Get (const iterator&, field_n_type) const;
     K_mf(void)        Put (TEvContainer&, iterator&, field_n_type, const std::string&);
   };
   TFTMode*  Recs_;  //Master 的container
   typedef TTViewArea<TFTMode, TFTModeFieldMaker>     TFTModeViewer;
public:
   K_ctor TFTModeVAC(TFTMode& recs)
     : Recs_(&recs)
     {}
   K_mf(TaViewArea)        CreateNewVA() {return TaViewArea(new TFTModeViewer(Recs_));}
};
//---------------------------------------------------------------------------
K_class TFTSysSetting
{
   typedef Kway::TViewAreaMgr        TViewAreaMgr;
//   TFTInn*           Inn_;
   TFTSysRec*        Recorder_;
   TFTSysSetsVAC*    VAC_;
   TFTSysSets*       Map_;
   TFTMode           FTMode_;
   TFTModeRec*       FTModeRec_;
   TFTModeVAC*       FTModeVA_;
   TViewAreaMgr      VAMgr_;
   TSettingPage*   SysInit_;
   Kway::byte      HostID_;
   bool            UseHostID_;
   K_mf(void) LoadDefault();
   K_mf(void) CreateVAC (void); //建立遠端VAC
   K_mf(void) DestoryVAC(void);
protected:
public:
   K_ctor     TFTSysSetting();
   K_dtor    ~TFTSysSetting();
   K_mf(const TViewAreaMgr*) GetViewAreaMgr() const { return &VAMgr_; }
   K_mf(std::string) GetSetting(TSysSettingItem) const;
   K_mf(bool)        IsAppendMode(TFileCode);
   K_mf(size_t)      GetRecLength(TFileCode);
   K_mf(size_t)      GetDelayTime(TFileCode); // 20070709
   K_mf(bool)        IsAckless(TFileCode);    // 20080701
   K_mf(std::string) GetFormatedStr(const std::string&, const std::string&, KStk::EMarket) const;
};
/////////////////////////////////////////////////////////////////////////////
K_fn(TFTSysSetting&) GetSysSetting();
//---------------------------------------------------------------------------
K_fn(void) MakePath(std::string& path);
//---------------------------------------------------------------------------
};  // Bx
};  // Tw
};  // Kway
#endif
