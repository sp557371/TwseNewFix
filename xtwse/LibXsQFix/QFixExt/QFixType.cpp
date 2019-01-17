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
      return std::string("���L��Tag��ơ�");
   }
   return *((inherited*)this);
}
//---------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////
// class TFixSesInfo impl
/////////////////////////////////////////////////////////////////////////////
K_mf(void) TFixSesInfo::SetFixLogPath (const char* filePath)
{
   // �Q�� TFile �����ڽT�{�ؿ��s�b�F�b QFix ���A�p�G�ؿ����s�b�|�o�Ϳ��~
   // �ؿ������n�� \\ �����A�_�h�|��@�O�ɮ�
   Kway::TFile file(filePath, (Kway::TFileMode)(Kway::fmOpenAlways|Kway::fmCreatePath));

   FixLogPath_.assign(filePath);
}
//---------------------------------------------------------------------------
