//---------------------------------------------------------------------------
#ifndef FTScheduleH
#define FTScheduleH
//---------------------------------------------------------------------------
#include "KwayBase.h"
#include "inn_file.h"
#include "ViewArea.h"
#include "ViewAreaMgr.h"
#include "TimerThread.h"
#include "fixed_num.h"
#include "TwStk.h"
#include "TwStkBroker.h"
#include "bxFT.h"
#include "FTJobDef.h"
#include "StdRecorder.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw   {
namespace Bx   {
namespace FT   {
//---------------------------------------------------------------------------
enum TFTScheduleFieldNo
{
  ftsfn_SeqNo,
  ftsfn_SchType,
  ftsfn_Rightaway,
  ftsfn_FileCode,
  ftsfn_Market,
  ftsfn_BrkID,
  ftsfn_Msg,
  ftsfn_ExecTime,
  ftsfn_RetryInt,
  ftsfn_RetryCount,
  ftsfn_LastExecTime,
  ftsfn_ExecCount,
  ftsfn_FinishTime,
  ftsfn_Memo,
  ftsfn_End,
};
//---------------------------------------------------------------------------
enum ESchType{
   estNone  = 'N',     // ������
   estToday = 'T',    // �u������
   estAlways= 'Y',   // �C��Ƶ{
};
//---------------------------------------------------------------------------
K_class TSchType : public Kstl::EnumT<ESchType>
{
   typedef Kstl::EnumT<ESchType>  inherited;
public:
   inline K_ctor      TSchType    (ESchType m = estAlways)    : inherited(m){ }
   K_mf(std::string)  as_string   () const;
};
//---------------------------------------------------------------------------
struct FTSchedule   // �u�@�Ƶ{
{
    typedef Kstl::char_ary<char, 64, 0> TMemo;
    typedef Kstl::fixed_num<int>        TNum;

    TSchType        SchType_;       // ����Ҧ�
    bool            Rightaway_;     // �ߧY����
    TFileCode       FileCode_;      // filecode
    KStk::TMarket   Market_;        // TSE/OTC
    KStk::TBrokerID BrkID_;         // Broker
    Kway::TKTime    ActiveTime_;    // �w�w����ɶ�
    TNum            DelayTime_;     // ���Ѯ�,���yN�����᭫��
    TNum            Retry_;         // ���զ��� 0:�N������,-1:Always
    Kway::TKTime    LastExecTime_;  // �W������ɶ�
    TNum            ExecCount_;     // ���榸��
    Kway::TKTime    FinishTime_;    // ���\�ɶ�
    Kway::TKDate    LastDate_;      // �̫ᦨ�\���
    Kway::TKDate    LastModDate_;   // �̫�ק���
    TMemo           Msg_;           // 20080909�ǰe�ɪ����[�T�� 
    TMemo           Memo_;          // �Ƶ�
    K_ctor FTSchedule() : ActiveTime_(TKTime::Now()), DelayTime_(3),
                          Retry_(0), ExecCount_(0),
                          SchType_(TSchType(estToday)), Rightaway_(false)
                          {ActiveTime_.IncMin();}
};
//---------------------------------------------------------------------------
struct SULoadSch   // �u�@�Ƶ{
{
    typedef Kstl::char_ary<char, 64, 0> TMemo;
    typedef Kstl::fixed_num<int>        TNum;

    TSchType        SchType_;       // ����Ҧ�
    bool            Rightaway_;     // �ߧY����
    TFileCode       FileCode_;      // filecode
    KStk::TMarket   Market_;        // TSE/OTC
    KStk::TBrokerID BrkID_;         // Broker
    Kway::TKTime    ActiveTime_;    // �w�w����ɶ�
    TNum            DelayTime_;     // ���Ѯ�,���yN�����᭫��
    TNum            Retry_;         // ���զ��� 0:�N������,-1:Always
    Kway::TKTime    LastExecTime_;  // �W������ɶ�
    TNum            ExecCount_;     // ���榸��
    Kway::TKTime    FinishTime_;    // ���\�ɶ�
    Kway::TKDate    LastDate_;      // �̫ᦨ�\���
    Kway::TKDate    LastModDate_;   // �̫�ק���
    TMemo           Memo_;          // �Ƶ�
    K_ctor SULoadSch() : ActiveTime_(TKTime::Now()), DelayTime_(3),
                                     Retry_(0), ExecCount_(0),
                                     SchType_(TSchType(estToday)), Rightaway_(false)
                                     {ActiveTime_.IncMin();}
};
//---------------------------------------------------------------------------
//typedef Kstl::rr_inn_file<Kway::uint64, Kway::word> TSchInn;
//---------------------------------------------------------------------------
//typedef Kstl::ev_map<TFileCode, const FTSchedule>               TScheduleMap;
typedef Kstl::fixed_num<Kway::word>    TScheduleNo;
typedef Kstl::ev_map<FT::TSeqNo, const FTSchedule>              TScheduleMap;
//typedef TXRecorder<TScheduleMap, TSchInn, sizeof(FTSchedule)>   TScheduleInn;
typedef TStdRecorder<TScheduleMap, FTSchedule>                  TScheduleInn;
//---------------------------------------------------------------------------
K_class TScheduleVAC : public Kway::TVACreater
{
   typedef Kway::TVACreater inherited;
   typedef TScheduleVAC     this_type;
   K_class TScheduleFieldMaker
   {
      typedef TScheduleMap              TEvContainer;
      typedef TEvContainer::iterator    iterator;
      typedef size_t                    field_n_type;
   public:
      inline K_mf(field_n_type)     GetFieldCount ()                const { return ftsfn_End; }
      K_mf(const Kway::TFieldDesp&) GetFieldDesp  (field_n_type n)  const;
      K_mf(std::string)             Get (const iterator& i, field_n_type fieldNo) const;
      K_mf(void)                    Put (TEvContainer& c,iterator& iter, field_n_type fieldNo, const std::string& str);
   };
   TScheduleMap*  Recs_;
public:
   K_ctor TScheduleVAC(TScheduleMap& recs) : Recs_(&recs) {}
   typedef Kway::TTViewArea<TScheduleMap, TScheduleFieldMaker>  TScheduleViewer;
   K_mf(Kway::TaViewArea)  CreateNewVA  ()
    { return Kway::TaViewArea(new TScheduleViewer(Recs_)); }
};
//---------------------------------------------------------------------------
typedef Kstl::ev_map<FT::TSeqNo, const SULoadSch>               TULoadSchMap;
//typedef TXRecorder<TULoadSchMap, TSchInn, sizeof(SULoadSch)>    TULoadSchInn;
typedef TStdRecorder<TULoadSchMap, SULoadSch>                   TULoadSchInn;
//---------------------------------------------------------------------------
K_class TULoadSchVAC : public Kway::TVACreater
{
   typedef Kway::TVACreater inherited;
   typedef TULoadSchVAC     this_type;
   K_class TULoadSchFieldMaker
   {
      typedef TULoadSchMap              TEvContainer;
      typedef TEvContainer::iterator    iterator;
      typedef size_t                    field_n_type;
   public:
      inline K_mf(field_n_type)     GetFieldCount ()                const {return 13;}
      K_mf(const Kway::TFieldDesp&) GetFieldDesp  (field_n_type n)  const;
      K_mf(std::string)             Get (const iterator& i, field_n_type fieldNo) const;
      K_mf(void)                    Put (TEvContainer& c,iterator& iter, field_n_type fieldNo, const std::string& str);
   };
   TULoadSchMap*  Recs_;
public:
   K_ctor TULoadSchVAC(TULoadSchMap& recs) : Recs_(&recs) {}
   typedef Kway::TTViewArea<TULoadSchMap, TULoadSchFieldMaker>  TULoadSchViewer;
   K_mf(Kway::TaViewArea)  CreateNewVA  ()
    { return Kway::TaViewArea(new TULoadSchViewer(Recs_)); }
};
//---------------------------------------------------------------------------
K_class TScheduleCenter
{
    typedef TScheduleCenter this_type;
    typedef Kway::TViewAreaMgr  TViewAreaMgr;
//    TSchInn*            Inn_;
    TScheduleInn*       Recorder_;
    TScheduleVAC*       VAC_;
    TScheduleMap*       Map_;
//    TSchInn*            ULInn_;
    TULoadSchInn*       ULRecorder_;
    TULoadSchVAC*       ULVAC_;
    TULoadSchMap*       ULMap_;
    Kway::TViewAreaMgr  VAMgr_;
    Kway::TTimer<this_type> Timer_;
protected:
    K_mf(void)  StartTimer  ()  { Timer_.Start(60*1000, &this_type::OnTimer); }
    K_mf(void)  StopTimer   ()  { Timer_.Stop();    }
    K_mf(void)  OnTimer     (TTimerBase*);
    K_mf(void)  DoReq       (TSeqNo, const FTSchedule*);
    K_mf(void)  DoULReq     (TSeqNo, const SULoadSch*);
    K_mf(void) CreateVAC (void); //�إ߻���VAC
    K_mf(void) DestoryVAC(void);
public:
    K_ctor TScheduleCenter ();
    K_dtor ~TScheduleCenter();
    K_mf(const TViewAreaMgr*) GetViewAreaMgr() const { return &VAMgr_; }
    K_mf(void) UpdateSchedule(TSeqNo, const char* msg = 0);
    K_mf(void) UpdateULoadSch(TSeqNo, const char* msg = 0);
};
//---------------------------------------------------------------------------
K_fn(const TScheduleCenter&) GetSchedule();
//---------------------------------------------------------------------------
};  // FT
};  // Bx
};  // Tw
};  // Kway
#endif
