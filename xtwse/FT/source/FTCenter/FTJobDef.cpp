//---------------------------------------------------------------------------
#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FTJobDef.h"
//---------------------------------------------------------------------------
using namespace Kway::Tw::Bx::FT;
//---------------------------------------------------------------------------
static char* FTFlowIDStr[] = {
    "處理中", "排隊中", "傳送中", "接收中", "等候回報", "己繞送處理", "完成", "委託中斷"
};
//---------------------------------------------------------------------------
K_mf(std::string) TFTFlowID::as_string() const
{
  if((unsigned)Data_ >= numofele(FTFlowIDStr))
    return std::string();
  return std::string(FTFlowIDStr[Data_]);
}
//---------------------------------------------------------------------------
K_mf(void) TFTFlowID::assign(const std::string& str)
{
  const char* cstr = str.c_str();
  for(int i=0; i<numofele(FTFlowIDStr); ++i)
    if(strcmp(FTFlowIDStr[i], cstr) == 0)
    {
      Data_ = (EFTFlowID)i;
      break;
    }
}
/////////////////////////////////////////////////////////////////////////////
static char* FTModeStr[] = {
    "主動下載", "申報/回覆", "券商要求", "定時排程", "券商通知", "交易所通知"
};
//---------------------------------------------------------------------------
K_mf(std::string) TEFTMode::as_string() const
{
  if((unsigned)Data_ >= numofele(FTModeStr))
    return std::string();
  return std::string(FTModeStr[Data_]);
}
//---------------------------------------------------------------------------
K_mf(void) TEFTMode::assign(const std::string& str)
{
  const char* cstr = str.c_str();
  for(int i=0; i<numofele(FTModeStr); ++i)
    if(strcmp(FTModeStr[i], cstr) == 0)
    {
      Data_ = (EFTMode)i;
      break;
    }
}
/////////////////////////////////////////////////////////////////////////////
static char* FTTypeStr[] = {
    "檔案", "訊息"
};
//---------------------------------------------------------------------------
K_mf(std::string) TEFTType::as_string() const
{
  if((unsigned)Data_ >= numofele(FTTypeStr))
    return std::string();
  return std::string(FTTypeStr[Data_]);
}
//---------------------------------------------------------------------------
K_mf(void) TEFTType::assign(const std::string& str)
{
  const char* cstr = str.c_str();
  for(int i=0; i<numofele(FTTypeStr); ++i)
    if(strcmp(FTTypeStr[i], cstr) == 0)
    {
      Data_ = (EFTType)i;
      break;
    }
}
//---------------------------------------------------------------------------


