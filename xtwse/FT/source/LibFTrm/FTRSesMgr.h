//---------------------------------------------------------------------------
#ifndef FTRSesMgrH
#define FTRSesMgrH
//---------------------------------------------------------------------------
#include "SesMgrR.h"
#include "TimerThread.h"
#include "KFile.h"
#include "stdSettingP.h"
#include "FTRMDef.h"
//---------------------------------------------------------------------------
namespace Kway{
namespace Tw{
namespace Bx{
namespace FT{
/////////////////////////////////////////////////////////////////////////////
K_class TRSesMgr:public Kway::TSesMgrR,
                 public TFTRBase
{
   typedef TSesMgrR               inherited;
   typedef TFTRBase               inherited_FT;
   typedef std::vector<TSesBase*> TSesList;
   typedef TRSesMgr               this_type;
   typedef Kstl::ev_map<TFileCode, Kway::byte> TFCodes;
   struct THostRec{
	   eLState        State_; 
	   Kway::word     QJobs_;
	   TFCodes        FCodes_;
	   K_ctor THostRec():QJobs_(0),State_(elsNone){};
   };
   typedef Kstl::ev_map<Kway::byte, THostRec>  TRecs;
   typedef Kstl::ev_map<TBrkMkt, TRecs>        THostRecs;
   typedef Kstl::ev_map<TBrkMkt, TFCodes>      TProcRecs;
private:
   TStates                 LocalState_;
   TSettingPage*           RSetting_;
   bool                    UseRM_;
   bool                    UseRD_;
   Kway::TViewAreaMgr      VAMgr_;
   TSesList                SesList_;
   TSesList                CliList_;
   TFile                   ORRecorder_;
   THostRecs*              HostRecs_;        // �Ҧ��i¶��D�����u�����p�[�`
   TProcRecs               ProcRecs_;        // �Ҧ��v�e�i�Ӫ��u�@�M��, �v�}�l�ǰe�Ჾ��
   Kway::byte              LocalHID_;
public:
   K_ctor TRSesMgr       ();
   K_dtor ~TRSesMgr      ();
   K_mf(bool) SendReq    (const TFTJob* , int hid = -1);
   K_mf(void) SesReg(TSesBase*);
   K_mf(void) SesDel(TSesBase*);
   K_mf(void) RegCli(TSesBase*);
   K_mf(void) DelCli(TSesBase*);
   K_mf(void) WriteLog(char*);
   K_mf(void) SendState (TState ls);
   K_mf(bool) IsUseORD  (){return UseRD_;}
   K_mf(bool) IsUseORM  (){return UseRM_;}
   K_mf(Kway::byte) GetLocalHID(){return LocalHID_;}
   K_mf(eLState)    GetState     (TBrkMkt m, int& hid);      // ���o�Ҧ�¶�����Һ��@���U�D�����A
   K_mf(int)        TestHandle   (TBrkMkt m,TFileCode fcode);// �P�_�Y���@�~�O�_�b��L���D���v�B�z
   K_mf(void)       ManageState  (TState e, Kway::byte hid); // �޲z��L�x�D�������A
   K_mf(void)       ResetState   (Kway::byte hid);           // �_�u�ɭn���m��L�x�D�������A
   K_mf(int)        Dispatchable (TBrkMkt m,int,TFileCode);  // �P�_�O�_��F���y������
   K_mf(void)       AddNewFCode  (TBrkMkt, TFileCode);
};
//---------------------------------------------------------------------------
};
};
};
};
//---------------------------------------------------------------------------
#endif
