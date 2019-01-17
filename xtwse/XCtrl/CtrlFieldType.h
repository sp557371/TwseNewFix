/////////////////////////////////////////////////////////////////////////////
#if !defined(_CtrlFieldType_h_)
#define _CtrlFieldType_h_
//---------------------------------------------------------------------------
#include "char_ary.h"
#include "KTime.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Kstl;

/////////////////////////////////////////////////////////////////////////////
// base type
/////////////////////////////////////////////////////////////////////////////
typedef char_ary <char, 80, ' '>  TText;
typedef fixed_num<int>            TSeqNum;
//---------------------------------------------------------------------------
typedef char_ary <char, 10, ' '>  TFixBrokerID;
typedef char_ary <char,  2, ' '>  TFixSocketID;
typedef char_ary <char, 14, ' '>  TFixTermNo;

/////////////////////////////////////////////////////////////////////////////
// class TTandBool
/////////////////////////////////////////////////////////////////////////////
class TTandBool : public char_ary1<char, ' '>
{
   typedef char_ary1<char, ' '>  inherited;

public:
   inline K_mf(void) assignT (bool b)
      {
         if (b)  inherited::assign('Y');
         else    inherited::assign('N');
      }

   inline K_mf(bool) is_true () const
      {
         if (inherited::data_[0] == 'Y')
            return true;
         return false;
      }
};

/////////////////////////////////////////////////////////////////////////////
// class TTandFuncCode
/////////////////////////////////////////////////////////////////////////////
enum ETandFuncCode
{
   tfc_None,
      tfc_StatusQuery          =  1,
      tfc_Suspend              =  2,
      tfc_SuspendWithTerminate =  3,
      tfc_Resume               =  4,
      tfc_ServiceDown          =  5,
      tfc_ServiceUp            =  6,
      tfc_Hold                 =  7,
      tfc_Release              =  8,
      tfc_SequenceReset        =  9,
      tfc_Heartbeat            = 10,
   tfc_Total
};
//---------------------------------------------------------------------------
class TTandFuncCode : public char_ary<char, 2, '0'>
{
   typedef char_ary<char, 2, '0'>  inherited;
   
public:
   K_mf(std::string) as_desc () const;

   inline K_mf(void) assignT (ETandFuncCode e)
      { inherited::assign((size_t)e); }
      
   inline K_mf(ETandFuncCode) get_orig () const
      { return (ETandFuncCode)inherited::as_int(); }

   inline K_mf(bool) is_invalid () const
      {
         ETandFuncCode funcCode = get_orig();
         if (funcCode > tfc_None && funcCode < tfc_Total)
            return false;
         return true;
      }         
};

/////////////////////////////////////////////////////////////////////////////
// class TMsgType
/////////////////////////////////////////////////////////////////////////////
enum EMsgType
{
   emt_Request     = 1,
   emt_Acknowledge = 2,
   emt_Reject      = 3,
};
//---------------------------------------------------------------------------
class TMsgType : public char_ary<char, 2, '0'>
{
   typedef char_ary<char, 2, '0'>  inherited;
   
public:
   inline K_mf(void) assignT (EMsgType e)  { inherited::assign((size_t)e); }

   inline K_mf(EMsgType) get_orig () const
      { return (EMsgType)inherited::as_int(); }
};

/////////////////////////////////////////////////////////////////////////////
// class TTandemTime (HHMMSSsss)
/////////////////////////////////////////////////////////////////////////////
class TTandemTime : public char_ary<char, 9, '0'>
{
   typedef char_ary<char, 9, '0'>  inherited;

public:
   K_mf(void) Now    ();
   K_mf(void) assign (const TKDateTimeMS&);

   K_mf(std::string) as_string () const;
   
   inline K_mf(std::string) as_orig () const  { return std::string(inherited::begin(), 9); }
};

/////////////////////////////////////////////////////////////////////////////
// class TConnState
/////////////////////////////////////////////////////////////////////////////
enum EConnState
{
   ecs_Disconnected            = 0,
   ecs_SocketConnection        = 1,
   ecs_SentLogonConnection     = 2,
   ecs_ReceivedLogonConnection = 3,
   ecs_ApplicationConnection   = 4,
};
//---------------------------------------------------------------------------
class TConnState : public char_ary<char, 1, ' '>
{
   typedef char_ary<char, 1, ' '>  inherited;

public:
   inline K_mf(void) assignT (EConnState e)
      {
         char ch = '0' + (byte)e;
         inherited::set(0, ch);
      }
};

/////////////////////////////////////////////////////////////////////////////
// class TStatusCode
/////////////////////////////////////////////////////////////////////////////
enum EStatusCode
{
   // 命令正常執行
   esc_Normal = 0,

   // 連線逾時
   // TMP Handler與Appia間連線逾時。TMP Handler對Appia的timeout設定為10秒。
   esc_AppiaTimeout = 1,

   // Appia服務不存在
   // Appia未啟動，TMP Handler無法建立連線。
   esc_AppiaNotConn = 2,

   // 檢查並更正TERM-NO
   // 輸入錯誤的TERM-NO。表示BROKER-ID、FIX SOCKET ID、FIX SESSION TERM-NO資料不一致或不存在。
   esc_FixSesIDError = 3,
   
   // 檢查並更正Outbound訊息序號
   // 當FUNCTION-CODE=04，NEW SEQUENCE NUMBER為必要欄位且為整數，才能設定Outbound訊息序號。
   esc_NewSeqNumError = 4,

   // 檢查並更正功能碼
   // 輸入不支援的FUNCTION-CODE。
   esc_FuncCodeError = 5,

   // 檢查並更正MESSAGE-TYPE
   // 輸入錯誤的MESSAGE-TYPE
   esc_MsgTypeError = 6,

   // TMP Handler服務不存在
   // TANDEM無法建立socket連線到TMP Handler
   esc_TmpHandlerNotConn = 7,
   
   // 連線逾時
   // TANDEM與TMP Handler間連線逾時
   esc_TmpHandlerTimeout = 8,

   // 這個AppiaGroup 沒有設定對Appia 的連線 20130222 eric
   esc_NoAppiaConn = 9,

   // 其它錯誤訊息
   esc_OtherError = 99,
};
//---------------------------------------------------------------------------
class TStatusCode : public char_ary<char, 4, ' '>
{
   typedef char_ary<char, 4, ' '>  inherited;

public:
   inline K_mf(void) assignT (EStatusCode e)
      {
         char buf[10];
         sprintf(buf, "%04d", e);
         inherited::assign(buf);
      }

   inline K_mf(EStatusCode) get_orig () const
      { return (EStatusCode)inherited::as_int(); }
};

/////////////////////////////////////////////////////////////////////////////
// class TFixSesID
/////////////////////////////////////////////////////////////////////////////
class TFixSesID
{
   TFixBrokerID  BrokerID_;
   TFixSocketID  SocketID_;

public:
   inline K_ctor TFixSesID ()
      {}
   
   inline K_ctor TFixSesID (const TFixBrokerID& p1, const TFixSocketID& p2)
      : BrokerID_(p1)
      , SocketID_(p2)
      {}

   inline K_ctor TFixSesID (const std::string& str, bool isFull = false)
      {
         if (isFull)  assign_full(str);
         else         assign     (str);
      }

   K_mf(void) assign      (const char*);
   K_mf(void) assign_full (const char*);
   
   // 140001
   K_mf(std::string) as_string () const;
   // T140001
   K_mf(std::string) as_full () const;

   inline K_mf(void) assign      (const std::string& str)  { assign     (str.c_str()); }
   inline K_mf(void) assign_full (const std::string& str)  { assign_full(str.c_str()); }
      
   inline K_mf(const TFixBrokerID&) GetBrokerID () const  { return BrokerID_; }
   inline K_mf(const TFixSocketID&) GetSocketID () const  { return SocketID_; }

   inline K_mf(bool) operator < (const TFixSesID& a) const
      {
         if (BrokerID_ < a.BrokerID_)
            return true;
         else if (BrokerID_ == a.BrokerID_) {
            if (SocketID_ < a.SocketID_)
               return true;
         }
         return false;
      }

   inline K_mf(bool) operator == (const TFixSesID& a) const
      {
         return (BrokerID_ == a.BrokerID_ && SocketID_ == a.SocketID_);
      }
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_CtrlFieldType_h_)
