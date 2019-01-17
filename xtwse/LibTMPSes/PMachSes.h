//---------------------------------------------------------------------------
#ifndef PMachSesH
#define PMachSesH
//---------------------------------------------------------------------------
#include "PacketSes.h"
#include "bxSesMgr.h"
//---------------------------------------------------------------------------
namespace Kway{
namespace Tw{
namespace bx{
using namespace Kway::Tw::Stk;
//---------------------------------------------------------------------------
struct SPMach : public TRptBody{
};
//---------------------------------------------------------------------------
struct SPMachLN:public SPMach{
   Kstl::char_ary<char,1,'\n'>  NewLine_;
};
//---------------------------------------------------------------------------
template<class TPkt>
K_class TPMachSesBase : public  TPktArraySes<TPkt,1000>
{
   typedef TPktArraySes<TPkt,1000> inherited;
   #if defined(__BORLANDC__)  // cooper: 在 BCB 上會有 [Ambiguity between 'TMarket' and 'Stk::TMarket'] 錯誤
   typedef Stk::TMarket                    TMarket;
   #endif
protected:
   using inherited::Pkt_;
private:
   TbxSesMgr*     SesMgr_;
   TOrdCenter*     OrdCenter_;
   Kway::dword     LastSeq_;
   TMarket         Market_;
   K_mf(void)      OnPktRecv        (Kway::word count){
      for(int i = 0; i < count; i++){
         if(Pkt_[i].StkNo_[4] == '/')
            continue;
         TOrdCenter::TRpt::TDeal rpt;
         rpt.TseOrdKey_.BrokerID_.assign(Pkt_[i].BrokerID_.as_string());
         rpt.TseOrdKey_.Market_ = (GetProcMkt()== m_Unknow?TMarket(m_Tse):GetProcMkt());
         rpt.TseOrdKey_.TseOrdID_.assign(Pkt_[i].OrderNo_.as_string());
// 20110516 過濾非由本TMP Handler 進行新單委託的成交回報資料
         Kway::Tw::Stk::TOrdCenter::TOrdHnd ordhnd = OrdCenter_->GetOrdHnd(rpt.TseOrdKey_, TStkQty(0), uf_OrdFlow, TSysOrdID(0));
         if(ordhnd.is_invalid())
            continue;
         else{
         	const TStkOrder* ord = ordhnd.GetConst();
         	if(ord->Qty_.is_0())
         	   continue;
         }
         rpt.StkNo_.assign(Pkt_[i].StkNo_.as_string());
         rpt.Qty_ = (Pkt_[i].MthQty_.as_int());
         rpt.Pri_.assign((double)(Pkt_[i].MthPri_.as_int())/100);
         rpt.TseTime_.assign(Pkt_[i].MthTime_.as_string());
         rpt.BSCode_.assign(Pkt_[i].BSCode_.as_string());
         rpt.IvacNo_.assign(Pkt_[i].IvacNo_.as_string());
         rpt.BSeqNo_.assign(Pkt_[i].SeqNo_.as_int());
         rpt.MSeqNo_.assign(Pkt_[i].RecNo_.as_int());
         static EOType  otypeidx[] = { ot_Gn, ot_CrA, ot_DbA, ot_CrS, ot_DbS,ot_Tse5, ot_Tse6 };
         rpt.OType_.assignT(otypeidx[Pkt_[i].OrderType_.as_int()]);
         OrdCenter_->RptDeal(rpt);
      }
   }
   K_mf(void)      OnMesLinkBroken  (TMesBase*, const std::string&){};
   K_mf(void)      OnMesLinkFail    (TMesBase*, const std::string&){};
   K_mf(void)      OnMesLinkReady   (TMesBase*){
   }
public:
   K_ctor          TPMachSesBase(TSesMgr* sm){SesMgr_ = (TbxSesMgr*)sm;
                                 OrdCenter_ = ((TbxSesMgr*)sm)->GetOrdCenter();
                                 Market_ = m_Unknow;
                                 LastSeq_ = 0;}
   K_dtor          ~TPMachSesBase(){};
   K_mf(bool)      SettingApply  (const std::string& s){
   	                           if(!s.empty()) 
   	                              Market_.assign(s); 
   	                           else 
   	                              Market_ = m_Unknow;
   	                           return true;}
};
//---------------------------------------------------------------------------
typedef TPMachSesBase<SPMach>   TPMachSes;
typedef TPMachSesBase<SPMachLN> TPMachSesLN;
//---------------------------------------------------------------------------
}
}
}
//---------------------------------------------------------------------------
#endif

