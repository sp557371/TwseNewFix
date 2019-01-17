//
// �q�Tclasses��¦: Session-Messager
//    class TSesMgr; �q�T [�B�z�{��Session]/[�]��Messager] �޲z��
//
// ����: $Id: SesMgr.h,v 1.6 2005/12/28 08:43:17 fonwin Exp $
//
//---------------------------------------------------------------------------
/*
921020:fonwin:
   �N    typedef TSesBase* (*TSesCreater) (const TSesReg&);
   �令  typedef TSesBase* (*TSesCreater) (const TSesReg&, TSesMgr&);
*/
//---------------------------------------------------------------------------
#ifndef SesMgrH
#define SesMgrH
//---------------------------------------------------------------------------
#include "MesRegs.h"
#include "Ses.h"
#include "ViewAreaMgr.h"
#include "fixed_num.h"
#include "ev_map.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
K_class TSesMgr;
//---------------------------------------------------------------------------
struct TSesReg
{
   const char*          Name_;

   typedef TSesBase*  (*TSesCreater)  (const TSesReg&, TSesMgr&);
   TSesCreater          SesFactory_;

   //�]�w�s�边
   TVAGetEditProp*      SettingEditor_;

   //�YSetting���]�t�K�X��,�s��ɪ��r��,�P��ܪ��r��,���Ӥ��|�@�˧a!
   typedef std::string (*TSesGetShowSetting) (const std::string&);
   TSesGetShowSetting   GetShowSetting_;
};
//---------------------------------------------------------------------------
K_class TSesRegC : public TSesReg
{
public:
   inline K_ctor  TSesRegC  ( const char*        name = ""
                            , TSesCreater        sesCreater = 0
                            , TVAGetEditProp*    settingEditor = 0
                            , TSesGetShowSetting getShowSetting = 0)
      {  Name_           = name;
         SesFactory_     = sesCreater;
         SettingEditor_  = settingEditor;
         GetShowSetting_ = getShowSetting;
      }
};
//---------------------------------------------------------------------------
K_class TSesRegBase : public TSesRegC
{
   typedef TSesRegC  inherited;
public:
   //�Y regAtCtor==false �h�غc����, �z�����ۦ�I�s RegToSysSesMgr(),
   //�קK�z���غc�|������, SesMgr�N�I�s�إ�Session, �i��y���ϥΨ�|���غc������!
   inline K_ctor TSesRegBase (const char* name, TSesCreater sesCreater, bool regAtCtor)
      : inherited(name, sesCreater)
      {
        if(regAtCtor)
           RegToSysSesMgr();
      }
   virtual K_dtor ~TSesRegBase() {
      UnregSysSesMgr();
   }
   K_mf(void) RegToSysSesMgr();
   K_mf(void) UnregSysSesMgr();
};
/////////////////////////////////////////////////////////////////////////////
K_class TSesMgr
{
   //---------------------------------
   //���U��T
   typedef Kstl::pod_vector<const TSesReg*>  TSesRegs;
   typedef TSesRegs::iterator                TSesRegi;
   TSesRegs    SesRegs_;

   struct  TMesRegs : public Kstl::pod_vector<const TMesReg*>  {};
   typedef TMesRegs::iterator                TMesRegi;
   TMesRegs    MesRegs_;
   //---------------------------------
   //�{�b���s�u���A
   class TStatus;
   //std::auto_ptr<TStatus>   Status_;  �]���ϥ� auto_ptr<> �|�y�� undefined structure �� warning
   //                                   �ҥH�����ϥ� pointer
   TStatus*     Status_;
   //---------------------------------
   //�]�w����
   class TSetting;
   friend class TSetting;
   //std::auto_ptr<TSetting>  Setting_;
   TSetting*   Setting_;
   //---------------------------------
   class TMesMeta;
   friend class TMesMeta;
   //---------------------------------
   class TSesID : public Kstl::fixed_num<dword> //���γo�ؤ覡�w�q Key,�|�y��BCB���ͪ��{���X�����D
   {
      typedef Kstl::fixed_num<dword>   inherited;
   public:
      K_ctor  TSesID  (orig_type n=orig_type())
               : inherited(n)
               { }
   };
   //---------------------------------
   struct TRegChangedBase {
      virtual K_dtor ~TRegChangedBase  () {}
   };
   typedef std::auto_ptr<TRegChangedBase> TRegChanged;
   K_mf(TRegChangedBase*)  BfRegChange  ();
   //---------------------------------
protected:
   /** ���ѵ��~�Ӫ��H,�����|�b�Ѻc�e,���NSes�����R��.
       �קKSes���Ψ�@�ǱN�n�Q�R�����귽.
       �� �ȯ�b�Ѻc�ɩI�s!!
   */
   K_mf(void) DestroyAllSessions ();
   //---------------------------------
   struct TSettingRec
   {
      std::string    SesName_;
      std::string    SesSetting_;
      std::string    MesName_;
      std::string    MesSetting_;
      bool           Enabled_;

      K_mf(bool) IsSettingChanged(const TSesMgr::TSettingRec& r) const {
         return SesName_    != r.SesName_
             || SesSetting_ != r.SesSetting_
             || MesName_    != r.MesName_
             || MesSetting_ != r.MesSetting_
             || Enabled_    != r.Enabled_;
      }
   };
   //---------------------------------
   enum ESettingChg {
      sc_Del,  ///< �R��:�R���Y�]�w,�ά�[�ק�]�h����sc_Del�q���R���ª�,�M��A��sc_Add�q���W�[�s��.
      sc_Add,  ///< �s�W:���׬O[�ק�]�άO[�W�[],����sc_Add�q��.
   };
   //---------------------------------
   /**  �Y���������ʬO�q VA �Ө�,�h�i�H�q�U�����o UserID:
         Kway::Pk::TSesServer::GetLastOpUser()
   */
   virtual K_mf(void) OnSettingChg (ESettingChg sc, dword settingID, const TSettingRec& rec)
      {  // do nothing.
      }
public:
           K_ctor  TSesMgr  ();
   virtual K_dtor ~TSesMgr  ();

   K_mf(TVACreater&)  GetSettingVACreater  ();
   K_mf(TVACreater&)  GetStatusVACreater   ();

   inline K_mf(void)  Reg  (const TSesReg** begin, const TSesReg** end)  { TRegChanged hnd(BfRegChange());  SesRegs_.add(begin, end); }
   inline K_mf(void)  Reg  (const TSesReg*  begin, const TSesReg*  end)  { TRegChanged hnd(BfRegChange());  SesRegs_.add(begin, end); }
   inline K_mf(void)  Reg  (const TSesReg*  begin)                       { Reg(begin, begin+1); }
   inline K_mf(void)  Del  (const TSesReg** begin, const TSesReg** end)  { TRegChanged hnd(BfRegChange());  SesRegs_.del(begin, end); }
   inline K_mf(void)  Del  (const TSesReg*  begin, const TSesReg*  end)  { TRegChanged hnd(BfRegChange());  SesRegs_.del(begin, end); }
   inline K_mf(void)  Del  (const TSesReg*  begin)                       { Del(begin, begin+1); }

   inline K_mf(void)  Reg  (const TMesReg** begin, const TMesReg** end)  { TRegChanged hnd(BfRegChange());  MesRegs_.add(begin, end); }
   inline K_mf(void)  Reg  (const TMesReg*  begin, const TMesReg*  end)  { TRegChanged hnd(BfRegChange());  MesRegs_.add(begin, end); }
   inline K_mf(void)  Reg  (const TMesReg*  begin)                       { Reg(begin, begin+1); }
   inline K_mf(void)  Del  (const TMesReg** begin, const TMesReg** end)  { TRegChanged hnd(BfRegChange());  MesRegs_.del(begin, end); }
   inline K_mf(void)  Del  (const TMesReg*  begin, const TMesReg*  end)  { TRegChanged hnd(BfRegChange());  MesRegs_.del(begin, end); }
   inline K_mf(void)  Del  (const TMesReg*  begin)                       { Del(begin, begin+1); }

   inline K_mf(void)  Reg  (const Kway::TMesRegs& mesRegs)               { TRegChanged hnd(BfRegChange());  MesRegs_.add(mesRegs.Begin(),mesRegs.End()); }
   inline K_mf(void)  Del  (const Kway::TMesRegs& mesRegs)               { TRegChanged hnd(BfRegChange());  MesRegs_.del(mesRegs.Begin(),mesRegs.End()); }

   K_mf(const TSesReg*)  GetSesReg   (const std::string&) const;
   K_mf(const TMesReg*)  GetMesReg   (const std::string&) const;

   //**********************************************
   #if defined(__NSX__)
   K_mf(void)  ReloadSetting     (const std::string& settingFileName, bool useIniFile = false); //�Ұʳ]�w��,�Ω���U������
   #else
   K_mf(void)  ReloadSetting     (const std::string& settingFileName); //�Ұʳ]�w��,�Ω���U������
   #endif
   //**********************************************
   K_mf(void)  DisconnectCurrent (TViewArea* statusVA);
   K_mf(TSesBase*) GetCurrentSes (TViewArea* statusVA);
   K_mf(bool)  AddSetting        (const std::string& sesName, const std::string& sesSetting,
                                  const std::string& mesName, const std::string& mesSetting);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
#endif

