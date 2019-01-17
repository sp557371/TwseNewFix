/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FixStkOrdCenterVA.h"
#include "FixStkIntf.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::FIX;
using namespace Kway::FIX::Stk;

/////////////////////////////////////////////////////////////////////////////
// class TFsOrdCenterVA::TSysOrdsFm
/////////////////////////////////////////////////////////////////////////////
class TFsOrdCenterVA::TSysOrdsFm
{
   typedef TFsSysOrds           container;
   typedef container::iterator  iterator;

   enum {
      IField_SysOrdID,
         IField_ClOrdID,
         IField_Account,
         IField_Symbol,
         IField_HandlInst,
         IField_Side,
         IField_OrderQty,
         IField_Price,
         IField_OrderID,
         IField_OrdStatus,
         IField_ExecType,
      IField_Total
   };

public:
   K_mf(const TFieldDesp&) GetFieldDesp (size_t n)                          const;
   K_mf(std::string)             Get          (const iterator& i, size_t fieldNo) const;

   inline K_mf(size_t) GetFieldCount () const  { return IField_Total; }
   inline K_mf(void)   Put           (container& c, iterator& i, size_t fieldNo, const std::string& str) {}
};
/////////////////////////////////////////////////////////////////////////////
// class TFsOrdCenterVA::TSysOrdsFm impl
/////////////////////////////////////////////////////////////////////////////
K_mf(const TFieldDesp&) TFsOrdCenterVA::TSysOrdsFm::GetFieldDesp (size_t n) const
{
   static TFieldDesp FdDsp[] = {
      { "SysOrdID",         10, ta_xLeft_yCenter,  ta_xLeft_yCenter,  tfs_Fixed,   },
      { "ClOrdID",          10, ta_xRight_yCenter, ta_xRight_yCenter, tfs_CanEdit, },
      { "Account",          10, ta_xRight_yCenter, ta_xRight_yCenter, tfs_CanEdit, },
      { "Symbol",           10, ta_xRight_yCenter, ta_xRight_yCenter, tfs_CanEdit, },
      { "HandlInst",        10, ta_xRight_yCenter, ta_xRight_yCenter, tfs_CanEdit, },
      { "Side",             10, ta_xRight_yCenter, ta_xRight_yCenter, tfs_CanEdit, },
      { "OrderQty",         10, ta_xRight_yCenter, ta_xRight_yCenter, tfs_CanEdit, },
      { "Price",            10, ta_xRight_yCenter, ta_xRight_yCenter, tfs_CanEdit, },
      { "OrderID",          10, ta_xRight_yCenter, ta_xRight_yCenter, tfs_CanEdit, },
      { "OrdStatus",        10, ta_xRight_yCenter, ta_xRight_yCenter, tfs_CanEdit, },
      { "ExecType",         10, ta_xRight_yCenter, ta_xRight_yCenter, tfs_CanEdit, },
   };
   return FdDsp[n];
}
//---------------------------------------------------------------------------

K_mf(std::string) TFsOrdCenterVA::TSysOrdsFm::Get (const iterator& i, size_t fieldNo) const
{
   if (fieldNo == IField_SysOrdID)
      return i.get_idx().as_format(6, 0);

   const TFsSysOrdRef* sysOrd = i->get();
   if (sysOrd == NULL)
      return std::string();

   switch (fieldNo) {
      case IField_ClOrdID:           return sysOrd->ClOrdID_         .as_string();
      case IField_Account:           return sysOrd->Account_         .as_string();
      case IField_Symbol:            return sysOrd->Symbol_          .as_string();
      case IField_HandlInst:         return sysOrd->HandlInst_       .as_desc  ();
      case IField_Side:              return sysOrd->Side_            .as_string();
      case IField_OrderQty:          return sysOrd->OrderQty_        .as_string();
      case IField_Price:             return sysOrd->Price_           .as_string();
      case IField_OrderID:           return sysOrd->OrderID_         .as_string();
      case IField_OrdStatus:         return sysOrd->OrdStatus_       .as_desc  ();
      case IField_ExecType:          return sysOrd->ExecType_        .as_desc  ();
   }
   return std::string();
}


/////////////////////////////////////////////////////////////////////////////
// class TFsOrdCenterVA impl
/////////////////////////////////////////////////////////////////////////////
K_ctor TFsOrdCenterVA::TFsOrdCenterVA (TTradeInfo* ti)
   : SysOrds_(ti->GetFixOrdCenter()->GetSysOrds())
{
}
//---------------------------------------------------------------------------

K_mf(TaViewArea) TFsOrdCenterVA::CreateNewVA ()
{
   typedef TTViewArea<TFsSysOrds, TSysOrdsFm>  TViewer;

   TViewControls vcs;
   TSysOrdsFm    maker;
   TViewer*      viewer;
      
   vcs.set(vco_InputKey);
   viewer = new TViewer(SysOrds_, maker, vcs);

   return TaViewArea(viewer);
}
//---------------------------------------------------------------------------
