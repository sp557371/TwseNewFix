//
// 通訊classes基礎: Session-Messager
//    Messager 註冊
//
// 版本: $Id: MesRegs.h,v 1.2 2003/12/29 07:45:15 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef MesRegsH
#define MesRegsH
//---------------------------------------------------------------------------
#include "ViewArea.h"
#include "Mes.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
struct TMesReg
{
   typedef TMesPtr  (*TMesCreater)  ();

   const char*    Name_;
   TMesCreater    MesFactory_;
   bool           IsServer_;

   //設定編輯器
   TVAGetEditProp*   SettingEditor_;

   //若Setting有包含密碼時,編輯時的字串,與顯示的字串,應該不會一樣吧!
   typedef std::string (*TGetShowSetting) (const std::string&);
   TGetShowSetting      GetShowSetting_;
};
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMesRegs
{
   const TMesReg**   begin_;
   const TMesReg**   end_;

   inline K_ctor  TMesRegs  (const TMesReg** begin, const TMesReg** end)
                     : begin_(begin),
                       end_(end)
                     { }
public:
   static K_mf(TMesRegs)  GetDefaultMesRegs  ();  //包含全部的Mes: 含Client端,與Server端
   static K_mf(TMesRegs)  GetClientMesRegs   ();
   static K_mf(TMesRegs)  GetServerMesRegs   ();

   inline K_mf(const TMesReg**)  Begin  () const    { return begin_; }
   inline K_mf(const TMesReg**)  End    () const    { return end_;   }

   K_mf(const TMesReg*)  Find  (const std::string& mesName) const;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
