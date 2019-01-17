//---------------------------------------------------------------------------
#ifndef pkftc_baseH
#define pkftc_baseH
//---------------------------------------------------------------------------
#include "SesMgr.h"
#include "SesCliPkFT.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Pk {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TPkFTC_Base
{
protected:
   TSesMgr        Mgr_;
   TaViewArea     StatusVA_;
   TaViewArea     SettingVA_;
   const TSesReg* CliSesReg_;

   //------------------------------------
   typedef TSesCliPkFT::TReqPtr (K_FASTCALL TSesCliPkFT::*TFnReqFT) (const std::string&, const std::string&);
   typedef std::vector<std::string> TFileNames;
   //------------------------------------
   K_mf(TSesCliPkFT*)  GetCurrentSes  ();
   K_mf(void)          AddConn        (const char* cmdbuf);
   K_mf(std::string)   TakeFileName   (char*& cmdbuf);
   //------------------------------------
   K_mf(TSesCliPkFT::TReqPtr)  ReqFile      (TSesCliPkFT*, char* cmdbuf, TFnReqFT);
   K_mf(void)                  ReqFileList  (const TFileNames& gets, const TFileNames& puts, size_t hostid);
   K_mf(void)                  ReqFileList  (TFnReqFT,
                                             TFileNames::const_iterator ifrom,
                                             TFileNames::const_iterator ito,
                                             size_t hostid);
public:
   K_ctor  TPkFTC_Base  (const TSesReg&);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Pk
} // namespace Kway
//---------------------------------------------------------------------------
#endif
