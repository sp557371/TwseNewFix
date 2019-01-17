#ifndef FixSesMgrH
#define FixSesMgrH
//---------------------------------------------------------------------------
#include "SesMgr.h"
#include "ViewAreaMgr.h"
#include "ev_map.h"
#include "TimerThread.h"
#include <vector>
#include "SignonSvrDB.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Fix  {
namespace Stk  {

K_class  TStkSesFix;
//---------------------------------------------------------------------------
K_class  TFixSesMgr : public Kway::TSesMgr
{
   Kway::TViewAreaMgr  VAMgr_;

   struct  SSesRegKey {
      std::string  BrokerID_;
      std::string  SesID_;

      explicit K_ctor  SSesRegKey(std::string brk, std::string id) : BrokerID_(brk), SesID_(id) {};
      inline bool operator==(const SSesRegKey& key) const { return BrokerID_==key.BrokerID_ && SesID_==key.SesID_; }
      inline bool operator< (const SSesRegKey& key) const { return BrokerID_==key.BrokerID_? SesID_<key.SesID_ : BrokerID_<key.BrokerID_; }
      K_mf(std::string)  MakeText() const { return std::string("<" + BrokerID_ + "," + SesID_ + ">"); }
   };

   typedef TFixSesMgr                              this_type;
   typedef Kway::TSesMgr                           inherited;
   typedef Kstl::ev_map<SSesRegKey, TStkSesFix*>   TSesMap;
   typedef TSesMap::iterator                       TSesMapI;
   typedef Kway::TTimer<this_type>                 TOTimer;
   typedef Kstl::ev_map<std::string, std::vector<std::string> >  TOrderMap;
   typedef TOrderMap::iterator                                   TOrderMapI;
   typedef Kway::Signon::TSignonSvrDB  TSignonSvrDB;

   // cooper 10.09.25 {
   //==============================================
   // class TSendOrdImpl
   class TSendOrdImpl : public ref_counter<>
   {
      typedef TSendOrdImpl              this_type;
      typedef std::vector<std::string>  TOrderV;
      typedef TOrderV::iterator         TOrderVI;
      typedef TTimer<this_type>         TTimerType;

      TFixSesMgr*   FixSesMgr_;
      TOrderV&      OrderV_;
      std::string   BrokerID_;
      int           SleepTime_;
      int           EachBatch_;
      int           CurrBatch_;
      TOrderVI      CurrOrderI_;
      TTimerType    Timer_;
      Kway::uint64  NextClock_;

   public:
             K_ctor  TSendOrdImpl (TFixSesMgr*, const std::string& brkID, TOrderV&);
      inline K_dtor ~TSendOrdImpl ()  { Timer_.Stop(); }

   private:
      K_mf(void) OnTimer (TTimerBase*);
   };
   friend class TSendOrdImpl;

   typedef wise_ref<TSendOrdImpl>         TSendOrdImplWsRef;
   typedef pod_vector<TSendOrdImplWsRef>  TSendOrdImplWsRefA;
      
   TSendOrdImplWsRefA  SendOrdImplA_;
   // } cooper 10.09.25

   TOTimer*    Timer_;
   K_mf(void)  OnTimer(TTimerBase*);
   
   TOTimer*    OrderTimer_;
   K_mf(void)  StartOrderTimer(size_t sec) { OrderTimer_->Start(sec*1000, &this_type::OnOrderTimer); }
   K_mf(void)  OnOrderTimer(TTimerBase*);
   
   TSignonSvrDB*   SignonDB_;
   
   int  SleepTime_, EachBatch_;
protected:
   TSesMap    Map_;
   TOrderMap  Orders_;
   
   K_mf(bool)  SendOrders();

public:
   K_ctor  TFixSesMgr();
   K_dtor  ~TFixSesMgr();
   
   K_mf(Kway::TViewAreaMgr*)  GetVAMgr() { return &VAMgr_; }
   K_mf(void)  RegTradSes(TStkSesFix* ses);
   K_mf(void)  UnRegTradSes(TStkSesFix* ses);
   K_mf(TStkSesFix*)  GetTradSes(const std::string& brokerID);
   K_mf(bool)         IsAnyTradSes() { return Map_.size()>0; }

   K_mf(bool)  IsSesIDExist(TStkSesFix* ses);
   K_mf(bool)  IsOnlineSesID(TStkSesFix* ses);
   K_mf(void)  Init(Kway::Signon::TSignonSvrDB* SG);
   K_mf(TSignonSvrDB*)  GetSignonDB() const { return SignonDB_; }
   K_mf(bool)  StartSesMgr();

};
//---------------------------------------------------------------------------
K_fn(TFixSesMgr&)  GetFixSesMgr();
//---------------------------------------------------------------------------
}//namespace Stk
}//namespace Fix
}//namespace Tw
}//namespace Kway
//---------------------------------------------------------------------------
#endif
