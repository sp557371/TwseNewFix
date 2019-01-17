/** @file EvDev.h

@brief UNIX���x,������i���ƥ�B�z�覡.

Solaris: /dev/poll
Linux:   epoll
FreeBSD  kqueue

@author ���H fonwin@ms21.hinet.net
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
/** ��fn��, �l�ͦ� TEventsDev �� class �w�q����.
    �Ҧp EvDev_devpoll.cpp
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
   /** �Ǧ^ str.empty() ��� FD_ ready, �_�h�Ǧ^ MakeErrStr(msg, errno);
   */
   K_mf(std::string)  CheckReadyFD  (const char* msg);

   /** �]�w fd, �۰ʳ]�� O_NONBLOCK.
       �Ǧ^ IsReadyFD()
   */
   K_mf(bool) SetFD (TFD);

   /** ���� FD.
   */
   virtual K_mf(void) CloseFD ();

   /** �Ǧ^�{�b��FD,�ñNFD�]���L��.
       �����������ʧ@.
   */
   virtual K_mf(TFD) ReleaseFD ();

   /** ���oFD.
   */
   inline K_mf(TFD) GetFD () const  { return FD_; }

public:
   /** �غc. �Y fd ����, �۰ʳ]�� O_NONBLOCK.
   */
   K_ctor  TAutoFD  (TFD = -1);

   /** �Ѻc. �۰����� FD_.
   */
   virtual K_dtor ~TAutoFD  ();

   /** FD�O�_����.
   */
   inline K_mf(bool) IsReadyFD () const   { return FD_ >= 0; }
};
//---------------------------------------------------------------------------
/** �۰ʳB�z�P EventsDev �����ƶ�.
*/
LIBNS_class TAutoEvFD : public TAutoFD
                      , public TEventItem
{
   TEventsDev* EvDev_;

protected:
   /** ���� FD.
   */
   virtual K_mf(void) CloseFD ();

   /** ���o EvDev �ƥ󤤤�.
   */
   inline K_mf(TEventsDev*) GetEvDev ()   { return EvDev_; }

public:
   /** �غc.
   */
   K_ctor TAutoEvFD (TFD fd = -1)    : TAutoFD(fd), EvDev_(NULL) {}

   /** �Ѻc.
   */
   virtual K_dtor ~TAutoEvFD ();

   /** �q�Y�� evDev ���o�ƥ�.
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

