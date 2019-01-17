//---------------------------------------------------------------------------
#ifndef DRSesH
#define DRSesH
//---------------------------------------------------------------------------
#include "PacketSes.h"
#include "bxSesMgr.h"
#include "KIniFile.h"
//---------------------------------------------------------------------------
namespace Kway{
namespace Tw{
namespace bx{
using namespace Kway::Tw::Stk;
//---------------------------------------------------------------------------
struct TAtodr{
   char_ary<char, 4,' '>  BrokerId_;
   char_ary<char, 7,' '>  IvacNo_;
   char_ary<char, 6,' '>  StkNo_;
   char_ary<char, 5,' '>  OrderNo_;
   char_ary<char, 1,' '>  FunctionCode_;
   char_ary<char, 1,' '>  LineBrk4_;      // 連線券商第四碼
   char_ary<char, 8,' '>  Qty_;           // 此次要求的數量
   char_ary<char, 6,' '>  Pri_;      
   char_ary<char, 1,' '>  LineBrk3_;      // 連線券商第四碼
   char_ary<char, 1,' '>  OrderType_;      
   char_ary<char, 1,' '>  BSCode_;      
   char_ary<char, 1,' '>  IvacNo_Flag_;      
   char_ary<char, 8,' '>  Date_;      
   char_ary<char, 6,' '>  OrderTime_;      
   char_ary<char, 2,' '>  MS_;      
   char_ary<char, 8,' '>  BfQty_;      
   char_ary<char, 2,' '>  PvcID_;      
   char_ary<char, 4,' '>  Printer_;      
   char_ary<char, 7,' '>  SeqNo_;      
};
//---------------------------------------------------------------------------
struct TAtodrLN:public TAtodr{
   Kstl::char_ary<char,1,'\n'>  NewLine_;
};
//---------------------------------------------------------------------------
template<class TPkt>
K_class TDRSesBase : public  TPacketSes<TPkt,0,sizeof(TPkt)>
{
   typedef TPacketSes<TPkt,0,sizeof(TPkt)> inherited;
   #if defined(__BORLANDC__)  // cooper: 在 BCB 上會有 [Ambiguity between 'TMarket' and 'Stk::TMarket'] 錯誤
   typedef Stk::TMarket                    TMarket;
   #endif
   typedef TDRSesBase this_type;
   typedef Kway::TTimer<this_type> TDRTimer;
private:
   TbxSesMgr*      SesMgr_;
   TOrdCenter*     OrdCenter_;
   TMarket         Market_;
   TDRTimer*       DRTimer_;
   K_mf(void)      OnPktRecv        (TPkt& rec){
      TOrdCenter::TRpt::TOrd rpt;
      rpt.TseOrdKey_.BrokerID_.assign(rec.BrokerId_.as_string());
      rpt.TseOrdKey_.Market_ = Market_;
      rpt.StkNo_.assign(rec.StkNo_.as_string());
      Kway::word cid = rec.FunctionCode_.as_int();
      rpt.BfQty_ = (rec.BfQty_.as_int());
      TStkQty qty;
      if(cid == 1 || cid == 2)
         qty = (rec.Qty_.as_int());
      else
         qty = ((rec.BfQty_.as_int() > rec.Qty_.as_int()) ? (rec.BfQty_.as_int()-rec.Qty_.as_int()) : 0);
      rpt.AfQty_ = qty;
      rpt.Pri_.assign((double)(rec.Pri_.as_int())/100);
      rpt.TseTime_.assign(rec.OrderTime_.as_string());
      rpt.BSCode_.assign(rec.BSCode_.as_string());
      rpt.TseOrdKey_.TseOrdID_.assign(rec.OrderNo_.as_string());
      rpt.IvacNo_.assign(rec.IvacNo_.as_string());
      static EOType  otypeidx[] = { ot_Gn, ot_CrA, ot_DbA, ot_CrS, ot_DbS,ot_Tse5, ot_Tse6 };
      rpt.OType_.assignT(otypeidx[rec.OrderType_.as_int()]);
      static EOrdCmdID  ocmdidx[] = { cid_New, cid_New, cid_ChgQty, cid_Del};
      rpt.OCmd_.assignT(ocmdidx[cid-1]);
      TOrdCenter::TOrdHnd OrdHnd;
      if(OrdCenter_)
         OrdHnd = OrdCenter_->RptOrd(rpt);
      if(!OrdHnd.is_invalid()){
         TStkOrder& ord = OrdHnd.GetModify();
         ord.TseOrderLine_.assign(rec.IvacNo_Flag_.as_string());
         //20111013 eric 修正刪改單無法處理的問題
         ord.TMPBfQty_ = (rec.BfQty_.as_int());
         ord.TMPAfQty_ = qty;
/*
         switch(rec.SubSysName_.as_int()){
            case 30:
            case 93: ord.TrxSegment_ = ts_Full; break;
            case 40:
            case 94: ord.TrxSegment_ = ts_Odd; break;
            case 32:
            case 98: ord.TrxSegment_ = ts_Fix; break;
            default: ord.TrxSegment_ = ts_Full; break;
         }
*/
      }
      ResetTimer();
   }
   K_mf(void)      OnMesLinkBroken  (TMesBase*, const std::string&){};
   K_mf(void)      OnMesLinkFail    (TMesBase*, const std::string&){};
   K_mf(void)      OnTimer          (TTimerBase*)
   {
      TKIniFile DRIniFile(std::string(GetConfPath()+"/DR.ini").c_str());
      std::string drdate = DRIniFile.ReadString("FT","DRReadyDate","");
      if(drdate == TKDate::Now().as_string(TKDate::sf_ELong,0)){
      	DRTimer_->Stop();
      	SetDRTMPReady(true);
      }
   }
   K_mf(void)      ResetTimer       () 
   {
   	DRTimer_->Stop();
   	DRTimer_->Start(30*1000,&this_type::OnTimer);   
   }
public:
   K_ctor          TDRSesBase(TSesMgr* sm, TMarket mkt)
   {
      SesMgr_ = (TbxSesMgr*)sm;
      Market_ = mkt;
      OrdCenter_ = ((TbxSesMgr*)sm)->GetOrdCenter();
      DRTimer_ = new TDRTimer(this);
   }
   K_dtor          ~TDRSesBase(){delete DRTimer_;};
   K_mf(bool)      SettingApply  (const std::string& s){ResetTimer(); return true;}
};
//---------------------------------------------------------------------------
typedef TDRSesBase<TAtodr>   TDRSes;
typedef TDRSesBase<TAtodrLN> TDRSesLN;
//---------------------------------------------------------------------------
}
}
}
//---------------------------------------------------------------------------
#endif

