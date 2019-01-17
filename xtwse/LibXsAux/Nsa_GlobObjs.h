/////////////////////////////////////////////////////////////////////////////
#if !defined(_NsaGlobObjs_h_)
#define _NsaGlobObjs_h_
//---------------------------------------------------------------------------
#include "wise_ptr.h"
#include "ev_map.h"
#include "cc++/thread.h"
#include "libnsAux.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   class TVACreater;
   using namespace Kstl;

/////////////////////////////////////////////////////////////////////////////
namespace Nsa {

/////////////////////////////////////////////////////////////////////////////
// 基本型態
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TGlobObj : public ref_counter<>
{
public:
   virtual K_dtor ~TGlobObj ()  {}

   virtual K_mf(bool) KeepLast () const  { return false; }
};

/////////////////////////////////////////////////////////////////////////////
// TGlobObjHndBase
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TGlobObjHndBase
{
protected:
   std::string  ObjName_;
   TGlobObj*    GlobObj_;

public:
   inline  K_ctor  TGlobObjHndBase (const char* cpObjName) : ObjName_(cpObjName), GlobObj_(NULL)  {}
   virtual K_dtor ~TGlobObjHndBase ();

   //==============================================
   // TGlobObjHndBase virtual function
   virtual K_mf(TGlobObj*) ObjCreater () = 0;

   //==============================================
   // TGlobObjHndBase function
   K_mf(bool) Initialize ();
   K_mf(bool) QueryValid ();
   K_mf(void) Cleanup    ();
   //
   K_mf(TGlobObj*) Reset (TGlobObj*);

   inline K_mf(bool)               IsValid    () const  { return (GlobObj_ != NULL); }
   inline K_mf(const std::string&) GetObjName () const  { return ObjName_;           }
};


/////////////////////////////////////////////////////////////////////////////
// class TGlobObjFactory
/////////////////////////////////////////////////////////////////////////////
template <class RefObj>
struct TGlobObjFactory
{
   typedef RefObj  TRefObj;

   inline K_mf(TRefObj*) Create ()  { return new TRefObj; }
};


/////////////////////////////////////////////////////////////////////////////
// TGlobObjHnd
/////////////////////////////////////////////////////////////////////////////
template <class RefObj, class Factory = TGlobObjFactory<RefObj> >
class TGlobObjHnd : public TGlobObjHndBase
{
public:
   typedef TGlobObjHndBase  inherited;
   typedef RefObj           TRefObj;
   typedef Factory          TFactory;

protected:
   TFactory  Factory_;

public:
   inline K_ctor TGlobObjHnd (const char* cpObjName, bool autoQuery = false)
      : inherited(cpObjName)
   {
      if (autoQuery)
         QueryValid();
   }
   
   inline K_ctor TGlobObjHnd (const char* cpObjName, TFactory& f)
      : inherited(cpObjName)
      , Factory_ (f)
   {}

   //==============================================
   // overwrite TGlobObjHndBase virtual function
   K_mf(TGlobObj*) ObjCreater ()
   {
      TRefObj* refObj = Factory_.Create();

      return dynamic_cast<TGlobObj*>(refObj);
   }

   //==============================================
   // TGlobObjHnd function
   K_mf(bool) InitUseFactory (TFactory& a)
   {
      Factory_ = a;
      return inherited::Initialize();
   }

   //==============================================
   // TGlobObjHnd inline function
   inline K_mf(TRefObj*) GetPtr ()  { return dynamic_cast<TRefObj*>(GlobObj_);  }

   inline K_mf(TRefObj*) operator -> ()  { return dynamic_cast<TRefObj*>(GlobObj_);  }
};


/////////////////////////////////////////////////////////////////////////////
// TGlobObjHndDef
/////////////////////////////////////////////////////////////////////////////
template <const char* ObjName, class RefObj, class Factory = TGlobObjFactory<RefObj> >
class TGlobObjHndDef : public TGlobObjHnd<RefObj, Factory>
{
   typedef TGlobObjHnd<RefObj, Factory>  inherited;

public:
   inline K_ctor TGlobObjHndDef (bool autoQuery = false) : inherited(ObjName, autoQuery)  {}
};


/////////////////////////////////////////////////////////////////////////////
// class TGlobObjs
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TGlobObjs
{
public:
   typedef std::string               TRecsKey;
   typedef Kstl::wise_ref<TGlobObj>  TGlobObjPtr;

   typedef Kstl::ev_map<TRecsKey, TGlobObjPtr>  TRecs;
   typedef TRecs::iterator                      TRecsI;

protected:
   class TRecsVAC;

   TRecs       Recs_;
   ost::Mutex  Mutex_;
   TRecsVAC*   RecsVAC_;

public:
   K_ctor  TGlobObjs ();
   K_dtor ~TGlobObjs ();

   // 自動查詢物件是否存在, 如果不存在則自動 create
   K_mf(bool) Initialize (TGlobObjHndBase&);

   // 查詢物件是否存在, 如果不存在則失敗
   K_mf(bool) QueryValid (TGlobObjHndBase&);

   K_mf(void) Cleanup ();
   K_mf(void) Cleanup (TGlobObjHndBase&);

   K_mf(void) Destroy ();

   K_mf(TVACreater&) GetVAC ();

   static K_mf(TGlobObjs&) GetInst ();
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_NsaGlobObjs_h_)
