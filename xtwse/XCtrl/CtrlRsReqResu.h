/////////////////////////////////////////////////////////////////////////////
#if !defined(_CtrlRsReqResu_h_)
#define _CtrlRsReqResu_h_
//---------------------------------------------------------------------------
#include "fixed_num.h"
#include "kutility.h"
#include "Nsa_Time.h"
#include "Nsa_GlobObjs.h"
#include "tplns/Nsa_EvMap_2k.h"
#include "tplns/Nsa_EvMap_2k_Hnd.h"
#include "CtrlFieldType.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   class TVACreater;
   using namespace Kstl;
   using namespace Nsa;

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// base type
/////////////////////////////////////////////////////////////////////////////
typedef fixed_num<size_t>  TRandNum;

/////////////////////////////////////////////////////////////////////////////
// struct TReqID
/////////////////////////////////////////////////////////////////////////////
struct TReqID
{
   TTimeStamp  Time_;
   TRandNum    RandNum_;
   TSeqNum     SeqNum_;

   K_mf(void) assign (const char*);

   K_mf(std::string) as_string () const;

   inline K_mf(void) assign (const std::string& str)  { assign(str.c_str()); }
   inline K_mf(void) clear  ()                        { Time_.Clear();       }

   inline K_mf(bool) is_invalid () const  { return Time_.IsInvalid(); }
   
   K_mf(bool) operator <  (const TReqID&) const;
   K_mf(bool) operator == (const TReqID&) const;
};


/////////////////////////////////////////////////////////////////////////////
// class TProcResu
/////////////////////////////////////////////////////////////////////////////
enum EProcResu
{
   epr_LinUp,              // 排隊中
   epr_Process,            // 處理中
   epr_ApaSent,            // Appia 的部分已全部傳送了
   epr_Finish,             // 已完成
   epr_Reject,             // Tandem Request 欄位資料錯誤
   epr_TandemLinkBroken,   // Tandem 連線中斷
   epr_AppiaLinkTimeout,   // 與 Appia 連線逾時
   epr_Total
};
//---------------------------------------------------------------------------
class TProcResu : public EnumT<EProcResu>
{
   typedef EnumT<EProcResu>  inherited;
   
public:
   inline K_ctor TProcResu (EProcResu e = epr_LinUp) : inherited(e)  {}

   K_mf(std::string) as_desc () const;

   inline K_mf(EProcResu) get_orig () const  { return inherited::Data_; }
};


/////////////////////////////////////////////////////////////////////////////
// class TApaSentList
/////////////////////////////////////////////////////////////////////////////
enum EApaSentList
{
   asl_Cmd,
   asl_Query,
   asl_Total
};
//---------------------------------------------------------------------------
class TApaSentList : public std::bitset<asl_Total>
{
   typedef std::bitset<asl_Total>  inherited;
   
public:
   K_mf(std::string) as_string () const;
};

/////////////////////////////////////////////////////////////////////////////
// struct SRsReqResu1
/////////////////////////////////////////////////////////////////////////////
struct SRsReqResu1
{
   TCommInn::rr_key mutable  RoomKey_;

   TTimeStamp     RecvTime_;
   TTimeStamp     SentTime_;
   TProcResu      ProcResu_;
   TTandFuncCode  FuncCode_;
   TMsgType       MsgType_;
   TTandemTime    MsgTime_;
   TTandBool      AllMarketFlag_;
   TSeqNum        NewSeqNum_;

   Nsa_EvMap_Default_Operator_Equal(SRsReqResu1)
};

/////////////////////////////////////////////////////////////////////////////
// struct SRsReqResu2
/////////////////////////////////////////////////////////////////////////////
struct SRsReqResu2
{
   TCommInn::rr_key mutable  RoomKey_;
   
   TApaSentList  ApaSentList_;
   TFixTermNo    TermNo_;
   TConnState    ConnState_;
   TTandBool     ServiceDownFlag_;
   TTandBool     SuspendFlag_;
   TTandBool     HoldFlag_;
   TSeqNum       InMsgSeqNum_;
   TTandemTime   LastInMsgTime_;
   TSeqNum       OutMsgSeqNum_;
   TTandemTime   LastOutMsgTime_;
   TStatusCode   StatusCode1_;
   TStatusCode   StatusCode2_;
   TText         Text_;
   TSeqNum       ReqIdx_; // 接收 Request 時的順序
                          // 因為第二層是使用 TFixSesID 當作 Key
                          // 但是在回覆 Tandem 時要按照原始順序回覆
                          // 所以這個欄位紀錄最初接收到的順序
   char_ary<char,5,0> AppiaGrp_;
   K_mf(bool) IsSentOk (ETandFuncCode) const;
   K_mf(bool) IsFinish (ETandFuncCode) const;

   Nsa_EvMap_Default_Operator_Equal(SRsReqResu2)
};

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif


/////////////////////////////////////////////////////////////////////////////
// class TCtrlRsReqResu
/////////////////////////////////////////////////////////////////////////////
class TCtrlRsReqResu : public TGlobObj
{
public:
   //==============================================
   // TRecsType
   typedef TEvMap2k<TReqID, TFixSesID, SRsReqResu2, SRsReqResu1>  TRecsType;
   typedef TRecsType::TMap1  TRsReqID;
   typedef TRecsType::TMap2  TRsFixSesID;

   //==============================================
   // TRecsHnd
   class TRecsHnd : public TEvMap2kHnd<TRsReqID>
   {
      typedef TEvMap2kHnd<TRsReqID>  inherited;
      
   public:
      inline K_ctor TRecsHnd (TCtrlRsReqResu* o)
         : inherited(&(o->RsReqID_))
      {}
      
      inline K_ctor TRecsHnd (TCtrlRsReqResu* o, const TReqID& key1)
         : inherited(&(o->RsReqID_), key1)
      {}
      
      inline K_ctor TRecsHnd (TCtrlRsReqResu* o, const TReqID& key1, const TFixSesID& key2)
         : inherited(&(o->RsReqID_), key1, key2)
      {}
   };
   friend class TRecsHnd;

private:
   class TRecsRecorder;
   class TRecsVac;

   TRsReqID        RsReqID_;
   TRecsRecorder*  RecsRecorder_;
   TRecsVac*       RecsVac_;

public:
   K_ctor  TCtrlRsReqResu ();
   K_dtor ~TCtrlRsReqResu ();
   
   K_mf(void) AddReq     (TReqID&, const SRsReqResu1&, TRsFixSesID&);
   K_mf(bool) FindNewReq (TReqID&);

   K_mf(TVACreater&) GetVac ();
};

/////////////////////////////////////////////////////////////////////////////
// class TCtrlRsReqResuGoh
/////////////////////////////////////////////////////////////////////////////
class TCtrlRsReqResuGoh : public TGlobObjHnd<TCtrlRsReqResu>
{
   typedef TGlobObjHnd<TCtrlRsReqResu>  inherited;
   
public:
   inline K_ctor TCtrlRsReqResuGoh (bool autoQuery = false)
      : inherited("Kway::TCtrlRsReqResu", autoQuery)
   {}
};

/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_CtrlRsReqResu_h_)
