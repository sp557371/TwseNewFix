//---------------------------------------------------------------------------
#ifndef SesSvrPkFTH
#define SesSvrPkFTH
//---------------------------------------------------------------------------
#include "SesServer.h"
#include "KFile.h"
//#include "SesMgr.h"   // struct TSesReg;
//---------------------------------------------------------------------------
namespace Kway {
class TMsgLog;
struct TSesReg;
//----------------------
namespace Pk {
class TSesFT_FileBlock;
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSesSvrFT : public TSesServer
{
public:
   class TImpl;
   K_ctor  TSesSvrFT  (TImpl*);
   K_dtor ~TSesSvrFT  ();
   //setting: root path
   K_mf(bool)  SettingApply          (const std::string&);
   typedef std::string (K_FASTCALL TFile::*TGetFileName) () const;
   K_mf(void)  SetAckFileName        (TGetFileName getFileName);
   K_mf(void)  SetCliPutFileMode     (TFileMode);
   K_mf(void)  SetCliGetAppFileMode  (TFileMode);
   K_mf(void)  SetMLog               (TMsgLog&);

// static K_mf(const TSesReg&)  GetReg      ();
   static K_mf(TImpl*)          CreateImpl  ();

protected:
   K_mf(TCheckResult)  OnPkCheck   (TSesPk&, const THead&, TSize recvSize);
   K_mf(void)          OnPkRecv    (TSesPk&, const THead&);
   K_mf(TWorkersBase*) GetWorkers  ();
   K_mf(void)          OnMesSendEmpty  (TMesBase*);

private:
   typedef TSesServer   inherited;
   //std::auto_ptr<TImpl> Impl_; 在BCB4,當有人繼承 TSesSvrFT,當解構時,使用 auto_ptr<> 會無法刪除 Impl_??
   TImpl*   Impl_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Pk
} // namespace Kway
//---------------------------------------------------------------------------
#endif

