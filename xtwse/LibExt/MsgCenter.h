//---------------------------------------------------------------------------
#ifndef MsgCenterH
#define MsgCenterH
//---------------------------------------------------------------------------
#include "char_ary.h"
#include "ViewAreaMgr.h"
#include "StdRecorder.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kstl;
//---------------------------------------------------------------------------
typedef Kstl::char_ary<char, 2,' '>           TMsgKey;
struct SMsg{
   Kstl::char_ary<char,80,' '>     Msg_;
   bool                            Error_;
   Kstl::char_ary<char,19,' '>     Remark_;
   K_ctor SMsg():Error_(false){};
};
typedef TDMapValue<SMsg>                      TDMsg;
typedef TDMap<TMsgKey, TMsgKey, TDMsg>        TMsgMapD;
typedef Kstl::ev_map<TMsgKey, TMsgMapD>       TMsgMap;
typedef TMsgMap::iterator                     TMsgi;
typedef TMsgMap::updater                      TMsgu;
typedef TMsgMapD::iterator                    TMsgDi;
typedef TMsgMapD::updater                     TMsgDu;
//---------------------------------------------------------------------------
K_class TMsgDFieldMaker
{
 public:
    K_mf(const Kway::TFieldDesp&) GetFieldDesp(size_t) const;
    K_mf(size_t) GetFieldCount() const { return 3; }
    K_mf(std::string) Get(const TMsgDi&, size_t) const;
    K_mf(void) Put(TMsgMapD&, TMsgDi&, size_t, const std::string&);
};
//---------------------------------------------------------------------------
typedef Kway::TTViewArea<TMsgMapD,TMsgDFieldMaker> TMsgDViewer;
//---------------------------------------------------------------------------
K_class TMsgDVACr:public Kway::TVACreater
{
 typedef Kway::TVACreater inherited;
 typedef TMsgDVACr    this_type;

public:
   K_mf(Kway::TaViewArea) CreateNewVA()
   {
      TViewControls   vcs;
      vcs.set(vco_Insert);
      vcs.set(vco_Erase);
      TMsgDFieldMaker a;
      return Kway::TaViewArea(new TMsgDViewer(0,a,vcs));
   }
};
//---------------------------------------------------------------------------
K_class TMsgVAC:public Kway::TVACreater
{
   typedef Kway::TVACreater     inherited;
   typedef TMsgVAC              this_type;
   TMsgDVACr                    DetailCreater_;

   K_class TMsgFieldMaker   
   {
     typedef TMsgMap                  TEvContainer;
     typedef TEvContainer::iterator   iterator;
     typedef size_t                   field_n_type;

     public:
       K_mf(const Kway::TFieldDesp&) GetFieldDesp(field_n_type n) const;

       inline K_mf(field_n_type) GetFieldCount() const {return 1;}

       K_mf(std::string) Get (const iterator& i, field_n_type fieldNo) const;

       K_mf(void) Put (TEvContainer& c,iterator& iter,field_n_type fieldNo,const std::string& str){};

   };
   TMsgMap* Recs_;

 public:
   K_ctor TMsgVAC(TMsgMap* map):Recs_(map){}
   typedef Kway::TTViewArea<TMsgMap,TMsgFieldMaker> TMsgViewer;
   K_mf(Kway::TaViewArea) CreateNewVA()
   {
      return Kway::TaViewArea(new TMsgViewer(Recs_));
   }
   K_mf(size_t) GetDetails(const char**& a);
   K_mf(Kway::TVACreater*) GetDetailVACreater (size_t detailID);
   K_mf(Kway::TVACreater*) OnMasterViewRowMoved (Kway::TViewArea& masterVA, Kway::TViewArea*& detailVA, size_t detailID);
};
//---------------------------------------------------------------------------
K_class TMsgTable 
{
private:
   typedef TMsgTable               this_type;
   typedef TStdRecorder2d<TMsgMap, TMsgMapD> TMsgRecorder;
   TMsgMap*                        MsgMap_;
   TMsgRecorder*                   Recorder_;
   TMsgVAC*                        MsgVA_;
   bool                            Persist_;
public:
   K_ctor                        TMsgTable(bool chgable = true);
   K_dtor                        ~TMsgTable();
   K_mf(std::string)             GetMsg     (TMsgKey, TMsgKey, bool& ErrCnt, const char* remark="");
   K_mf(std::string)             GetMsg     (const char*, bool& ErrCnt, const char* remark="");
   K_mf(TMsgMap*)                GetMap()   {return MsgMap_;}
   K_mf(TMsgVAC*)                GetVA()    {return MsgVA_;}
};
//---------------------------------------------------------------------------
K_fn(TMsgTable*)             GetMsgCenter  ();
K_fn(std::string)            GetErrMsg     (TMsgKey, TMsgKey, const char* remark="");
K_fn(std::string)            GetErrMsg     (TMsgKey, TMsgKey, bool& ErrCnt, const char* remark="");
K_fn(std::string)            GetErrMsg     (const char*, const char* remark="");
K_fn(std::string)            GetErrMsg     (const char*, bool& ErrCnt, const char* remark="");
K_fn(void)                   SetMsgCenter  (TMsgTable*);
//---------------------------------------------------------------------------
#endif

