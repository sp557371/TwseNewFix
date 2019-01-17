//---------------------------------------------------------------------------
#ifndef SesMgrRH
#define SesMgrRH
//---------------------------------------------------------------------------
#include "SesMgr.h"
#include "KFile.h"
//---------------------------------------------------------------------------
namespace Kway {
//---------------------------------------------------------------------------
K_class TSesMgrR : public TSesMgr
{
private:
   TFile                   Log_;
protected:
   K_mf(void) OnSettingChg (ESettingChg sc, dword settingID, const TSettingRec& rec);
public:
   K_ctor                  TSesMgrR(const char*);
   K_dtor                  ~TSesMgrR();
};
//---------------------------------------------------------------------------
};//namespace Kway
#endif
