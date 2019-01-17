//---------------------------------------------------------------------------
#ifndef PkSignonH
#define PkSignonH
//---------------------------------------------------------------------------
#include "SesPk.h"
#include "pod_vector.h"
#include "fixed_num.h"
#include "Signon/Policy/PolicyBase.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Pk {
/////////////////////////////////////////////////////////////////////////////
using Kway::Signon::Policy::TOnlineID;
using Kway::Signon::Policy::TPolicyBaseRef;
/////////////////////////////////////////////////////////////////////////////
//
// 已使用的MsgCode及說明, 請參考 "cspkMsgCode.txt"
//
//------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
static const TMsgCode   mcSignon        = 0x0002;
static const TMsgCode   mcPolicyChanged = 0x0003;
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   TSignonFuncCode;
   enum TSignonFuncCode_t
#else
   enum TSignonFuncCode
#endif
{
   fnc_SignonUser,      // Client  -> Server : 登入訊息
   fnc_ChgPass,         // Client <-> Server
   fnc_SignonResult,    // Server  -> Client
   fnc_PolicyChanged,   // Server  -> Client : Policy異動.
   fnc_ChkTableRight,   // Client <-> Server : 檢查資料表權限(審核資料時使用)
} K_EnumPacked;
//---------------------------------------------------------------------------
class TSignonBase : public THead
{
   typedef TSignonBase  this_type;
protected:
   TSignonFuncCode   FnCode_;
   byte              UserLen_;

   K_ctor  TSignonBase  (char* Dat_, const std::string& user, TSignonFuncCode fnCode)
      : FnCode_(fnCode),
        UserLen_(static_cast<byte>(user.length()))
      { memcpy(Dat_, user.c_str(), UserLen_);
        SetMsgCode(mcSignon); }

   explicit K_ctor  TSignonBase  ()
      : FnCode_(fnc_PolicyChanged)
      , UserLen_(0)
      { SetMsgCode(mcSignon); 
      }
public:
   inline K_mf(TSignonFuncCode)  GetFnCode  () const  { return FnCode_; }

   inline K_mf(TWorker::TCheckResult)  PrevPkCheck  (TSize recvSize) const
            { if(GetPkSize() < sizeof(this_type))  // [訊息大小] 小於此訊息 [最小的期望值]
                 return TWorker::cpr_Error;        //    => cpr_Error
              if(recvSize < sizeof(this_type))     // 此訊息已收到的資料,尚無法判斷 PkSize 是否合理
                 return TWorker::cpr_Continue;     //    => cpr_Continue
              return TWorker::cpr_HeadOK; }
};
//---------------------------------------------------------------------------
struct TSignonReq
{
   //--------------------------------------
   class TSignonUser : public TSignonBase {
         typedef TSignonBase  inherited;
         typedef TSignonUser  this_type;
         word                    PolicyLen_; //每個Policy之間用 '\0' 分開
         byte                    PassLen_;
         TOnlineID::orig_type    OnlineID_;
         typedef TPkRec          TFiller;
         union {
            TFiller  Filler_;
            char     Dat_[];
         };
   public:
      //----- for client -----
      K_ctor  TSignonUser  (const std::string& user,  const std::string& pass, TOnlineID onlineID,
                            const char** policyNamesBegin, dword policyNamesCount,
                            THiLoTransRef serverHiLoRef)
         : inherited(Dat_, user, fnc_SignonUser),
           OnlineID_(onlineID)
         { PolicyLen_ = SetPolicyNames(Dat_ + UserLen_, policyNamesBegin, policyNamesCount);
           memcpy(Dat_ + UserLen_ + PolicyLen_, pass.c_str(), PassLen_ = static_cast<byte>(pass.length()));
           SetPkSize(GetExpectSize());
           static const THiLoTransDef   defs[] = { MAC_HiLoTransDef(this_type, PolicyLen_),
                                                   MAC_HiLoTransDef(this_type, OnlineID_), };
           Kway::HiLoTrans(this, serverHiLoRef, defs, numofele(defs)); }
      //----- for servr -----
      static inline K_mf(TSize) GetMinSize     ()        { return sizeof(this_type) - sizeof(TFiller); }
      inline K_mf(TSize)        GetExpectSize  () const  { return static_cast<TSize>(sizeof(*this) - sizeof(Filler_) + UserLen_ + PassLen_ + PolicyLen_); }
      inline K_mf(std::string)  GetUser        () const  { return std::string(Dat_,UserLen_); }
      inline K_mf(std::string)  GetPolicyNames () const  { return std::string(Dat_+UserLen_,PolicyLen_); }
      inline K_mf(std::string)  GetPass        () const  { return std::string(Dat_+UserLen_+PolicyLen_,PassLen_); }
      inline K_mf(TOnlineID)    GetOnlineID    () const  { return OnlineID_; }
   };
   //-----------------------------------
   class TChgPass : public TSignonBase {
         typedef TSignonBase  inherited;
         typedef TChgPass     this_type;
         typedef TPkRec       TFiller;
         byte        OldPassLen_;
         byte        NewPassLen_;
         union {
            TFiller  Filler_;
            char     Dat_[];
         };
   public:
      //----- for client -----
      K_ctor  TChgPass  (const std::string& user, const std::string& oldPass, const std::string& newPass)
                        // THiLoTransRef  //目前用不到
         : inherited(Dat_, user, fnc_ChgPass)
         { memcpy(Dat_+UserLen_,             oldPass.c_str(), OldPassLen_ = static_cast<byte>(oldPass.length()));
           memcpy(Dat_+UserLen_+OldPassLen_, newPass.c_str(), NewPassLen_ = static_cast<byte>(newPass.length()));
           SetPkSize(GetExpectSize()); }
      //----- for server -----
      static inline K_mf(TSize) GetMinSize     ()        { return sizeof(this_type) - sizeof(TFiller); }
      inline K_mf(TSize)        GetExpectSize  () const  { return static_cast<TSize>(sizeof(*this) - sizeof(Filler_)  + UserLen_ + OldPassLen_ + NewPassLen_); }
      inline K_mf(std::string)  GetUser        () const  { return std::string(Dat_,UserLen_); }
      inline K_mf(std::string)  GetOldPass     () const  { return std::string(Dat_+UserLen_,OldPassLen_); }
      inline K_mf(std::string)  GetNewPass     () const  { return std::string(Dat_+UserLen_+OldPassLen_,NewPassLen_); }
   };
   //-----------------------------------
   class TChkTableRight : public TSignonBase {
         typedef TSignonBase     inherited;
         typedef TChkTableRight  this_type;
         typedef TPkRec          TFiller;
         byte        PassLen_;
         byte        TableNameLen_;
         union {
            TFiller  Filler_;
            char     Dat_[];
         };
   public:
      //----- for client -----
      K_ctor  TChkTableRight  (const std::string& user, const std::string& pass, const std::string& tableName)
         : inherited(Dat_, user, fnc_ChkTableRight)
         { memcpy(Dat_+UserLen_,          pass.c_str(),      PassLen_ = static_cast<byte>(pass.length()));
           memcpy(Dat_+UserLen_+PassLen_, tableName.c_str(), TableNameLen_ = static_cast<byte>(tableName.length()));
           SetPkSize(GetExpectSize()); }
      //----- for server -----
      static inline K_mf(TSize) GetMinSize     ()        { return sizeof(this_type) - sizeof(TFiller); }
      inline K_mf(TSize)        GetExpectSize  () const  { return static_cast<TSize>(sizeof(*this) - sizeof(Filler_)  + UserLen_ + PassLen_ + TableNameLen_); }
      inline K_mf(std::string)  GetUser        () const  { return std::string(Dat_,UserLen_); }
      inline K_mf(std::string)  GetPass        () const  { return std::string(Dat_+UserLen_,PassLen_); }
      inline K_mf(std::string)  GetTableName   () const  { return std::string(Dat_+UserLen_+PassLen_,TableNameLen_); }
   };
   //---------------------------------------------
   static inline K_mf(TWorker::TCheckResult)  PkCheck  (const TSignonBase& pk, TSize recvSize)
      { TWorker::TCheckResult cpr = pk.PrevPkCheck(recvSize);
        if(cpr == TWorker::cpr_HeadOK)
           switch(pk.GetFnCode()) {
           case fnc_SignonUser:     return TWorker::PkCheck(static_cast<const TSignonUser&>(pk), recvSize);
           case fnc_ChgPass:        return TWorker::PkCheck(static_cast<const TChgPass&>(pk),    recvSize);
           case fnc_ChkTableRight:  return TWorker::PkCheck(static_cast<const TChkTableRight&>(pk), recvSize);
           default:                 return TWorker::cpr_Error;
           }
        return cpr;
      }
};
//---------------------------------------------------------------------------
struct TSignonAck
{
   //----------------------------------------
   class TSignonResult : public TSignonBase {
         typedef TSignonBase     inherited;
         typedef TSignonResult   this_type;
         word                    PolicyLen_;
         TOnlineID::orig_type    OnlineID_;
         typedef TPkRec          TFiller;
         union {
            TFiller  Filler_;
            char     Dat_[sizeof(TFiller)];
         };
   protected:
      //----- for Policy Changed
      K_ctor  TSignonResult  ()
               : inherited()
               , PolicyLen_(0)
               , OnlineID_()
               { SetPkSize(GetExpectSize());
               }

   public:
      //----- for server -----
      K_ctor  TSignonResult  (const std::string& user, const std::string& result, TOnlineID onlineID)
               : inherited(Dat_, user, fnc_SignonResult),
                 PolicyLen_(static_cast<word>(result.length())),
                 OnlineID_(onlineID)
               { memcpy(Dat_ + UserLen_, result.c_str(), PolicyLen_);
                 SetPkSize(GetExpectSize()); }

      //----- for [Signon OK] & [Policy Changed]
      inline K_mf(bool)  AddPolicy  (const std::string& policyName, const TPolicyBaseRef& ref)
               { if(ref.first==0  ||  ref.second==0)
                    return false;
                 PolicyLen_ += reinterpret_cast<TPolicyAck*>(Dat_ + UserLen_ + PolicyLen_)->Set(policyName, ref);
                 SetPkSize(GetExpectSize());
                 return true;
               }
   private:
      inline K_mf(TSize)        GetExpectSize  () const  { return static_cast<TSize>(sizeof(*this) - sizeof(Filler_) + UserLen_ + PolicyLen_); }
   public:  //----- for client -----
      static inline K_mf(TSize) GetMinSize     ()        { return sizeof(this_type) - sizeof(TFiller); }
             K_mf(TSize)        GetExpectSize  (THiLoTransRef ref) const
                                                         { if(ref.IsEqLocal())   return GetExpectSize();
                                                           return static_cast<TSize>(sizeof(*this) - sizeof(Filler_) + UserLen_ + HiLoTransT(PolicyLen_,ref)); }
      K_mf(void)  HiLoTrans  (THiLoTransRef ref)  { static const THiLoTransDef   defs[] =
                                                         { MAC_HiLoTransDef(this_type, OnlineID_),
                                                           MAC_HiLoTransDef(this_type, PolicyLen_), };
                                                    Kway::HiLoTrans(this, ref, defs, numofele(defs));
                                                    word ofs = 0;
                                                    while(ofs < PolicyLen_) {
                                                      TPolicyAck*  ack = reinterpret_cast<TPolicyAck*>(Dat_+UserLen_+ofs);
                                                      ack->TransPolicyLen(ref);
                                                      ofs += ack->GetSize();
                                                    }
                                                  }
      //底下的 mf,必須先執行 HiLoTrans() 之後才會正確!
      inline K_mf(bool)               IsSuccess    () const  { return OnlineID_ > 0; }
      inline K_mf(TOnlineID)          GetOnlineID  () const  { return OnlineID_;     }
      inline K_mf(std::string)        GetUser      () const  { return std::string(Dat_,UserLen_); }
      inline K_mf(std::string)        GetResult    () const  { return IsSuccess() ? std::string() : std::string(Dat_+UserLen_, PolicyLen_); }
      inline K_mf(const TPolicyAck*)  GetPolicy    (word& ofs) const
               { if(ofs >= PolicyLen_)
                    return 0;
                 const TPolicyAck*  ack = reinterpret_cast<const TPolicyAck*>(Dat_+UserLen_+ofs);
                 ofs += ack->GetSize();
                 return ack; }
   };
   //-----------------------------------
   class TPolicyChanged : public TSignonResult
   {
   public:
      inline K_ctor  TPolicyChanged  ()   : TSignonResult()    {}
   };
   //-----------------------------------
   class TChgPass : public TSignonBase {
      typedef TSignonBase  inherited;
      typedef TChgPass     this_type;
      typedef char         TFiller[256*2];
      byte     ResultLen_;
      union {
         TFiller  Filler_;   // user + result
         char     Dat_[];
      };
   public:
      //----- for server -----
      K_ctor  TChgPass  (const std::string& user, const std::string& result)
         : inherited(Dat_, user, fnc_ChgPass)
         { memcpy(Dat_ + UserLen_, result.c_str(), ResultLen_ = static_cast<byte>(result.length()));
           SetPkSize(GetExpectSize()); }
   private:
      inline K_mf(TSize)        GetExpectSize  () const  { return static_cast<TSize>(sizeof(*this) - sizeof(Filler_)  + UserLen_ + ResultLen_); }
   public:  //----- for client -----
      static inline K_mf(TSize) GetMinSize     ()        { return sizeof(this_type) - sizeof(TFiller); }
      inline K_mf(TSize)        GetExpectSize  (THiLoTransRef) const
                                                         { return static_cast<TSize>(sizeof(*this) - sizeof(Filler_)  + UserLen_ + ResultLen_); }
      inline K_mf(bool)         IsSuccess      () const  { return ResultLen_==0; }
      inline K_mf(std::string)  GetUser        () const  { return std::string(Dat_,UserLen_); }
      inline K_mf(std::string)  GetResult      () const  { return std::string(Dat_+UserLen_, ResultLen_); }
   };
   //-----------------------------------
   class TChkTableRight : public TSignonBase {
      typedef TSignonBase     inherited;
      typedef TChkTableRight  this_type;
      typedef char            TFiller[256*2];
      byte     TableNameLen_;
      byte     ResultLen_;
      union {
         TFiller  Filler_;   // user + result
         char     Dat_[];
      };
   public:
      //----- for server -----
      K_ctor  TChkTableRight  (const std::string& user, const std::string& tableName, const std::string& result)
         : inherited(Dat_, user, fnc_ChkTableRight)
         { memcpy(Dat_ + UserLen_,                 tableName.c_str(), TableNameLen_ = static_cast<byte>(tableName.length()));
           memcpy(Dat_ + UserLen_ + TableNameLen_, result.c_str(),    ResultLen_ = static_cast<byte>(result.length()));
           SetPkSize(GetExpectSize()); }
   private:
      inline K_mf(TSize)        GetExpectSize  () const  { return static_cast<TSize>(sizeof(*this) - sizeof(Filler_)  + UserLen_ + TableNameLen_ + ResultLen_); }
   public:  //----- for client -----
      static inline K_mf(TSize) GetMinSize     ()        { return sizeof(this_type) - sizeof(TFiller); }
      inline K_mf(TSize)        GetExpectSize  (THiLoTransRef) const
                                                         { return static_cast<TSize>(sizeof(*this) - sizeof(Filler_)  + UserLen_ + TableNameLen_ + ResultLen_); }
      inline K_mf(bool)         IsSuccess      () const  { return ResultLen_==0; }
      inline K_mf(std::string)  GetUser        () const  { return std::string(Dat_,UserLen_); }
      inline K_mf(std::string)  GetTableName   () const  { return std::string(Dat_+UserLen_, TableNameLen_); }
      inline K_mf(std::string)  GetResult      () const  { return std::string(Dat_+UserLen_+TableNameLen_, ResultLen_); }
   };
   //---------------------------------------------
   static inline K_mf(TWorker::TCheckResult)  PkCheck  (const TSignonBase& pk, TSize recvSize, THiLoTransRef ref)
      { TWorker::TCheckResult cpr = pk.PrevPkCheck(recvSize);
        if(cpr == TWorker::cpr_HeadOK)
           switch(pk.GetFnCode()) {
           case fnc_PolicyChanged: return TWorker::PkCheckHiLoRef(static_cast<const TPolicyChanged&>(pk), recvSize, ref);
           case fnc_SignonResult:  return TWorker::PkCheckHiLoRef(static_cast<const TSignonResult&>(pk),  recvSize, ref);
           case fnc_ChgPass:       return TWorker::PkCheckHiLoRef(static_cast<const TChgPass&>(pk),       recvSize, ref);
           case fnc_ChkTableRight: return TWorker::PkCheckHiLoRef(static_cast<const TChkTableRight&>(pk), recvSize, ref);
           default:                return TWorker::cpr_Error;
           }
        return cpr;
      }
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
} // namespace Pk
} // namespace Kway
//---------------------------------------------------------------------------
#endif

