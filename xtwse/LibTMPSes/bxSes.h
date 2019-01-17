//---------------------------------------------------------------------------
#ifndef bxSesH
#define bxSesH
//---------------------------------------------------------------------------
#include <map>
#include "bxDef.h"
#include "bxSesMgr.h"
#include "Ses.h"
#include "TwStkOrdCenter.h"
#include "TimerThread.h"
#include "ExcMgrBase.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace bx {
/////////////////////////////////////////////////////////////////////////////
K_class TWorker;
//---------------------------------------------------------------------------
typedef TPacketKey     TKey;
typedef std::map<TKey,TWorker*>   TWorkerRegs; //�T���B�z�̵��U��,�HMsgCode��@�}�C����,�d�߹�ڳB�z�T��������
//---------------------------------------------------------------------------
enum TCheckResult
{
   cpr_HeadOK,    //���T��OK,�N�T��������,�� OnPkRecv() �q��,���ݭn�A�I�s OnPkCheck()
   cpr_Continue,  //���T���ثe�L�k�P�_�O�_OK,������s��ƮɦA�I�s OnPkCheck() �ˬd�@��
   cpr_Error,     //���T�������D,�N�w���쪺��ƲM��
};
//---------------------------------------------------------------------------
K_class TWorker
{
public:
   K_ctor   TWorker  (TWorkerRegs* regs, TKey key)
   {
        TWorkerRegs::iterator   i = regs->find(key);
        if(i == regs->end())
                regs->insert(std::make_pair(key,this));
        else
                i->second = this;
   }
   inline  K_ctor   TWorker  ()                                {}
   virtual K_dtor  ~TWorker  ()                                {}

   //�ˬd�T���O�_OK, OnPKCheck()�b�I�s OnHeadRecv()���e,�ܤַ|�I�s�@��!
   //  �p�G�z���T���@�w���� HeadSize, �h�i�H�����мg�� method
   //  ���T���D�n�Ω�P�_�O�_�٦������ƭn��
   //         ��ܦ��T���L��, ���Ǧ^ cpr_Error
   virtual K_mf(TCheckResult)  OnPkCheck  (TbxSes& Ses,const THead& pk, TSize recvSize,TSize* PacketSize) = 0;

   //��ڸ�ƺ򱵦b THead ����
   virtual K_mf(void)  OnPkRecv  (TbxSes& Ses,const THead& pk,TReqPtr& ReqPtr)  {}
   //��������l�t�εo�X�n�D��
   virtual K_mf(bool)  SendReq   (TbxSes& Ses,TPacketCode code,TReqPtr& ReqPtr) {return false;}
   //��������^���l�t�εo�X�n�D��
   virtual K_mf(bool)  SendReq   (TbxSes& Ses,TPacketCode code,TSize Num) {return false;}
   //���s�u�l�t�εo�X�n�J�n�D��
   virtual K_mf(bool)  LinkReq   (TbxSes& Ses,TPacketCode code,TApCode apcode) {return false;}
};
/////////////////////////////////////////////////////////////////////////////
K_class TWorkersBase
{
public:
   virtual K_dtor ~TWorkersBase  ()    {}

   //���XWorker���U��
   virtual K_mf(TWorkerRegs*)  GetWorkerRegs    ()       = 0;
   //���XWorker���U���j�p
   virtual K_mf(TSize)         GetWorkersCount  () const = 0;
   //���X�Y MsgCode �� Worker
   virtual K_mf(TWorker*)      GetWorker     (TKey key)
   {
        TWorkerRegs* regs = GetWorkerRegs();
        TWorkerRegs::iterator   i = regs->find(key);
        if(i == regs->end())
                return 0;
        else
                return i->second;
   }
   //�]�w�Y MsgCode �� Worker, �Ǧ^�]�w�e�� Worker
   K_mf(TWorker*)              SetWorker     (TKey, TWorker* newWorker);
};
/////////////////////////////////////////////////////////////////////////////
class TWorkers : public TWorkersBase
{
protected:
   TWorkerRegs Workers_;
public:
   K_ctor              TWorkers        ()                   { }
   K_mf(TWorkerRegs*)  GetWorkerRegs   ()                   { return &Workers_; }
   K_mf(TSize)         GetWorkersCount () const             { return static_cast<word>(Workers_.size()); }
   K_mf(TWorker*)      GetWorker       (TKey key);
};
/////////////////////////////////////////////////////////////////////////////
K_class  TbxSes : public TSesBase
{
   typedef TSesBase  inherited;
   typedef TbxSes    this_type;
   typedef Kway::Tw::Stk::TOrdCenter  TOrdCenter;
   //typedef wise_ptr<TbxSesMgr>  TMgrPtr;
public:
   //----- �غc/�Ѻc
   K_ctor  TbxSes  (TbxSesMgr*);
   K_dtor ~TbxSes  ();



   //----- �T���B�z/�ǰe
   virtual K_mf(bool)  SendPk       (const THead&,TSize);      //�z��ڭn�ǰe�����,�����򱵦b THead ����

   //----- ���A�B�z
   enum EState {
      sp_Creating,
      sp_Destroying,
      sp_Error,
      sp_TimeEarly,
      sp_WaitConnected,
      sp_WaitLinkReady,     //���Զi�J�s�u�l�t��
      sp_WaitLoginReady,    //���Եn��
      sp_WaitSubSysReady,   //���ԱҰ����Τl�t��
      sp_Ready,             //���Τl�t�αҰʧ���
      sp_WaitForOrder,      //���Ա����e�U
      sp_WaitForOrdReply,   //���ԩe�U�^��
      sp_StartRptSys,       //�_�l����^���l�t��
      sp_WaitForRpt,        //���Ԧ���^�����
      sp_RecievingRpt,      //��������^�����
      sp_Wait050,           //���ݳs�u�T�{ 
      sp_WaitToResend,      //���ݭ��e�e�U 
      sp_Signout,           //�t�εn�X
   };

   virtual  K_mf(void)   SetState  (EState newState, const std::string& reason);
   inline  K_mf(EState)  GetState  () const   { return State_; }
   inline K_mf(void)            SetAppendNo(const TAppendNo appno){ AppendNo_ = appno; }
   inline K_mf(TAppendNo)       GetAppendNo() const { return AppendNo_;}
   virtual K_mf(void)    OnWorkerFinished(TPacketCode PkCode,const THead&) = 0;    //Worker�u�@�����^��
   virtual K_mf(void)    OnLinkWorkerFinished(TPacketCode PkCode,const THead&) = 0;    //LinkWorker�u�@�����^��

   //���إ�bxSession ��]�w�Ψ��Ϊ������禡
   //�C��bxSes �إ߫�аȥ��������l�Ƴ]�wBrokerID.Password.PvcID.Market��ApCode
   //----------------------------------
   inline K_mf(void)            SetPassWord(const TSize pwd){PassWord_ = pwd;}
   inline K_mf(TSize)           GetPassWord() const         {return PassWord_;}
   inline K_mf(void)            SetBrokerID(const TBrkID id){BrokerID_ = id; }
   inline K_mf(TBrkID)          GetBrokerID() const         {return BrokerID_;}
   inline K_mf(void)            SetPvcID(const TPvcID pvcid){PvcID_ = pvcid; }
   inline K_mf(TPvcID)          GetPvcID() const            {return PvcID_;}
   inline K_mf(void)            SetMarket(EMarket mkt)      {Market_.assignT(mkt);}
   inline K_mf(TMarket)         GetMarket() const           {return Market_;}
   inline K_mf(void)            SetApCode(TApCode apcode)   {ApCode_ = apcode;}
   inline K_mf(TSubSysName)     GetSubSys()                 {return SubSys_;}
   inline K_mf(std::string)     GetSetting()                {return SettingStr_;}
   virtual K_mf(bool)            IsFree()                    {return (State_ == sp_WaitForOrder && 
                                                                     ReqPtr_.get() == 0);}
   virtual K_mf(bool)            IsSending()                 {return State_ == sp_WaitForOrdReply;}
   inline K_mf(bool)            IsFix()                     {return IsFix_;}
   //-----------------------------------

   //������U��bxMgr ��,bxMgr �|���� Mgr ��pointer
   K_mf(void)           SetMgr(TbxSesMgr* mgr)        { MgrPtr_ = mgr;}
   K_mf(TbxSesMgr*)     GetMgr()                      {return MgrPtr_;}
   virtual K_mf(void)   DelSubSysReg()  = 0;
   virtual K_mf(TApCode) GetApCode(){return ApCode_;}

   K_mf(bool)           SettingApply  (const std::string&);
   K_mf(TSesEvHandler*) GetbxSesEvHandler() {return GetSesEvHandler();}
   TOrdCenter*          OrdCenterPtr_;
   TSesID               ID_;
   virtual K_mf(void)   OrdUpdate(TPacketCode PkCode,const THead&) {};
   virtual K_mf(void)    ReStartLink(){};
protected:

   //�Ұʭp�ɾ�,��z�]�w���ɶ���F,�h�|�ϥ� OnTimer() �q���z
   virtual K_mf(void)   StartTimer  (size_t ms);
   //����p�ɾ�,���A���� OnTimer() ���ƥ�
   K_mf(void)           StopTimer   ();

   virtual K_mf(void)  OnMesLinkReady   (TMesBase*);// SetState(sp_WaitConnected)
   virtual K_mf(void)  OnMesLinkBroken  (TMesBase*, const std::string&);//�M�������w�İ�
   K_mf(void)  OnMesLinkFail    (TMesBase*, const std::string&);//SetState(sp_Error);

   //�Y�z���Ұʭp�ɾ�,�h�|�z�L OnTimer() �ӧi���l�ͥX�Ӫ�class
   virtual K_mf(void)  OnTimer           (TTimerBase*);//do nothing
   K_mf(void)  OnRptTimer       (TTimerBase*);
   K_mf(void)  ResetRptTimer    ();
//   virtual K_mf(void)  AddWorkers        (){};
   //�� Mes �������Ʈ�,�z�L OnMesRecvReady() �q��
   //  �b���B�|�����@�ӧ���T��,�éI�s Worker �B�z
   virtual K_mf(void)  OnMesRecvReady    (TMesBase*, size_t size);//�����

   TWorkers                Workers_;
   size_t                  RecvPos_;   //�w���쪺��Ƥj�p
   TCheckResult            PkResult_;
   size_t                  SkipBytes_;
   TbxRec                  RecvPk_[2]; //RecvPk_[1]�@�O�@��!
   EState                  State_;
   Kway::word              FailCount_;
   TSubSysName             SubSys_;
   TTimer<this_type>       Timer_;
   TTimer<this_type>       RptTimer_;
   TReqPtr                 ReqPtr_;
   TMarket                 Market_;             //Ses ���ݥ����O
   TbxSesMgr*              MgrPtr_;             //�޲z��bxMgr
   bool                    IsFix_; 
   TApCode                 ApCode_;
   TBrkID                  BrokerID_;
   TAppendNo               AppendNo_;
   TSize                   PassWord_;
   TPvcID                  PvcID_;
   std::string             SettingStr_;         //�]�w�Ѽ�
   std::string             SesTermID_;
//   TSize                   FailCount_;          //�s��Link Fail������
};
/////////////////////////////////////////////////////////////////////////////
} // namespace bx
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif

