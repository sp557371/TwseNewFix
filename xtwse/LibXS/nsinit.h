/** @file nsinit.h
@brief libns���Ұʤε�������
*/
//---------------------------------------------------------------------------
#ifndef nsinitH
#define nsinitH
//---------------------------------------------------------------------------
#include "KwayList.h"
//---------------------------------------------------------------------------
namespace Kway {
/////////////////////////////////////////////////////////////////////////////
LIBNS_class TNSIniter;
LIBNS_class TNSCleanup;
//---------------------------------------------------------------------------
/** @brief �Nlibns��l��.

�b�z�ϥΥ���@�� libns �����󤧫e, �z�������إߤ@�� TNSIniter,
�p�G�z���T�w�O�_�w�g���H�إߤF TNSIniter �]�S���Y, �z�i�H���ƫإ�,
�]���u����̫�@�� TNSIniter �Ѻc��, �~�|�N���� libns ���M�z�{��.
*/
LIBNS_class TNSIniter
{
   friend class TNSCleanup;
   class THolder;
   static K_mf(THolder&)  GetHolder  ();
   K_mf(void)  Init  ();
public:
           K_ctor  TNSIniter  (int argc, char* argv[]);
           K_ctor  TNSIniter  ();
   virtual K_dtor ~TNSIniter  ();

   enum TNSState {
      ns_Running,
      ns_Cleaning,
      ns_Cleaned,
   };
   static K_mf(TNSState)  GetNSState  ();
};
//---------------------------------------------------------------------------
/** @brief libns���M�z�{��.

��̫�@�� TNSIniter ������, �|�I�s�Ҧ��� TNSCleanup �� OnNSCleanup(),
�ҥH�p�G�z�n�b libns �����e����@�ǲM�z���u�@��, �u�n�~�� TNSCleanup,
�M���мg OnNSCleanup() �N�i�H�F��ت��F.
�V�߫غc�� TNSCleanup, �|�V������ OnNSCleanup().
*/
LIBNS_class TNSCleanup : public TNodeBase
{
   friend class TNSIniter;
   friend class TNSIniter::THolder;
   virtual K_mf(void) OnNSCleanup   () = 0;
   virtual K_mf(void) OnNSRestartup () {};

   inline K_mf(TNSCleanup*)  GetNext  ()  { return static_cast<TNSCleanup*>(TNodeBase::GetNext()); }
protected:
           K_ctor  TNSCleanup  ();
   virtual K_dtor ~TNSCleanup  ();
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Kway
//---------------------------------------------------------------------------
#endif
