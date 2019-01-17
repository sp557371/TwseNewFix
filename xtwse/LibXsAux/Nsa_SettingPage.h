
   // 請改使用 Nsa_SettPage.h

/////////////////////////////////////////////////////////////////////////////
#if !defined(_NsaSettingPage_h_)
#define _NsaSettingPage_h_
//---------------------------------------------------------------------------
#include "ev_map.h"
#include "char_ary.h"
#include "libnsAux.h"
//---------------------------------------------------------------------------
namespace Kway {
   class TVACreater;
};

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
// 基本宣告
/////////////////////////////////////////////////////////////////////////////
typedef std::string  TSpeKey;
//---------------------------------------------------------------------------
enum ESettingPageType
{
   spt_None,
      spt_Bool,
      spt_Int,
      spt_StdStr,
      spt_TKTime,
};
/////////////////////////////////////////////////////////////////////////////
// struct SSpeDat
/////////////////////////////////////////////////////////////////////////////
struct SSpeDat
{
   ESettingPageType  Type_;
   void*             Variable_;
   std::string       Setting_;  // 附加設定
   std::string       Memo_;     // 備註

   inline K_ctor SSpeDat ()
      : Type_    (spt_None)
      , Variable_(NULL)
   {}
};


/////////////////////////////////////////////////////////////////////////////
// class TSettingPage
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TSettingPage
{
public:
   typedef Kstl::ev_map<TSpeKey, SSpeDat>  TRecs;
   typedef TRecs::iterator                 TRecsI;
   typedef TRecs::updater                  TRecsU;

protected:
   class TRecsRecorder;
   class TRecsVAC;

   TRecs           Recs_;
   TRecsRecorder*  RecsRecorder_;
   TRecsVAC*       RecsVAC_;

public:
   K_ctor  TSettingPage (const char* filePath);
   K_dtor ~TSettingPage ();

   //==============================================
   // TSettingPage virtual function
   virtual K_mf(bool) SettingApply   (TRecs*);
   virtual K_mf(void) AfSettingApply (TRecs*)  {}
   
   //==============================================
   // TSettingPage function
   // 在尚未呼叫 SettingApply 使用
   K_mf(TRecsI) Reg (const char* item, const char* setting, ESettingPageType spType, void* variable, const char* memo = NULL);

   K_mf(TVACreater&) GetVAC ();

protected:
   K_mf(void) Update_Variable (ESettingPageType, void* Variable, const std::string&);

   // 給繼承 TSettingPage 物件初始化使用
   inline K_mf(void) SettingApply ()  { SettingApply(&Recs_); }


/*
   // 給 ev_map_setting_viewer 呼叫使用
   virtual K_mf(bool) SettingApply (TRecs* Recs);

   // 在已呼叫 SettingApply 後使用
   K_mf(void)   RegApply (bool* IsUse, const char* cpItem, const char* Setting, ESettingPageType SpType, void* Variable, const char* cpMemo = NULL);

   // 將 Reg 的資料 IsUse, Variable 設為 NULL
   K_mf(void) RegDisable (const char* cpItem);
   // 將 Reg 的資料從檔案移除
   K_mf(void) RegRemove  (const char* cpItem);

   // 更新設定值
   K_mf(void) Update      (const char* cpItem, const std::string& strNewSetting);
   K_mf(void) Update_Bool (const char* cpItem, bool bNewSetting);
   K_mf(void) Update_Int  (const char* cpItem, int  nNewVal);
*/

};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_NsaSettingPage_h_)
