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
   irr_End,                   ///< ���n�D�w�B�z����.
   irr_ContinueRecv    = 0x01,///< �R�O������,�~�򱵦�.
   irr_ExceptionResult = 0x02,///< ���T�����`,���O���n�D���檺�T��.
   irr_ClearBuffer     = 0x04,///< �M�������w�İ�.
   irr_SendAgain       = 0x10 + irr_ContinueRecv,///< �A�e�@���R�O(req�i��w����Cmd_),�A�Τ��i���έn�D(�Ҧp: asc + cd + get).
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
   ///> ��ڰe�� Process ���R�O.
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
      @param cmd    �e�� Process ���R�O.
      @param req    req�i��b�q����R��, �ҥH�ФūO�dreq.
      @param result [�U�F�R�O����] �� [���� PromptStr] ���T��, �]�t PromptStr.
      @param endingStrIndex �Y result �s�b req.GetEndingStrs() �ҳ]�w���r��,
                            �h�|�Ǧ^��Ҥޭ�, �_�h�Ǧ^ -1
      @param ipcr   �q����]:
                        ipcr_PromptStr  = �w���� PromptStr
                        ipcr_CmdTimeout = �R�O����O��
                        ipcr_LinkBroken = Process���`����
                        ipcr_SesDestroy = Session����
   */
   virtual K_mf(TIPCReqReturn) OnIPCReqResult ( TSesIPC*
                                              , const TSesIPCReq&  req
                                              , const std::string& result
                                              , int                endingStrIndex
                                              , TSesIPCResultCode  ipcr) = 0;
};
//---------------------------------------------------------------------------
/** �t�d Process �� [���q] �@�~.
*/
LIBNS_class TSesIPC : public TSesBase
{
   ///> Process�����ܦr��,�Y [0]=='\n'��ܦ��r�ꥲ���b�s���@��}�Y
   std::string PromptStr_;
   ///> �� PromptWaitMS_ (ms) �S����������Ʈ�:
   ///  �N�ˬd: �e�X AutoCmds
   ///  �Y�S�e�X AutoCmds, �B���� PromptStr_ �h�e�X PendingReqs
   unsigned    PromptWaitMS_;
   ///> �h�[�n�e�@�� [�O���s�b] ���R�O.
   unsigned    HeartbeatMS_;  //=0 ��ܨS�� HeartbeatCmd_
   std::string HeartbeatCmd_;

   struct TAutoCmd {
      std::string Prompt_;  //����[�S�w�r��]����,�Y [0]=='\n' ��ܦ��r�ꥲ���b�s���@��}�Y
      std::string Cmd_;     //�n�e�X���R�O.
      unsigned    MaxCount_;//LinkReady����,�̦h���榸��,0=����.
      unsigned    RunCount_;//LinkReady����,�w���榸��.
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
   std::string CmdTail_; //�w�] = "\r"
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

   ///> �Ȥ��\ Mgr �I�s:
   friend LIBNS_class TSesIPCMgr;
   K_mf(bool) CancelRunningCmd (TSesIPCReq& req, bool delReq);
   K_mf(int)  CancelRunningCmd (TSesIPCReqHandler&, bool delReq);
protected:
   virtual K_mf(void) OnCheckPrompt(TTimerBase*);

   ///> �@���Ǧ^ true, �hreq�֦��v�k this.
   K_mf(bool) SendCmd (TSesIPCReq* req);

   /** ����w���ɶ�(PromptWaitMS_)��,�S�����������,
       �B�S������ AutoCmd�BPendingReq, �B���� PromptStr_ ��,
       �|�I�s��mf
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
   ///> �Ǧ^true: ��� opname �w�Q�B�z.
   virtual K_mf(bool) OnCfgParam (char* opname, char* param);
   ///> ����}�Y���ťծ�����, �Y���]�t[�޸�]�h��޸�����.
   static K_mf(char*) GetAStr (char* param);
   ///> �]�w TSesIPCReq, param�榡 = �R�O�r��, TimeoutSecs, ExceptionAction, ���`�����r��1, ���`�����r��2, ...
   static K_mf(void) SetIPCReq (char* param, TSesIPCReq& req);
public:
   K_ctor  TSesIPC(TSesIPCMgr&);
   K_ctor ~TSesIPC();

   K_mf(bool) SettingApply (const std::string& cfgfilename);

   inline K_mf(TSesIPCMgr*) GetMgr () const {
      return Mgr_;
   }

   ///> �w�] = "\r"
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

   ///> �O�_���������檺req.
   inline K_mf(bool) ReqEmpty () const {
      return PendingReqs_.empty();
   }
   ///> �ȧ� req ���̫e���԰���,���Ҽ{�{�b�O�_��Session�i�H����req.
   inline K_mf(void) ReqPushFront (std::auto_ptr<TSesIPCReq> req) {
      PendingReqs_.push_front(req.release());
   }
   ///> �q��SesIPC�w�ǳƦn�F.
   virtual K_mf(void) OnSesReady (TSesIPC&) {
   }
   ///> ���X�̫e���� req
   virtual K_mf(std::auto_ptr<TSesIPCReq>) ReqPopFront (TSesIPC&);
   /** �e�X�n�D.
       @return false �S���إߥ��� Session.
               true  �R�O�B�z��.
   */
   K_mf(bool) SendCmd (std::auto_ptr<TSesIPCReq> req);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

