/**
@file TwStkClOrdIDMgr.h
@brief TwStk �� ClOrdID�޲z��.
@version $Id: TwStkClOrdIDMgr.h,v 1.2 2005/01/14 07:50:21 fonwin Exp $

�� �ѩ󲣥� ClOrdID ��Ӫ�|�ӥθ��h���j�M�ɶ�,
   �ҥH�t�Ψä��|�w���إ� ClOrdIDMgr,
   �ӬO�бz�b�ݭn�Ψ쪺�ɭԦۦ�إߤΨñN�ݭn��ClOrdID�浹�L�޲z.
   �Ҧp:
     SesFIX ������ A,B,C 3�a�k�H�s�u�U��,
     �h�b[�k�H��]�n�JSesFIX��, �P�_���s�u�O A �� B �� C,
     �M����O��X������ ClOrdIDMgr:
       // static
       TClOrdIDMgr* TSesFIX::GetClOrdIDMgr (dword cliGroup)
       {  static TClOrdIDMgr*   Mgr_[3];
          if(cliGroup >= numofele(Mgr_))
             return 0;
          if(TClOrdIDMgr* mgr = Mgr_[cliGroup])
             return mgr;
          return Mgr_[cliGroup] = new TClOrdIDMgr();
       }
   �άO�z�i�H�ϥ� TClOrdGrpMgr ��U�޲z�h�� ClOrdIDMgr.

�� ClOrdIDMgr���Ѫ��A��:
     (1) ���Ѭd��:
            1. �� ClOrdID   �d SysOrdID, TseOrdKey
            2. �� SysOrdID  �d ClOrdID-lists
            3. �� TseOrdKey �d ClOrdID-lists
            4. �� ClOrdID   �d ClOrdID-lists
     (2) ����:�s�W/�R�� ClOrdID.
     (3) �h�D�������P�B (�ϥ� ClOrdID + TseOrdKey)

�� TAutoClOrdGrpMgr �i�g�� [�e�U�Ѳ��ʨƥ�] �Ӧ۰ʳB�z ClOrdIDMgr.
   �p�G���ݭn�z�i�H�ۦ沣�� TAutoClOrdGrpMgr.
   �n�`�N���O:
      (1) �z�����n�b TOrdCenter �إߤ���, �ߨ�إ� TAutoClOrdGrpMgr,
          �קK���s���e�U����(�e�U����^���B�e�U�n�D�B�e�U�P�B...)�S���B�z��.
      (2) ClOrdGrp �� UserID ���e 4 �X.
*/
//---------------------------------------------------------------------------
#ifndef TwStkClOrdIDMgrH
#define TwStkClOrdIDMgrH
//---------------------------------------------------------------------------
#include "TwStkOrdCenter.h"
#include "TextReader.h"
#include <list>
//---------------------------------------------------------------------------
namespace Kway {
namespace Tw {
namespace Stk {
/////////////////////////////////////////////////////////////////////////////
// ClOrdID ���s��, �ۦP�s�ժ� ClOrdID ���i����.
// �O���P�Blog�ɷ|�Ψ�,�קK�P�B�ɶפJ��L�s�ժ�ClOrdID.
typedef Kstl::fixed_num<dword>   TClOrdGrp;
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
struct TClOrdLogRec {
   inline K_ctor  TClOrdLogRec  () {}
   inline K_ctor  TClOrdLogRec  ( const TClOrdGrp&            clOrdGrp
                                , const TOrdSource::TClOrdID& clOrdID
                                , const TTseOrdKey&           tseOrdKey
                                , const TOrigSysOrdID&        origSysOrdID
                                , bool                        add)
      : Time_(TKDateTime::Now())
      , ClOrdGrp_(clOrdGrp)
      , ClOrdID_(clOrdID)
      , TseOrdKey_(tseOrdKey)
      , OrigSysOrdID_(origSysOrdID)
      , IsAdd_(add)
      { }

   TKDateTime              Time_;
   TClOrdGrp               ClOrdGrp_;
   TOrdSource::TClOrdID    ClOrdID_;
   TTseOrdKey              TseOrdKey_;
   TOrigSysOrdID           OrigSysOrdID_;
   bool                    IsAdd_;
};
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TClOrdIDMap : public TOnNewTseOrdKey
{
public:
   // types.
   typedef TOrdSource::TClOrdID     TCli;
   typedef std::list<const TCli*>   TCliList;

   // ctor/dtor.
   K_ctor  TClOrdIDMap  (const TClOrdGrp&, TOrdCenter&, TFile* outLog);
   K_dtor ~TClOrdIDMap  ();

   // member functions.
   inline K_mf(bool)  AddClOrdID  (const TCli& cli, const TTseOrdKey& id) { return AddClOrdID(cli,id,true); }
   inline K_mf(bool)  AddClOrdID  (const TCli& cli, const TSysOrdID&  id) { return AddClOrdID(cli,id,true); }
   inline K_mf(bool)  DelClOrdID  (const TCli& cli)                       { return DelClOrdID(cli,true);    }
          K_mf(void)  OnLogRec    (const TClOrdLogRec&);

   K_mf(TSysOrdID)        GetSysOrdID  (const TCli&)       const;
   K_mf(TTseOrdKey)       GetTseOrdKey (const TCli&)       const;
   K_mf(const TCliList*)  GetCliList   (const TSysOrdID&)  const;
   K_mf(const TCliList*)  GetCliList   (const TTseOrdKey&) const;
   K_mf(const TCliList*)  GetCliList   (const TCli&)       const;

   inline K_mf(TClOrdGrp) GetClOrdGrp () const  { return ClOrdGrp_; }

protected:
   K_mf(bool)  AddClOrdID  (const TCli&, const TTseOrdKey&, bool needLog);
   K_mf(bool)  AddClOrdID  (const TCli&, const TSysOrdID&,  bool needLog);
   K_mf(bool)  DelClOrdID  (const TCli&, bool needLog);

private:
   K_mf(const TCliList*)  GetTseOrdKey2CliList  (const TTseOrdKey&) const;

   K_mf(ev_result)  OnNewTseOrdKey  (const TOnNewTseOrdKey::TArg&);
   K_mf(bool)       WriteLog        (const TCli&, const TTseOrdKey&, const TOrigSysOrdID&, bool add);

   TClOrdGrp   ClOrdGrp_;
   TOrdCenter* OrdCenter_;
   int         WaittingLog_;
   TFile*      OutLog_;

   // �� TCli �� SysOrdID
   typedef std::map<TCli, TSysOrdID>   TCli2SysOrdID;
   TCli2SysOrdID                       Cli2SysOrdID_;

   // �C���e�U�����@�� ClOrdID-list.
   struct TCliListRec {
      mutable TCliList  CliList_;
      mutable bool      WaitLog_;
      inline K_ctor  TCliListRec  () : WaitLog_(false) {}
             K_mf(bool)  erase  (const TCli*) const;
      inline K_mf(bool)  empty  () const   { return CliList_.empty(); }
   };
   typedef Kstl::ev_ary<TSysOrdID, TCliListRec>    TSysOrdID2CliList;
   TSysOrdID2CliList                               SysOrdID2CliList_;

   template <class TCli2OrdIDs, class TOrdID2CliList>
   static const TCliListRec*  AddClOrdID ( const typename TClOrdIDMap::TCli&
                                         , const typename TOrdID2CliList::key_type&
                                         , TCli2OrdIDs&
                                         , TOrdID2CliList&);

   // �w�� TseOrdKey, ���٨S SysOrdID �� ClOrdID,
   // (�o�ͭ�]:�i��Ѩ�L�D������,���e�U��Ʃ|���P�B����)
   // �ѩ� ClOrdID �̲��٬O�|������ SysOrdID,
   // �ҥH���B����ƶȦb�|�������� SysOrdID ���e����.
   typedef std::map<TCli, TTseOrdKey>        TCli2TseOrdKey;
   TCli2TseOrdKey                            Cli2TseOrdKey_;
   typedef std::map<TTseOrdKey, TCliListRec> TTseOrdKey2CliList;
   TTseOrdKey2CliList                        TseOrdKey2CliList_;
};
/////////////////////////////////////////////////////////////////////////////
/** ���ѳB�z�q [in/out log��] �إ� map ���A��.
*/
TWSTK_class TClOrdIDMgrBase : protected Kway::TSyncRecReader<TClOrdLogRec>   // for input log.
{
   typedef Kway::TSyncRecReader<TClOrdLogRec>   TInLog;
protected:
   TFile    OutLogFile_; // for output log.

   K_mf(void)  ImportClOrdIDFromLog  ();
public:
   K_ctor  TClOrdIDMgrBase  ( const std::string& inLogFileName
                            , const std::string& outLogFileName );
};
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TClOrdIDMgr : protected TClOrdIDMgrBase
                        , public    TClOrdIDMap
{
   K_mf(void)  OnSyncRec  (TClOrdLogRec&);
public:
   K_ctor  TClOrdIDMgr  ( const TClOrdGrp& clOrdGrp, TOrdCenter& ordCenter
                        , const std::string& inLogFileName
                        , const std::string& outLogFileName );
};
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TClOrdGrpMgr : protected TClOrdIDMgrBase
{
   typedef std::map<TClOrdGrp, TClOrdIDMap>     TClOrdIDMaps;
   TOrdCenter*    OrdCenter_;
   TClOrdIDMaps   ClOrdIDMaps_;
   K_mf(void)  OnSyncRec  (TClOrdLogRec&);
public:
   K_ctor  TClOrdGrpMgr  ( TOrdCenter&
                         , const std::string& inLogFileName
                         , const std::string& outLogFileName );

   K_mf(TClOrdIDMap&)  GetClOrdIDMgr  (const TClOrdGrp&);

   inline K_mf(TOrdCenter&)  GetOrdCenter  () const   { return *OrdCenter_; }
};
/////////////////////////////////////////////////////////////////////////////
TWSTK_class TAutoClOrdGrpMgr : public TClOrdGrpMgr
                             , public TSysOrdsEvHandler
{
   typedef TClOrdGrpMgr    inherited;
   K_mf(TEvResult)  OnOrdAfEndUpdate  (TSysOrds::TUpdaterImpl&);

   /** �w�]�ϥ� ordSource.UserID_ ���e 4 �X.
       ordSource = u.GetConst()->LastSource_.Time_.IsInvalid()
                     ? u.GetConst()->OrigSource_
                     : u.GetConst()->LastSource_;
       �Ǧ^ TClOrdGrp(0) ���, ���[�J ClOrdID.
   */
   virtual K_mf(TClOrdGrp)  GetClOrdGrp  (TSysOrds::TUpdaterImpl&, const TOrdSource&);
public:
   /** logFileName: �O�� ClOrdID ������.
   */
   K_ctor  TAutoClOrdGrpMgr  (TOrdCenter&, const std::string& logFileName);
   K_dtor ~TAutoClOrdGrpMgr  ();

   K_mf(TClOrdIDMap&)  GetClOrdIDMgr  (const TOrdSource::TUserID&);
   using inherited::GetClOrdIDMgr;
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Stk
} // namespace Tw
} // namespace Kway
//---------------------------------------------------------------------------
#endif

