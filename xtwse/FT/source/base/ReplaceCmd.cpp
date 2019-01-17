//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include <stdio.h>
#include "KTime.h"
#include "ReplaceCmd.h"
//---------------------------------------------------------------------------
using namespace Kway;
static char* DateCmdStr[] = {"[EY]", "[CY]", "[M]", "[D]", "[?]"};
//---------------------------------------------------------------------------
K_ctor TDateCmd::TDateCmd(std::string& cmd)
{
  Cmd_ = edc_None;
  for(int i=0;i<numofele(DateCmdStr);i++)
    if(cmd == DateCmdStr[i])
      Cmd_ = (EDateCmd)i;
}
//---------------------------------------------------------------------------
K_mf(std::string) TDateCmd::GetValue()
{
  char buf[32];
  TKDate date = TKDate::Now();
  switch(Cmd_)
  {
    case edc_EY:    sprintf(buf, "%04d", date.GetEYear());  break;
    case edc_CY:    sprintf(buf, "%02d", date.GetCYear());  break;
    case edc_M:     sprintf(buf, "%02d", date.GetMon());    break;
    case edc_D:     sprintf(buf, "%02d", date.GetDay());    break;
    default:        buf[0] = 0;                             break;
  }
  return std::string(buf);
}
//---------------------------------------------------------------------------
K_mf(std::string) TDateCmd::GetCmd  ()
{
  return DateCmdStr[(int)Cmd_];
}
/////////////////////////////////////////////////////////////////////////////
static char* TimeCmdStr[] = {"[hh]", "[12h]", "[mm]", "[ss]", "[?]"};
//---------------------------------------------------------------------------
K_ctor TTimeCmd::TTimeCmd(std::string& cmd)
{
  Cmd_ = etc_None;
  for(int i=0;i<numofele(TimeCmdStr);i++)
    if(cmd == TimeCmdStr[i])
      Cmd_ = (ETimeCmd)i;
}
//---------------------------------------------------------------------------
K_mf(std::string) TTimeCmd::GetValue()
{
  char buf[32];
  TKTime time = TKTime::Now();
  int hour = time.GetHour();
  switch(Cmd_)
  {
    case etc_hh:    sprintf(buf, "%02d", hour);             break;
    case etc_12h:   sprintf(buf, "%s%02d", hour >= 12 ? "PM":"AM", hour >= 12 ? hour-12:hour); break;
    case etc_mm:    sprintf(buf, "%02d", time.GetMin());    break;
    case etc_ss:    sprintf(buf, "%02d", time.GetSec());    break;
    default:        buf[0] = 0;                             break;
  }
  return std::string(buf);
}
//---------------------------------------------------------------------------
K_mf(std::string) TTimeCmd::GetCmd  ()
{
  return TimeCmdStr[(int)Cmd_];
}
/////////////////////////////////////////////////////////////////////////////
static char* StkCmdStr[] = {"[B]", "[BR]", "[MK]", "[ST]", "[?]"};
//---------------------------------------------------------------------------
K_ctor TStkCmd::TStkCmd(std::string& cmd)
{
  Cmd_ = esc_None;
  for(int i=0;i<numofele(StkCmdStr);i++)
    if(cmd == StkCmdStr[i])
      Cmd_ = (EStkCmd)i;
}
//---------------------------------------------------------------------------
K_mf(std::string) TStkCmd::GetValue()
{
  char buf[32];
  switch(Cmd_)
  {
    default:        buf[0] = 0;                             break;
  }
  return std::string(buf);
}
//---------------------------------------------------------------------------
K_mf(std::string) TStkCmd::GetCmd  ()
{
  return StkCmdStr[(int)Cmd_];
}
/////////////////////////////////////////////////////////////////////////////
static char SplitBegin = '[';
static char SplitEnd   = ']';
//---------------------------------------------------------------------------
K_ctor TReplaceCmd::TReplaceCmd(const std::string&)
{
}
//---------------------------------------------------------------------------
K_mf(void) TReplaceCmd::GetCmdStr(const char* cmd)
{
  int   spos=0, epos=0;
  char* p = const_cast<char*>(cmd);
  for(int i=0;i<(int)strlen(cmd);i++,p++)
  {
    if(*p == SplitBegin)
      spos = i;
    else if(*p == SplitEnd)
      epos = i;
    if(spos > 0 && epos > 0)
    {
      spos = epos = 0;
    }
  }
}
//---------------------------------------------------------------------------
K_mf(void) TReplaceCmd::GetCmdObj(std::string& str)
{
/*  static TDateCmd dcmd(str);
  static TTimeCmd tcmd(str);
  static TStkCmd  scmd(str);
  if(dcmd.IsEffect())
    Cmds_.push_back(new TDateCmd(str));
  else if(tcmd.IsEffect())
    Cmds_.push_back(new TTimeCmd(str));
  else if(scmd.IsEffect())
    Cmds_.push_back(new TStkCmd(str));
  else
    Cmds_.push_back(new TStrCmd(str));*/
}
//---------------------------------------------------------------------------


