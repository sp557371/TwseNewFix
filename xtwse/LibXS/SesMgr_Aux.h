//
// �q�Tclasses��¦: Session-Messager
//    ���U class TSesMgr; �B�z������ classes
//
// ����: $Id: SesMgr_Aux.h,v 1.16 2005/05/13 05:40:23 fonwin Exp $
//
//---------------------------------------------------------------------------
#ifndef SesMgr_AuxH
#define SesMgr_AuxH
//---------------------------------------------------------------------------
#include "SesMgr.h"
#include "recorder.h"
#include "inn_file.h"
#include "ev_ary.h"
#include "nsinit.h"
//*************************************************
#if defined(__NSX__)
#include "KIniFile.h"
#endif
//*************************************************
//---------------------------------------------------------------------------
#include <queue>
/////////////////////////////////////////////////////////////////////////////
using namespace Kway;
/////////////////////////////////////////////////////////////////////////////
class TSesMgr::TMesMeta : public TMesNoThread, public TSesBase, public TSesEvHandler
{
   typedef TMesNoThread inheritedMes;
   typedef TSesBase     inheritedSes;
   TSesMgr&          Mgr_;
   TSesID            ID_;
   const TSesReg*    SesReg_;    //�� Mes_ �� Server Style �ɨϥ�
   TSesID            ServerID_;  //�� this �� Server ���ͥX�� client �ɨϥ�
   TMesPtr           MesPtr_;

   friend class TSesMgr::TSetting;//��]�w�����ܮ�, �i��ݭn�վ� ID_ ����
   friend class TSesMgr::TStatus; //��AddClient()��,�i��ݭn�վ� ID_ ����

//Handle ��ڪ� Mes ���ͪ��ƥ�, �N�ƥ��൹ Ses �� SesMgr
   K_mf(void)            OnMesLinkReady      (TMesBase*);
   K_mf(void)            OnMesLinkFail       (TMesBase*, const std::string& reason);
   K_mf(void)            OnMesLinkBroken     (TMesBase*, const std::string& reason);
   K_mf(void)              MesLinkError      (const char*, const std::string& reason);
   K_mf(void)            OnMesSendEmpty      (TMesBase*);
   K_mf(void)            OnMesRecvReady      (TMesBase*, size_t size);
   K_mf(void)            OnMesStateChanged   (TMesBase*, TMesStateLevel msl, const std::string& msg);
   K_mf(TMesEvHandler*)  GetClientEvHandler  (TMesBase* mesServer);
   K_mf(bool)            OnMesClientConnect  (TMesBase* mesServer, TMesBase* mesClient);
   K_mf(void)            OnMesLeave          (TMesBase*);
   K_mf(void)            OnMesBfDestroy      (TMesBase*);
   K_mf(void)              MesLeave          (const char*);
   K_mf(void)            OnMesEnter          (TMesBase*);
//Handle Ses���ƥ�
   K_mf(void)  OnSesStateChanged  (TSesBase*, const std::string& reason);
   K_mf(void)  OnSesSetRemark     (TSesBase*, const std::string& remark);
   K_mf(void)  OnMesSetRemark     (TMesBase*, const std::string& remark);
   K_mf(void)  OnSesBfDestroy     (TSesBase*);

//���� Mes ���ާ@��k, �൹��ڪ� Mes
   //�}��/�����s�u
   K_mf(bool)  OpenImpl  ();
   K_mf(bool)  CloseImpl ();
   //�]�w�޲z
   K_mf(std::string)  GetSetting   ();
   K_mf(bool)         SettingApply (const std::string&);
   //�ǰe/�������
   K_mf(int)  Send  (const void* buffer, size_t size);
   K_mf(int)  Recv  (      void* buffer, size_t size);
   //������[�ǰe/����]�w�İϪ��j�p
   K_mf(bool)  GetBufSize  (size_t& sendBufSz, size_t& recvBufSz);
   K_mf(bool)  SetBufSize  (size_t  sendBufSz, size_t  recvBufSz);
   //�����o�{�b[�ǰe/����]�w�İ�,���h��Bytes����Ƶ��ݳB�z,�Ǧ^-1���Mes�����Ѧ��S��
   K_mf(int)  GetSendSize  ();
   K_mf(int)  GetRecvSize  ();
   //�����o��Mes���ѧO�N��
   K_mf(std::string)  GetUID      ();
   //���̫��Ʊq����UID��(�A�Ω�UDP...�������s������Mes)
   K_mf(std::string)  GetLastUID  ();

   K_ctor      TMesMeta  (TSesMgr&, const TSesReg&, TSesID serverID);
   K_mf(void)  SesCreate (const TSesReg&);

public:
           K_ctor  TMesMeta  (TSesMgr&, const TSesID, const TMesReg&, const TSesReg&);
   virtual K_dtor ~TMesMeta  ();

   K_mf(bool)  MesSettingApply  (const std::string&);
   K_mf(bool)  SesSettingApply  (const std::string&);

   inline K_mf(bool)  IsServer  () const   { return SesReg_ != 0; }

   using TMesNoThread::Open;
   using TMesNoThread::Close;
};
/////////////////////////////////////////////////////////////////////////////
class TSesMgr::TStatus : public  TVACreater
                       , private TNSCleanup
{
   K_mf(void)  OnNSCleanup  ();

   struct TStatusRec
   {
      TSettingRec          Setting_;
      std::string          SesStatus_;
      std::string          MesStatus_;
      std::string          Remark_;
      typedef Kstl::wise_ref<TMesMeta>   TMesMetaPtr;
      mutable TMesMetaPtr  MesMeta_;

      inline K_ctor      TStatusRec     ()         : MesMeta_(0)  {}
      inline K_mf(void)  ClearResource  () const   { MesMeta_.reset();  }
      inline K_mf(bool)  IsServer       () const   { return MesMeta_.get() ? MesMeta_->IsServer() : false; }
   };
   typedef ev_ary<TSesID,TStatusRec>   TStatusRecs_t;
   K_typedef(TStatusRecs_t,            TRecs);
   typedef TRecs                       TStatusRecs;
   //typedef ev_ary<TSesID, TStatusRec>  TStatusRecs;
   //typedef TStatusRecs                 TRecs;
   class   TFieldMaker;
   //
   TRecs                StatusRecs_;
   std::queue<TSesID>   FreeIDs_;
   K_mf(void)  DelClient  (const TRecs::iterator&);

   friend class TSesMgr::TSetting;
   typedef TTViewArea<TRecs,TFieldMaker>  TStatusVA;

   K_mf(bool)  GetCurrentIterator  (TViewArea*, TRecs::iterator&);
public:
   K_ctor            TStatus       ();
   K_dtor           ~TStatus       ();
   K_mf(TaViewArea)  CreateNewVA   ();
   K_mf(bool)        AddClient     (TMesMeta*);
   K_mf(bool)        DelClient     (TMesMeta*);
   K_mf(bool)        SetSesStatus  (TSesID, const std::string&);
   K_mf(bool)        SetMesStatus  (TSesID, const std::string&);
   K_mf(bool)        SetRemark     (TSesID, const std::string&);
   K_mf(bool)        DisconnectCurrent  (TViewArea*);
   K_mf(TSesBase*)   GetCurrentSes      (TViewArea*);
};
/////////////////////////////////////////////////////////////////////////////
class TSesMgr::TSetting : public TVACreater
{
   class  TFieldMaker;
   class  TViewArea;
   struct TWaitOpenMeta;
   //---------------------------------------------------------------------------
   struct TSettingRec : public TSesMgr::TSettingRec
   {
      TSesID mutable RunningID_;

      K_ctor  TSettingRec  ()  { Enabled_ = true; }
   };
   //---------------------------------------------------------------------------
   typedef Kstl::rr_inn_file<Kway::uint32,Kway::uint16>  TInn;
   struct TSettingKey : public Kstl::fixed_num<dword> {};//���γo�ؤ覡�w�q Key,�|�y��BCB���ͪ��{���X�����D
   #ifdef __alpha // for short Symbol
   #  define TSettingRec_RoomKey    TSrRk
   #endif
   struct TSettingRec_RoomKey : public TSettingRec
   {
      TInn::rr_key mutable   RoomKey_;
   };
   typedef ev_map<TSettingKey, const TSettingRec_RoomKey>   TSettingRecs;
public:
   typedef TSettingRecs          TRecs;
   typedef TSettingRec_RoomKey   TRec;
private:
   //---------------------------------------------------------------------------
   class TSettingRecorder : public Kstl::recorder<TRecs,TInn>,
                            public Kstl::inn_rooms_peeker<TRecs,TInn>
   {
      typedef Kstl::recorder<TRecs,TInn>  inheritedRecorder;
      typedef TSettingRec_RoomKey   TRec;
      typedef TSettingKey           TRecKey;
      K_mf(bool)    peek_room      (TRecs&,  rr_room);
      K_mf(bool)    into_room      (rr_room, rr_key&, const TRecKey&, const TRec&);
      K_mf(bool)    into_new_room  (TInn&, const TRecs::const_iterator&);
      K_mf(bool)    into_room      (TInn&, const TRecs::iterator&);
      K_mf(rr_key)  get_room_key   (TInn&, const TRecs::iterator&);

      std::string InnFileName_;
   public:
      K_ctor      TSettingRecorder  ();
      K_dtor     ~TSettingRecorder  ();
      //K_mf(void)  BindInn         (TInn&, TRecs&);
      K_mf(void)  LoadFrom          (const std::string& iniFileName, TRecs&);
      K_mf(void)  SaveTo            (const TRecs&);
   };
   //---------------------------------------------------------------------------
   //**********************************************
   #if defined(__NSX__)
   class TSettingIni : public TKIniFile
   {
      typedef TKIniFile  inherited;
      
   public:
      K_mf(void) LoadFrom (const std::string& iniFileName, TRecs&);
      K_mf(void) SaveTo   (const TRecs&);
   };
   #endif
   //**********************************************

   //**********************************************
   #if defined(__NSX__)
   TSettingRecorder  Recorder_;
   TSettingIni       SettingIni_;
   bool              UseIniFile_;
   #else //----------------------------------------
   TSettingRecorder  Recorder_;
   #endif
   //**********************************************
   TRecs             SettingRecs_;
   TSesMgr&          Mgr_;

   typedef TStatus::TRecs  TStatusRecs;
   K_mf(bool)       CheckChanged   (TStatusRecs& origStatusRecs, TStatusRecs& newStatusRecs,
                                    const TSettingRec& setting, TSesID idExpect);
   K_mf(TMesMeta*)  CreateSesMes   (TStatusRecs& statusRecs, const TSettingRec&, TSesID, bool autoOpen);
public:
   K_ctor              TSetting      (TSesMgr&);
   //**********************************************
   #if defined(__NSX__)
   K_mf(void)          Reload        (const std::string& settingFileName, bool useIniFile = false);
   #else
   K_mf(void)          Reload        (const std::string& settingFileName);
   #endif
   //**********************************************
   K_mf(TaViewArea)    CreateNewVA   ();
   K_mf(bool)          SettingApply  (TRecs& newSetting);
   K_mf(const TRecs&)  GetSetting    ()   { return SettingRecs_; }

   K_mf(const TSesReg*)  GetSesReg   (const std::string&) const;
   K_mf(const TMesReg*)  GetMesReg   (const std::string&) const;
};
/////////////////////////////////////////////////////////////////////////////
#endif

