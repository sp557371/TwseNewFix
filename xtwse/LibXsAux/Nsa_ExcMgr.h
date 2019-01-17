/////////////////////////////////////////////////////////////////////////////
#if !defined(_NsaExcMgr_h_)
#define _NsaExcMgr_h_
//---------------------------------------------------------------------------
#include "TimerThread.h"
#include "Nsa_UtilComm.h"
#include "Nsa_ExcMgrBase.h"
#include "tplnsold/Nse2_EvMap_2k1d.h"
#include "libnsAux.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// struct SEvent
/////////////////////////////////////////////////////////////////////////////
struct SEvent                                           //��2�h Value �� Structure
{
   TErrLevel                      EvLevel_;               //����
   TObjID                         ObjID_;                 //�ƥ�s�էO
   TBoolChar                      EvLogON_;               //�g�J�ƥ��˵������\��}��
   TBoolChar                      NetSendON_;             //NetSend���\��}��
   Kstl::char_ary<char, 60, ' '>  EvMsg_;                 //�T��
   Kstl::char_ary<char, 30, ' '>  Remark_;                //�Ƶ�

   Nse2::TNseInn::rr_key mutable  RoomKey_;

   inline K_ctor SEvent ()
      : EvLogON_  (true)
      , NetSendON_(false)
   {}

   Nse2_EvMap_Default_Operator_Equal(SEvent)
};
/////////////////////////////////////////////////////////////////////////////
// class TEvMap
/////////////////////////////////////////////////////////////////////////////
typedef Nse2::TEvMap2k1d<TErrKind, TErrNum, SEvent>  TEvMapType;
typedef TEvMapType::TMap1  TEvMap;
typedef TEvMapType::TMap2  TEvMapD;
typedef TEvMap::iterator   TEvi;     //��1�h Map �� iterator
typedef TEvMap::updater    TEvu;     //��1�h Map �� updater
typedef TEvMapD::iterator  TEvDi;    //��2�h Map �� iterator
typedef TEvMapD::updater   TEvDu;    //��2�h Map �� updater


//=============================================================================
//�o�@�q�O�B�z��ܳ���
//=============================================================================
LIBNSAUX_class TErrMsgDFieldMaker                              //��2�h��������
{
   typedef TEvMapD                  TEvContainer;
   typedef TEvContainer::iterator   iterator;
   typedef size_t                   field_n_type;
private:
   TGroupDef*                      GDef_;
   TVAEditProp_GetKeys<TGroupDef>  Keys_;
public:
   K_ctor TErrMsgDFieldMaker(TGroupDef* gdef)
       : GDef_(gdef),
         Keys_(*GDef_)
       {}
   K_mf(const TFieldDesp&) GetFieldDesp(size_t)  const;
   K_mf(size_t)            GetFieldCount()       const { return 7; }
   K_mf(std::string) Get(const iterator&, size_t) const;
   K_mf(void)        Put(TEvContainer&, iterator&, size_t, const std::string&);
};
//=============================================================================
typedef TTViewArea<TEvMapD, TErrMsgDFieldMaker> TErrMsgDViewer;    //��2�h��Viewer
//=============================================================================
LIBNSAUX_class TErrMsgDVACr : public TVACreater            //��2�h��VAC
{
    typedef TVACreater      inherited;
    typedef TErrMsgDVACr    this_type;
private:
    TGroupDef*  GDef_;
 public:
    K_ctor TErrMsgDVACr(TGroupDef* gdef) : GDef_(gdef) {}
    K_mf(TaViewArea) CreateNewVA()
      { TViewControls   vcs;
        vcs.set(vco_Insert);             //�]�w�i�s�W
        vcs.set(vco_Erase);              //�]�w�i�R��
        vcs.set(vco_CopyCurrent);
        TErrMsgDFieldMaker a(GDef_);
        return TaViewArea(new TErrMsgDViewer(0, a, vcs));
      }
};
//=============================================================================
LIBNSAUX_class TErrMsgVAC : public TVACreater              //��1�h��VAC
{
   typedef TVACreater   inherited;
   typedef TErrMsgVAC   this_type;
   TErrMsgDVACr         DetailCreater_;

   LIBNSAUX_class TErrMsgFieldMaker
   {
      typedef TEvMap                   TEvContainer;
      typedef TEvContainer::iterator   iterator;
      typedef size_t                   field_n_type;
   public:
          K_mf(const TFieldDesp&) GetFieldDesp (field_n_type) const;
   inline K_mf(field_n_type)      GetFieldCount() const {return 1;}
          K_mf(std::string) Get(const iterator&, field_n_type) const;
          K_mf(void)        Put(TEvContainer&, iterator&, field_n_type, const std::string&) {}
   };
   TEvMap* Recs_;               //��1�h�� Map

   K_mf(size_t) GetDetails(const char**& a);
   K_mf(TVACreater*) GetDetailVACreater  (size_t detailID);
   K_mf(TVACreater*) OnMasterViewRowMoved(TViewArea&, TViewArea*&, size_t);
public:
   typedef TTViewArea<TEvMap, TErrMsgFieldMaker> TMsgViewer;
   K_ctor TErrMsgVAC(TEvMap& map, TGroupDef* gdef)
        : Recs_(&map),
          DetailCreater_(gdef)
        {}
   K_mf(TaViewArea) CreateNewVA()
     { TViewControls   vcs;
       vcs.set(vco_Insert);
       vcs.set(vco_Erase);
       TErrMsgFieldMaker a;
       return TaViewArea(new TMsgViewer(Recs_, a, vcs));
     }
};


/////////////////////////////////////////////////////////////////////////////
// struct SEvLog  (�ƥ�O����)
/////////////////////////////////////////////////////////////////////////////
struct SEvLog
{
   TKDateTimeMS                   Time_;
   TObjID                         ObjID_;       // �ƥ�s�էO
   TErrCodeSet                    EvCode_;      // �ƥ�N��
   TErrLevel                      EvLevel_;     // ����
   Kstl::char_ary<char, 60, ' '>  EvMsg_;       // �T��
   std::string                    MsgOwner_;    // ���[�T��
   std::string                    MsgDetail_;   // �Բӻ���
};
//---------------------------------------------------------------------------
typedef Kstl::ev_map<dword, SEvLog>  TEvLogs;


/////////////////////////////////////////////////////////////////////////////
// class TEvLogFieldMaker
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TEvLogFieldMaker                              //��2�h��������
{
   typedef TEvLogs                  TEvContainer;
   typedef TEvContainer::iterator   iterator;
   typedef size_t                   field_n_type;
public:
   K_ctor TEvLogFieldMaker() {}
   K_mf(const TFieldDesp&) GetFieldDesp (size_t) const;
   K_mf(size_t)            GetFieldCount() const { return 8; }
   K_mf(std::string)  Get(const iterator&, size_t) const;
   K_mf(void)         Put(TEvContainer&, iterator&, size_t, const std::string&) {}
};
//---------------------------------------------------------------------------
struct TEvKeyMaker
{
   inline K_mf(void) operator()(dword& akey, const char* ckey) const
      { akey = atoi(ckey); }
};
//---------------------------------------------------------------------------
typedef TTViewArea<TEvLogs, TEvLogFieldMaker, TEvKeyMaker> TEvLogViewer;
//---------------------------------------------------------------------------
LIBNSAUX_class TEvLogVACr : public TVACreater            //��2�h��VAC
{
    typedef TVACreater  inherited;
    typedef TEvLogVACr  this_type;
private:
   TEvLogs*             EvLogs_;
public:
   K_ctor TEvLogVACr(TEvLogs* evlog): EvLogs_(evlog) {}
   K_mf(TaViewArea) CreateNewVA() { return TaViewArea(new TEvLogViewer(EvLogs_)); }
};


/////////////////////////////////////////////////////////////////////////////
// class TExcMgr
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TExcMgr : public TExcMgrBase
{
   typedef TExcMgrBase                                inherited;
   typedef TExcMgr                                    this_type;
   typedef Nsa::TRecorder2d<TEvMap, TEvMapD, SEvent>  TEvRecorder;

   class TLogRecorder;

   TEvMap*            ErrMap_;
   TEvRecorder*       Recorder_;
   TErrMsgVAC*        ErrVA_;
   TErrCodeSet        HB_;
   TEvLogs*           EvLogs_;
   TEvLogVACr*        EvLogVA_;
   TLogRecorder*      EvLogRecorder_;
   TFile              EvLogFile_;
   TTimer<this_type>  Timer_;

public:
   K_ctor  TExcMgr (bool isHB = false, bool doLog = true);
   K_dtor ~TExcMgr ();

   K_mf(std::string) GetFuncStatus (const TErrCodeSet& ercode);

protected:
   //==============================================
   // overwrite TExcMgrBase virtual function
   K_mf(void) WriteEvLog (const TObjID&,
                          const TErrCodeSet&,
                          const TErrLevel&,
                          const std::string& msg,
                          const std::string& msgOwner,
                          const std::string& msgDetail);

   
   
   
   
   K_mf(bool) GetErrDef  (const TErrCodeSet&, TErrLevel&, TObjID&, std::string& ermsg);
   
   K_mf(void) OnTimer    (TTimerBase*);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_NsaExcMgr_h_)
