#ifdef __BORLANDC__
  #pragma hdrstop
  #pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#include "FTExtDef.h"
//---------------------------------------------------------------------------
using namespace Kway;
using namespace Kway::Tw::Bx;
using namespace Kway::Tw::Bx::FT;
//---------------------------------------------------------------------------
static TFTSenderBase*  FTSender_;
//---------------------------------------------------------------------------
K_fn(void) SetFTSender(TFTSenderBase* fts)
{
	FTSender_ = fts;
}
//---------------------------------------------------------------------------
K_fn(TFTSenderBase*)  GetFTSender()
{
	return FTSender_;
}
//---------------------------------------------------------------------------
K_ctor TFTSenderBase::TFTSenderBase()
{
   FTSender_ = this;
}
//---------------------------------------------------------------------------
