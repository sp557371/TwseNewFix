//---------------------------------------------------------------------------
#ifndef MLog_IDsH
#define MLog_IDsH
//---------------------------------------------------------------------------
#include "ev_map.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
/** class-id
*/
enum EModuleID {
   mid_libns      = 0x10,

   mid_etmpgw     = 0x20,
   mid_etmpex     = 0x21,
   mid_etmpblb    = 0x22,
   mid_etmptseca  = 0x23,
   mid_etmpapi    = 0x24,

   mid_pkftc      = 0x30,
   mid_nsfmerg    = 0x40,
   //以後若有增加請到 SysLogMgr.txt 之中註冊.
};
//---------------------------------------------------------------------------
enum EClassGroup {
   cgp_Kernel  = 0x10,
   cgp_Mes     = 0x20,
   cgp_Ses     = 0x30,
};
//---------------------------------------------------------------------------
LIBNS_class TClassID
{
   EModuleID      ModuleID_;
   EClassGroup    ClassGroup_;
   dword          SeqNo_;
public:
   inline K_ctor  TClassID  (EModuleID mid, EClassGroup cgp, dword seqNo)
      : ModuleID_(mid)
      , ClassGroup_(cgp)
      , SeqNo_(seqNo)
      { }
   inline K_ctor  TClassID  ()
      { }

   K_mf(std::string)  as_string  () const;
   K_mf(int)          makestr    (char*) const;
   inline K_mf(bool)  assign     (const std::string& str)   { return assign(str.c_str());  }
   inline K_mf(bool)  assign     (const char* cstr)         { return assign(&cstr); }
          K_mf(bool)  assign     (const char**);

   inline K_mf(bool)  operator== (const TClassID& a) const  { return ModuleID_   == a.ModuleID_
                                                                  && ClassGroup_ == a.ClassGroup_
                                                                  && SeqNo_      == a.SeqNo_; }
   inline K_mf(bool)  operator<  (const TClassID& a) const  { return ModuleID_   ==  a.ModuleID_
                                                                  ? (ClassGroup_ ==  a.ClassGroup_
                                                                     ? SeqNo_      < a.SeqNo_
                                                                     : ClassGroup_ < a.ClassGroup_)
                                                                  : ModuleID_      < a.ModuleID_; }
};
/////////////////////////////////////////////////////////////////////////////
/** message-id
*/
enum EMsgLevel {        //可根據 MsgID 設定,改變某 MsgID 的 MsgLevel.
   ml_Emergency = 0x10,   //例如:交易線路斷線,系統異常...
   ml_Alert     = 0x20,   //
   ml_Critical  = 0x30,   //例如:某交易已送出但許久未回應...
   ml_Error     = 0x40,   //通常:物件啟動時發生錯誤...
   ml_Warning   = 0x50,   //通常:物件啟動時有些問題但仍可運作,執行階段有些問題但仍可運作...
   ml_Notice    = 0x60,   //例如:改變系統時間...
   ml_Info      = 0x70,   //例如:物件的建立,解構,正在傳送某大量的資料...
   ml_Debug     = 0x7f,   //Debug用的訊息.

   ml_LogSystem = ml_Info,  //Log系統自動記錄的訊息.
};
LIBNS_fn(std::string)  GetMsgLevelStr  (EMsgLevel);
LIBNS_fn(const char*)  GetMsgLevelStr  (EMsgLevel, char*);

struct TMsgID {
   inline K_ctor  TMsgID  (EMsgLevel ml, dword msgid)
      : MsgLevel_(ml)
      , MsgID_(msgid)
      { }

   EMsgLevel   MsgLevel_;
   dword       MsgID_;
};
struct TDefaultMsg
{
   dword       MsgID_;
   const char* DefaultMsg_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
