/////////////////////////////////////////////////////////////////////////////
#if !defined(_FixStkOrdFlow_h_)
#define _FixStkOrdFlow_h_
//---------------------------------------------------------------------------
// libnsAux
#include "tpl/Nsa_EvObjCtn.h"
// FixStk
#include "FixStkOrdFlowReq.h"
#include "FixStkRptFlow.h"

/////////////////////////////////////////////////////////////////////////////
namespace Kway {
   using namespace Nsa;

namespace FIX {
namespace Stk {
//---------------------------------------------------------------------------
class TTradeInfo;
class TTradeLog;
//---------------------------------------------------------------------------
#define FixStk_GetFixMsg(FixMsg, QfField, KwField)    \
   {                                                  \
      QfField QfField##Tmp;                           \
      if (FixMsg.findField(QfField##Tmp))             \
         KwField.assign(QfField##Tmp);                \
   }                                                  \
//---------------------------------------------------------------------------
// FixStk_FindTagVal
#define FixStk_FindTagVal(FixMsg, TagId, KwFd)        \
   if (FixMsg.findField(TagId, tagVal))               \
      KwFd.assign(tagVal);                            \
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
// ECoidStatus
/////////////////////////////////////////////////////////////////////////////
enum ECoidStatus
{
   ecs_Empty,     // ClOrdID �ť�
   ecs_Duplicate, // ClOrdID ����
   ecs_Valid,     // ClOrdID �O���Ī�
};

/////////////////////////////////////////////////////////////////////////////
// EFindOrdResult
/////////////////////////////////////////////////////////////////////////////
// �M��[�©e�U]�����G
enum EFindOrdResult
{
   for_Success,
      for_OrderNotFound,   // �䤣���l�e�U
      for_ClOrdIDDup,      // ClOrdID ����
      for_OrderDup,        // ���Ъ��e�U
      for_OrdContNotMatch, // �e�U���e����
};

/////////////////////////////////////////////////////////////////////////////
// class TOfSpecRule    (OrdFlow�S���B�z�޿�)
/////////////////////////////////////////////////////////////////////////////
enum EOfSpecRule
{
   // �@�몺 Fix �O�ϥ�[�Ѿl�q],�ҥ�ҬO�ϥ�[�R��q]
   osr_ChgQty_UseDec,

   // ����[���]�P�_; Ex:�ҥ�ҨS������\��
   osr_ChgPri_Disable,

   osr_Total,
};
typedef std::bitset<osr_Total>  TOfSpecRule;

/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif

/////////////////////////////////////////////////////////////////////////////
// class TFsOrdFlow
/////////////////////////////////////////////////////////////////////////////
K_class TFsOrdFlow
{
public:
   typedef TFsOrdCenter::TOrdHnd  TOrdHnd;

   /*
   enum EReqOldOrdResu
   {
      ror_Success,
         ror_OrderNotFound, // �䤣���l�e�U
         ror_ClOrdIDDup,    // ClOrdID ����
         ror_OrderDup,      // ���Ъ��e�U
   };
   */

   //==============================================
   // class TFlowStep
   class TFlowStepWorker;

   class TFlowStep
   {
      friend class TFlowStepWorker;

      // �Ǧ^false��,��ܤ��_�e�U�y�{!
      virtual K_mf(bool) OnFsOrdReq (TOfReqPtr) = 0;
   };
   
   //==============================================
   // ����e�U�n�D
   K_mf(bool) DoReqNos  (TOfReqNos);
   K_mf(bool) DoReqOcr  (TOfReqOcr);
   K_mf(bool) DoReqOcrr (TOfReqOcrr);
   K_mf(bool) DoReqOsr  (TOfReqOsr);
   K_mf(bool) DoReqAno  (TOfReqAno);
   K_mf(bool) DoReqRno  (TOfReqRno);

   //==============================================
   // TFsOrdFlow inline function
   //inline K_mf(bool) AddFlowStep (TFixOrdCmdID cid, TFlowStep* fs, EFlowPrio fp = efp_None)
   //   { return FlowSteps_[cid].Add((TFlowSteps::TKey)fp, fs); }
   inline K_mf(bool) AddFlowStep (TFixOrdCmdID cid, TFlowStep* fs)  { return FlowSteps_[cid].Add(fs); }
   inline K_mf(void) DelFlowStep (TFixOrdCmdID cid, TFlowStep* fs)  { return FlowSteps_[cid].Del(fs); }

   inline K_mf(void) SetRptFlow (TRptFlow* rf)  { RptFlow_ = rf; }
   
   inline K_mf(TOrdTrack*)    GetOrdTrack  () const  { return OrdTrack_;  }
   inline K_mf(TRptFlow*)     GetRptFlow   () const  { return RptFlow_;   }
   inline K_mf(TFsOrdCenter*) GetOrdCenter () const  { return OrdCenter_; }
   
   //==============================================
   // OrdFlow Req Create
   static inline K_mf(TOfReqNos)  CreateReqNos  (const TMessageRef& m)  { return TOfReqNos (new TOfReqNosBase (m)); }
   static inline K_mf(TOfReqOcr)  CreateReqOcr  (const TMessageRef& m)  { return TOfReqOcr (new TOfReqOcrBase (m)); }
   static inline K_mf(TOfReqOcrr) CreateReqOcrr (const TMessageRef& m)  { return TOfReqOcrr(new TOfReqOcrrBase(m)); }
   static inline K_mf(TOfReqOsr)  CreateReqOsr  (const TMessageRef& m)  { return TOfReqOsr (new TOfReqOsrBase (m)); }

   static inline K_mf(TOfReqAno) CreateReqAno (const TMessageRef& m, TFixRefOrdID roid, bool isValidOrd)
      { return TOfReqAno(new TOfReqAnoBase(m, roid, isValidOrd)); }

   static inline K_mf(TOfReqRno) CreateReqRno ()  { return TOfReqRno(new TOfReqRnoBase); }

protected:
   typedef TEvObjCtn<TFlowStep>  TFlowSteps;

   TTradeInfo*     TradeInfo_;
   TCoids*         Coids_;
   TFsOrdCenter*   OrdCenter_;
   TRptFlow*       RptFlow_;
   TOrdTrack*      OrdTrack_;
   TTradeLog*      TradeLog_;
   TFlowSteps      FlowSteps_[fcid_Total];
   TOfSpecRule     SpecRule_;
   // �� Fix �q�����]�w
   TOfOrdFieldSet  OfSetNos_;
   TOfOrdFieldSet  OfSetOcr_;
   TOfOrdFieldSet  OfSetOcrr_;
   TOfOrdFieldSet  OfSetOsr_;
   TOfOrdFieldSet  OfSetAno_;

public:
   K_ctor TFsOrdFlow (TTradeInfo*);
   
   // �]�w fix ������
   K_mf(void) SetFixVer (int);

   inline K_mf(TFsOrdCenter*) GetOrdCenter ()  { return OrdCenter_; }

protected:
   //======================================================
   // TFsOrdFlow virtual function
   //======================================================
   // ���o�ª��e�U
   virtual K_mf(void)           VirInsNewOrd  (const TFixSysOrdID&, const TFsStkOrder&) {}
   virtual K_mf(EFindOrdResult) VirFindOldOrd (TOfReqOldBase&, TFixSysOrdID&);
   
   // �U�R��d�@�~�e�|���ǳƤ@�ǰ����
   virtual K_mf(void) VirOnGetOrdFiled (const Message&, SOfOrdField&) = 0;

   // ���ͷs�e�U��, �ݭn��J StkOrd ��ƪ��ƥ�
   virtual K_mf(void) VirInitNewOrdField (const Message&, const SOfOrdField&, TFsStkOrder&) = 0;
   
   // NewOrderSingle ���ƥ�
   virtual K_mf(bool) OnProcReqNos (TOfReqNosBase*) = 0;
   
   // OrderCancelRequest ���ƥ�
   virtual K_mf(void) VirBfProcOcr (TOfReqOcrBase*) = 0;
   virtual K_mf(bool) VirProcOcr   (TOfReqOcrBase*) = 0;

   // OrderCancelReplaceRequest ���ƥ�
   virtual K_mf(void) VirBfProcOcrr (TOfReqOcrrBase*) = 0;
   virtual K_mf(bool) VirProcOcrr   (TOfReqOcrrBase*) = 0;
   
   // OrderStatusRequest ���ƥ�
   virtual K_mf(void) VirBfProcOsr (TOfReqOsrBase*) = 0;
   virtual K_mf(bool) VirProcOsr   (TOfReqOsrBase*) = 0;
   
   // ��e�U�䤣��ɪ��B�z����
   virtual K_mf(bool) VirProcOrderNotFound (TOfReqOldBase*) = 0;

   //======================================================
   // TFsOrdFlow function
   //======================================================
   // �M���l�e�U, �ˬd ClOrdID
   K_mf(EFindOrdResult) DoReqOld (TOfReqOldBase&, SOrdTrack&);
   
   // �o�{ ClOrdID ���ƪ��B�z����
   K_mf(void) ProcClOrdIDTheSame (EFixOrdCmdID, TFixSysOrdID);

   // �B�z�e�m�@�~�o�{�����~
   //K_mf(void) ProcReqOldError (TOfReqOldBase&);
   K_mf(void) ProcReqError (TOfReqBase&, int rejCode);

   // �q Fix �q�夤����w�q���
   K_mf(void) GetOrdFiled (const TOfOrdFieldSet&, const Message&, SOfOrdField&);

   // �b���ͷs�e�U��, �ݭn��J StkOrd �����
   K_mf(void) InitOrdField (const Message&, const SOfOrdField&, TFsStkOrder&);

   // �}�l�B�z�e�U�o
   K_mf(bool) DoFlowSteps (TOfReqBase&);
};   

/////////////////////////////////////////////////////////////////////////////
}; // namespace Stk
}; // namespace FIX
}; // namespace Kway

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_FixStkOrdFlow_h_)
