/**
@file TwStkClOrdIDMgr.h
@brief TwStk 的 ClOrdID管理員.
@version $Id: TwStkClOrdIDMgr.h,v 1.2 2005/01/14 07:50:21 fonwin Exp $

※ 由於產生 ClOrdID 對照表會耗用較多的搜尋時間,
   所以系統並不會預先建立 ClOrdIDMgr,
   而是請您在需要用到的時候自行建立及並將需要的ClOrdID交給他管理.
   例如:
     SesFIX 有提供 A,B,C 3家法人連線下單,
     則在[法人端]登入SesFIX後, 判斷此連線是 A 或 B 或 C,
     然後分別找出對應的 ClOrdIDMgr:
       // static
       TClOrdIDMgr* TSesFIX::GetClOrdIDMgr (dword cliGroup)
       {  static TClOrdIDMgr*   Mgr_[3];
          if(cliGroup >= numofele(Mgr_))
             return 0;
          if(TClOrdIDMgr* mgr = Mgr_[cliGroup])
             return mgr;
          return Mgr_[cliGroup] = new TClOrdIDMgr();
       }
   或是您可以使用 TClOrdGrpMgr 協助管理多個 ClOrdIDMgr.

※ ClOrdIDMgr提供的服務:
     (1) 提供查詢:
            1. 用 ClOrdID   查 SysOrdID, TseOrdKey
            2. 用 SysOrdID  查 ClOrdID-lists
            3. 用 TseOrdKey 查 ClOrdID-lists
            4. 用 ClOrdID   查 ClOrdID-lists
     (2) 提供:新增/刪除 ClOrdID.
     (3) 多主機之間同步 (使用 ClOrdID + TseOrdKey)

※ TAutoClOrdGrpMgr 可經由 [委託書異動事件] 來自動處理 ClOrdIDMgr.
   如果有需要您可以自行產生 TAutoClOrdGrpMgr.
   要注意的是:
      (1) 您必須要在 TOrdCenter 建立之後, 立刻建立 TAutoClOrdGrpMgr,
          避免有新的委託異動(委託成交回報、委託要求、委託同步...)沒有處理到.
      (2) ClOrdGrp 為 UserID 的前 4 碼.
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
// ClOrdID 的群組, 相同群組的 ClOrdID 不可重複.
// 記錄同步log時會用到,避免同步時匯入其他群組的ClOrdID.
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

   // 用 TCli 找 SysOrdID
   typedef std::map<TCli, TSysOrdID>   TCli2SysOrdID;
   TCli2SysOrdID                       Cli2SysOrdID_;

   // 每筆委託都有一個 ClOrdID-list.
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

   // 已知 TseOrdKey, 但還沒 SysOrdID 的 ClOrdID,
   // (發生原因:可能由其他主機收單,但委託資料尚未同步完成)
   // 由於 ClOrdID 最終還是會對應到 SysOrdID,
   // 所以此處的資料僅在尚未對應到 SysOrdID 之前有效.
   typedef std::map<TCli, TTseOrdKey>        TCli2TseOrdKey;
   TCli2TseOrdKey                            Cli2TseOrdKey_;
   typedef std::map<TTseOrdKey, TCliListRec> TTseOrdKey2CliList;
   TTseOrdKey2CliList                        TseOrdKey2CliList_;
};
/////////////////////////////////////////////////////////////////////////////
/** 提供處理從 [in/out log檔] 建立 map 的服務.
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

   /** 預設使用 ordSource.UserID_ 的前 4 碼.
       ordSource = u.GetConst()->LastSource_.Time_.IsInvalid()
                     ? u.GetConst()->OrigSource_
                     : u.GetConst()->LastSource_;
       傳回 TClOrdGrp(0) 表示, 不加入 ClOrdID.
   */
   virtual K_mf(TClOrdGrp)  GetClOrdGrp  (TSysOrds::TUpdaterImpl&, const TOrdSource&);
public:
   /** logFileName: 記錄 ClOrdID 的明細.
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

