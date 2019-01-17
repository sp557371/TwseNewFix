#ifndef DspMgrH
#define DspMgrH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include <set>
#include "DspDef.h"
#include "FTExtDef.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
//---------------------------------------------------------------------------
K_class TDispatchMgr/* : public TFileDispatch*/
{
    typedef std::set<const TFileDispatch*>  TDispatchSet;
    typedef TDispatchSet::iterator          TDispatchSetI;
    typedef TDispatchSet::const_iterator    TDispatchSetCI;

    TDispatchSet    DispatchSet_;
    const TFileGet* FileGet_;
    TFTSenderBase*  FTSender_;
public:
        K_ctor  TDispatchMgr() : FileGet_(NULL) {FTSender_ = 0;}
        K_dtor ~TDispatchMgr() {}
        K_mf(bool) DoReg(const TFileDispatch*);
        K_mf(TFTSenderBase*) DoReg(const TFileDispatch*,bool);
        K_mf(bool) UnReg(const TFileDispatch*);
        K_mf(bool) DoReg(const TFileGet*);
        K_mf(bool) UnReg(const TFileGet*);
        K_mf(void) DoDispatch(const TFTJob&, const std::string& fname) const;
        K_mf(bool) GetSource (const TFTJob&, const std::string& src, const std::string& dst) const;
        K_mf(std::string) GetDLCmd (const TFTJob&, const std::string& fname) const;
        K_mf(std::string) GetULCmd (const TFTJob&, const std::string& src, const std::string& dst) const;
        K_mf(std::string) Get3TLocalPath() const;
        K_mf(void) SetSender(TFTSenderBase* fts){FTSender_ = fts;};
        K_mf(TFTSenderBase*) GetSender(){return FTSender_;};
};
//---------------------------------------------------------------------------
K_fn(const TDispatchMgr&) GetDispatchMgr();
K_fn(const TDispatchMgr&) GetDispatchMgr(TFTSenderBase*);
K_fn(void) WriteSystemFail  (const std::string&);
K_fn(void) WriteSettingError(const std::string&);
K_fn(void) WriteSystemCmd   (const std::string&);
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif

