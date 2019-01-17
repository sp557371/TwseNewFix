//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "BxLoger.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx;
//---------------------------------------------------------------------------
K_mf(void) TKOESLog::WriteLog (const std::string& msg, const std::string& msg2, const std::string& mark)
{
  if(FName_.empty())
    return;

  Log_.Open(FName_.c_str(), Kway::TFileMode(Kway::fmRandomAccess | Kway::fmOpenAlways | Kway::fmCreatePath));
  if(Log_.IsOpened())
  {
    Log_.Seek(0, Kway::fsEnd);
    KoesLogFmt fmt;
    fmt.Date_.assign(Kway::TKDate::Now().as_string(Kway::TKDate::sf_ELong, 0));
    fmt.Time_.assign(Kway::TKTime::Now().as_string(false) + "00");
    fmt.Msg_   .assign(msg);
    fmt.Msg2_  .assign(msg2);
    fmt.Remark_.assign(mark);
    Log_.Write(&fmt, sizeof(fmt));
  }
  Log_.Close();
}
//////////////////////////////////////////////////////////////////////////////
K_mf(void) TXFTLog::WriteLog (const std::string& msg, const std::string& msg2, const std::string& mark)
{
  if(FName_.empty())
    return;

  Log_.Open(FName_.c_str(), Kway::TFileMode(Kway::fmRandomAccess | Kway::fmOpenAlways | Kway::fmCreatePath));
  if(Log_.IsOpened())
  {
    Log_.Seek(0, Kway::fsEnd);
    FTLogFmt fmt;
    fmt.Date_.assign(Kway::TKDate::Now().as_string(Kway::TKDate::sf_ELong));
    fmt.Time_.assign(Kway::TKTime::Now().as_string());
    fmt.Name_.assign(msg);
    fmt.Reason_.assign(mark);
    Log_.Write(&fmt, sizeof(fmt));
  }
  Log_.Close();
}
//---------------------------------------------------------------------------

