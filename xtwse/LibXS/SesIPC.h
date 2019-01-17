//---------------------------------------------------------------------------
#ifndef SesIPCH
#define SesIPCH
//---------------------------------------------------------------------------
#include "Ses.h"
#include "TimerThread.h"
#include "pod_vector.h"
#include <deque>
#include <vector>
#include <memory>
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSesIPC;
LIBNS_class TSesIPCMgr;
LIBNS_class TSesIPCReqHandler;
//---------------------------------------------------------------------------
enum TSesIPCResultCode {
   ipcr_PromptStr,
   ipcr_CmdTimeout,
   ipcr_LinkBroken,
   ipcr_SesDestroy,
   ipcr_MgrDestroy,
};
//---------------------------------------------------------------------------
enum TIPCReqReturn {
   irr_End,                   ///< 此要求已處理完畢.
   irr_ContinueRecv    = 0x01,///< 命令未完成,繼續接收.
   irr_ExceptionResult = 0x02,///< 此訊息異常,不是此要求期望的訊息.
   irr_ClearBuffer     = 0x04,///< 清除接收緩衝區.
   irr_SendAgain       = 0x10 + irr_ContinueRecv,///< 再送一次命令(req可能已改變Cmd_),適用不可分割要求(例如: asc + cd + get).
   irr_ReqKeepAlive    = 0x80,
};
//---------------------------------------------------------------------------
typedef std::vector<std::string> TEndingStrs;
//---------------------------------------------------------------------------
LIBNS_class TSesIPCReq
{
   std::string          Cmd_;
   TSesIPCReqHandler*   ResultHandler_;
   TEndingStrs          EndingStrs_;
   unsigned             TimeoutSecs_;
   TIPCReqReturn        ExceptionAction_;
protected:
   virtual K_mf(TIPCReqReturn) OnEndingStr    (TSesIPC*, const std::string& result, int endingStrIndex, TSesIPCResultCode);
   virtual K_mf(TIPCReqReturn) OnExceptionEnd (TSesIPC*, const std::string& result, TSesIPCResultCode);
public:
   inline K_ctor TSesIPCReq(const std::string& cmd, TSesIPCReqHandler* handler)
      : Cmd_(cmd)
      , ResultHandler_(handler)
      , ExceptionAction_(TIPCReqReturn(irr_ContinueRecv|irr_ExceptionResult))
      { }
   inline K_ctor TSesIPCReq(const TSesIPCReq& r)
      : Cmd_(r.Cmd_)
      , ResultHandler_(r.ResultHandler_)
      , EndingStrs_(r.EndingStrs_)
      , TimeoutSecs_(r.TimeoutSecs_)
      , ExceptionAction_(r.ExceptionAction_)
      { }
   virtual K_dtor ~TSesIPCReq () {
   }

   inline K_mf(void) AddEndingStr(const std::string& endingStr) {
      EndingStrs_.push_back(endingStr);
   }
   inline K_mf(void) SetEndingStrs(const TEndingStrs& endingStrs) {
      EndingStrs_ = endingStrs;
   }
   inline K_mf(const TEndingStrs&) GetEndingStrs () const {
      return EndingStrs_;
   }
   inline K_mf(void) SetResultHandler (TSesIPCReqHandler* handler) {
      ResultHandler_ = handler;
   }
   inline K_mf(TSesIPCReqHandler*) GetResultHandler () const {
      return ResultHandler_;
   }
   ///> 實際送給 Process 的命令.
   inline K_mf(const std::string&) GetCmd () const {
      return Cmd_;
   }
   inline K_mf(void) SetCmd (const std::string& cmd) {
      Cmd_ = cmd;
   }

   inline K_mf(unsigned) GetTimeoutSecs () const {
      return TimeoutSecs_;
   }
   inline K_mf(void) SetTimeoutSecs (unsigned v) {
      TimeoutSecs_ = v;
   }

   inline K_mf(TIPCReqReturn) GetExceptionAction () const {
      return ExceptionAction_;
   }
   inline K_mf(void) SetExceptionAction (TIPCReqReturn v) {
      ExceptionAction_ = v;
   }

   virtual K_mf(TIPCReqReturn) CheckResult (TSesIPC*, const std::string& result, TSesIPCResultCode);
};
//---------------------------------------------------------------------------
LIBNS_class TSesIPCReqHandler
{
public:
   virtual K_dtor ~TSesIPCReqHandler() {
   }

   /**
      @param cmd    送給 Process 的命令.
      @param req    req可能在通知後刪除, 所以請勿保留req.
      @param result [下了命令之後] 到 [收到 PromptStr] 的訊息, 包含 PromptStr.
      @param endingStrIndex 若 result 存在 req.GetEndingStrs() 所設定的字串,
                            則會傳回其所引值, 否則傳回 -1
      @param ipcr   通知原因:
                        ipcr_PromptStr  = 已收到 PromptStr
                        ipcr_CmdTimeout = 命令執行逾時
                        ipcr_LinkBroken = Process異常結束
                        ipcr_SesDestroy = Session結束
   */
   virtual K_mf(TIPCReqReturn) OnIPCReqResult ( TSesIPC*
                                              , const TSesIPCReq&  req
                                              , const std::string& result
                                              , int                endingStrIndex
                                              , TSesIPCResultCode  ipcr) = 0;
};
//---------------------------------------------------------------------------
/** 負責 Process 的 [溝通] 作業.
*/
LIBNS_class TSesIPC : public TSesBase
{
   ///> Process的提示字串,若 [0]=='\n'表示此字串必須在新的一行開頭
   std::string PromptStr_;
   ///> 等 PromptWaitMS_ (ms) 沒有收到任何資料時:
   ///  就檢查: 送出 AutoCmds
   ///  若沒送出 AutoCmds, 且收到 PromptStr_ 則送出 PendingReqs
   unsigned    PromptWaitMS_;
   ///> 多久要送一次 [保持存在] 的命令.
   unsigned    HeartbeatMS_;  //=0 表示沒有 HeartbeatCmd_
   std::string HeartbeatCmd_;

   struct TAutoCmd {
      std::string Prompt_;  //收到[特定字串]之後,若 [0]=='\n' 表示此字串必須在新的一行開頭
      std::string Cmd_;     //要送出的命令.
      unsigned    MaxCount_;//LinkReady之後,最多執行次數,0=不限.
      unsigned    RunCount_;//LinkReady之後,已執行次數.
      unsigned    TimeoutSecs_;

      inline K_ctor TAutoCmd (const std::string& prompt, const std::string& cmd, unsigned maxCount, unsigned timeoutSecs)
         : Prompt_(prompt)
         , Cmd_(cmd)
         , MaxCount_(maxCount)
         , RunCount_()
         , TimeoutSecs_(timeoutSecs)
         { }
   };
   typedef std::vector<TAutoCmd>   TAutoCmds;
   TAutoCmds   AutoCmds_;
   std::string CurCmd_;
   std::string CmdTail_; //預設 = "\r"
   unsigned    CmdTimeoutSecs_;

   TSesIPCMgr*    Mgr_;
   TSesIPCReq*    RunningReq_;
   bool           CheckingRunnging_;
   bool           IsPromptReady_;
   std::string    ResultBuffer_;

   TTimer<TSesIPC>  Timer_;
   K_mf(void) StopTimer    ()   { Timer_.Stop(); }
   K_mf(void) OnCmdTimeout (TTimerBase*);

   K_mf(bool) CheckRunningReq (TSesIPCResultCode);
   K_mf(bool) CheckAutoCmd    ();
   K_mf(bool) FindPrompt     (const std::string& prompt, bool atEnd);

   ///> 僅允許 Mgr 呼叫:
   friend LIBNS_class TSesIPCMgr;
   K_mf(bool) CancelRunningCmd (TSesIPCReq& req, bool delReq);
   K_mf(int)  CancelRunningCmd (TSesIPCReqHandler&, bool delReq);
protected:
   virtual K_mf(void) OnCheckPrompt(TTimerBase*);

   ///> 一旦傳回 true, 則req擁有權歸 this.
   K_mf(bool) SendCmd (TSesIPCReq* req);

   /** 當指定的時間(PromptWaitMS_)內,沒有收到任何資料,
       且沒有任何 AutoCmd、PendingReq, 且收到 PromptStr_ 時,
       會呼叫此mf
   virtual K_mf(void) OnPromptIdle ();
   */

   K_mf(void) OnMesLinkReady  (TMesBase*);
   K_mf(void) OnMesLinkFail   (TMesBase*, const std::string& reason);
   K_mf(void) OnMesLinkBroken (TMesBase*, const std::string& reason);
   K_mf(void) OnMesRecvReady  (TMesBase* mes, size_t /*size*/);
   K_mf(const std::string&) GetReceiveBuffer() const {
      return ResultBuffer_;
   }

   virtual K_mf(void) SendCmd (const std::string& cmd, unsigned timeoutSecs);

   class TCfgReader;
   friend class TCfgReader;
   ///> 傳回true: 表示 opname 已被處理.
   virtual K_mf(bool) OnCfgParam (char* opname, char* param);
   ///> 先把開頭的空白拿掉後, 若有包含[引號]則把引號拿掉.
   static K_mf(char*) GetAStr (char* param);
   ///> 設定 TSesIPCReq, param格式 = 命令字串, TimeoutSecs, ExceptionAction, 正常結束字串1, 正常結束字串2, ...
   static K_mf(void) SetIPCReq (char* param, TSesIPCReq& req);
public:
   K_ctor  TSesIPC(TSesIPCMgr&);
   K_ctor ~TSesIPC();

   K_mf(bool) SettingApply (const std::string& cfgfilename);

   inline K_mf(TSesIPCMgr*) GetMgr () const {
      return Mgr_;
   }

   ///> 預設 = "\r"
   inline K_mf(void) SetCmdTail (const std::string& v) {
      CmdTail_ = v;
   }
   inline K_mf(const std::string&) GetCmdTail () const {
      return CmdTail_;
   }
   inline K_mf(void) SetPromptStr (const std::string& prompt) {
      PromptStr_ = prompt;
   }
   inline K_mf(const std::string&) GetPromptStr () const {
      return PromptStr_;
   }
   inline K_mf(void) AddAutoCmd (const std::string& prompt, const std::string& cmd, unsigned maxCount, unsigned timeoutSecs) {
      AutoCmds_.push_back(TAutoCmd(prompt,cmd,maxCount,timeoutSecs));
   }
   inline K_mf(bool) IsReadySendCmd () const {
      return(!RunningReq_  &&  IsPromptReady_  &&  !CheckingRunnging_);
   }

   static K_mf(int) FindPromptPos (const std::string& buffer, const std::string& prompt, bool atEnd);

   virtual K_mf(std::string) GetSesCmdPrompt ();
   virtual K_mf(bool)        DoSesCmd        (const std::string& logmsg, const std::string& cmd);
};
//---------------------------------------------------------------------------
LIBNS_class TSesIPCMgr
{
   ///> Pending Requests
   typedef std::deque<TSesIPCReq*>     TPendingReqs;
   TPendingReqs   PendingReqs_;

protected:
   ///> Sessions.
   typedef Kstl::pod_vector<TSesIPC*>  TSessions;
   TSessions   Sessions_;
   K_mf(void) ClearSessions ();

public:
   virtual K_dtor ~TSesIPCMgr();

   virtual K_mf(void) AddSes(TSesIPC&);
   virtual K_mf(void) DelSes(TSesIPC&);

   K_mf(bool) CancelCmd (TSesIPCReq& req, bool delReq);
   K_mf(int)  CancelCmd (TSesIPCReqHandler& handler, bool delReq);

   ///> 是否仍有未執行的req.
   inline K_mf(bool) ReqEmpty () const {
      return PendingReqs_.empty();
   }
   ///> 僅把 req 放到最前等候執行,不考慮現在是否有Session可以執行req.
   inline K_mf(void) ReqPushFront (std::auto_ptr<TSesIPCReq> req) {
      PendingReqs_.push_front(req.release());
   }
   ///> 通知SesIPC已準備好了.
   virtual K_mf(void) OnSesReady (TSesIPC&) {
   }
   ///> 取出最前面的 req
   virtual K_mf(std::auto_ptr<TSesIPCReq>) ReqPopFront (TSesIPC&);
   /** 送出要求.
       @return false 沒有建立任何 Session.
               true  命令處理中.
   */
   K_mf(bool) SendCmd (std::auto_ptr<TSesIPCReq> req);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

