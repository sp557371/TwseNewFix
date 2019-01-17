//
// 使用者密碼管理政策.
//    class TUPPass;
//
// 版本: $Id: UPPass.h,v 1.2 2005/01/14 07:53:58 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef UPPassH
#define UPPassH
//---------------------------------------------------------------------------
//
// 政策名稱 = TUPPass::PolicyName
// 政策內容 = TUPPass
//
#include "Signon/libnsSignonPolicy.h"
#include "Signon/Policy/PolicyBase.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Signon {
namespace Policy {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
struct TUserPassPolicy
{
   byte  MaxErrCount_;     // 最多錯誤次數.
   word  ValidDays_;       // 有效天數,0表示用久有效.
   byte  MinLength_;       // 最少長度.
   byte  HistoryCount_;    // 舊密碼保留筆數.
   union {
      byte     Flags_;
      struct {
         byte  NeedAlpha_  : 1,  ///< 密碼必須包含: 字母.
               NeedNumber_ : 1,  ///< 密碼必須包含: 數字.
               NeedSymbol_ : 1,  ///< 密碼必須包含: 符號.
               DiffUserID_ : 1,  ///< 密碼必須與UserID不同.
               NeedUpper_  : 1,  ///< 密碼必須包含: 大寫字母.
               NeedLower_  : 1;  ///< 密碼必須包含: 小寫字母.
      };
   };
   byte  MaxCountFromSameIP_;  // [相同]來源可重複登入次數.(0=不限)
   byte  MaxCountFromDiffIP_;  // [不同]來源可重複登入次數.
                               // 雖然欄位名稱用IP,但不限於使用IP來源.

   byte  MinDiffChars_; ///< Require passwords to contain at least [n] different chars.
   byte  ChgHours_;     ///< 指定時間內,
   byte  MaxChgTimes_;  ///< 最多允許的異動次數.
   word  Durations_;    // 多少天未登入即不可登入,0表示用久有效.
   byte  Repeat_;       // 相同的字元最多連續出現多少次
   byte  IncDecRepeat_; // 遞增減字元最多次數

   inline K_ctor  TUserPassPolicy  ()
      : MaxErrCount_(3)
      , ValidDays_(90)
      , MinLength_(7)
      , HistoryCount_(6)
      , Flags_(0)
      , MaxCountFromSameIP_(0)
      , MaxCountFromDiffIP_(0)
      , MinDiffChars_(0)
      , ChgHours_(0)
      , MaxChgTimes_(0)
      , Durations_(0)
      , Repeat_(0)
      , IncDecRepeat_(0)
      { NeedAlpha_  = true;
        NeedUpper_  = true;
        NeedLower_  = true;
        NeedNumber_ = true;
        NeedSymbol_ = false;
        DiffUserID_ = true;
      }
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
SIGPO_class TUPPass : public TTRecPolicy<TUserPassPolicy>
{
   typedef TTRecPolicy<TUserPassPolicy>   inherited;
public:
   inline K_ctor      TUPPass  (const TPolicyBaseRef& ref)  : inherited(ref) {}
   static const char  PolicyName[];
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Policy
} // namespace Kway
} // namespace Signon
//---------------------------------------------------------------------------
#endif

