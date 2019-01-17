//
// template ev_handler_base<>
// [�ƥ�B�z��] ����:
//    �ĥ� bit mask ���覡�P�_: �Y�ƥ�O�_�ݭn�I�s
//
// ����: $Id: ev_handler.h,v 1.5 2005/12/28 07:54:29 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef ev_handlerH
#define ev_handlerH
/////////////////////////////////////////////////////////////////////////////
#include <bitset>
//---------------------------------------------------------------------------
#include "pod_vector.h"
//---------------------------------------------------------------------------
namespace Kstl {
//
// Kstl �R�W�W�h����:�аѦ� "ev_container.h"
//
/////////////////////////////////////////////////////////////////////////////
template <size_t EvMaskN>
class ev_handler_base
{
public:
       K_ctor  ev_handler_base  ()    : ev_mask_() { ev_mask_.set(); }

//���լO�_�ݭn���ͬY�ƥ�
inline K_mf(bool)  is_need_ev  (size_t a) const  { return ev_mask_.test(a); }

protected:
   std::bitset<EvMaskN>    ev_mask_; //�]��1,��ܭn���ͨƥ�
};
//---------------------------------------------------------------------------
/* TEvs ���w�q�覡�p�U��: �Ѧ� "ev_container.h"
typedef pod_vector<container_ev_handler*> container_ev_handlers;
typedef container_ev_handlers::iterator   container_ev_handleri;
container_ev_handlers   evs_;
*/
//---------------------------------------------------------------------------
template <class TEvs, class TEvCaller>
void call_ev_handler  (TEvs& evs, size_t evMask, TEvCaller& evCall)
{  //�]�� evCall()�٦��Ѽƶǻ������D,������B�z,
   //�ҥH�Ȯɱĥ� [����] ���覡 Kstl_call_ev_handler
}
//-------------------------------------------------------
#define Kstl_call_ev_handler(tev, evMask, evCall)       \
   if(!evs_.empty()) {                                  \
      typedef Kstl::pod_vector<tev*>   tevs;            \
      typedef tevs::iterator           tevi;            \
      tevi  evsend = evs_.end();                        \
      for(tevi L = evs_.begin();  L != evsend;  ++L) {  \
         tev*  ev = *L;                                 \
         if(ev  &&  ev->is_need_ev(evMask))             \
            ev->evCall;                                 \
      }                                                 \
   }                                                    \
/////////////////////////////////////////////////////////////////////////////
template <class T>
struct TGetParamType {
   typedef T&        TParam;
   typedef const T&  TConstParam;
};

template <class T>
struct TGetParamType<T*> {
   typedef T*        TParam;
   typedef const T*  TConstParam;
};
/////////////////////////////////////////////////////////////////////////////
/** ���@�ǳ�@�ƥ�q���� class �Ө�, �i�ϥΥH�U���覡:
    ������Υi�Ѧ� TwStkOrdCenter.cpp: struct TOnNewTseOrdKey
class TEvHandler {
   struct TArg { ... };
   virtual K_mf(ev_result)  OnEvCallback_UseYourName  (const TArg&) = 0;
   friend inline K_mf(ev_result)  CallEv  (TEvHandler* pThis, const TArg& a)
        { return pThis->OnEvCallback_UseYourName(a); }
};

typedef Kstl::ev_handlers<TEvHandler>  TEvHandlers;
TEvHandlers    EvHandlers_;

if(!EvHandlers_.empty()) {
   TArg  a;
   TEvHandlers::caller<TArg>  caller;
   EvHandlers_.call_evs(caller,a);
}
*/
enum ev_result {
   evr_DelEvHandler,
   evr_Continue,
};

template <class TEvHandler, class TEvHandlers = pod_vector<TEvHandler*> >
class ev_handlers
{
   TEvHandlers    handlers_;
   typedef typename TEvHandlers::value_type                value_type;
   typedef typename TGetParamType<value_type>::TParam      value_param;
   typedef typename TGetParamType<value_type>::TConstParam const_value_param;
public:
   inline K_mf(bool)   empty () const                       { return handlers_.empty(); }
   inline K_mf(size_t) size  () const                       { return handlers_.size();  }
   inline K_mf(bool)   find  (const_value_param a) const    { typename TEvHandlers::const_iterator i;
                                                              return handlers_.find(a,i); }
   inline K_mf(TEvHandler*) get (size_t id) const           { return id >= size() ? NULL : handlers_[id]; }


   /** TODO: call_evs()�i�椤, ���ӭn�T�� del()?
   */
   inline K_mf(bool)  add  (value_param a)  { return handlers_.add(a);  }
   inline K_mf(bool)  del  (value_param a)  { return handlers_.del(a);  }

   template <class CallEv, class TArg>
   void  call_evs_r  (CallEv& caller, TArg& a)
      {  for(size_t L = size();  L > 0;)
            switch(caller(handlers_[--L], a)) {
            case evr_DelEvHandler:
                        handlers_.erase(handlers_.begin() + L);
                        break;
            case evr_Continue:
            default:    break;
            }
      } // call_evs_r()

   template <class CallEv, class TArg>
   void  call_evs  (CallEv& caller, TArg& a)
      {  size_t    sz = size();
         for(size_t L = 0;  L < sz;)
            switch(caller(handlers_[L], a)) {
            case evr_DelEvHandler:
                        handlers_.erase(handlers_.begin() + L);
                        --sz;
                        break;
            case evr_Continue:
            default:    ++L;
                        break;
            }
      } // call_evs_r()

   template <class TArg>
   struct caller {
      inline K_mf(ev_result) operator() (const value_param h, const TArg& a)
         { return CallEv(h, a);
         }
   };

   K_mf(void)  delete_all_handlers  ()
      {  typename TEvHandlers::iterator  iend = handlers_.end();
         for(typename TEvHandlers::iterator i = handlers_.begin();  i != iend;  ++i)
            delete (*i);
         handlers_.clear();
      }
};
/////////////////////////////////////////////////////////////////////////////
enum reg_action {
      ra_Add,
      ra_Del,
      ra_Get,
};
//---------------------------------------------------------------------------
template <class TEvHandler, class TEvHandlers = pod_vector<TEvHandler*> >
class reg_ev_handlers : public ev_handlers<TEvHandler, TEvHandlers>
{
   typedef ev_handlers<TEvHandler>                          inherited;
   typedef reg_ev_handlers<TEvHandler>                      this_type;
   typedef typename TEvHandlers::value_type                 value_type;
   typedef typename TGetParamType<value_type>::TParam       value_param;
   typedef typename TGetParamType<value_type>::TConstParam  const_value_param;
public:
   static this_type&  reg_to_singleton  (value_param handler, reg_action ra)
      {  static this_type  EvHandlers;
         switch(ra) {
         case ra_Add:   EvHandlers.add(handler);   break;
         case ra_Del:   EvHandlers.del(handler);   break;
         default:
         case ra_Get:   break;
         }
         return EvHandlers;
      }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kstl
//---------------------------------------------------------------------------
#endif

