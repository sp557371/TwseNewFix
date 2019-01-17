//---------------------------------------------------------------------------
#ifndef ExtFlowsH
#define ExtFlowsH
//---------------------------------------------------------------------------
#include "TwStkOrdFlow.h"
#include "TwStkOrdCenter.h"
#include "ExtDef.h"
#include "ViewAreaMgr.h"
//---------------------------------------------------------------------------
/*
�o�ӼҲլO�]�p�ΨӸ��J�Ȼs�Ʀw���y�{, �åi�H�ɥ[���Ψ����Ȼs�ƪ��w���Ҳ�
�n�ƥ��o�Ӫ���[�JOrdFlow ����
*/
namespace Kway{
namespace Tw{
namespace Stk{
//---------------------------------------------------------------------------
enum EReturnValue{
	erv_Success      = 0, // �ˬd�y�{�ߧY�^��success
	erv_Fail         = 1, // �ˬd�y�{�ߧY�^�Х���
	erv_WaitResponse = 2, // �i��~���ˬd�y�{, �ݵ��ݦ^�Ы�A�I�s��^��Member Function
};
//---------------------------------------------------------------------------
class TExtFlows;
K_class TFlowBase
{
	Kway::word       StepID_;
protected:
	TExtFlows*       Flows_;
public:
   virtual K_mf(EReturnValue)     DoOrdReq(TOrdFlow::TReqPtr&) = 0;
           K_mf(void)             SetStepID(Kway::word id){StepID_ = id;}
           K_mf(Kway::word)       GetStepID(){return StepID_;}
           K_mf(void)             SetFlows(TExtFlows* ef){Flows_ = ef;}
};
//---------------------------------------------------------------------------
K_class TExtFlows :public  TOrdFlow::TFlowStep
{
private:
   typedef std::vector<TFlowBase*>  TFlows; 
protected:
   TFlows                           Flows_;
   Kway::word                       Steps_;
   Kway::TViewAreaMgr               ScVA_;
public:
           K_ctor     TExtFlows() : Steps_(0){};
   virtual K_mf(bool) AddFlowItem (TFlowBase* fb){
   	                              if(Flows_.size() > 0)
   	                                 for(TFlows::iterator L = Flows_.begin(); L != Flows_.end(); L++){
   	                                    if((*L) == fb)
   	                                       return false;
   	                                 }
   	                              fb->SetStepID(++Steps_);
   	                              fb->SetFlows(this);
   	                              Flows_.push_back(fb);
   	                              return true;
                                   }
   virtual K_mf(void)  DelFlowItem(TFlowBase* fb){
   	                              if(Flows_.size() > 0)
   	                                 for(TFlows::iterator L = Flows_.begin(); L != Flows_.end(); L++){
   	                                    if((*L) == fb){
   	                                       Flows_.erase(L);
   	                                       return;
   	                                    }
   	                                 }
                                   }
   virtual K_mf(bool)  OnOrdReq   (TOrdFlow::TReqPtr req){
   	                              return DoSteps(req,0);
   	                           }
   virtual K_mf(bool)  DoSteps (TOrdFlow::TReqPtr req, Kway::word stepid){
   	                              if(Flows_.size() > 0)
   	                                 for(TFlows::iterator L = Flows_.begin(); L != Flows_.end(); L++){
   	                                 	if((*L)->GetStepID() <= stepid)
   	                                 	   continue;
   	                                    std::string msg;
   	                                    EReturnValue rtn = (*L)->DoOrdReq(req);
   	                                    switch(rtn){
   	                                       case erv_Fail:
   	                                          return false;
   	                                       case erv_WaitResponse:
   	                                          return false;
   	                                       default:   break;
   	                                    }
   	                                 }
   	                              DoSendOrd(req);
   	                              return true;
   	                           }
   virtual K_mf(void)  DoSendOrd(TOrdFlow::TReqPtr req){};
   virtual K_mf(Kway::TViewAreaMgr*) GetVA(){return &ScVA_;}
};
//---------------------------------------------------------------------------
};
};
};
//---------------------------------------------------------------------------
K_fn(Kway::Tw::Stk::TExtFlows*) GetExtFlows();
K_fn(void) SetExtFlows(Kway::Tw::Stk::TExtFlows*);
/////////////////////////////////////////////////////////////////////////////
#endif
