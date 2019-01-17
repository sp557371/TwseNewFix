//---------------------------------------------------------------------------
#ifndef ProcMonH
#define ProcMonH
//---------------------------------------------------------------------------
#include "ViewAreaMgr.h"
#include "ExtDef.h"
#include "stdSettingP.h"
#include "fixed_num.h"
#include "TimerThread.h"
#include "ExcMgrBase.h"
//---------------------------------------------------------------------------
namespace KStk = Kway::Tw::Stk;
namespace Kway{
using namespace Kstl;
//---------------------------------------------------------------------------
struct SProcItem{
   KStk::TBrokerID          BrokerID_;
   KStk::TMarket            Market_;
   bool                     StopMonit_;
   TKTime                   LastTime_;
   fixed_num<Kway::word>    Count_;
   fixed_num<Kway::word>    TotCount_;
   TKTime                   LastChkTime_;
   char_ary<char,40,' '>    Memo_;
   K_ctor SProcItem(){
                       StopMonit_ = true;
                       Count_     = 0;
                       TotCount_  = 0;
                     }
};
//---------------------------------------------------------------------------
typedef char_ary<char,10,' '>              TSesID;
typedef Kstl::ev_map<TSesID, SProcItem>    TProcItem;
//---------------------------------------------------------------------------
struct TProcItemFieldMaker
{
   typedef TProcItem                 TEvContainer;
   typedef TEvContainer::iterator   iterator;
   typedef size_t                   field_n_type;

   K_mf(const Kway::TFieldDesp&)  GetFieldDesp  (field_n_type n) const
          {
            static Kway::TFieldDesp f[] = {
                     { "對應FIX ID"   , 12, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_Fixed,  },
                     { "券商代號"     ,  8, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit,},
                     { "市場別"       ,  6, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit,},
                     { "停止監看"     ,  8, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit,},
                     { "最後啟動時間" , 12, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_Null,   },
                     { "連續失敗次數" , 12, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_Null,   },
                     { "總失敗次數"   , 12, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_Null,   },
                     { "最後檢查時間" , 12, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_Null,   },
                     { "備註"         , 20, Kway::ta_xCenter_yCenter, Kway::ta_xLeft_yCenter  , Kway::tfs_Null,}};
            return f[n];
          }
   inline K_mf(field_n_type)  GetFieldCount () const    { return 9; }

   K_mf(std::string)  Get  (const iterator& i, field_n_type fieldNo) const
         {
            char  buf[128];
            switch(fieldNo) {
               case 0:  return i->first.as_string();
               case 1:  return i->second.BrokerID_.as_string();
               case 2:  return i->second.Market_.as_string();
               case 3:  return i->second.StopMonit_?"Y":"";
               case 4:  return i->second.LastTime_.as_string();
               case 5:  return i->second.Count_.as_string();
               case 6:  return i->second.TotCount_.as_string();
               case 7:  return i->second.LastChkTime_.as_string();
               case 8:  return i->second.Memo_.as_string();
            }
            return std::string();
         }
   K_mf(void)  Put  (TEvContainer& c, iterator& iter, field_n_type fieldNo, const std::string& str)
         {
            if(fieldNo < 1 || fieldNo > 3)
               return;
            TEvContainer::updater u = c.begin_update(iter);
            switch(fieldNo) {
               case 1:  u->BrokerID_.assign(str); break;
               case 2:  u->Market_.assign(str); break;
               case 3:  u->StopMonit_ = (str[0]=='y' || str[0]=='Y')?true:false; break;
            }
         }
}; // class TSampleFieldMaker
//---------------------------------------------------------------------------
typedef TTViewArea<TProcItem, TProcItemFieldMaker> TProcItemVA;
//---------------------------------------------------------------------------
K_class TProcMon :public TVACreater,
                  public TCmdMgrObj
{
   typedef TProcMon          this_type;
   typedef TTimer<this_type> TPTimer;
private:
   TProcItem               ProcItem_;
   Kway::word              ChkInterval_;
   Kway::word              MaxCount_;
   bool                    AutoChk_;
   bool                    AutoStart_;
   std::string             ExecPathName_;
   TPTimer*                Timer_;
   K_mf(void)              OnTimer     (TTimerBase*);
   K_mf(void)              AddSession(std::string sid, KStk::TMarket, KStk::TBrokerID);
   K_mf(void)              DelSession(std::string sid, KStk::TMarket, KStk::TBrokerID);
   K_mf(bool)              ChknWake(std::string sid, KStk::TMarket, KStk::TBrokerID,bool chk=false);
public:
   TSettingPage   		   *MonConf_;
   K_ctor                  TProcMon();
   K_dtor                  ~TProcMon();
   K_mf(TaViewArea)        CreateNewVA();
   K_mf(bool)              DoCommand      (Kway::word cmdid, std::string msg = "");
};
//---------------------------------------------------------------------------
};
#endif

