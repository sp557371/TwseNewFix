//---------------------------------------------------------------------------
#ifndef PkConnectedH
#define PkConnectedH
//---------------------------------------------------------------------------
#include "SesPk.h"
#include "Signon/Policy/PolicyBase.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Pk {
/////////////////////////////////////////////////////////////////////////////
using Kway::Signon::Policy::TPolicyBaseRef;
/////////////////////////////////////////////////////////////////////////////
//●Connected:由 Client端在 OnLinkReady()時送出
//               Server回覆 ServerName,及允許登入的群組(可能沒有)
static const TMsgCode   mcConnected = 0x0000;
//---------------------------------------------------------------------------
class TConnectedReq : public THead
{
   typedef TConnectedReq   this_type;
   typedef TPkRec          TFiller;
public:
   //----- for server -----
   static inline K_mf(TSize) GetMinSize      ()         { return sizeof(this_type) - sizeof(TFiller); }
   inline K_mf(TSize)        GetExpectSize   () const   { word PolicyLen = ( HiLoRef_ == THiLoTransRef::DwordLocalMachine_
                                                                              ? PolicyLen_
                                                                              : HiLoTransT(PolicyLen_, THiLoTransRef(HiLoRef_)) );
                                                          return static_cast<TSize>(sizeof(*this) - sizeof(Filler_) + ApNameLen_ + PolicyLen); }
   inline K_mf(THiLoTransRef) GetHiLoRef  () const      { return THiLoTransRef(HiLoRef_); }
          K_mf(void)          HiLoTrans   ()            { static const THiLoTransDef   defs[] =
                                                                        { MAC_HiLoTransDef(this_type, ApVer_.Major_),
                                                                          MAC_HiLoTransDef(this_type, ApVer_.Minor_),
                                                                          MAC_HiLoTransDef(this_type, ApVer_.Release_),
                                                                          MAC_HiLoTransDef(this_type, ApVer_.Build_),
                                                                          MAC_HiLoTransDef(this_type, PolicyLen_), };
                                                          Kway::HiLoTrans(const_cast<this_type*>(this), THiLoTransRef(HiLoRef_), defs, numofele(defs)); }
   //底下的 mf,必須先執行 HiLoTrans() 之後才會正確!
   inline K_mf(TVer)         GetApVer        () const   { return ApVer_; }
   inline K_mf(std::string)  GetApName       () const   { return std::string(ApName_,ApNameLen_); }
   inline K_mf(std::string)  GetPolicyNames  () const   { return std::string(ApName_+ApNameLen_,PolicyLen_); }
   //----- for client -----
   inline K_ctor  TConnectedReq  (const TVer& apVer, const std::string& apName,
                                  const char** policyNamesBegin, dword policyNamesCount)
            : HiLoRef_(THiLoTransRef::DwordLocalMachine_),
              ApVer_(apVer),
              ApNameLen_(ApNameLen_ = static_cast<byte>(apName.length()))
            { memcpy(ApName_, apName.c_str(), ApNameLen_);
              PolicyLen_ = SetPolicyNames(ApName_+ApNameLen_, policyNamesBegin, policyNamesCount);
              SetMsgCode(mcConnected);
              SetPkSize(GetExpectSize()); }
private:
   dword       HiLoRef_;
   TVer        ApVer_;     //程式版本
   byte        ApNameLen_;
   word        PolicyLen_;
   union {
      TFiller  Filler_;
      char     ApName_[sizeof(TFiller)];  //工作站的程式名稱
   };
};
//---------------------------------------------------------------------------
class TConnectedAck : public THead
{
   typedef TConnectedAck   this_type;
   typedef TPkRec          TFiller;
public:
   //----- for server -----
   inline K_ctor  TConnectedAck  (const TVer& serverVer, const std::string& serverName)
            : HiLoRef_(THiLoTransRef::DwordLocalMachine_),
              ServerVer_(serverVer),
              ServerNameLen_(static_cast<byte>(serverName.length())),
              PolicyLen_(0)
            { SetMsgCode(mcConnected);
              memcpy(Dat_, serverName.c_str(), ServerNameLen_);
              SetPkSize(GetExpectSize()); }

   inline K_mf(void)  AddPolicy  (const std::string& policyName, const TPolicyBaseRef& ref)
               { PolicyLen_ += reinterpret_cast<TPolicyAck*>(Dat_+ServerNameLen_+PolicyLen_)->Set(policyName,ref);
                 SetPkSize(GetExpectSize()); }
   //----- for client -----
   static inline K_mf(TSize) GetMinSize    ()        { return sizeof(this_type) - sizeof(TFiller); }
   inline K_mf(TSize)        GetExpectSize () const  { word PolicyLen = ( HiLoRef_ == THiLoTransRef::DwordLocalMachine_
                                                                           ? PolicyLen_
                                                                           : HiLoTransT(PolicyLen_, THiLoTransRef(HiLoRef_)) );
                                                       return static_cast<TSize>(sizeof(*this) - sizeof(Filler_) + ServerNameLen_ + PolicyLen); }
   inline K_mf(THiLoTransRef)  GetHiLoRef     () const   { return THiLoTransRef(HiLoRef_); }
          K_mf(void)           HiLoTrans      ()         { static const THiLoTransDef   defs[] =
                                                                        { MAC_HiLoTransDef(this_type, ServerVer_.Major_),
                                                                          MAC_HiLoTransDef(this_type, ServerVer_.Minor_),
                                                                          MAC_HiLoTransDef(this_type, ServerVer_.Release_),
                                                                          MAC_HiLoTransDef(this_type, ServerVer_.Build_),
                                                                          MAC_HiLoTransDef(this_type, PolicyLen_), };
                                                           Kway::HiLoTrans(this, THiLoTransRef(HiLoRef_), defs, numofele(defs));
                                                           word ofs = 0;
                                                           while(ofs < PolicyLen_) {
                                                              TPolicyAck*  ack = reinterpret_cast<TPolicyAck*>(Dat_ + ServerNameLen_ + ofs);
                                                              ack->TransPolicyLen(THiLoTransRef(HiLoRef_));
                                                              ofs += ack->GetSize();
                                                           }
                                                          }
   //底下的 mf,必須先執行 HiLoTrans() 之後才會正確!
   inline K_mf(std::string)        GetServerName  () const   { return std::string(Dat_, ServerNameLen_); }
   inline K_mf(TVer)               GetServerVer   () const   { return ServerVer_; }
   inline K_mf(const TPolicyAck*)  GetPolicy      (word& ofs) const
            { if(ofs >= PolicyLen_)
                 return 0;
              const TPolicyAck*  ack = reinterpret_cast<const TPolicyAck*>(Dat_ + ServerNameLen_ + ofs);
              ofs += ack->GetSize();
              return ack; }
private:
   dword       HiLoRef_;
   TVer        ServerVer_;       // Server版本
   byte        ServerNameLen_;
   word        PolicyLen_;
   union {
      TFiller  Filler_;
      char     Dat_[sizeof(TFiller)];    // ServerName = HostName + ApName
   };
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Pk
} // namespace Kway
//---------------------------------------------------------------------------
#endif
