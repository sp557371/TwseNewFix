//---------------------------------------------------------------------------
#ifndef MesIPCH
#define MesIPCH
//---------------------------------------------------------------------------
#include "Mes.h"
#include "KTime.h"
#include "KFile.h"
#include <bitset>
//---------------------------------------------------------------------------
namespace Kway {
//---------------------------------------------------------------------------
LIBNS_class TMesIPC_ImplBase;
/////////////////////////////////////////////////////////////////////////////
enum EProcessState {
   ps_Empty,
   ps_OutSch,
   ps_Opening,
   ps_Running,
   ps_Closing,
   ps_Closed,
   ps_StartErr,
   ps_CloseErr,
   ps_Destroyed,
};
//---------------------------------------------------------------------------
enum EMonFlag
{
   mf_AutoRestart,
   mf_OutSchQuit,
   mf_UseCtrlBreak,

   mf_Count,//MonFlag的數量
};
//---------------------------------------------------------------------------
typedef std::bitset<7>         TWeekSet;
typedef std::bitset<mf_Count>  TMonFlags;
//---------------------------------------------------------------------------
LIBNS_struct TProcessSet
{
   std::string    CommandLine_;
   std::string    ExeName_;
   std::string    Arguments_;
   std::string    ExecDir_;
   Kway::TKTime   TimeBegin_;
   Kway::TKTime   TimeEnd_;
   TWeekSet       WeekDays_;
   std::string    StdinPrekey_;//輸入導向檔
   std::string    StdoutLog_;  //輸出紀錄檔
   TMonFlags      MonFlags_;
   std::string    ExitCmd_;
   unsigned       WaitTerminateSecs_;
   bool           DebugMode_;

   K_ctor TProcessSet() : WaitTerminateSecs_(5), DebugMode_(false) {}

   /* 設定 CommandLine_, ExecDir_(執行檔所在路徑), ExeName_, Arguments_
      若在呼叫此method之前 !ExecDir_.empty(), 則 ExecDir_ 不會改變!
   */
   K_mf(void) SetCommandLine(const std::string& cmdLine);

   K_mf(bool) IsInSch() const;
   K_mf(bool) IsOutSch() const;

   K_mf(void) SetParam (const char* opname, const char* param);
};
/////////////////////////////////////////////////////////////////////////////
/** 負責 Process 的 [啟動]/[結束] 作業.
*/
LIBNS_class TMesIPC : public TMesBase
{
   typedef TMesBase  inherited;
public:
   K_ctor  TMesIPC();
   K_dtor ~TMesIPC();

   ///> 參數 = command line
   K_mf(bool)        SettingApply (const std::string&);
   K_mf(std::string) GetSetting   ()   { return Setting_; }
   ///> UID = GetSetting()
   K_mf(std::string) GetUID       ()   { return Setting_; }
   K_mf(bool)        CloseReason  (const std::string& reason);

   ///> 送指令給被啟動的程式(透過stdin)
   K_mf(int) Send (const void* buffer, size_t size);
   ///> 收到程式[印出stdout,stderr]的訊息
   K_mf(int) Recv (void* buffer, size_t size);
   ///> Impl收到process輸出的資料後,會呼叫這兒
   K_mf(void) RecvReady (char* buffer, size_t rdsz);
   ///> Impl發現process狀態改變,會呼叫這兒
   K_mf(void) SetProcessState (EProcessState);
   inline K_mf(EProcessState) GetProcessState() const {
      return ProcessState_;
   }

protected:
   K_mf(bool) OpenImpl  ();
   K_mf(bool) CloseImpl ();

private:
   class TImpl;
   friend class TImpl;
   friend LIBNS_class TMesIPC_ImplBase;
   TImpl*         Impl_;
   std::string    Setting_;
   TProcessSet    ProcessSet_;
   EProcessState  ProcessState_;
   std::string    Buffer_;
   unsigned       BufferIdx_;
   char*          CurBufPtr_;
   unsigned       CurBufIdx_;
   unsigned       CurBufSize_;

   K_mf(void) ClearBuffer();
};
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMesIPC_ImplBase
{
protected:
   TMesIPC*    Owner_;
   TProcessSet ProcessSet_;
   std::string LastMessage_;
   TFile       FStdin_; //送給process的 stdin file.
   TFile       FStdout_;//process輸出的 stdout,stderr 紀錄

   K_mf(bool) OpenStdFile (Kway::TFile& fd, const std::string& fname, Kway::TFileMode fm, const char* prompt);
   K_mf(void) OpenStdFiles();
public:
   inline K_ctor TMesIPC_ImplBase (TMesIPC* owner)
      : Owner_(owner)
      { }
   virtual K_dtor ~TMesIPC_ImplBase() {
   }

   virtual K_mf(bool) OpenProcess  (const TProcessSet&) = 0;
   virtual K_mf(bool) CloseProcess () = 0;
   virtual K_mf(int)  SendStdin    (const void* buffer, size_t size) = 0;

   inline K_mf(const std::string&) GetLastMessage() const {
      return LastMessage_;
   }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

