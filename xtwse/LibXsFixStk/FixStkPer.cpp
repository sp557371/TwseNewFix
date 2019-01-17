/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FixStkPer.h"
#include "Nsa_Per.h"
#include "Nsa_LogFile.h"
#include "QFixExt/QFixPer.h"
#include "FixStkOrder.h"
#include "FixStkOrdTrack.h"
#include "FixStkRptTrack.h"
//---------------------------------------------------------------------------
using namespace Kstl;

/////////////////////////////////////////////////////////////////////////////
// TFsStkOrderRec
/////////////////////////////////////////////////////////////////////////////
template <class TPersistent, class TStore, class Order>
bool Store_FixOrderRec (TPersistent& per, TStore& storage, Order& ord)
{
   /*
   bool ret = storage.store(per, ord.SenderSubID_)        &&    // 050
              storage.store(per, ord.TargetSubID_)        &&    // 057
              storage.store(per, ord.Account_)            &&    // 001
              storage.store(per, ord.ClOrdID_)            &&    // 011
              storage.store(per, ord.CumQty_)             &&    // 014
              storage.store(per, ord.HandlInst_)          &&    // 021
              storage.store(per, ord.OrderID_)            &&    // 037
              storage.store(per, ord.OrderQty_)           &&    // 038
              storage.store(per, ord.OrdStatus_)          &&    // 039
              storage.store(per, ord.OrdType_)            &&    // 040
              storage.store(per, ord.OrigClOrdID_)        &&    // 041
              storage.store(per, ord.Price_)              &&    // 044
              storage.store(per, ord.Symbol_)             &&    // 055
              storage.store(per, ord.Side_)               &&    // 054
              storage.store(per, ord.Text_)               &&    // 058
              storage.store(per, ord.TimeInForce_)        &&    // 059
              storage.store(per, ord.ExecType_)           &&    // 150
              storage.store(per, ord.LeavesQty_)          &&    // 151
              storage.store(per, ord.RefOrdID_)           &&
              storage.store(per, ord.FlowID_)             &&
              storage.store(per, ord.LastMsg_);

   if (!ret)
      return ret;

   //==============================================
   // Reserve
   int i;
   
   //for (i=0; i<TFsStkOrderRec::Reserve_DataType_Size; ++i) {
   //   if (!storage.store(per, ord.ReseChar_[i]))
   //      return false;
   //}
   if (!storage.store(per, ord.ReseChar_, TFsStkOrderRec::Reserve_DataType_Size*sizeof(TFixChar)))
      return false;

   //for (i=0; i<TFsStkOrderRec::Reserve_DataType_Size; ++i) {
   //   if (!storage.store(per, ord.ReseQty_[i]))
   //      return false;
   //}
   if (!storage.store(per, ord.ReseQty_, TFsStkOrderRec::Reserve_DataType_Size*sizeof(TFixQty2)))
      return false;
          
   //for (i=0; i<TFsStkOrderRec::Reserve_DataType_Size; ++i) {
   //   if (!storage.store(per, ord.ResePri_[i]))
   //      return false;
   //}
   if (!storage.store(per, ord.ResePri_, TFsStkOrderRec::Reserve_DataType_Size*sizeof(TFixPri2)))
      return false;

   for (i=0; i<TFsStkOrderRec::Reserve_DataType_Size; ++i) {
      if (!storage.store(per, ord.ReseStr_[i]))
         return false;
   }
   */

   //==============================================
   // 為了避免在存檔的時候一直換房間(房間大小不夠)
   // 乾脆一次建大一點囉
   static const unsigned RecFixedLen = 384;

   unsigned savedSize = per.get_saved_size();
   if (savedSize < RecFixedLen) {
      char_ary<char, RecFixedLen, 0> recFixed;
      unsigned                       diffSize = (RecFixedLen - savedSize);

      if (!storage.store(per, recFixed.begin(), diffSize))
         return false;
   }
   else {
      using namespace Kway::Nsa;
      
      char      msgBuf[1024];
      int       msgLen;
      TLogFile& logFile = TLogFile::GetInst();

      msgLen = sprintf(msgBuf, "<FIX::Stk::Store_FixOrderRec> savedSize[%d] >= RecFixedLen[%d], ClOrdID[%s], RefOrdID[%d]",
                                 savedSize,
                                 RecFixedLen,
                                 ord.ClOrdID_ .as_string().c_str(),
                                 ord.RefOrdID_.get_orig());

      logFile.LogSize(elt_Warning, msgBuf, msgLen);
   }

   return true;
}
//---------------------------------------------------------------------------

K_fn(bool) Kstl::pload (const Kstl::persistent& per, TFsStkOrderRec& rec)
{
   Kstl::persistent_load_nsa storage;
   return Store_FixOrderRec(per, storage, rec);
}
//---------------------------------------------------------------------------

K_fn(bool) Kstl::psave (Kstl::persistent& per, const TFsStkOrderRec& rec)
{
   Kstl::persistent_save_nsa storage;
   return Store_FixOrderRec(per, storage, rec);
}


/////////////////////////////////////////////////////////////////////////////
// struct SOrdTrack
/////////////////////////////////////////////////////////////////////////////
template <class TPersistent, class TStore, class TTrack>
bool Store_FixOrdTrackRec (TPersistent& per, TStore& storage, TTrack& track)
{
   bool ret = storage.store(per, track.RecvTime_)    &&
              storage.store(per, track.OrdCmd_)      &&
              storage.store(per, track.MsgSeqNum_)   &&  // 034
              storage.store(per, track.MsgType_)     &&  // 035
              storage.store(per, track.ClOrdID_)     &&  // 011
              storage.store(per, track.OrigClOrdID_) &&  // 041
              storage.store(per, track.OrderQty_)    &&  // 038
              storage.store(per, track.Price_)       &&  // 044
              storage.store(per, track.FlowID_);

   if (!ret)
      return ret;

   //==============================================
   // 為了避免在存檔的時候一直換房間(房間大小不夠)
   // 乾脆一次建大一點囉
   static const unsigned RecFixedLen = 128;

   unsigned savedSize = per.get_saved_size();
   if (savedSize < RecFixedLen) {
      char_ary<char, RecFixedLen, 0> recFixed;
      unsigned                       diffSize = (RecFixedLen - savedSize);

      if (!storage.store(per, recFixed.begin(), diffSize))
         return false;
   }

   return true;
}
//---------------------------------------------------------------------------

K_fn(bool) Kstl::pload (const Kstl::persistent& per, SOrdTrack& track)
{
   Kstl::persistent_load_nsa storage;
   return Store_FixOrdTrackRec(per, storage, track);
}
//---------------------------------------------------------------------------

K_fn(bool) Kstl::psave (Kstl::persistent& per, const SOrdTrack& track)
{
   Kstl::persistent_save_nsa storage;
   return Store_FixOrdTrackRec(per, storage, track);
}


//*************************************************
#if defined(_FixStkVer_RptTrack_)
/////////////////////////////////////////////////////////////////////////////
// struct SRptTrack
/////////////////////////////////////////////////////////////////////////////
template <class TPersistent, class TStore, class TTrack>
bool Store_FixRptTrackRec (TPersistent& per, TStore& storage, TTrack& track)
{
   bool ret = storage.store(per, track.MsgType_)     &&
              storage.store(per, track.ClOrdID_)     &&  // 011
              storage.store(per, track.OrigClOrdID_) &&  // 041
              storage.store(per, track.Text_)        &&  // 058
              storage.store(per, track.OrdStatus_)   &&  // 039
              storage.store(per, track.ExecType_)    &&  // 150
              storage.store(per, track.CumQty_)      &&  // 014
              storage.store(per, track.LastShares_)  &&  // 032
              storage.store(per, track.OrderQty_)    &&  // 038
              storage.store(per, track.LeavesQty_)   &&  // 151
              storage.store(per, track.LastPx_)      &&  // 031
              storage.store(per, track.Price_);          // 044

   return ret;
}
//---------------------------------------------------------------------------

K_fn(bool) Kstl::pload (const Kstl::persistent& per, SRptTrack& track)
{
   Kstl::persistent_load storage;
   return Store_FixRptTrackRec(per, storage, track);
}
//---------------------------------------------------------------------------

K_fn(bool) Kstl::psave (Kstl::persistent& per, const SRptTrack& track)
{
   Kstl::persistent_save storage;
   return Store_FixRptTrackRec(per, storage, track);
}
//---------------------------------------------------------------------------
#endif // defined(_FixStkVer_RptTrack_)
//*************************************************
