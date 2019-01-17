//---------------------------------------------------------------------------
#ifndef SesFTPH
#define SesFTPH
//---------------------------------------------------------------------------
#include "SesIPC.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TFtpReq;
LIBNS_class TSesFTP;
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TFtpDirList;
//---------------------------------------------------------------------------
struct TFtpDirListRec {
   std::string FileName_;
};
//---------------------------------------------------------------------------
LIBNS_class TFtpDirListRecHandler {
public:
   /**
   @param rec =NULL��ܸ�Ķ����
   */
   virtual K_mf(void) OnFtpDirListRec (TSesIPC*, const TFtpDirList&, const TFtpDirListRec* rec) = 0;
};
//---------------------------------------------------------------------------
LIBNS_class TFtpDirList : public TSesIPCReq
{
   typedef TSesIPCReq      inherited;
   TFtpDirListRecHandler*  FileRecHandler_;
   unsigned                DirFilePos_;
protected:
   K_mf(TIPCReqReturn) OnEndingStr (TSesIPC* ses, const std::string& result, int endingStrIndex, TSesIPCResultCode ipcr);
public:
   inline K_ctor TFtpDirList (const std::string cmd)
      : inherited(cmd, 0)
      , FileRecHandler_(0)
      , DirFilePos_(0)
      { }
   inline K_ctor TFtpDirList (const TFtpDirList& r)
      : inherited(r)
      , FileRecHandler_(r.FileRecHandler_)
      , DirFilePos_(r.DirFilePos_)
      { }

   inline K_mf(void) SetFileRecHandler (TFtpDirListRecHandler* handler) {
      FileRecHandler_ = handler;
   }
   inline K_mf(void) SetDirFilePos (unsigned pos) {
      DirFilePos_ = pos;
   }
};
/////////////////////////////////////////////////////////////////////////////
/** �t�d �P ftp client Process �� [���q] �@�~.
*/
LIBNS_class TSesFTP : public TSesIPC
{
   typedef TSesIPC   inherited;
   
   TSesIPCReq  SetAscii_;
   TSesIPCReq  SetBinary_;
   TSesIPCReq  LocalCD_;
   TSesIPCReq  RemoteCD_;
   TFtpDirList GetDirList_;
   TSesIPCReq  GetFile_;
   TSesIPCReq  PutFile_;
protected:
   virtual K_mf(bool) OnCfgParam (char* opname, char* param);

   K_mf(TSesIPCReq*) SendReqCmd (TSesIPCReqHandler* handler, const TSesIPCReq& req, const std::string* cmdapp);
public:
   K_ctor TSesFTP (TSesIPCMgr&);

   /** ���B���Ǧ^�Ȧb [�B�z������] �|�Q�R��!
       �Y�Ǧ^ NULL, ���Session���L��, �L�k�B�z�n�D.
       �Y���ݭn�N cmd �Ʀb Queue, �h�бq Mgr �e�X�n�D.
   */
   inline K_mf(TSesIPCReq*) SetAscii (TSesIPCReqHandler* handler) {
      return SendReqCmd(handler, SetAscii_, 0);
   }
   inline K_mf(TSesIPCReq*) SetBinary (TSesIPCReqHandler* handler) {
      return SendReqCmd(handler, SetBinary_, 0);
   }
   inline K_mf(TSesIPCReq*) LocalCD (TSesIPCReqHandler* handler, const std::string& path) {
      return SendReqCmd(handler, LocalCD_, &path);
   }
   inline K_mf(TSesIPCReq*) RemoteCD (TSesIPCReqHandler* handler, const std::string& path) {
      return SendReqCmd(handler, RemoteCD_, &path);
   }

   K_mf(TFtpDirList*) GetDirList (TFtpDirListRecHandler* handler);
   K_mf(TSesIPCReq*)  GetFile    (TSesIPCReqHandler* handler, const std::string& remoteFileName, const std::string& localFileName  = std::string());
   K_mf(TSesIPCReq*)  PutFile    (TSesIPCReqHandler* handler, const std::string& localFileName,  const std::string& remoteFileName = std::string());
   K_mf(std::string)  GetFileCmd (const std::string& remoteFileName, const std::string& localFileName  = std::string());
   K_mf(std::string)  PutFileCmd (const std::string& localFileName,  const std::string& remoteFileName = std::string());
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

