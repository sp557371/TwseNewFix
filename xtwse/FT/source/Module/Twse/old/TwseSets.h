//---------------------------------------------------------------------------
#ifndef TwseSetsH
#define TwseSetsH
//---------------------------------------------------------------------------
#include "TwStk.h"
#include "FTJobDef.h"
#include "SetsVAC.h"
//---------------------------------------------------------------------------
namespace Kway  {
namespace Tw    {
namespace Bx    {
//---------------------------------------------------------------------------
enum TTwseSettingItem
{
  dsi_K02Path = 1,
  dsi_EOF,
};
//---------------------------------------------------------------------------
K_class TTwseSetting : public TSettingBase
{
  typedef TSettingBase              inherited;
  typedef TTwseSetting		         this_type;
  typedef Kway::Tw::Stk::TMarket    TMarket;
protected:
  virtual K_mf(void) AssignItemStr(const char**&, const char**&, size_t&);
public:
  K_ctor     TTwseSetting();
  K_dtor    ~TTwseSetting();
  K_mf(std::string) GetSetting (TTwseSettingItem) const;
  K_mf(bool) GetDLCmd   (const std::string&, const std::string&, std::string&, std::string&) const;
  K_mf(bool) GetULCmd(const std::string&, const std::string&, std::string&, std::string&) const;
};
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
};  // BX
};  // Tw
};  // Kway
//---------------------------------------------------------------------------
#endif

