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
typedef std::map<TKey,TWorker*>   TWorkerRegs; //訊息處理者註冊表,以MsgCode當作陣列索引,查詢實際處理訊息的物件
//---------------------------------------------------------------------------
enum TCheckResult
{
   cpr_HeadOK,    //此訊息OK,將訊息收完後,用 OnPkRecv() 通知,不需要再呼叫 OnPkCheck()
   cpr_Continue,  //此訊息目前無法判斷是否OK,當有收到新資料時再呼叫 OnPkCheck() 檢查一次
   cpr_Error,     //此訊息有問題,將已收到的資料清除
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

   //檢查訊息是否OK, OnPKCheck()在呼叫 OnHeadRecv()之前,至少會呼叫一次!
   //  如果您的訊息一定等於 HeadSize, 則可以不必覆寫此 method
   //  此訊息主要用於判斷是否還有後續資料要收
   //         表示此訊息無效, 應傳回 cpr_Error
   virtual K_mf(TCheckResult)  OnPkCheck  (TbxSes& Ses,const THead& pk, TSize recvSize,TSize* PacketSize) = 0;

   //實際資料緊接在 THead 之後
   virtual K_mf(void)  OnPkRecv  (TbxSes& Ses,const THead& pk,TReqPtr& ReqPtr)  {}
   //↓給交易子系統發出要求用
   virtual K_mf(bool)  SendReq   (TbxSes& Ses,TPacketCode code,TReqPtr& ReqPtr) {return false;}
   //↓給成交回報子系統發出要求用
   virtual K_mf(bool)  SendReq   (TbxSes& Ses,TPacketCode code,TSize Num) {return false;}
   //給連線子系統發出登入要求用
   virtual K_mf(bool)  LinkReq   (TbxSes& Ses,TPacketCode code,TApCode apcode) {return false;}
};
/////////////////////////////////////////////////////////////////////////////
K_class TWorkersBase
{
public:
   virtual K_dtor ~TWorkersBase  ()    {}

   //取出Worker註冊表
   virtual K_mf(TWorkerRegs*)  GetWorkerRegs    ()       = 0;
   //取出Worker註冊表的大小
   virtual K_mf(TSize)         GetWorkersCount  () const = 0;
   //取出某 MsgCode 的 Worker
   virtual K_mf(TWorker*)      GetWorker     (TKey key)
   {
        TWorkerRegs* regs = GetWorkerRegs();
        TWorkerRegs::iterator   i = regs->find(key);
        if(i == regs->end())
                return 0;
        else
                return i->second;
   }
   //設定某 MsgCode 的 Worker, 傳回設定前的 Worker
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
   //----- 建構/解構
   K_ctor  TbxSes  (TbxSesMgr*);
   K_dtor ~TbxSes  ();



   //----- 訊息處理/傳送
   virtual K_mf(bool)  SendPk       (const THead&,TSize);      //您實際要傳送的資料,必須緊接在 THead 之後

   //----- 狀態處理
   enum EState {
      sp_Creating,
      sp_Destroying,
      sp_Error,
      sp_TimeEarly,
      sp_WaitConnected,
      sp_WaitLinkReady,     //等候進入連線子系統
      sp_WaitLoginReady,    //等候登錄
      sp_WaitSubSysReady,   //等候啟動應用子系統
      sp_Ready,             //應用子系統啟動完成
      sp_WaitForOrder,      //等候接受委託
      sp_WaitForOrdReply,   //等候委託回報
      sp_StartRptSys,       //起始成交回報子系統
      sp_WaitForRpt,        //等候成交回報資料
      sp_RecievingRpt,      //接收成交回報資料
      sp_Wait050,           //等待連線確認 
      sp_WaitToResend,      //等待重送委託 
      sp_Signout,           //系統登出
   };

   virtual  K_mf(void)   SetState  (EState newState, const std::string& reason);
   inline  K_mf(EState)  GetState  () const   { return State_; }
   inline K_mf(void)            SetAppendNo(const TAppendNo appno){ AppendNo_ = appno; }
   inline K_mf(TAppendNo)       GetAppendNo() const { return AppendNo_;}
   virtual K_mf(void)    OnWorkerFinished(TPacketCode PkCode,const THead&) = 0;    //Worker工作完畢回傳
   virtual K_mf(void)    OnLinkWorkerFinished(TPacketCode PkCode,const THead&) = 0;    //LinkWorker工作完畢回傳

   //↓建立bxSession 後設定及取用的相關函式
   //每個bxSes 建立後請務必先執行初始化設定BrokerID.Password.PvcID.Market及ApCode
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

   //↓當註冊至bxMgr 後,bxMgr 會給予 Mgr 的pointer
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

   //啟動計時器,當您設定的時間到了,則會使用 OnTimer() 通知您
   virtual K_mf(void)   StartTimer  (size_t ms);
   //停止計時器,不再產生 OnTimer() 的事件
   K_mf(void)           StopTimer   ();

   virtual K_mf(void)  OnMesLinkReady   (TMesBase*);// SetState(sp_WaitConnected)
   virtual K_mf(void)  OnMesLinkBroken  (TMesBase*, const std::string&);//清除接收緩衝區
   K_mf(void)  OnMesLinkFail    (TMesBase*, const std::string&);//SetState(sp_Error);

   //若您有啟動計時器,則會透過 OnTimer() 來告知衍生出來的class
   virtual K_mf(void)  OnTimer           (TTimerBase*);//do nothing
   K_mf(void)  OnRptTimer       (TTimerBase*);
   K_mf(void)  ResetRptTimer    ();
//   virtual K_mf(void)  AddWorkers        (){};
   //當 Mes 有收到資料時,透過 OnMesRecvReady() 通知
   //  在此處會接收一個完整訊息,並呼叫 Worker 處理
   virtual K_mf(void)  OnMesRecvReady    (TMesBase*, size_t size);//當有資料

   TWorkers                Workers_;
   size_t                  RecvPos_;   //已收到的資料大小
   TCheckResult            PkResult_;
   size_t                  SkipBytes_;
   TbxRec                  RecvPk_[2]; //RecvPk_[1]作保護用!
   EState                  State_;
   Kway::word              FailCount_;
   TSubSysName             SubSys_;
   TTimer<this_type>       Timer_;
   TTimer<this_type>       RptTimer_;
   TReqPtr                 ReqPtr_;
   TMarket                 Market_;             //Ses 所屬市場別
   TbxSesMgr*              MgrPtr_;             //管理的bxMgr
   bool                    IsFix_; 
   TApCode                 ApCode_;
   TBrkID                  BrokerID_;
   TAppendNo               AppendNo_;
   TSize                   PassWord_;
   TPvcID                  PvcID_;
   std::string             SettingStr_;         //設定參數
   std::string             SesTermID_;
//   TSize                   FailCount_;          //連續Link Fail的次數
};
/////////////////////////////////////////////////////////////////////////////
} // namespace bx
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif

