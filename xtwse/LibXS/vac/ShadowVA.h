//
// ViewArea Client - Shadow ViewArea
//
// 版本: $Id: ShadowVA.h,v 1.2 2005/08/09 05:48:08 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef ShadowVAH
#define ShadowVAH
//---------------------------------------------------------------------------
#include "ViewAreaMgr.h"
#include "Pk/PkSesVA.h"
#include "ev_vector.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Pk {
//---------------------------------------------------------------------------
class TSesVAC;
class TSesVAAck;
/////////////////////////////////////////////////////////////////////////////
class TFieldDatas : public Kway::TStrings {
   typedef Kway::TStrings  inherited;
public:
   K_ctor      TFieldDatas  ()                        : IsLoading_(false) {}
   K_ctor      TFieldDatas  (const Kway::TStrings& a) : inherited(a), IsLoading_(false) {}
   K_mf(void)  operator=    (const Kway::TStrings& a) { inherited::operator=(a); }
   bool mutable   IsLoading_;
};
typedef Kstl::ev_vector<std::vector<TFieldDatas> >   TShadowRecs;
//---------------------------------------------------------------------------
class TFieldInfos;

class TFieldInfo : protected TFieldDesp {
   friend class TFieldInfos;
   enum TLoadState {
      ls_Notyet,
      ls_Loadding,
      ls_Loadded,
   };
   typedef TVAGetEditProp::TEditStyle  TEditStyle;
   struct TEnums {
      TEditStyle     Style_;
      TStrings       Strs_;
      std::string    Key_;
      TLoadState     LoadState_;
   };
   TEnums         Enums_;
   std::string    TitleStr_;

   K_mf(TFieldDesp&)  GetFD ()    { return *this; }
   K_mf(TFieldInfo&)  operator=   (const TFieldDesp&);
public:
   K_ctor             TFieldInfo  ();
   K_ctor             TFieldInfo  (const TFieldInfo&);
   K_ctor             TFieldInfo  (const TFieldDesp&, TEditStyle, TVAGetEditProp*);
   K_mf(TFieldInfo&)  operator=   (const TFieldInfo&);
   K_mf(void)         SetEnums    (const TStrings&);

   inline K_mf(const TFieldDesp&)  GetFieldDesp () const   { return *this; }
};
//---------------------------------------------------------------------------
typedef std::vector<TFieldInfo>  TFieldInfoRecs;
class TFieldInfos : public TFieldInfoRecs, public TVAGetEditProp
{
   Pk::TSesVAC*   Ses_;
   int            Layer_;

   K_mf(TEditStyle)  GetEnums  (const TViewArea&, const TViewCursor*, size_t fieldNo, TStrings*, bool& selectOnly);
   K_mf(bool)        CanEdit   (const TViewArea&, const TViewCursor*, size_t fieldNo);
public:
   K_ctor  TFieldInfos  (Pk::TSesVAC& ses, int layer)
            : Ses_(&ses),
              Layer_(layer)
            { }

   inline K_mf(Pk::TSesVAC&)  GetSes   ()   { return *Ses_; }
   inline K_mf(TSesVALayer)   GetLayer ()   { return static_cast<TSesVALayer>(Layer_); }
};
/////////////////////////////////////////////////////////////////////////////
struct TShadowVACr : protected TVAEvHandler,
                     protected TShadowRecs,  //請注意繼承順序,必須Recs先, TVACreater後
                     public    TVACreater    //因為 TVACreater 會產生 VA, 而 VA 會參考到 Recs
{                                            //所以將 TVACreater 放在之後,在解構時才會比Recs先被消滅
   K_ctor  TShadowVACr  (Pk::TSesVAC&, int layer);
   K_dtor ~TShadowVACr  ();

   //override for TVAEvHandler
   K_mf(void)  OnViewRowMoved  (TViewArea&, const TViewCursor*, const TViewCursor*);
   K_mf(void)  OnViewBfDestroy (TViewArea&);

   //override for TVACreater
   K_mf(TaViewArea)  CreateNewVA           ();
   K_mf(size_t)      GetDetails            (const char**&);
   K_mf(TVACreater*) GetDetailVACreater    (size_t detailID);

   //some services for TSesVAC
          K_mf(void)          OnPkAck      (const Pk::TSesVAAck&);
          K_mf(void)          ClearAll     ();
          K_mf(TShadowVACr*)  NewNextLayer ();
   inline K_mf(TShadowVACr*)  GetNextLayer ()   { return NextLayerVACr_; }

   //some services for TShadowVA, TShadowVACursor
   inline K_mf(TShadowRecs&)  GetContainer  ()  { return *this; }
   inline K_mf(Pk::TSesVAC&)  GetSes        ()  { return FieldInfos_.GetSes();   }
   inline K_mf(TSesVALayer)   GetLayer      ()  { return FieldInfos_.GetLayer(); }

   /** 設定將要建立的 VA 的 IdName.
   */
   inline K_mf(const std::string&)  GetVAIdName  () const                  { return VAIdName_; }
   inline K_mf(void)                SetVAIdName  (const std::string& v)    { VAIdName_ = v;
                                                                             if(VA_)   VA_->SetIdName(v); }
protected:
   K_mf(bool)  IsInfoReady  ();

   TShadowVACr*   NextLayerVACr_;
   Pk::TStrs      DetailStrs_;
   size_t         RowCount_;
   TFieldInfos    FieldInfos_;
   TViewArea*     VA_;
   TViewControls  ViewControls_;
   std::string    VAIdName_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Pk
} // namespace Kway
//---------------------------------------------------------------------------
#endif

