//---------------------------------------------------------------------------
#ifndef BxLogerH
#define BxLogerH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include "char_ary.h"
#include <string>
#include "KFile.h"
//---------------------------------------------------------------------------
namespace Kway  {
namespace Tw    {
namespace Bx    {
//---------------------------------------------------------------------------
K_class TBxLogBase
{
protected:
    Kway::TFile Log_;
    std::string FName_;
public:
        K_ctor TBxLogBase()  {}
virtual K_dtor ~TBxLogBase() {}
        K_mf(void) SetFileName(const std::string& fname) { FName_ = fname; }
virtual K_mf(void) WriteLog   (const std::string& msg, const std::string& msg2, const std::string& mark) = 0;
};
//---------------------------------------------------------------------------
K_class TKOESLog : public TBxLogBase
{
  struct KoesLogFmt
  {
    typedef Kstl::char_ary<char, 8, ' '>  TChar8;
    typedef Kstl::char_ary<char, 1, ' '>  TFiller;
    typedef Kstl::char_ary<char,10, ' '>  TMsg;
    typedef Kstl::char_ary<char,20, ' '>  TMsg2;
    typedef Kstl::char_ary<char,30, ' '>  TRemark;
    typedef Kstl::char_ary<char, 1,'\n'>  TNewLine;

    TChar8    Date_;      // LogDate
    TFiller   Filler1_;
    TChar8    Time_;      // LogTime
    TFiller   Filler2_;
    TMsg      Msg_;       // Single Message
    TFiller   Filler3_;
    TMsg2     Msg2_;      // 2nd Message
    TFiller   Filler4_;
    TRemark   Remark_;    // Remark
    TNewLine  NewLine_;
  };
public:
        K_ctor TKOESLog() {}
virtual K_dtor ~TKOESLog(){}
virtual K_mf(void) WriteLog (const std::string& msg, const std::string& msg2, const std::string& mark);
};
//---------------------------------------------------------------------------
K_class TXFTLog : public TBxLogBase
{
  struct FTLogFmt
  {
    typedef Kstl::char_ary<char,10, ' '>  TDate;
    typedef Kstl::char_ary<char, 8, ' '>  TTime;
    typedef Kstl::char_ary<char,20, ' '>  TTagName;
    typedef Kstl::char_ary<char,40, ' '>  TReason;
    typedef Kstl::char_ary<char, 1, ' '>  TFiller;
    typedef Kstl::char_ary<char, 1, '\n'> TNewLine;

    TDate     Date_;        // LogDate
    TFiller   Filler1_;
    TTime     Time_;        // LogTime
    TFiller   Filler2_;
    TTagName  Name_;        // Tag Name
    TFiller   Filler3_;
    TReason   Reason_;      // Reason
    TNewLine  NewLine_;     // NewLine '\n'
  };
public:
        K_ctor TXFTLog() {}
virtual K_dtor ~TXFTLog(){}
virtual K_mf(void) WriteLog (const std::string& msg, const std::string& msg2, const std::string& mark);
};
//---------------------------------------------------------------------------
}   // Bx
}   // Tw
}   // Kway
//---------------------------------------------------------------------------
#endif

