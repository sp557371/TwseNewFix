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
//---------------------------------------------------------------------------
namespace Kway {
//namespace Tw   {
//namespace Bx   {
//namespace FT   {
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
typedef  Kstl::ev_map<SKey4Prod, SK03Data2>    TK03Map;
typedef  TStdRecorder<TK03Map, SK03Data2>      TK03Recorder;

typedef  Kstl::ev_map<SKey4Prod, SK05Data2>    TK05Map;

typedef  Kstl::ev_map<int, std::string>             TPortsMap;
typedef  Kstl::ev_map<std::string, int>             TSesIDsMap;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
K_class  TwseFilesHandler
{
private:
   typedef  TwseFilesHandler          this_type;
   typedef  Kway::Tw::Stk::TMarket    TMarket;
   typedef  std::vector<std::string>  TStringV;
   typedef  TStringV::iterator        TStringVI;
   
   TK03Map*       K03Map_;
   TK03Recorder*  K03Recorder_;
   
   TK05Map*       K05Map_;
   std::string    IsK05Ready_;  //避免程式掛點,K05找不到
   TPortsMap*     PortsMap_;
   TSesIDsMap*    SesIDsMap_;
   
   std::string    debug;
   
   Kway::TTimer<this_type>  Timer_;
   K_mf(void)          StartTimer(size_t sec) { Timer_.Start(sec*1000, &this_type::OnTimer); }
   K_mf(void)          StopTimer() { Timer_.Stop(); }
   virtual K_mf(void)  OnTimer(Kway::TTimerBase*);   
      
protected:
   K_mf(std::string)  GetSocketPort(std::string& begPort, std::string& sessionID);
   K_mf(void)  InsertK03Map(SKey4Prod& key, SK03Data2& value);
   K_mf(void)  InsertK05Map(SKey4Prod& key, SK05Data2& value);
   K_mf(void)  RebuildK05Map(TKIniFile& main);
   
   //K_mf(bool)  FillSK01Data2(SK01Data2* data, std::string& source);
   
   K_mf(bool)  Read2Container(TFile* file, TFileType& type, TStringV& v);
   K_mf(void)  SetK01IniFile(SK01Data2& k01, TKIniFile& main, TKIniFile& xtwse, TKIniFile& appia, std::string& applyDate);
   K_mf(void)  SetK04IniFile(SK04Data2& k04, TKIniFile& trade, TMarket& mkt);
   K_mf(void)  ConfigOrSave(SK03Data2& k03, TKIniFile& main, TKIniFile& appia);
   K_mf(void)  ConfigOrSave(TMarket& mkt, SK05Data2& k05, TKIniFile& main, TKIniFile& appia);
   
   K_mf(void)  ProcessK01(TMarket& mkt, TFileType& type, TStringV& v);
   K_mf(void)  ProcessK03(TMarket& mkt, TFileType& type, TStringV& v);
   K_mf(void)  ProcessK04(TMarket& mkt, TFileType& type, TStringV& v);
   K_mf(void)  ProcessK05(TMarket& mkt, TFileType& type, TStringV& v, std::string& fullname);
   K_mf(void)  ProcessK06(TMarket& mkt, TFileType& type, TStringV& v);
   K_mf(void)  ProcessK07(TMarket& mkt, TFileType& type, TStringV& v);
   K_mf(void)  ProcessK08(TMarket& mkt, TFileType& type, TStringV& v);

public:
   //---<建構式與解構式>-----------------------------------------------------
   K_ctor  TwseFilesHandler();
   K_dtor  ~TwseFilesHandler();
   
   //---<接收處理檔案>-------------------------------------------------------
   K_mf(void)  StartHandleAFile(TMarket& mkt, std::string& fname, std::string& path);
   
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//};  // FT
//};  // Bx
//};  // Tw
};  // Kway
//---------------------------------------------------------------------------
#endif
