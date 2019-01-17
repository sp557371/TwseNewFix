//---------------------------------------------------------------------------
#ifndef bxSesH
#define bxSesH
//---------------------------------------------------------------------------
#include <map>
#include "bxHeadDef.h"
#include "Ses.h"
#include "SesPacket.h"
#include "bxSesMgr.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
K_class TbxSesBase;
K_class TWorker;
//訊息處理者註冊表,以MsgCode當作陣列索引,查詢實際處理訊息的物件
typedef std::map<TWorkKey, TWorker*> TWorkerRegs;
struct SMsg {
   const static char ACode_LineOpened [3];
   const static char ACode_LineClosed [3];
   const static char ACode_APPReady   [3];
   const static char ACode_ApTimeout  [3];
   const static char ACode_HBTimeout  [3];
   const static char ACode_LinkTimeout[3];
};
//---------------------------------------------------------------------------
K_class TAlarmMsg
{
public:
   Kstl::char_ary<char, 8, ' '>   MsgTime_;
   Kstl::char_ary<char, 2, ' '>   Filler_;
   Kstl::char_ary<char, 2, ' '>   PvcID_;
   Kstl::char_ary<char,30, ' '>   LinkSet_;
   Kstl::char_ary<char, 2, ' '>   StatusCode_;
   Kstl::char_ary<char,40, ' '>   Memo_;
   Kstl::char_ary<char, 2, 0  >   NewLine_;
   K_ctor TAlarmMsg(const std::string& pvcid)
   {
      MsgTime_.assign(TKTime::Now().as_string());
      Filler_ .assign("  ");
      PvcID_  .assign(pvcid);
      NewLine_.assign("\n");
   }
   K_mf(void) SetMsg(const char* status, const char* set, const char* memo)
   {
      LinkSet_   .assign(set);
      StatusCode_.assign(status);
      Memo_      .assign(memo);
   }
};
//---------------------------------------------------------------------------
struct SStatusStr {
   const static char Processing;
   const static char Linked;
   const static char Retrying;
};
//---------------------------------------------------------------------------
K_class TPvcStatus
{
public:
   typedef Kstl::char_ary <char, 4, ' '> TPassWd;
   typedef Kstl::char_ary1<char, ','> TComma;
   typedef Kstl::char_ary1<char, ' '> TStatus;
   typedef Kstl::char_ary1<char, '\n'> TNewLine;

   TBrkID       Broker_;
   TComma       Comma1_;
   TPassWd      Passwd_;
   TComma       Comma2_;
   TPvcID       PvcID_;
   TComma       Comma3_;
   TApKey       ApCode_;
   TComma       Comma4_;
   TStatus      Status_;
   TNewLine     NewLine_;
   K_mf(void) SetStatus(const char status) { Status_ = status; }
};
//---------------------------------------------------------------------------
K_class TWorker
{
    friend K_class TbxSesBase;
    TWorkKey    WorkKey_;
protected:
virtual K_mf(int)       GetPacketSize(TbxSesBase&, const TbxData&) = 0;
virtual K_mf(bool)      APacket      (TbxSesBase&, const TbxData&) = 0;
virtual K_mf(int)       BeforeAPacket(TbxSesBase&, const TbxData&) = 0;
virtual K_mf(void)      AfterAPacket (TbxSesBase&, const TbxData&) = 0;

public:
        K_ctor TWorker  (TWorkerRegs* regs, TWorkKey key)
        {
          TWorkerRegs::iterator i = regs->find(key);
          if(i == regs->end())
            regs->insert(std::make_pair(key, this));
          else
            i->second = this;
        }
inline  K_ctor   TWorker  ()    {}
virtual K_dtor  ~TWorker  ()    {}
virtual K_mf(bool) DoReq  (TbxSesBase&, const TBxMsg, const TControlHead&) = 0;
        K_mf(void) SetWorkKey(TWorkKey key) { WorkKey_ = key; }
        K_mf(TWorkKey) GetWorkKey()         { return WorkKey_;}
static const std::string WorkerErrMsg;
};
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
K_class TWorkersBase
{
protected:
    //取出Worker註冊表
    virtual K_mf(TWorkerRegs*)  GetWorkerRegs    ()       = 0;
    //取出Worker註冊表的大小
    virtual K_mf(TBxSize)       GetWorkersCount  () const = 0;

public:
    virtual K_dtor ~TWorkersBase()   {}
    //取出某 MsgCode 的 Worker
    virtual K_mf(TWorker*) GetWorker(TWorkKey aKey)
    {
      TWorkerRegs* regs = GetWorkerRegs();
      TWorkerRegs::iterator i = regs->find(aKey);
      return (i == regs->end() ? 0:i->second);
    }
    //設定某 MsgCode 的 Worker, 傳回設定前的 Worker
    K_mf(TWorker*) SetWorker(TWorker* aNewWorker)
    {
      TWorkerRegs* regs = GetWorkerRegs();
      TWorkKey key = aNewWorker->GetWorkKey();
      TWorkerRegs::iterator i = regs->find(key);
      if(i == regs->end())
      {
        regs->insert(std::make_pair(key, aNewWorker));
        return 0;
      }
      else
      {
        TWorker* old = i->second;
        i->second = aNewWorker;
        return old;
      }
    }
};
/////////////////////////////////////////////////////////////////////////////
class TWorkers : public TWorkersBase
{
private:
    TWorkerRegs Workers_;
protected:
    K_mf(TWorkerRegs*)  GetWorkerRegs   ()       { return &Workers_; }
    K_mf(TBxSize)       GetWorkersCount () const { return (word)Workers_.size(); }
public:
    K_mf(TWorker*)      GetWorker       (TWorkKey aKey)
    {
      TWorkerRegs::iterator i = Workers_.find(aKey);
      return (i == Workers_.end() ? 0:i->second);
    }
};
/////////////////////////////////////////////////////////////////////////////
typedef TTSesPacket<TbxData, szMsgKey>   TbxSesPkt;
/////////////////////////////////////////////////////////////////////////////
K_class TbxSesBase : public TbxSesPkt
{
    typedef TbxSesPkt   inherited;
    typedef TbxSesBase  this_type;
protected:
    TWorkers        Workers_;
    TbxFTSesMgr*    SesMgr_;
public:
    TPvcStatus      Status_;
protected:

virtual K_mf(int)   GetPacketSize   (TbxData&);
virtual K_mf(int)   BeforeAPacket   (TbxData&);
        //↓APacket 傳回 true, 則會進行 AfterAPacket(),否則繼續處理下一個 packet
virtual K_mf(bool)  APacket         (TbxData&);
virtual K_mf(void)  AfterAPacket    (TbxData&) {}
virtual K_mf(void)  DoTimeOut       (TTimerBase* tm) { inherited::DoTimeOut(tm); }
public:
        K_ctor  TbxSesBase(TbxFTSesMgr&);
        K_dtor ~TbxSesBase() {}
        K_mf(bool) WriteLog (const char* data, size_t len, TPvcID pvcno, bool input=true, TBrkID brk=TBrkID())
        { if(SesMgr_)
            return SesMgr_->WriteLog(data, len, pvcno, input, brk);
          return false;
        }
        K_mf(TbxFTSesMgr*) GetSesMgr() {return SesMgr_;}
};
/////////////////////////////////////////////////////////////////////////////
};  // Bx
};  // Tw
};  // Kway
//---------------------------------------------------------------------------
#endif





