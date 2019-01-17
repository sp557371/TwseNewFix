//---------------------------------------------------------------------------
#ifndef PkSesVAH
#define PkSesVAH
//---------------------------------------------------------------------------
#include "SesPk.h"
#include "ViewArea.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Pk {
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
static const TMsgCode   mcSesVA = 0x010;
//---------------------------------------------------------------------------
typedef word   TSesVALayer;
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   TSesVAFunc;
   enum TSesVAFunc_t
#else
   enum TSesVAFunc
#endif
{ //for Server Ack
      vaf_VACrDetails,
      vaf_VAFields,
      vaf_VAFieldEnums,
      vaf_VAResetRecs,
      vaf_VARecStrs,
      vaf_VARecInsStrs,
      vaf_VARecErase,
      vaf_VACurRowMoved,
      vaf_VAIdName,
      vaf_SettingFail,
} K_EnumPacked;
//---------------------------------------------------------------------------
#ifdef K_EnumPacked_Bug
   typedef byte   TSesVAReqFunc;
   enum TSesVAReqFunc_t
#else
   enum TSesVAReqFunc
#endif
{ //for Client Req
      varf_UpdateField,
      varf_MoveCurRow,
      varf_SelectDetail,
      varf_GetEditEnums,
      varf_FindKey,
      varf_FindLowerBound,
      varf_EraseKey,
      varf_FetchKey,
      varf_FetchCopyCurrent,
      varf_SwapCurrent,
      varf_SettingApply,
      varf_SettingCancel,
} K_EnumPacked;
//---------------------------------------------------------------------------
struct TSesVAField
{
   word                          FieldDisplayWidth_;  //欄位顯示寬度(pixels) = FieldDisplayWidth_ * 字元寬度(pixels) (不可為0)
   TAlignment                    TitleAlignment_;     //標題的對齊方式
   TAlignment                    FieldAlignment_;     //欄位資料的對齊方式
   TFieldStyle                   FieldStyle_;
   char                          PasswordChar_;
   TVAGetEditProp::TEditStyle    EnumsStyle_;
   char                          Title_[];

   K_mf(void)  HiLoTrans  (THiLoTransRef ref)
      { static const THiLoTransDef   defs[] =
                     { MAC_HiLoTransDef(TSesVAField, FieldDisplayWidth_),
                       MAC_HiLoTransDef(TSesVAField, FieldStyle_), };
        Kway::HiLoTrans(this, ref, defs, numofele(defs)); }
};
//---------------------------------------------------------------------------
class TStrs {
   TStrings                   Strs_;
   std::vector<const char*>   CStrs_;

   inline K_mf(void)  Init  ()
      { size_t count = Strs_.size();
        CStrs_.resize(count);
        for(size_t L = 0;  L < count;  ++L)
           CStrs_[L] = Strs_[L].c_str();
      }
public:
   inline K_ctor      TStrs  (const TStrings& strs)   : Strs_(strs), CStrs_()  { Init(); }
   inline K_ctor      TStrs  ()                       { }
   inline K_mf(void)  Assign (const TStrings& strs)   { Strs_ = strs;  Init(); }

   inline K_mf(size_t)        size   () const    { return Strs_.size(); }
   inline K_mf(const char**)  get    () const    { if(Strs_.empty()) return 0;
                                                   const char* const* a = &CStrs_[0];
                                                   return (const char**)a; }
   inline K_mf(void)          clear  ()          { Strs_.clear(); CStrs_.clear(); }
   inline K_mf(bool)          empty  () const    { return CStrs_.empty();  }
};
/////////////////////////////////////////////////////////////////////////////
class TSesVAReq : public THead
{
   typedef TSesVAReq   this_type;
   typedef TPkRec      TFiller;
public:
   //----- for client -----
   //要求全部的 VAMgr-Names
   inline K_ctor  TSesVAReq  ()
            : Layer_(0),
              DatLen_(0)
            { SetMsgCode(mcSesVA);
              SetPkSize(GetExpectSize()); }
   //要求某 VAMgr 全部的 RegNames
   inline K_ctor  TSesVAReq  (word reqID, const char* vaMgrName, THiLoTransRef ref)
            : Layer_(0),
              DatLen_(static_cast<TSize>(sizeof(reqID)+strlen(vaMgrName)))
            { *reinterpret_cast<word*>(Dat_) = HiLoTransT(reqID,ref);
              memcpy(Dat_+sizeof(reqID), vaMgrName, DatLen_-sizeof(reqID));
              SetMsgCode(mcSesVA);
              SetPkSize(GetExpectSize());
              DatLen_ = HiLoTransT(DatLen_,ref); }
   //要求某筆資料(layer必定 > 1)
   inline K_ctor  TSesVAReq  (TSesVALayer layer, size_t distBegin, THiLoTransRef ref)
            : Layer_(HiLoTransT(layer,ref)),
              DatLen_(sizeof(dword))
            { *reinterpret_cast<dword*>(Dat_) = HiLoTransT((dword)distBegin,ref);
              SetMsgCode(mcSesVA);
              SetPkSize(GetExpectSize());
              DatLen_ = HiLoTransT(DatLen_,ref); }
   //varf_MoveCurRow   =移動某 VA 的 CurRow,
   //varf_SelectDetail =選擇某DetailTab,
   //varf_GetEditEnums =取得某欄位的Enums
   inline K_ctor  TSesVAReq  (TSesVAReqFunc fn, TSesVALayer layer, size_t distBegin, THiLoTransRef ref)
            : Layer_(HiLoTransT(TSesVALayer(1),ref)),//Layer_==1時,為特殊要求!
              DatLen_(sizeof(FuncReq))
            { reinterpret_cast<FuncReq*>(Dat_)->Fn_        = fn;
              reinterpret_cast<FuncReq*>(Dat_)->Layer_     = HiLoTransT(layer,ref);
              reinterpret_cast<FuncReq*>(Dat_)->DistBegin_ = HiLoTransT((dword)distBegin,ref);
              SetMsgCode(mcSesVA);
              SetPkSize(GetExpectSize());
              DatLen_ = HiLoTransT(DatLen_,ref); }
   //varf_FindKey  =尋找某筆資料
   //varf_EraseKey =刪除某筆資料
   inline K_ctor  TSesVAReq  (TSesVAReqFunc fn, TSesVALayer layer, const std::string& key, THiLoTransRef ref)
            : Layer_(HiLoTransT(TSesVALayer(1),ref)),//Layer_==1時,為特殊要求!
              DatLen_(static_cast<TSize>(sizeof(fn)+sizeof(layer)+key.length()))
            { reinterpret_cast<FuncReq*>(Dat_)->Fn_    = fn;
              reinterpret_cast<FuncReq*>(Dat_)->Layer_ = HiLoTransT(layer,ref);
              memcpy(reinterpret_cast<FuncReq*>(Dat_)->Key_, key.data(), key.length());
              SetMsgCode(mcSesVA);
              SetPkSize(GetExpectSize());
              DatLen_ = HiLoTransT(DatLen_,ref); }
   //更新某筆資料的某欄位
   inline K_ctor  TSesVAReq  (TSesVALayer layer, const std::string& key,
                              dword fieldNo, const std::string& newFieldValue,
                              THiLoTransRef ref)
            : Layer_(HiLoTransT(TSesVALayer(1),ref)),//Layer_==1時,為特殊要求!
              DatLen_(static_cast<TSize>(sizeof(FuncReq) + key.length() + newFieldValue.length() + 1))
            { reinterpret_cast<FuncReq*>(Dat_)->Fn_      = varf_UpdateField;
              reinterpret_cast<FuncReq*>(Dat_)->Layer_   = HiLoTransT(layer,ref);
              reinterpret_cast<FuncReq*>(Dat_)->FieldNo_ = HiLoTransT(fieldNo,ref);
              memcpy(Dat_+sizeof(FuncReq),                key.c_str(),          key.length()+1);
              memcpy(Dat_+sizeof(FuncReq)+key.length()+1, newFieldValue.data(), newFieldValue.length());
              SetMsgCode(mcSesVA);
              SetPkSize(GetExpectSize());
              DatLen_ = HiLoTransT(DatLen_,ref); }

   //----- for server -----
   inline K_mf(TSesVALayer)  GetLayer      () const  { return Layer_; }
   inline K_mf(TSize)        GetDatLen     () const  { return DatLen_; }
   static inline K_mf(TSize) GetMinSize    ()        { return sizeof(this_type) - sizeof(TFiller); }
   inline K_mf(TSize)        GetExpectSize () const  { return static_cast<TSize>(sizeof(*this) - sizeof(Filler_) + DatLen_); }
   inline K_mf(word)         GetVAMgrReqID () const  { return K_Ptr2Ref(reinterpret_cast<const word*>(Dat_)); }
   inline K_mf(std::string)  GetVAMgrName  () const  { return std::string(Dat_+sizeof(word),DatLen_-sizeof(word)); }
   inline K_mf(dword)        GetDistBegin  (TSize& ofs) const  { ofs += static_cast<TSize>(sizeof(dword));
                                                                 return K_Ptr2Ref(reinterpret_cast<const dword*>(Dat_)); }
   struct FuncReq {
      TSesVAReqFunc  Fn_;
      TSesVALayer    Layer_;
      union {
         dword       DistBegin_; //fn_ == varf_MoveCurRow
         dword       DetailID_;  //fn_ == varf_SelectDetail
         dword       FieldNo_;   //fn_ == varf_GetEditEnums
         int32       Direction_; //fn_ == varf_SwapCurrent
         char        Key_[];     //fn_ == varf_FindKey 、 varf_FindLowerBound 、 varf_EraseKey
      };
   };
   inline K_mf(const FuncReq*)  GetFuncReq () const  { return reinterpret_cast<const FuncReq*>(Dat_); }
   inline K_mf(std::string)     GetFuncKey () const  { return std::string(Dat_   +(sizeof(TSesVAReqFunc)+sizeof(TSesVALayer)),
                                                                          DatLen_-(sizeof(TSesVAReqFunc)+sizeof(TSesVALayer))); }
   inline K_mf(std::string)     GetFuncFieldKey () const  { const char* keyeos = static_cast<const char*>(memchr(Dat_+sizeof(FuncReq), 0, DatLen_-sizeof(FuncReq)));
                                                            return std::string( Dat_+sizeof(FuncReq),
                                                                                keyeos ? (keyeos-Dat_-sizeof(FuncReq))
                                                                                       : (DatLen_-sizeof(FuncReq)) ); }
   inline K_mf(std::string)     GetFuncFieldVal () const  { const char* keyeos = static_cast<const char*>(memchr(Dat_+sizeof(FuncReq), 0, DatLen_-sizeof(FuncReq)));
                                                            if(keyeos)  return std::string(keyeos+1, Dat_+DatLen_-keyeos-1);
                                                            return std::string(); }

private:
   TSesVALayer Layer_;
   TSize       DatLen_;
   union {
      char     Dat_[];
      TFiller  Filler_;
   };
};
//---------------------------------------------------------------------------
class TSesVAAck : public THead
{
   typedef TSesVAAck    this_type;
   typedef TPkRec       TFiller;

   K_mf(void)  Init  (const char** strs, size_t count)
            { if(count > 0) {
                do {
                  size_t len = strlen(*strs)+1;//+1 =含eos
                  memcpy(Dat_ + DatLen_, *strs, len);
                  DatLen_ += static_cast<word>(len);
                  ++strs;
                } while(--count > 0);
                if(**(--strs) != 0)
                  --DatLen_;//如果最後不是空字串,則不需含最後的eos
              }
              SetMsgCode(mcSesVA);
              SetPkSize(GetExpectSize()); }
   inline K_mf(TSize)  GetExpectSize () const  { return static_cast<TSize>(sizeof(*this) - sizeof(Filler_) + DatLen_); }
   typedef dword  TVACtrlsT;
public:
   //----- for server -----
   //回覆全部的 VAMgr-Names
   inline K_ctor  TSesVAAck  (const char** vaMgrNames, size_t count)
            : Layer_(0),
              DatLen_(0)
            { Init(vaMgrNames, count); }
   //回覆某 VAMgr 全部的 RegNames
   inline K_ctor  TSesVAAck  (word reqID, const char** strs, size_t count)
            : Layer_(1),
              DatLen_(sizeof(TSesVAFunc)+sizeof(word))
            {          *reinterpret_cast<TSesVAFunc*>(Dat_) = vaf_VAResetRecs;
              K_Ptr2Ref(reinterpret_cast<word*>(Dat_+sizeof(TSesVAFunc))) = reqID;
              Init(strs, count); }
   //回覆某項要求全部的字串資料
   inline K_ctor  TSesVAAck  (TSesVALayer layer, TSesVAFunc fn, const char** strs, size_t count)
            : Layer_(layer),
              DatLen_(sizeof(TSesVAFunc))
            { *reinterpret_cast<TSesVAFunc*>(Dat_) = fn;
              Init(strs, count); }
   //回覆VA-Infos 及 第一筆資料
   inline K_ctor  TSesVAAck  (TSesVALayer layer, size_t rowCount, const TViewControls& vcs,
                              const char** strs, size_t count)
            : Layer_(layer),
              DatLen_(sizeof(TSesVAFunc)+sizeof(dword)+sizeof(TVACtrlsT))
            {          *reinterpret_cast<TSesVAFunc*>(Dat_) = vaf_VAResetRecs;
              K_Ptr2Ref(reinterpret_cast<dword*>     (Dat_+sizeof(TSesVAFunc))) = (dword)rowCount;
              K_Ptr2Ref(reinterpret_cast<TVACtrlsT*> (Dat_+sizeof(TSesVAFunc)+sizeof(dword))) = static_cast<TVACtrlsT>(vcs.to_ulong());
              Init(strs, count); }
   //回覆某筆資料
   inline K_ctor  TSesVAAck  (TSesVALayer layer, TSesVAFunc fn, size_t distBegin, const char** strs, size_t count)
            : Layer_(layer),
              DatLen_(sizeof(fn)+sizeof(dword))
            {          *reinterpret_cast<TSesVAFunc*>(Dat_) = fn;
              K_Ptr2Ref(reinterpret_cast<dword*>(Dat_+sizeof(TSesVAFunc))) = (dword)distBegin;
              Init(strs, count); }
   //刪除資料通知
   inline K_ctor  TSesVAAck  (TSesVALayer layer, size_t distBegin, size_t eraseCount)
            : Layer_(layer),
              DatLen_(sizeof(TSesVAFunc)+sizeof(dword)+sizeof(dword))
            {          *reinterpret_cast<TSesVAFunc*>(Dat_) = vaf_VARecErase;
              K_Ptr2Ref(reinterpret_cast<dword*>(Dat_+sizeof(TSesVAFunc))) = (dword)distBegin;
              K_Ptr2Ref(reinterpret_cast<dword*>(Dat_+sizeof(TSesVAFunc)+sizeof(dword))) = (dword)eraseCount;
              SetMsgCode(mcSesVA);
              SetPkSize(GetExpectSize()); }
   //VA-CurRow移動通知(回覆 varf_FindKey)
   inline K_ctor  TSesVAAck  (TSesVALayer layer, size_t distBegin)
            : Layer_(layer),
              DatLen_(sizeof(TSesVAFunc)+sizeof(dword))
            {          *reinterpret_cast<TSesVAFunc*>(Dat_) = vaf_VACurRowMoved;
              K_Ptr2Ref(reinterpret_cast<dword*>(Dat_+sizeof(TSesVAFunc))) = (dword)distBegin;
              SetMsgCode(mcSesVA);
              SetPkSize(GetExpectSize()); }

   //通知設定失敗訊息.
   inline K_ctor  TSesVAAck  (TSesVALayer layer, const std::string& msg)
            : Layer_(layer),
              DatLen_(TSize(sizeof(TSesVAFunc)+msg.size()+1))
            { *reinterpret_cast<TSesVAFunc*>(Dat_) = vaf_SettingFail;
              memcpy(Dat_+sizeof(TSesVAFunc), msg.c_str(), msg.size()+1);
              SetMsgCode(mcSesVA);
              SetPkSize(GetExpectSize()); }

   inline K_mf(bool)  AddField  (const Kway::TFieldDesp& fd)
            { size_t titleLen = strlen(fd.Title_)+1;
              if((int)(sizeof(TPkRec) - (GetPkSize() + sizeof(TSesVAField) + titleLen)) < 0)
                 return false;
              TSesVAField* field = reinterpret_cast<TSesVAField*>(Dat_ + DatLen_);
              field->FieldDisplayWidth_ = fd.FieldDisplayWidth_;
              field->TitleAlignment_    = fd.TitleAlignment_;
              field->FieldAlignment_    = fd.FieldAlignment_;
              field->FieldStyle_        = fd.FieldStyle_;
              field->PasswordChar_      = fd.PasswordChar_;
              bool   selectOnly;
              field->EnumsStyle_ = (fd.GetEditProp_ ? fd.GetEditProp_->GetEnums(*(TViewArea*)0,0,0,0,selectOnly)
                                                    : TVAGetEditProp::es_NotSupported);
              strcpy(field->Title_, fd.Title_);
              DatLen_ += static_cast<TSize>(sizeof(TSesVAField) + titleLen);
              SetPkSize(GetExpectSize());
              return true; }

   //----- for client -----
   inline K_mf(word)       GetLayer      (THiLoTransRef ref) const  { return HiLoTransT(Layer_,ref);  }
   inline K_mf(word)       GetDatLen     (THiLoTransRef ref) const  { return HiLoTransT(DatLen_,ref); }
   inline K_mf(TSesVAFunc) GetFunc       (THiLoTransRef ref) const  { return *reinterpret_cast<const TSesVAFunc*>(Dat_); }
   inline K_mf(word)       GetVAMgrReqID (THiLoTransRef ref) const  { return HiLoTransT(*reinterpret_cast<const word*>(Dat_+sizeof(TSesVAFunc)),ref); }
   inline K_mf(const char*)GetSettingMsg () const                   { return Dat_ + sizeof(TSesVAFunc); }
   inline K_mf(dword)      GetFirstN     (THiLoTransRef ref, TSize& ofs) const
                                                                    { ofs  = sizeof(TSesVAFunc)+sizeof(dword);
                                                                      return HiLoTransT(*reinterpret_cast<const dword*>(Dat_+sizeof(TSesVAFunc)),ref); }
   inline K_mf(dword)      GetRowCount   (THiLoTransRef ref, TSize& ofs) const   { return GetFirstN(ref,ofs); }
   inline K_mf(dword)      GetFieldNo    (THiLoTransRef ref, TSize& ofs) const   { return GetFirstN(ref,ofs); }
   inline K_mf(dword)      GetDistBegin  (THiLoTransRef ref, TSize& ofs) const   { return GetFirstN(ref,ofs); }
   inline K_mf(dword)      GetEraseCount (THiLoTransRef ref, TSize& ofs) const
                                                                    { ofs = sizeof(TSesVAFunc)+sizeof(dword)+sizeof(dword);
                                                                      return HiLoTransT(*reinterpret_cast<const dword*>(Dat_+sizeof(TSesVAFunc)+sizeof(dword)),ref); }
   inline K_mf(TViewControls) GetViewControls (THiLoTransRef ref, TSize& ofs) const
                                                            { const TVACtrlsT* rtn = reinterpret_cast<const TVACtrlsT*>(Dat_+ofs);
                                                              ofs += static_cast<TSize>(sizeof(TVACtrlsT));
                                                              return TViewControls(HiLoTransT(*rtn,ref)); }

   inline K_mf(std::string)  GetStr    (THiLoTransRef ref, TSize& ofs) const
            { TSize  DatLen = HiLoTransT(DatLen_,ref);
              if(ofs >= DatLen)
                 return std::string();
              TSesVALayer  Layer = HiLoTransT(Layer_,ref);
              if(ofs==0  &&  Layer > 0) {
                 ofs += static_cast<TSize>(sizeof(TSesVAFunc));
                 if(Layer==1)
                    ofs += static_cast<TSize>(sizeof(word));//ReqID
              }
              const char*  peos = static_cast<const char*>(memchr(Dat_+ofs, 0, DatLen-ofs));
              if(peos == 0) {//已經沒有eos了
                 std::string  result(Dat_+ofs, DatLen-ofs);
                 ofs = DatLen;
                 return result;
              }
              std::string  result(Dat_+ofs);
              ofs += static_cast<TSize>(result.length()+1);
              return result; }
   inline K_mf(void)  GetStrs   (THiLoTransRef ref, TStrings& strs, TSize ofs) const
            { TSize  DatLen = HiLoTransT(DatLen_,ref);
              while(ofs < DatLen)
                 strs.push_back(GetStr(ref,ofs));
              if(strs.size()==1  &&  strs[0].empty())
                 strs.clear();
            }
   inline K_mf(const TSesVAField*)  GetField  (THiLoTransRef ref, TSize& ofs)
            { TSize  DatLen = HiLoTransT(DatLen_,ref);
              if(ofs >= DatLen)
                 return 0;
              TSesVALayer  Layer = HiLoTransT(Layer_,ref);
              if(ofs==0  &&  Layer > 0)
                 ofs += static_cast<TSize>(sizeof(TSesVAFunc));
               TSesVAField*   fd = reinterpret_cast<TSesVAField*>(Dat_ + ofs);
               ofs += static_cast<TSize>(sizeof(TSesVAField) + strlen(fd->Title_) + 1);
               fd->HiLoTrans(ref);
               return fd; }

   static inline K_mf(TSize) GetMinSize    ()                         { return sizeof(this_type) - sizeof(TFiller); }
   inline K_mf(TSize)        GetExpectSize (THiLoTransRef ref) const  { return static_cast<TSize>(sizeof(*this) - sizeof(Filler_) + HiLoTransT(DatLen_,ref)); }
private:
   TSesVALayer Layer_;  //第n層: 0=全部的 VAMgr-Names:每個Name中間用eos分開
                        //       1=某 VAMgr 全部的 [資料表]-Names
   TSize       DatLen_;
   union {
      char     Dat_[];
      TFiller  Filler_;
   };
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

