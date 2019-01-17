//
// 通訊classes基礎: Session-Messager
//    class TSesMgr; 通訊 [處理程序Session]/[設備Messager] 管理員
//
// 版本: $Id: SesMgr.h,v 1.6 2005/12/28 08:43:17 fonwin Exp $
//
//---------------------------------------------------------------------------
/*
921020:fonwin:
   將    typedef TSesBase* (*TSesCreater) (const TSesReg&);
   改成  typedef TSesBase* (*TSesCreater) (const TSesReg&, TSesMgr&);
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

   //設定編輯器
   TVAGetEditProp*      SettingEditor_;

   //若Setting有包含密碼時,編輯時的字串,與顯示的字串,應該不會一樣吧!
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
   //若 regAtCtor==false 則建構之後, 您必須自行呼叫 RegToSysSesMgr(),
   //避免您的建構尚未完成, SesMgr就呼叫建立Session, 可能造成使用到尚未建構的物件!
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
   //註冊資訊
   typedef Kstl::pod_vector<const TSesReg*>  TSesRegs;
   typedef TSesRegs::iterator                TSesRegi;
   TSesRegs    SesRegs_;

   struct  TMesRegs : public Kstl::pod_vector<const TMesReg*>  {};
   typedef TMesRegs::iterator                TMesRegi;
   TMesRegs    MesRegs_;
   //---------------------------------
   //現在的連線狀態
   class TStatus;
   //std::auto_ptr<TStatus>   Status_;  因為使用 auto_ptr<> 會造成 undefined structure 的 warning
   //                                   所以直接使用 pointer
   TStatus*     Status_;
   //---------------------------------
   //設定紀錄
   class TSetting;
   friend class TSetting;
   //std::auto_ptr<TSetting>  Setting_;
   TSetting*   Setting_;
   //---------------------------------
   class TMesMeta;
   friend class TMesMeta;
   //---------------------------------
   class TSesID : public Kstl::fixed_num<dword> //不用這種方式定義 Key,會造成BCB產生的程式碼有問題
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
   /** 提供給繼承的人,有機會在解構前,先將Ses全部刪除.
       避免Ses有用到一些將要被刪除的資源.
       ※ 僅能在解構時呼叫!!
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
      sc_Del,  ///< 刪除:刪除某設定,或為[修改]則先用sc_Del通知刪除舊的,然後再用sc_Add通知增加新的.
      sc_Add,  ///< 新增:不論是[修改]或是[增加],都用sc_Add通知.
   };
   //---------------------------------
   /**  若此次的異動是從 VA 而來,則可以從下式取得 UserID:
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
   K_mf(void)  ReloadSetting     (const std::string& settingFileName, bool useIniFile = false); //啟動設定值,用於註冊完畢後
   #else
   K_mf(void)  ReloadSetting     (const std::string& settingFileName); //啟動設定值,用於註冊完畢後
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

