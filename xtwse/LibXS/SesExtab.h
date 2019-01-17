//---------------------------------------------------------------------------
/* 資料表同步機制,與 KPTS-ExTable 資料表同步格式相容:
(Modifier)GroupName:TableName:Key:[FieldNo] BfStr => AfStr
          !!!!!!!!! !!!!!!!!! ^^^  ^^^^^^^  ^^^^^    ^^^^^
(Modifier)GroupName:MasterTableName:MasterKey:DetailGroup:DetailTableName:DetailKey:[FieldNo] BfStr => AfStr
          !!!!!!!!! !!!!!!!!!!!!!!! ^^^^^^^^^ !!!!!!!!!!! !!!!!!!!!!!!!!! ^^^^^^^^^  ^^^^^^^  ^^^^^    ^^^^^
範例：
(*999)Cust:CustBase:6160-000001-2:[0] 中xxxxxxxx => 中信
(*999)Cust:ScDetail:6160-000001-2:ScRec:Bal:1101:[0]  => 5,000
(*999)Cust:ScDetail:6160-000001-2:ScRec:Order:1101:[0]  => 5,000
(*999)Stk:Base:1101:[0] 臺  泥 => 臺泥
(*999)Stk:SfDetail:1101:Sf:SfData:1-1復華:[2]  => y

(Console/OP)LnmUser:LnUser:6160/999:[1] 999 => aaa
(Console/OP)LnmUser:LnMCustDet:6160/999:LnMCust:LnMCust:6160-000001-2:[Add]
(Console/OP)LnmUser:LnMCustDet:6160/999:LnMCust:LnMCust:6160-000001-2:[1] 0 => 1
(Console/OP)LnmUser:LnMCustDet:6160/999:LnMCust:LnMCust:6160-000001-2:[Del]
*/
//
// $Id: SesExtab.h,v 1.3 2005/04/26 07:00:01 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SesExtabH
#define SesExtabH
//---------------------------------------------------------------------------
#include "SesBufferLine.h"
#include "KFile.h"
#include "VALogger.h"   // class TTableLog;   class TTableLog2d;
#include "pod_vector.h"
#include <map>
#include <string>
#include <set>
//---------------------------------------------------------------------------
namespace Kway {
namespace Extab {
/////////////////////////////////////////////////////////////////////////////
struct TRecLoc
{
   std::string    GroupName_;
   std::string    TableName_;
   std::string    Key_;
   TRecLoc*       Detail_;
};
//---------------------------------------------------------------------------
struct TRecDat
{
   size_t         FieldNo_;
   std::string    BfStr_;
   std::string    AfStr_;

   static const size_t  fn_AddDefault = static_cast<size_t>(-1);
   static const size_t  fn_DelRec     = static_cast<size_t>(-2);
};
//---------------------------------------------------------------------------
struct TRecMod : public TRecLoc,
                 public TRecDat
{
   std::string   Modifier_;
};
/////////////////////////////////////////////////////////////////////////////
class TExtabUpdaterMgr;
//---------------------------------------------------------------------------
class TExtabUpdater
{
   virtual K_mf(void)  OnUpdate  (const TRecMod&) = 0;
   friend class TExtabUpdaterMgr;
public:
   virtual K_dtor  ~TExtabUpdater  ()   {}
};
/////////////////////////////////////////////////////////////////////////////
struct TExtabLoc
{
   const char*    GroupName_;
   const char*    TableName_;
};
//---------------------------------------------------------------------------
LIBNS_class TExtabFilter
{
public:
   virtual K_mf(bool) OnExtabFilter (TExtabUpdaterMgr&, const std::string& regname, const TRecMod&) = 0;
};
//---------------------------------------------------------------------------
LIBNS_class TExtabUpdaterMgr
{
   typedef Kstl::pod_vector<TExtabUpdater*>        TExtabUpdaters;
   typedef std::map<std::string, TExtabUpdaters>   TUpdaters;
   TUpdaters   Updaters_;
   TFile       LogFile_;
public:
   //相同名稱 [可以] 有多個 TExtabUpdater
   K_mf(bool)  Add  (TExtabUpdater*, const TExtabLoc* begin, const TExtabLoc* end);
   K_mf(bool)  Del  (TExtabUpdater*);
   K_mf(bool)  RecvLine  (const std::string&, TExtabFilter*);

   K_mf(bool)  SetLogName  (const std::string&, std::string* msg);
   K_mf(void)  WriteLine   (const TRecMod&);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Extab
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSesExtab : public TSesTextline, public Extab::TExtabFilter
{
   typedef TSesTextline    inherited;
   K_mf(void)  OnTextline  (const char*, size_t sz);
public:
   K_ctor  TSesExtab  (Extab::TExtabUpdaterMgr&);

   K_mf(bool)  SettingApply  (const std::string&);

   K_mf(bool) OnExtabFilter (Extab::TExtabUpdaterMgr&, const std::string& regname, const Extab::TRecMod&);
private:
   Extab::TExtabUpdaterMgr*   Mgr_;
   bool                       IsDenyAll_;

   typedef std::set<size_t>   TFieldNos;
   struct TFilter {
      bool        IsForDeny_;
      TFieldNos   FieldNos_;
   };
   typedef std::map<std::string, TFilter> TFilters;
   TFilters Filters_;
};
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TExtabLog : public TTableLog
{
protected:
   Extab::TExtabUpdaterMgr*   Mgr_;
   Extab::TRecMod             Mod_;
public:
   K_ctor      TExtabLog  (Extab::TExtabUpdaterMgr&,
                           const std::string& groupName, const std::string& tableName);
   K_mf(void)  WriteMod   (const std::string& key,
                           size_t             fieldNo,
                           const std::string& bf,
                           const std::string& af,
                           TViewArea* va);
   K_mf(void)  WriteAdd   (const std::string& key, TViewArea* va);
   K_mf(void)  WriteDel   (const std::string& key, TViewArea* va);
   inline K_mf(void)  SetDetail  (Extab::TRecLoc* d)  { Mod_.Detail_ = d; }
   inline K_mf(Extab::TExtabUpdaterMgr*)  GetMgr () const   { return Mgr_; }
};
//---------------------------------------------------------------------------
LIBNS_class TExtabLog2d : public TTableLog2d
{
protected:
   TExtabLog         Master_;
   Extab::TRecLoc    Detail_;
public:
   K_ctor   TExtabLog2d  (Extab::TExtabUpdaterMgr&,
                          const std::string& mGroupName, const std::string& mTableName,
                          const std::string& dGroupName, const std::string& dTableName);
   K_mf(void)  WriteMod  (const std::string& masterKey,
                          const std::string& detailKey,
                          size_t             fieldNo,
                          const std::string& bf,
                          const std::string& af,
                          TViewArea* va);
   K_mf(void)  WriteAdd  (const std::string& masterKey, const std::string& detailKey, TViewArea* va);
   K_mf(void)  WriteDel  (const std::string& masterKey, const std::string& detailKey, TViewArea* va);

   inline K_mf(Extab::TExtabUpdaterMgr*)  GetMgr () const   { return Master_.GetMgr(); }
};
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
template <class TBaseLogger, class TBaseUpdater>
struct TExtabLogUpdater : public TBaseLogger, public TBaseUpdater
{
   K_ctor  TExtabLogUpdater  (Extab::TExtabUpdaterMgr&  mgr,
                              const std::string& groupName,
                              const std::string& tableName)
         : TBaseLogger(mgr, groupName, tableName)
         { Extab::TExtabLoc loc[] = { { groupName.c_str(), tableName.c_str() } };
           mgr.Add(this, loc, loc+numofele(loc));
         }
   K_ctor  TExtabLogUpdater  (Extab::TExtabUpdaterMgr&  mgr,
                              const std::string& mGroupName,
                              const std::string& mTableName,
                              const std::string& dGroupName,
                              const std::string& dTableName)
         : TBaseLogger(mgr, mGroupName, mTableName, dGroupName, dTableName)
         { Extab::TExtabLoc loc[] = { { mGroupName.c_str(), mTableName.c_str() },
                                      { dGroupName.c_str(), dTableName.c_str() }, };
           mgr.Add(this, loc, loc+numofele(loc));
         }
};
//===========================================================================
template <class TFieldMaker>
class TTExtabUpdater : public Extab::TExtabUpdater
{
public:
   typedef typename TFieldMaker::TEvContainer   TRecs;
   typedef typename TRecs::iterator             Iterator;
   inline K_mf(void)    SetRecs  (TRecs* recs)    { Recs_ = recs; }
   inline K_mf(TRecs*)  GetRecs  () const         { return Recs_; }

   K_mf(void)  OnUpdate  (const Extab::TRecMod& mod)
   {
      typename TRecs::key_type  key;
      key.assign(mod.Key_);
      switch(mod.FieldNo_) {
      case Extab::TRecDat::fn_AddDefault:
                  (*Recs_)[key];
                  break;
      case Extab::TRecDat::fn_DelRec:
                  Recs_->erase(key);
                  break;
      default:    (*Recs_)[key];
                  Iterator i = Recs_->find(key);
                  if(i==Recs_->end())
                     break;
                  TFieldMaker   fields;
                  size_t        fieldn = FieldNoFromLog(mod.FieldNo_);
                  //if(fields.Get(i, fieldn) == mod.BfStr_)
                     fields.Put(*Recs_, i, fieldn, mod.AfStr_);
                  break;
      }
   }
private:
   TRecs*  Recs_;
   virtual K_mf(size_t)  FieldNoFromLog  (size_t n) const   { return n; }
};
//---------------------------------------------------------------------------
template <class TMasterRecs, class TDetailFieldMaker>
class TTExtabUpdater2d : public Extab::TExtabUpdater
{
public:
   typedef typename TDetailFieldMaker::TEvContainer   TDetailRecs;

   inline K_ctor      TTExtabUpdater2d() : MasterRecs_(0)   { }
   inline K_mf(void)  SetMasterRecs  (TMasterRecs* recs)    { MasterRecs_ = recs; }

   K_mf(void)  OnUpdate  (const Extab::TRecMod& mod)
   {
      typename TMasterRecs::key_type  mkey;
      typename TDetailRecs::key_type  dkey;
      mkey.assign(mod.Key_);
      dkey.assign(mod.Detail_->Key_);
      switch(mod.FieldNo_) {
      case Extab::TRecDat::fn_AddDefault:
                  (*MasterRecs_)[mkey][dkey];
                  break;
      case Extab::TRecDat::fn_DelRec: {
                  typename TMasterRecs::iterator  i = MasterRecs_->find(mkey);
                  if(i==MasterRecs_->end())
                     break;
                  i->second.erase(dkey);
                  } break;
      default:    {
                  (*MasterRecs_)[mkey][dkey];
                  typename TMasterRecs::iterator  mi = MasterRecs_->find(mkey);
                  if(mi==MasterRecs_->end())
                     break;
                  TDetailRecs&                     drecs = mi->second;
                  typename TDetailRecs::iterator   di    = drecs.find(dkey);
                  if(di==drecs.end())
                     break;
                  TDetailFieldMaker   fields;
                  size_t              fieldn = FieldNoFromLog(mod.FieldNo_);
                  //if(fields.Get(di, fieldn) == mod.BfStr_)
                     fields.Put(drecs, di, fieldn, mod.AfStr_);
                  } break;
      }
   }
private:
   TMasterRecs*  MasterRecs_;
   virtual K_mf(size_t)  FieldNoFromLog  (size_t n) const   { return n; }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

