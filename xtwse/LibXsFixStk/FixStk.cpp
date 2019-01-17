/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FixStk.h"
#include "Nsa_Base.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::FIX;
using namespace Kway::FIX::Stk;
   
/////////////////////////////////////////////////////////////////////////////
// class TFixOrdCmdID impl
/////////////////////////////////////////////////////////////////////////////
static const char* FixOrdCmdID_List[] = {
   "",
   "N",
   "Q",
   "D",
   "CQ",
   "CP",
   "CPQ",
   "AN",
   "RN",
};
//---------------------------------------------------------------------------

static const char* FixOrdCmdID_Desc[] = {
   "",
   "新增",
   "查詢",
   "刪單",
   "改量",
   "改價",
   "改價量",
   "自動新增",
   "回報新增",
};
//---------------------------------------------------------------------------

K_mf(void) TFixOrdCmdID::assign (const char* str)
{
   Data_ = fcid_Unknown;
   if (str == NULL || strlen(str) == 0)
      return;
      
   for (int i=fcid_Start; i<fcid_Total; ++i) {
      if (stricmp(str, FixOrdCmdID_List[i]) == 0) {
         Data_ = (EFixOrdCmdID)i;
         break;
      }
   }
}
//---------------------------------------------------------------------------

K_mf(std::string) TFixOrdCmdID::as_string () const
{
   if (Data_ > fcid_Unknown && Data_ < fcid_Total)
      return std::string(FixOrdCmdID_List[Data_]);

   return std::string();
}
//---------------------------------------------------------------------------

K_mf(std::string) TFixOrdCmdID::as_desc () const
{
   if (Data_ > fcid_Unknown && Data_ < fcid_Total)
      return std::string(FixOrdCmdID_Desc[Data_]);

   return std::string();
}


/////////////////////////////////////////////////////////////////////////////
// class TFixOrdFlowID impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TFixOrdFlowID::assign (const char* str)
{
   int val = atoi(str);
   if (val >= fof_None && val < fof_Total)
      Data_ = (EFixOrdFlowID)val;
   else
      Data_ = fof_None;
}
//---------------------------------------------------------------------------

K_mf(std::string) TFixOrdFlowID::as_string () const
{
   static const char* Item_List[] = {
      "",
      "Pending",
      "Sent",
      "Finish",
      "AbortOrdClear",
      "AbortOrdNoChange",
   };

   return std::string(Item_List[Data_]);
}


/////////////////////////////////////////////////////////////////////////////
// class TFixRptCmdID impl
/////////////////////////////////////////////////////////////////////////////
static const char* FixRptCmdID_Desc[] = {
   "",
   "1.ExecutionReport(Order)",
   "2.ExecutionReport(Reject)",
   "3.ExecutionReport(Deal)",
   "4.OrderCancelReject",
};
//---------------------------------------------------------------------------

K_mf(void) TFixRptCmdID::assign (const char* str)
{
   Data_ = frid_Unknown;
   if (str == NULL || strlen(str) == 0)
      return;
      
   Data_ = (EFixRptCmdID)(atoi(str));
}
//---------------------------------------------------------------------------

K_mf(std::string) TFixRptCmdID::as_string () const
{
   if (Data_ > frid_Unknown && Data_ < frid_Total)
      return std::string(FixRptCmdID_Desc[Data_]);

   return std::string();
}


/////////////////////////////////////////////////////////////////////////////
// class TBaseFunc impl
/////////////////////////////////////////////////////////////////////////////
K_mf(std::string) TBaseFunc::GetVersion ()
{
   return std::string("1.2.1");
}
//---------------------------------------------------------------------------

K_mf(std::string) TBaseFunc::GetModifyDate ()
{
   return std::string("2011-05-18");
}
//---------------------------------------------------------------------------
