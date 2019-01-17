/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "TwseFsOrdCenterVA.h"
#include "TwseFsOrd.h"
//---------------------------------------------------------------------------
using namespace Kway;

/////////////////////////////////////////////////////////////////////////////
// class TTwseFsOrdCenterVA impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TTwseFsOrdCenterVA::TTwseFsOrdCenterVA (TTradeInfo* ti)
   : SysOrds_ (ti->GetFixOrdCenter()->GetSysOrds())
   , OrdTrack_(ti->GetFixOrdTrack())
   //**********************************************
   #if defined(_FixStkVer_RptTrack_)
   , RptTrack_(ti->GetFixRptTrack())
   #endif
   //**********************************************
{
}
//---------------------------------------------------------------------------

K_mf(TaViewArea) TTwseFsOrdCenterVA::CreateNewVA ()
{
   TViewControls vcs;
   TSysOrdsFm    maker;
   TSysOrdsVA*   viewer;

   vcs.set(vco_InputKey);
   viewer = new TSysOrdsVA(SysOrds_, maker, vcs);

   return TaViewArea(viewer);
}
//---------------------------------------------------------------------------

K_mf(size_t) TTwseFsOrdCenterVA::GetDetails (const char**& a)
{
   //**********************************************
   #if !defined(_FixStkVer_RptTrack_)
   static const char* desp[] = { "쯥캵었�{" };
   #else
   static const char* desp[] = { "쯥캵었�{", "�^놜었�{" };
   #endif
   //**********************************************
   a = desp;
   return numofele(desp);
}
//---------------------------------------------------------------------------

K_mf(TVACreater*) TTwseFsOrdCenterVA::GetDetailVACreater (size_t detailID)
{
   //**********************************************
   #if !defined(_FixStkVer_RptTrack_)
   return &OrdTrackVACr_;

   #else //----------------------------------------
   
   if (detailID == 0)
      return &OrdTrackVACr_;
   return &RptTrackVACr_;
   #endif
   //**********************************************
}
//---------------------------------------------------------------------------

K_mf(TVACreater*) TTwseFsOrdCenterVA::OnMasterViewRowMoved (TViewArea& masterVA, TViewArea*& detailVA, size_t detailID)
{
   TSysOrdsVA&                    va  = static_cast<TSysOrdsVA&>(masterVA);
   const TSysOrdsVA::cursor_type* cur = va.GetCurRow();
   TSysOrdID                      sysOrdID;

   if (cur->IsInvalid() == false) {
      sysOrdID = cur->GetIterator().get_idx();
   }

   if (detailID == 0) {
      TOrdTrackVA* detailView = static_cast<TOrdTrackVA*>(detailVA);

      if (sysOrdID.is_0()) {
         detailView->SetContainer(0);
      }
      else {
         detailView->SetContainer(OrdTrack_->FindRsSeqNo(sysOrdID));
      }
   }
   //**********************************************
   #if defined(_FixStkVer_RptTrack_)
   else {
      TRptTrackVA* detailView = static_cast<TRptTrackVA*>(detailVA);

      if (sysOrdID.is_0()) {
         detailView->SetContainer(0);
      }
      else {
         detailView->SetContainer(RptTrack_->FindRsSeqNo(sysOrdID));
      }
   }
   #endif
   //**********************************************

   return inherited::DetailVACreaterNoChange;
}


/////////////////////////////////////////////////////////////////////////////
// class TTwseFsOrdCenterVA::TSysOrdsFm impl
/////////////////////////////////////////////////////////////////////////////
enum ESysOrdsField
{
   ISysOrds_SysOrdID,
      ISysOrds_RefOrdID,
      ISysOrds_TargetSubID,
      ISysOrds_SenderSubID,
      ISysOrds_OrderID,
      ISysOrds_ClOrdID,
      //ISysOrds_OrigClOrdID,
      ISysOrds_OrderQty,
      ISysOrds_LeavesQty,
      ISysOrds_CumQty,
      ISysOrds_OrdStatus,
      ISysOrds_ExecType,
      ISysOrds_FlowID,
      ISysOrds_LastMsg,
      ISysOrds_Text,
      ISysOrds_Account,
      ISysOrds_Symbol,
      ISysOrds_Side,
      ISysOrds_Price,
      ISysOrds_TwseOrdType,
      ISysOrds_TwseIvacnoFlag,
      ISysOrds_TwseExCode,
      ISysOrds_TwseWarrantLP,
      ISysOrds_OrdType,
      ISysOrds_HandlInst,
      ISysOrds_TimeInForce,
   ISysOrds_Total
};
//---------------------------------------------------------------------------

K_mf(size_t) TTwseFsOrdCenterVA::TSysOrdsFm::GetFieldCount () const
{
   return ISysOrds_Total; 
}
//---------------------------------------------------------------------------

K_mf(const TFieldDesp&) TTwseFsOrdCenterVA::TSysOrdsFm::GetFieldDesp (size_t n) const
{
   static TFieldDesp FdDsp[] = {
      { "SysOrdID",         10, ta_xLeft_yCenter,   ta_xLeft_yCenter,   tfs_Fixed,   },
      { "RefOrdID",         10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "TargetSubID",      10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "SenderSubID",      10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "OrderID",          10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "ClOrdID",          10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      //{ "OrigClOrdID",      10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "OrderQty",         10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "LeavesQty",        10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "CumQty",           10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "OrdStatus",        10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "ExecType",         10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "FlowID",           10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "LastMsg",          30, ta_xLeft_yCenter,   ta_xLeft_yCenter,   tfs_CanEdit, },
      { "Text",             15, ta_xLeft_yCenter,   ta_xLeft_yCenter,   tfs_CanEdit, },
      { "Account",          10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "Symbol",           10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "Side",             10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "Price",            10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "TwseOrdType",      10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "TwseIvacnoFlag",   10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "TwseExCode",       10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "TwseWarrantLP",    10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "OrdType",          10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "HandlInst",        10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "TimeInForce",      10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------

K_mf(std::string) TTwseFsOrdCenterVA::TSysOrdsFm::Get (const iterator& i, size_t fieldNo) const
{
   if (fieldNo == ISysOrds_SysOrdID)
      return i.get_idx().as_format(6, 0);

   const FIX::Stk::TFsSysOrdRef* sysOrd = i->get();
   if (sysOrd == NULL)
      return std::string();

   switch (fieldNo) {
      case ISysOrds_RefOrdID:       return sysOrd->RefOrdID_     .as_string();
      case ISysOrds_SenderSubID:    return sysOrd->SenderSubID_  .as_string();
      case ISysOrds_ClOrdID:        return sysOrd->ClOrdID_      .as_string();
      case ISysOrds_Account:        return sysOrd->Account_      .as_string();
      //case ISysOrds_OrigClOrdID:    return sysOrd->OrigClOrdID_  .as_string();
      case ISysOrds_Symbol:         return sysOrd->Symbol_       .as_string();
      case ISysOrds_Side:           return sysOrd->Side_         .as_string();
      case ISysOrds_Price:          return sysOrd->Price_        .as_string();
      case ISysOrds_OrderID:        return sysOrd->OrderID_      .as_string();
                                                                 
      case ISysOrds_CumQty:         return sysOrd->CumQty_       .as_string();
      case ISysOrds_OrderQty:       return sysOrd->OrderQty_     .as_string();
      case ISysOrds_LeavesQty:      return sysOrd->LeavesQty_    .as_string();
                                                                 
      case ISysOrds_OrdType:        return sysOrd->OrdType_      .as_string();
      case ISysOrds_HandlInst:      return sysOrd->HandlInst_    .as_string();
      case ISysOrds_OrdStatus:      return sysOrd->OrdStatus_    .as_string();
      case ISysOrds_TimeInForce:    return sysOrd->TimeInForce_  .as_string();
      case ISysOrds_Text:           return sysOrd->Text_         .as_string();
      case ISysOrds_FlowID:         return sysOrd->FlowID_       .as_string();
      
      #if defined(_FixTag_Twse_Ver2_)
      case ISysOrds_LastMsg:        return sysOrd->LastMsg_      .as_string();
      #else
      case ISysOrds_LastMsg:        return sysOrd->LastMsg_;
      #endif
      
      case ISysOrds_ExecType:
         {
            TFixTwseExecType execType(sysOrd->ExecType_);
            return execType.as_string();
         }
      
      case ISysOrds_TargetSubID:
         {
            TFixTwseTargetSubID twseTargetSubID(sysOrd->TargetSubID_);
            return twseTargetSubID.as_string();
         }
         
      // Reserve Char
      case ISysOrds_TwseOrdType:
         {
            const STwseFsOrdReseChar* dat = reinterpret_cast<const STwseFsOrdReseChar*>(sysOrd->GetReseChar());
            return dat->TwseOrdType_.as_string();
         }
      case ISysOrds_TwseIvacnoFlag:
         {
            const STwseFsOrdReseChar* dat = reinterpret_cast<const STwseFsOrdReseChar*>(sysOrd->GetReseChar());
            return dat->TwseIvacnoFlag_.as_string();
         }
      case ISysOrds_TwseExCode:
         {
            const STwseFsOrdReseChar* dat = reinterpret_cast<const STwseFsOrdReseChar*>(sysOrd->GetReseChar());
            return dat->TwseExCode_.as_string();
         }
      case ISysOrds_TwseWarrantLP:
         {
            const STwseFsOrdReseChar* dat = reinterpret_cast<const STwseFsOrdReseChar*>(sysOrd->GetReseChar());
            return dat->TwseWarrantLP_.as_string();
         }
   }
   return std::string();
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFsOrdCenterVA::TSysOrdsFm::Put (container& c, iterator& i, size_t fieldNo, const std::string& str)
{
   if (fieldNo == ISysOrds_SysOrdID)
      return;
   
   updater                 u      = c.begin_update(i);
   FIX::Stk::TFsSysOrdRef* sysOrd = (*u).get();
   
   if (!sysOrd)
      return;
   
   switch (fieldNo) {
      case ISysOrds_Account:    sysOrd->Account_  .assign(str);  break;
      case ISysOrds_LeavesQty:  sysOrd->LeavesQty_.assign(str);  break;
   }
}


/////////////////////////////////////////////////////////////////////////////
// class TTwseFsOrdCenterVA::TOrdTrackFm impl
/////////////////////////////////////////////////////////////////////////////
enum EOrdTrackField
{
   IOrdTrack_SysSeqNo,
      IOrdTrack_RecvTime,
      IOrdTrack_MsgSeqNum,
      IOrdTrack_OrdCmd,
      IOrdTrack_MsgType,
      IOrdTrack_ClOrdID,
      IOrdTrack_OrigClOrdID,
      IOrdTrack_OrderQty,
      IOrdTrack_Price,
      IOrdTrack_FlowID,
      IOrdTrack_TwseWarrantLP,
   IOrdTrack_Total
};
//---------------------------------------------------------------------------

K_mf(size_t) TTwseFsOrdCenterVA::TOrdTrackFm::GetFieldCount () const
{
   return IOrdTrack_Total; 
}
//---------------------------------------------------------------------------

K_mf(const TFieldDesp&) TTwseFsOrdCenterVA::TOrdTrackFm::GetFieldDesp (size_t n) const
{
   static TFieldDesp FdDsp[] = {
      { "SeqNo",         10, ta_xLeft_yCenter,   ta_xLeft_yCenter,   tfs_Fixed,   },
      { "RecvTime",      10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "MsgSeqNum",     10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "OrdCmd",        10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "MsgType",       10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "ClOrdID",       10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "OrigClOrdID",   10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "OrderQty",      10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "Price",         10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "FlowID",        10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "TwseWarrantLP", 10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------

K_mf(std::string) TTwseFsOrdCenterVA::TOrdTrackFm::Get (const iterator& i, size_t fieldNo) const
{
   switch (fieldNo) {
      case IOrdTrack_SysSeqNo:     return i->first.as_string();
      case IOrdTrack_RecvTime:     return i->second.RecvTime_   .as_string();
      case IOrdTrack_MsgSeqNum:    return i->second.MsgSeqNum_  .as_string();
      case IOrdTrack_OrdCmd:       return i->second.OrdCmd_     .as_desc  ();
      case IOrdTrack_MsgType:      return i->second.MsgType_    .as_string();
      case IOrdTrack_ClOrdID:      return i->second.ClOrdID_    .as_string();
      case IOrdTrack_OrigClOrdID:  return i->second.OrigClOrdID_.as_string();
      case IOrdTrack_OrderQty:     return i->second.OrderQty_   .as_string();
      case IOrdTrack_Price:        return i->second.Price_      .as_string();
      case IOrdTrack_FlowID:       return i->second.FlowID_     .as_string();
      case IOrdTrack_TwseWarrantLP:
         {
            STwseOrdTrackReseChar* otRChar = reinterpret_cast<STwseOrdTrackReseChar*>(i->second.GetRChar());
            return otRChar->TwseWarrantLP_.as_string();
         }
   }
   return std::string();
}
//---------------------------------------------------------------------------

K_mf(void) TTwseFsOrdCenterVA::TOrdTrackFm::Put (container& c, iterator& i, size_t fieldNo, const std::string& str)
{
   if (fieldNo == IOrdTrack_SysSeqNo)
      return;
      
   if (fieldNo == IOrdTrack_FlowID) {
      updater u = c.begin_update(i);
      
      switch (fieldNo) {
         case IOrdTrack_FlowID:  u->FlowID_.assign(str);  break;
      }
   }
}


//*************************************************
#if defined(_FixStkVer_RptTrack_)
/////////////////////////////////////////////////////////////////////////////
// class TTwseFsOrdCenterVA::TRptTrackFm impl
/////////////////////////////////////////////////////////////////////////////
enum ERptTrackField
{
   IRptTrack_SysSeqNo,
      IRptTrack_MsgType,
      IRptTrack_ClOrdID,
      IRptTrack_OrigClOrdID,
      IRptTrack_OrdStatus,
      IRptTrack_ExecType,
      IRptTrack_OrderQty,
      IRptTrack_CumQty,
      IRptTrack_LeavesQty,
      IRptTrack_Price,
      IRptTrack_LastShares,
      IRptTrack_LastPx,
      IRptTrack_Text,
   IRptTrack_Total
};
//---------------------------------------------------------------------------

K_mf(size_t) TTwseFsOrdCenterVA::TRptTrackFm::GetFieldCount () const
{
   return IRptTrack_Total; 
}
//---------------------------------------------------------------------------

K_mf(const TFieldDesp&) TTwseFsOrdCenterVA::TRptTrackFm::GetFieldDesp (size_t n) const
{
   static TFieldDesp FdDsp[] = {
      { "SeqNo",       10, ta_xLeft_yCenter,   ta_xLeft_yCenter,   tfs_Fixed,   },
      { "MsgType",     10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "ClOrdID",     10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "OrigClOrdID", 10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "OrdStatus",   10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "ExecType",    10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "OrderQty",    10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "CumQty",      10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "LeavesQty",   10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "Price",       10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "LastShares",  10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "LastPx",      10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
      { "Text",        10, ta_xCenter_yCenter, ta_xCenter_yCenter, tfs_CanEdit, },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------

K_mf(std::string) TTwseFsOrdCenterVA::TRptTrackFm::Get (const iterator& i, size_t fieldNo) const
{
   switch (fieldNo) {
      case IRptTrack_SysSeqNo:     return i->first.as_string();
      case IRptTrack_MsgType:      return i->second.MsgType_    .as_string();
      case IRptTrack_ClOrdID:      return i->second.ClOrdID_    .as_string();
      case IRptTrack_OrigClOrdID:  return i->second.OrigClOrdID_.as_string();
      case IRptTrack_OrdStatus:    return i->second.OrdStatus_  .as_string();
      case IRptTrack_ExecType:     return i->second.ExecType_   .as_string();
      case IRptTrack_CumQty:       return i->second.CumQty_     .as_string();
      case IRptTrack_OrderQty:     return i->second.OrderQty_   .as_string();
      case IRptTrack_LeavesQty:    return i->second.LeavesQty_  .as_string();
      case IRptTrack_Price:        return i->second.Price_      .as_string();
      case IRptTrack_LastShares:   return i->second.LastShares_ .as_string();
      case IRptTrack_LastPx:       return i->second.LastPx_     .as_string();
      case IRptTrack_Text:         return i->second.Text_       .as_string();
   }
   return std::string();
}
//---------------------------------------------------------------------------
#endif
//*************************************************
