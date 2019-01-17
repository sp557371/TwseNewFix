//---------------------------------------------------------------------------#ifndef bxRptSesH
#define bxRptSesH
//---------------------------------------------------------------------------
#include "bxLinkSes.h"
#include "bxRpt.h"
#include "bxRptWorker.h"
#include "bxSesMgr.h"
//---------------------------------------------------------------------------
namespace Kway  {
namespace Tw    {
namespace Bx    {
namespace Rpt   {
/////////////////////////////////////////////////////////////////////////////
K_class TbxRptSes : public TbxLinkSes
{
   typedef TbxLinkSes       inherited;
   typedef TbxRptSes        this_type;
   typedef std::set<long>           TSeqSet;
   typedef TSeqSet::iterator        TSeqSetI;
   TbxFTSesMgr& SesMgr_;
   TWork_R5000  R5000_;
   TWork_R5010  R5010_;
   TWork_R5020  R5020_;
   size_t       RptTimeOut_;
   TSeqSet      SeqSet_;
   TSeqSet      ReSeqSet_;      // �w�n�D�^��, ���Ԧ^�ɵ��G��SET
   TFile        Pmach_;
   long         LastSeqNo_;     // �̫᪺����^���Ǹ�

public:
   enum TRptState
   {
     rpt_Starting=5000, // �}��RPT�@�~
     rpt_RptReply,      // RPT�w�^��
     rpt_Data,          // ������
     rpt_ReData,        // �ɺ|
     rpt_LinkChk,       // RPT�s�u�T�{�T��
     rpt_Logout,        // RPT�@�~����
     rpt_RptEnd,        // RPT�^���w����
   };
protected:
        K_mf(bool)  SettingApply    (const std::string&);
        K_mf(void)  AddWorkers      ();
        K_mf(void)  StartRptSys     ();
        K_mf(void)  OnSubSysReady   (TApKey);
        K_mf(void)  OnSubSysBroken  (TStCode);
        K_mf(void)  OnMesLinkBroken (TMesBase*, const std::string&);//�M�������w�İ�
        K_mf(void)  OnMesLinkFail   (TMesBase*, const std::string&);//SetState(sp_Error);
virtual K_mf(void)  DoTimeOut       (TTimerBase*);
        K_mf(bool)  CallRecover     (long, long);
        K_mf(void)  CheckSetCont    ();     // �ˬd�����
        K_mf(void)  UpdateState     ();

public:
        K_ctor  TbxRptSes  (TbxFTSesMgr*, KStk::TMarket);
        K_dtor ~TbxRptSes  ();
        K_mf(TWorkKey) GetWorkKey   (TFunCode);
        K_mf(void)  SetState        (TRptState, const std::string&);
 inline K_mf(void)  StartRptTimer   () { StartTimer(RptTimeOut_);   }
 inline K_mf(void)  StopRptTimer    () { StopTimer();               }
        K_mf(TSeqNo)GetNextSeqNo    ();
        K_mf(bool)  SendPkt         (const TControlHead&);
        K_mf(bool)  WritePmach      (const TRptBody&);
        K_mf(void)  CheckRptCount   (long recs);    // �ˬd���쪺���ƬO�_����
        K_mf(void)  RptRecover      (const TRptBody&, bool);
        K_mf(void)  ImpRptFile      (std::string&);
virtual K_mf(const std::string) SesName() { return std::string("����^��"); }
};
/////////////////////////////////////////////////////////////////////////////
}; // Rpt
}; // Bx
}; // Tw
}; // Kway
//---------------------------------------------------------------------------
#endif


