//---------------------------------------------------------------------------
#ifndef PkOMSH
#define PkOMSH
//---------------------------------------------------------------------------
#include "SesPk.h"
#include "kpersistent.h"
#include "OmsOrderFactory.h"
//---------------------------------------------------------------------------
namespace Kway {
namespace Pk {
/**
BCB: 若使用 console mode, 請記得加上 -b- 選項.
*/
/////////////////////////////////////////////////////////////////////////////
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(push,1)
#else
#pragma pack(1)
#endif
//---------------------------------------------------------------------------
static const TMsgCode   mcOMS = 0x050;
/////////////////////////////////////////////////////////////////////////////
#ifdef K_EnumPacked_Bug
   typedef byte   TPkFrOMSFunc;
   enum TPkFrOMSFunc_t
#else
   enum TPkFrOMSFunc
#endif
{
   omsf_Data,        // 若一次傳遞大量資料( > 4K),則可利用此封包,最後再傳遞實際的FuncCode.
   omsf_RecoverOrder,
   omsf_RecoverEnd,
   omsf_OrderUpdate,
   omsf_OrderErase,
   omsf_OrderPartsUpdate,
   omsf_SyTabUpdate,
   omsf_AuditLog,    // ParentID + ChildID + [dword+string]
   omsf_ReqCmdResult,
   omsf_ReqCmdPending,
} K_EnumPacked;
//---------------------------------------------------------------------------
struct TPkFrOMSHead : public THead
{
   TSize    DatLen_;
};
//---------------------------------------------------------------------------
/** Package from OMS.
*/
class TPkFrOMS : public TPkFrOMSHead
{
   typedef TPkFrOMS  this_type;

   enum {   DatSize = sizeof(TPkRec)-sizeof(TPkFrOMSHead)   };
   char     Dat_[DatSize];

   inline K_mf(TSize) GetExpectSize () const    { return static_cast<TSize>(GetMinSize() + DatLen_); }
public:
   //----- for OMS Server -----
   inline K_ctor  TPkFrOMS  (TPkFrOMSFunc func)
            {  SetMsgCode(TMsgCode(mcOMS+func));
               SetDatLen(0);
            }
   /** for OMS Server: TPkAry<>
   */
   inline K_ctor TPkFrOMS ()
            {  SetMsgCode(TMsgCode(mcOMS+omsf_Data));
            }
   inline K_mf(void)  SetDatLen  (TSize sz)
            {  DatLen_ = sz;
               SetPkSize(GetExpectSize());
            }
   inline K_mf(void)  SetDatLen  (TSize sz, THiLoTransRef)
            {  SetDatLen(sz);
            }
   inline static K_mf(TSize) GetMaxDatSize ()   { return DatSize-1; }

   //----- for Client -----
   static inline K_mf(TSize) GetMinSize    ()                        { return sizeof(this_type) - DatSize; }
   inline K_mf(const void*)  GetDat        () const                  { return Dat_; }
   inline K_mf(void*)        GetDat        ()                        { return Dat_; }
   inline K_mf(TSize)        GetDatLen     (THiLoTransRef r) const   { return HiLoTransT(DatLen_,r); }
   inline K_mf(TSize)        GetExpectSize (THiLoTransRef r) const   { return static_cast<TSize>(GetMinSize() + HiLoTransT(DatLen_,r)); }
};
/////////////////////////////////////////////////////////////////////////////
#ifdef K_EnumPacked_Bug
   typedef byte   TPkToOMSFunc;
   enum TPkToOMSFunc_t
#else
   enum TPkToOMSFunc
#endif
{
   omst_Data,
   omst_NewOrder,
   omst_ReqCmd,      // ParentOrder/ChildOrder操作指令(由Producer解釋內容).
   omst_SyTabUpdate,
   omst_ModifyOrder, //設定委託基本欄位.(IsHiding,Dealer...)
   omst_AuditLogReg, //[(Reg/Unreg)+ParentID+ChildID]
   omst_DoPendingReq,//執行 Pending ReqCmd.
} K_EnumPacked;
//---------------------------------------------------------------------------
struct TPkToOMSHead : public THead
{
   TSize    DatLen_;
};

/** Package to OMS.
*/
class TPkToOMS : public TPkToOMSHead
{
   typedef TPkToOMS  this_type;
   enum {   DatSize = sizeof(TPkRec)-sizeof(TPkToOMSHead)   };
   char     Dat_[DatSize];

public:
   /** for OMS Client: TPkAry<>
   */
   inline K_ctor TPkToOMS ()
            {  SetMsgCode(TMsgCode(mcOMS+omst_Data));
            }
   inline K_mf(void)  SetDatLen  (TSize sz, THiLoTransRef r)
            {  DatLen_ = sz;
               SetPkSize(GetExpectSize());
               DatLen_ = HiLoTransT(DatLen_,r);
            }
   inline static K_mf(TSize) GetMaxDatSize ()   { return DatSize-1; }

   //----- for server -----
   static inline K_mf(TSize) GetMinSize    ()         { return sizeof(this_type) - DatSize; }
   inline K_mf(const void*)  GetDat        () const   { return Dat_;    }
   inline K_mf(void*)        GetDat        ()         { return Dat_;    }
   inline K_mf(TSize)        GetDatLen     () const   { return DatLen_; }
   inline K_mf(TSize)        GetExpectSize () const   { return static_cast<TSize>(GetMinSize() + DatLen_); }
};
//---------------------------------------------------------------------------
#ifdef HANDLE_PRAGMA_PACK_PUSH_POP
#pragma pack(pop)
#else
#pragma pack()
#endif
/////////////////////////////////////////////////////////////////////////////
/** 當有大量資料時,一個 TPkXXOMS 可能不夠放,此時請用:
*/
template <class TPk, unsigned PkCount = 10>
class TPkAry : public Kstl::persistent
{
   typedef Kstl::persistent   inherited;
   TPk            Pks_[PkCount];
   int            PkIdx_;
   mutable bool   FirstGetPk_;
   TMsgCode       MsgCode_;
   TSesPk*        Ses_;
   K_mf(void*) ReallocWrBuf (unsigned& sz)
      {  if(PkIdx_ >= numofele(Pks_)-1)
            return NULL;
         TPk*  pk = Pks_ + PkIdx_;
         pk->SetDatLen(TSize(get_saved_size()), GetHiLoTrans());
         if(Ses_) {
            Ses_->SendPk(*pk);
         } else {
            ++PkIdx_;
            ++pk;
         }
         void* newBuf = pk->GetDat();
         reset(newBuf, sz = TPk::GetMaxDatSize());
         return newBuf;
      }
protected:
   inline K_mf(bool) IsFirstGetPk () const   { return FirstGetPk_; }

public:
   template <class TPkFunc>
   inline K_ctor TPkAry (TPkFunc func, TSesPk* ses = NULL, THiLoTransRef hiLoTrans = THiLoTransRef(THiLoTransRef::DwordLocalMachine_))
      :  inherited(Pks_->GetDat(), TPk::GetMaxDatSize(), hiLoTrans)
      ,  PkIdx_(0)
      ,  FirstGetPk_(true)
      ,  MsgCode_(TMsgCode(mcOMS+func))
      ,  Ses_(ses)
      {
      }
   K_dtor ~TPkAry ()
      {  if(Ses_  &&  FirstGetPk_  &&  get_saved_size()>0) {
            int   L = 0;
            if(const TPk*  pk = GetPk(L))
               Ses_->SendPk(*pk);
         }
      }
   K_mf(const TPk*) GetPk (int& L) const
      {  if(L > PkIdx_)
            return NULL;
         if(FirstGetPk_) {
            FirstGetPk_ = false;
            TPk*  pk = const_cast<TPk*>(Pks_ + PkIdx_);
            pk->SetMsgCode(MsgCode_);
            pk->SetDatLen(TSize(get_saved_size()), GetHiLoTrans());
         }
         return Pks_ + L++;
      }
   inline K_mf(int) LeftBufCount () const
      {  return numofele(Pks_) - PkIdx_;
      }
};
/////////////////////////////////////////////////////////////////////////////
} // namespace Pk
} // namespace Kway
//---------------------------------------------------------------------------
#endif

