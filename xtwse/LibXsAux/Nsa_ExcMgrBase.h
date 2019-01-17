/////////////////////////////////////////////////////////////////////////////
#if !defined(_NsaExcMgrBase_h_)
#define _NsaExcMgrBase_h_
//---------------------------------------------------------------------------
#include "char_ary.h"
#include "ViewAreaMgr.h"
#include "kutility.h"
#include "KTime.h"
#include "wise_ptr.h"
#include "tplns/Nsa_EvMap_0File.h"
#include "libnsAux.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// class TErrLevel
/////////////////////////////////////////////////////////////////////////////
// �p�G�o�䦳�s�W���خ�, TErrLevel::LevUseTypeStr_ �]�ݤ@�_�s�W
enum EErrLevel
{
   emlGreen,         // �@��T��
   emlYellow,        // ĵ�i
   emlRed,           // �Y�����~
   emlNotDef = 99,   // ���w�q
};
//---------------------------------------------------------------------------
LIBNSAUX_class TErrLevel : public Kstl::EnumT<EErrLevel>         //�B�z���Ū�CLASS
{
   typedef Kstl::EnumT<EErrLevel> inherited;

public:
   inline K_ctor TErrLevel (EErrLevel a = emlGreen) : inherited(a) {}

   K_mf(std::string) as_string () const;
   K_mf(void)        assign    (const std::string&);

public:
   static const char*  LevUseTypeStr_[3];
};


/////////////////////////////////////////////////////////////////////////////
// class TGroupKind
/////////////////////////////////////////////////////////////////////////////
enum EGroupKind
{                //�P�@�s�ժ��ƥ󤤬O�Ҧ����T���o�e�ӷ����O���`�~��ܥ��`(�p:����ҳs�u),�٬O�u�n��
   egkAll,       //�@�Өƥ�o�e�ӷ����`�Y�i(�p:�G�լۦP�����e�^�s�u),�b�o�بϥΤ@�Ӹs�����O���w�q�覡,
   egkOne,       //�ѨϥΪ̿��
};
//---------------------------------------------------------------------------
LIBNSAUX_class TGroupKind : public Kstl::EnumT<EGroupKind>         //�B�z���Ū�CLASS
{
   typedef Kstl::EnumT<EGroupKind> inherited;

public:
   inline K_ctor TGroupKind (EGroupKind a = egkAll) : inherited(a) {}

   K_mf(std::string) as_string () const;
   K_mf(void)        assign    (const std::string&);

public:
   static const char*  GroupKindStr_[2];
};


/////////////////////////////////////////////////////////////////////////////
// struct TErrCodeSet
/////////////////////////////////////////////////////////////////////////////
typedef Kstl::char_ary<char, 1, ' '>  TErrKind; // ���~����    �p'G'�N��g/w �Ҳ��ͪ����~, 'X'��ܥ���t�β��ͪ����~..
typedef Kstl::char_ary<char, 3, ' '>  TErrNum;  // ���~�N�X
//---------------------------------------------------------------------------
struct TErrCodeSet
{
   typedef TErrCodeSet this_type;

   TErrKind  ErrKind_;
   TErrNum   ErrNum_;

   inline K_ctor TErrCodeSet ()                 {}
   inline K_ctor TErrCodeSet (const char* str)  { assign(str); }

   K_mf(void) assign (const char* str)
   {
      ErrKind_.clear();
      ErrNum_ .clear();

      if (str != NULL) {
         size_t len = strlen(str);
         if (len >= 1)  ErrKind_.assign(str);
         if (len >= 2)  ErrNum_ .assign(str+1);
      }
   }

   inline K_mf(std::string) as_string () const  { return ErrKind_.as_string()+ErrNum_.as_string(); }

   inline K_mf(bool) operator == (const this_type& a) const  { return (ErrKind_ == a.ErrKind_ && ErrNum_ == a.ErrNum_); }
   inline K_mf(bool) operator != (const this_type& a) const  { return !operator == (a);                                 }
};
//---------------------------------------------------------------------------
// �o�ةw�q�t�Ϊ��A�����c�����(���x�s),�i�b���ݥX�ݸs�ժ����A�θs�դ��U�o�e�ӷ������A
//
//---------------------------------------------------------------------------
typedef Kstl::char_ary<char,4,' '>             TObjID;
typedef Kstl::char_ary<char,40,' '>            TEvMsgKey;
//---------------------------------------------------------------------------
struct SMsgValue
{
   typedef Kstl::char_ary<char,60,' '>  TMsg;

   TErrLevel     MsgErrLevel_;
   TErrCodeSet   MsgErrCode_;
   TMsg          Msg_;
   TKDateTimeMS  MsgTime_;
   std::string   MsgDetail_;
};
//---------------------------------------------------------------------------
typedef Kstl::ev_map<TEvMsgKey, SMsgValue>  TEvMsgMap;
//---------------------------------------------------------------------------
struct SObjStat     //�O���ثe�U���󪬺A�����c
{
   TErrLevel    ObjErrLevel_;
   TEvMsgMap    MsgMap_;
};
//---------------------------------------------------------------------------
typedef Kstl::ev_map<TObjID, SObjStat>   TObjMap;  //�O���ثe�t�Ϊ��A����ƪ�
//---------------------------------------------------------------------------
LIBNSAUX_class TMsgVACr : public TVACreater    //��2�h��VAC
{
   typedef TVACreater  inherited;
   typedef TMsgVACr          this_type;
   //---------------------------------------------------------------------------
   LIBNSAUX_class TMsgFieldMaker                              //��2�h��������
   {
      typedef TEvMsgMap                TEvContainer;
      typedef TEvContainer::iterator   iterator;
      typedef size_t                   field_n_type;
   public:
          K_mf(const TFieldDesp&) GetFieldDesp (size_t) const;
   inline K_mf(size_t)            GetFieldCount()       const { return 5; }
          K_mf(std::string) Get(const iterator&, size_t) const;
          K_mf(void)        Put(TEvContainer&, iterator&, size_t, const std::string&) {}
   };
   //---------------------------------------------------------------------------
public:
   typedef TTViewArea<TEvMsgMap,TMsgFieldMaker> TMsgViewer;    //��2�h��Viewer
           K_ctor  TMsgVACr() {}
   virtual K_dtor ~TMsgVACr() {}
   K_mf(TaViewArea) CreateNewVA()
   {
      TViewControls   vcs;
      vcs.set(vco_Erase);
      TMsgFieldMaker a;
      return TaViewArea(new TMsgViewer(0, a, vcs));
   }
};
//---------------------------------------------------------------------------
LIBNSAUX_class TObjVAC : public TVACreater              //��1�h��VAC
{
   typedef TVACreater   inherited;
   typedef TObjVAC      this_type;
   TMsgVACr             DetailCreater_;
   //---------------------------------------------------------------------------
   LIBNSAUX_class TObjFieldMaker
   {
     typedef TObjMap                  TEvContainer;
     typedef TEvContainer::iterator   iterator;
     typedef size_t                   field_n_type;
   public:
          K_mf(const TFieldDesp&) GetFieldDesp (field_n_type) const;
   inline K_mf(field_n_type)      GetFieldCount() const {return 2;}
          K_mf(std::string)       Get          (const iterator&, field_n_type) const;
          K_mf(void)              Put          (TEvContainer&, iterator&, field_n_type, const std::string&) {}
   };
   //---------------------------------------------------------------------------
   TObjMap*     Recs_;           //��1�h�� Map
   typedef TTViewArea<TObjMap, TObjFieldMaker> TObjViewer;
public:
            K_ctor  TObjVAC(TObjMap* map) : Recs_(map) {}
   virtual  K_dtor ~TObjVAC() {}
   K_mf(TaViewArea) CreateNewVA()
   {
      TViewControls   vcs;
      vcs.set(vco_Erase);
      TObjFieldMaker a;
      return TaViewArea(new TObjViewer(Recs_, a, vcs));
   }
   K_mf(size_t)      GetDetails          (const char**&);
   K_mf(TVACreater*) GetDetailVACreater  (size_t);
   K_mf(TVACreater*) OnMasterViewRowMoved(TViewArea&, TViewArea*&, size_t);
};
//---------------------------------------------------------------------------
// �o�ةw�q�s�ղΦX�ƥ󪬺A�覡���x�s���c�����
//
//---------------------------------------------------------------------------
typedef Kstl::char_ary<char, 40, ' '>  TGroupRemark;
//---------------------------------------------------------------------------
struct SGroupDef
{
   TGroupKind   GroupKind_;
   TGroupRemark Remark_;
};
//---------------------------------------------------------------------------
typedef Kstl::ev_map<TObjID, SGroupDef>   TGroupDef;
typedef TEvMapReco<TGroupDef, SGroupDef>  TGDRecorder;
//---------------------------------------------------------------------------
LIBNSAUX_class TGroupDefFieldMaker
{
    typedef TGroupDef                TEvContainer;
    typedef TEvContainer::iterator   iterator;
    typedef size_t                   field_n_type;
public:
        K_mf(const TFieldDesp&) GetFieldDesp (field_n_type) const;
 inline K_mf(field_n_type)      GetFieldCount() const { return 3; }
        K_mf(std::string)       Get (const iterator&, field_n_type) const;
        K_mf(void)              Put (TEvContainer&, iterator&, field_n_type, const std::string&);
};
//---------------------------------------------------------------------------
typedef TTViewArea<TGroupDef, TGroupDefFieldMaker> TGroupDefViewer;
//---------------------------------------------------------------------------
LIBNSAUX_class TGroupDefVACr : public TVACreater
{
    typedef TVACreater     inherited;
    typedef TGroupDefVACr  this_type;
private:
   TGroupDef*    GroupDef_;
public:
   K_ctor TGroupDefVACr(TGroupDef* gd) : GroupDef_(gd) {}
   K_mf(TaViewArea) CreateNewVA()
   {
      TViewControls   vcs;
      vcs.set(vco_Insert);             //�]�w�i�s�W
      vcs.set(vco_Erase);              //�]�w�i�R��
      TGroupDefFieldMaker a;
      return TaViewArea(new TGroupDefViewer(GroupDef_, a, vcs));
   }
};


/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif


///////////////////////////////////////////////////////////////////////
// class TExcMgrObj
///////////////////////////////////////////////////////////////////////
class TExcMgrObj
{
public:
   virtual K_dtor ~TExcMgrObj ()  {}

   virtual K_mf(bool) DoAlert (const TErrLevel&,
                               const TErrCodeSet&,
                               const TErrLevel&,
                               const std::string& msg,
                               const std::string& msgOwner,
                               const std::string& msgDetail) = 0;
};


///////////////////////////////////////////////////////////////////////
// class TExcMgrBase
///////////////////////////////////////////////////////////////////////
LIBNSAUX_class TExcMgrBase : public Kstl::ref_counter<unsigned>
{
public:
   typedef wise_ref<TExcMgrBase>  TExcMgrBaseRef;

private:
   typedef std::vector<TExcMgrObj*>     TMgrObjs;
   typedef Kstl::ref_counter<unsigned>  inherited;

protected:
   TExcMgrObj*  ExcMgrObj_;
   TMgrObjs                MgrObjs_;
   TErrLevel               SysState_;
   TObjMap                 ObjMap_;
   TObjVAC*                ObjVA_;
   TGroupDef               GroupDef_;
   TGroupDefVACr*          GroupDefVA_;
   TGDRecorder*            GDRecorder_;
   TViewAreaMgr VAMgr_;
   bool                    DoLog_;

public:
           K_ctor     TExcMgrBase() : ExcMgrObj_(NULL) {}
   virtual K_dtor    ~TExcMgrBase() {}
           K_mf(void) RegExcMgrObj(TExcMgrObj*);
           K_mf(void) DelExcMgrObj(TExcMgrObj*);

   //==============================================
   // TExcMgrBase virtual function
   virtual K_mf(void) Alert (const char*,
                             const std::string& msgOwner  = std::string(),
                             const std::string& msgDetail = std::string());

   virtual K_mf(void) Alert (const TErrCodeSet&,
                             const std::string& msgOwner  = std::string(),
                             const std::string& msgDetail = std::string());

   virtual K_mf(std::string) GetFuncStatus (const TErrCodeSet& ercode)  { return std::string(); }

   inline  K_mf(TViewAreaMgr&) GetVAMgr ()  { return VAMgr_; }

   static K_mf(TExcMgrBaseRef) GetEventMgr ();

protected:
   //==============================================
   // TExcMgrBase virtual function
   virtual K_mf(void) WriteEvLog (const TObjID&,
                                  const TErrCodeSet&,
                                  const TErrLevel&,
                                  const std::string& msg,
                                  const std::string& msgOwner,
                                  const std::string& msgDetail) = 0;

   virtual K_mf(TErrLevel) GetSysState (const TErrCodeSet&,
                                        const TErrLevel&,
                                        const TObjID&,
                                        const std::string& msg,
                                        const std::string& msgOwner,
                                        const std::string& msgDetail);

   virtual K_mf(bool)      GetErrDef    (const TErrCodeSet&
                                       , TErrLevel&
                                       , TObjID&
                                       , std::string& errMsg)
      { return false;
      }
   virtual K_mf(TGroupKind)GetGroupKind (const TObjID&);

};
//---------------------------------------------------------------------------
typedef TExcMgrBase::TExcMgrBaseRef  TExcMgrBaseRef;

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_NsaExcMgrBase_h_)
