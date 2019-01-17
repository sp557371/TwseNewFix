//---------------------------------------------------------------------------
#ifndef SetsVACH
#define SetsVACH
//---------------------------------------------------------------------------
#include "char_ary.h"
#include "fixed_num.h"
#include "ev_map.h"
#include "ViewArea.h"
#include "ViewAreaMgr.h"
#include "TimerThread.h"
#include "StdRecorder.h"
//---------------------------------------------------------------------------
namespace Kway  {
namespace Tw    {
namespace Bx    {
/////////////////////////////////////////////////////////////////////////////
typedef Kstl::fixed_num<int>        TKey;
typedef Kstl::char_ary<char,30,' '> TName;
typedef Kstl::char_ary<char,40,' '> TData;
typedef Kstl::char_ary<char,40,' '> TMemo;
//---------------------------------------------------------------------------
struct TItemSet
{
  TName     Name_;
  TData     Data_;
  TMemo     Memo_;
};
//---------------------------------------------------------------------------
enum TItemField
{
  if_ID,
  if_Name,
  if_Data,
  if_Memo,
  if_EOF
};
//---------------------------------------------------------------------------
typedef Kstl::ev_map<TKey, TItemSet>    TItemSets;
//---------------------------------------------------------------------------
K_class TSetsVAC : public Kway::TVACreater
{
   typedef TVACreater   inherited;
   typedef TSetsVAC	    this_type;
protected:
   K_class TSetsFieldMaker
   {
     typedef TItemSets                TEvContainer;
     typedef TEvContainer::iterator   iterator;
     typedef size_t                   field_n_type;
   public:
     K_mf(const Kway::TFieldDesp&) GetFieldDesp (field_n_type) const;
     inline K_mf(field_n_type)     GetFieldCount() const { return if_EOF; }
     K_mf(std::string) Get (const iterator&, field_n_type) const;
     K_mf(void)        Put (TEvContainer&, iterator&, field_n_type, const std::string&);
   };
   TItemSets*  Recs_;  //Master 的container
   typedef TTViewArea<TItemSets, TSetsFieldMaker>     TSetsViewer;
public:
   K_ctor TSetsVAC(TItemSets& recs)
     : Recs_(&recs)
     {}
   K_mf(TaViewArea) CreateNewVA() {return TaViewArea(new TSetsViewer(Recs_));}
};
//---------------------------------------------------------------------------
/*
typedef Kstl::rr_inn_file<Kway::uint64, Kway::word>         TSetsInn;
typedef TXRecorder<TItemSets, Xinn_type, sizeof(TItemSet)>	TSetsRec;
*/
typedef TStdRecorder<TItemSets, TItemSet>       TSetsRec;
//---------------------------------------------------------------------------
K_class TSettingBase
{
  typedef Kway::TViewAreaMgr        TViewAreaMgr;
  typedef TSettingBase		        this_type;
//  TSetsInn*     Inn_;
  TSetsRec*     Recorder_;
  TSetsVAC*     VAC_;
protected:
  TItemSets*    Map_;
  //TViewAreaMgr  VAMgr_;
  const std::string     VAName_;
  const std::string     RecName_;
        std::string     LastErr_;
  K_mf(void) LoadDefault();
  K_mf(void) DestoryVAC(void);
  K_mf(void) CreateVAC (void); //建立遠端VAC
  Kway::TTimer<this_type> Timer_;
  K_mf(void)                OnTimer(TTimerBase*);
  virtual K_mf(void)        AssignItemStr(const char**&, const char**&, size_t&) = 0;
          K_mf(std::string) GetLastError() const { return LastErr_; }
public:
  TViewAreaMgr  VAMgr_;
  K_ctor     TSettingBase(const std::string& vaName, const std::string& recName);
  K_dtor    ~TSettingBase();
  K_mf(const TViewAreaMgr*) GetViewAreaMgr() const { return &VAMgr_; }
};
//---------------------------------------------------------------------------
};  // BX
};  // Tw
};  // Kway
//---------------------------------------------------------------------------
#endif
