//---------------------------------------------------------------------------
#ifndef bxLinkSesH
#define bxLinkSesH
//---------------------------------------------------------------------------
#include "bxHeadDef.h"
#include "bxSes.h"
#include "bxLink.h"
#include "bxLinkWorker.h"
#include "TwStk.h"
#include "SesMgr.h"
#include "BxLoger.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
//---------------------------------------------------------------------------
K_class TbxLinkSes : public TbxSesBase
{
   typedef TbxSesBase       inherited;
   typedef TbxLinkSes       this_type;
   typedef KStk::TMarket    TMarket;
private:
   TWork_L1010  L1010_;
   TWork_L1020  L1020_;
   TWork_L1030  L1030_;
//   TLinkErrInfo LinkErrInfo_;
   size_t       LinkTimeOut_;
   int          State_;
   int          RetryCnt_;

   TPvcID       PvcID_;
   TMarket      Market_;
   TAppendNo    AppendNo_;
   TBrkID       Broker_;
   TApKey       ApCode_;
   TBxSize      Passwd_;
   TBxLogBase*  Loger_;
public:
   enum TLinkState
   {
     ls_Exception=0,    // �������~
     ls_Error,          // ���~
     ls_TimeEarly,      // �ɶ�����
     ls_WaitRestart,    // ���J���s�Ұ�
     ls_WaitWakeup,     // ���Զ}���T���^��
     ls_WaitLogon,      // ���Եn���T���^��
     ls_WaitSubSystem,  // ���Գs�u�l�t�αҰʰT���^��
     ls_Logout,         // �������u�T���^��
     ls_LinkReady       // �s�u�l�t�αҰ�OK
   };
   int          RecID_;

protected:
//        K_mf(void)   SetLinkErrInfo ();
        K_mf(void)   AddLinkWorkers ();
virtual K_mf(void)   OnMesLinkReady (TMesBase*);
virtual K_mf(void)   OnMesLinkBroken(TMesBase*, const std::string&);
virtual K_mf(void)   OnMesLinkFail  (TMesBase*, const std::string&);
virtual K_mf(bool)   SettingApply   (const std::string& setting);
virtual K_mf(void)   DoTimeOut      (TTimerBase*);
public:
        K_ctor       TbxLinkSes(TbxFTSesMgr*, TMarket);
virtual K_dtor      ~TbxLinkSes();
        K_mf(void)   SetState  (int state, const std::string& reason);
        K_mf(void)   SetError  (const std::string&, const std::string&);
        K_mf(void)   ReLink    ();          //�s�u�l�t�έ��s�s�u
        K_mf(void)   UnLink    ();          //�����s�u�l�t��
        K_mf(void)   StartLinkTimer()       { StartTimer(LinkTimeOut_); }
        K_mf(bool)   DoWorker  (const TWorkKey, const TBxMsg, const TControlHead&);
        K_mf(TWorkKey)GetWorkKey    (TFunCode);
virtual K_mf(void)   OnSubSysReady  (TApKey) = 0;
virtual K_mf(void)   OnSubSysBroken (TSubSysName,TStCode)= 0;
virtual K_mf(bool)   Send           (TbxData&);

   K_mf(void)   SetLinkTimeOut  (size_t ms)     { LinkTimeOut_ = ms;}
   K_mf(void)   SetAppendNo     (TAppendNo an)  { AppendNo_    = an;}
   K_mf(TAppendNo)  GetAppendNo () const        { return AppendNo_; }
   K_mf(TBrkID)     GetBrokerID () const        { return Broker_;   }
   K_mf(TApKey)     GetApCode   () const        { return ApCode_;   }
   K_mf(TBxSize)    GetPassword () const        { return Passwd_;   }
   K_mf(TPvcID)     GetPvcID    () const        { return PvcID_;    }
   K_mf(TMarket)    GetMarket   () const        { return Market_;   }
   K_mf(int)        GetSesState ()              { return State_;    }
   K_mf(TBxLogBase*)GetLoger    () const        { return Loger_;    }
   K_mf(void)       WriteLoger  (const std::string&, const std::string&);
virtual K_mf(const std::string) SesName() = 0;
};
/////////////////////////////////////////////////////////////////////////////
};  // Bx
};  // Tw
};  // Kway
//---------------------------------------------------------------------------
#endif


