//
// �P�@��{�������P thread, �����^�� main thread �ӦP�B
//    �z�����b�{����l�Ʈ�, �I�s TSyncObj::Initial();
//    �Y�z�� busy loop, �ЩI�s   TSyncObj::CheckSync();
//
// ����: $Id: SyncEv.h,v 1.8 2006/04/04 03:51:18 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SyncEvH
#define SyncEvH
//---------------------------------------------------------------------------
#include "nsinit.h"
//---------------------------------------------------------------------------
//#include <cc++/thread.h>
namespace ost {
   class Thread;
}
//---------------------------------------------------------------------------
//�b�D���������@�Өƥ�!
//---------------------------------------------------------------------------
namespace Kway {
LIBNS_class TInpHandler;
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TSyncObj
{
   class TSyncHandle;
   class TSyncWorker;
   friend class TSyncWorker;
   friend class TNSIniter;
   static K_mf(TSyncWorker&) GetSyncWorker  ();
   static K_mf(void)         InitSyncWorker ();
   #ifdef K_UNIXLIKE
      static K_mf(int)  GetSyncFD ();
   #endif

public:
   enum TState
   {
      ssWaitting,
      ssExecing,
      ssExecuted,
      ssAbort,
      ssIdle,
      ssDestroying,
   };

private:
   TState   State_;

public:
   inline  K_ctor  TSyncObj ()  : State_(ssIdle) {}
   virtual K_dtor ~TSyncObj ();

   //�קK�~�Ӫ̤w��,�����n���� Sync(), �Y Sync()=0, �|�y�� pure virtual function.
   //�ҥH���B�����n���@�Ӥ�������Ʊ��� Sync();
   virtual K_mf(void)  Sync  ();

   inline K_mf(TState)  GetState  () const  { return State_; }

   //�^��D��������� operator(),
   //  ���槹��~��^: true
   //  �L�k�^��D���������: false
   K_mf(bool)  operator()  ();
   K_mf(bool)  WaitSync    (bool wait);

   // 93/08/01����,�Шϥ� nsinit.h �� libns [�Ұ�/����]����.
   // static K_mf(void)  Initial  ();   //�аȥ��b�z���D�{���ҰʮɩI�s�� function

   static K_mf(void)  CheckSync  ();   //�Цb�z�� busy loop �����I�s�� function (�i�b���N Thread ���I�s)

   //�� �Цb AppRun() ���e, ���I�s Initial()
   //�Y�z�� unix(Windows Console) �{��, �q main() �}�l, �ӥB�S���O���Ʊ��n���F,
   //�i���� AppRun() �ӱN�{���ܦ� demon (TInpHandler*=0, �N�|�ܦ�daemon�F), �ä��_���B�z Sync �ƥ�,
   static K_mf(int)   AppRun      (int (*MainInit) (), int argc, char** argv, TInpHandler*);
   //�Y�z�n�ۤv����{�����y�{,�Цb MainThread��, �A���a��I�s ProcessSync()
   static K_mf(void)  ProcessSync ();
   //���� AppRun(), AppQuit()���|��^, ���Ӱ����Ʊ�������, �{���|�qAppRun()��^!
   static K_mf(void)  AppQuit ();
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif

