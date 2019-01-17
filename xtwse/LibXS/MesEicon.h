//---------------------------------------------------------------------------
#ifndef MesEiconH
#define MesEiconH
//---------------------------------------------------------------------------
#include "Mes.h"
#include "MesRegs.h"
#include "MesEiconService.h"
//---------------------------------------------------------------------------
using namespace Kway;
//---------------------------------------------------------------------------
class TMesEiconService;
//---------------------------------------------------------------------------
K_class TMesEicon : public TMesBase
{
   typedef TMesBase  inherited;
   friend class TMesEiconService;
public:
   K_ctor  TMesEicon  ();
   K_dtor ~TMesEicon  ();

   K_mf(bool)         SettingApply  (const std::string&);
   K_mf(std::string)  GetSetting    ();
   K_mf(bool)         Open          ();
   K_mf(bool)         Close         ();
   K_mf(int)          Send          (const void* buffer, size_t size);
   K_mf(int)          Recv          (      void* buffer, size_t size);

   static K_mf(TMesReg&)  GetReg  ();

private:
   word              LineNo_;
   word              PvcNo_;
   TMesEiconService* Service_;
};
////////////////////////////////////////////////////////////////////////////
#endif
