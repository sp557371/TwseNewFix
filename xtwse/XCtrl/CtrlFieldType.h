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
   // �R�O���`����
   esc_Normal = 0,

   // �s�u�O��
   // TMP Handler�PAppia���s�u�O�ɡCTMP Handler��Appia��timeout�]�w��10��C
   esc_AppiaTimeout = 1,

   // Appia�A�Ȥ��s�b
   // Appia���ҰʡATMP Handler�L�k�إ߳s�u�C
   esc_AppiaNotConn = 2,

   // �ˬd�ç�TERM-NO
   // ��J���~��TERM-NO�C���BROKER-ID�BFIX SOCKET ID�BFIX SESSION TERM-NO��Ƥ��@�P�Τ��s�b�C
   esc_FixSesIDError = 3,
   
   // �ˬd�ç�Outbound�T���Ǹ�
   // ��FUNCTION-CODE=04�ANEW SEQUENCE NUMBER�����n���B����ơA�~��]�wOutbound�T���Ǹ��C
   esc_NewSeqNumError = 4,

   // �ˬd�ç󥿥\��X
   // ��J���䴩��FUNCTION-CODE�C
   esc_FuncCodeError = 5,

   // �ˬd�ç�MESSAGE-TYPE
   // ��J���~��MESSAGE-TYPE
   esc_MsgTypeError = 6,

   // TMP Handler�A�Ȥ��s�b
   // TANDEM�L�k�إ�socket�s�u��TMP Handler
   esc_TmpHandlerNotConn = 7,
   
   // �s�u�O��
   // TANDEM�PTMP Handler���s�u�O��
   esc_TmpHandlerTimeout = 8,

   // �o��AppiaGroup �S���]�w��Appia ���s�u 20130222 eric
   esc_NoAppiaConn = 9,

   // �䥦���~�T��
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
