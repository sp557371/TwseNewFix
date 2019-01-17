//---------------------------------------------------------------------------
#ifndef ORBaseH
#define ORBaseH
//---------------------------------------------------------------------------
#include "TwStkOrder.h"
#include "TwStkOrdFlow.h"
#include "ExtDef.h"
//---------------------------------------------------------------------------
class TORBase 
{
private:
protected:	// User declarations
   typedef Kway::Tw::Stk::TOrdCenter        TOrdCenter;
   typedef Kway::Tw::Stk::TOrdFlow          TOrdFlow;
   typedef Kway::Tw::Stk::TMarket           TMarket;
   typedef Kway::Tw::Stk::TOrdFlow::TReqPtr TReqPtr;
   TOrdCenter*             OrdCenter_;        // ¶��Ҳժ�OrdCenter
   TOrdFlow*               OrdFlow_;          // ¶��Ҳժ�OrdFlow
   TStkLines               LineState_;        // �Ҧ��i¶��D�����u�����p�[�`
public:		
           K_ctor          TORBase      (){};
           K_dtor          ~TORBase     (){};
   virtual K_mf(bool)      SendOrd      (TReqPtr&, TMarket mkt, ETradeKind tk = tk_Normal, int sz = -1)
                                                                           {return false;} // �s�e�U��F�᪺�@�~
   virtual K_mf(void)      SetState     (TStkLines ls){};                 // �������u���ƶq���ܮɩҵo�X���q��
   virtual K_mf(Kway::word) GetLineState (TMarket m, ETradeKind tk){      // ���o�Ҧ�¶�����Ҧ^�����i�νu���ƶq
   	                                   return LineState_.GetL(m,tk);}
   virtual K_mf(bool)      IsUseORD     (){return true;}                  // �O�_�i�����@�~
   virtual K_mf(bool)      IsUseORM     (){return true;}                  // �O�_�i�i���_�u¶��@�~
   virtual K_mf(void)      ResetScheduler() {};
};
/////////////////////////////////////////////////////////////////////////////
#endif
