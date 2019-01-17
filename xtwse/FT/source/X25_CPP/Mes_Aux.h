/**
@file Mes_Aux.h

@brief 將 TMes 事件回到 main-thread 之中處理.

版本: $Id: Mes_Aux.h,v 1.2 2005/07/25 01:57:36 blue Exp $
*/
#ifndef Mes_AuxH
#define Mes_AuxH
//---------------------------------------------------------------------------
#include "Mes.h"
#include "SyncEv.h"
//---------------------------------------------------------------------------
namespace Kway {
//---------------------------------------------------------------------------
#ifdef __GNUC__
#  define CAST_MF(TFn,fn)      TFn(fn)
#else
#  define CAST_MF(TFn,fn)      static_cast<TFn>(fn)
#endif
/////////////////////////////////////////////////////////////////////////////
//為了使 Mes 的事件能在主Thread之中處理, 所以必須使用同步機制
template <class TMes, class TMesEvFn>
class TMesSync : public TSyncObj
{
   typedef TSyncObj  inherited;
protected:
   TMes*&      Mes_;
   TMesEvFn    Fn_;
public:
   K_ctor  TMesSync  (TMes** m)
      : Mes_(*m)
      { }

   inline K_mf(bool)  operator()  (TMesEvFn fn)
      { if(Mes_==0)
           return false;
        Fn_  = fn;
        return inherited::operator()(); }
};
//---------------------------------------------------------------------------
typedef K_mf(void)  (TMesBase::*MesMF_0)  ();   // TMesBase::SetLinkReady

template <class TMes>
class TMesSync_MF_0 : public TMesSync<TMes, MesMF_0>
{
   typedef TMesSync<TMes, MesMF_0>  inherited;
public:
   K_ctor  TMesSync_MF_0  (TMes** m)
      : inherited(m)
      { }

   K_mf(void)  Sync  ()    { if(Mes_)  (Mes_->*Fn_)(); }
};
//---------------------------------------------------------------------------
template <class TMes, class TArg1>
class TMesSync_MF_1 : public TMesSync<TMes, MesMF_0>
{
   typedef TMesSync<TMes, MesMF_0>  inherited;
   typedef K_mf(void)  (TMesBase::*TFn)  (TArg1);
   TArg1    A1_;
public:
   K_ctor  TMesSync_MF_1  (TMes** m, TArg1 a1)
      : inherited(m)
      , A1_(a1)
      { }

   inline K_mf(bool)  operator()  (TFn fn)
      { return inherited::operator()(CAST_MF(MesMF_0,fn)); }

   K_mf(void)  Sync  ()    { if(Mes_)  (Mes_->*CAST_MF(TFn,Fn_))(A1_); }
};
//---------------------------------------------------------------------------
template <class TMes, class TArg1, class TArg2>
class TMesSync_MF_2 : public TMesSync<TMes, MesMF_0>
{
   typedef TMesSync<TMes, MesMF_0>  inherited;
   typedef K_mf(void)  (TMesBase::*TFn)  (TArg1,TArg2);
   TArg1    A1_;
   TArg2    A2_;
public:
   K_ctor  TMesSync_MF_2  (TMes** m, TArg1 a1, TArg2 a2)
      : inherited(m)
      , A1_(a1)
      , A2_(a2)
      { }

   inline K_mf(bool)  operator()  (TFn fn)
      { return inherited::operator()(CAST_MF(MesMF_0,fn)); }

   K_mf(void)  Sync  ()    { if(Mes_)  (Mes_->*CAST_MF(TFn,Fn_))(A1_,A2_); }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

