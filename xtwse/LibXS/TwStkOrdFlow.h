//
// TwStk �� [�e�U�y�{]
//
// ����: $Id: TwStkOrdFlow.h,v 1.5 2006/04/04 03:32:46 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef TwStkOrdFlowH
#define TwStkOrdFlowH
/////////////////////////////////////////////////////////////////////////////
#include "TwStkOrdCenter.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace Stk {
//---------------------------------------------------------------------------
TWSTK_class  TTrProfile; // TwStkTrProfile.h
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TOrdFlow : public TKeyObject {
public:
   explicit K_ctor  TOrdFlow  (const std::string& logPath, TOrdCenter&);

   //Types.
   typedef Kway::Tw::Stk::TOrdCmdID    TOrdCmdID;  // �e�U�n�D�N��(0=�s�e�U,1=��q...)
   typedef TOrdCenter::TOrdHnd         TOrdHnd;
   typedef Kway::Tw::Stk::TTrProfile   TTrProfile;

   //�e�U�n�D����--------------�� TReq-----------------------
   TWSTK_class TReqBase {
   public:
#ifdef _MSC_VER
      inline K_ctor TReqBase() {}
#endif
      inline  K_mf(bool)        IsFinish     () const  { return FlowID_.IsAllFinish(); }
      inline  K_mf(TOrdFlow*)   GetOrdFlow   () const  { return OrdFlow_;    }
      inline  K_mf(TOrdFlowID)  GetOrdFlowID () const  { return FlowID_;     }
      inline  K_mf(TOrdCmdID)   GetOrdCmdID  () const  { return OCmd_;       }
      inline  K_mf(TTrProfile&) GetTrProfile () const  { return *TrProfile_; }
      inline  K_mf(TOrdHnd&)    GetOrdHnd    ()        { return OrdHnd_;     }
      inline  K_mf(std::string) GetLastMsg   () const  { return LastMsg_;    }
      inline  K_mf(bool)        IsJamStk     () const  { return IsJamStk_;   }
      inline  K_mf(void)        SetIsJamStk  (bool v)  { IsJamStk_ = v;      }

      //��b TTrProfile �Ѻc��,�������]���b���檺Reqs->TrProfile_
      virtual K_mf(void) ClrTrProfile ();

      //��Y�өe�U�y�{�B�B�z�i�@�q��(���@�w�O��Step����), �I�sUpdate()��s�T��.
      virtual K_mf(void) Update (const std::string&, EOrdFlowID);
   protected:
              K_ctor  TReqBase  (TOrdCmdID, TTrProfile*, TSysOrdID);
      virtual K_dtor ~TReqBase  ();

      TOrdCmdID     OCmd_;
      TSysOrdID     SysOrdID_;
      std::string   LastMsg_;
      TOrdFlowID    FlowID_;
      bool          Updating_;
      TTrProfile*   TrProfile_;

      friend class TOrdFlow;  //��}�l�i�J�e�U�y�{��,TOrdFlow�����]�w���������
      TOrdHnd       OrdHnd_;
      TOrdFlow*     OrdFlow_;
      bool          IsJamStk_;   //���Ѳ�, �w�]��false, �浹 TFlowStep �]�w�γB�z.
   };
   typedef wise_ptr<TReqBase>    TReqPtr;
   typedef pod_vector<TReqPtr>   TReqPtrs;
   typedef TReqPtr::ref_obj      TReqRefBase;
   //�e�U�n�D����--------------�s�e�U(�t�j��) TReqNew --------------------
   // TReqXXX �����ϥ� new ���覡�غc, ���i�ϥ�[�ϰ��ܼ�]���覡����!
   // �ҥH�Шϥ� CreateReqXXX(...) �� member function �Ө��o�e�U�n�D����!
   TWSTK_class TReqNewBase : public TReqRefBase {
      typedef TReqRefBase   inherited;
   protected:
      K_ctor  TReqNewBase  (TOrdCmdID, TTrProfile&, TSysOrdID);
      K_ctor  TReqNewBase  (TTrProfile&, TSysOrdID);//=0�s�W,else�j��
      friend class TOrdFlow;
   public:
      //data members
      TOrdFields_ForNew    Ord_;
      TScForces            ScForces_; //�����ި�j���X��(���s���e�U�]�i�H����J�Y�Ǳj���X��,��ܤ����ˬd�Y�Ƕ���)
      dword                ScCond_;   //2006.02.10.��������,�ѭ����d��ɦۦ�w�q��N�q,�w�]��0.
   };
   typedef wise_ref<TReqNewBase> TReqNew;
   static inline K_mf(TReqNew)   CreateReqNew  (TTrProfile& tpf, TSysOrdID id=0)
                                                   { return TReqNew(new TReqNewBase(tpf,id)); }
   //�e�U�n�D����--------------�©e�U���B�z TReqOld --------------------
   TWSTK_class TReqOld : public TReqRefBase {
      typedef TReqRefBase   inherited;
   protected:
      K_ctor  TReqOld  (TOrdCmdID, TTrProfile&, TSysOrdID);
   public:
      //data members
      TOrdSource  OrdSource_;
   };
   //�e�U�n�D����--------------�d�� TReqQry --------------------
   TWSTK_class TReqQryBase : public TReqOld {
      typedef TReqOld   inherited;
   protected:
      K_ctor  TReqQryBase  (TTrProfile&, TSysOrdID);
      friend class TOrdFlow;
   };
   typedef wise_ref<TReqQryBase> TReqQry;
   static inline K_mf(TReqQry)   CreateReqQry  (TTrProfile& tpf, TSysOrdID id)
                                                   { return TReqQry(new TReqQryBase(tpf,id)); }
   //�e�U�n�D����--------------�d�� TReqChgQty --------------------
   // cooper {
   //    ����ҳo�䥲���n�O�d��l���A,�p�G��q ReqQty �� 0 ��,�٬O��q
   // } cooper
   TWSTK_class TReqChgQtyBase : public TReqOld {
      typedef TReqOld   inherited;
      TStkQty ReqQty_;  // >0���ƶq, <0�R��ƶq, ==0�R��
   protected:
      K_ctor  TReqChgQtyBase  (TTrProfile&, TSysOrdID, TStkQty);
      K_ctor  TReqChgQtyBase  (TTrProfile&, TSysOrdID); // cooper
      friend class TOrdFlow;
   public:
      inline K_mf(TStkQty)  GetReqQty   () const   { return ReqQty_; }
   };
   typedef wise_ref<TReqChgQtyBase> TReqChgQty;
   static inline K_mf(TReqChgQty)  CreateReqChgQty  (TTrProfile& tpf, TSysOrdID id, TStkQty qty)
                                                       { return TReqChgQty(new TReqChgQtyBase(tpf,id,qty)); }
   //�e�U�n�D����--------------�R�� TReqDel --------------------
   TWSTK_class TReqDelBase : public TReqChgQtyBase {
      typedef TReqChgQtyBase   inherited;
   protected:
      K_ctor  TReqDelBase  (TTrProfile&, TSysOrdID);
      friend class TOrdFlow;
   };
   typedef wise_ref<TReqDelBase> TReqDel;
   static inline K_mf(TReqDel)  CreateReqDel  (TTrProfile& tpf, TSysOrdID id)
                                                   { return TReqDel(new TReqDelBase(tpf,id)); }
   //�e�U�n�D����--------------��� TReqChgPri --------------------
   TWSTK_class TReqChgPriBase : public TReqDelBase {
      typedef TReqDelBase  inherited;
      TStkPri  ReqPri_; //������.
      TReqNew  ReqNew_; //�R�榨�\��N�n�e�X���s�e�U.

   protected:
      friend class TOrdFlow;
      K_ctor     TReqChgPriBase (TTrProfile&, TSysOrdID, TStkPri);
      K_mf(bool) SetReqNew      (TOrdFlow*);//OrdFlow�bDoReq()���\��|���I�s���禡.
      K_mf(void) ClrTrProfile   ();
   public:
      inline K_mf(TReqNew) GetReqNew () const   { return ReqNew_; }
      inline K_mf(TStkPri) GetReqPri () const   { return ReqPri_; }
      virtual K_mf(void)   Update    (const std::string&, EOrdFlowID);
   };
   typedef wise_ref<TReqChgPriBase> TReqChgPri;
   static inline K_mf(TReqChgPri)  CreateReqChgPri  (TTrProfile& tpf, TSysOrdID id, TStkPri pri)
                                                   { return TReqChgPri(new TReqChgPriBase(tpf,id,pri)); }
   //�e�U�n�D����--------------[*�պ�] TReqTriMul --------------------
   //TWSTK_class TReqTriMul : public TReqNew {
   //};
   //�e�U�n�D����--------------[/�պ�] TReqTriDiv --------------------
   //TWSTK_class TReqTriDiv : public TReqNew {
   //};
   //================ �e�U�y�{���U =============
   TWSTK_class TFlowStep {
      virtual K_mf(bool)  OnOrdReq  (TReqPtr) = 0;//�Ǧ^false��,��ܤ��_�e�U�y�{!
      friend class TOrdFlow;
   };

   // cooper { key �q 0 �}�l
   K_mf(bool) AddFlowStep (TOrdCmdID cid, TFlowStep& fs, size_t key);
   // } cooper

   inline K_mf(bool)  AddFlowStep  (TOrdCmdID cid, TFlowStep& fs)  { return FlowSteps_[cid].add(&fs); }
   inline K_mf(bool)  DelFlowStep  (TOrdCmdID cid, TFlowStep& fs)  { return FlowSteps_[cid].del(&fs); }
          K_mf(int)   DelFlowStep  (TFlowStep&);

   //================ ����e�U�n�D =============
   K_mf(bool)  DoReq  (TReqNew);
   K_mf(bool)  DoReq  (TReqQry);
   K_mf(bool)  DoReq  (TReqDel);
   K_mf(bool)  DoReq  (TReqChgQty);
   K_mf(bool)  DoReq  (TReqChgPri);
   //K_mf(bool)  DoReq  (TReqTriMul);
   //K_mf(bool)  DoReq  (TReqTriDiv);

   inline K_mf(TOrdCenter&) GetOrdCenter() const   { return *OrdCenter_; }
private:
   friend class TReqBase;
   K_mf(bool)  DoFlowSteps  (TReqBase&, size_t stepn);
   K_mf(bool)  DoReqOld     (TReqOld&);
   K_mf(bool)  DoReqNew     (TReqNew, bool doFlow);

   typedef Kstl::pod_vector<TFlowStep*>   TFlowSteps;
   typedef TFlowSteps::iterator           TFlowStepi;
   TFlowSteps     FlowSteps_[cid_End];
   TOrdCenter*    OrdCenter_;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif

