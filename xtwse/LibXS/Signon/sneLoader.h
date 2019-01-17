//
// Singon Extension Module Loader
//
// 版本: $Id: sneLoader.h,v 1.4 2006/04/04 03:53:06 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef sneLoaderH
#define sneLoaderH
//---------------------------------------------------------------------------
#include "ModuleLoader.h"
#include "SignonSvr_DC.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
//---------------------------------------------------------------------------
/*
   Signon Extension Module 簡稱 sne
*/
/////////////////////////////////////////////////////////////////////////////
class TsneLoader : public TModuleLoader
{
   typedef TModuleLoader   inherited;
   typedef TsneLoader      this_type;
   class TsneHandle : public TModuleHandle {
      typedef TModuleHandle  inherited;
   public:
      TSignonSvr_DataCenter*  DC_;
      TsneStartObj            sneStartObj_;
      K_ctor  TsneHandle  ();
      K_dtor ~TsneHandle  ();
   protected:
      K_mf(bool)  CheckLoadLib  (const std::string& libName, bool isStart);
   };
   wise_ptr<TsneHandle>    sneHandle_;
   TSignonSvr_DataCenter*  DC_;
   virtual K_mf(const TModuleHandle*)  GetModuleHandle    () const   { return sneHandle_.get(); }
   virtual K_mf(TModuleHandle&)        FetchModuleHandle  ()         { sneHandle_.testnew();
                                                                       sneHandle_->DC_ = DC_;
                                                                       return*(sneHandle_.get()); }
public:
   virtual K_mf(void)  ResetModuleHandle  ();

           K_mf(void)  SetDC (TSignonSvr_DataCenter&);
   inline  K_mf(void)  swap  (TsneLoader& a)  { inherited::swap(a);
                                                sneHandle_.swap(a.sneHandle_);
                                                std::swap(DC_,a.DC_); }
};
/////////////////////////////////////////////////////////////////////////////
namespace for_bcb_bug {//不加上這段無用的定義,會造成 BCB compiler 說 TModuleRecorder 的定義有問題!!
struct TBCB_bug_filler {};
typedef TSimpleMap_AddRoomKey<int,TBCB_bug_filler,TModuleRecInn::rr_key>::TEvRecs  TBugRecs;
}
//---------------------------------------------------------------------------
//typedef TSimpleRecorder_AddRoomKey<TModuleRecKey, TsneLoader, TModuleRecInn, sizeof(TModuleRecBase)>
//        TsneRecorder;
K_typedef_TSimpleRecorder_AddRoomKey(TsneRecorder, TModuleRecKey, TsneLoader, TModuleRecInn, sizeof(TModuleRecBase));
//---------------------------------------------------------------------------
//資料顯示方法
class TsneFieldMaker : public TModuleRecFieldMakerBase
{
   typedef TModuleRecFieldMakerBase    inherited;
   TSignonSvr_DataCenter*  DC_;
public:
   typedef TsneRecorder::TRecs         TEvContainer;
   typedef TEvContainer::iterator      iterator;

   K_ctor             TsneFieldMaker   (TSignonSvr_DataCenter& dc)  : DC_(&dc) {}
   K_mf(std::string)  Get  (const iterator& i, field_n_type fieldNo) const
                                 { return inherited::Get(i->first, i->second, fieldNo); }
   K_mf(void)         Put  (TEvContainer&, iterator&, field_n_type, const std::string&);
};
/////////////////////////////////////////////////////////////////////////////
class TsneMgr : public TModuleMgr<TsneRecorder, TsneFieldMaker>
{
   typedef TModuleMgr<TsneRecorder,TsneFieldMaker>  inherited;
public:
   K_ctor  TsneMgr  (TSignonSvr_DataCenter&);

private:
   K_mf(void)            LoadLib           (TRecs::updater::reference);
   K_mf(TsneFieldMaker)  CreateFieldMaker  ()  { return TsneFieldMaker(*DC_); }
   TSignonSvr_DataCenter*  DC_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Signon
} // namespace Kway
//---------------------------------------------------------------------------
#endif

