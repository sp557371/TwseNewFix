#ifndef FTFlowH
#define FTFlowH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include "FTJobDef.h"
#include "FTJobMap2d.h"
#include "NotifyMgr.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
//---------------------------------------------------------------------------
K_class TFTFlowHandler  // �[�J�u�@�Ƶ{�� Virtual Function
{
public:
    virtual K_mf(TSysID) DoReq(const TFTNewJob&) = 0;
};
//---------------------------------------------------------------------------
K_class TFTNotifyHandler    // �q�������� Virtual Function
{
protected:
            TSenderMgr* SenderMgr_;
            bool        Ready_;
            TEnvMode    EvMode_;
public:
            K_ctor TFTNotifyHandler(TSenderMgr* mgr) : SenderMgr_(mgr),Ready_(false) {
               EvMode_.assignT(evm_2T); // 20090213 �w�]����h�[�c, �T�h���q��ses�������wevm_3T
            }
            K_mf(void) Unlink  () { SenderMgr_ = NULL; }
    virtual K_mf(bool) DoNotify(const TFTJob&) = 0;
    virtual K_mf(bool) IsReady() {return Ready_;}
    virtual K_mf(void) SendFileCmd   (bool, int, std::string&) = 0;
            K_mf(TEnvMode) GetEvMode(){return EvMode_;}
};
//---------------------------------------------------------------------------
K_class TFTCenterHandler    // �[�J�e�U���ߪ� Virtual Function
{
public:
    virtual K_mf(void) DoSend  (const TBrkID&, const TMarket&, const TSysID&) = 0;
};
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif
