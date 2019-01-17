//---------------------------------------------------------------------------
//#pragma hdrstop
#include "ExtDef.h"
#include "KIniFile.h"
//---------------------------------------------------------------------------
//#pragma package(smart_init)
using namespace Kway;
//---------------------------------------------------------------------------
static Kway::Tw::Stk::THostID   HostID;
static std::string              ProcName_;
static std::string              ProgName_;
static TSegDefs                 SegDefs_;
static std::string              ConfPath_;
static std::string              LogPath_;
static Kway::Tw::Stk::TMarket   Market_(Kway::Tw::Stk::m_Unknow);
static Kway::Tw::Stk::TBrokerID BrokerID_;
static Kway::byte               DebugLevel_(255);
K_gd(TGetConfig)          Kway::GetLogPath;
K_gd(TGetConfig)          Kway::GetProcLogPath;
K_gd(TGetConfig)          Kway::GetConfPath;
static bool                     TMPReady_(false);
static bool                     AllTMPReady_(false);
static bool                     DRTMPReady_(false);
//---------------------------------------------------------------------------
K_fn(std::string) GetProcName()
{
   return ProcName_;
}
//---------------------------------------------------------------------------
K_fn(void)    SetProcName(std::string pn)
{
   ProcName_ = pn;
}
//---------------------------------------------------------------------------
K_fn(std::string) GetProgName()
{
   return ProgName_;
}
//---------------------------------------------------------------------------
K_fn(void)    SetProgName(std::string pn)
{
   ProgName_ = pn;
}
//---------------------------------------------------------------------------
K_mf(const std::string) DoGetConfPath()
{
   if(ConfPath_.empty()){
      TKIniFile KIniFile("./Settings/Main.ini");
      ConfPath_  = KIniFile.ReadString("Main", "ConfPath","./Settings/");
      ConfPath_ += "/";
   }
   return (const std::string)ConfPath_;
}
//---------------------------------------------------------------------------
K_mf(const std::string) DoGetLogPath()
{
   if(LogPath_.empty()){
      TKIniFile KIniFile("./Settings/Main.ini");
      LogPath_  = KIniFile.ReadString("Main", "LogPath","./Logs/%4y%2m%2d/");
      LogPath_ += "/";
   }
   return (const std::string)LogPath_;
}
//---------------------------------------------------------------------------
K_fn(std::string) GetProcConfPath()
{
   return GetConfPath()+ProcName_+"/";
}
//---------------------------------------------------------------------------
K_mf(const std::string) DoGetProcLogPath()
{
   return (const std::string)(DoGetLogPath()+ProcName_+"/");
}
//---------------------------------------------------------------------------
K_fn(std::string) GetHostIDStr()
{
   return "["+HostID.as_string()+"]";
}
//---------------------------------------------------------------------------
K_fn(Kway::Tw::Stk::THostID) GetHostID()
{
   return HostID;
}
//---------------------------------------------------------------------------
K_fn(void)    SetHostID(Kway::Tw::Stk::THostID hid)
{
   HostID = hid;
}
//---------------------------------------------------------------------------
K_fn(TSegDefs*) GetSegDefs()
{
   return &SegDefs_;
}
//---------------------------------------------------------------------------
K_fn(Kway::byte) GetDebugLevel()
{
   if(DebugLevel_ == 255){
      TKIniFile KIniFile("./Settings/Main.ini");
      DebugLevel_  = KIniFile.ReadInt("Main", "DebugLog",0);
   }
   return DebugLevel_;
}
//---------------------------------------------------------------------------
K_fn(Kway::byte) GetSubSystemTK(Kway::Tw::Stk::TMarket mkt, ETradeKind tk)
{
   switch(mkt){
      case Kway::Tw::Stk::m_Tse:
         switch(tk){
            case tk_Normal: return 30;
            case tk_Odd:    return 40;
            case tk_Fix:    return 32;
            case tk_Auct:   return 70;
            case tk_Lend:   return 31;
            case tk_Tend:   return 41;
         }
      case Kway::Tw::Stk::m_Otc:
         switch(tk){
            case tk_Normal: return 93;
            case tk_Odd:    return 94;
            case tk_Fix:    return 98;
//            case tk_Auct:   return 93;  // OTC ¼ÈµL©ç½æ
            case tk_Lend:   return 90;
            case tk_Tend:   return 97;
         }
   }
   return 0;
}
//---------------------------------------------------------------------------
K_fn(Kway::byte) GetSubSystemAP(Kway::Tw::Stk::TMarket mkt, char apcode)
{
   switch(mkt){
      case Kway::Tw::Stk::m_Tse:
         switch(apcode){
            case '0': return 30;
            case '2': return 40;
            case '7': return 32;
            case '5': return 70;
            case '4': return 31;
            case '6': return 41;
         }
      case Kway::Tw::Stk::m_Otc:
         switch(apcode){
            case '0':  return 93;
            case '2':  return 94;
            case '7':  return 98;
//          case '5':    return 93;  // OTC ¼ÈµL©ç½æ
            case '4':  return 90;
            case '8':  return 97;
         }
   }
   return 0;
}
//---------------------------------------------------------------------------
K_fn(ETradeKind)             GetMktTKBySub (int subsys, Kway::Tw::Stk::TMarket& mkt)
{
   switch(subsys){
      case 30: mkt = Kway::Tw::Stk::m_Tse; return tk_Normal;
      case 31: mkt = Kway::Tw::Stk::m_Tse; return tk_Lend;
      case 32: mkt = Kway::Tw::Stk::m_Tse; return tk_Fix;
      case 40: mkt = Kway::Tw::Stk::m_Tse; return tk_Odd;
      case 41: mkt = Kway::Tw::Stk::m_Tse; return tk_Tend;
      case 70: mkt = Kway::Tw::Stk::m_Tse; return tk_Auct;
      case 90: mkt = Kway::Tw::Stk::m_Otc; return tk_Lend;
      case 93: mkt = Kway::Tw::Stk::m_Otc; return tk_Normal;
      case 94: mkt = Kway::Tw::Stk::m_Otc; return tk_Odd;
      case 97: mkt = Kway::Tw::Stk::m_Otc; return tk_Tend;
      case 98: mkt = Kway::Tw::Stk::m_Otc; return tk_Fix;
      default: return tk_None;
   }         
   return tk_None;
}
//---------------------------------------------------------------------------
K_fn(Kway::Tw::Stk::TMarket) GetProcMkt()
{
   return Market_;
}
//---------------------------------------------------------------------------
K_fn(void) SetProcMkt(Kway::Tw::Stk::TMarket m)
{
   Market_ = m;
}
//---------------------------------------------------------------------------
K_fn(Kway::Tw::Stk::TBrokerID) GetProcBrkID()
{
   return BrokerID_;
}
//---------------------------------------------------------------------------
K_fn(void) SetProcBrkID(Kway::Tw::Stk::TBrokerID brk)
{
   BrokerID_ = brk;
}
//---------------------------------------------------------------------------
K_fn(void) InitProc()
{
   GetLogPath     = DoGetLogPath;
   GetProcLogPath = DoGetProcLogPath;
   GetConfPath    = DoGetConfPath;
}
//---------------------------------------------------------------------------
K_fn(bool) IsTMPReady()
{
   return TMPReady_;
}
//---------------------------------------------------------------------------
K_fn(void) SetTMPReady(bool r)
{
   TMPReady_ = r;
}
//---------------------------------------------------------------------------
K_fn(bool) IsAllTMPReady()
{
   return AllTMPReady_;
}
//---------------------------------------------------------------------------
K_fn(void) SetAllTMPReady(bool r)
{
   AllTMPReady_ = r;
}
//---------------------------------------------------------------------------
K_fn(bool) IsDRTMPReady()
{
   return DRTMPReady_;
}
//---------------------------------------------------------------------------
K_fn(void) SetDRTMPReady(bool r)
{
   DRTMPReady_ = r;
}
//---------------------------------------------------------------------------
