/////////////////////////////////////////////////////////////////////////////
#if !defined(_QFixTagDef_h_)
#define _QFixTagDef_h_
//---------------------------------------------------------------------------
#include <string>
#include "QFixTagType.h"
//---------------------------------------------------------------------------
#define _FixTag_Twse_Ver2_
//#define _FixTag_Twse_Ver1_
//#define _FixTag_XFixGw_

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
namespace FIX  {

/////////////////////////////////////////////////////////////////////////////
// defined(_FixTag_Twse_Ver2_)
/////////////////////////////////////////////////////////////////////////////
#if defined(_FixTag_Twse_Ver2_)
   //==============================================
   // header
   typedef TFixCaString<10>  TFixSenderSubID;         // 050
   typedef TFixCaString< 3>  TFixTargetSubID;         // 057
   // 不使用的 tag
   // 115 OnBehalfOfCompID
   // 128 DeliverToCompID

   //==============================================
   // body
   typedef TFixCaString<10>  TFixAccount;       // 001
   typedef TFixCaString<30>  TFixClOrdID;       // 011
   typedef TFixCaString<20>  TFixExecID;        // 017
   typedef TFixCaString<10>  TFixOrderID;       // 037
   typedef TFixCaString<30>  TFixOrigClOrdID;   // 041
   typedef TFixCaString<10>  TFixSymbol;        // 055
   typedef TFixCaString<99>  TFixText;          // 058


/////////////////////////////////////////////////////////////////////////////
// defined(_FixTag_Twse_Ver1_)
/////////////////////////////////////////////////////////////////////////////
#elif defined(_FixTag_Twse_Ver1_)
   //==============================================
   // header
   typedef TFixStdString1  TFixSenderSubID;   // 050
   typedef TFixStdString1  TFixTargetSubID;   // 057
   // 不使用的 tag
   // 115 OnBehalfOfCompID
   // 128 DeliverToCompID

   //==============================================
   // body
   typedef TFixStdString1  TFixAccount;       // 001
   typedef TFixStdString1  TFixClOrdID;       // 011
   typedef TFixStdString1  TFixExecID;        // 017
   typedef TFixStdString1  TFixOrderID;       // 037
   typedef TFixStdString1  TFixOrigClOrdID;   // 041
   typedef TFixStdString1  TFixSymbol;        // 055
   typedef TFixStdString1  TFixText;          // 058

/////////////////////////////////////////////////////////////////////////////
// defined(_FixTag_XFixGw_)
/////////////////////////////////////////////////////////////////////////////
#elif defined(_FixTag_XFixGw_)
   //==============================================
   // header
   typedef TFixCaString< 20>  TFixOnBehalfOfCompID;   // 115
   typedef TFixCaString< 20>  TFixDeliverToCompID;    // 128

   //==============================================
   // body
   typedef TFixCaString< 30>  TFixAccount;      // 001
   typedef TFixCaString< 40>  TFixClOrdID;      // 011
   typedef TFixCaString<100>  TFixExecID;       // 017
   typedef TFixCaString< 50>  TFixOrderID;      // 037
   typedef TFixCaString< 40>  TFixOrigClOrdID;  // 041
   typedef TFixCaString< 30>  TFixSymbol;       // 055
   typedef TFixCaString<100>  TFixText;         // 058
   typedef TFixCaString< 50>  TFixRawData;      // 096

/////////////////////////////////////////////////////////////////////////////
// default
/////////////////////////////////////////////////////////////////////////////
#else
#endif

/////////////////////////////////////////////////////////////////////////////
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_QFixTagDef_h_)
