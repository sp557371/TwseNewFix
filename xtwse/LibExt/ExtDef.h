//---------------------------------------------------------------------------
#ifndef ExtDefH
#define ExtDefH
//---------------------------------------------------------------------------
#include <string>
#include <string.h>
#include "TwStkOrder.h"
#include "ExtFuncs.h"
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
using namespace Kway;
/////////////////////////////////////////////////////////////////////////////
enum ETradeKind{
   tk_Normal = 0,   // 一般交易,(整股)
   tk_Odd    = 1,   // 零股
   tk_Fix    = 2,   // 盤後
   tk_Auct   = 3,   // 拍賣
   tk_Lend   = 4,   // 標借
   tk_Tend   = 5,   // 標購
   tk_None   = 6,
};
//---------------------------------------------------------------------------
enum EAttr{            // 作為試算用途
   ea_Normal = 0x00,   // 正常單筆委託
   ea_Big    = 0x01,   // 大單, 超過499張
   ea_Mul    = 0x02,   // 乘號試算
   ea_Div    = 0x04,   // 除號試算
   ea_Trial  = 0x0f,   // 需要進行試算
   ea_DAS    = 0x10,   // DMA 拆單後的委託(Dma Auto Slicing)
   ea_FIX    = 0x20,   // 來自於FIX連線的委託
   ea_ABO    = 0x40,   // 需要進行自動圈存的委託(Auto-Booking required Order)
   ea_Pass   = 0x80,   // 己安控完畢, 不需再次檢查
   ea_All    = 0xff,   // 
};
//---------------------------------------------------------------------------
K_class TMktLine{
public:
   Kway::word Lines_;
   Kway::word QOrds_;
   K_mf(void) Reset(){
         Lines_ = 0;
         QOrds_ = 0;
   }
};
//---------------------------------------------------------------------------
K_class TTLine{
public:
   TMktLine   Normal_;
   TMktLine   Odd_;
   TMktLine   Fix_;
   TMktLine   Auct_;
   TMktLine   Lend_;
   TMktLine   Tend_;
   inline K_mf(void) Clear(){
                Normal_.Reset();
                Auct_.Reset();
                Lend_.Reset();
                Tend_.Reset();
              } 
   inline K_mf(void) SetL(ETradeKind tk, Kway::word value){
      switch(tk){
         case tk_Normal: Normal_.Lines_ = value; break;  
         case tk_Odd   : Odd_.Lines_    = value; break;  
         case tk_Fix   : Fix_.Lines_    = value; break;  
         case tk_Auct  : Auct_.Lines_   = value; break;  
         case tk_Lend  : Lend_.Lines_   = value; break;  
         case tk_Tend  : Tend_.Lines_   = value; break;
         default       : break;
      }
   }  
   inline K_mf(Kway::word) GetL(ETradeKind tk){
      switch(tk){
         case tk_Normal: return Normal_.Lines_;  
         case tk_Odd   : return Odd_.Lines_;  
         case tk_Fix   : return Fix_.Lines_;  
         case tk_Auct  : return Auct_.Lines_;  
         case tk_Lend  : return Lend_.Lines_;  
         case tk_Tend  : return Tend_.Lines_;
         default       : return 0;
      }
   }  
   inline K_mf(void) SetQ(ETradeKind tk, Kway::word value){
      switch(tk){
         case tk_Normal: Normal_.QOrds_ = value; break;
         case tk_Odd   : Odd_.QOrds_    = value; break;
         case tk_Fix   : Fix_.QOrds_    = value; break;
         case tk_Auct  : Auct_.QOrds_   = value; break;
         case tk_Lend  : Lend_.QOrds_   = value; break;
         case tk_Tend  : Tend_.QOrds_   = value; break;
         default       : break;
      }
   }
   inline K_mf(Kway::word) GetQ(ETradeKind tk){
      switch(tk){
         case tk_Normal: return Normal_.QOrds_;  
         case tk_Odd   : return Odd_.QOrds_;  
         case tk_Fix   : return Fix_.QOrds_;  
         case tk_Auct  : return Auct_.QOrds_;  
         case tk_Lend  : return Lend_.QOrds_;  
         case tk_Tend  : return Tend_.QOrds_;
         default       : break;
      }
   }
   inline K_mf(Kway::word) GetRatio(ETradeKind tk){
      switch(tk){
         case tk_Normal: return (Normal_.Lines_?(Normal_.QOrds_/Normal_.Lines_):59999);
         case tk_Odd   : return (Odd_.Lines_?(Odd_.QOrds_/Odd_.Lines_):59999);
         case tk_Fix   : return (Fix_.Lines_?(Fix_.QOrds_/Fix_.Lines_):59999);
         case tk_Auct  : return (Auct_.Lines_?(Auct_.QOrds_/Auct_.Lines_):59999);
         case tk_Lend  : return (Lend_.Lines_?(Lend_.QOrds_/Lend_.Lines_):59999);
         case tk_Tend  : return (Tend_.Lines_?(Tend_.QOrds_/Tend_.Lines_):59999);
         default       : return 59999;
      }
   }
};
//---------------------------------------------------------------------------
K_class TStkLines
{
typedef Kway::Tw::Stk::TMarket TMarket;
public:
   TTLine   Tse_;
   TTLine   Otc_;
   TTLine   OtcR_;

   inline K_mf(void) SetL(TMarket m, ETradeKind tk, Kway::word value){
      switch(m){
         case Kway::Tw::Stk::m_Tse:  Tse_.SetL(tk,value);  break;
         case Kway::Tw::Stk::m_Otc:  Otc_.SetL(tk,value);  break;
         case Kway::Tw::Stk::m_OtcR: OtcR_.SetL(tk,value); break;
      }
   }
   inline K_mf(Kway::word) GetL(TMarket m, ETradeKind tk){
      switch(m){
         case Kway::Tw::Stk::m_Tse:  return Tse_.GetL(tk);
         case Kway::Tw::Stk::m_Otc:  return Otc_.GetL(tk);
         case Kway::Tw::Stk::m_OtcR: return OtcR_.GetL(tk); 
         default: return 0;
      }
   }
   inline K_mf(void) SetAll(Kway::word value){
      Kway::byte tk;
      for(tk = tk_Normal; tk!= tk_None; tk++){
         Tse_.SetL((ETradeKind)tk,value);
         Otc_.SetL((ETradeKind)tk,value); 
         OtcR_.SetL((ETradeKind)tk,value); 
      }
   }
   inline K_mf(Kway::word) GetAll(){
      Kway::byte tk;
      Kway::word lines = 0;
      for(tk = tk_Normal; tk!= tk_None; tk++){
         lines += Tse_.GetL((ETradeKind)tk);
         lines += Otc_.GetL((ETradeKind)tk); 
         lines += OtcR_.GetL((ETradeKind)tk); 
      }
      return lines;
   }
   inline K_mf(void) SetQ(TMarket m, ETradeKind tk, Kway::word value){
      switch(m){
         case Kway::Tw::Stk::m_Tse:  Tse_.SetQ(tk,value);  break;
         case Kway::Tw::Stk::m_Otc:  Otc_.SetQ(tk,value);  break;
         case Kway::Tw::Stk::m_OtcR: OtcR_.SetQ(tk,value); break;
         default    : break;
      }
   }
   inline K_mf(Kway::word) GetQ(TMarket m, ETradeKind tk){
      switch(m){
         case Kway::Tw::Stk::m_Tse:  return Tse_.GetQ(tk);
         case Kway::Tw::Stk::m_Otc:  return Otc_.GetQ(tk);
         case Kway::Tw::Stk::m_OtcR: return OtcR_.GetQ(tk); 
         default: return 0;
      }
   }
   inline K_mf(Kway::word) GetRatio(TMarket m, ETradeKind tk){
      switch(m){
         case Kway::Tw::Stk::m_Tse:  return Tse_.GetRatio(tk);
         case Kway::Tw::Stk::m_Otc:  return Otc_.GetRatio(tk);
         case Kway::Tw::Stk::m_OtcR: return OtcR_.GetRatio(tk);
         default    : return 59999;
      }
   }
   inline K_mf(void) Reset() {
      Kway::byte tk;
      for(tk = tk_Normal; tk!= tk_None; tk++){
         Tse_.SetL((ETradeKind)tk,0);
         Tse_.SetQ((ETradeKind)tk,0);
         Otc_.SetL((ETradeKind)tk,0); 
         Otc_.SetQ((ETradeKind)tk,0); 
         OtcR_.SetL((ETradeKind)tk,0); 
         OtcR_.SetQ((ETradeKind)tk,0); 
      }
   }
};
//---------------------------------------------------------------------------
struct TSeg{
   Kstl::fixed_num<Kway::byte> SubSys_;
   TKTime                      BTime_;
   TKTime                      ETime_;
   bool                        IsOpen_;
   Kway::byte                  Weight_;
   Kway::word                  TargetL_;
   Kway::word                  CurrentL_;
   K_ctor TSeg(){
      SubSys_ = 0;
      BTime_.assign("00:00:00");
      ETime_.assign("00:00:00");
      IsOpen_   = false;
      Weight_   = 1;
      TargetL_  = 0;
      CurrentL_ = 0;
   }
};
//---------------------------------------------------------------------------
struct TSegDef{
   TSeg             Seg_[tk_None]; 
   Kway::word       TotalL_;
   K_ctor           TSegDef(){TotalL_ = 0;}
   K_mf(word)       GetTotWeight(){ // 20090701 因拍賣標借標購交易為自動切換線路, 故不考慮比重
      Kway::word  Weight = 0;
      for(int L = 0; L <= tk_Fix; L++)
         if(Seg_[L].IsOpen_)
            Weight += Seg_[L].Weight_;
      return Weight;
   }
   K_mf(ETradeKind) GetMinRatio(){
      double ratio[tk_None];
      for(int L = 0; L <= tk_Fix; L++){
         if(Seg_[L].IsOpen_){
            ratio[L] = Seg_[L].TargetL_;
            ratio[L] /= Seg_[L].Weight_;
         }else
            ratio[L] = 59999;
      }
      return (ratio[tk_Normal] <= ratio[tk_Odd] ? 
             (ratio[tk_Normal] <= ratio[tk_Fix] ? 
              tk_Normal:(ratio[tk_Odd] <= ratio[tk_Fix] ?
              tk_Odd : tk_Fix)):
              (ratio[tk_Odd] <= ratio[tk_Fix]?tk_Odd : tk_Fix));
   }
   K_mf(ETradeKind) GetMaxRatio(){
      double ratio[tk_None];
      for(int L = 0; L <= tk_Fix; L++){
         if(Seg_[L].IsOpen_){
            ratio[L] = Seg_[L].TargetL_;
            ratio[L] /= Seg_[L].Weight_;
         }else
            ratio[L] = 0;
      }
      return (ratio[tk_Normal] >= ratio[tk_Odd] ? 
             (ratio[tk_Normal] >= ratio[tk_Fix] ? 
              tk_Normal:(ratio[tk_Odd] >= ratio[tk_Fix] ?
              tk_Odd : tk_Fix)):
              (ratio[tk_Odd] >= ratio[tk_Fix] ?tk_Odd : tk_Fix));
   }
   K_mf(bool) IsOpen(){
      for(int L = 0; L <= tk_Fix; L++)
         if(Seg_[L].IsOpen_)
            return true;
      return false;
   }
};
//---------------------------------------------------------------------------
struct TSegDefs{
   TSegDef Tse_;
   TSegDef Otc_;
   K_mf(void) Reset(){
      Tse_.TotalL_ = 0;
      Otc_.TotalL_ = 0;
      for(int L = 0; L < tk_None; L++){
         Tse_.Seg_[L].TargetL_  = 0;
         Tse_.Seg_[L].CurrentL_ = 0;
         Otc_.Seg_[L].TargetL_  = 0;
         Otc_.Seg_[L].CurrentL_ = 0;
      }
   }
};
//---------------------------------------------------------------------------
K_fn(std::string)            GetProgName();
K_fn(void)                   SetProgName(std::string);
K_fn(std::string)            GetProcName();
K_fn(void)                   SetProcName(std::string);
//K_fn(std::string)            GetConfPath();
//K_fn(std::string)            GetLogPath();
K_fn(std::string)            GetProcConfPath();
//K_fn(std::string)            GetProcLogPath();
K_fn(Kway::Tw::Stk::THostID) GetHostID();
K_fn(std::string)            GetHostIDStr();
K_fn(void)                   SetHostID(Kway::Tw::Stk::THostID HostID);
K_fn(TSegDefs*)              GetSegDefs();
K_fn(Kway::byte)             GetSubSystemTK(Kway::Tw::Stk::TMarket, ETradeKind);
K_fn(Kway::byte)             GetSubSystemAP(Kway::Tw::Stk::TMarket, char);
K_fn(ETradeKind)             GetMktTKBySub (int, Kway::Tw::Stk::TMarket& mkt);
K_fn(Kway::Tw::Stk::TMarket) GetProcMkt();
K_fn(void)                   SetProcMkt(Kway::Tw::Stk::TMarket);
K_fn(Kway::Tw::Stk::TBrokerID) GetProcBrkID();
K_fn(void)                     SetProcBrkID(Kway::Tw::Stk::TBrokerID);
K_fn(Kway::byte)             GetDebugLevel();
K_fn(void)                   InitProc();
K_fn(void)                   SetTMPReady(bool);
K_fn(bool)                   IsTMPReady();
K_fn(void)                   SetAllTMPReady(bool);
K_fn(bool)                   IsAllTMPReady();
K_fn(void)                   SetDRTMPReady(bool);
K_fn(bool)                   IsDRTMPReady();
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
   #pragma pack(pop)
#else
   #pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
#endif
