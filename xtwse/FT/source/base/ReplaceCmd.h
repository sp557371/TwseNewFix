//---------------------------------------------------------------------------
#ifndef ReplaceCmdH
#define ReplaceCmdH
//---------------------------------------------------------------------------
#include <string>
#include <vector>
#include "KwayBase.h"
namespace Kway  {
//---------------------------------------------------------------------------
K_class TCmd
{
protected:
    virtual K_mf(std::string) GetValue() = 0;
    virtual K_mf(std::string) GetCmd  () = 0;
};
//---------------------------------------------------------------------------
K_class TStrCmd : public TCmd
{
    std::string Cmd_;
public:
        K_ctor TStrCmd(std::string& str) : Cmd_(str) {}
virtual K_dtor ~TStrCmd() {}
    K_mf(std::string) GetValue() { return Cmd_;  }
    K_mf(std::string) GetCmd  () { return "[N]"; }
};
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   EDateCmd;
   enum EDateCmd_t
#else
   enum EDateCmd
#endif
{
  edc_EY,    // 西元年
  edc_CY,    // 中國年
  edc_M,     // Month
  edc_D,     // Day
  edc_None,  // none
} K_EnumPacked;
K_class TDateCmd : public TCmd
{
    EDateCmd    Cmd_;
public:
        K_ctor TDateCmd(EDateCmd cmd) : Cmd_(cmd) {}
        K_ctor TDateCmd(std::string&);
virtual K_dtor ~TDateCmd() {}
    K_mf(std::string) GetValue();
    K_mf(std::string) GetCmd  ();
    K_mf(bool)        IsEffect() { return Cmd_!=edc_None; }
};
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   ETimeCmd;
   enum ETimeCmd_t
#else
   enum ETimeCmd
#endif
{
  etc_hh,   // 時(24小時制)
  etc_12h,  // 時(12小時制)
  etc_mm,   // 分
  etc_ss,   // 秒
  etc_None,  // none
} K_EnumPacked;
K_class TTimeCmd : public TCmd
{
    ETimeCmd    Cmd_;
public:
        K_ctor TTimeCmd(ETimeCmd cmd) : Cmd_(cmd) {}
        K_ctor TTimeCmd(std::string&);
virtual K_dtor ~TTimeCmd() {}
    K_mf(std::string) GetValue();
    K_mf(std::string) GetCmd  ();
    K_mf(bool)        IsEffect() { return Cmd_!=etc_None; }
};
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   EStkCmd;
   enum EStkCmd_t
#else
   enum EStkCmd
#endif
{
  esc_Brk,      // 券商代號
  esc_Branch,   // 分公司碼
  esc_Market,   // 市場別
  esc_Stk,      // 股票代號
  esc_None,     // none
} K_EnumPacked;
K_class TStkCmd : public TCmd
{
    EStkCmd     Cmd_;
public:
        K_ctor TStkCmd(EStkCmd cmd) : Cmd_(cmd) {}
        K_ctor TStkCmd(std::string&);
virtual K_dtor ~TStkCmd() {}
    K_mf(std::string) GetValue();
    K_mf(std::string) GetCmd  ();
    K_mf(bool)        IsEffect() { return Cmd_!=esc_None; }
};
//---------------------------------------------------------------------------
K_class TReplaceCmd
{
    typedef TReplaceCmd this_type;
    typedef std::vector<TCmd*>  TCmdVec;
    typedef TCmdVec::iterator   TCmdVecI;
    TCmdVec Cmds_;
protected:
    K_mf(void) GetCmdStr(const char*);
    K_mf(void) GetCmdObj(std::string&);
public:
    K_ctor TReplaceCmd(const std::string& cmd);
    K_dtor ~TReplaceCmd() {}
};
//---------------------------------------------------------------------------
};  // Kway
#endif
 