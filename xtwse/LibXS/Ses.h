//
// 通訊classes基礎: Session-Messager
//    class TSesBase;
//    class TSesEvHandler;
//
// 版本: $Id: Ses.h,v 1.3 2004/12/23 02:23:31 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SesH
#define SesH
//---------------------------------------------------------------------------
#include "Mes.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSesBase;
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSesEvHandler
{
friend class TSesBase;
virtual K_mf(void)  OnSesStateChanged  (TSesBase*, const std::string& reason)  {}
virtual K_mf(void)  OnSesSetRemark     (TSesBase*, const std::string& reason)  {}
virtual K_mf(void)  OnSesBfDestroy     (TSesBase*)  {}
virtual K_mf(void)  OnSesLeave         (TSesBase*)  {}
virtual K_mf(void)  OnSesEnter         (TSesBase*)  {}
};
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSesBase : public TMesEvHandler
{
   TSesEvHandler* EvHandler_;
   TMesBase*      Mes_;
protected:
           K_mf(void)  OnMesBfDestroy  (TMesBase*);
           K_mf(void)  OnMesLeave      (TMesBase*);
           K_mf(void)  OnMesEnter      (TMesBase*);
           K_mf(void)  OnSetRemark     (const std::string& remark);//呼叫 EvHandler_ 的 OnSesSetRemark()
   virtual K_mf(void)  OnStateChanged  (const std::string& reason);//呼叫 EvHandler_ 的 OnSesStateChanged()
   inline  K_mf(TSesEvHandler*)  GetSesEvHandler  () const  { return EvHandler_; }
   inline  K_mf(TMesBase*)       ClearMes         ()        { TMesBase* mes = Mes_;  Mes_ = 0;  return mes; }
public:
           K_ctor  TSesBase  ();
   virtual K_dtor ~TSesBase  ();

          K_mf(void)  SetEvHandler  (TSesEvHandler*);

   inline K_mf(void)       SetMes  (TMesBase* mes)  { mes->SetEvHandler(this); }
   inline K_mf(TMesBase*)  GetMes  ()               { return Mes_; }
   inline K_mf(bool)       Close   ()               { if(Mes_) return Mes_->Close();
                                                      return false; }
   inline K_mf(bool)       Open    ()               { if(Mes_) return Mes_->Open();
                                                      return false; }

   virtual K_mf(bool)  SettingApply  (const std::string&)   { return true; }

   virtual K_mf(std::string) GetSesCmdPrompt () {
      return std::string();
   }
   /// logmsg 可以是 userid, 或其他任何字串
   virtual K_mf(bool) DoSesCmd (const std::string& logmsg, const std::string& cmd) {
      return false;
   }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
