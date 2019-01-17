/////////////////////////////////////////////////////////////////////////////
#if defined(__BORLANDC__)
#pragma hdrstop
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "QFixType.h"
#include "KFile.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::FIX;

/////////////////////////////////////////////////////////////////////////////
// class TFieldValue impl
/////////////////////////////////////////////////////////////////////////////
static const std::string NoTagStr("\0x02");
//---------------------------------------------------------------------------
K_mf(void) TFieldValue::SetNoTag ()
{
   inherited::operator = (NoTagStr);
}
//---------------------------------------------------------------------------

K_mf(bool) TFieldValue::IsNoTag () const
{
   return (inherited::compare(NoTagStr) == 0);
}
//---------------------------------------------------------------------------

K_mf(std::string) TFieldValue::as_string () const
{
   if (IsNoTag()) {
      return std::string("●無此Tag資料●");
   }
   return *((inherited*)this);
}
//---------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////
// class TFixSesInfo impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TFixSesInfo::SetFixLogPath (const char* filePath)
{
   // 利用 TFile 來幫我確認目錄存在；在 QFix 中，如果目錄不存在會發生錯誤
   // 目錄必須要有 \\ 結尾，否則會當作是檔案
   Kway::TFile file(filePath, (Kway::TFileMode)(Kway::fmOpenAlways|Kway::fmCreatePath));

   FixLogPath_.assign(filePath);
}
//---------------------------------------------------------------------------
