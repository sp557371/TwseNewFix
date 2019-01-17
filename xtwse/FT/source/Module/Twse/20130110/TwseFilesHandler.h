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
   TKDate     SettingDate_;  //避免當日傳兩次K03,需以後面一次為主
   SK03Data2  K03Data2_;
   
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
struct  SK01Values
{
   TKDate     ApplyDate_;
   SK01Data2  K01Data2_;
};
//---------------------------------------------------------------------------
struct  SK13Key
{
   TStr10  Broker_;    //證券商代號
   TStr2   FixPVCID_;  //FIX Socket ID
   
   inline bool  operator< (const SK13Key& a) const { return (Broker_==a.Broker_ ? FixPVCID_<a.FixPVCID_ : Broker_<a.Broker_);}
   inline bool  operator==(const SK13Key& a) { return (Broker_==a.Broker_ && FixPVCID_==a.FixPVCID_); }
   
   K_mf(std::string)  as_string() const { return std::string(Broker_.as_string()+FixPVCID_.as_string()); }
};
//---------------------------------------------------------------------------
struct  SK13Value
{
   TStr1   BfFixVer_;  //FIX版本別, 1=4.2, 2=4.4
   TStr1   AfFixVer_;  //FIX版本別, 1=4.2, 2=4.4
};
//---------------------------------------------------------------------------
//typedef  Kstl::ev_map<SKey4Prod, SK03Data2>    TK03Map;
//typedef  TStdRecorder<TK03Map, SK03Data2>      TK03Recorder;
//typedef  Kstl::ev_map<SKey4Prod, SProdValues>  TK03Map;
typedef  Kstl::ev_map<SKey4ProdSort, SProdValues>  TK03Map;  //[improve]20100810-避免排序問題
typedef  TStdRecorder<TK03Map, SProdValues>        TK03Recorder;

typedef  Kstl::ev_map<SKey4Prod, SK05Data2>    TK05Map;

typedef  Kstl::ev_map<TStr4, TStr4>            TK11Map;
typedef  TStdRecorder<TK11Map, TStr4>          TK11Recorder;
typedef  TK11Map::iterator                     TK11MapI;
typedef  TK11Map::updater                      TK11MapU;

typedef  Kstl::ev_map<SK13Key, SK13Value>      TK13Map;
typedef  TStdRecorder<TK13Map, SK13Value>      TK13Recorder;
typedef  TK13Map::iterator                     TK13MapI;
typedef  TK13Map::updater                      TK13MapU;

typedef  Kstl::ev_map<SKey4Prod, SK01Values>   TK01Map;
typedef  TStdRecorder<TK01Map, SK01Values>     TK01Recorder;

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
   TK03Map*       K03Map_;
   TK03Recorder*  K03Recorder_;
   
   TK01Map*       K01Map_;
   TK01Recorder*  K01Recorder_;
   
   TK05Map*       K05Map_;
   std::string    IsTK05Ready_;  //避免程式掛點,K05找不到
   std::string    IsOK05Ready_;  //避免程式掛點,K05找不到
   //std::string    IsK03Ready_;
   std::string    IsTK03Ready_;
   std::string    IsOK03Ready_;
   std::string    IsTK11Ready_;
   std::string    IsOK11Ready_;
   std::string    IsTK11Done_;   //確認是否已經執行過K11
   std::string    IsOK11Done_;
   TK11Map*       K11Map_;
   TK11Recorder*  K11Recorder_;
   //TPortsMap*     PortsMap_;
   //TSesIDsMap*    SesIDsMap_;
   std::string    IsTK13Ready_;
   std::string    IsOK13Ready_;
   std::string    IsTK13Done_;   //確認是否已經執行過K13
   std::string    IsOK13Done_;
   TK13Map*       K13Map_;
   TK13Recorder*  K13Recorder_;
   
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
   
   Kway::TTimer<this_type>  K10Timer_;
   K_mf(void)          StartK10Timer(size_t sec) { K10Timer_.Start(sec*1000, &this_type::OnK10Timer); }
   K_mf(void)          StopK10Timer() { K10Timer_.Stop(); }
   virtual K_mf(void)  OnK10Timer(Kway::TTimerBase*);  
   /*
   Kway::TTimer<this_type>  WKTimer_;
   K_mf(void)          StartWKTimer(size_t sec) { WKTimer_.Start(sec*1000, &this_type::OnWKTimer); }
   K_mf(void)          StopWKTimer() { WKTimer_.Stop(); }
   virtual K_mf(void)  OnWKTimer(Kway::TTimerBase*);
   */
   Kway::TTimer<this_type>  K11Timer_;
   K_mf(void)          StartK11Timer(size_t sec);// { K11Timer_.Start(sec*1000, &this_type::OnK11Timer); }
   K_mf(void)          StopK11Timer();// { K11Timer_.Stop(); }
   virtual K_mf(void)  OnK11Timer(Kway::TTimerBase*);  

   Kway::TTimer<this_type>  K13Timer_;
   K_mf(void)          StartK13Timer(size_t sec);
   K_mf(void)          StopK13Timer();
   virtual K_mf(void)  OnK13Timer(Kway::TTimerBase*);  

   //K_mf(void)  CreateWKMap();
   //K_mf(void)  EraseSettingPort(std::string& sessionID);
   K_mf(void)  WriteMsg(const char*, const char*);
      
protected:
   //K_mf(std::string)  GetSocketPort(std::string& begPort, std::string& sessionID);
   K_mf(void)  CheckK03Map();
   //K_mf(void)  InsertK03Map(SKey4Prod& key, SK03Data2& value);
   //K_mf(void)  InsertK03Map(SKey4Prod& key, SProdValues& value);
   //K_mf(void)  SetOnlineDate(SKey4Prod& key);
   K_mf(void)  InsertK03Map(SKey4ProdSort& key, SProdValues& value);
   K_mf(void)  SetOnlineDate(SKey4ProdSort& key);
   K_mf(void)  InsertK05Map(SKey4Prod& key, SK05Data2& value);
   K_mf(void)  RebuildK05Map(TKIniFile& main);
   
   //K_mf(bool)  CheckK05();
   //K_mf(bool)  CheckK03(TMarket& mkt);
   K_mf(void)  CheckK03Today();
   K_mf(bool)  CheckK03orK05(TMarket& mkt, TFileType type);
   K_mf(bool)  ChkNInsertData(SK01Data2& k01, std::string& applyDate);
   K_mf(bool)  ChkNInsertData(SK03Data2& k03, TKDate& today, dword& seq);
   K_mf(bool)  ChkNInsertData(SK05Data2& k05, TMarket& mkt);
   K_mf(bool)  ConfigTester();
   K_mf(void)  ConfigProduction();
   K_mf(void)  ConfigK11();
   K_mf(void)  ConfigK13();
   
   K_mf(bool)  Read2Container(TFile* file, TFileType& type, TStringV& v);
   K_mf(bool)  SetK01IniFile(SK01Data2& k01, TKIniFile& main, TKIniFile& xtwse, TKIniFile& appia, std::string& applyDate);
   K_mf(void)  SetK04IniFile(SK04Data2& k04, TKIniFile& trade, TMarket& mkt);
   K_mf(bool)  ConfigOrSave(SKey4ProdSort& key, SK03Data2& k03, TKIniFile& main, TKIniFile& appia);
   //K_mf(bool)  ConfigOrSave(SKey4Prod& key, SK03Data2& k03, TKIniFile& main, TKIniFile& appia);
   //K_mf(void)  ConfigOrSave(TMarket& mkt, SK05Data2& k05, TKIniFile& main, TKIniFile& appia);
   K_mf(bool)  ConfigThrottle(TKIniFile& main, TKIniFile& appia);
   K_mf(bool)  Throttling(TSesIDsMap& map, std::string& path, const char* buf);
   K_mf(std::string)  MakeString(std::string& orig, TSesIDsMap& map);
   K_mf(void)  ReadMultiplierS(TKIniFile& main, TSesIDsMap& map);
   
   //K01,K03,K05寫入Backup/update/Main.ini or Backup/update/XTester.ini
   //K04,K06,K08寫入Settings/Main.ini or Settings/TradeSegment.ini
   //Appia相關寫入Backup/update/twse.ini
   K_mf(void)  ProcessK01(TMarket& mkt, TFileType& type, TStringV& v);
   K_mf(void)  ProcessK03(TMarket& mkt, TFileType& type, TStringV& v, std::string& fullname);
   K_mf(void)  ProcessK04(TMarket& mkt, TFileType& type, TStringV& v);
   K_mf(void)  ProcessK05(TMarket& mkt, TFileType& type, TStringV& v, std::string& fullname, bool isRebuild=false);
   K_mf(void)  ProcessK06(TMarket& mkt, TFileType& type, TStringV& v);
   K_mf(void)  ProcessK07(TMarket& mkt, TFileType& type, TStringV& v);
   K_mf(void)  ProcessK08(TMarket& mkt, TFileType& type, TStringV& v);
   K_mf(void)  ProcessK09(TMarket& mkt, TFileType& type, TStringV& v);
   K_mf(void)  ProcessK11(TMarket& mkt, TFileType& type, TStringV& v);
   K_mf(void)  ProcessK12(TMarket& mkt, TFileType& type, TStringV& v);
   K_mf(void)  ProcessK13(TMarket& mkt, TFileType& type, TStringV& v);
   
   //K_mf(void)  SetWKReceived(int);
   K_mf(std::string)  GetTradeAccount(std::string& acc, std::string& brk);
   K_mf(void)  GenerateK10();
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
