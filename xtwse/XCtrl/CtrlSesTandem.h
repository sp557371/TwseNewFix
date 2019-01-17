/////////////////////////////////////////////////////////////////////////////
#if !defined(_CtrlSesTandem_h_)
#define _CtrlSesTandem_h_
//---------------------------------------------------------------------------
#include <set>
#include "TimerThread.h"
#include "Nsa_LogFile.h"
#include "tplns/Nsa_PktSes.h"
#include "CtrlRsReqResu.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Nsa;

   class TCtrlSesMgr;

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// base type
/////////////////////////////////////////////////////////////////////////////
typedef char_ary<char, 8, '0'>  TTandSeqNum;
typedef char_ary<char, 4, '0'>  TTandRandom;

/////////////////////////////////////////////////////////////////////////////
// class TTandHdrCode
/////////////////////////////////////////////////////////////////////////////
class TTandHdrCode : public char_ary<char, 2, ' '>
{
   typedef char_ary<char, 2, ' '>  inherited;

public:
   inline K_ctor TTandHdrCode ()
      {
         char ch = 0xFE;
         
         inherited::set(0, ch);
         inherited::set(1, ch);
      }

   inline K_mf(bool) is_invalid () const
      {
         char ch = 0xFE;
         return (inherited::operator [] (0) == ch && inherited::operator [] (1) == ch);
      }
};

/////////////////////////////////////////////////////////////////////////////
// class TTotalRecord
/////////////////////////////////////////////////////////////////////////////
#define Sz_STandPkt_Body_Size  10
//---------------------------------------------------------------------------
class TTotalRecord : public char_ary<char, 2, '0'>
{
   typedef char_ary<char, 2, '0'>  inherited;

public:
   inline K_mf(int) as_int () const
      {
         int val = inherited::as_int();
         if (val > Sz_STandPkt_Body_Size)
            val = Sz_STandPkt_Body_Size;
         return val;
      }
};


/////////////////////////////////////////////////////////////////////////////
// struct STandPktHdr
/////////////////////////////////////////////////////////////////////////////
struct STandPktHdr
{
   TTandHdrCode   HdrCode_;
   TTandFuncCode  FuncCode_;
   TMsgType       MsgType_;
   TTandemTime    MsgTime_;   // 請輸入訊息傳出之時間(HHMMSS)
   TTandRandom    Random_;
};
/////////////////////////////////////////////////////////////////////////////
// struct STandPktReq
/////////////////////////////////////////////////////////////////////////////
struct STandPktReq
{
   struct SBody
   {
      TFixBrokerID  BrokerID_;
      TFixSocketID  SocketID_;
      TFixTermNo    TermNo_;
   };

   TTandBool     AllMarketFlag_;
   TTandSeqNum   NewSeqNum_;
   TTotalRecord  TotalRecord_;  // BODY的筆數(最多10筆)
   SBody         Body_[Sz_STandPkt_Body_Size];
};
/////////////////////////////////////////////////////////////////////////////
// struct STandPktResp
/////////////////////////////////////////////////////////////////////////////
struct STandPktResp
{
   struct SBody
   {
      TFixBrokerID  BrokerID_;
      TFixSocketID  SocketID_;
      TFixTermNo    TermNo_;
      TConnState    ConnState_;
      TTandBool     ServiceDownFlag_;
      TTandBool     SuspendFlag_;
      TTandBool     HoldFlag_;
      TTandSeqNum   InMsgSeqNum_;
      TTandemTime   LastInMsgTime_;  // 最後收到Inbound訊息的時間(HHMMSS)
      TTandSeqNum   OutMsgSeqNum_;
      TTandemTime   LastOutMsgTime_; // 最後送出Outbound訊息的時間(HHMMSS)
      TStatusCode   StatusCode1_;
      TStatusCode   StatusCode2_;
      TText         Text_;
   };

   TTandBool     AllMarketFlag_;
   TTotalRecord  TotalRecord_;  // BODY的筆數(最多10筆)
   SBody         Body_[Sz_STandPkt_Body_Size];

   inline K_mf(void) clear ()
      {
         SBody body;
         for (int i=0; i<Sz_STandPkt_Body_Size; ++i)
            Body_[i] = body;
      }
};
/////////////////////////////////////////////////////////////////////////////
// struct STandPkt
/////////////////////////////////////////////////////////////////////////////
struct STandPkt : public STandPktHdr
{
   typedef STandPktHdr  inherited;

   char  Buffer_[sizeof(STandPktResp)];

   inline K_ctor STandPkt ()
      {}

   inline K_ctor STandPkt (ETandFuncCode funcCode, EMsgType msgType, int randNum)
      {
         char buf[20];

         inherited::FuncCode_.assignT(funcCode);
         inherited::MsgType_ .assignT(msgType);
         inherited::MsgTime_ .Now    ();
            
         sprintf(buf, "%04d", randNum);
         inherited::Random_.assign(buf);
      }

   inline K_mf(STandPktReq*)  GetReq  ()  { return reinterpret_cast<STandPktReq*> (&Buffer_[0]); }
   inline K_mf(STandPktResp*) GetResp ()  { return reinterpret_cast<STandPktResp*>(&Buffer_[0]); }
};
/////////////////////////////////////////////////////////////////////////////
// packet length
/////////////////////////////////////////////////////////////////////////////
#define Pl_STandPktHdr       sizeof(STandPktHdr)
#define Pl_STandPktReq       sizeof(STandPktReq)
#define Pl_STandPktResp      sizeof(STandPktResp)
#define Pl_STandPktReq_All   (Pl_STandPktHdr + Pl_STandPktReq)
#define Pl_STandPktResp_All  (Pl_STandPktHdr + Pl_STandPktResp)

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif


/////////////////////////////////////////////////////////////////////////////
// class TCtrlSesTandem
/////////////////////////////////////////////////////////////////////////////
class TCtrlSesTandem : public TPacketSes<STandPkt, Pl_STandPktHdr>
{
   typedef TPacketSes<STandPkt, Pl_STandPktHdr>  inherited;
   typedef TCtrlSesTandem                        this_type;
   typedef TTimer<this_type>                     TTimerType;
   typedef TCtrlRsReqResu::TRsFixSesID           TRsFixSesID;
   typedef TCtrlRsReqResu::TRecsHnd              TRsReqResuHnd;
   // class TMyReqIDs
   typedef std::set<TReqID>                      TMyReqIDs;
   typedef TMyReqIDs::iterator                   TMyReqIDsI;

   TCtrlSesMgr*       SesMgr_;
   TCtrlRsReqResuGoh  RsReqResuGoh_;
   TReqID             ReqID_;
   TLogFile           LogFile_;
   TTimerType         Timer_;
   dword              Interval_;
   TMyReqIDs          MyReqIDs_;
   std::string        LinkIP_;

public:
   K_ctor  TCtrlSesTandem (TCtrlSesMgr*);
   K_dtor ~TCtrlSesTandem ();

protected:
   //==============================================
   // overwrite TSesBase virtual function
   K_mf(void) OnMesLinkReady  (TMesBase*);
   K_mf(void) OnMesLinkBroken (TMesBase*, const std::string&);
   K_mf(void) OnMesLinkFail   (TMesBase*, const std::string&);
   
   //==============================================
   // overwrite TPacketSes virtual function
   K_mf(int)  GetPacketSize (STandPkt*, bool isSend = false);
   K_mf(void) OnPktRecv     (STandPkt&, int pktSize);
   K_mf(void) OnSendPkt     (STandPkt&, int pktSize);

   //==============================================
   // TCtrlSesTandem function
   K_mf(void) AddReq   (STandPkt&);
   K_mf(bool) SendResp (const TReqID&, const SRsReqResu1&, TRsFixSesID&);
   K_mf(void) OnTimer  (TTimerBase*);
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_CtrlSesTandem_h_)
