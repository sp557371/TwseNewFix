//
// �q�Tclasses��¦: Session-Messager
//    class TSesCliPkFT; �ɮ׶ǿ�B�z�{��(Client��)
//
// ����: $Id: SesCliPkFT.h,v 1.6 2005/12/28 08:41:38 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SesCliPkFTH
#define SesCliPkFTH
//---------------------------------------------------------------------------
#include "SesClient.h"
#include "wise_ptr.h"
#include "Pk/PkFT.h"
//#include "SesMgr.h"   // struct TSesReq;
//---------------------------------------------------------------------------
namespace Kway {
struct TSesReg;
//----------------------
namespace Pk {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSesCliPkFT : public TSesClient
{
   typedef TSesClient   inherited;
public:
   class TImpl;
   K_ctor  TSesCliPkFT  (TImpl*, TVer, const std::string& apName);
   K_dtor ~TSesCliPkFT  ();

   enum TReqState {
      rs_ReqError,
      rs_WaittingConn,
      rs_ReqSending,
      rs_RemoteError,
      rs_WaittingData,
      rs_Canceling,
      rs_Finished,
   };
   enum TPutAppPriority {
      pap_Normal = 0,   //�w�]��
      pap_NextAfThisEOF,//���ɨS�e��(eof)���ǰe����ftf_ReqPutAppend,�Υ�����ftf_ReqPutStatic.
      pap_WaitBfEOF,    //�e�@���ɮרS�e��,���ǰe����
   };
   class TAfFinishedFT;
   struct TReqRec {
      TReqState      State_;
      TSesFTFunc     ReqFuncCode_;
      TFile          LocalFile_;
      word           RemoteID_;
      std::string    RemoteFileName_;
      TFileSizeFT    RemoteFileSize_;
      std::string    ReqRemoteFileName_;
      std::string    Result_;
      std::string    PendingLocalFileName_;
      TFileMode      PendingLocalFileMode_;
      union {
         size_t            PutIdleCount_;
         TPutAppPriority   PutaPriority_;
      };
      TAfFinishedFT* OnAfFinished_;
      uint64         RetryCounter_;//ms
   };
   class TAfFinishedFT {
      friend class TSesCliPkFT;
      virtual K_mf(void) OnAfFinishedFT (TSesCliPkFT*, TReqRec*) = 0;
      virtual K_mf(void) OnStartFT      (TSesCliPkFT*, TReqRec*) {
      }
   public:
      virtual K_dtor  ~TAfFinishedFT  ()   { }
   };
   typedef Kstl::wise_ptr<TReqRec>  TReqPtr;
   typedef std::vector<TReqPtr>     TReqRecs;
   //��if(localFileName.empty())   localFileName = remoteFileName;
   K_mf(TReqPtr) GetFile    (const std::string& remoteFileName, const std::string& localFileName);
   K_mf(TReqPtr) GetAppFile (const std::string& remoteFileName, const std::string& localFileName);
   //��if(remoteFileName.empty())  remoteFileName = localFileName;
   K_mf(TReqPtr) PutFile    (const std::string& localFileName, const std::string& remoteFileName);
   K_mf(TReqPtr) PutAppFile (const std::string& localFileName, const std::string& remoteFileName);

   K_mf(const TReqRecs&)  GetReqs        () const;
   K_mf(bool)             CancelReqFile  (size_t id);//cancel at GetReqs()[id]
   K_mf(bool)             CancelReqFile  (TReqRec*);
   K_mf(int)              GetReqID       (TReqRec*);

   K_mf(bool) IsReadyFT ();

   static K_mf(const TSesReg&)  GetReg      ();
   static K_mf(TImpl*)          CreateImpl  ();

protected:
   K_mf(TWorkersBase*) GetWorkers  ();
   K_mf(TCheckResult)  OnPkCheck   (TSesPk&, const THead&, TSize recvSize);
   K_mf(void)          OnPkRecv    (TSesPk&, const THead&);
   K_mf(void)          OnFileBlock (const TSesFT_FileBlock&);

           K_mf(void)  OnConnectedAck   (const TConnectedAck&);
           K_mf(void)  OnSignon         (const std::string&);
   virtual K_mf(void)  CheckReadyReq    ();
           K_mf(void)  OnMesLinkBroken  (TMesBase*, const std::string&);
           K_mf(void)  OnMesLinkReady   (TMesBase*);
           K_mf(void)  OnMesSendEmpty   (TMesBase*);

   friend class TImpl;
   // for get/put static file finished
   //  or get/put static/append file remote error
   // �w�]Ĳ�o req->OnAfFinished_()�ƥ�.
   virtual K_mf(void) OnAfFinished (TReqPtr);
   virtual K_mf(void) OnStartFT    (TReqPtr);

private:
   //std::auto_ptr<TImpl> Impl_; �bBCB4,���H�~�� TSesSvrFT,��Ѻc��,�ϥ� auto_ptr<> �|�L�k�R�� Impl_??
   TImpl*   Impl_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Pk
} // namespace Kway
//---------------------------------------------------------------------------
#endif

