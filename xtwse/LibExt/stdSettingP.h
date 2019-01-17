//---------------------------------------------------------------------------
#ifndef stdSettingPH
#define stdSettingPH
//---------------------------------------------------------------------------
#include "char_ary.h"
#include "ViewAreaMgr.h"
#include "StdRecorder.h"
#include "TwStk.h"
#include "TableLogger.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw::Stk;
using namespace Kstl;
//---------------------------------------------------------------------------
typedef char_ary<char,40,' '> TDesp;
//---------------------------------------------------------------------------

enum eType {
   ebool,
   eword,
   eTKTime,
   eTKDate,
   eTStkQty,
   eTStkPri,
   eTStkAmt,
   echar_ary,
   ebyte,
   eNone,
};

//---------------------------------------------------------------------------
struct SstdSetting {
   typedef SstdSetting this_type;
   bool           Used_;
   TDesp          Item_;
   TDesp          Setting_;
   TDesp          Memo_;
   bool*          IsUse_;
   eType          Type_;
   void*          Variable_;
   inline K_mf(bool)  operator!= (const this_type& a) const {return (Used_ != a.Used_ || Setting_ != a.Setting_);}
};
//---------------------------------------------------------------------------
typedef Kway::word                       TKey;
typedef Kstl::ev_map<TKey, SstdSetting>  TstdSettings;
//---------------------------------------------------------------------------
struct TNumKeyMaker
{
   inline K_mf(void) operator()(TKey& akey, const char* ckey) const
                               {akey = atoi(ckey);}
};
//---------------------------------------------------------------------------
struct TstdFieldMaker
{
   typedef TstdSettings             TEvContainer;
   typedef TEvContainer::iterator   iterator;
   typedef size_t                   field_n_type;

   K_mf(const Kway::TFieldDesp&)  GetFieldDesp  (field_n_type n) const
          {
            static Kway::TFieldDesp f[] = {
                     { "項次",      6, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_Fixed,  },
                     { "啟用",      4, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit,},
                     { "項目",     50, Kway::ta_xCenter_yCenter, Kway::ta_xLeft_yCenter  , Kway::tfs_Null,   },
                     { "附加設定", 10, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit,},
                     { "備註",     30, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit,}};
            return f[n];
          }
   inline K_mf(field_n_type)  GetFieldCount () const    { return 5; }

   K_mf(std::string)  Get  (const iterator& i, field_n_type fieldNo) const
         {
            char  buf[128];
            switch(fieldNo) {
            case 0:  sprintf(buf,"%d",i->first); return std::string(buf);
            case 1:  return (i->second.Used_?"Y":"N");
            case 2:  return i->second.Item_.as_string();
            case 3:  return i->second.Setting_.as_string();
            case 4:  return i->second.Memo_.as_string();
            }
            return std::string();
         }
   K_mf(void)  Put  (TEvContainer& c, iterator& iter, field_n_type fieldNo, const std::string& str)
         {
            if(fieldNo ==0 )
               return;
            TEvContainer::updater   u = c.begin_update(iter);
            switch(fieldNo) {
            case 1:u->Used_ = ((str[0]=='Y' || str[0]== 'y')?true:false);  break;
            case 2:u->Item_.assign(str);     break;
            case 3:u->Setting_.assign(str);  break;
            case 4:u->Memo_.assign(str);     break;
            default:break;
            }
         }
}; // class TSampleFieldMaker
/////////////////////////////////////////////////////////////////////////////
class TSettingPage;
K_class TstdVA :public TTViewArea<TstdSettings, TstdFieldMaker, TNumKeyMaker>
{
   typedef TTViewArea<TstdSettings, TstdFieldMaker, TNumKeyMaker> inherited;
   typedef TSettingPage   T;
   typedef K_mf(bool)(T::*TSettingApply)(TstdSettings*);
private:
   T*             Obj_;
   TstdSettings   Shadow_;
   TstdSettings*  Origin_;
   TSettingApply  Apply_;
   TstdFieldMaker FMaker_;
   K_mf(bool)     SettingApply()
   {
      TstdSettings orig = *Origin_;
      *Origin_ = Shadow_;
      if (Obj_ && Apply_)
         return (Obj_->*Apply_)(&orig);
      return false;
   }
   K_mf(bool)    SettingCancel()
   {
      Shadow_ = *Origin_;
      return true;
   }
public:
   K_ctor TstdVA(T* obj, TSettingApply ap, TViewControls vcs, TstdSettings* c)
      :inherited(0,FMaker_, vcs)
   {
      Obj_   = obj;
      Apply_ = ap;
      Origin_ = c;
      Shadow_ = *c;
      inherited::SetContainer(&Shadow_);
   }
};
//---------------------------------------------------------------------------
typedef TStdRecorder<TstdSettings, SstdSetting> TstdRecorder;
//---------------------------------------------------------------------------
K_class TSettingPage :public TVACreater
{
private:
   typedef TstdSettings::iterator    TSettingi;
   typedef TstdSettings::updater     TSettingu;
   typedef TSettingPage              this_type;
   TViewArea*                        VA_;
public:
   TstdSettings                  Settings_;
   TstdRecorder                  Recorder_;
   TTableLogger<TstdFieldMaker>* Log_;
   std::string                   LogName_;
   K_ctor                        TSettingPage(const char* fileName, const char* logname = 0);
   K_dtor                        ~TSettingPage();
   K_mf(bool)                    SettingApply(TstdSettings* orig = 0);
   K_mf(bool)                    SettingCancel();
   K_mf(void)                    Reg(void*, const char*, const char*, eType vtype = eNone, void* variable = 0);
   K_mf(TViewArea*)              GetVA();
   K_mf(TaViewArea)              CreateNewVA();
};
//---------------------------------------------------------------------------
#endif
