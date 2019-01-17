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

   mf_Count,//MonFlag���ƶq
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
   std::string    StdinPrekey_;//��J�ɦV��
   std::string    StdoutLog_;  //��X������
   TMonFlags      MonFlags_;
   std::string    ExitCmd_;
   unsigned       WaitTerminateSecs_;
   bool           DebugMode_;

   K_ctor TProcessSet() : WaitTerminateSecs_(5), DebugMode_(false) {}

   /* �]�w CommandLine_, ExecDir_(�����ɩҦb���|), ExeName_, Arguments_
      �Y�b�I�s��method���e !ExecDir_.empty(), �h ExecDir_ ���|����!
   */
   K_mf(void) SetCommandLine(const std::string& cmdLine);

   K_mf(bool) IsInSch() const;
   K_mf(bool) IsOutSch() const;

   K_mf(void) SetParam (const char* opname, const char* param);
};
/////////////////////////////////////////////////////////////////////////////
/** �t�d Process �� [�Ұ�]/[����] �@�~.
*/
LIBNS_class TMesIPC : public TMesBase
{
   typedef TMesBase  inherited;
public:
   K_ctor  TMesIPC();
   K_dtor ~TMesIPC();

   ///> �Ѽ� = command line
   K_mf(bool)        SettingApply (const std::string&);
   K_mf(std::string) GetSetting   ()   { return Setting_; }
   ///> UID = GetSetting()
   K_mf(std::string) GetUID       ()   { return Setting_; }
   K_mf(bool)        CloseReason  (const std::string& reason);

   ///> �e���O���Q�Ұʪ��{��(�z�Lstdin)
   K_mf(int) Send (const void* buffer, size_t size);
   ///> ����{��[�L�Xstdout,stderr]���T��
   K_mf(int) Recv (void* buffer, size_t size);
   ///> Impl����process��X����ƫ�,�|�I�s�o��
   K_mf(void) RecvReady (char* buffer, size_t rdsz);
   ///> Impl�o�{process���A����,�|�I�s�o��
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
   TFile       FStdin_; //�e��process�� stdin file.
   TFile       FStdout_;//process��X�� stdout,stderr ����

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

