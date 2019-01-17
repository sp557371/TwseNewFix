//---------------------------------------------------------------------------
#ifndef bxDRSesH
#define bxDRSesH
//---------------------------------------------------------------------------
#include "bxLinkSes.h"
#include "bxDR.h"
#include "bxDRWorker.h"
#include "bxSesMgr.h"
//---------------------------------------------------------------------------
namespace Kway  {
namespace Tw    {
namespace Bx    {
namespace DR    {
/////////////////////////////////////////////////////////////////////////////
K_class TbxDRSes : public TbxLinkSes
{
   typedef TbxLinkSes       inherited;
   typedef TbxDRSes        this_type;
   typedef std::set<long>           TSeqSet;
   typedef TSeqSet::iterator        TSeqSetI;
   TbxFTSesMgr& SesMgr_;
   TWork_R5000  R5000_;
   TWork_R5010  R5010_;
   TWork_R5020  R5020_;
   size_t       RptTimeOut_;
   TSeqSet      SeqSet_;
//   TSeqSet      ReSeqSet_;      // �w�n�D�^��, ���Ԧ^�ɵ��G��SET
   TFile        Pmach_;
   long         LastSeqNo_;     // �̫᪺����^���Ǹ�
   bool         Recovering_;    // �ɺ|�i�椤
   bool         Finish_;        // �v�g����̫�@���T���F(�`���ưT��), ���ΦA�w���|�հ��̫�@���ɺ|�n�D�F
   bool         IsNewLine_;     // �]�w PMach �榡�O�_����
   std::string  RecoverFName_;
   Kway::TTimer<this_type>  RecoverTimer_;

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
        K_mf(void)  StartupFile     ();
        K_mf(bool)  SettingApply    (const std::string&);
        K_mf(void)  AddWorkers      ();
        K_mf(void)  StartRptSys     ();
        K_mf(void)  OnSubSysReady   (TApKey);
        K_mf(void)  OnSubSysBroken  (TSubSysName,TStCode);
        K_mf(void)  OnMesLinkReady  (TMesBase*);
        K_mf(void)  OnMesLinkBroken (TMesBase*, const std::string&);//�M�������w�İ�
        K_mf(void)  OnMesLinkFail   (TMesBase*, const std::string&);//SetState(sp_Error);
virtual K_mf(void)  DoTimeOut       (TTimerBase*);
        K_mf(void)  UpdateState     ();

public:
        K_ctor  TbxDRSes  (TbxFTSesMgr*, KStk::TMarket);
        K_dtor ~TbxDRSes  ();
        K_mf(TWorkKey) GetWorkKey   (TFunCode);
        K_mf(void)  SetState        (TRptState, const std::string&);
 inline K_mf(void)  StartRptTimer   () { StopRptTimer(); StartTimer(RptTimeOut_);   }
 inline K_mf(void)  StopRptTimer    () { StopTimer();               }
        K_mf(bool)  SendPkt         (const TControlHead&);
        K_mf(bool)  WriteDR         (const TDRBody&);
virtual K_mf(const std::string) SesName() { return std::string("����^��"); }
        K_mf(void)  DRFinish        ();
};
/////////////////////////////////////////////////////////////////////////////
}; // DR
}; // Bx
}; // Tw
}; // Kway
//---------------------------------------------------------------------------
#endif


