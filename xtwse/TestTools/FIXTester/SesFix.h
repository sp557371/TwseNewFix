//---------------------------------------------------------------------------
#ifndef SesFixH
#define SesFixH
/////////////////////////////////////////////////////////////////////////////
#include <map>
#include <string>
#include "FixRecorder.h"
#include "TimerThread.h"
#include "Ses.h"
#include "FixEvHand.h"
#include "SesMgr.h"
#include <vector>
#include "Nsa_Time.h"
#include <stdlib.h>
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Fix  {
namespace Stk  {
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
enum ESessionRejectReason {
   srrInvalidTagNumber            = 0,
   srrRequiredTagMissing          = 1,
   srrTagNotDefinedForThisMessage = 2,
   srrUndefinedTag                = 3,
   srrTagSpecifiedWithoutAValue   = 4,
   srrValueIsIncorrect            = 5,
   srrIncorrectDataFormat         = 6,
   srrDecryptionProblem           = 7,
   srrSignatureProblem            = 8,
   srrCompIDProblem               = 9,
   srrSendingTimeAccuracyProblem  = 10,
   srrInvalidMsgType              = 11,
   srrXMLValidationError          = 12,
   srrTagAppearsMoreThenOnce      = 13,
   srrTagSpecifiedOutOfRequiredOrder = 14,
   srrRepeatingGroupFieldsOutOfOrder = 15,
   srrIncorrectNumInGroupCount       = 16,
};
//---------------------------------------------------------------------------
K_class TStkSesFix : public Kway::TSesBase
                   , private TFixMsgEv
                   , private TFixEv
{
   typedef Kway::TSesBase                     inherited;
   typedef Kstl::container_ev_handler         inherited_evs;
   typedef TStkSesFix                         this_type;
   typedef std::pair<TFixEvMask, TFixMsgEv*>  TRegEvPair;
   typedef std::map<std::string,TRegEvPair>   TRegEvs;
   typedef TRegEvs::iterator                	 TRegEvsI;
   typedef TRegEvs::value_type              	 TRegEvsV;
   typedef Kway::Nsa::TTimeStamp              TTimeStamp;

   K_mf(bool)  SetMsgHandler(const char* aMsgType, TFixEvMask evm, TFixMsgEv* aHandler);

   TRegEvs          _RegEvs;
   Kway::byte       _BeginStringLen;
   char             _BeginString[64];
   std::string      FIXVersion_;
   TStkFixRecorder* _FixRecorder;
   //char             String_[64];
   char             HeadCode;
   bool             _IsRecvChecking;
   int              _RPos;
   SFixStr          _RecvBuf;
   char*            _RPosLastCheck;
   long             _GiveupBytes;
   bool             _CloseOnTxEmpty;
   std::string      _TestID;
   bool             _IsLogonOK;
   // Get from SettingApply
   int              _HeartBtInt, _PWD;
   std::string      _SenderCompID, _TargetCompID, _BrokerID;
   TTimeStamp       _EndTime; //開始與結束時間
   bool             _Log;
   int              Counter_;

   virtual K_mf(void)  OnMesLinkReady(Kway::TMesBase*);
   virtual K_mf(void)  OnMesLinkFail(Kway::TMesBase*, const std::string& reason);
   virtual K_mf(void)  OnMesLinkBroken(Kway::TMesBase*, const std::string& reason);
   virtual K_mf(void)  OnMesRecvReady(Kway::TMesBase*, size_t /*size*/);// { CheckRecv(); }
   virtual K_mf(bool)  SettingApply(const std::string&);

   Kway::TTimer<this_type>  SenderTimer_;
   K_mf(void)  StartSenderTimer(size_t sec) { SenderTimer_.Start(sec*1000, &this_type::OnSenderTimer); }
   K_mf(void)  StopSenderTimer() { SenderTimer_.Stop(); }
   virtual K_mf(void)  OnSenderTimer(Kway::TTimerBase*);
   
   Kway::TTimer<this_type>  TargetTimer_;
   K_mf(void)  StartTargetTimer(size_t sec) { TargetTimer_.Start(sec*1000, &this_type::OnTargetTimer); }
   K_mf(void)  StopTargetTimer() { TargetTimer_.Stop(); }
   virtual K_mf(void)  OnTargetTimer (Kway::TTimerBase*);
   
   Kway::TTimer<this_type>  Timer_;
   K_mf(void)  StartTimer(size_t sec) { Timer_.Start(sec*1000, &this_type::OnTimer); }
   K_mf(void)  StopTimer() { Timer_.Stop(); }
   virtual K_mf(void)  OnTimer(Kway::TTimerBase*);
   
   Kway::TTimer<this_type>  LogonTimer_;
   K_mf(void)  StartLogonTimer(size_t sec) { LogonTimer_.Start(sec*1000, &this_type::OnLogonTimer); }
   K_mf(void)  StopLogonTimer() { LogonTimer_.Stop(); }
   virtual K_mf(void)  OnLogonTimer(Kway::TTimerBase*);

   virtual K_mf(void)  OnMesSendEmpty(Kway::TMesBase*);
   virtual K_mf(void)  CheckRecv();
   
   K_mf(int)  CheckBeginStr(const char* aMsgBegin, int aMsgLen);
   // virtual event
   K_mf(void)  DoEvent(const TFixMsg&);
   // Event Handler
   K_mf(void)  OnTestReq(TStkSesFix*, const TFixMsg&);
   K_mf(void)  OnHeartbeat(TStkSesFix*, const TFixMsg&);
   K_mf(std::string)  GetRawData();
   //K_mf(std::string) GetTradeAccount(std::string& acc);
   
protected:
   K_mf(bool)  SendBody(const SFixStr& aFixBody, int aMsgSeqNum, bool isResend);
   K_mf(void)  SendSampleFile();
   K_mf(bool)  CheckLast50(std::string& a);
   K_mf(std::string) GetAppendNo();

public:
   K_ctor  TStkSesFix(const char*);
   K_dtor  ~TStkSesFix();

   virtual K_mf(bool)  Send(const char* aMsgType, const TFixTagValue* aBodyBegin, const TFixTagValue* aBodyEnd, int aMsgSeqNum=-1);
   
   K_mf(void)  SendTestReq();
   K_mf(bool)  Send(const char* aMsgType, const TFixValues& aBodyValues) { return Send(aMsgType, &(*aBodyValues.begin()), &(*aBodyValues.end())); }
   K_mf(bool)  SendAnOrder(const char* aMsgType, std::string& str);
   K_mf(bool)  ResendRequest(int aBeginSeqNum, int aEndSeqNum);
   K_mf(bool)  SendReject(const TFixMsg&, const char* aReasonCode, const char* aText, TFixTag);
   K_mf(bool)  SendReject(const TFixMsg&, ESessionRejectReason, TFixTag);

   K_mf(char*) SkipAdminMessage(const SFixStr&);
   K_mf(bool)  Resend(const SFixStr&);
   K_mf(void)  SetCloseOnTxEmpty() { _CloseOnTxEmpty=true; }

   K_mf(bool)  RegMsgHandler(const char* aMsgType, TFixEvMask evm, TFixMsgEv* aMsgHandler);
   K_mf(bool)  DelMsgHandler(const char* aMsgType, TFixEvMask evm);
   K_mf(void)  SetHeartbeatInterval(int aSecs);
   K_mf(void)  SetLogonOK();
   K_mf(void)  SetLogoutOK();

   K_mf(std::string)  GetSesID()    { return _SenderCompID; }
   K_mf(std::string)  GetBrokerID() { return _BrokerID; }
   K_mf(std::string)	 GetTradeID()  { return _FixRecorder->GetTradeID(); }
   K_mf(TStkFixRecorder&)  GetRecorder() { return (*_FixRecorder); }
   K_mf(void)  DoRefresh();
   K_mf(void)  SendLogon();
   K_mf(void)  SendLogout();
   K_mf(void)  SendHeartBt();
   K_mf(void)  SendOrder(std::string& ord);
   K_mf(bool)  IsLogonOK() { return _IsLogonOK; }
   K_mf(bool)  GetWriteOrNot() { return _Log; }
};
/////////////////////////////////////////////////////////////////////////////
extern const char*  GetLocalDateTime();
extern const char*  GetUTCDateTime(bool datestr=true, bool timestr=true);
extern const char*  ConvertToUTCDateTime(const Kway::TKTime*, bool datestr=true, bool timestr=true);
extern std::string  GetFixDateTimeMs(bool utc=true, bool date=true, bool time=true, bool ms=true, bool log=false);
}; // Stk
}; // Fix
}; // Tw
}; // Kway
#endif

