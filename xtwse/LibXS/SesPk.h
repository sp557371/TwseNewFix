//
// 通訊classes基礎: Session-Messager
//    class TSesPk; 一種基本的封包結構
//    << 以後應該可以改用 template 的方式設計 >>
//
// 版本: $Id: SesPk.h,v 1.5 2006/04/04 03:27:31 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SesPkH
#define SesPkH
//---------------------------------------------------------------------------
#ifdef _MSC_VER
#pragma warning ( disable : 4200)
#endif
//---------------------------------------------------------------------------
#include "Ses.h"
#include "HiLoTrans.h"
#include "TimerThread.h"
#include "Signon/Policy/PolicyBase.h"
//---------------------------------------------------------------------------
#include <stdio.h> //for sprintf()
//---------------------------------------------------------------------------
//
// 已使用的 MsgCode 請參考 "pkMsgCode.txt"
//
//---------------------------------------------------------------------------
namespace Kway {
namespace Pk {
/////////////////////////////////////////////////////////////////////////////
using Kway::Signon::Policy::TPolicyBaseRef;
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSesPk;
//---------------------------------------------------------------------------
//#include "PkHeartbeat.h"
class THeartbeat;
/////////////////////////////////////////////////////////////////////////////
LIBNS_fn(word)  SetPolicyNames  (char* buf, const char** policyNamesBegin, size_t policyNamesCount);
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
#ifdef _MSC_VER
#pragma warning ( disable : 4200 )
#endif
LIBNS_class TPolicyAck
{
   byte     PolicyNameLen_;
   word     PolicyDataLen_;
   char     Dat_[];
   //char   PolicyName_[];
   //byte   PolicyData_[];
public:
   //----- for server -----
   inline K_mf(word)  Set  (const std::string& policyName, const TPolicyBaseRef& ref)
         { memcpy(Dat_,                policyName.c_str(), PolicyNameLen_ = static_cast<byte>(policyName.length()));
           memcpy(Dat_+PolicyNameLen_, ref.first,          PolicyDataLen_ = ref.second);
           return GetSize(); }
   //----- for client -----
   inline K_mf(word)         GetSize       () const  { return static_cast<word>(sizeof(*this) + PolicyNameLen_ + PolicyDataLen_); }
   inline K_mf(std::string)  GetPolicyName () const  { return std::string(Dat_, PolicyNameLen_); }
   inline K_mf(const void*)  GetPolicyData () const  { return(Dat_ + PolicyNameLen_); }
   inline K_mf(word)         GetPolicyLen  () const  { return PolicyDataLen_; }
   inline K_mf(void)       TransPolicyLen  (THiLoTransRef ref)   { PolicyDataLen_ = HiLoTransT(PolicyDataLen_,ref); }
};
/////////////////////////////////////////////////////////////////////////////
struct TVer  //程式版本
{
   word  Major_;
   word  Minor_;
   word  Release_;
   word  Build_;

   inline K_ctor  TVer  (word ma=0, word mi=0, word r=0, word b=0)
            : Major_(ma), Minor_(mi), Release_(r), Build_(b)
            { }

   inline K_mf(bool) IsNull () const {
      return Major_==0 && Minor_==0 && Release_==0 && Build_==0;
   }

   inline K_mf(std::string)  as_string  () const
            { char   buf[128];
#if _MSC_VER >= 1400
			  sprintf_s(buf, sizeof(buf), "%d.%d.%d.%d", Major_, Minor_, Release_, Build_);
#else
			  sprintf(buf, "%d.%d.%d.%d", Major_, Minor_, Release_, Build_);
#endif
              return std::string(buf); }
};
/////////////////////////////////////////////////////////////////////////////
typedef word   TSize;   //包含 THead 的訊息長度
typedef word   TMsgCode;
//---------------------------------------------------------------------------
class THead
{  //封包大小1.5byte:0～0xfff(4095)
   //訊息代碼1.5byte
   typedef THead   this_type;
   union {
      byte     Header_[3];
      struct {
         byte  DatSz_LoByte_;
         byte  DatSz_HiByte_x0f;
      };
      struct {
         byte  MsgCode_NullByte_;
         byte  MsgCode_HiByte_xf0;
         byte  MsgCode_LoByte_;
      };
   };

public:
   inline K_mf(TSize)  GetPkSize () const       { return static_cast<TSize>(DatSz_LoByte_ | (((word)DatSz_HiByte_x0f & 0x0f) << 8)); }
   inline K_mf(void)   SetPkSize (TSize size)   { DatSz_LoByte_    = static_cast<byte>(size);
                                                  DatSz_HiByte_x0f = static_cast<byte>((size >> 8) & 0x0f)
                                                                   | static_cast<byte>(DatSz_HiByte_x0f & 0xf0); }

   inline K_mf(TMsgCode) GetMsgCode () const          { return(word(word(MsgCode_HiByte_xf0 & 0xf0)<<4)
                                                                  | word(MsgCode_LoByte_));      }
   inline K_mf(void)     SetMsgCode (TMsgCode code)   { MsgCode_HiByte_xf0 = byte((code & 0xf00) >> 4)
                                                                           | byte(MsgCode_HiByte_xf0 & 0x0f);
                                                        MsgCode_LoByte_ = byte(code);        }
   //資料從這兒開始
   //byte  Dat_[];
};
//---------------------------------------------------------------------------
struct TPkRec : public THead
{
   byte  Dat_[0x1000-sizeof(THead)];  //每筆資料最多 4k bytes
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
static const TSize      HeadSize = sizeof(THead);
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TWorker;                 //訊息處理者:實際處理訊息的物件
//---------------------------------------------------------------------------
typedef TWorker**  TWorkerRegs;  //訊息處理者註冊表,以MsgCode當作陣列索引,查詢實際處理訊息的物件
//---------------------------------------------------------------------------
LIBNS_class TWorker
{
public:
   inline  K_ctor   TWorker  (TWorkerRegs regs, TMsgCode mc)   { regs[mc] = this; }
   inline  K_ctor   TWorker  ()                                {}
   virtual K_dtor  ~TWorker  ()                                {}

   enum TCheckResult
   {
      cpr_HeadOK,    //此訊息OK,將訊息收完後,用 OnPkRecv() 通知,不需要再呼叫 OnPkCheck()
      cpr_Continue,  //此訊息目前無法判斷是否OK,當有收到新資料時再呼叫 OnPkCheck() 檢查一次
      cpr_Error,     //此訊息有問題,將已收到的資料清除
   };

   //檢查訊息是否OK, OnPkCheck()在呼叫 OnPkRecv()之前,至少會呼叫一次!
   //  如果您的訊息一定等於 pk.GetPkSize(), 則可以不必覆寫此 method
   //  此method主要是讓您可以檢查 pk.GetPkSize() 是否是您想要的,
   //  比如說:如果您期望的訊息大小為 10 bytes,可是 pk.GetPkSize() 傳回 23,
   //         表示此訊息無效, 應傳回 cpr_Error
   virtual K_mf(TCheckResult)  OnPkCheck  (TSesPk&, const THead& pk, TSize recvSize) = 0;

   //實際資料緊接在 THead 之後
   virtual K_mf(void)  OnPkRecv  (TSesPk&, const THead&) = 0;

   //提供一個簡單檢查訊息是否正確的機制:
   template <class TPk>
   static K_mf(TCheckResult)  PkCheck  (const TPk& pk, TSize recvSize)
            { if(pk.GetPkSize() < pk.GetMinSize()) // [訊息大小] 小於此訊息 [最小的期望值]
                 return cpr_Error;                 //    => cpr_Error
              if(recvSize < pk.GetMinSize())       // 此訊息已收到的資料,尚無法判斷 PkSize 是否合理
                 return cpr_Continue;              //    => cpr_Continue
              // 訊息大小(PkSize) 與 [期望的大小] 相同則為 cpr_HeadOK, 否則 cpr_Error
              return(pk.GetPkSize()==pk.GetExpectSize() ? cpr_HeadOK : cpr_Error); }
   template <class TPk>
   static K_mf(TCheckResult)  PkCheckHiLoRef  (const TPk& pk, TSize recvSize, THiLoTransRef hiLoRef)
            { if(pk.GetPkSize() < pk.GetMinSize())
                 return cpr_Error;
              if(recvSize < pk.GetMinSize())
                 return cpr_Continue;
              return(pk.GetPkSize()==pk.GetExpectSize(hiLoRef) ? cpr_HeadOK : cpr_Error); }
};
//---------------------------------------------------------------------------
LIBNS_class TWorkersBase
{
public:
   virtual K_dtor ~TWorkersBase  ()    {}

   //取出Worker註冊表
   virtual K_mf(TWorkerRegs)   GetWorkerRegs    () const = 0;
   //取出Worker註冊表的大小
   virtual K_mf(TMsgCode)      GetWorkersCount  () const = 0;
   //取出某 MsgCode 的 Worker
   virtual K_mf(TWorker*)      GetMsgWorker     (TMsgCode mc) const  { return(mc >= GetWorkersCount() ? 0 : GetWorkerRegs()[mc]); }
   //設定某 MsgCode 的 Worker, 傳回設定前的 Worker
   K_mf(TWorker*)              SetMsgWorker     (TMsgCode, TWorker* newWorker);
   //當某個Ses死亡時,呼叫此mf通知
   virtual K_mf(void) OnSesPkDestroy (TSesPk*) {
   }
};
//---------------------------------------------------------------------------
template <TMsgCode maxCount = 0x1000>
class TWorkers : public TWorkersBase
{
protected:
   TWorker* Workers_[maxCount];

public:
   K_ctor             TWorkers        ()                   { memset(Workers_, 0, sizeof(Workers_));    }
   K_mf(TWorkerRegs)  GetWorkerRegs   () const             { return const_cast<TWorkerRegs>(Workers_); }
   K_mf(TMsgCode)     GetWorkersCount () const             { return numofele(Workers_); }
   K_mf(TWorker*)     GetMsgWorker    (TMsgCode mc) const  { return mc >= numofele(Workers_) ? 0 : Workers_[mc]; }
};
/////////////////////////////////////////////////////////////////////////////
LIBNS_class  TSesPk : public TSesBase
                    , protected TWorker//專門處理未註冊的訊息
{
   typedef TSesBase  inherited;
   typedef TSesPk    this_type;
public:
   typedef TWorker::TCheckResult   TCheckResult;

   //----- 建構/解構
   K_ctor  TSesPk  (TWorkersBase&);
   K_dtor ~TSesPk  ();

   //----- 訊息處理/傳送
   virtual K_mf(bool)  SendPk       (const THead&);      //您實際要傳送的資料,必須緊接在 THead 之後
   virtual K_mf(void)  OnHeartbeat  (const THeartbeat&); //當收到Heartbeat()時

   //----- 狀態處理
   enum EState {
      sp_Creating,
      sp_Destroying,
      sp_Error,
      sp_WaitLinkReady,     //Session連線中
      sp_WaitConnected,     //連線要求傳送中,等Server回覆
      sp_WaitSignonInput,   //連線OK,等候使用者簽入
      sp_WaitSignonResult,  //登入要求傳送中,等Server回覆
      sp_Ready,             //已登入完成
      sp_Signout,           //系統登出
   };
          K_mf(void)    SetState  (EState newState, const std::string& reason);
   inline K_mf(EState)  GetState  () const   { return State_; }

protected:
   K_mf(void) OnBfDestroy ();
   //當收到沒有註冊的訊息時,會透過 OnPkCheck() 通知衍生出來的class
   //此處預設處理 THeartbeat 的訊息, 若為 THeartbeat, 則會呼叫 OnHeartbeat()
   virtual K_mf(TCheckResult)  OnPkCheck  (TSesPk&, const THead&, TSize recvSize);
   virtual K_mf(void)          OnPkRecv   (TSesPk&, const THead&);

   //啟動計時器,當您設定的時間到了,則會使用 OnTimer() 通知您
   K_mf(void)  StartTimer  (dword ms);
   //停止計時器,不再產生 OnTimer() 的事件
   K_mf(void)  StopTimer   ();

   K_mf(void)  OnMesLinkReady   (TMesBase*);// SetState(sp_WaitConnected)
   K_mf(void)  OnMesLinkBroken  (TMesBase*, const std::string&);//清除接收緩衝區
   K_mf(void)  OnMesLinkFail    (TMesBase*, const std::string&);//SetState(sp_Error);

private:
   //當訊息處理完畢時,會透過 OnPkWorkFinished() 來告知衍生出來的class
   virtual K_mf(void)  OnPkWorkFinished  (const THead&);//do nothing
   //若您有啟動計時器,則會透過 OnTimer() 來告知衍生出來的class
   virtual K_mf(void)  OnTimer           (TTimerBase*);//do nothing
   //當 Mes 有收到資料時,透過 OnMesRecvReady() 通知
   //  在此處會接收一個完整訊息,並呼叫 Worker 處理
   K_mf(void)  OnMesRecvReady    (TMesBase*, size_t size);//當有資料

   TWorkersBase*           Workers_;
   dword                   RecvPos_;   //已收到的資料大小
   TWorker::TCheckResult   PkResult_;
   dword                   SkipBytes_;
   TPkRec                  RecvPk_[2]; //RecvPk_[1]作保護用!
   TTimer<this_type>       Timer_;
   EState                  State_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Pk
} // namespace Kway
//---------------------------------------------------------------------------
#endif

