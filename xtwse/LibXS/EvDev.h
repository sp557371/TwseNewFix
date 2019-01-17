/** @file EvDev.h

@brief UNIX平台,比較先進的事件處理方式.

Solaris: /dev/poll
Linux:   epoll
FreeBSD  kqueue

@author 風人 fonwin@ms21.hinet.net
*/
#ifndef EvDevH
#define EvDevH
/////////////////////////////////////////////////////////////////////////////
#include "KwayBase.h"
#include <cc++/thread.h>
#include <string>
#include <deque>
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
enum EEvMask
{
   ev_None   = 0x00,
   ev_Read   = 0x01,
   ev_Write  = 0x02,
   ev_RdWr   = ev_Read|ev_Write,
};
//---------------------------------------------------------------------------
enum EEvResult
{
   er_Continue,
   er_Remove,
   er_Delete,
};
//---------------------------------------------------------------------------
class TEventsDev;
typedef int    TFD;
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TEventItem
{
public:
   virtual K_dtor ~TEventItem () {}

   virtual K_mf(EEvResult)  OnEvRead  (TFD, TEventsDev*) = 0;
   virtual K_mf(EEvResult)  OnEvWrite (TFD, TEventsDev*) = 0;
};
//---------------------------------------------------------------------------
LIBNS_class TEventsDev : public ost::Mutex
{
  bool                     IsQuit_;
  std::deque<TEventItem*>  Evs_;

  virtual K_mf(bool)  AddImpl  (TFD, EEvMask, TEventItem* newItem, TEventItem* oldItem) = 0;
  virtual K_mf(bool)  DelImpl  (TFD, TEventItem*) = 0;

   K_mf(EEvResult)  CheckEvResult  (TFD, TEventItem*, EEvResult);

protected:
  K_mf(void)  DoEvent  (TFD, EEvMask);

public:
  inline  K_ctor  TEventsDev () : IsQuit_(false)  {}
  virtual K_dtor ~TEventsDev () {}

  virtual K_mf(bool)  Run  () = 0;
  virtual K_mf(void)  Quit () { IsQuit_ = true; }
          K_mf(bool)  Add  (TFD, EEvMask, TEventItem*);
          K_mf(bool)  Del  (TFD, TEventItem*);

  inline K_mf(bool) IsQuit () const    { return IsQuit_; }
};
//---------------------------------------------------------------------------
/** 此fn由, 衍生自 TEventsDev 的 class 定義完成.
    例如 EvDev_devpoll.cpp
         EvDev_select.cpp
*/
LIBNS_fn(TEventsDev&)  GetGlobalEvDev  ();
//---------------------------------------------------------------------------
LIBNS_fn(std::string)  MakeErrStr  (const char* str, int eno);
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TAutoFD
{
   TFD   FD_;

protected:
   /** 傳回 str.empty() 表示 FD_ ready, 否則傳回 MakeErrStr(msg, errno);
   */
   K_mf(std::string)  CheckReadyFD  (const char* msg);

   /** 設定 fd, 自動設成 O_NONBLOCK.
       傳回 IsReadyFD()
   */
   K_mf(bool) SetFD (TFD);

   /** 關閉 FD.
   */
   virtual K_mf(void) CloseFD ();

   /** 傳回現在的FD,並將FD設為無效.
       不做關閉的動作.
   */
   virtual K_mf(TFD) ReleaseFD ();

   /** 取得FD.
   */
   inline K_mf(TFD) GetFD () const  { return FD_; }

public:
   /** 建構. 若 fd 有效, 自動設成 O_NONBLOCK.
   */
   K_ctor  TAutoFD  (TFD = -1);

   /** 解構. 自動關閉 FD_.
   */
   virtual K_dtor ~TAutoFD  ();

   /** FD是否有效.
   */
   inline K_mf(bool) IsReadyFD () const   { return FD_ >= 0; }
};
//---------------------------------------------------------------------------
/** 自動處理與 EventsDev 相關事項.
*/
LIBNS_class TAutoEvFD : public TAutoFD
                      , public TEventItem
{
   TEventsDev* EvDev_;

protected:
   /** 關閉 FD.
   */
   virtual K_mf(void) CloseFD ();

   /** 取得 EvDev 事件中心.
   */
   inline K_mf(TEventsDev*) GetEvDev ()   { return EvDev_; }

public:
   /** 建構.
   */
   K_ctor TAutoEvFD (TFD fd = -1)    : TAutoFD(fd), EvDev_(NULL) {}

   /** 解構.
   */
   virtual K_dtor ~TAutoEvFD ();

   /** 從某個 evDev 取得事件.
   */
   K_mf(bool) Attach (TEventsDev&, EEvMask);
};
//---------------------------------------------------------------------------
LIBNS_class TStdin : public TAutoFD
                   , public TEventItem
{
   typedef TAutoFD   inherited;
   std::string       Buffer_;

   virtual K_mf(EEvResult)  OnCmdLine (char*, TEventsDev*) = 0;

public:
   K_ctor  TStdin  ();
   K_dtor ~TStdin  ();

   K_mf(EEvResult)  OnEvRead  (TFD, TEventsDev*);
   K_mf(EEvResult)  OnEvWrite (TFD, TEventsDev*);
   K_mf(bool)       Attach    (TEventsDev&);
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

