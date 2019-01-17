/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FixStkRptCenter.h"
#include "TextReader.h"
#include "src/Code/Message.h"
#include "Nsa_Per.h"
#include "Nsa_UtilComm.h"
#include "QFixExt/QFixPer.h"
#include "tplns/Nsa_EvCtn_0File.h"
#include "tplns/Nsa_EvCtn_1k_VA.h"
#include "FixStkIntf.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::FIX;
using namespace Kway::FIX::Stk;

/////////////////////////////////////////////////////////////////////////////
// class TRptCenter::TRsLoader
/////////////////////////////////////////////////////////////////////////////
class TRptCenter::TRsLoader : public TTextReader
{
   typedef TTextReader  inherited;

   TRsRpt&  RsRpt_;

public:
   inline K_ctor TRsLoader (const char* fileName, TRsRpt& rsRpt)
      : inherited(fileName, fmRead)
      , RsRpt_   (rsRpt)
   {
      inherited::StartNotifyReadLN(sm_ReadAll);
   }

   K_mf(EOnLineResult) OnTextLine (const std::string& str)
   {
      TFixSysRptID           fsrid;
      SStkRptWsRef*          datWsRef = new SStkRptWsRef;
      SStkRptWsPtr           datWsPtr(datWsRef);
      std::string::size_type beg, end;

      // UpdTime TFixSysRptID TFixSysOrdID TFixMsgType2 Message
      end = str.find(' ');
      if (end == std::string::npos)
         return olr_Continue;
      datWsRef->UpdTime_.assign(str.substr(0, end));
      
      beg = end + 1;
      end = str.find(' ', beg);
      if (end == std::string::npos)
         return olr_Continue;
      fsrid.assign(str.substr(beg, end-beg));

      beg = end + 1;
      end = str.find(' ', beg);
      if (end == std::string::npos)
         return olr_Continue;
      datWsRef->SysOrdID_.assign(str.substr(beg, end-beg));

      beg = end + 1;
      end = str.find(' ', beg);
      if (end == std::string::npos)
         return olr_Continue;
      datWsRef->MsgType_.assign(str.substr(beg, end-beg));

      datWsRef->Message_.assign(str.substr(end+1));

      RsRpt_.insert(TRsRpt::value_type(fsrid, datWsPtr));
      
      return olr_Continue;
   }
};


/////////////////////////////////////////////////////////////////////////////
// class TRptCenter impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TRptCenter::TRptCenter (TTradeInfo* ti)
   : RsRptVac_   (NULL)
   , IsAutoFlush_(ti->GetFlushRptCenter())
{
   char fullPath[256];
   
   sprintf(fullPath, "%sSysRpts3.txt", ti->GetLogPath().c_str());
   TRsLoader rsLoader(fullPath, RsRpt_);

   RsRptText_.Open(fullPath, (TFileMode)(fmAppend|fmCreatePath));
}
//---------------------------------------------------------------------------

K_mf(TFixSysRptID) TRptCenter::NewRpt (const TFixSysOrdID& sysOrdID, const TFixMsgType2& msgType, const std::string& message)
{
   SStkRptWsRef* datWsRef = new SStkRptWsRef;
   SStkRptWsPtr  datWsPtr(datWsRef);

   datWsRef->SysOrdID_ = sysOrdID;
   datWsRef->MsgType_  = msgType;
   datWsRef->Message_  = message;

   return NewRpt(datWsPtr);
}
//---------------------------------------------------------------------------

K_mf(TFixSysRptID) TRptCenter::NewRpt (SStkRptWsPtr& datWsPtr)
{
   TFixSysRptID newKey(1);

   if (!RsRpt_.empty()) {
      newKey = (RsRpt_.end()-1).get_idx();
      ++newKey;
   }

   datWsPtr->UpdTime_ = TTimeStamp::Now();

   RsRpt_.insert(TRsRpt::value_type(newKey, datWsPtr));
      
   WriteToFile(newKey, datWsPtr.get());

   return newKey;
}
//---------------------------------------------------------------------------

K_mf(TFixSysRptID) TRptCenter::GetLastRptID () const
{
   if (RsRpt_.empty())
      return TFixSysRptID();

   TFixSysRptID rptID((RsRpt_.end()-1).get_idx());

   return rptID;
}
//---------------------------------------------------------------------------

K_mf(void) TRptCenter::WriteToFile (const TFixSysRptID& srid, const SStkRptWsRef* datWsRef)
{
   char buf[1024*10];
   int  len;
   
   // UpdTime, TFixSysRptID, TFixSysOrdID, TFixMsgType2, Message
   len = sprintf(buf, "%s %d %d %s %s\n",
                           datWsRef->UpdTime_ .as_string().c_str(),
                           srid.get_orig(),
                           datWsRef->SysOrdID_.get_orig(),
                           datWsRef->MsgType_ .as_string().c_str(),
                           datWsRef->Message_ .c_str());

   RsRptText_.Write(buf, len);
   
   if (IsAutoFlush_)
      RsRptText_.Flush();
}
//---------------------------------------------------------------------------

K_mf(std::string) TRptCenter::FindOrderID (const TFixSysRptID& srid) const
{
   TRsRptCi citer = RsRpt_.find(srid);
   if (citer == RsRpt_.end())
      return std::string();

   const SStkRptWsRef* cprRpt = citer->get();
   if (!cprRpt)
      return std::string();

   Message     fixMsg(cprRpt->Message_, false);
   std::string orderID;

   fixMsg.findField(FIELD::OrderID, orderID);
   return orderID;
}


/////////////////////////////////////////////////////////////////////////////
// class TSysRptsFm
/////////////////////////////////////////////////////////////////////////////
class TSysRptsFm
{
   typedef TRptCenter::TRsRpt   container;
   typedef container::iterator  iterator;
   enum {
      IField_SysRptID,
         IField_SysOrdID,
         IField_UpdTime,
         IField_MsgType,
         IField_ClOrdID,
         IField_OrigClOrdID,
         IField_OrderID,
         IField_OrdStatus,
         IField_ExecType,
         IField_OrderQty,
         IField_LeavesQty,
         IField_CumQty,
         IField_LastQty,
         IField_LastPx,
         IField_TransactTime,
         IField_ExecID,
         IField_Text,
         IField_OrdRejReason,
         IField_ExecTransType,
         IField_ExecRestatementReason,
         IField_CxlRejResponseTo,
         IField_CxlRejReason,
         IField_Message,
      IField_Total
   };

public:
   // GetFieldDesp
   K_mf(const Kway::TFieldDesp&) GetFieldDesp (size_t n) const
   {
      static Kway::TFieldDesp FdDsp[] = {
         { "SysRptID",              10, Kway::ta_xLeft_yCenter,   Kway::ta_xLeft_yCenter,   Kway::tfs_Fixed,   },
         { "SysOrdID",               8, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_Fixed,   },
         { "UpdTime",               10, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "MsgType",                8, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "ClOrdID",               10, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "OrigClOrdID",           10, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "OrderID",                8, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "OrdStatus",             10, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "ExecType",              12, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "OrderQty",              11, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "LeavesQty",             13, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "CumQty",                 9, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "LastQty",               10, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "LastPx",                 9, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "TransactTime",          15, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "ExecID",                15, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "Text",                  15, Kway::ta_xLeft_yCenter,   Kway::ta_xLeft_yCenter,   Kway::tfs_CanEdit, },
         { "OrdRejReason",          12, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "ExecTransType",         13, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "ExecRestatementReason", 21, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "CxlRejResponseTo",      16, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "CxlRejReason",          12, Kway::ta_xCenter_yCenter, Kway::ta_xCenter_yCenter, Kway::tfs_CanEdit, },
         { "Message",               30, Kway::ta_xLeft_yCenter,   Kway::ta_xLeft_yCenter,   Kway::tfs_CanEdit, },
      };
      return FdDsp[n];
   };

   // Get
   K_mf(std::string) Get (const iterator& i, size_t fieldNo) const
   {
      #define FixStk_TSysRptsFm_Get(FieldNum)               \
         {                                                  \
            Message     fixMsg(cdatWsRef->Message_, false); \
            std::string str;                                \
            if (fixMsg.findField(FieldNum, str))            \
               return str;                                  \
         }                                                  \
         break;                                             \
      //------------------------------------------------------

      if (fieldNo == IField_SysRptID) {
         return i.get_idx().as_format(6, 0);
      }

      const SStkRptWsRef* cdatWsRef = i->get();
      if (!cdatWsRef)
         return std::string();

      switch (fieldNo) {
         case IField_UpdTime:                return cdatWsRef->UpdTime_ .as_string();
         case IField_SysOrdID:               return cdatWsRef->SysOrdID_.as_string();
         case IField_MsgType:                return cdatWsRef->MsgType_ .as_string();
         case IField_ClOrdID:                FixStk_TSysRptsFm_Get(FIELD::ClOrdID)
         case IField_OrigClOrdID:            FixStk_TSysRptsFm_Get(FIELD::OrigClOrdID)
         case IField_OrderID:                FixStk_TSysRptsFm_Get(FIELD::OrderID)
         case IField_OrdStatus:              FixStk_TSysRptsFm_Get(FIELD::OrdStatus)
         case IField_ExecType:               FixStk_TSysRptsFm_Get(FIELD::ExecType)
         case IField_OrderQty:               FixStk_TSysRptsFm_Get(FIELD::OrderQty)
         case IField_LeavesQty:              FixStk_TSysRptsFm_Get(FIELD::LeavesQty)
         case IField_CumQty:                 FixStk_TSysRptsFm_Get(FIELD::CumQty)
         case IField_LastQty:                FixStk_TSysRptsFm_Get(FIELD::LastQty)
         case IField_LastPx:                 FixStk_TSysRptsFm_Get(FIELD::LastPx)
         case IField_TransactTime:           FixStk_TSysRptsFm_Get(FIELD::TransactTime)
         case IField_ExecID:                 FixStk_TSysRptsFm_Get(FIELD::ExecID)
         case IField_Text:                   FixStk_TSysRptsFm_Get(FIELD::Text)
         case IField_OrdRejReason:           FixStk_TSysRptsFm_Get(FIELD::OrdRejReason)
         case IField_ExecTransType:          FixStk_TSysRptsFm_Get(FIELD::ExecTransType)
         case IField_ExecRestatementReason:  FixStk_TSysRptsFm_Get(FIELD::ExecRestatementReason)
         case IField_CxlRejResponseTo:       FixStk_TSysRptsFm_Get(FIELD::CxlRejResponseTo)
         case IField_CxlRejReason:           FixStk_TSysRptsFm_Get(FIELD::CxlRejReason)
         case IField_Message:                return cdatWsRef->Message_;
      }
      return std::string();
   }

   inline K_mf(size_t) GetFieldCount () const  { return IField_Total; }
   inline K_mf(void)   Put           (container& c, iterator& i, size_t fieldNo, const std::string& str) {}
};
/////////////////////////////////////////////////////////////////////////////
// class TRptCenter::TRsRptVac
/////////////////////////////////////////////////////////////////////////////
class TRptCenter::TRsRptVac : public TEvCtn1kVA<TRsRpt, TSysRptsFm>
{
   typedef TEvCtn1kVA<TRsRpt, TSysRptsFm>  inherited;

public:
   inline K_ctor TRsRptVac (TRsRpt* c) : inherited(c) {}
};


/////////////////////////////////////////////////////////////////////////////
// class TRptCenter impl
/////////////////////////////////////////////////////////////////////////////
K_dtor TRptCenter::~TRptCenter ()
{
   if (RsRptVac_)  delete RsRptVac_;
}
//---------------------------------------------------------------------------

K_mf(TVACreater&) TRptCenter::GetSysRptsVAC ()
{
   if (RsRptVac_ == NULL)
      RsRptVac_ = new TRsRptVac(&RsRpt_);
   return *RsRptVac_;
}
//---------------------------------------------------------------------------
