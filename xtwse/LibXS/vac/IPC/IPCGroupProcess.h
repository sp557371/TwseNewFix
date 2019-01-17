//---------------------------------------------------------------------------
#ifndef IPCGroupProcessH
#define IPCGroupProcessH

#include "ev_map.h"
#include "KTime.h"
/////////////////////////////////////////////////////////////////////////////
struct TIPCGroupProcess
{
   std::string AgentName_;
   std::string ProcessParam_;    // TMesIPC::SettingApply()
   std::string DependOn_;        // ProcessID
   std::string DependOnKeyWord_; // ex: "ExitCode=0"

   std::string GroupStatus_;//此行設定的[群組最後訊息]

   K_ctor TIPCGroupProcess() : HasExitMsg_(false) {
   }
   K_mf(void) ClrLastStatus () {
      GroupStatus_.clear();
      LastStatus_.clear();
      HasExitMsg_ = false;
   }
   K_mf(void) SetLastStatus (const std::string& status, bool isExitMsg) {
      if(HasExitMsg_ && !isExitMsg)
         return;
      LastStatus_ = status;
      HasExitMsg_ = isExitMsg;
   }
   K_mf(const std::string&) GetLastStatus () const {
      return LastStatus_;
   }
private:
   std::string LastStatus_; //此行程的最後訊息
   bool        HasExitMsg_;
};
class TIPCGroupProcesses : public Kstl::ev_map<std::string, TIPCGroupProcess>
{
   std::string GroupName_;
public:
   inline K_mf(const std::string&) GetGroupName() const {
      return GroupName_;
   }
   inline K_mf(void) SetGroupName(const std::string& value) {
      GroupName_ = value;
   }
};
/////////////////////////////////////////////////////////////////////////////
struct TIPCGroupRec
{
   std::string          GroupSetting_;
   bool                 GroupEnabled_;
   Kway::TKDate         HoldDay_;
   TIPCGroupProcesses   Processes_;

   inline K_ctor TIPCGroupRec ()
      : GroupEnabled_(false)
      { }

   K_mf(std::string) GetPersistentStr () const;
   K_mf(void)        SetPersistentStr (const std::string& groupName, const std::string& str);

   K_mf(void) ClrLastStatus () {
      FirstStatus_.clear();
      LastStatus_.clear();
      LastStatusIPC_.clear();
   }
   K_mf(void) SetLastStatus (const std::string& status, const std::string& ipc) {
      LastStatus_ = status;
      LastStatusIPC_ = ipc;
      if(FirstStatus_.empty())
         FirstStatus_ = status;
   }
   inline K_mf(const std::string&) GetLastStatus () const {
      return LastStatus_;
   }
   inline K_mf(const std::string&) GetFirstStatus () const {
      return FirstStatus_;
   }
   inline K_mf(const std::string&) GetLastStatusIPC () const {
      return LastStatusIPC_;
   }
private:
   K_mf(void) SetGroupSetting(const std::string&);

   std::string  LastStatus_;
   std::string  LastStatusIPC_;
   std::string  FirstStatus_;//執行原因
};
//---------------------------------------------------------------------------
typedef Kstl::ev_map<std::string, TIPCGroupRec> TIPCGroupRecs;
/////////////////////////////////////////////////////////////////////////////
#endif
