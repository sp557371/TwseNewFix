//---------------------------------------------------------------------------
#ifndef FTNamingH
#define FTNamingH
//---------------------------------------------------------------------------
#include "kutility.h"
#include "fixed_num.h"
#include "ev_map.h"
#include "ViewArea.h"
#include "ViewAreaMgr.h"
#include "inn_file.h"
#include "SimpleRecorder.h"
#include "bxFT.h"
#include "StdRecorder.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef Kway::byte ENamingMode;
   enum ENamingMode_t
#else
   enum ENamingMode
#endif
{
   nm_Head,     // 總公司
   nm_Branch,   // 分公司
   nm_FTR,      // 流水號
   nm_End,      // 定義結束
} K_EnumPacked;
K_class TNamingMode : public Kstl::EnumT<ENamingMode>
{
   typedef Kstl::EnumT<ENamingMode>   inherited;
public:
   inline K_ctor      TNamingMode(ENamingMode a = nm_Head)  : inherited(a) {}
   K_mf(std::string)  as_string  () const;
   K_mf(void)         assign     (const std::string&);
};
//---------------------------------------------------------------------------
//typedef Kstl::rr_inn_file<Kway::uint64, Kway::word> TNamInn;
//---------------------------------------------------------------------------
struct FTNaming     // 檔案命名原則
{
    TNamingMode NamingMode_;    // 命名原則 // 總公司, 分公司, FTR
};
//---------------------------------------------------------------------------
typedef Kstl::ev_map<TFileCode, const FTNaming>             TNamingMap;
//typedef TXRecorder<TNamingMap, TNamInn, sizeof(FTNaming)>   TNamingInn;
typedef TStdRecorder<TNamingMap, FTNaming>                  TNamingInn;
//---------------------------------------------------------------------------
K_class TNamingVAC : public Kway::TVACreater
{
   typedef Kway::TVACreater inherited;
   typedef TNamingVAC       this_type;
   K_class TNamingFieldMaker
   {
      typedef TNamingMap                TEvContainer;
      typedef TEvContainer::iterator    iterator;
      typedef size_t                    field_n_type;
   public:
      inline K_mf(field_n_type)     GetFieldCount ()                const { return 2; }
      K_mf(const Kway::TFieldDesp&) GetFieldDesp  (field_n_type n)  const;
      K_mf(std::string)             Get (const iterator& i, field_n_type fieldNo) const;
      K_mf(void)                    Put (TEvContainer& c,iterator& iter, field_n_type fieldNo, const std::string& str);
   };
   TNamingMap*  Recs_;
public:
   K_ctor TNamingVAC(TNamingMap& recs) : Recs_(&recs) {}
   typedef Kway::TTViewArea<TNamingMap, TNamingFieldMaker>  TNamingViewer;
   K_mf(Kway::TaViewArea)  CreateNewVA  ()
    { return Kway::TaViewArea(new TNamingViewer(Recs_)); }
};
//---------------------------------------------------------------------------
K_class TNamingCenter
{
    typedef Kway::TViewAreaMgr  TViewAreaMgr;
//    TNamInn*            Inn_;
    TNamingInn*         Recorder_;
    TNamingVAC*         VAC_;
    TNamingMap*         Map_;
    Kway::TViewAreaMgr  VAMgr_;
    K_mf(void) CreateVAC (void); //建立遠端VAC
    K_mf(void) DestoryVAC(void);
public:
    K_ctor TNamingCenter ();
    K_dtor ~TNamingCenter();
    K_mf(const TViewAreaMgr*) GetViewAreaMgr() const { return &VAMgr_; }
};
//---------------------------------------------------------------------------
K_fn(const TNamingCenter&) GetNaming()
{
  static TNamingCenter ftNaming;
  return ftNaming;
}
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif

