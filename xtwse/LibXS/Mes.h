//
// 通訊classes基礎: Session-Messager
//    class TMesBase;
//    class TMesEvHandler;
//
// 版本: $Id: Mes.h,v 1.10 2005/06/26 11:18:50 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef MesH
#define MesH
//---------------------------------------------------------------------------
#include <string>
#include "ref_counter.h"
#include "wise_ptr.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
// Communication Messager
// 更新紀錄:
// ● 920704:
//   1.      int  Send  (void* buffer, size_t size)
//      改成 int  Send  (const void* buffer, size_t size) = 0;
//   2. 在需要的地方,增加 inline 修飾字
//   3. 增加 TMesEvHandler::GetClientEvHandler() 事件
//           TMesBase::GetClientEvHandler()
//   4. TMesBase建構時,將 IsLinkReady_ 設為 false
//
// ● 920709
//   1.增加 OnMesBfDestroy () 事件
//   2.將   TMesEvHandler& EvHandler_;
//     改成 TMesEvHandler* EvHandler_;
//     並在呼叫事件時,判斷 EvHandler_ 是否為 0
//   3.增加 public:  TMesEvHandler* GetEvHandler ();
//                   void           SetEvHandler (TMesEvHandler*);
//   4.將   TMesBase::TMesBase(TMesEvHandler&);
//     改成 TMesBase::TMesBase()不帶參數
//   5.增加 TMesEvHandler::OnMesLeave() 事件
//          TMesEvHandler::OnMesEnter() 事件
//   6.增加 K_mf() K_ctor K_dtor
//   7. OnClientConnect() 傳回 bool 代表成功或失敗,若傳回false,則 clientMes 不可再使用!
// ●920716
//   增加 TSyncObj 機制
//   將 const char* 改成 const std::string&
// ●920804
//   取消 virtual K_mf(bool)  IsListenServer  ();
//   改由註冊時指定
// ●921225
//   改成 ref_counter_ns
//---------------------------------------------------------------------------
LIBNS_class TMesBase;
LIBNS_class TMesNoThread;
//---------------------------------------------------------------------------
enum TMesStateLevel
{  //如何決定Warning,Error,Fatal,請由撰寫Mes的人自行研判,但需提供文件說明
   mslNormal,     //一般狀況,例如:由[連線中]變成[連線Ready]
   mslWarning,    //可能有異常,但不嚴重,可由Mes自行解決
   mslError,      //一般錯誤,但不嚴重,可由Mes自行解決
   mslFatal,      //嚴重的錯誤,需人工介入處理
};
enum EMesLinkState {
   mls_Initing,
   mls_Opening,
   mls_LinkReady,
   mls_ListenReady,
   mls_Closing,
   mls_Closed,
   mls_LinkError,
   mls_Destroying,
};
//---------------------------------------------------------------------------
LIBNS_class TMesEvHandler
{
// PS.底下說明中的[遠端]只是一個概念,例如:
//    1.若開啟的Mes為Local電腦上的RS232 port,若開啟成功就算是與[遠端]連線完成
//    2.若開啟的是一個MesModem:
//      則必須撥號並與遠端的Modem溝通完畢,且可開始收送資料,才算是與遠端連線完成
friend class TMesBase;
friend class TMesNoThread;
//連線開啟後,與遠端連線完成,可開始收送資料
//  接續MesModem的例子:若與遠端的Modem溝通完畢,已可開始收送資料時呼叫此Event
virtual K_mf(void)  OnMesLinkReady   (TMesBase*) = 0;
//連線開啟後,無法與遠端連線
//  接續MesModem的例子:若與遠端忙線、Modem溝通失敗...則呼叫此Event
virtual K_mf(void)  OnMesLinkFail    (TMesBase*, const std::string& reason) = 0;
//已經連線,之後的斷線
//  接續MesModem的例子:雙方Modem已經連線Ready,但電話斷線...則呼叫此Event
virtual K_mf(void)  OnMesLinkBroken  (TMesBase*, const std::string& reason) = 0;

//傳送緩衝區資料已送完
virtual K_mf(void)  OnMesSendEmpty  (TMesBase*) { }
//接收緩衝區已有資料,但若Mes不支援預先提供已收到的資料量,則size填入0
virtual K_mf(void)  OnMesRecvReady  (TMesBase*, size_t /*size*/) { }

//當Mes的狀態改變了,請由此處通知
virtual K_mf(void)  OnMesStateChanged (TMesBase*, TMesStateLevel, const std::string& /*stateMsg*/) { }
virtual K_mf(void)  OnMesSetRemark    (TMesBase*, const std::string& /*remarkMsg*/) { }

//若為Mes為 ListenServer:
// (1) 先取得一個 EvHandler, 若傳回 NULL, 則表示此 Handler 不處理 Client 連線的事件
//     請拒絕 Client 的連線
virtual K_mf(TMesEvHandler*)  GetClientEvHandler (TMesBase* mesServer)  { return NULL; }
// (2) 透過此事件通知,有Client連線上來了
virtual K_mf(bool)  OnMesClientConnect  (TMesBase* mesServer, TMesBase* mesClient)  { return false; }

//當Mes解構時,需要呼叫此事件:
virtual K_mf(void)  OnMesBfDestroy (TMesBase*)  {}

//當Mes與 MesEvHandler [脫離關係/建立關係]時,會呼叫此事件:
virtual K_mf(void)  OnMesLeave (TMesBase*)   {}
virtual K_mf(void)  OnMesEnter (TMesBase*)   {}

public:
virtual K_dtor  ~TMesEvHandler  ()  {}
};
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TMesBase : public Kstl::ref_counter<>
{
   EMesLinkState  LinkState_;
   TMesEvHandler* EvHandler_;
   class TMesSyncChange;
   friend class TMesSyncChange;
   friend class TMesNoThread;
public:
   //建構 & 解構
           K_ctor  TMesBase ()  : LinkState_(mls_Initing), EvHandler_(0)  { }
protected:
   virtual K_dtor ~TMesBase ()  { OnBfDestroy(); }

public:
   typedef Kstl::auto_ref<TMesBase>   TLocker;
   typedef Kstl::wise_ref<TMesBase>   TMesPtr;

   //判斷連線是否Ready
   inline  K_mf(bool)           IsLinkReady  ()          { return LinkState_ == mls_LinkReady; }
   inline  K_mf(EMesLinkState)  GetLinkState () const    { return LinkState_;   }

   //此Mes是否會產生 OnMesClientConnect() 事件?
   //virtual K_mf(bool)  IsListenServer  ()  { return false; }

   //設定/取出Mes的參數(Mes的設定,請用標準文數字及符號表示,且需提供文件說明)
   virtual K_mf(std::string)  GetSetting   ()                     { return std::string(); }
   virtual K_mf(bool)         SettingApply (const std::string&)   { return true; }

   //開啟/關閉連線
   virtual K_mf(bool)  Open         ();
   virtual K_mf(bool)  Close        ();
   virtual K_mf(bool)  CloseReason  (const std::string& reason);//呼叫 OnSetRemark(reason), SetLinkError(reason), Close();

   //傳送/接收資料
   virtual K_mf(int)  Send  (const void* buffer, size_t size) = 0;
   virtual K_mf(int)  Recv  (      void* buffer, size_t size) = 0;

   //↓控制[傳送/接收]緩衝區的大小
   virtual K_mf(bool)  GetBufSize  (size_t& sendBufSz, size_t& recvBufSz)  { return false; }
   virtual K_mf(bool)  SetBufSize  (size_t  sendBufSz, size_t  recvBufSz)  { return false; }

   //↓取得現在[傳送/接收]緩衝區,有多少Bytes的資料等待處理,傳回-1表示Mes不提供此特性
   virtual K_mf(int)  GetSendSize  ()  { return -1; }
   virtual K_mf(int)  GetRecvSize  ()  { return -1; }

   //↓取得此Mes的識別代號,例:若為Local RS232= "dev/COM1"
   //                         若為TCP  Client= "ip/127.0.0.1"
   //                         若為TCP  Server= "ip/listen:5000"
   //                         ...以上僅為舉例,實際傳回值只要可以看得懂即可
   virtual K_mf(std::string)  GetUID  () { return std::string(); }
   //↓最後資料從哪個UID來(適用於UDP...之類的廣播接收Mes)
   virtual K_mf(std::string)  GetLastUID  () { return GetUID(); }

   inline K_mf(TMesEvHandler*)  GetEvHandler  ()  { return EvHandler_; }
          K_mf(void)            SetEvHandler  (TMesEvHandler* newEvHandler);
protected:
   //當Mes解構時呼叫此事件
   K_mf(void)  OnBfDestroy  ();
   //當與遠端連線完成,請呼叫此method
   K_mf(void)  SetLinkReady ();
   //當Server端開始等候連線.
   K_mf(void)  StartListen  ();
   //當[連線不成功] or [斷線],請呼叫這裡的method
   K_mf(void)  SetLinkError  (const std::string& reason);
   //當緩衝區的資料已傳送完畢,請呼叫這裡的method
   K_mf(void)  OnSendEmpty  ();
   //當接收緩衝區已有資料,請呼叫這裡的method
   //若Mes不支援預先提供已收到的資料量,則size填入0
   K_mf(void)  OnRecvReady  (size_t size);
   //當Mes有狀態改變了,請從這兒通知
   K_mf(void)  OnStateChanged  (TMesStateLevel msLevel, const std::string& stateMsg);
   K_mf(void)  OnSetRemark     (const std::string& remarkMsg);
   //當Mes為 ListenServer 時,若有Client連線則會產生底下的事件
   K_mf(TMesEvHandler*)  GetClientEvHandler ();
   K_mf(bool)            OnClientConnect    (TMesBase* mesClient);

private:
   virtual K_mf(bool)  OpenImpl   () = 0;
   virtual K_mf(bool)  CloseImpl  () = 0;
};
//---------------------------------------------------------------------------
LIBNS_class TMesNoThread : public TMesBase
{
   typedef TMesBase  inherited;
protected:
   inline K_mf(void)  OnSendEmpty     ()            { if(EvHandler_)  EvHandler_->OnMesSendEmpty(this);    }
   inline K_mf(void)  OnRecvReady     (size_t sz)   { if(EvHandler_)  EvHandler_->OnMesRecvReady(this,sz); }
   inline K_mf(void)  OnStateChanged  (TMesStateLevel msl, const std::string& stateMsg)
                                                    { if(EvHandler_)  EvHandler_->OnMesStateChanged(this, msl, stateMsg); }
   inline K_mf(void)  OnSetRemark     (const std::string& remarkMsg)
                                                    { if(EvHandler_)  EvHandler_->OnMesSetRemark(this, remarkMsg); }
};
//---------------------------------------------------------------------------
typedef TMesBase::TMesPtr  TMesPtr;
/////////////////////////////////////////////////////////////////////////////
}; // namespace Kway
#endif

