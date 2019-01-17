/////////////////////////////////////////////////////////////////////////////
#if !defined(_Nsa_EvtDisp_h_)
#define _Nsa_EvtDisp_h_
//---------------------------------------------------------------------------
#include <stdio.h>
#include "ev_map.h"
#include "wise_ptr.h"
#include "MemPool.h"
#include "TimerThread.h"
#include "Nsa_EvtDispRes.h"
#include "tpl/Nsa_EvObjCtn.h"
#include "libnsAux.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;

namespace Nsa  {

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// 資料結構
/////////////////////////////////////////////////////////////////////////////
typedef int  TEvtDispID;
typedef int  TEvtDispResu;
//---------------------------------------------------------------------------
struct SEvtDispPktHdr
{
   TEvtDispID    ID_;
   size_t        Size_;
   TEvtDispResu  DefResu_;

   inline K_ctor SEvtDispPktHdr ()
      : ID_(edi_None), Size_(0), DefResu_(edr_None)
   {}

   inline K_ctor SEvtDispPktHdr (TEvtDispID id)
      : ID_(id), Size_(0), DefResu_(edr_None)
   {}

   inline K_ctor SEvtDispPktHdr (TEvtDispID id, size_t size, TEvtDispResu defResu = edr_None)
      : ID_(id), Size_(size), DefResu_(defResu)
   {}
};
/*
/////////////////////////////////////////////////////////////////////////////
// 一些常用的資料結構
/////////////////////////////////////////////////////////////////////////////
// 只適用於 TEvHnd::SendEvent(SEvtDispPktHdr&)
struct SEvtDispPktStdString : SEvtDispPktHdr
{
   std::string  Value_;

   inline K_ctor SEvtDispPktStdString ()
      : SEvtDispPktHdr()
   {}
   
   inline K_ctor SEvtDispPktStdString (TEvtDispID id, const std::string& str)
      : SEvtDispPktHdr(id)
      , Value_        (str)
   {}

   inline K_ctor SEvtDispPktStdString (TEvtDispID id, int val)
      : SEvtDispPktHdr(id)
   {
      SetInt(val);
   }

   K_mf(void) SetInt (int val)
   {
      char buf[20];
      sprintf(buf, "%d", val);
      Value_.assign(buf);
   }

   inline K_mf(int) GetInt () const  { return atoi(Value_.c_str()); }
};
*/
/////////////////////////////////////////////////////////////////////////////
// struct SEvtDispPktStdStringN
//    只適用於 TEvHnd::SendEvent(SEvtDispPktHdr&)
/////////////////////////////////////////////////////////////////////////////
struct SEvtDispPktStdStringN : SEvtDispPktHdr
{
   typedef pod_vector<std::string>  TData;

   TData  Data_;

   K_ctor SEvtDispPktStdStringN (TEvtDispID id, const std::string& str1)
      : SEvtDispPktHdr(id)
   {
      Data_.resize(1);
      Data_[0] = str1;
   }

   K_ctor SEvtDispPktStdStringN (TEvtDispID id, const std::string& str1, const std::string& str2)
      : SEvtDispPktHdr(id)
   {
      Data_.resize(2);
      Data_[0] = str1;
      Data_[1] = str2;
   }

   K_ctor SEvtDispPktStdStringN (TEvtDispID id, const std::string& str1,
                                                const std::string& str2,
                                                const std::string& str3)
      : SEvtDispPktHdr(id)
   {
      Data_.resize(3);
      Data_[0] = str1;
      Data_[1] = str2;
      Data_[2] = str3;
   }

   K_ctor SEvtDispPktStdStringN (TEvtDispID id, const std::string* strs[], size_t size)
      : SEvtDispPktHdr(id)
   {
      Data_.resize(size);
      
      for (size_t i=0; i<size; ++i) {
         if (strs[i] != NULL)
            Data_[i] = *(strs[i]);
      }
   }

   K_mf(void) SetInt (size_t idx, int val)
   {
      if (idx >= Data_.size())
         return;

      char buf[20];
      sprintf(buf, "%d", val);
      Data_[idx].assign(buf);
   }

   K_mf(int) GetInt (size_t idx) const
   {
      if (idx >= Data_.size())
         return 0;

      return atoi(Data_[idx].c_str()); 
   }
};


/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif

/////////////////////////////////////////////////////////////////////////////
// class TEvtDisp
/////////////////////////////////////////////////////////////////////////////
LIBNSAUX_class TEvtDisp
{
   typedef TEvtDisp  this_type;

   class TEvtDispWorker;

public:
   //==============================================
   // class TProcHnd
   LIBNSAUX_class TProcHnd
   {
      friend class TEvtDispWorker;

   protected:
      std::string  DbgName_;

   public:
              K_ctor  TProcHnd (const char* dbgName = NULL);
      virtual K_dtor ~TProcHnd ();

      K_mf(void) RegEv (const TEvtDispID* idList, size_t idListSize);
      K_mf(void) DelEv ();

      inline K_mf(std::string) GetName () const  { return DbgName_; }

      static K_mf(TEvtDispResu) SendEvent (TEvtDispID, TEvtDispResu defResu = edr_None);
      static K_mf(TEvtDispResu) SendEvent (SEvtDispPktHdr&);

      static K_mf(void) PostEvent (TEvtDispID, TEvtDispResu);
      static K_mf(void) PostEvent (SEvtDispPktHdr&);

   protected:
      virtual K_mf(TEvtDispResu) OnEvtProc (SEvtDispPktHdr*) = 0;
   };
   friend class TProcHnd;

   typedef TEvObjCtn<TProcHnd>                 TProcHndCtn;
   typedef wise_ptr<TProcHndCtn>               TProcHndCtnPtr;
   typedef ev_map<TEvtDispID, TProcHndCtnPtr>  TProcIDHndCtn;
   typedef TProcIDHndCtn::iterator             TProcIDHndCtnI;

protected:
   TProcIDHndCtn            ProcIDHndCtn_;
   Kway::TMemQueue          TimerQueue_;
   Kway::TTimer<this_type>  Timer_;

public:
   K_ctor  TEvtDisp ();
   K_dtor ~TEvtDisp ();

   K_mf(TEvtDispResu) SendEvent (SEvtDispPktHdr&);
   K_mf(void)         PostEvent (SEvtDispPktHdr&);

   //inline K_mf(void) RegEvHnd (TEvHnd* Hnd)  { EvHndCtn_.Add(Hnd); }
   K_mf(void) RegEvHnd (const TEvtDispID*, size_t, TProcHnd*);
   K_mf(void) DelEvHnd (TProcHnd*);
   
   static K_mf(TEvtDisp&) GetInst ();

protected:
   K_mf(void) OnTimer (Kway::TTimerBase*);

   K_mf(TEvtDispResu) ReadyToFire (SEvtDispPktHdr*);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Nsa
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_Nsa_EvtDisp_h_)
