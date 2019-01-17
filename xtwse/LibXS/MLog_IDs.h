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
   //�H��Y���W�[�Ш� SysLogMgr.txt �������U.
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
enum EMsgLevel {        //�i�ھ� MsgID �]�w,���ܬY MsgID �� MsgLevel.
   ml_Emergency = 0x10,   //�Ҧp:����u���_�u,�t�β��`...
   ml_Alert     = 0x20,   //
   ml_Critical  = 0x30,   //�Ҧp:�Y����w�e�X���\�[���^��...
   ml_Error     = 0x40,   //�q�`:����Ұʮɵo�Ϳ��~...
   ml_Warning   = 0x50,   //�q�`:����Ұʮɦ��ǰ��D�����i�B�@,���涥�q���ǰ��D�����i�B�@...
   ml_Notice    = 0x60,   //�Ҧp:���ܨt�ήɶ�...
   ml_Info      = 0x70,   //�Ҧp:���󪺫إ�,�Ѻc,���b�ǰe�Y�j�q�����...
   ml_Debug     = 0x7f,   //Debug�Ϊ��T��.

   ml_LogSystem = ml_Info,  //Log�t�Φ۰ʰO�����T��.
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
