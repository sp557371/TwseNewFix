#ifndef DspDefH
#define DspDefH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include "bxFT.h"
#include "FTJobDef.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
//---------------------------------------------------------------------------
K_class TFileDispatch
{
    typedef Kway::Tw::Stk::TMarket  TMarket;
public:
        K_ctor  TFileDispatch() {}
virtual K_dtor ~TFileDispatch() {}
virtual K_mf(void) DoDispatch(const TFTJob&, const std::string&) const = 0;
virtual K_mf(std::string) GetDLCmd(const TFTJob&, const std::string&) const {return std::string();}
};
//---------------------------------------------------------------------------
K_class TFileGet
{
    typedef Kway::Tw::Stk::TMarket  TMarket;
public:
        K_ctor  TFileGet() {}
virtual K_dtor ~TFileGet() {}
virtual K_mf(bool) GetSource (const TFTJob&, const std::string& src, const std::string& dst) const = 0;
virtual K_mf(std::string) GetULCmd (const TFTJob&, const std::string& src, const std::string& dst) const
                         {return std::string();}
// 20081002: 3T架構下要繞送的話, 一定有"本機暫存路徑"的設定, 否則必須return 空字串
virtual K_mf(std::string) Get3TLocalPath() const {return std::string();} 

};
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
//---------------------------------------------------------------------------
#endif


