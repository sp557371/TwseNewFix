//---------------------------------------------------------------------------
#ifndef TwseFilesHandlerH
#define TwseFilesHandlerH
//---------------------------------------------------------------------------
#include "KFile.h"
#include "Twse2KwayFiles.h"
#include "TwStk.h"
#include "TimerThread.h"
#include <vector>
#include "KIniFile.h"
#include "StdRecorder.h"
#include "ExcMgrBase.h"
#include "tinyxml/tinyxml.h"
#include "KTime.h"
#include "ViewArea.h"
#include "ViewAreaMgr.h"
#include "fixed_num.h"
#include "TwseFileLog.h"
#include "FTExtDef.h"
//---------------------------------------------------------------------------
namespace Kway {
//namespace Tw   {
//namespace Bx   {
//namespace FT   {
//---------------------------------------------------------------------------
/*
enum  EWKStatus
{
   wk_NotYet,  //檔案未到
   wk_NoNeed,  //不需檢查
   wk_Arrived, //已到達
   wk_Alerted, //已通知
};
//---------------------------------------------------------------------------
struct  SWorkCode
{
   TKTime                        CheckTime_;
   bool                          IsReceived_;
   EWKStatus                     IsChecked_;
   Kstl::char_ary<char, 128, 0>  Memo_;
   
   K_ctor  SWorkCode() { IsReceived_ = false;  IsChecked_ = wk_NotYet;  Memo_.clear(); }
};
//---------------------------------------------------------------------------
typedef  Kstl::fixed_num<uint32>                TWorkKey;
typedef  Kstl::ev_map<TWorkKey, SWorkCode>      TWorkCodeMap;
typedef  TStdRecorder<TWorkCodeMap, SWorkCode>  TWorkCodeRecorder;
typedef  TWorkCodeMap::iterator                 TWorkCodeMapI;
typedef  TWorkCodeMap::updater                  TWorkCodeMapU;
//---------------------------------------------------------------------------
K_class  TWorkCodeVAC : public Kway::TVACreater
{
   typedef Kway::TVACreater         inherited;
   typedef TWorkCodeVAC             this_type;

   K_class TWorkCodeFieldMaker
   {
      typedef TWorkCodeMap              TEvContainer;
      typedef TEvContainer::iterator    iterator;
      typedef size_t                    field_n_type;
   public:
      K_mf(const Kway::TFieldDesp&)   GetFieldDesp(field_n_type n) const;
      K_mf(field_n_type)       GetFieldCount() const;
      K_mf(std::string)   Get(const iterator& i, field_n_type fieldNo) const;
      K_mf(void)          Put(TEvContainer& c,iterator& iter,field_n_type fieldNo, const std::string& str);
   };
   TWorkCodeMap*  Recs_;  //Master 的container

public:
   K_ctor  TWorkCodeVAC(TWorkCodeMap& map) : Recs_(&map) { }
   typedef Kway::TTViewArea<TWorkCodeMap, TWorkCodeFieldMaker>   TWorkCodeViewer;

   K_mf(Kway::TaViewArea)  CreateNewVA() { return Kway::TaViewArea(new TWorkCodeViewer(Recs_)); }
};
*/
//---------------------------------------------------------------------------
struct  SSessionID 
{
   TStr1  Market_;
   TStr4  Broker_;
   TStr2  PVCID_;
   
   inline bool  operator< (const SSessionID& a) const { return (Market_==a.Market_ ? (Broker_==a.Broker_ ? PVCID_<a.PVCID_ : Broker_<a.Broker_) 
                                                                                   : Market_<a.Market_); }
   inline bool  operator==(const SSessionID& a) const { return (Market_==a.Market_ && Broker_==a.Broker_ && PVCID_==a.PVCID_); }
   
   inline K_mf(std::string)  as_string() const { return std::string(Market_.as_string()+Broker_.as_string()+PVCID_.as_string()); }
};
//---------------------------------------------------------------------------
struct  SKey4Prod
{
   TStr1       Type_;
   SSessionID  SesID_;
   
   inline bool  operator< (const SKey4Prod& a) const { return (Type_==a.Type_ ? SesID_<a.SesID_ : Type_<a.Type_);}
   inline bool  operator==(const SKey4Prod& a) const { return (Type_==a.Type_ && SesID_==a.SesID_); }
   inline K_mf(std::string)  GetSesID() const { return std::string(SesID_.as_string()); } 
};
//---------------------------------------------------------------------------
struct  SProdValues
{
   TKDate     OnlineDate_;
   TKDate     SettingDate_;  //避免當日傳兩次F03,需以後面一次為主
   SF03Data2  F03Data2_;
   
   K_ctor  SProdValues() { OnlineDate_.Clear();   SettingDate_.Clear(); }
};
//---------------------------------------------------------------------------
struct  SKey4ProdSort
{
   TKDate      Date_;
   dword       Seq_;  //insert序號         
   TStr1       Type_;
   SSessionID  SesID_;
   
   inline bool  operator< (const SKey4ProdSort& a) const { return (Date_==a.Date_ ? Seq_<a.Seq_ : Date_<a.Date_);}
   inline bool  operator==(const SKey4ProdSort& a) const { return (Date_==a.Date_ && Seq_==a.Seq_); }
   inline K_mf(std::string)  GetSesID() const { return std::string(SesID_.as_string()); } 
};
//---------------------------------------------------------------------------
struct  SF01Values
{
   TKDate     ApplyDate_;
   SF01Data2  F01Data2_;
};
//---------------------------------------------------------------------------
/*
struct  SF13Key
{
   TStr10  Broker_;    //證券商代號
   TStr2   FixPVCID_;  //FIX Socket ID
   
   inline bool  operator< (const SF13Key& a) const { return (Broker_==a.Broker_ ? FixPVCID_<a.FixPVCID_ : Broker_<a.Broker_);}
   inline bool  operator==(const SF13Key& a) { return (Broker_==a.Broker_ && FixPVCID_==a.FixPVCID_); }
   
   K_mf(std::string)  as_string() const { return std::string(Broker_.as_string()+FixPVCID_.as_string()); }
};
//---------------------------------------------------------------------------
struct  SF13Value
{
   TStr1   BfFixVer_;  //FIX版本別, 1=4.2, 2=4.4
   TStr1   AfFixVer_;  //FIX版本別, 1=4.2, 2=4.4
};
*/
//---------------------------------------------------------------------------
typedef  Kstl::ev_map<std::string, TKIniFile*>     TKIniFileMap; //F03,F05依appia群組及FW群組,寫到不同的twse.ini.APPxx及appia-smp.ini.FWxxx
typedef  Kstl::ev_map<SKey4ProdSort, SProdValues>  TF03Map;  //[improve]20100810-避免排序問題
typedef  TStdRecorder<TF03Map, SProdValues>        TF03Recorder;

typedef  Kstl::ev_map<SKey4Prod, SF05Data2>    TF05Map;

typedef  Kstl::ev_map<TStr4, TStr4>            TF11Map;
typedef  TStdRecorder<TF11Map, TStr4>          TF11Recorder;
typedef  TF11Map::iterator                     TF11MapI;
typedef  TF11Map::updater                      TF11MapU;
/*
typedef  Kstl::ev_map<SF13Key, SF13Value>      TF13Map;
typedef  TStdRecorder<TF13Map, SF13Value>      TF13Recorder;
typedef  TF13Map::iterator                     TF13MapI;
typedef  TF13Map::updater                      TF13MapU;
*/
typedef  Kstl::ev_map<SKey4Prod, SF01Values>   TF01Map;
typedef  TStdRecorder<TF01Map, SF01Values>     TF01Recorder;

typedef  Kstl::ev_map<int, std::string>        TPortsMap;
typedef  Kstl::ev_map<std::string, int>        TSesIDsMap;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
K_class  TwseFilesHandler //: public TViewAreaMgr
{
private:
   typedef  TwseFilesHandler          this_type;
   typedef  Kway::Tw::Stk::TMarket    TMarket;
   typedef  std::vector<std::string>  TStringV;
   typedef  TStringV::iterator        TStringVI;
   
   TwseFileLog*   Logger_;
   TF03Map*       F03Map_;
   TF03Recorder*  F03Recorder_;
   
   TF01Map*       F01Map_;
   TF01Recorder*  F01Recorder_;
   
   TF05Map*       F05Map_;
   std::string    IsTF05Ready_;  //避免程式掛點,F05找不到
   std::string    IsOF05Ready_;  //避免程式掛點,F05找不到
   std::string    IsTF03Ready_;
   std::string    IsOF03Ready_;
   std::string    IsTF11Ready_;
   std::string    IsOF11Ready_;
   std::string    IsTF11Done_;   //確認是否已經執行過F11
   std::string    IsOF11Done_;
   TF11Map*       F11Map_;
   TF11Recorder*  F11Recorder_;
   //TPortsMap*     PortsMap_;
   //TSesIDsMap*    SesIDsMap_;
   //std::string    IsTF13Ready_;
   //std::string    IsOF13Ready_;
   //std::string    IsTF13Done_;   //確認是否已經執行過F13
   //std::string    IsOF13Done_;
   //TF13Map*       F13Map_;
   //TF13Recorder*  F13Recorder_;
   
   TExcMgrBase*   Mgr_;
   TFTSenderBase* FTSender_;
   //TWorkCodeMap*       WorkCodeMap_;
   //TWorkCodeRecorder*  WorkCodeRecorder_;
   //TWorkCodeVAC*       WorkCodeVAC_;
   
   std::string    debug;
   
   Kway::TTimer<this_type>  Timer_;
   K_mf(void)          StartTimer(size_t sec) { Timer_.Start(sec*1000, &this_type::OnTimer); }
   K_mf(void)          StopTimer() { Timer_.Stop(); }
   virtual K_mf(void)  OnTimer(Kway::TTimerBase*);  
   
   Kway::TTimer<this_type>  F10Timer_;
   K_mf(void)          StartF10Timer(size_t sec) { F10Timer_.Start(sec*1000, &this_type::OnF10Timer); }
   K_mf(void)          StopF10Timer() { F10Timer_.Stop(); }
   virtual K_mf(void)  OnF10Timer(Kway::TTimerBase*);  
   /*
   Kway::TTimer<this_type>  WKTimer_;
   K_mf(void)          StartWKTimer(size_t sec) { WKTimer_.Start(sec*1000, &this_type::OnWKTimer); }
   K_mf(void)          StopWKTimer() { WKTimer_.Stop(); }
   virtual K_mf(void)  OnWKTimer(Kway::TTimerBase*);
   */
   Kway::TTimer<this_type>  F11Timer_;
   K_mf(void)          StartF11Timer(size_t sec);// { F11Timer_.Start(sec*1000, &this_type::OnF11Timer); }
   K_mf(void)          StopF11Timer();// { F11Timer_.Stop(); }
   virtual K_mf(void)  OnF11Timer(Kway::TTimerBase*);  
/*
   Kway::TTimer<this_type>  F13Timer_;
   K_mf(void)          StartF13Timer(size_t sec);
   K_mf(void)          StopF13Timer();
   virtual K_mf(void)  OnF13Timer(Kway::TTimerBase*);  
*/
   //K_mf(void)  CreateWKMap();
   //K_mf(void)  EraseSettingPort(std::string& sessionID);
   K_mf(void)  WriteMsg(const char*, const char*);
      
protected:
   //K_mf(std::string)  GetSocketPort(std::string& begPort, std::string& sessionID);
   K_mf(void)  CheckF03Map();
   //K_mf(void)  SetOnlineDate(SKey4Prod& key);
   K_mf(void)  InsertF03Map(SKey4ProdSort& key, SProdValues& value);
   K_mf(void)  SetOnlineDate(SKey4ProdSort& key);
   K_mf(void)  InsertF05Map(SKey4Prod& key, SF05Data2& value);
   K_mf(void)  RebuildF05Map(TKIniFile& main);
   
   K_mf(void)  CheckF03Today();
   K_mf(bool)  CheckF03orF05(TMarket& mkt, TFileType type);
   K_mf(bool)  ChkNInsertData(SF01Data2& f01, std::string& applyDate, TKIniFile& main);
   K_mf(bool)  ChkNInsertData(SF03Data2& f03, TKDate& today, dword& seq, TKIniFile& main);
   K_mf(bool)  ChkNInsertData(SF05Data2& f05, TMarket& mkt, TKIniFile& main);
   K_mf(bool)  ConfigTester();
   K_mf(void)  ConfigProduction();
   K_mf(void)  ConfigF11();
   //K_mf(void)  ConfigF13();
   
   K_mf(bool)  Read2Container(TFile* file, TFileType& type, TStringV& v);
   K_mf(bool)  SetF01IniFile(SF01Data2& f01, TKIniFile& main, TKIniFile& xtwse, TKIniFile& appia, std::string& applyDate);
   K_mf(bool)  ConfigOrSave(SKey4ProdSort& keyProd, SF03Data2& f03, TKIniFile& main, TKIniFile& appia, TKIniFile& passwords);
   // 20131031 交易所決定將密碼檔passwords.ini每天統一由F05的資料重新產生
   K_mf(void)  ConfigPasswords(TKIniFile& main, TKIniFile& passwords);
   //F05 SessionID 寫入 appia-smp.ini 給forwarder使用
   K_mf(void)  ConfigAppiaSmp(TKIniFile& main);
   K_mf(void)  ConfigFixVer(TKIniFile& main);
   K_mf(bool)  ConfigThrottle(TKIniFile& main);
//   K_mf(bool)  Throttling(TSesIDsMap& map, std::string& path, const char* buf);
   K_mf(bool)  Throttling(TSesIDsMap& map, std::string& path);
   K_mf(std::string)  MakeString(std::string& orig, TSesIDsMap& map);
   K_mf(void)  ReadMultiplierS(TKIniFile& main, TSesIDsMap& map);
   
   //F01,F03,F05寫入Backup/update/Main.ini or Backup/update/XTester.ini
   //F04,F06,F08寫入Settings/Main.ini or Settings/TradeSegment.ini
   //Appia相關寫入Backup/update/twse.ini
   K_mf(void)  ProcessF01(TMarket& mkt, TFileType& type, TStringV& v);
   K_mf(void)  ProcessF03(TMarket& mkt, TFileType& type, TStringV& v, std::string& fullname);
   K_mf(void)  ProcessF04(TMarket& mkt, TFileType& type, TStringV& v);
   K_mf(void)  ProcessF05(TMarket& mkt, TFileType& type, TStringV& v, std::string& fullname, bool isRebuild=false);
   K_mf(void)  ProcessF06(TMarket& mkt, TFileType& type, TStringV& v);
   K_mf(void)  ProcessF07(TMarket& mkt, TFileType& type, TStringV& v);
   K_mf(void)  ProcessF08(TMarket& mkt, TFileType& type, TStringV& v);
   //K_mf(void)  ProcessK09(TMarket& mkt, TFileType& type, TStringV& v);
   K_mf(void)  ProcessF11(TMarket& mkt, TFileType& type, TStringV& v);
   //K_mf(void)  ProcessF12(TMarket& mkt, TFileType& type, TStringV& v);
   //K_mf(void)  ProcessF13(TMarket& mkt, TFileType& type, TStringV& v);
   
   //K_mf(void)  SetWKReceived(int);
   K_mf(std::string)  GetTradeAccount(std::string& acc, std::string& brk);
   K_mf(void)  GenerateF10();
   K_mf(void)  WriteDefaultAppia(TKIniFile&, std::string&, bool isprod = true);

public:
   //---<建構式與解構式>-----------------------------------------------------
   K_ctor  TwseFilesHandler();
   K_dtor  ~TwseFilesHandler();
   
   //---<接收處理檔案>-------------------------------------------------------
   K_mf(void)  StartHandleAFile(TMarket& mkt, std::string& fname, std::string& path, bool isRebuild=false);
   
   //K_mf(TWorkCodeVAC*)  GetVAC() { return WorkCodeVAC_; }
   K_mf(void)  SetFTSender(TFTSenderBase* fts);
   
};
//---------------------------------------------------------------------------
//};  // FT
//};  // Bx
//};  // Tw
};  // Kway
//---------------------------------------------------------------------------
#endif
