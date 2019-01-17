#ifdef __BORLANDC__
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "bxRecorder.h"
#include "fixed_num.h"
#include "TwStkTseOrdKey.h"
#include "ExtDef.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw;
using namespace Kway::Tw::bx;
/////////////////////////////////////////////////////////////////////////////
K_ctor  TbxRecorder::TbxRecorder(Kway::word fmt)
{
   BSOKFmt_ = fmt;
   BFSDLog_.Open(std::string(GetProcLogPath()+"SndRecv.log").c_str(), (TFileMode)(fmRandomAccess|fmOpenAlways|fmCreatePath));
   BFSDLog_.Seek(BFSDLog_.GetFileSize(), fsBegin);
}
//---------------------------------------------------------------------------
K_dtor TbxRecorder::~TbxRecorder()
{
//   BSOKLog_.Close();
   BFSDLog_.Close();
   AFSDLog_.Close();
}
//---------------------------------------------------------------------------
K_mf(bool)   TbxRecorder::WriteBFSD(TSesID& id, const void* t010, size_t sz)
{
   char buf[256];
   char tmp[256];
   memset(buf,0,256);
   memcpy(buf, t010, sz);
   sprintf(tmp,"%s %s Send ==> %s",TKDateTimeMS::Now().as_string(TKDate::sf_ELong).c_str(),id.as_string().c_str(),buf);
   BFSDLog_.WriteLN(tmp);
   return true;
}
//---------------------------------------------------------------------------^M
K_mf(bool)   TbxRecorder::WriteAFSD(TSesID& id, const void* t020, size_t sz)
{
   char buf[256];
   char tmp[256];
   memset(buf,0,256);
   memcpy(buf, t020, sz);
   sprintf(tmp,"%s %s Recv <== %s",TKDateTimeMS::Now().as_string(TKDate::sf_ELong).c_str(),id.as_string().c_str(),buf);
   BFSDLog_.WriteLN(tmp);
   return true;
}
//---------------------------------------------------------------------------
K_mf(bool)   TbxRecorder::WriteLog(TSesID& id, const char* msg)
{
   char tmp[256];
   sprintf(tmp,"%s %s %s",TKDateTimeMS::Now().as_string(TKDate::sf_ELong).c_str(),id.as_string().c_str(),msg);
   BFSDLog_.WriteLN(tmp);
   return true;
}
//---------------------------------------------------------------------------^M
K_mf(bool)   TbxRecorder::WriteBSOK(TT020* t020,TStkOrder& StkOrder)
{
   //FILE *fp;
   std::string TempStr;
   char buf[32]; 
   if(BSOKFmt_ == 0){
      TBSOK BSOK;
      BSOK.PVC_.assign("00");
      BSOK.LineNo_.assign("00");
      BSOK.SubSys_ = t020->SubSysName_;
      BSOK.FnCode_ = t020->FunctionCode_;
      BSOK.MgType_ = t020->MessageType_;
      BSOK.MgTime_ = t020->MessageTime_;
      BSOK.StCode_ = t020->StatusCode_;
      BSOK.BrokerID_ = t020->BrokerId_;
      BSOK.LPVCID_ = t020->PvcId_;
      BSOK.OrderNo_ = t020->OrderNo_;
      BSOK.CustNo_ = t020->IvacNo_;
      BSOK.CustFlag_ = t020->IvacNo_Flag_;
      BSOK.StkNo_ = t020->StkNo_;
      BSOK.Price_ = t020->Pri_;
      sprintf(buf,"%08d", t020->Qty_.as_int());
      BSOK.Qty_.assign(std::string(buf));
      BSOK.BSCode_ = t020->BSCode_;
      BSOK.ExchgCode_ = t020->ExchageCode_;
      BSOK.OType_ = t020->OrderType_;
      BSOK.OBrokerID_.assign("    ");
      sprintf(buf,"%02d%04d",TKDate::Now().GetCYear(),t020->OrderDate_.as_int()%10000);
      BSOK.TDate_.assign(buf);
      BSOK.TTime_ = t020->OrderTime_;
      sprintf(buf,"%08d", t020->BfQty_.as_int());
      BSOK.BeforeQty_.assign(std::string(buf));
      sprintf(buf,"%08d", t020->AfQty_.as_int());
      BSOK.AfterQty_.assign(std::string(buf));
      BSOK.TFlag_.assign("Y");
      BSOK.TseSaleNo_.assign(StkOrder.SaleNo_.as_string());
      BSOK.Filler1_.assign("  ");
      BSOK.ActOType_.assign(StkOrder.OType_.as_string());
      BSOK.Market_.assign(StkOrder.Market_.as_string());
//      BSOKLog_.Write(&BSOK, sizeof(BSOK));
      return true;
   }else if(BSOKFmt_ == 1){
      TBSOK96 BSOK;
      BSOK.LineNo_.assign("00");
      BSOK.SubSys_ = t020->SubSysName_;
      BSOK.FnCode_ = t020->FunctionCode_;
      BSOK.MgType_ = t020->MessageType_;
      BSOK.MgTime_.assign(t020->MessageTime_.as_int());
      BSOK.StCode_ = t020->StatusCode_;
      BSOK.BrokerID_ = t020->BrokerId_;
      BSOK.LPVCID_ = t020->PvcId_;
      Kway::Tw::Stk::TTseOrdID OrdID;
      OrdID.assign(t020->OrderNo_.as_string());
      BSOK.TermID_.set(0,t020->OrderNo_[0]);
//      BSOK.OrderNo_.assign(OrdID.GetSeqNo());
      BSOK.CustNo_.assign(t020->IvacNo_.as_int()/10);
      BSOK.CustChk_.set(0,t020->IvacNo_[6]);
      BSOK.CustFlag_ = t020->IvacNo_Flag_;
      BSOK.StkNo_ = t020->StkNo_;
      BSOK.Price_.assign(t020->Pri_.as_int());
      BSOK.Qty_.assign(t020->Qty_.as_int());
      BSOK.BSCode_ = t020->BSCode_;
      BSOK.ExchgCode_ = t020->ExchageCode_;
      BSOK.OType_ = t020->OrderType_;
      BSOK.TDate_.assign(t020->OrderDate_.as_int()%10000+TKDate::Now().GetCYear()*10000);
      BSOK.TTime_.assign(t020->OrderTime_.as_int());
      BSOK.BeforeQty_.assign(t020->BfQty_.as_int());
      BSOK.AfterQty_.assign(t020->AfQty_.as_int());
      BSOK.TFlag_.assign("Y");
      BSOK.SalesNo_.assign(StkOrder.SaleNo_.as_string());
      BSOK.YPrice_.clear();
      BSOK.YQty_.clear();
      BSOK.Htsq_.clear();
      BSOK.Insq_.clear();
      BSOK.Mgsq_.clear();
      BSOK.KTime_.assign(t020->OrderTime_.as_int());
      BSOK.SCode_.assign("0");
      BSOK.OCode_.assign("9");
      BSOK.TLNo_.assign(StkOrder.OrigSource_.UserID_.as_string());
//      BSOKLog_.Write(&BSOK, sizeof(BSOK));
      return true;
   }
}
//---------------------------------------------------------------------------
K_mf(bool)   TbxRecorder::WriteBSOK(TO020* t020,TStkOrder& StkOrder)
{
   //FILE *fp;
   std::string TempStr;
   //↓填入資料,PVC及LineNO 在TCP下暫時先寫死為"00"
   char buf[32];
   if(BSOKFmt_ == 0){
      TBSOK BSOK;
      BSOK.PVC_.assign("00");
      BSOK.LineNo_.assign("00");
      BSOK.SubSys_ = t020->SubSysName_;
      BSOK.FnCode_ = t020->FunctionCode_;
      BSOK.MgType_ = t020->MessageType_;
      BSOK.MgTime_ = t020->MessageTime_;
      BSOK.StCode_ = t020->StatusCode_;
      BSOK.BrokerID_ = t020->BrokerId_;
      BSOK.LPVCID_ = t020->PvcId_;
      BSOK.OrderNo_ = t020->OrderNo_;
      BSOK.CustNo_ = t020->IvacNo_;
      BSOK.CustFlag_.assign(" ");// = t020->IvacNo_Flag_;
      BSOK.StkNo_ = t020->StkNo_;
      BSOK.Price_ = t020->Pri_;
      sprintf(buf,"%08d", t020->Qty_.as_int());
      BSOK.Qty_.assign(std::string(buf));
      BSOK.BSCode_ = t020->BSCode_;
//      BSOK.ExchgCode_.assign(" ");// = t020->ExchageCode_;
      BSOK.ExchgCode_ = t020->TradeKind_;
      BSOK.OType_.assign("0");// = t020->OrderType_;
      BSOK.OBrokerID_.assign("    ");
      sprintf(buf,"%02d%04d",TKDate::Now().GetCYear(),t020->OrderDate_.as_int()%10000);
      BSOK.TDate_.assign(buf);
      BSOK.TTime_.assign(t020->OrderTime_.as_string());
      sprintf(buf,"%08d", t020->BfQty_.as_int());
      BSOK.BeforeQty_.assign(std::string(buf));
      sprintf(buf,"%08d", t020->AfQty_.as_int());
      BSOK.AfterQty_.assign(std::string(buf));
      BSOK.TFlag_.assign("Y");
      BSOK.TseSaleNo_.assign(StkOrder.SaleNo_.as_string());
      BSOK.Filler1_.assign("  ");
      BSOK.ActOType_.assign(StkOrder.OType_.as_string());
      BSOK.Market_.assign(StkOrder.Market_.as_string());
//      BSOKLog_.Write(&BSOK, sizeof(BSOK));
      return true;
   }else if(BSOKFmt_ == 1){
      TBSOK96 BSOK;
      BSOK.LineNo_.assign("00");
      BSOK.SubSys_ = t020->SubSysName_;
      BSOK.FnCode_ = t020->FunctionCode_;
      BSOK.MgType_ = t020->MessageType_;
      BSOK.MgTime_.assign(t020->MessageTime_.as_int());
      BSOK.StCode_ = t020->StatusCode_;
      BSOK.BrokerID_ = t020->BrokerId_;
      BSOK.LPVCID_ = t020->PvcId_;
      Kway::Tw::Stk::TTseOrdID OrdID;
      OrdID.assign(t020->OrderNo_.as_string());
      BSOK.TermID_.set(0,t020->OrderNo_[0]);
//      BSOK.OrderNo_.assign(OrdID.GetSeqNo());
      BSOK.CustNo_.assign(t020->IvacNo_.as_int()/10);
      BSOK.CustChk_.set(0,t020->IvacNo_[6]);
      BSOK.CustFlag_.assign(" ");// = t020->IvacNo_Flag_;
      BSOK.StkNo_ = t020->StkNo_;
      BSOK.Price_.assign(t020->Pri_.as_int());
      BSOK.Qty_.assign(t020->Qty_.as_int());
      BSOK.BSCode_ = t020->BSCode_;
//      BSOK.ExchgCode_.assign(" ");// = t020->ExchageCode_;
      BSOK.ExchgCode_ = t020->TradeKind_;
      BSOK.OType_.assign("0");// = t020->OrderType_;
      BSOK.TDate_.assign(t020->OrderDate_.as_int()%10000+TKDate::Now().GetCYear()*10000);
      BSOK.TTime_.assign(t020->OrderTime_.as_int());
      BSOK.BeforeQty_.assign(t020->BfQty_.as_int());
      BSOK.AfterQty_.assign(t020->AfQty_.as_int());
      BSOK.TFlag_.assign("Y");
      BSOK.SalesNo_.assign(StkOrder.SaleNo_.as_string());
      BSOK.YPrice_.clear();
      BSOK.YQty_.clear();
      BSOK.Htsq_.clear();
      BSOK.Insq_.clear();
      BSOK.Mgsq_.clear();
      BSOK.KTime_.assign(t020->OrderTime_.as_int());
      BSOK.SCode_.assign("0");
      BSOK.OCode_.assign("9");
      BSOK.TLNo_.assign(StkOrder.OrigSource_.UserID_.as_string());
//      BSOKLog_.Write(&BSOK, sizeof(BSOK));
      return true;
   }
}
//---------------------------------------------------------------------------
K_mf(bool)   TbxRecorder::WriteBSOK(TA020* a020,TStkOrder& StkOrder)
{
   //FILE *fp;
   TBSOK BSOK;
   std::string TempStr;
   //↓填入資料,PVC及LineNO 在TCP下暫時先寫死為"00"
   char buf[32];
   BSOK.PVC_.assign("00");
   BSOK.LineNo_.assign("00");
   BSOK.SubSys_ = a020->SubSysName_;
   BSOK.FnCode_ = a020->FunctionCode_;
   BSOK.MgType_ = a020->MessageType_;
   BSOK.MgTime_ = a020->MessageTime_;
   BSOK.StCode_ = a020->StatusCode_;
   BSOK.BrokerID_ = a020->BrokerId_;
   BSOK.LPVCID_ = a020->PvcId_;
   BSOK.OrderNo_ = a020->OrderNo_;
   BSOK.CustNo_ = a020->IvacNo_;
   BSOK.CustFlag_.assign(" ");// = a020->IvacNo_Flag_;
   BSOK.StkNo_ = a020->StkNo_;
   BSOK.Price_ = a020->AfPri_;
   sprintf(buf,"%08d", a020->Qty_.as_int());
   BSOK.Qty_.assign(std::string(buf));
//   BSOK.BSCode_ = a020->BSCode_;
   BSOK.ExchgCode_.assign(" ");// = a020->ExchageCode_;
   BSOK.OType_.assign("A");// = a020->OrderType_;
   BSOK.OBrokerID_.assign("    ");
   sprintf(buf,"%02d%04d",TKDate::Now().GetCYear(),a020->OrderDate_.as_int()%10000);
   BSOK.TDate_.assign(buf);
   BSOK.TTime_.assign(a020->OrderTime_.as_string());
   sprintf(buf,"%08d", a020->BfQty_.as_int());
   BSOK.BeforeQty_.assign(std::string(buf));
   sprintf(buf,"%08d", a020->AfQty_.as_int());
   BSOK.AfterQty_.assign(std::string(buf));
   BSOK.TFlag_.assign("Y");
   BSOK.TseSaleNo_.assign(StkOrder.SaleNo_.as_string());
   BSOK.Filler1_.assign("  ");
   BSOK.ActOType_.assign(StkOrder.OType_.as_string());
   BSOK.Market_.assign(StkOrder.Market_.as_string());
//   BSOKLog_.Write(&BSOK, sizeof(BSOK));
   return true;
}
//---------------------------------------------------------------------------
K_mf(bool)   TbxRecorder::WriteBSOK(TV020* v020,TStkOrder& StkOrder)
{
   //FILE *fp;
   TBSOK BSOK;
   std::string TempStr;
   //↓填入資料,PVC及LineNO 在TCP下暫時先寫死為"00"
   char buf[32];
   BSOK.PVC_.assign("00");
   BSOK.LineNo_.assign("00");
   BSOK.SubSys_ = v020->SubSysName_;
   BSOK.FnCode_ = v020->FunctionCode_;
   BSOK.MgType_ = v020->MessageType_;
   BSOK.MgTime_ = v020->MessageTime_;
   BSOK.StCode_ = v020->StatusCode_;
   BSOK.BrokerID_ = v020->BrokerId_;
   BSOK.LPVCID_ = v020->PvcId_;
   BSOK.OrderNo_ = v020->OrderNo_;
   BSOK.CustNo_ = v020->IvacNo_;
   BSOK.CustFlag_.assign(" ");// = v020->IvacNo_Flag_;
   BSOK.StkNo_ = v020->StkNo_;
   BSOK.Price_.assign(v020->Pri_.begin(),6); // 標借交易只取後6位數, 可能會喪失百位數
   sprintf(buf,"%08d", v020->Qty_.as_int());
   BSOK.Qty_.assign(std::string(buf));
//   BSOK.BSCode_ = v020->BSCode_;
   BSOK.ExchgCode_.assign(" ");// = v020->ExchageCode_;
   BSOK.OType_.assign("L");// = v020->OrderType_;
   BSOK.OBrokerID_.assign("    ");
   sprintf(buf,"%02d%04d",TKDate::Now().GetCYear(),v020->OrderDate_.as_int()%10000);
   BSOK.TDate_.assign(buf);
   BSOK.TTime_.assign(v020->OrderTime_.as_string());
   sprintf(buf,"%08d", v020->BfQty_.as_int());
   BSOK.BeforeQty_.assign(std::string(buf));
   sprintf(buf,"%08d", v020->AfQty_.as_int());
   BSOK.AfterQty_.assign(std::string(buf));
   BSOK.TFlag_.assign("Y");
   BSOK.TseSaleNo_.assign(StkOrder.SaleNo_.as_string());
   BSOK.Filler1_.assign("  ");
   BSOK.ActOType_.assign(StkOrder.OType_.as_string());
   BSOK.Market_.assign(StkOrder.Market_.as_string());
//   BSOKLog_.Write(&BSOK, sizeof(BSOK));
   return true;
}
//---------------------------------------------------------------------------
K_mf(bool)   TbxRecorder::WriteBSOK(TE020* e020,TStkOrder& StkOrder)
{
   //FILE *fp;
   TBSOK BSOK;
   std::string TempStr;
   //↓填入資料,PVC及LineNO 在TCP下暫時先寫死為"00"
   char buf[32];
   BSOK.PVC_.assign("00");
   BSOK.LineNo_.assign("00");
   BSOK.SubSys_ = e020->SubSysName_;
   BSOK.FnCode_ = e020->FunctionCode_;
   BSOK.MgType_ = e020->MessageType_;
   BSOK.MgTime_ = e020->MessageTime_;
   BSOK.StCode_ = e020->StatusCode_;
   BSOK.BrokerID_ = e020->BrokerId_;
   BSOK.LPVCID_ = e020->PvcId_;
   BSOK.OrderNo_ = e020->OrderNo_;
   BSOK.CustNo_ = e020->IvacNo_;
   BSOK.CustFlag_.assign(" ");// = e020->IvacNo_Flag_;
   BSOK.StkNo_ = e020->StkNo_;
   BSOK.Price_ = e020->AfPri_;
   sprintf(buf,"%08d", e020->Qty_.as_int());
   BSOK.Qty_.assign(std::string(buf));
//   BSOK.BSCode_ = e020->BSCode_;
   BSOK.ExchgCode_.assign(" ");// = e020->ExchageCode_;
   BSOK.OType_.assign("T");// = e020->OrderType_;
   BSOK.OBrokerID_.assign("    ");
   sprintf(buf,"%02d%04d",TKDate::Now().GetCYear(),e020->OrderDate_.as_int()%10000);
   BSOK.TDate_.assign(buf);
   BSOK.TTime_.assign(e020->OrderTime_.as_string());
   sprintf(buf,"%08d", e020->BfQty_.as_int());
   BSOK.BeforeQty_.assign(std::string(buf));
   sprintf(buf,"%08d", e020->AfQty_.as_int());
   BSOK.AfterQty_.assign(std::string(buf));
   BSOK.TFlag_.assign("Y");
   BSOK.TseSaleNo_.assign(StkOrder.SaleNo_.as_string());
   BSOK.Filler1_.assign("  ");
   BSOK.ActOType_.assign(StkOrder.OType_.as_string());
   BSOK.Market_.assign(StkOrder.Market_.as_string());
//   BSOKLog_.Write(&BSOK, sizeof(BSOK));
   return true;
}
//---------------------------------------------------------------------------
K_mf(bool)   TbxRecorder::WriteBSOK(TTseControlHead* t030,TStkOrder& StkOrder,TBrkID BrkID,TPvcID PvcID, bool broken)
{
   std::string TempStr;
   if(BSOKFmt_ == 0){
      TBSOK BSOK;
      //↓填入資料,PVC及LineNO 在TCP下暫時先寫死為"00"
      char buf[32];
      BSOK.PVC_.assign("00");
      BSOK.LineNo_.assign("00");
      BSOK.SubSys_ = t030->SubSysName_;
      BSOK.FnCode_ = t030->FunctionCode_;
      BSOK.MgType_ = t030->MessageType_;
      BSOK.MgTime_ = t030->MessageTime_;
      BSOK.StCode_ = t030->StatusCode_;
      BSOK.BrokerID_.assign(BrkID.as_string());
      BSOK.LPVCID_.assign(PvcID.as_string());
      BSOK.OrderNo_.assign(StkOrder.TseOrdID_.as_string(true));
      BSOK.CustNo_.assign(StkOrder.IvacKey_.GetIvacNo().as_string(0,true));
      BSOK.CustFlag_.assign("");
      BSOK.StkNo_.assign(StkOrder.StkNo_.as_string());
      BSOK.Price_.assign(StkOrder.Pri_.as_format(4,2));
      int qty;
      if(t030->SubSysName_.as_int() == 40 || t030->SubSysName_.as_int() == 94)
         qty = StkOrder.Qty_;
      else
         qty = StkOrder.Qty_ / 1000;
      sprintf(buf,"%08d",qty);
      BSOK.Qty_.assign(buf);
      BSOK.BSCode_.assign(StkOrder.BSCode_.as_string());
      BSOK.ExchgCode_.assign(" ");
      BSOK.OType_.assign(StkOrder.OType_.as_string());
      BSOK.OBrokerID_.assign("    ");
      BSOK.TDate_.assign(TKDate::Now().as_string(TKDate::sf_CLong,0));
      BSOK.TTime_.assign(t030->MessageTime_.as_string());
      if(t030->SubSysName_.as_int() == 40 || t030->SubSysName_.as_int() == 94)
         qty = StkOrder.BfQty_;
      else
         qty = StkOrder.BfQty_ / 1000;
      sprintf(buf,"%08d",qty);
      BSOK.BeforeQty_.assign(buf);
      if(t030->SubSysName_.as_int() == 40 || t030->SubSysName_.as_int() == 94)
         qty = StkOrder.AfQty_;
      else
         qty = StkOrder.AfQty_ / 1000;
      sprintf(buf,"%08d",qty);
      BSOK.AfterQty_.assign(buf);
      BSOK.TFlag_.assign(broken?"T":"Y");
      BSOK.TseSaleNo_.assign(StkOrder.SaleNo_.as_string());
      BSOK.Filler1_.assign("  ");
      BSOK.ActOType_.assign(StkOrder.OType_.as_string());
      BSOK.Market_.assign(StkOrder.Market_.as_string());
//      BSOKLog_.Write(&BSOK, sizeof(BSOK));
   }else if(BSOKFmt_ == 1){
      TBSOK96 BSOK;
      int subsys = t030->SubSysName_.as_int();
      int qty;
      Kstl::char_ary<char,8,' '> tmp;
      BSOK.LineNo_.assign("00");
      BSOK.SubSys_ = t030->SubSysName_;
      BSOK.FnCode_ = t030->FunctionCode_;
      BSOK.MgType_ = t030->MessageType_;
      BSOK.MgTime_.assign(t030->MessageTime_.as_int());
      BSOK.StCode_ = t030->StatusCode_;
      BSOK.BrokerID_.assign(BrkID.as_string());
      BSOK.LPVCID_.assign(PvcID.as_string());
      BSOK.TermID_.set(0,StkOrder.TseOrdID_.GetTermID()[0]);
//      BSOK.OrderNo_.assign(StkOrder.TseOrdID_.GetSeqNo());
      BSOK.CustNo_.assign(StkOrder.IvacKey_.GetIvacNo().ToInt_NoChk());
      BSOK.CustChk_.set(0,StkOrder.IvacKey_.GetIvacNo().as_string(0,true)[6]);
      BSOK.CustFlag_.assign(" ");
      BSOK.StkNo_.assign(StkOrder.StkNo_.as_string());
      tmp.assign(StkOrder.Pri_.as_format(4,2));
      BSOK.Price_.assign(tmp.as_int());
      BSOK.Qty_.assign((subsys == 40 || subsys == 94)?(dword)StkOrder.Qty_:(dword)StkOrder.Qty_/1000);
      BSOK.BSCode_.assign(StkOrder.BSCode_.as_string());
      BSOK.ExchgCode_.assign(" ");
      BSOK.OType_.assign(StkOrder.OType_.as_string());
      tmp.assign(TKDate::Now().as_string(TKDate::sf_CLong,0));
      BSOK.TDate_.assign(tmp.as_int());
      BSOK.TTime_.assign(t030->MessageTime_.as_int());
      BSOK.BeforeQty_.assign((subsys == 40 || subsys == 94)?(dword)StkOrder.BfQty_:(dword)StkOrder.BfQty_/1000);
      BSOK.AfterQty_.assign((subsys == 40 || subsys == 94)?(dword)StkOrder.AfQty_:(dword)StkOrder.AfQty_/1000);
      BSOK.TFlag_.assign(broken?"T":"Y");
      BSOK.SalesNo_.assign(StkOrder.SaleNo_.as_string());
      BSOK.YPrice_.clear();
      BSOK.YQty_.clear();
      BSOK.Htsq_.clear();
      BSOK.Insq_.clear();
      BSOK.Mgsq_.clear();
      BSOK.KTime_.assign(t030->MessageTime_.as_int());
      BSOK.SCode_.assign("0");
      BSOK.OCode_.assign("9");
      BSOK.TLNo_.assign(StkOrder.OrigSource_.UserID_.as_string());
//      BSOKLog_.Write(&BSOK, sizeof(BSOK));
   }
   return true;

}
//---------------------------------------------------------------------------
